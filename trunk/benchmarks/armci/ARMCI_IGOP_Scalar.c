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

void checkanswer(int num, int buffer, int answer)
{
    if ( buffer != answer ) printf("Test %d failed expected: %d actual: %d \n", num, answer, buffer);
    else                    printf("Test %d passed expected: %d actual: %d \n", num, answer, buffer);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    int provided;

    int rank, size;

    char* sum    = "+";
    char* prod   = "*";
    char* max    = "max";
    char* min    = "min";
    char* absmax = "absmax";
    char* absmin = "absmin";
    char* or     = "or";

    int count = 0;
    int buffer;
    int answer;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ARMCI_Init_args(&argc, &argv);

    ARMCI_Barrier();

    /* TESTING SUM OPERATOR */

    buffer = ( rank==1 ? 1 : 0 );
    answer = 1;
    armci_msg_igop(&buffer, sizeof(int), sum); 
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    buffer = 1;
    answer = size;
    armci_msg_igop(&buffer, sizeof(int), sum);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TESTING PROD OPERATOR */

    buffer = ( rank==0 ? 0 : 1 );
    answer = 0;
    armci_msg_igop(&buffer, sizeof(int), prod);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    buffer = 2;
    answer = pow(2,size);
    armci_msg_igop(&buffer, sizeof(int), prod);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TEST OF MAX OPERATOR */

    buffer = rank;
    answer = (size-1);
    armci_msg_igop(&buffer, sizeof(int), max);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TEST OF MIN OPERATOR */

    buffer = rank;
    answer = 0;
    armci_msg_igop(&buffer, sizeof(int), min);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TEST OF ABSMAX OPERATOR */

    buffer = rank;
    answer = rank;
    armci_msg_igop(&buffer, sizeof(int), absmax);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    buffer = -rank;
    answer = -rank;
    armci_msg_igop(&buffer, sizeof(int), absmax);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TEST OF ABSMIN OPERATOR */

    buffer = rank;
    answer = 0;
    armci_msg_igop(&buffer, sizeof(int), absmin);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    buffer = -rank;
    answer = 0;
    armci_msg_igop(&buffer, sizeof(int), absmin);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* TESTING OR OPERATOR */

    buffer = ( rank==0 ? 1 : 0 );
    answer = 1;
    armci_msg_igop(&buffer, sizeof(int), or);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    buffer = 0;
    answer = 0;
    armci_msg_igop(&buffer, sizeof(int), or);
    checkanswer(buffer,answer);
    count++;
    ARMCI_Barrier();

    /* END OF TESTS */

    ARMCI_Finalize();

    MPI_Finalize();

    return 0;
}
