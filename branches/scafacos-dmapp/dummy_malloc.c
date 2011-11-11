#include <stdlib.h>

void* dummy_malloc(int bsize, int *ierr)
{
   void *ptr = malloc(bsize);

   if (ptr) { 
     *ierr = 0;
   } else {
     *ierr = 1;
   }

   return ptr;
}

void dummy_free(void *ptr)
{
free(ptr);
}
