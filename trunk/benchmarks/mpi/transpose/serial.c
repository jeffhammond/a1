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
    int dim;
    matrix_t a; /* input reference */
    matrix_t b; /* in/out in-place */
    matrix_t d; /* output out-of-place */

    dim = ( argc>1 ? atoi(argv[1]) : 100 );
    printf("dim = %d\n",dim);

    alloc_matrix(&a, dim);
    alloc_matrix(&b, dim);
    alloc_matrix(&d, dim);

    random_matrix(&a);
    copy_matrix(&a,&b);
    zero_matrix(&d);

    if (dim<10)
    {
        printf("BEFORE\n");
        printf("=============\n");
        print_matrix(&a);
        printf("=============\n");
        print_matrix(&b);
        printf("=============\n");
        print_matrix(&d);
        printf("=============\n");
    }

    trans_matrix_ip(&b);
    trans_matrix_oop(&a,&d);

    if (dim<10)
    {
        printf("AFTER\n");
        printf("=============\n");
        print_matrix(&a);
        printf("=============\n");
        print_matrix(&b);
        printf("=============\n");
        print_matrix(&d);
        printf("=============\n");
    }

    printf("testing if in-place is the same as out-of-place\n");
    compare_matrix(&b,&d);
    printf("done\n");

    free_matrix(&a);
    free_matrix(&b);
    free_matrix(&d);

    return 0;
}











