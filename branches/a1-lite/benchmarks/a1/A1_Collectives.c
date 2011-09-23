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
#include <assert.h>
#include <a1.h>

int test_allreduce_int32(A1_reduce_op_t op,
                         int32_t expected,
                         int n,
                         int32_t* in,
                         int32_t* out);

int test_allreduce_int64(A1_reduce_op_t op,
                         int64_t expected,
                         int n,
                         int64_t* in,
                         int64_t* out);

int test_allreduce_float(A1_reduce_op_t op,
                         float expected,
                         int n,
                         float* in,
                         float* out);

int test_allreduce_double(A1_reduce_op_t op,
                          double expected,
                         int n,
                         double* in,
                         double* out);

int main()
{
    A1_Initialize(A1_THREAD_SINGLE);

    int rank, size;
    int i, n;
    int expected;
    A1_reduce_op_t op;
    A1_datatype_t type;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    if (rank == 0) printf("A1 Collectives Correctness Testing - INT32\n");

    for (n = 1; n < 1024; n *= 2)
    {
        if (rank == 0) printf("%d elements\n", n);

        int32_t* in = malloc(n * sizeof(int32_t));
        assert(in != NULL);
        int32_t* out = malloc(n * sizeof(int32_t));
        assert(out != NULL);

        if (rank == 0) printf("A1_SUM/A1_INT32 A\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = ((size - 1) * (size)) / 2;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_SUM/A1_INT32 B\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = size;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_INT32 A\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_INT32 B\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = -rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1 - size;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_INT32 A\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = size - 1;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_INT32 B\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = -rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_INT32 A\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 2;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        for (i = 0; i < size; i++)
            expected *= 2;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_INT32 B\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT32 A\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = 1;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT32 B\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = (rank == 0);
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        test_allreduce_int32(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT32 C\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = 0;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int32(op, expected, n, in, out);

        free(in);
        free(out);
    }

    if (rank == 0) printf("A1 Collectives Correctness Testing - INT64\n");

    for (n = 1; n < 1024; n *= 2)
    {
        if (rank == 0) printf("%d elements\n", n);

        int64_t* in = malloc(n * sizeof(int64_t));
        assert(in != NULL);
        int64_t* out = malloc(n * sizeof(int64_t));
        assert(out != NULL);

        if (rank == 0) printf("A1_SUM/A1_INT64 A\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = ((size - 1) * (size)) / 2;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_SUM/A1_INT64 B\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = size;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_INT64 A\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_INT64 B\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = -rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1 - size;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_INT64 A\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = size - 1;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_INT64 B\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = -rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_INT64 A\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 2;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        for (i = 0; i < size; i++)
            expected *= 2;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_INT64 B\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = rank;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT64 A\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = 1;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT64 B\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = (rank == 0);
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 1;
        test_allreduce_int64(op, expected, n, in, out);

        if (rank == 0) printf("A1_OR/A1_INT64 C\n");
        op = A1_OR;
        for (i = 0; i < n; i++)
            in[i] = 0;
        for (i = 0; i < n; i++)
            out[i] = 0;
        expected = 0;
        test_allreduce_int64(op, expected, n, in, out);

        free(in);
        free(out);
    }

    if (rank == 0) printf("A1 Collectives Correctness Testing - FLOAT\n");

    for (n = 1; n < 1024; n *= 2)
    {
        if (rank == 0) printf("%d elements\n", n);

        float* in = malloc(n * sizeof(float));
        assert(in != NULL);
        float* out = malloc(n * sizeof(float));
        assert(out != NULL);

        if (rank == 0) printf("A1_SUM/A1_FLOAT A\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 1.0f*((size - 1) * (size)) / 2;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_SUM/A1_FLOAT B\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1.0f;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 1.0f*size;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_FLOAT A\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = 1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 0.0f;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_FLOAT B\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = -1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 1.0f - size;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_FLOAT A\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = 1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = size - 1.0f;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_FLOAT B\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = -1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 0.0f;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_FLOAT A\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 2.0f;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 1.0f;
        for (i = 0; i < size; i++)
            expected *= 2;
        test_allreduce_float(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_FLOAT B\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 1.0f*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0f;
        expected = 0.0f;
        test_allreduce_float(op, expected, n, in, out);

        free(in);
        free(out);
    }

    if (rank == 0) printf("A1 Collectives Correctness Testing - DOUBLE\n");

    for (n = 1; n < 1024; n *= 2)
    {
        if (rank == 0) printf("%d elements\n", n);

        double* in = malloc(n * sizeof(double));
        assert(in != NULL);
        double* out = malloc(n * sizeof(double));
        assert(out != NULL);

        if (rank == 0) printf("A1_SUM/A1_DOUBLE A\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 1.0*((size - 1) * (size)) / 2;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_SUM/A1_DOUBLE B\n");
        op = A1_SUM;
        for (i = 0; i < n; i++)
            in[i] = 1.0;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 1.0*size;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_DOUBLE A\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = 1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 0.0;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_MIN/A1_DOUBLE B\n");
        op = A1_MIN;
        for (i = 0; i < n; i++)
            in[i] = -1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 1.0 - size;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_DOUBLE A\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = 1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = size - 1.0;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_MAX/A1_DOUBLE B\n");
        op = A1_MAX;
        for (i = 0; i < n; i++)
            in[i] = -1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 0.0;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_DOUBLE A\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 2.0;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 1.0;
        for (i = 0; i < size; i++)
            expected *= 2;
        test_allreduce_double(op, expected, n, in, out);

        if (rank == 0) printf("A1_PROD/A1_DOUBLE B\n");
        op = A1_PROD;
        for (i = 0; i < n; i++)
            in[i] = 1.0*rank;
        for (i = 0; i < n; i++)
            out[i] = 0.0;
        expected = 0.0;
        test_allreduce_double(op, expected, n, in, out);

        free(in);
        free(out);
    }


    A1_Finalize();

    return 0;
}

int test_allreduce_int32(A1_reduce_op_t op,
                         int32_t expected,
                         int n,
                         int32_t* in,
                         int32_t* out)
{
    int i, rank, size;
    int validation;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    A1_Barrier_group(A1_GROUP_WORLD);

    A1_Allreduce_group(A1_GROUP_WORLD, n, op, A1_INT32, in, out);

    A1_Barrier_group(A1_GROUP_WORLD);

    validation = 0;
    for (i = 0; i < n; i++)
    {
        validation += (out[i] != expected);
    }
    if (validation == 0)
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, n);
        fflush(stdout);
        return (0);
    }
    else
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group failed for %d elements\n", rank, n);
        for (i = 0; i < n; i++)
        {
            printf("out[%d] = %d expected = %d\n", i, out[i], expected);
        }
        fflush(stdout);
        return (1);
    }
}

int test_allreduce_int64(A1_reduce_op_t op,
                         int64_t expected,
                         int n,
                         int64_t* in,
                         int64_t* out)
{
    int i, rank, size;
    int validation;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    A1_Barrier_group(A1_GROUP_WORLD);

    A1_Allreduce_group(A1_GROUP_WORLD, n, op, A1_INT64, in, out);

    A1_Barrier_group(A1_GROUP_WORLD);

    validation = 0;
    for (i = 0; i < n; i++)
    {
        validation += (out[i] != expected);
    }
    if (validation == 0)
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, n);
        fflush(stdout);
        return (0);
    }
    else
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group failed for %d elements\n", rank, n);
        for (i = 0; i < n; i++)
        {
            printf("out[%d] = %d expected = %d\n", i, out[i], expected);
        }
        fflush(stdout);
        return (1);
    }
}

