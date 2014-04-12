
#include "armcix.h"
#include <hwi/include/bqc/A2_inlines.h>
#include <pami.h>

typedef struct {
  volatile unsigned  * counter;
  char               * buffer;
} _PackState;


#define RC(statement) \
{ \
    int rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        printf(#statement " rc = %d, line %d\n", rc, __LINE__); \
        exit(-1); \
    } \
}

#define POLL_CNT   100

int pami_init(pami_client_t        * client,        /* in/out:  client      */
              pami_context_t       * context,       /* in/out:  context     */
              char                 * clientname,    /* in/out:  clientname  */
              size_t               * num_contexts,  /* in/out:  num_contexts*/
              pami_configuration_t * configuration, /* in:      config      */
              size_t                 num_config,    /* in:      num configs */
              pami_task_t          * task_id,       /* out:     task id     */
              size_t               * num_tasks);    /* out:     num tasks   */

int pami_shutdown(pami_client_t      * client,     /* in/out:  client      */
                  pami_context_t     * context,    /* in/out:  context     */
		  size_t               * num_contexts);    /* in/out:  num_contexts*/


void       send_contig ( pami_context_t    context,
			 size_t            dispatch, 
			 void            * metadata,
			 size_t            hdrsize,
			 char            * buffer,
			 size_t            sndlen, 
			 pami_endpoint_t   target );

void packStrided   ( ARMCIX_Info_t     * ainfo,
		     void              * dstbuf,
		     void              * srcbuf );		  

void send_strided  ( pami_context_t    context,
		     size_t            dispatchc,
		     size_t            dispatchs,
		     ARMCIX_Info_t   * ainfo,
		     size_t            hdrsize,
		     char            * buffer,
		     pami_endpoint_t   target );

void     data_fn_scale1 (void                  * target,
			 void                  * src,
			 size_t                  bytes,
			 void                  * cookie);

void     data_fn    (void                  * target,
		     void                  * src,
		     size_t                  bytes,
		     void                  * cookie);

void     qpx_data_fn_scale1 (void                  * target,
			     void                  * src,
			     size_t                  bytes,
			     void                  * cookie);

void     qpx_data_fn (void                  * target,
		      void                  * src,
		      size_t                  bytes,
		      void                  * cookie);

void     data_fn_strided    (void                  * target,
			     void                  * src,
			     size_t                  bytes,
			     void                  * cookie);

void      qpx_data_fn_strided_1 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie);

void      qpx_data_fn_strided_2 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie);

void      qpx_data_fn_strided_4 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie);

void      qpx_data_fn_strided_8 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie);

/* --------------------------------------------------------------- */
void dispatch_contig 
( pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv );       /**< OUT: receive message structure */


void dispatch_strided
( pami_context_t       context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t        * recv );       /**< OUT: receive message structure */


void cb_done   (pami_context_t   context,
		void            * cookie,
		pami_result_t    result);
