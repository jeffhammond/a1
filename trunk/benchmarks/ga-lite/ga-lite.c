/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * ga-lite.c
 *
 *  Created on: Sep 20, 2010
 *      Author: jeff
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <mpi.h>

/**************************************************************************/
#define GA_MAX_ARRAYS 1000

typedef struct global_array_s
{
    int             active;
    MPI_Win         window;
    MPI_Datatype    type;
    int             ndim;
    int*            dims;
    int*            blocksize;
    char*           name;
}
global_array_t;

typedef struct ga_distribution_s
{
    int             ndim;
    int*            dims;
    int*            chunk;
}
ga_distribution_t;

/**************************************************************************/

MPI_Comm GA_COMM_WORLD;

global_array_t* ga_list;

/**************************************************************************
 * HELPER FUNCTIONS
 **************************************************************************/

int xga_valid_range(global_array_t* handle, int* lo, int* hi,)
{
    int i;

    /* checking for stupidity */
    for (i=0; i<ndim; i++)
    {
        if (lo[i] <= handle->dims[i]) return -1;
        if (handle->dims[i] <= hi[i]) return +1;
    }

    return 0;
}

void xga_create_distribution(int ndim, int* dims, int* blocksize)
{
    int i;
    int

    for (i=0; i<ndim; i++)
    {
        blocksize[i] = dims[i]/nproc;
        while ( nproc*blocksize[i] < dims[i]) blocksize[i]++;
    }

    return;
}



/**************************************************************************/

/* collective */
void GA_Initialize()
{
    int mpi_initialized;
    int mpi_status;

    /* MPI has to be initialized for this implementation to work */
    MPI_Initialized(&mpi_initialized);
    assert(mpi_initialized==1);

    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_dup(MPI_COMM_WORLD,&GA_COMM_WORLD);
    assert(mpi_status==0);

    /* allocate ga list */
    ga_list = malloc( GA_MAX_ARRAYS * sizeof(global_array_t) );
    assert(ga_list != NULL);

    return;
}


/* collective */
void GA_Terminate()
{
    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_free(&GA_COMM_WORLD);
    assert(mpi_status==0);

    free(ga_list);

    return;
}

/**************************************************************************
message  - string to print          [input]
code     - code to print            [input]
local
***************************************************************************/
void GA_Error(char *message, int code)
{
    int mpi_status;

    printf("%s",message);

    mpi_status = MPI_Abort(GA_COMM_WORLD,code);
    assert(mpi_status==0);

    return;
}

/* local */
int GA_Nodeid()
{
    int mpi_status;
    int mpi_rank;

    mpi_status = MPI_Comm_rank(GA_COMM_WORLD,&mpi_rank);
    assert(mpi_status==0);

    return mpi_rank;
}

/* local */
int GA_Nnodes()
{
    int mpi_status;
    int mpi_size;

    mpi_status = MPI_Comm_size(GA_COMM_WORLD,&mpi_size);
    assert(mpi_status==0);

    return mpi_size;
}

/* collective */
void GA_Sync()
{
    int mpi_status;

    mpi_status = MPI_Barrier(GA_COMM_WORLD);
    assert(mpi_status==0);

    return;
}

/**************************************************************************
      array_name        - a unique character string                   [input]
      type              - data type (MT_F_DBL,MT_F_INT,MT_F_DCPL)     [input]
      ndim              - number of array dimensions                  [input]
      dims[ndim]        - array of dimensions                         [input]
      chunk[ndim]       - array of chunks, each element specifies minimum size that
                          given dimensions should be chunked up into  [input]
Creates an ndim-dimensional array using the regular distribution model and returns integer handle representing the array.
The array can be distributed evenly or not. The control over the distribution is accomplished by specifying chunk (block) size for all or some of array dimensions. For example, for a 2-dimensional array, setting chunk[0]=dim[0] gives distribution by vertical strips (chunk[0]*dims[0]); setting chunk[1]=dim[1] gives distribution by horizontal strips (chunk[1]*dims[1]). Actual chunks will be modified so that they are at least the size of the minimum and each process has either zero or one chunk. Specifying chunk[i] as <1 will cause that dimension to be distributed evenly.

As a convenience, when chunk is specified as NULL, the entire array is distributed evenly.

Return value: a non-zero array handle means the call was succesful.
This is a collective operation.
collective
****************************************************************************/
int NGA_Create(int type, int ndim, int dims[], char *array_name, int chunk[])
{
//    MT_F_DBL
//    MT_F_INT
//    MT_F_DCPL
    MPI_Type_size(MPI_INT, &type_size);

    return handle;
}

/* collective */
void GA_Destroy(int g_a)
{
    return;
}

/* collective */
void GA_Zero(int g_a)
{
    return;
}

/* collective */
void GA_Copy(int g_a, int g_b)
{
    return;
}

/* collective */
void GA_Fill(int g_a, void *value)
{
    return;
}

