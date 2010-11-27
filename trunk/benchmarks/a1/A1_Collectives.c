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
#include <a1.h>
#include <armci.h>

int main()
{
    A1_Initialize(A1_THREAD_SINGLE);

    int rank, size;

    rank = A1_Process_id(A1_GROUP_WORLD);

    if (rank == 0) printf("A1 Collectives Correctness Testing - A1_SUM/A1_INT32 \n");
    test_sum_int32();

    if (rank == 0) printf("A1 Collectives Correctness Testing - A1_PROD/A1_INT32 \n");
    test_prod_int32();

    if (rank == 0) printf("A1 Collectives Correctness Testing - A1_MAX/A1_INT32 \n");
    test_max_int32();

    if (rank == 0) printf("A1 Collectives Correctness Testing - A1_MIN/A1_INT32 \n");
    test_min_int32();

    A1_Finalize();

    return 0;
}

int test_sum_int32()
{
    int i, rank, size, msgsize, bufsize;
    int expected, validation;
    int32_t* buffer;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    bufsize = 1024;
    buffer = (int32_t *) malloc(bufsize*sizeof(int32_t));

    A1_Barrier_group(A1_GROUP_WORLD);

    for (msgsize = 1; msgsize <= 1024 ; msgsize *= 2)
    {
        for (i = 0; i < bufsize; i++) buffer[i] = rank;

        A1_Allreduce_group(A1_GROUP_WORLD,
                           msgsize,
                           A1_SUM,
                           A1_INT32,
                           (void *) buffer,
                           (void *) buffer);

        expected = ((size-1)*(size))/2;

        validation = 0;

        for (i = 0; i < msgsize; i++)
        {
            validation += (buffer[i] != expected);
        }

        if (validation==0)
        {
            printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, msgsize);
        }
        else
        {
            for (i = 0; i < msgsize; i++)
            {
                printf("[%d] A1_Allreduce_group failed at element %d: Expected = %d, Actual = %d\n",
                       rank, i, expected, buffer[i]);
            }
            free(buffer);
            return(1);
        }
        fflush(stdout);

        A1_Barrier_group(A1_GROUP_WORLD);
    }
    free(buffer);
    return(0);
}

int test_prod_int32()
{
    int i, rank, size, msgsize, bufsize;
    int expected, validation;
    int32_t* buffer;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    bufsize = 1024;
    buffer = (int32_t *) malloc(bufsize*sizeof(int32_t));

    A1_Barrier_group(A1_GROUP_WORLD);

    for (msgsize = 1; msgsize <= 1024 ; msgsize *= 2)
    {
        for (i = 0; i < bufsize; i++) buffer[i] = 2;

        A1_Allreduce_group(A1_GROUP_WORLD,
                           msgsize,
                           A1_PROD,
                           A1_INT32,
                           (void *) buffer,
                           (void *) buffer);

        expected = pow(2,size);

        validation = 0;

        for (i = 0; i < msgsize; i++)
        {
            validation += (buffer[i] != expected);
        }

        if (validation==0)
        {
            printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, msgsize);
        }
        else
        {
            for (i = 0; i < msgsize; i++)
            {
                printf("[%d] A1_Allreduce_group failed at element %d: Expected = %d, Actual = %d\n",
                       rank, i, expected, buffer[i]);
            }
            free(buffer);
            return(1);
        }
        fflush(stdout);

        A1_Barrier_group(A1_GROUP_WORLD);
    }
    free(buffer);
    return(0);
}

int test_max_int32()
{
    int i, rank, size, msgsize, bufsize;
    int expected, validation;
    int32_t* buffer;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    bufsize = 1024;
    buffer = (int32_t *) malloc(bufsize*sizeof(int32_t));

    A1_Barrier_group(A1_GROUP_WORLD);

    for (msgsize = 1; msgsize <= 1024 ; msgsize *= 2)
    {
        for (i = 0; i < bufsize; i++) buffer[i] = rank;

        A1_Allreduce_group(A1_GROUP_WORLD,
                           msgsize,
                           A1_MAX,
                           A1_INT32,
                           (void *) buffer,
                           (void *) buffer);

        expected = size-1;

        validation = 0;

        for (i = 0; i < msgsize; i++)
        {
            validation += (buffer[i] != expected);
        }

        if (validation==0)
        {
            printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, msgsize);
        }
        else
        {
            for (i = 0; i < msgsize; i++)
            {
                printf("[%d] A1_Allreduce_group failed at element %d: Expected = %d, Actual = %d\n",
                       rank, i, expected, buffer[i]);
            }
            free(buffer);
            return(1);
        }
        fflush(stdout);

        A1_Barrier_group(A1_GROUP_WORLD);
    }
    free(buffer);
    return(0);
}

int test_min_int32()
{
    int i, rank, size, msgsize, bufsize;
    int expected, validation;
    int32_t* buffer;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    bufsize = 1024;
    buffer = (int32_t *) malloc(bufsize*sizeof(int32_t));

    A1_Barrier_group(A1_GROUP_WORLD);

    for (msgsize = 1; msgsize <= 1024 ; msgsize *= 2)
    {
        for (i = 0; i < bufsize; i++) buffer[i] = -rank;

        A1_Allreduce_group(A1_GROUP_WORLD,
                           msgsize,
                           A1_MIN,
                           A1_INT32,
                           (void *) buffer,
                           (void *) buffer);

        expected = 1-size;

        validation = 0;

        for (i = 0; i < msgsize; i++)
        {
            validation += (buffer[i] != expected);
        }

        if (validation==0)
        {
            printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, msgsize);
        }
        else
        {
            for (i = 0; i < msgsize; i++)
            {
                printf("[%d] A1_Allreduce_group failed at element %d: Expected = %d, Actual = %d\n",
                       rank, i, expected, buffer[i]);
            }
            free(buffer);
            return(1);
        }
        fflush(stdout);

        A1_Barrier_group(A1_GROUP_WORLD);
    }
    free(buffer);
    return(0);
}
