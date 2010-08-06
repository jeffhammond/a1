#include "dcmfdimpl.h"

void* A1DI_Unpack_data_strided(void *pointer, void *trg_ptr, int *trg_stride_ar,\
        int *count, int stride_level)
{
     int i;

     A1U_FUNC_ENTER();

     if(stride_level > 0) {
         for(i=0; i<count[stride_level]; i++)
         {
            pointer = A1DI_Unpack_data_strided(pointer, (void *) ((size_t)trg_ptr + i*trg_stride_ar[stride_level-1]), trg_stride_ar,\
                                count, stride_level-1);
         }
     } else {
         memcpy(trg_ptr, pointer, count[0]);
         pointer = (void *)((size_t)pointer + count[0]);
     }

  fn_exit:
     A1U_FUNC_EXIT();
     return pointer;

  fn_fail:
     goto fn_exit;
}

int A1DI_Unpack_strided(void *packet)
{
     int result = A1_SUCCESS;
     void *temp;
     A1D_Packed_puts_header_t *header;

     A1U_FUNC_ENTER();

     header = (A1D_Packed_puts_header_t *) packet;

     /*Unpacking and Copying data*/
     temp = (void *)((size_t)packet + sizeof(A1D_Packed_puts_header_t));
     A1DI_Unpack_data_strided(temp, header->vaddress, header->trg_stride_ar, header->count,\
                 header->stride_levels);

     if(header->is_getresponse) {
         A1D_Expecting_getresponse = 0;
     }

  fn_exit:
     A1U_FUNC_EXIT();
     return result;

  fn_fail:
     goto fn_exit;
}

void* A1DI_Pack_data_strided(void *pointer, void *source_ptr, int *src_stride_ar,\
        int *count, int stride_level)
{
    int i, size;

    A1U_FUNC_ENTER();

    if(stride_level > 0) {
         for(i=0; i<count[stride_level]; i++)
         {
            pointer = A1DI_Pack_data_strided(pointer, (void *) ((size_t)source_ptr + i*src_stride_ar[stride_level-1]),
                                src_stride_ar, count, stride_level-1);
         }
    } else {
         memcpy( pointer, source_ptr, count[0]);
         pointer = (void *)((size_t)pointer + count[0]);
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return pointer;

  fn_fail:
    goto fn_exit;
}

int A1DI_Pack_strided(void **packet, int *size_packet, void *source_ptr, int *src_stride_ar, void *target_ptr,
        int *trg_stride_ar, int *count, int stride_levels, int is_getresponse)
{
    int result = A1_SUCCESS;
    int i, size_data;
    void *temp;
    A1D_Packed_puts_header_t header;

    A1U_FUNC_ENTER();

    size_data = count[0];
    for(i=1; i<=stride_levels; i++)  size_data *= count[i];
    *size_packet = sizeof(A1D_Packed_puts_header_t) + size_data;

    result = posix_memalign(packet, 64, *size_packet);
    A1U_ERR_POP(result!=A1_SUCCESS,"packet allocation failed \n");

    /*Copying header information*/
    header.vaddress = target_ptr;
    header.stride_levels = stride_levels;
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_levels*sizeof(uint32_t));
    memcpy(header.count, count, (stride_levels+1)*sizeof(uint32_t));
    header.is_getresponse = is_getresponse;  
    memcpy(*packet, &header, sizeof(A1D_Packed_puts_header_t));

    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Packed_puts_header_t));
    A1DI_Pack_data_strided(temp, source_ptr, src_stride_ar, count, stride_levels);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}