/* collective */
void GA_Scale(int g_a, void *value)
{
    return;
}

/**************************************************************************
g_a, g_b, g_c                        - array handles       [input]
double/complex/int      *alpha       - scale factor        [input]
double/complex/int      *beta        - scale factor        [input]
The arrays (which must be the same shape and identically aligned) are added together element-wise
     c = alpha * a  +  beta * b.
This is a collective operation.
***************************************************************************/
void GA_Add(void *alpha, int g_a, void* beta, int g_b, int g_c)
{
    return;
}


/**************************************************************************
g_a        - global array handle                                                  [input]
ndim       - number of dimensions of the global array
lo[ndim]   - array of starting indices for global array section                   [input]
hi[ndim]   - array of ending indices for global array section                     [input]
buf        - pointer to the local buffer array where the data goes                [output]
ld[ndim-1] - array specifying leading dimensions/strides/extents for buffer array [input]
Copies data from global array section to the local array buffer. The local array is assumed to be have the same number of dimensions as the global array. Any detected inconsitencies/errors in the input arguments are fatal.
Example:
For ga_get operation transfering data from the [10:14,0:4] section of 2-dimensional 15x10 global array into local buffer 5x10 array we have:
lo={10,0}, hi={14,4}, ld={10}
***************************************************************************/
void NGA_Get(int g_a, int lo[], int hi[], void* buf, int ld[])
{
    return;
}

/**************************************************************************
g_a        - global array handle                                                  [output]
ndim       - number of dimensions of the global array
lo[ndim]   - array of starting indices for global array section                   [input]
hi[ndim]   - array of ending indices for global array section                     [input]
buf        - pointer to the local buffer array where the data is                  [input]
ld[ndim-1] - array specifying leading dimensions/strides/extents for buffer array [input]
Copies data from local array buffer to the global array section . The local array is assumed to be have the same number of dimensions as the global array.
Any detected inconsitencies/errors in input arguments are fatal.
***************************************************************************/
void NGA_Put(int g_a, int lo[], int hi[], void* buf, int ld[])
{
    return;
}

/**************************************************************************
g_a        - global array handle                                                  [input]
ndim       - number of dimensions of the global array
lo[ndim]   - array of starting indices for array section                          [input]
hi[ndim]   - array of ending indices for array section                            [input]
buf        - pointer to the local buffer array                                    [input]
ld[ndim-1] - array specifying leading dimensions/strides/extents for buffer array [input]
double/DoubleComplex/long *alpha     scale factor                                 [input]
Combines data from local array buffer with data in the global array section. The local array is assumed to be have the same number of dimensions as the global array.
global array section (lo[],hi[]) += *alpha * buffer
***************************************************************************/
void NGA_Acc(int g_a, int lo[], int hi[], void* buf, int ld[], void* alpha)
{
    return;
}

/**************************************************************************
g_a              array handle         [input]
subscript[ndim]  element subscript    [output]
Return in owner the GA compute process id that 'owns' the data. If any element of subscript[] is out of bounds "-1" is returned.
This operation is local.
***************************************************************************/
int NGA_Locate(int g_a, int subscript[])
{
    return rank;
}

/**************************************************************************
      g_a           - global array handle                               [input]
      ndim          - number of dimensions of the global array
      lo[ndim]      - array of starting indices for array section       [input]
      hi[ndim]      - array of ending indices for array section         [input]
      map[][2*ndim] - array with mapping information                    [output]
      procs[nproc]  - list of processes that own a part of array section[output]
Return the list of the GA processes id that 'own' the data. Parts of the specified patch might be actually 'owned' by several processes. If lo/hi are out of bounds "0" is returned, otherwise return value is equal to the number of processes that hold the data .

        map[i][0:ndim-1]         - lo[i]
        map[i][ndim:2*ndim-1]    - hi[i]
        procs[i]                 - processor id that owns data in patch lo[i]:hi[i]
This operation is local.
***************************************************************************/
int NGA_Locate_region(int g_a, int lo[], int hi[], int map[], int procs[])
{
    return num_proc;
}

/**************************************************************************
g_a  - array handle                  [input]
type - data type                     [output]
ndim - number of dimensions          [output]
dims - array of dimensions           [output]
***************************************************************************/
void NGA_Inquire(int g_a, int *type, int *ndim, int dims[])
{
    return;
}

/**************************************************************************
g_a        - array handle                                [input]
iproc      - process number                              [input]
ndim       - number of dimensions of the global array
lo[ndim]   - array of starting indices for array section [input]
hi[ndim]   - array of ending indices for array section   [input]
If no array elements are owned by process iproc, the range is returned as lo[ ]=0 and hi[ ]= -1 for all dimensions.
This operation is local.
***************************************************************************/
void NGA_Distribution(int g_a, int iproc, int lo[], int hi[])
{
    return;
}
