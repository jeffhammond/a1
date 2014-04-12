
#include "a1-pami.h"
#include "common.h"
#include "spi/include/kernel/memory.h"

typedef struct {  
  size_t                 id;
  pami_dispatch_hint_t   options;
  char                 * name;
  pami_result_t          result;
} dispatch_info_t;

dispatch_info_t dispatch[2];
pami_client_t         a1_client;
pami_context_t      * a1_contexts;

volatile int a1_collective_flag = 0;

void a1_collective_done (void *ctxt, void * clientdata, pami_result_t err)
{
  int * active = (int *) clientdata;
  (*active)--;
}

pami_geometry_t       a1_wgeometry;
pami_xfer_t           a1_barrier;
pami_xfer_t           a1_allg;

size_t                num_contexts = 1;
unsigned              world_rank;
size_t                world_size;

Kernel_MemoryRegion_t   a1spi_mr;
pami_memregion_t      * a1pami_mr;
void                 ** va_base_table;

void _internal_A1_Barrier ();
void _internal_A1_Allg (void *sbuf, int scount, void *rbuf, int rcount);

int A1_Initialize(int threadlevel) {
  
  pami_result_t rc = PAMI_SUCCESS;  
  
  /* initialize the client */
  char * clientname = "A1-PAMI";
  rc = PAMI_Client_create(clientname, &a1_client, NULL, 0);
  TEST_ASSERT(rc == PAMI_SUCCESS,"PAMI_Client_create");
  
  /* query properties of the client */
  pami_configuration_t config;  
  
  config.name = PAMI_CLIENT_NUM_TASKS;
  rc = PAMI_Client_query( a1_client, &config,1);
  TEST_ASSERT(rc == PAMI_SUCCESS,"PAMI_Client_query");
  world_size = config.value.intval;
  TEST_ASSERT( world_size > 1 , "world_size > 1" );
    
  config.name = PAMI_CLIENT_TASK_ID;
  rc = PAMI_Client_query( a1_client, &config,1);
  TEST_ASSERT(rc == PAMI_SUCCESS,"PAMI_Client_query");
  world_rank = config.value.intval;
  
  config.name = PAMI_CLIENT_NUM_CONTEXTS;
  rc = PAMI_Client_query( a1_client, &config, 1);
  TEST_ASSERT(rc == PAMI_SUCCESS,"PAMI_Client_query");
  num_contexts = config.value.intval;
  
  assert (num_contexts > 0);
  num_contexts = 1;
  
  /* initialize the contexts */
  a1_contexts = (pami_context_t *) malloc(num_contexts*sizeof(pami_context_t));
  TEST_ASSERT(a1_contexts!=NULL ,"malloc( num_contexts * sizeof(pami_context_t))");
  
  rc = PAMI_Context_createv( a1_client, NULL, 0, a1_contexts, num_contexts );
  TEST_ASSERT( rc == PAMI_SUCCESS , "PAMI_Context_createv" ); 

    dispatch[0].id = 10;
  dispatch[0].options = (pami_dispatch_hint_t) {0};
  dispatch[0].options.recv_copy = PAMI_HINT_DISABLE;
  dispatch[0].options.recv_contiguous = PAMI_HINT_ENABLE;
  dispatch[0].name = (char*)" Contig Dispatch ";

  dispatch[1].id = 11;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].options.recv_copy = PAMI_HINT_DISABLE;
  dispatch[1].options.recv_contiguous = PAMI_HINT_ENABLE;
  dispatch[1].name = (char*)" Strided Dispatch ";

  pami_dispatch_callback_function fn, fn_nt;
  fn.p2p    = dispatch_contig;
  fn_nt.p2p = dispatch_strided;

  //Set the dispatch ids
  dispatch[0].result = PAMI_Dispatch_set (a1_contexts[0],
					  dispatch[0].id,
					  fn,
					  (void *) & _recv_active,
					  dispatch[0].options);  
  dispatch[1].result = PAMI_Dispatch_set (a1_contexts[0],
					  dispatch[1].id,
					  fn_nt,
					  (void *) & _recv_active,
					  dispatch[1].options);

  //Initialize memory regions 
  va_base_table = (void **) malloc(sizeof(void*) * world_size);
  uint32_t retval;
  retval = Kernel_CreateMemoryRegion (&a1spi_mr, va_base_table, sizeof(void*));
  TEST_ASSERT(retval==0,"Kernel_CreateMemoryRegion");
  assert (a1spi_mr.BaseVa != NULL);
  
  pami_memregion_t pami_mr;
  size_t bytes_out;
  PAMI_Memregion_create(a1_contexts[0], 
			a1spi_mr.BaseVa, 
			a1spi_mr.Bytes,
			&bytes_out,
			&pami_mr);

  a1pami_mr = (pami_memregion_t *) malloc(sizeof(pami_memregion_t) * world_size);    