int test_allreduce_float(A1_reduce_op_t op,
                         float expected,
                         int n,
                         float* in,
                         float* out)
{
    int i, rank, size;
    int validation;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    A1_Barrier_group(A1_GROUP_WORLD);

    A1_Allreduce_group(A1_GROUP_WORLD, n, op, A1_FLOAT, in, out);

    A1_Barrier_group(A1_GROUP_WORLD);

    validation = 0;
    for (i = 0; i < n; i++)
    {
        validation += (out[i] != expected);
    }
    if (validation == 0)
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, n);
        fflush(stdout);
        return (0);
    }
    else
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group failed for %d elements\n", rank, n);
        for (i = 0; i < n; i++)
        {
            printf("out[%d] = %lf expected = %lf\n", i, out[i], expected);
        }
        fflush(stdout);
        return (1);
    }
}

int test_allreduce_double(A1_reduce_op_t op,
                         double expected,
                         int n,
                         double* in,
                         double* out)
{
    int i, rank, size;
    int validation;

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    A1_Barrier_group(A1_GROUP_WORLD);

    A1_Allreduce_group(A1_GROUP_WORLD, n, op, A1_DOUBLE, in, out);

    A1_Barrier_group(A1_GROUP_WORLD);

    validation = 0;
    for (i = 0; i < n; i++)
    {
        validation += (out[i] != expected);
    }
    if (validation == 0)
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group successful for %d elements\n", rank, n);
        fflush(stdout);
        return (0);
    }
    else
    {
        if (rank == 0) printf("[%d] A1_Allreduce_group failed for %d elements\n", rank, n);
        for (i = 0; i < n; i++)
        {
            printf("out[%d] = %lf expected = %lf\n", i, out[i], expected);
        }
        fflush(stdout);
        return (1);
    }
}
