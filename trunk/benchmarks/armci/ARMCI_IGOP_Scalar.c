/* 
 * The following is a notice of limited availability of the code, and disclaimer
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
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer listed
 *   in this license in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <armci.h>

int main(int argc, char **argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ARMCI_Init_args(&argc, &argv);
    ARMCI_Barrier();

    char* sum    = "+";
    char* prod   = "*";
    char* max    = "max";
    char* min    = "min";
    char* absmax = "absmax";
    char* absmin = "absmin";
    char* or     = "or";

    int buffer;

    /* TESTING SUM OPERATOR */

    buffer = ( rank==1 ? 1 : 0 );
    armci_msg_igop(&buffer, sizeof(int), sum); 

    if ( buffer != 1 ) printf("Validation failed expected: %d actual: %d \n", 1, buffer);
    else               printf("Validation successful \n");
    fflush(stdout);

    buffer = 1;
    armci_msg_igop(&buffer, sizeof(int), sum);

    if ( buffer != size ) printf("Validation failed expected: %d actual: %d \n", size, buffer);
    else                  printf("Validation successful \n");
    fflush(stdout);

    /* TESTING PROD OPERATOR */

    buffer = ( rank==0 ? 0 : 1 );
    armci_msg_igop(&buffer, sizeof(int), prod);

    if ( buffer != 0 ) printf("Validation failed expected: %d actual: %d \n", 0, buffer);
    else               printf("Validation successful \n");
    fflush(stdout);

    buffer = 2;
    armci_msg_igop(&buffer, sizeof(int), prod);

    if ( buffer != pow(2,size) ) printf("Validation failed expected: %d actual: %d \n", pow(2,size), buffer);
    else                         printf("Validation successful \n");
    fflush(stdout);

    /* TEST OF MAX OPERATOR */

    buffer = rank;
    armci_msg_igop(&buffer, sizeof(int), max);

    if ( buffer != (size-1) ) printf("Validation failed expected: %d actual: %d \n", size-1, buffer);
    else                      printf("Validation successful \n");
    fflush(stdout);

    /* TEST OF MIN OPERATOR */

    buffer = rank;
    armci_msg_igop(&buffer, sizeof(int), min);

    if( buffer !=0 ) printf("Validation failed expected: %d actual: %d \n", 0, buffer);
    else             printf("Validation successful \n");
    fflush(stdout);

    /* TEST OF ABSMAX OPERATOR */

    buffer = rank;
    armci_msg_igop(&buffer, sizeof(int), absmax);

    if ( buffer != rank ) printf("Validation failed expected: %d actual: %d \n", rank, buffer);
    else                  printf("Validation successful \n");
    fflush(stdout);

    buffer = -rank;
    armci_msg_igop(&buffer, sizeof(int), absmax);

    if ( buffer != -rank ) printf("Validation failed expected: %d actual: %d \n", -rank, buffer);
    else                   printf("Validation successful \n");
    fflush(stdout);

    /* TEST OF ABSMIN OPERATOR */

    buffer = rank;
    armci_msg_igop(&buffer, sizeof(int), absmin);

    if ( buffer != 0 ) printf("Validation failed expected: %d actual: %d \n", 0, buffer);
    else               printf("Validation successful \n");
    fflush(stdout);

    buffer = -rank;
    armci_msg_igop(&buffer, sizeof(int), absmin);

    if ( buffer != 0 ) printf("Validation failed expected: %d actual: %d \n", 0, buffer);
    else                   printf("Validation successful \n");
    fflush(stdout);

    /* TESTING OR OPERATOR */

    buffer = ( rank==0 ? 1 : 0 );
    armci_msg_igop(&buffer, sizeof(int), or);

    if ( buffer != 1 ) printf("Validation failed expected: %d actual: %d \n", 1, buffer);
    else               printf("Validation successful \n");
    fflush(stdout);

    buffer = 0;
    armci_msg_igop(&buffer, sizeof(int), or);

    if ( buffer != 0 ) printf("Validation failed expected: %d actual: %d \n", 0, buffer);
    else               printf("Validation successful \n");
    fflush(stdout);

    /* END OF TESTS */

    ARMCI_Barrier();

    ARMCI_Finalize();

    MPI_Finalize();

    return 0;
}
