#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main()
{
    int i,dim=1024;
    double x[dim],y[dim];

    for (i=0;i<dim;i++) x[i]=(float)rand()/RAND_MAX;
    for (i=0;i<dim;i++) printf("x[%4d] = %30.15f\n",i,x[i]);

    for (i=0;i<dim;i++)
    {
        
    }



    return(0);
}
