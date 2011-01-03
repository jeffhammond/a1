/********************************************************************
 * The following is a notice of limited availability of the code, and disclaimer
 * which must be included in the prologue of the code and in all source listings
 * of the code.
 *
 * Copyright (c) 2010 Argonne Leadership Computing Facility, Argonne National Laboratory
 *
 * Permission is hereby granted to use, reproduce, prepare derivative works, and
 * to redistribute to others.
 *
 *                 LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer listed
 *    in this license in the documentation and/or other materials
 *    provided with the distribution.
 *
 *  - Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * intellectual property rights.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************/

#include "parmci.h
#include "a1d_api.h"

/* initialization and termination */

int PARMCI_Init()
{
    return A1D_Startup();
}

int PARMCI_Init_args(int *argc, char ***argv)
{
    fprintf(stderr,"PARMCI_Init_args: argc/argv may not be setup properly by DCMF \n");
    return A1D_Startup();
}

void PARMCI_Finalize()
{
    A1D_Cleanup();
    return;
}

/* memory management */

void *PARMCI_Malloc_local(armci_size_t bytes)
{
    return A1D_Allocate_local((long)bytes);
}

int PARMCI_Malloc(void** ptr_arr, armci_size_t bytes)
{
    return A1D_Allocate_shared(ptr_arr, (long)bytes);
}

int PARMCI_Free_local(void* ptr)
{
    A1D_Free_local(ptr);
    return(0);
}

int PARMCI_Free(void* ptr);
{
    A1D_Free_shared(ptr);
    return(0);
}

void *PARMCI_Memat(armci_meminfo_t* meminfo, int memflg)
{
    fprintf(stderr,"PARMCI_Memat: not implemented \n");
    assert(0);
    return;
}

void PARMCI_Memget(size_t bytes, armci_meminfo_t* meminfo, int memflg)
{
    fprintf(stderr,"PARMCI_Memat: not implemented \n");
    assert(0);
    return;
}

/* synchronization */

void PARMCI_Barrier()
{
    /* no need to flush right now since Put/Acc wait on remote completion */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);
    return;
}

void PARMCI_Fence(int proc)
{
    /* no need to flush right now since Put/Acc wait on remote completion */
    return;
}
void PARMCI_AllFence()
{
    /* no need to flush right now since Put/Acc wait on remote completion */
    return;
}

int PARMCI_Test(armci_hdl_t * nb_handle)
{
    /* non-blocking calls aren't, hence no testing is required */
    return(0);
}

int PARMCI_Wait(armci_hdl_t * nb_handle)
{
    /* non-blocking calls aren't, hence no testing is required */
    return(0);
}

int PARMCI_WaitProc(int proc)
{
    /* non-blocking calls aren't, hence no testing is required */
    return(0);
}

int PARMCI_WaitAll()
{
    /* non-blocking calls aren't, hence no testing is required */
    return(0);
}


/* remote atomic update and mutexes */

