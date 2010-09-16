#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

static __inline__ unsigned long long getticks(void)
{
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;
  return(result);
}

int main()
{
    int i,j,count=1,dim=1024;
    double x[dim],y1[dim],y2[dim];
    double t0,t1;

    printf("TESTING IMPLEMENTATIONS OF VECTOR ABSOLUTE VALUE\n");

    for (i=0;i<dim;i++) x[i] = 1.0 - 2*(double)rand()/(double)RAND_MAX;
    //for (i=0;i<dim;i++) printf("x[%4d] = %30.15f\n",i,x[i]);
    for (i=0;i<dim;i++) y1[i] = 0.0;
    for (i=0;i<dim;i++) y2[i] = 0.0;

    printf("if-else VERSION\n");

    // WARM-UP
    for (i=0;i<dim;i++)
    {
        if (x[i]>0.0) y1[i] = x[i];
        else          y1[i] = -x[i];
    }

    // TIMING
    t0 = getticks();
    for (j=0;j<count;j++)
        for (i=0;i<dim;i++)
        {
            if (x[i]>0.0) y1[i] = x[i];
            else          y1[i] = -x[i];
        }
    t1 = getticks();
    printf("time for if-else version = %30llu\n",t1-t0);

    printf("a?:b:c VERSION\n");

    // WARM-UP
    for (i=0;i<dim;i++)
        y1[i] = x[i]>0.0 ? x[i] : -x[i];

    // TIMING
    t0 = getticks();
    for (j=0;j<count;j++)
        for (i=0;i<dim;i++)
            y1[i] = x[i]>0.0 ? x[i] : -x[i];
    t1 = getticks();
    printf("time for  a?b:c  version = %30llu\n",t1-t0);

    printf("FABS VERSION\n");

    // WARM-UP
    for (i=0;i<dim;i+=2)
        y1[i] = fabs(x[i]);

    // TIMING
    t0 = getticks();
    for (j=0;j<count;j++)
        for (i=0;i<dim;i+=2)
            y1[i] = fabs(x[i]);
    t1 = getticks();
    printf("time for  fabs() version = %30llu\n",t1-t0);

    printf("ASM VERSION\n");

    double _Complex z1,z2;

    // WARM-UP
    for (i=0;i<dim;i+=2)
    {
       z1 = __lfpd(&x[i]); 
       z2 = __fpabs(z1);
       __stfpd(&y2[i],z2); 
    }

    // TIMING
    t0 = getticks();
    for (j=0;j<count;j++)
        for (i=0;i<dim;i+=2)
        {
           z1 = __lfpd(&x[i]); 
           z2 = __fpabs(z1);
           __stfpd(&y2[i],z2); 
        }
    t1 = getticks();
    printf("time for  ASM    version = %30llu\n",t1-t0);

    printf("VERIFYING ASM VERSION\n");

    for (i=0;i<dim;i++)
        if (y1[i] != y2[i])
            printf("%4d %30.15f %30.15f %30.15f\n",i,x[i],y1[i],y2[i]);

    printf("ALL DONE\n");

    return(0);
}