#ifdef ENABLE_MPI
  MPI_Barrier (MPI_COMM_WORLD);
  
  //Populate remote vabase table
  MPI_Allgather (&a1spi_mr.BaseVa, sizeof(void*), MPI_BYTE, 
		 va_base_table, 
		 sizeof(void*), MPI_BYTE, MPI_COMM_WORLD);  
  
  //Populate the memory region tables
  MPI_Allgather (&pami_mr, sizeof(pami_memregion_t), MPI_BYTE, a1pami_mr, 
		 sizeof(pami_memregion_t), MPI_BYTE, MPI_COMM_WORLD);
#else
  //Initialize barrier and collectives
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md = NULL;
  pami_algorithm_t    *must_query_algo = NULL;
  pami_metadata_t     *must_query_md = NULL;
  pami_xfer_type_t     xfer_type = PAMI_XFER_BARRIER;
  
  /* Docs01:  Get the World Geometry */
  pami_result_t result = PAMI_Geometry_world (a1_client, &a1_wgeometry);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to get world geometry: result = %d\n", result);
    return -1;
  }
  
  result = PAMI_Geometry_algorithms_num(a1_wgeometry,
					xfer_type,
					(size_t*)num_algorithm);
  
  if (result != PAMI_SUCCESS || num_algorithm[0]==0)
  {
    fprintf (stderr,
	     "Error. Unable to query algorithm, or no algorithms available result = %d\n",
	     result);
    return -1;
  }
  
  always_works_algo = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[0]);
  always_works_md   = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[0]);
  must_query_algo   = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[1]);
  must_query_md     = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[1]);

  /* Docs05:  Query the algorithm lists */
  result = PAMI_Geometry_algorithms_query(a1_wgeometry,
					  xfer_type,
					  always_works_algo,
					  always_works_md,
					  num_algorithm[0],
					  must_query_algo,
					  must_query_md,
					  num_algorithm[1]);
  a1_barrier.cb_done   = a1_collective_done;
  a1_barrier.cookie    = (void*) & a1_collective_flag;
  a1_barrier.algorithm = always_works_algo[0];  

  free (always_works_algo);
  free (always_works_md);
  free (must_query_algo);
  free (must_query_md);

  _internal_A1_Barrier();

  xfer_type = PAMI_XFER_ALLGATHER;
  result = PAMI_Geometry_algorithms_num(a1_wgeometry,
					xfer_type,
					(size_t*)num_algorithm);
  
  if (result != PAMI_SUCCESS || num_algorithm[0]==0)
  {
    fprintf (stderr,
	     "Error. Unable to query algorithm, or no algorithms available result = %d\n",
	     result);
    return -1;
  }
  
  always_works_algo = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[0]);
  always_works_md   = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[0]);
  must_query_algo   = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[1]);
  must_query_md     = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[1]);

  /* Docs05:  Query the algorithm lists */
  result = PAMI_Geometry_algorithms_query(a1_wgeometry,
					  xfer_type,
					  always_works_algo,
					  always_works_md,
					  num_algorithm[0],
					  must_query_algo,
					  must_query_md,
					  num_algorithm[1]);
  a1_allg.cb_done   = a1_collective_done;
  a1_allg.cookie    = (void*) & a1_collective_flag;
  a1_allg.algorithm = always_works_algo[0];  

  free (always_works_algo);
  free (always_works_md);
  free (must_query_algo);
  free (must_query_md);

  _internal_A1_Allg(&a1spi_mr.BaseVa, sizeof(void*), 
		    va_base_table, sizeof(void*));  

  _internal_A1_Allg (&pami_mr,  sizeof(pami_memregion_t), 
		     a1pami_mr, sizeof(pami_memregion_t));
#endif

#if ENABLE_PROGRESS
  A1PAMI_Progress_init();
#endif
}

