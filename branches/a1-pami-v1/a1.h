
typedef enum 
{
  A1_SUCCESS = 0,
  A1_ERROR,
} A1_Result_t;

#define A1_THREAD_SINGLE     0
#define A1_THREAD_FUNNELED   1
#define A1_THREAD_SERIALIZED 2
#define A1_THREAD_MULTIPLE   3

typedef enum
{
  A1_INT32, /**< int32 */
  A1_INT64, /**< int64 */
  A1_UINT32, /**< uint32 */
  A1_UINT64, /**< uint64 */
  A1_FLOAT, /**< single-precision */
  A1_DOUBLE, /**< double-precision */
} A1_datatype_t;


typedef enum
{
  A1_FETCH_AND_ADD,
  A1_SWAP,
} A1_atomic_op_t;

/**
 * \brief A1 non-blocking handle type.
 *
 * \see A1_Test_handle, A1_Test_handle_list,
 *      A1_Wait_handle, A1_Wait_handle_list,
 *      A1_Reset_handle, A1_Reset_handle_list
 *
 * \ingroup TYPEDEFS
 *
 */

typedef void* A1_handle_t;

/**
 * \brief A1 io vector type.
 *
 * \see A1_PutV, A1_GetV, A1_PutAccV
 *
 * \ingroup TYPEDEFS
 *
 */

typedef struct A1_iov_t
{
  void **source_ptr_ar;
  void **target_ptr_ar;
  int size; /*In bytes*/
  int ptr_ar_len;
} A1_iov_t;

/**
 * Initialize the A1 Library
 */
int A1_Initialize (int threadlevel); 

/**
 * Cleanup the A1 Library
 */
int A1_Finalize(void);

