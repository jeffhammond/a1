
#include "common.h"

volatile unsigned _send_active = 1;
//volatile unsigned _recv_active = 1;

void cb_done   (pami_context_t   context,
		void            * cookie,
		pami_result_t    result)
{
  volatile unsigned * value = (volatile unsigned *) cookie;
  //fprintf(stderr, "cb_done() cookie = %p, %d => %d\n", cookie, *value, *value - 1);
  --*value;
}

void cb_pack_done   (pami_context_t   context,
		     void            * cookie,
		     pami_result_t    result)
{
  _PackState *pstate = (_PackState *)cookie;
  -- *(pstate->counter);
  free(pstate->buffer);
}

void cb_recv_done   (pami_context_t   context,
		     void            * cookie,
		     pami_result_t    result)
{
  if (cookie != NULL)
    free(cookie);
}


/* --------------------------------------------------------------- */
void dispatch_contig_int
( pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv )       /**< OUT: receive message structure */
{  
  A1PAMI_Assert (header_size == sizeof(A1PAMI_Info_t));
  A1PAMI_Info_t *ainfo = (A1PAMI_Info_t *)header_addr;

  //fprintf(stderr, "dispatch_contig_int scale %d, dstbuf %lx\n", 
  //  ainfo->scale_i, ainfo->dstaddr);

  if (recv)
  {
    if (ainfo->scale_i != 1) {
      ainfo             = (A1PAMI_Info_t *)malloc(header_size);
      *ainfo            = *(A1PAMI_Info_t *)header_addr;
      recv->data_fn     = idata_fn; 
      recv->data_cookie = ainfo;
      recv->local_fn    = cb_recv_done;
      recv->cookie      = ainfo;
    }
    else {
      recv->data_fn     = idata_fn_scale1; 
      recv->data_cookie = NULL;      
      recv->local_fn    = NULL;
      recv->cookie      = NULL;
    }      
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = ainfo->dstaddr;
    recv->offset   = 0;    
  }
  else 
    idata_fn (ainfo->dstaddr, (void*)pipe_addr, pipe_size, ainfo);
}

/* --------------------------------------------------------------- */
void dispatch_contig_long
( pami_context_t       context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t        * recv )       /**< OUT: receive message structure */
{
  A1PAMI_Assert (header_size == sizeof(A1PAMI_Info_t));
  A1PAMI_Info_t *ainfo = (A1PAMI_Info_t *)header_addr;
  //fprintf(stderr, "dispatch_contig() %g %p\n", 
  //	  ainfo->scale, ainfo->dstaddr);
  
  if (recv)
  {
    if (ainfo->scale_l != 1) {
      ainfo             = (A1PAMI_Info_t *)malloc(header_size);
      *ainfo            = *(A1PAMI_Info_t *)header_addr;
      recv->data_fn     = ldata_fn; 
      recv->data_cookie = ainfo;
      recv->local_fn    = cb_recv_done;
      recv->cookie      = ainfo;
    }
    else {
      recv->data_fn     = ldata_fn_scale1; 
      recv->data_cookie = NULL;      
      recv->local_fn    = NULL;
      recv->cookie      = NULL;
    }      
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = ainfo->dstaddr;
    recv->offset   = 0;    
  }
  else 
    ldata_fn (ainfo->dstaddr, (void*)pipe_addr, pipe_size, ainfo);
}

/* --------------------------------------------------------------- */
void dispatch_contig 
( pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv )       /**< OUT: receive message structure */
{
  A1PAMI_Assert (header_size == sizeof(A1PAMI_Info_t));
  A1PAMI_Info_t *ainfo = (A1PAMI_Info_t *)header_addr;
  //fprintf(stderr, "dispatch_contig() %g %p\n", 
  //	  ainfo->scale, ainfo->dstaddr);
  
  if (recv)
  {
    int aligned = ( ( ((uint64_t)ainfo->dstaddr) & 0x1FUL ) == 0 );
    if (ainfo->scale != 1.0) {
      ainfo             = (A1PAMI_Info_t *)malloc(header_size);
      *ainfo            = *(A1PAMI_Info_t *)header_addr;
      recv->data_fn     = (aligned)?(qpx_data_fn):(data_fn); 
      recv->data_cookie = ainfo;
      recv->local_fn    = cb_recv_done;
      recv->cookie      = ainfo;
    }
    else {
      recv->data_fn     = (aligned) ? (qpx_data_fn_scale1):(data_fn_scale1); 
      recv->data_cookie = NULL;      
      recv->local_fn    = NULL;
      recv->cookie      = NULL;
    }      
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = ainfo->dstaddr;
    recv->offset   = 0;    
  }
  else {
    data_fn (ainfo->dstaddr, (void*)pipe_addr, pipe_size, ainfo);
    //volatile unsigned * value = (volatile unsigned *) cookie;
    //--*value;
  } 
}