int A1_Finalize () {

#if ENABLE_PROGRESS
  A1PAMI_Progress_finalize();
#endif

#ifdef ENABLE_MPI
  MPI_Barrier (MPI_COMM_WORLD);
#else
  _internal_A1_Barrier();
#endif

  pami_result_t result;
  result = PAMI_Context_destroyv(a1_contexts, 1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(a1_client);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }
  
  return 0;
}

int A1_AllFence () {
#if ENABLE_PROGRESS
#else
  _internal_A1_Barrier();
#endif
}


int _internal_A1_getSize() {
  return world_size;
}

int _internal_A1_getRank() {
  return world_rank;
}

void _internal_A1_Barrier () {
  pami_result_t result;
  a1_collective_flag = 1;
  result = PAMI_Collective(a1_contexts[0], &a1_barrier); 
  while (a1_collective_flag)
    result = PAMI_Context_advance (a1_contexts[0], 100);  
}

void _internal_A1_Allg (void *sbuf, int scount, void *rbuf, int rcount) {
  pami_result_t result;
  a1_collective_flag = 1;
  a1_allg.cmd.xfer_allgather.sndbuf     = sbuf;
  a1_allg.cmd.xfer_allgather.stype      = PAMI_TYPE_BYTE;
  a1_allg.cmd.xfer_allgather.stypecount = scount;
  a1_allg.cmd.xfer_allgather.rcvbuf     = rbuf;
  a1_allg.cmd.xfer_allgather.rtype      = PAMI_TYPE_BYTE;
  a1_allg.cmd.xfer_allgather.rtypecount = rcount;

  result = PAMI_Collective(a1_contexts[0], &a1_allg); 
  while (a1_collective_flag)
    result = PAMI_Context_advance (a1_contexts[0], 100);  
}

int A1_PutAcc(int             target,
	      void          * srcbuf,
	      void          * dstbuf,
	      int             bytes,
	      A1_datatype_t   dt,
	      void          * scale)
{
  A1PAMI_Assert (dt == A1_DOUBLE);

  A1PAMI_Info_t ainfo;
  ainfo.scale    = *(double *)scale;
  ainfo.dstaddr  = dstbuf;

  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (a1_client, target, 0, &target_e) );
  send_contig ( a1_contexts[0],
	        dispatch[0].id,
	        &ainfo,
		sizeof(A1PAMI_Info_t),
		srcbuf,
		bytes, 
		target_e );		  
}

int A1_NbPutAcc(int             target,
		void          * source_ptr,
		void          * target_ptr,
		int             bytes,
		A1_datatype_t   a1_type,
		void          * scaling,
		A1_handle_t     handle) 
{
  return PAMI_ERROR;
}


int A1_PutAccS(int             target,
	       int             stride_levels,
	       int           * block_sizes,
	       void          * srcbuf,
	       int           * src_stride_ar,
	       void          * dstbuf,
	       int           * trg_stride_ar,
	       A1_datatype_t   dt,
	       void          * scale)
{
  TEST_ASSERT (stride_levels == 1, "A1_PutAccS");

  A1PAMI_Info_t ainfo;
  ainfo.scale    = *(double *)scale;
  ainfo.dstaddr  = dstbuf;
  ainfo.count[0] = block_sizes[0];
  ainfo.count[1] = block_sizes[1];
  ainfo.sstride  = src_stride_ar[0];
  ainfo.dstride  = trg_stride_ar[0];

  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (a1_client, target, 0, &target_e) );
  send_strided ( a1_contexts[0],
		 dispatch[0].id,   //contig dispatch
		 dispatch[1].id,   //strided dispatch
		 &ainfo,
		 sizeof(A1PAMI_Info_t),
		 srcbuf,
		 target_e );		  
}

int A1_NbPutAccS(int target,
		 int stride_levels,
		 int *block_sizes,
		 void* source_ptr,
		 int *src_stride_ar,
		 void* target_ptr,
		 int *trg_stride_ar,
		 A1_datatype_t a1_type,
		 void* scaling,
		 A1_handle_t handle)
{
  return PAMI_ERROR;
}

int A1_Flush (int target)
{
  return 0;
}

int A1_Put  (int      target, 
	     void   * src_ptr, 
	     void   * target_ptr, 
	     int bytes)
{
  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (a1_client, target, 0, &target_e) );
  
  put_contig (a1_contexts[0],
	      src_ptr,
	      target_ptr,
	      a1spi_mr.BaseVa,
	      va_base_table[target],
	      a1pami_mr[world_rank],
	      a1pami_mr[target],
	      bytes,
	      target_e
	      );
}