/**
 * * \brief Blocking copy of contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (local) source memory.
 * * \param[in] target_ptr  Starting address in the (remote) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * *
 * * \see A1_NbPut, A1_MultiPut
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_Put(int target, void* source_ptr, void* target_ptr, int bytes);

/**
 * * \brief Non-Blocking copy of contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (local) source memory.
 * * \param[in] target_ptr  Starting address in the (remote) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * * \param[out] handle    Opaque handle for the request
 * *
 * * \see A1_Put, A1_NbMultiPut
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPut(int target,
      void* source_ptr,
      void* target_ptr,
      int bytes,
      A1_handle_t handle);

/**
 * * \brief Blocking copy of strided data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] count      Block size in each dimension in bytes.
 * * \param[in] stride_levels  The number of levels of stride.
 * * \param[in] source_ptr   Starting address in the (local) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at source in bytes.
 * * \param[in] target_ptr   Starting address in the (remote) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at target in bytes.
 * *
 * * \see A1_Put, A1_PutV, A1_MultiPut, A1_MultiPutS, A1_MultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_PutS(int target,
      int stride_levels,
      int *block_sizes,
      void* source_ptr,
      int *src_stride_ar,
      void* target_ptr,
      int *trg_stride_ar);

/**
 * * \brief Non-Blocking copy of strided data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] count      Block size in each dimension in bytes.
 * * \param[in] stride_levels  The number of levels of stride.
 * * \param[in] source_ptr   Starting address in the (local) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at source in bytes.
 * * \param[in] target_ptr   Starting address in the (remote) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at target in bytes.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPutS(int target,
       int stride_levels,
       int *block_sizes,
       void* source_ptr,
       int *src_stride_ar,
       void* target_ptr,
       int *trg_stride_ar,
       A1_handle_t handle);

/**
 * * \brief Blocking copy of non-contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_PutV(int target, A1_iov_t *iov_ar, int ar_len);

/**
 * * \brief Non-Blocking copy of non-contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPutV(int target, A1_iov_t *iov_ar, int ar_len, A1_handle_t handle);

/**
 * * \brief Blocking accumulate of contiguous data from local memory onto remote memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (local) source memory.
 * * \param[in] target_ptr  Starting address in the (remote) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * * \param[in] a1_type    Datatype of buffer and scaling factor.
 * * \param[in] scaling    Factor for scaling source.
 * *
 * * \see A1_Put, A1_Copy, A1_CopyAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_PutAcc(int target,
       void* source_ptr,
       void* target_ptr,
       int bytes,
       A1_datatype_t a1_type,
       void* scaling);

/**
 * * \brief Non-Blocking accumulate of contiguous data from local memory onto remote memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (local) source memory.
 * * \param[in] target_ptr  Starting address in the (remote) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * * \param[in] a1_type    Datatype of buffer and scaling factor.
 * * \param[in] scaling    Factor for scaling source
 * * \param[out] handle    Opaque A1 handle
 * *
 * * \see A1_Put, A1_Copy, A1_CopyAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPutAcc(int target,
        void* source_ptr,
        void* target_ptr,
        int bytes,
        A1_datatype_t a1_type,
        void* scaling,
        A1_handle_t handle);

/**
 * * \brief Blocking accumulate of strided data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] stride_levels  The number of levels of stride.
 * * \param[in] block_sizes   Block size in each dimension in bytes.
 * * \param[in] source_ptr   Starting address in the (local) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at source in bytes.
 * * \param[in] target_ptr   Starting address in the (remote) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at target in bytes.
 * * \param[in] a1_type     Type of data and scaling factor
 * * \param[in] *scaling    Scaling factor in the accumulate operation.
 * *
 * * \see A1_Put, A1_PutS, A1_PutAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_PutAccS(int target,
       int stride_levels,
       int *block_sizes,
       void* source_ptr,
       int *src_stride_ar,
       void* target_ptr,
       int *trg_stride_ar,
       A1_datatype_t a1_type,
       void* scaling);

/**
 * * \brief Non-Blocking accumulate of strided data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] stride_levels  The number of levels of stride.
 * * \param[in] block_sizes   Block size in each dimension in bytes.
 * * \param[in] source_ptr   Starting address in the (local) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at source in bytes.
 * * \param[in] target_ptr   Starting address in the (remote) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at target in bytes.
 * * \param[in] a1_type     Type of data and scaling factor
 * * \param[in] *scaling    Scaling factor in the accumulate operation.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_Put, A1_PutS, A1_PutAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPutAccS(int target,
        int stride_levels,
        int *block_sizes,
        void* source_ptr,
        int *src_stride_ar,
        void* target_ptr,
        int *trg_stride_ar,
        A1_datatype_t a1_type,
        void* scaling,
        A1_handle_t handle);

/**
 * * \brief Blocking accumulate of non-contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of
 * *               chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * * \param[in] a1_type     Type of data and scaling factor
 * * \param[in] *scaling    Scaling factor in the accumulate operation.
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_PutAccV(int target,
       A1_iov_t *iov_ar,
       int ar_len,
       A1_datatype_t a1_type,
       void* scaling);

/**
 * * \brief Non-Blocking accumulate of non-contiguous data from local memory to remote memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of
 * *               chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * * \param[in] a1_type     Type of data and scaling factor
 * * \param[in] *scaling    Scaling factor in the accumulate operation.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbPutAccV(int target,
        A1_iov_t *iov_ar,
        int ar_len,
        A1_datatype_t a1_type,
        void* scaling,
        A1_handle_t handle);

/**
 * * \brief Blocking copy of contiguous data from remote memory to local memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (remote) source memory.
 * * \param[in] target_ptr  Starting address in the (local) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * *
 * * \see A1_NbGet, A1_Put, A1_Copy
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_Get(int target, void* source_ptr, void* target_ptr, int bytes);

/**
 * * \brief Non-Blocking copy of contiguous data from remote memory to local memory.
 * *
 * * \param[out] rc      The error code.
 * * \param[in] target    Rank of the remote process.
 * * \param[in] source_ptr  Starting address in the (remote) source memory.
 * * \param[in] target_ptr  Starting address in the (local) target memory.
 * * \param[in] bytes     Amount of data to transfer in bytes.
 * * \param[out] handle    Opaque handle for the request
 * *
 * * \see A1_Get, A1_Put, A1_Copy
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbGet(int target,
      void* source_ptr,
      void* target_ptr,
      int bytes,
      A1_handle_t handle);

/**
 * * \brief Blocking copy of strided data from remote memory to local memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] source_ptr   Starting address in the (remote) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at (remote process) source in bytes.
 * * \param[in] target_ptr   Starting address in the (local) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at (local process) target in bytes.
 * * \param[in] count      Block size in each dimension in bytes.
 * * \param[in] stride_levels  The number of levels of stride.
 * *
 * * \see A1_Put, A1_Get, A1_Copy, A1_PutAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_GetS(int target,
      int stride_levels,
      int *block_sizes,
      void* source_ptr,
      int *src_stride_ar,
      void* target_ptr,
      int *trg_stride_ar);

/**
 * * \brief Non-Blocking copy of strided data from remote memory to local memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] source_ptr   Starting address in the (remote) source memory.
 * * \param[in] src_stride_ar  Array of stride distances at (remote process) source in bytes.
 * * \param[in] target_ptr   Starting address in the (local) target memory.
 * * \param[in] trg_stride_ar  Array of stride distances at (local process) target in bytes.
 * * \param[in] count      Block size in each dimension in bytes.
 * * \param[in] stride_levels  The number of levels of stride.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_Put, A1_Get, A1_Copy, A1_PutAcc
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbGetS(int target,
       int stride_levels,
       int *block_sizes,
       void* source_ptr,
       int *src_stride_ar,
       void* target_ptr,
       int *trg_stride_ar,
       A1_handle_t handle);
/**
 * * \brief Blocking copy of non-contiguous data from remote memory to local memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of
 * *               chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_GetV(int target, A1_iov_t *iov_ar, int ar_len);

/**
 * * \brief Non-Blocking copy of non-contiguous data from remote memory to local memory.
 * *
 * * \param[out] rc       The error code.
 * * \param[in] target     Rank of the remote process.
 * * \param[in] iov_ar     Array of io vectors. Each vector represents a set of
 * *               chunks of same size.
 * * \param[in] ar_len     Number of elements in the array.
 * * \param[out] handle     Opaque A1 handle
 * *
 * * \see A1_NbPut, A1_NbPutV, A1_NbMultiPut, A1_NbMultiPutS, A1_NbMultiPutV
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_NbGetV(int target, A1_iov_t *iov_ar, int ar_len, A1_handle_t handle);

/**
 * * \brief Blocking get-and-accumulate operation (aka RMW) on arbitrary data.
 * *    This operation is message-wise atomic.
 * *
 * * \warning Obviously, the user cannot mix RMW with other operations without
 * *     proper synchronization.
 * *
 * * \param[out] rc        The error code.
 * * \param[in] target      Rank of the target process.
 * * \param[in] source_ptr_in  Pointer of modification data at source process.
 * * \param[in] source_ptr_out  Pointer of original read data at source process.
 * * \param[in] target_ptr    Pointer of modified data at target process.
 * * \param[in] bytes      Number of bytes to update.
 * * \param[in] op        Operation to be performed.
 * * \param[in] a1_type     Type of data and value.
 * *
 * * \ingroup DATA_TRANSFER
 * */

int A1_Rmw(int target,
     void* source_ptr_in,
     void* source_ptr_out,
     void* target_ptr,
     int bytes,
     A1_atomic_op_t op,
     A1_datatype_t a1_type);

int A1_AllocSegment(void **buf, int size);

int A1_Flush (int target);

int A1_AllFence ();