/* --------------------------------------------------------------- */
void dispatch_strided 
( pami_context_t       context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t        * recv )       /**< OUT: receive message structure */
{
  A1PAMI_Assert (header_size == sizeof(A1PAMI_Info_t));
  A1PAMI_Info_t *ainfo = (A1PAMI_Info_t *)header_addr;

  //fprintf(stderr, "dispatch_strided() %g %p\n", 
  //  ainfo->scale, ainfo->dstaddr);    
  if (recv)
  {
    ainfo = (A1PAMI_Info_t *)malloc(header_size);
    *ainfo = *(A1PAMI_Info_t *)header_addr;
    recv->local_fn = cb_recv_done;
    recv->cookie   = ainfo;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = ainfo->dstaddr;
    recv->offset   = 0;
    if (ainfo->count[0] == 32)      
      recv->data_fn  = qpx_data_fn_strided_1;
    else if (ainfo->count[0] == 64)      
      recv->data_fn  = qpx_data_fn_strided_2;
    else if (ainfo->count[0] == 128)      
      recv->data_fn  = qpx_data_fn_strided_4;
    else if (ainfo->count[0] == 256)      
      recv->data_fn  = qpx_data_fn_strided_8;
    else
      recv->data_fn  = data_fn_strided; 
    recv->data_cookie = ainfo;
  }
  else {
    data_fn_strided (ainfo->dstaddr, (void*)pipe_addr, pipe_size, ainfo);
    volatile unsigned * value = (volatile unsigned *) cookie;
    --*value;
  } 
}

void       send_contig ( pami_context_t    context,
			 size_t            dispatch,
			 void            * metadata,
			 size_t            hdrsize,
			 char            * buffer,
			 size_t            sndlen, 
			 pami_endpoint_t   target )
{    
  //fprintf(stderr, "Calling send dispatch %ld, hdrsize %ld, bytes %ld\n",
  //dispatch, hdrsize, sndlen);

  pami_send_t parameters;
  parameters.send.dispatch        = dispatch;
  parameters.send.header.iov_base = metadata;
  parameters.send.header.iov_len  = hdrsize;
  parameters.send.data.iov_base   = buffer;
  parameters.send.data.iov_len    = sndlen;
  parameters.events.cookie        = (void *) & _send_active;
  parameters.events.local_fn      = NULL; //cb_done;
  parameters.events.remote_fn     = cb_done;
  parameters.send.dest = target;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));
  
#if ENABLE_PROGRESS
  pami_work_t  work;
  PAMI_Context_post (context, &work, (pami_work_function)PAMI_Send, 
		     (void*)&parameters);  
  while (_send_active == 1);
#else
  RC( PAMI_Send (context, &parameters) ); 
  while (_send_active) PAMI_Context_advance (context, POLL_CNT);
#endif
  _send_active = 1;
}

void send_strided  ( pami_context_t    context,
		     size_t            dispatchc,
		     size_t            dispatchs,
		     A1PAMI_Info_t   * ainfo,
		     size_t            hdrsize,
		     char            * buffer,
		     pami_endpoint_t   target )
{  
  //fprintf(stderr, "send_strided dispatch %ld, hdrsize %ld\n",
  //  dispatch, hdrsize);
  int i = 0;
  _PackState  pstate;
  pami_send_t parameters;
  parameters.send.header.iov_base = ainfo;
  parameters.send.header.iov_len  = hdrsize;
  parameters.send.data.iov_base   = NULL;
  parameters.events.cookie        = (void *) & _send_active;
  parameters.events.local_fn      = NULL;
  parameters.events.remote_fn     = NULL;
  parameters.send.dest            = target;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

  if (ainfo->count[0] >= 512) {
    parameters.send.dispatch        = dispatchc;      
    for (i = 0; i < ainfo->count[1]; ++i) {
      parameters.send.data.iov_base   = buffer + ainfo->sstride*i;
      parameters.send.data.iov_len    = ainfo->count[0];
      if (i == ainfo->count[1] - 1)
	parameters.events.remote_fn     = cb_done;
      RC( PAMI_Send (context, &parameters) );
    }
    while (_send_active) PAMI_Context_advance (context, POLL_CNT);
    _send_active = 1;
  }
  else {
    parameters.send.dispatch        = dispatchs;
    int bytes = ainfo->count[0] * ainfo->count[1];
    void *packbuf = malloc (bytes);
    packStrided (ainfo, packbuf, buffer);  
    pstate.buffer = packbuf;
    pstate.counter = & _send_active;
    
    parameters.send.data.iov_base   = packbuf;
    parameters.send.data.iov_len    = bytes;
    parameters.events.remote_fn     = cb_pack_done;
    parameters.events.cookie        = (void*)&pstate;
    
    //fprintf (stderr, "Calling pami_send\n");

    RC( PAMI_Send (context, &parameters) );
    while (_send_active) PAMI_Context_advance (context, POLL_CNT);
    _send_active = 1;  
    //fprintf (stderr, "After pami_send\n");
  }
}