int A1_Get(int       target, 
	   void    * src_ptr, 
	   void    * target_ptr, 
	   int       bytes)
{
  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (a1_client, target, 0, &target_e) );
  
  get_contig (a1_contexts[0],
	      src_ptr,
	      target_ptr,
	      va_base_table[target],
	      a1spi_mr.BaseVa,
	      a1pami_mr[target],
	      a1pami_mr[world_rank],
	      bytes,
	      target_e
	      );  
}

int A1_AllocSegment (void ** buf, int size)
{
  *buf = malloc (size);
  return 0;
}

int A1_FreeSegment (void * buf)
{
  free (buf);
}

volatile unsigned rma_done = 1;


int A1_Rmw (int             target,
	    void          * src_ptr_in,
	    void          * src_ptr_out,
	    void          * target_ptr,
	    int             bytes,
	    A1_atomic_op_t  op,
	    A1_datatype_t   a1_type)
{
  A1PAMI_Assert (a1_type == A1_INT32);
  pami_type_t   type;
  pami_atomic_t operation;
  
  if (a1_type == A1_INT32)
    type   = PAMI_TYPE_SIGNED_INT;
  else if (a1_type == A1_INT64)
    type   = PAMI_TYPE_SIGNED_LONG;
  else {
    fprintf(stderr, "Unsupported type\n");
    A1PAMI_Assert(0);
  }

  if (op == A1_FETCH_AND_ADD)
    operation = PAMI_ATOMIC_FETCH_ADD; 
  else if (op == A1_SWAP)
    operation = PAMI_ATOMIC_FETCH_SET;
  else {
    fprintf(stderr, "Unsupported atomic op\n");
    A1PAMI_Assert (0);
  }    
  
  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (a1_client, target, 0, &target_e) );
  
  _rmw (a1_contexts[0], src_ptr_in, src_ptr_out, 
	target_ptr, operation, type, target_e);
}

typedef void (*pamix_progress_function) (pami_context_t context, void *cookie);

typedef pami_result_t (*pamix_progress_register_fn) 
  (pami_context_t            context,
   pamix_progress_function   progress_fn,
   pamix_progress_function   suspend_fn,
   pamix_progress_function   resume_fn,
   void                     * cookie);

typedef pami_result_t (*pamix_progress_enable_fn)(pami_context_t   context,
						  int              event_type);

typedef pami_result_t (*pamix_progress_disable_fn)(pami_context_t  context,
						   int             event_type);

#define PAMI_EXTENSION_OPEN(client, name, ext)  \
({                                              \
  pami_result_t rc;                             \
  rc = PAMI_Extension_open(client, name, ext);  \
  A1PAMI_Assert (rc == PAMI_SUCCESS);      \
})
#define PAMI_EXTENSION_FUNCTION(type, name, ext)        \
({                                                      \
  void* fn;                                             \
  fn = PAMI_Extension_symbol(ext, name);                \
  A1PAMI_Assert (fn != NULL);				\
  (type)fn;                                             \
})


pami_extension_t            a1pami_ext_progress;
pamix_progress_register_fn  a1pami_progress_register;
pamix_progress_enable_fn    a1pami_progress_enable;
pamix_progress_disable_fn   a1pami_progress_disable;

int A1PAMI_Progress_init() {
  PAMI_EXTENSION_OPEN(a1_client,"EXT_async_progress",&a1pami_ext_progress);
  a1pami_progress_register = PAMI_EXTENSION_FUNCTION(pamix_progress_register_fn, "register", a1pami_ext_progress);
  a1pami_progress_enable   = PAMI_EXTENSION_FUNCTION(pamix_progress_enable_fn,   "enable",   a1pami_ext_progress);
  a1pami_progress_disable  = PAMI_EXTENSION_FUNCTION(pamix_progress_disable_fn,  "disable",  a1pami_ext_progress);
  
  a1pami_progress_register (a1_contexts[0], NULL, NULL, NULL, NULL);
  a1pami_progress_enable   (a1_contexts[0], 0 /*progress all*/);  
  
  return 0;
}

int A1PAMI_Progress_finalize() {
  a1pami_progress_disable  (a1_contexts[0], 0 /*progress all*/);    
  PAMI_Extension_close (a1pami_ext_progress);
}

