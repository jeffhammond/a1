
#include "common.h"

extern unsigned _recv_active;

typedef struct
{
  size_t                 id;
  pami_dispatch_hint_t   options;
  char                 * name;
  pami_result_t          result;
} dispatch_info_t;

dispatch_info_t dispatch[2];
pami_client_t         armcix_client;
pami_context_t        armcix_context;

volatile int pami_barrier_flag = 0;

void pami_barrier_done (void *ctxt, void * clientdata, pami_result_t err)
{
  int * active = (int *) clientdata;
  (*active)--;
}

pami_geometry_t       world_geometry;
pami_xfer_t           pami_barrier;
size_t                num_contexts = 1;
unsigned              my_task;
size_t                num_tasks;

void ARMCIX_Initialize (int *mt, int *nt) {  
  RC( pami_init (&armcix_client,        /* Client             */
                 &armcix_context,       /* Context            */
                 "ARMCIX",       /* Clientname=default */
                 &num_contexts,  /* num_contexts       */
                 NULL,           /* null configuration */
                 0,              /* no configuration   */
                 &my_task,      /* task id            */
                 &num_tasks) );  /* number of tasks    */

  *nt = (int) num_tasks;
  *mt = (int) my_task;
  pami_configuration_t configuration;
  
  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  RC( PAMI_Client_query(armcix_client, &configuration, 1) );
  size_t num_local_tasks = configuration.value.intval;

  int num_dispatch = 2;
  configuration.name = PAMI_CLIENT_WTICK;
  RC( PAMI_Client_query(armcix_client, &configuration, 1) );
  double tick = configuration.value.doubleval;

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

  dispatch[0].result = PAMI_Dispatch_set (armcix_context,
					  dispatch[0].id,
					  fn,
					  (void *) & _recv_active,
					  dispatch[0].options);
  
  dispatch[1].result = PAMI_Dispatch_set (armcix_context,
					  dispatch[1].id,
					  fn_nt,
					  (void *) & _recv_active,
					  dispatch[1].options);

  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = num_tasks - 1;
  pami_endpoint_t origin, target;
  RC( PAMI_Endpoint_create (armcix_client, origin_task, 0, &origin) );
  RC( PAMI_Endpoint_create (armcix_client, target_task, 0, &target) );

  sleep(1); /* work-around for ticket #385 */

  //Initialize barrier
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md = NULL;
  pami_algorithm_t    *must_query_algo = NULL;
  pami_metadata_t     *must_query_md = NULL;
  pami_xfer_type_t     xfer_type = PAMI_XFER_BARRIER;
  
  /* Docs01:  Get the World Geometry */
  pami_result_t result = PAMI_Geometry_world (armcix_client, &world_geometry);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to get world geometry: result = %d\n", result);
    return;
  }
  
  result = PAMI_Geometry_algorithms_num(world_geometry,
					xfer_type,
					(size_t*)num_algorithm);
  
  if (result != PAMI_SUCCESS || num_algorithm[0]==0)
  {
    fprintf (stderr,
	     "Error. Unable to query algorithm, or no algorithms available result = %d\n",
	     result);
    return;
  }
  
  always_works_algo = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[0]);
  always_works_md   = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[0]);
  must_query_algo   = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[1]);
  must_query_md     = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[1]);

  /* Docs05:  Query the algorithm lists */
  result = PAMI_Geometry_algorithms_query(world_geometry,
					  xfer_type,
					  always_works_algo,
					  always_works_md,
					  num_algorithm[0],
					  must_query_algo,
					  must_query_md,
					  num_algorithm[1]);
  pami_barrier.cb_done   = pami_barrier_done;
  pami_barrier.cookie    = (void*) & pami_barrier_flag;
  pami_barrier.algorithm = always_works_algo[0];  
}

void ARMCIX_Finalize () {
  RC( pami_shutdown(&armcix_client, &armcix_context, &num_contexts) );
}


void ARMCIX_Acc (int               dt,
		 void            * scale,
		 void            * buffer,
		 void            * dstbuf,
		 int               bytes,
		 int               target)
{
  ARMCIX_Info_t ainfo;
  ainfo.scale    = *(double *)scale;
  ainfo.dstaddr  = dstbuf;

  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (armcix_client, target, 0, &target_e) );
  send_contig ( armcix_context,
	        dispatch[0].id,
	        &ainfo,
		sizeof(ARMCIX_Info_t),
		buffer,
		bytes, 
		target_e );		  
}

void ARMCIX_AccS (int               dt,
		  void            * scale,
		  void            * buffer,
		  int               src_stride_arr[],
		  void            * dstbuf,
		  int               dst_stride_arr[],
		  int               count[],
		  int               levels,
		  int               target)
{
  ARMCIX_Info_t ainfo;
  ARMCIX_Assert (levels == 1);
  ainfo.scale    = *(double *)scale;
  ainfo.dstaddr  = dstbuf;
  ainfo.count[0] = count[0];
  ainfo.count[1] = count[1];
  ainfo.sstride  = src_stride_arr[0];
  ainfo.dstride  = dst_stride_arr[0];

  pami_endpoint_t target_e;
  RC( PAMI_Endpoint_create (armcix_client, target, 0, &target_e) );
  send_strided ( armcix_context,
		 dispatch[0].id,   //contig dispatch
		 dispatch[1].id,   //strided dispatch
		 &ainfo,
		 sizeof(ARMCIX_Info_t),
		 buffer,
		 target_e );		  
}

void ARMCIX_Barrier () {
  pami_result_t result;
  pami_barrier_flag = 1;
  result = PAMI_Collective(armcix_context, &pami_barrier); 
  while (pami_barrier_flag)
    result = PAMI_Context_advance (armcix_context, 100);  
}

unsigned long long ARMCIX_Wtimebase() {
  return PAMI_Wtimebase(armcix_client);
}

double ARMCIX_Wtime() {
  return PAMI_Wtime(armcix_client);
}

void ARMCIX_Fence (int target) {}
