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

void alloc_matrix(matrix_t * mat, int dim)
{
    double * ptr;

    ptr = malloc(dim*dim*sizeof(double));
    assert(ptr);
    mat->data = ptr;
    mat->dim = dim;

    return;
}

void free_matrix(matrix_t * mat)
{
    free(mat->data);

    return;
}

void zero_matrix(matrix_t * mat)
{
    int i,j,dim;
    double * data;

    dim = mat->dim;
    data = mat->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            data[i*dim+j] = 0.0*(i*dim+j);

    return;
}

void setup_matrix(matrix_t * mat)
{
    int i,j,dim;
    double * data;

    dim = mat->dim;
    data = mat->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            data[i*dim+j] = 1.0*(i*dim+j);

    return;
}

void random_matrix(matrix_t * mat)
{
    int i,j,dim;
    double * data;

    dim = mat->dim;
    data = mat->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            data[i*dim+j] = (double)rand()/RAND_MAX;

    return;
}

void copy_matrix(matrix_t * in, matrix_t * out)
{
    int i,j,dim;
    double * idata;
    double * odata;

    assert( in->dim == out->dim );

    dim = in->dim;
    idata = in->data;
    odata = out->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            odata[i*dim+j] = idata[i*dim+j];

    return;
}

void print_matrix(matrix_t * mat)
{
    int i,j,dim;
    double * data;

    dim = mat->dim;
    data = mat->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            printf("in(%d,%d) = %lf\n",i,j,data[i*dim+j]);

    return;
}

void compare_matrix(matrix_t * in, matrix_t * out)
{
    int i,j,dim,count;
    double * idata;
    double * odata;

    assert( in->dim == out->dim );

    dim = in->dim;
    idata = in->data;
    odata = out->data;

    count = 0;
    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            if (odata[i*dim+j] != idata[i*dim+j])
            {
                count++;
                printf("not equal: i=%d j=%d in=%lf out=%lf\n",i,j,idata[i*dim+j],odata[i*dim+j]);
            }

    if (count) printf("mismatch count = %d\n",count);

    return;
}


void trans_matrix_ip(matrix_t * mat)
{
    int i,j,dim;
    double * data;
    double ij,ji;

    dim = mat->dim;
    data = mat->data;

    for(i=0;i<dim;i++)
        for(j=0;j<i;j++)
        {
            ij = data[(i)*dim+(j)];
            ji = data[(j)*dim+(i)];
            data[(i)*dim+(j)] = ji;
            data[(j)*dim+(i)] = ij;
        }

    return;
}

void trans_matrix_oop(matrix_t * in, matrix_t * out)
{
    int i,j,dim;
    double * idata;
    double * odata;

    assert( in->dim == out->dim );

    dim = in->dim;
    idata = in->data;
    odata = out->data;

    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            odata[i*dim+j] = idata[j*dim+i];

    return;
}









