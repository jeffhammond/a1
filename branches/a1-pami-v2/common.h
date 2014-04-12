
#ifndef   __A1PAMI_COMMON_H__
#define   __A1PAMI_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "a1.h"
#include "a1-pami.h"
#include <hwi/include/bqc/A2_inlines.h>

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

void       send_contig ( pami_context_t    context,
			 size_t            dispatch, 
			 void            * metadata,
			 size_t            hdrsize,
			 char            * buffer,
			 size_t            sndlen, 
			 pami_endpoint_t   target );

void _rmw  ( pami_context_t      context,
	     void              * src_ptr_in,
	     void              * src_ptr_out,
	     void              * target_ptr,
	     pami_atomic_t       op,
	     pami_type_t         type,
	     pami_endpoint_t     target );

void packStrided   ( A1PAMI_Info_t     * ainfo,
		     void              * dstbuf,
		     void              * srcbuf );		  

void send_strided  ( pami_context_t    context,
		     size_t            dispatchc,
		     size_t            dispatchs,
		     A1PAMI_Info_t   * ainfo,
		     size_t            hdrsize,
		     char            * buffer,
		     pami_endpoint_t   target );

void     idata_fn_scale1 (void                  * target,
			  void                  * src,
			  size_t                  bytes,
			  void                  * cookie);
void     ldata_fn_scale1 (void                  * target,
			  void                  * src,
			  size_t                  bytes,
			  void                  * cookie);
void     data_fn_scale1 (void                  * target,
			 void                  * src,
			 size_t                  bytes,
			 void                  * cookie);

void     idata_fn    (void                  * target,
		      void                  * src,
		      size_t                  bytes,
		      void                  * cookie);
void     ldata_fn    (void                  * target,
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
void dispatch_contig_int
( pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv );       /**< OUT: receive message structure */

void dispatch_contig_long
( pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv );       /**< OUT: receive message structure */

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

#endif