int PARMCI_Rmw(int op, int *ploc, int *prem, int extra, int proc)
{
    fprintf(stderr,"PARMCI_Rmw: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_Create_mutexes(int num)
{
    fprintf(stderr,"PARMCI_Create_mutexes: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_Destroy_mutexes()
{
    fprintf(stderr,"PARMCI_Destroy_mutexes: not implemented \n");
    assert(0);
    return(-1);
}

void PARMCI_Lock(int mutex, int proc)
{
    fprintf(stderr,"PARMCI_Lock: not implemented \n");
    assert(0);
    return(-1);
}

void PARMCI_Unlock(int mutex, int proc)
{
    fprintf(stderr,"PARMCI_Unlock: not implemented \n");
    assert(0);
    return(-1);
}

/* blocking one-sided */

int PARMCI_Get(void *src, void *dst, int bytes, int proc)
{
    return A1D_GetC(proc, bytes, src, dst);
}

int PARMCI_Put(void *src, void *dst, int bytes, int proc)
{
    return A1D_PutC(proc, bytes, src, dst);
}

int PARMCI_Acc(int type, void *scale, void *src, void* dst, int bytes, int proc)
{
    return A1D_AccC(proc, byes, src, dst, type, scale);
}

int PARMCI_GetS(void *src_ptr, int *src_stride_arr,
                void *dst_ptr, int *dst_stride_arr,
                int *block_sizes, int stride_levels, int proc)
{
    return A1D_GetS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr);
}

int PARMCI_PutS(void *src_ptr, int *src_stride_arr,
                void *dst_ptr, int *dst_stride_arr,
                int *block_sizes, int stride_levels, int proc)
{
    return A1D_PutS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr);
}

int PARMCI_AccS(int optype, void *scale,
                void *src_ptr, int *src_stride_arr,
                void *dst_ptr, int *dst_stride_arr,
                int *count, int stride_levels, int proc)
{
    return A1D_AccS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr,
                    type, scale);
}


int PARMCI_GetV(armci_giov_t * array_descr, int len, int proc)
{
    fprintf(stderr,"PARMCI_GetV: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutV(armci_giov_t * array_descr, int len, int proc)
{
    fprintf(stderr,"PARMCI_PutV: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_AccV(int type, void *scale, armci_giov_t * array_descr, int len, int proc)
{
    fprintf(stderr,"PARMCI_AccV: not implemented \n");
    assert(0);
    return(-1);
}

/* non-blocking one-sided */

int PARMCI_NbGet(void *src, void *dst, int bytes, int proc,
                 armci_hdl_t * nb_handle)
{
    return A1D_GetC(proc, bytes, src, dst);
}

int PARMCI_NbPut(void *src, void *dst, int bytes, int proc,
                 armci_hdl_t * nb_handle)
{
    return A1D_PutC(proc, bytes, src, dst);
}

int PARMCI_NbAcc(int type, void *scale, void *src, void* dst, int bytes, int proc,
                 armci_hdl_t * nb_handle)
{
    return A1D_AccC(proc, byes, src, dst, type, scale);
}

int PARMCI_NbGetS(void *src_ptr, int *src_stride_arr,
                  void *dst_ptr, int *dst_stride_arr,
                  int *block_sizes, int stride_levels, int proc,
                  armci_hdl_t * nb_handle)
{
    return A1D_GetS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr);
}

int PARMCI_NbPutS(void *src_ptr, int *src_stride_arr,
                  void *dst_ptr, int *dst_stride_arr,
                  int *block_sizes, int stride_levels, int proc,
                  armci_hdl_t * nb_handle)
{
    return A1D_PutS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr);
}

int PARMCI_NbAccS(int optype, void *scale,
                  void *src_ptr, int *src_stride_arr,
                  void *dst_ptr, int *dst_stride_arr,
                  int *count, int stride_levels, int proc,
                  armci_hdl_t * nb_handle)
{
    return A1D_AccS(proc, stride_levels, block_sizes,
                    src_ptr, src_stride_arr,
                    dst_ptr, dst_stride_arr,
                    type, scale);
}


int PARMCI_NbGetV(armci_giov_t * array_descr, int len, int proc,
                  armci_hdl_t * nb_handle)
{
    fprintf(stderr,"PARMCI_NbGetV: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_NbPutV(armci_giov_t * array_descr, int len, int proc,
                  armci_hdl_t * nb_handle)
{
    fprintf(stderr,"PARMCI_NbPutV: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_NbAccV(int type, void *scale, armci_giov_t * array_descr, int len, int proc,
                  armci_hdl_t * nb_handle)
{
    fprintf(stderr,"PARMCI_NbAccV: not implemented \n");
    assert(0);
    return(-1);
}

/* ??? extensions */

int PARMCI_Put_flag(void *src_ptr, void *dst_ptr, int bytes, int *flag, int val, int proc)
{
    fprintf(stderr,"PARMCI_Put_flag: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutS_flag(void *src_ptr, int *src_stride_arr,
                     void *dst_ptr, int *dst_stride_arr,
                     int *count, int stride_levels,
                     int *flag, int val, int proc)
{
    fprintf(stderr,"PARMCI_PutS_flag: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutS_flag_dir(void *src_ptr, int *src_stride_arr,
                         void *dst_ptr, int *dst_stride_arr,
                         int *count, int stride_levels,
                         int *flag, int val, int proc)
{
    fprintf(stderr,"PARMCI_PutS_flag_dir: not implemented \n");
    assert(0);
    return(-1);
}

/* CAF extensions */

int PARMCI_PutValueInt(int src, void *dst, int proc)
{
    fprintf(stderr,"PARMCI_PutValueInt: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutValueLong(long src, void *dst, int proc)
{
    fprintf(stderr,"PARMCI_PutValueLong: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutValueFloat(float src, void *dst, int proc)
{
    fprintf(stderr,"PARMCI_PutValueFloat: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_PutValueDouble(double src, void *dst, int proc)
{
    fprintf(stderr,"PARMCI_PutValueDouble: not implemented \n");
    assert(0);
    return(-1);
}

int PARMCI_GetValueInt(void *src, int proc)
{
    fprintf(stderr,"PARMCI_GetValueInt: not implemented \n");
    assert(0);
    return(-1);
}

long PARMCI_GetValueLong(void *src, int proc)
{
    fprintf(stderr,"PARMCI_GetValueLong: not implemented \n");
    assert(0);
    return(-1);
}

float PARMCI_GetValueFloat(void *src, int proc)
{
    fprintf(stderr,"PARMCI_GetValueFloat: not implemented \n");
    assert(0);
    return(-1);
}

double PARMCI_GetValueDouble(void *src, int proc)
{
    fprintf(stderr,"PARMCI_GetValueDouble: not implemented \n");
    assert(0);
    return(-1);
}

/**********************************************/