void       put_contig ( pami_context_t        context,
			void                * srcbuf,
			void                * dstbuf,
			void                * src_base,
			void                * dst_base,
			pami_memregion_t    * src_mr,      
			pami_memregion_t    * dst_mr,      
			size_t                sndlen, 
			pami_endpoint_t       target )
{    
  //fprintf(stderr, "Calling send dispatch %ld, hdrsize %ld, bytes %ld\n",
  //  dispatch, hdrsize, sndlen);

  pami_rput_simple_t  rput;
  rput.rma.dest = target;
  rput.rma.hints.buffer_registered = PAMI_HINT_ENABLE;
  rput.rma.hints.use_rdma = PAMI_HINT_ENABLE;
  rput.rma.bytes = sndlen;
  rput.rma.cookie = (void*)&_send_active;
  rput.rma.done_fn = NULL; //cb_done;

  rput.rdma.local.mr      = src_mr;
  rput.rdma.local.offset  = (size_t)srcbuf - (size_t)src_base;
  rput.rdma.remote.mr     = dst_mr;
  rput.rdma.remote.offset = (size_t)dstbuf - (size_t)dst_base;    
  rput.put.rdone_fn = cb_done;

#if ENABLE_PROGRESS
  pami_work_t  work;
  PAMI_Context_post (context, &work, (pami_work_function)PAMI_Rput, 
		     (void*)&rput);  
  while (_send_active == 1);
#else
  RC( PAMI_Rput (context, &rput) ); 
  while (_send_active) PAMI_Context_advance (context, POLL_CNT);
#endif
  _send_active = 1;
}


void       get_contig ( pami_context_t        context,
			void                * lbuf,
			void                * rbuf,
			void                * lbase,
			void                * rbase,
			pami_memregion_t    * lmr,      
			pami_memregion_t    * rmr,      
			size_t                sndlen, 
			pami_endpoint_t       target )
{    
  pami_rget_simple_t  rget;
  rget.rma.dest = target;
  rget.rma.bytes = sndlen;
  rget.rma.cookie = (void*)&_send_active;
  rget.rma.done_fn = cb_done;
  rget.rma.hints.buffer_registered = PAMI_HINT_ENABLE;
  rget.rma.hints.use_rdma = PAMI_HINT_ENABLE;

  rget.rdma.local.mr      = lmr;
  rget.rdma.local.offset  = (size_t)lbuf - (size_t)lbase;
  rget.rdma.remote.mr     = rmr;
  rget.rdma.remote.offset = (size_t)rbuf - (size_t)rbase;    

  assert (_send_active == 1);
#if ENABLE_PROGRESS
  pami_work_t  work;
  PAMI_Context_post (context, &work, (pami_work_function)PAMI_Rget, 
		     (void*)&rget);  
  while (_send_active == 1);
#else
  RC( PAMI_Rget (context, &rget) ); 
  while (_send_active) PAMI_Context_advance (context, POLL_CNT);
#endif
  _send_active = 1;
}


void    _rmw ( pami_context_t      context,
	       void              * src_ptr_in,
	       void              * src_ptr_out,
	       void              * target_ptr,
	       pami_atomic_t       op,
	       pami_type_t         type,
	       pami_endpoint_t     target )
{
  pami_rmw_t  rmw;
  memset(&rmw, 0, sizeof(pami_rmw_t));  
  rmw.cookie  = (void *)&_send_active;
  rmw.done_fn = cb_done;
  
  int test = 0; 
  rmw.local     = src_ptr_out;
  rmw.remote    = target_ptr;
  rmw.value     = src_ptr_in;
  rmw.test      = &test;  
  rmw.type      = type;
  rmw.operation = op;   
  rmw.dest = target;

#if ENABLE_PROGRESS
  pami_work_t  work;
  PAMI_Context_post (context, &work, (pami_work_function)PAMI_Rmw, 
		     (void*)&rmw);  
  while (_send_active == 1);
#else
  RC( PAMI_Rmw (context, &rmw) );
  while (_send_active) PAMI_Context_advance (context, POLL_CNT);
#endif
  _send_active = 1;
}
