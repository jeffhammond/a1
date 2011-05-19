/*
 * The following is a notice of limited availability of the code, and disclairankr
 * which must be included in the prologue of the code and in all source listings
 * of the code.
 *
 * Copyright (c) 2010  Argonne Leadership Computing Facility, Argonne National
 * Laboratory
 *
 * Permission is hereby granted to use, reproduce, prepare derivative works, and
 * to redistribute to others.
 *
 *
 *                          LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * rankt:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclairankr.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclairankr listed
 *   in this license in the docurankntation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the narank of the copyright holders nor the naranks of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringeranknt of that parties
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
 */

#include "matrix.h"

int main(int argc, char* argv[])
{
    int provided;
    int rank,size;
    int rc;

    int dim;
    matrix_t in;
    matrix_t out;

    int gridsize;
    int color;
    MPI_Comm gridcomm;

    int dims[2];
    int periodic[2];
    int coords[2];
    MPI_Comm cartcomm;

    int mycoords[2];
    int partner;
    int tags[2];
    MPI_Request reqs[2];
    MPI_Status stats[2];

    int test;
    int i;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    assert(provided==MPI_THREAD_SINGLE);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    gridsize = sqrt(size);
    if (rank==0) printf("gridsize = %d\n",gridsize); fflush(stdout);

    dim = ( argc>1 ? atoi(argv[1]) : 100 );
    if (rank==0) printf("dim = %d\n",dim); fflush(stdout);

    color = ( rank < (gridsize*gridsize) ? 0 : 1 );
    rc = MPI_Comm_split(MPI_COMM_WORLD,color,rank,&gridcomm);

    MPI_Comm_size(gridcomm,&test);
    //printf("%d: gridcomm size = %d\n",rank,test); fflush(stdout);

    if ( rank < (gridsize*gridsize) )
    {
        /* create a 2d communicator to make things easier */
        dims[0] = gridsize;
        dims[1] = gridsize;
        periodic[0] = 0;
        periodic[1] = 0;
        rc = MPI_Cart_create(gridcomm,2,dims,periodic,0,&cartcomm);
        //printf("%d: cartcomm size = %d\n",rank,test); fflush(stdout);

        /* finding the my partner with rank corresponding to (j,i)
         * in the 2D process grid corresponding to my rank, which is
         * (i,j) in the process grid. */
        rc =  MPI_Cart_coords (cartcomm,rank,2,mycoords);
        coords[0] = mycoords[1];
        coords[1] = mycoords[0];
        rc = MPI_Cart_rank(cartcomm,coords,&partner);
        printf("me = %d partner = %d on grid: (%d,%d) and (%d,%d)\n",
                rank,partner,mycoords[0],mycoords[1],coords[0],coords[1]); fflush(stdout);

        alloc_matrix(&in,  dim);
        alloc_matrix(&out, dim);

        random_matrix_parallel(&in,rank);
        zero_matrix(&out);

        /* print before */
        fflush(stdout);
        MPI_Barrier(cartcomm);
        if (rank==0) printf("BEFORE\n");
        if (rank==0) printf("=============\n");
        MPI_Barrier(cartcomm);
        for (i=0;i<(gridsize*gridsize);i++)
        {
            if (i==rank)
            {
                printf("rank = %d\n",rank);
                print_matrix(&in);
                printf("=============\n");
                fflush(stdout);
            }
            MPI_Barrier(cartcomm);
        }

        tags[0] = 0;
        tags[1] = 0;
#ifdef BOTH_NONBLOCKING_VERSION
        rc = MPI_Irecv(out.data,out.dim,MPI_DOUBLE,partner,tags[0],cartcomm,&reqs[0]);
        rc = MPI_Isend(in.data, in.dim, MPI_DOUBLE,partner,tags[1],cartcomm,&reqs[1]);
        rc = MPI_Waitall(2,reqs,MPI_STATUSES_IGNORE);
#else
        /* lower triangle of processor grid receives */
        if (mycoords[0]<mycoords[1])
        {
            rc = MPI_Recv(out.data,out.dim*out.dim,MPI_DOUBLE,partner,tags[0],cartcomm,&stats[0]);
            trans_matrix_ip(&out);
        }
        else if (mycoords[0]>mycoords[1])
        {
            rc = MPI_Send(in.data, in.dim*in.dim,  MPI_DOUBLE,partner,tags[1],cartcomm);
        }

        /* now the other way */
        if (mycoords[0]>mycoords[1])
        {
            rc = MPI_Recv(out.data,out.dim*out.dim,MPI_DOUBLE,partner,tags[0],cartcomm,&stats[0]);
            trans_matrix_ip(&out);
        }
        else if (mycoords[0]<mycoords[1])
        {
            rc = MPI_Send(in.data, in.dim*in.dim,  MPI_DOUBLE,partner,tags[1],cartcomm);
        }

        /* local transpose of diagonal blocks */
        if (mycoords[0]==mycoords[1])
        {
            trans_matrix_oop(&in,&out);
        }

        /* no communication for the diagonal */
#endif

        /* print after */
        fflush(stdout);
        MPI_Barrier(cartcomm);
        if (rank==0) printf("AFTER\n");
        if (rank==0) printf("=============\n");
        MPI_Barrier(cartcomm);
        for (i=0;i<(gridsize*gridsize);i++)
        {
            if (i==rank)
            {
                printf("rank = %d\n",rank);
                print_matrix(&out);
                printf("=============\n");
                fflush(stdout);
            }
            MPI_Barrier(cartcomm);
        }

        free_matrix(&in);
        free_matrix(&out);

        rc = MPI_Comm_free(&cartcomm);
    }

    rc = MPI_Comm_free(&gridcomm);

    MPI_Barrier(MPI_COMM_WORLD);
    printf("%d: ALL DONE!\n",rank);
    fflush(stdout);
    MPI_Finalize();

    return 0;
}











