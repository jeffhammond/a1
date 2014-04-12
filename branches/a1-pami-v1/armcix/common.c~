
#include "common.h"

volatile unsigned _send_active = 1;
volatile unsigned _recv_active = 1;

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
  free(cookie);
}


static size_t      _iteration = 0;
ARMCIX_Info_t      ainfo_arr[2];

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
  ARMCIX_Assert (header_size == sizeof(ARMCIX_Info_t));
  ARMCIX_Info_t *ainfo = (ARMCIX_Info_t *)header_addr;
  //fprintf(stderr, "dispatch_contig() %g %p\n", 
  //  ainfo->scale, ainfo->dstaddr);
  
  if (recv)
  {
    if (ainfo->scale != 1.0) {
      ainfo             = (ARMCIX_Info_t *)malloc(header_size);
      *ainfo            = *(ARMCIX_Info_t *)header_addr;
      recv->data_fn     = qpx_data_fn; 
      recv->data_cookie = ainfo;
      recv->local_fn    = cb_recv_done;
      recv->cookie      = ainfo;
    }
    else {
      recv->data_fn     = qpx_data_fn_scale1; 
      recv->data_cookie = NULL;      
      recv->local_fn    = NULL;
      recv->cookie      = NULL;
    }      
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = ainfo->dstaddr;
    recv->offset   = 0;    
  }
  else {
    if (ainfo->scale != 1.0) 
      data_fn (ainfo->dstaddr, (void*)pipe_addr, pipe_size, ainfo);
    else
      data_fn_scale1(ainfo->dstaddr, (void*)pipe_addr, pipe_size, NULL);
    volatile unsigned * value = (volatile unsigned *) cookie;
    --*value;
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
  ARMCIX_Assert (header_size == sizeof(ARMCIX_Info_t));
  ARMCIX_Info_t *ainfo = (ARMCIX_Info_t *)header_addr;

  //fprintf(stderr, "dispatch_strided() %g %p\n", 
  //  ainfo->scale, ainfo->dstaddr);    
  if (recv)
  {
    ainfo = (ARMCIX_Info_t *)malloc(header_size);
    *ainfo = *(ARMCIX_Info_t *)header_addr;
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
  //  dispatch, hdrsize, sndlen);

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

  RC( PAMI_Send (context, &parameters) ); 
  while (_send_active) PAMI_Context_advance (context, POLL_CNT);
  _send_active = 1;
}

void send_strided  ( pami_context_t    context,
		     size_t            dispatchc,
		     size_t            dispatchs,
		     ARMCIX_Info_t   * ainfo,
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

int pami_init(pami_client_t        * client,        
              pami_context_t       * context,       
              char                 * clientname,    
              size_t               * num_contexts,  
              pami_configuration_t * configuration, 
              size_t                 num_config,    
              pami_task_t          * task_id,       
              size_t               * num_tasks)     
{
  pami_result_t        result        = PAMI_ERROR;
  char                 cl_string[]   = "TEST";
  pami_configuration_t l_configuration;
  size_t               max_contexts;

  if(clientname == NULL)
    clientname = cl_string;

  /* Docs01:  Create the client */
  result = PAMI_Client_create (clientname, client, NULL, 0);
  if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to initialize pami client %s: result = %d\n",
                 clientname,result);
        return 1;
      }

  /* Docs03:  Create the client */
  l_configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(*client, &l_configuration,1);
  if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to query configuration.name=(%d): result = %d\n",
                 l_configuration.name, result);
        return 1;
      }
  max_contexts = l_configuration.value.intval;
  *num_tasks = (*num_tasks<max_contexts)?*num_tasks:max_contexts;

  l_configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(*client, &l_configuration,1);
  if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to query configuration.name=(%d): result = %d\n",
                 l_configuration.name, result);
        return 1;
      }
  *task_id = l_configuration.value.intval;

  l_configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(*client, &l_configuration,1);
  if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to query configuration.name=(%d): result = %d\n",
                 l_configuration.name, result);
        return 1;
      }
  *num_tasks = l_configuration.value.intval;
  /* Docs04:  Create the client */

  /* Docs05:  Create the client */
  result = PAMI_Context_createv(*client, configuration, num_config, context, *num_contexts);
  if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to create pami context: result = %d\n",
                 result);
        return 1;
      }

  /* Docs06:  Create the client */
  return 0;
}

int pami_shutdown(pami_client_t        * client,         
                  pami_context_t       * context,        
                  size_t               * num_contexts)   
{
  pami_result_t result;
  /* Docs07:  Destroy the client and contexts */
  result = PAMI_Context_destroyv(context, *num_contexts);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
      return 1;
    }

  result = PAMI_Client_destroy(client);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
      return 1;
    }
  /* Docs08:  Destroy the client and contexts*/
  return 0;
}


