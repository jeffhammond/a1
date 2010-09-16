#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int main(int argc, char* argv[])
{
    int i,j,k,count=10;

    int dim = ( argc>1 ? atoi(argv[1]) : 1024 );
    printf("TESTING DP ACCUMULATE OF DIMENSION %d\n",dim);

    unsigned long long t0, t1;

    double* a;
    double* b;
    double* c;

    double  scale = 0.1;

    posix_memalign((void**)&a, 16*sizeof(double), dim*sizeof(double));
    posix_memalign((void**)&b, 16*sizeof(double), dim*sizeof(double));
    posix_memalign((void**)&c, 16*sizeof(double), dim*sizeof(double));

    for (i=0;i<dim;i++) a[i] = 1.0 - 2*(double)rand()/(double)RAND_MAX;

    printf("BASIC VERSION\n");

    // WARM-UP
    for (i=0;i<dim;i++) b[i] = 0.0;
    for (i=0;i<dim;i++) b[i] += scale*a[i];

    // TIMING
    for (i=0;i<dim;i++) b[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i++) b[i] += scale*a[i];
    }
    t1 = getticks();
    printf("time for basic version        = %30llu\n",t1-t0);

    printf("INTRINSICS VERSION 1\n");

    // WARM-UP
    for (i=0;i<dim;i++) c[i] = 0.0;
    for (i=0;i<dim;i+=2)
    {
        __stfpd(&c[i], __fxcpmadd( __lfpd(&c[i]), __lfpd(&a[i]), scale) );
    }

    // TIMING
    for (i=0;i<dim;i++) c[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i+=2)
        {
            __stfpd(&c[i], __fxcpmadd( __lfpd(&c[i]), __lfpd(&a[i]), scale) );
        }
    }
    t1 = getticks();
    printf("time for intrinsics version 1 = %30llu\n",t1-t0);

    // VERIFICATION
    for (i=0;i<dim;i++)
    {
        if (b[i] != c[i])
        {
            printf("%4d %30.15f %30.15f\n",i,b[i],c[i]);
        }
    }

    printf("INTRINSICS VERSION 2\n");

    // WARM-UP
    for (i=0;i<dim;i++) c[i] = 0.0;
    for (i=0;i<dim;i+=4)
    {
        {
            double _Complex a0, a2, c0, c2;
            a0 = __lfpd(&a[i  ]);
            a2 = __lfpd(&a[i+2]);
            c0 = __lfpd(&c[i  ]);
            c2 = __lfpd(&c[i+2]);
            c0 = __fxcpmadd(c0,a0,scale);
            c2 = __fxcpmadd(c2,a2,scale);
            __stfpd(&c[i  ],c0);
            __stfpd(&c[i+2],c2);
        }
    }

    // TIMING
    for (i=0;i<dim;i++) c[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i+=4)
        {
            {
                double _Complex a0, a2, c0, c2;
                a0 = __lfpd(&a[i  ]);
                a2 = __lfpd(&a[i+2]);
                c0 = __lfpd(&c[i  ]);
                c2 = __lfpd(&c[i+2]);
                c0 = __fxcpmadd(c0,a0,scale);
                c2 = __fxcpmadd(c2,a2,scale);
                __stfpd(&c[i  ],c0);
                __stfpd(&c[i+2],c2);
            }
        }
    }
    t1 = getticks();
    printf("time for intrinsics version 2 = %30llu\n",t1-t0);

    // VERIFICATION
    for (i=0;i<dim;i++)
    {
        if (b[i] != c[i])
        {
            printf("%4d %30.15f %30.15f\n",i,b[i],c[i]);
        }
    }

    printf("INTRINSICS VERSION 3\n");

    // WARM-UP
    for (i=0;i<dim;i++) c[i] = 0.0;
    for (i=0;i<dim;i+=8)
    {
        __stfpd(&c[i  ], __fxcpmadd( __lfpd(&c[i  ]), __lfpd(&a[i  ]), scale) );
        __stfpd(&c[i+2], __fxcpmadd( __lfpd(&c[i+2]), __lfpd(&a[i+2]), scale) );
        __stfpd(&c[i+4], __fxcpmadd( __lfpd(&c[i+4]), __lfpd(&a[i+4]), scale) );
        __stfpd(&c[i+6], __fxcpmadd( __lfpd(&c[i+6]), __lfpd(&a[i+6]), scale) );
    }

    // TIMING
    for (i=0;i<dim;i++) c[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i+=8)
        {
            __stfpd(&c[i  ], __fxcpmadd( __lfpd(&c[i  ]), __lfpd(&a[i  ]), scale) );
            __stfpd(&c[i+2], __fxcpmadd( __lfpd(&c[i+2]), __lfpd(&a[i+2]), scale) );
            __stfpd(&c[i+4], __fxcpmadd( __lfpd(&c[i+4]), __lfpd(&a[i+4]), scale) );
            __stfpd(&c[i+6], __fxcpmadd( __lfpd(&c[i+6]), __lfpd(&a[i+6]), scale) );
        }
    }
    t1 = getticks();
    printf("time for intrinsics version 3 = %30llu\n",t1-t0);

    // VERIFICATION
    for (i=0;i<dim;i++)
    {
        if (b[i] != c[i])
        {
            printf("%4d %30.15f %30.15f\n",i,b[i],c[i]);
        }
    }

    printf("INTRINSICS VERSION 4\n");

    // WARM-UP
    for (i=0;i<dim;i++) c[i] = 0.0;
    for (i=0;i<dim;i+=16)
    {
        __stfpd(&c[i   ], __fxcpmadd( __lfpd(&c[i   ]), __lfpd(&a[i   ]), scale) );
        __stfpd(&c[i+ 2], __fxcpmadd( __lfpd(&c[i+ 2]), __lfpd(&a[i+ 2]), scale) );
        __stfpd(&c[i+ 4], __fxcpmadd( __lfpd(&c[i+ 4]), __lfpd(&a[i+ 4]), scale) );
        __stfpd(&c[i+ 6], __fxcpmadd( __lfpd(&c[i+ 6]), __lfpd(&a[i+ 6]), scale) );
        __stfpd(&c[i+ 8], __fxcpmadd( __lfpd(&c[i+ 8]), __lfpd(&a[i+ 8]), scale) );
        __stfpd(&c[i+10], __fxcpmadd( __lfpd(&c[i+10]), __lfpd(&a[i+10]), scale) );
        __stfpd(&c[i+12], __fxcpmadd( __lfpd(&c[i+12]), __lfpd(&a[i+12]), scale) );
        __stfpd(&c[i+14], __fxcpmadd( __lfpd(&c[i+14]), __lfpd(&a[i+14]), scale) );
    }

    // TIMING
    for (i=0;i<dim;i++) c[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i+=16)
        {
            __stfpd(&c[i   ], __fxcpmadd( __lfpd(&c[i   ]), __lfpd(&a[i   ]), scale) );
            __stfpd(&c[i+ 2], __fxcpmadd( __lfpd(&c[i+ 2]), __lfpd(&a[i+ 2]), scale) );
            __stfpd(&c[i+ 4], __fxcpmadd( __lfpd(&c[i+ 4]), __lfpd(&a[i+ 4]), scale) );
            __stfpd(&c[i+ 6], __fxcpmadd( __lfpd(&c[i+ 6]), __lfpd(&a[i+ 6]), scale) );
            __stfpd(&c[i+ 8], __fxcpmadd( __lfpd(&c[i+ 8]), __lfpd(&a[i+ 8]), scale) );
            __stfpd(&c[i+10], __fxcpmadd( __lfpd(&c[i+10]), __lfpd(&a[i+10]), scale) );
            __stfpd(&c[i+12], __fxcpmadd( __lfpd(&c[i+12]), __lfpd(&a[i+12]), scale) );
            __stfpd(&c[i+14], __fxcpmadd( __lfpd(&c[i+14]), __lfpd(&a[i+14]), scale) );
        }
    }
    t1 = getticks();
    printf("time for intrinsics version 4 = %30llu\n",t1-t0);

    // VERIFICATION
    for (i=0;i<dim;i++)
    {
        if (b[i] != c[i])
        {
            printf("%4d %30.15f %30.15f\n",i,b[i],c[i]);
        }
    }

    printf("BASIC VERSION\n");

    // WARM-UP
    for (i=0;i<dim;i++) b[i] = 0.0;
    for (i=0;i<dim;i++) b[i] += scale*a[i];

    // TIMING
    for (i=0;i<dim;i++) b[i] = 0.0;
    t0 = getticks();
    for (j=0;j<count;j++)
    {
        for (i=0;i<dim;i++) b[i] += scale*a[i];
    }
    t1 = getticks();
    printf("time for basic version        = %30llu\n",t1-t0);

    free(a);
    free(b);
    free(c);

    printf("ALL DONE\n");

    return(0);
}
