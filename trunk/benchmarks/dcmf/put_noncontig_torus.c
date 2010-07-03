#include "bench.h"

void put_direct(int dim1, int dim2) {

   memregion_init(MAX_DIM*MAX_DIM*sizeof(double)); 

   if(myrank == 0) {

      DCMF_Request_t put_req[dim1];
      DCMF_Callback_t put_done, put_ack;
      int done_count, ack_count;
      int j, k, dst;

      put_done.function = done;
      put_done.clientdata = (void *) &done_count;
      put_ack.function = done;
      put_ack.clientdata = (void *) &ack_count;

      t_usec = 0;
      for(dst=0; dst<nranks; dst++) {

         if(dst !=  myrank) {

           t_start = DCMF_Timebase(); 
           for(j=0; j<ITERS; j++) {                

              ack_count = dim1;
              for(k=0; k<dim1; k++) { 
                 DCMF_Put(&put_reg,
                    &put_req[k],
                    put_done,
                    DCMF_SEQUENTIAL_CONSISTENCY,
                    dst,
                    dim2*sizeof(double),
                    memregion[0],
                    memregion[dst],
                    k*MAX_DIM*sizeof(double),
                    k*MAX_DIM*sizeof(double),
                    put_ack);
               }
               while(ack_count) DCMF_Messager_advance(); 

            }
            t_stop = DCMF_Timebase();
            t_usec += ((t_stop-t_start)/clockMHz);
         }
 
      }
      printf("%20.0f ", t_usec/((nranks-1)*ITERS));
      fflush(stdout);

   }
 
   barrier();

   memregion_finalize();

}

void send_pack(int dim1, int dim2) {

   memregion_init(MAX_DIM*MAX_DIM*sizeof(double));

   if(myrank == 0) {

       DCMF_Request_t snd_req;
       DCMF_Callback_t snd_done;
       DCQuad msginfo;
       int done_count;
       unsigned int j, k, dst, size;
       char *pack_buffer;
       struct noncontig_header pack_header;
       
       pack_buffer = (char *) malloc (sizeof(struct noncontig_header) + dim1*dim2*sizeof(double));

       snd_done.function = done;
       snd_done.clientdata = (void *) &done_count;

       for(dst=0; dst<nranks; dst++) {

         barrier();

         if(dst !=  myrank) {

           for(j=0; j<ITERS; j++) {

                 done_count = 1;

                 DCMF_Memregion_query(memregion[dst], &size, &(pack_header.vaddress));   
                 pack_header.stride = MAX_DIM*sizeof(double);
                 pack_header.d1 = dim1;
                 pack_header.d2 = dim2*sizeof(double);
                 memcpy(pack_buffer, &pack_header, sizeof(pack_header)); 
                 int msgsize = sizeof(pack_header); 
                 for(k=0; k<dim1; k++) {
                      memcpy(pack_buffer + msgsize, window + k*MAX_DIM*sizeof(double), dim2*sizeof(double));
                      msgsize += dim2*sizeof(double);
                 }

                 DCMF_Send(&snd_noncontig_reg,
                    &snd_req,
                    snd_done,
                    DCMF_SEQUENTIAL_CONSISTENCY,
                    dst,
                    msgsize,
                    pack_buffer,
                    &msginfo,
                    1);

                 while(done_count) DCMF_Messager_advance();

           }

         }

       }

       free(pack_buffer);

       t_usec = 0;
       barrier();
       allreduce(-1, (char *) &t_usec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);
       barrier();

       t_avg = t_avg/(nranks-1);
       printf("%20.0f \n", t_avg);
       fflush(stdout);  

     } else {

       int dst; 

       t_usec = 0;
       for(dst=0; dst<nranks; dst++) {

         barrier();

         if(dst ==  myrank) {

            t_start = DCMF_Timebase();

            snd_rcv_noncontig_active = ITERS;
            while(snd_rcv_noncontig_active) DCMF_Messager_advance();

            t_stop = DCMF_Timebase();    
            t_usec = ((t_stop-t_start)/clockMHz);
            t_usec = t_usec/ITERS;

         }
       }

       barrier();
       allreduce(-1, (char *) &t_usec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);
       barrier();
        
     }

     memregion_finalize();

}

int main ()
{
  DCMF_Messager_initialize();
  int dim1, dim2;
  char buffer[50];

  init();

  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  allreduce_init(DCMF_DEFAULT_GLOBALALLREDUCE_PROTOCOL);

  control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

  put_init(DCMF_DEFAULT_PUT_PROTOCOL, DCMF_TORUS_NETWORK);

  send_noncontig_init(DCMF_DEFAULT_SEND_PROTOCOL, DCMF_TORUS_NETWORK);

  if(myrank == 0) {
    sprintf(buffer,"%20s  %20s  %20s", "Dimensions", "DirectPut Latency (us)",\
           "PackSend Latency (us)");
    printf("%s \n", buffer);
    fflush(stdout);
  }

  barrier();
  
  for(dim1=2; dim1<=512; dim1*=2) {
 
    for(dim2=1; dim2<=512; dim2*=2) {

       if(myrank == 0) {
           sprintf(buffer, "%dX%d", dim1, dim2);
           printf("%20s  ", buffer);
           fflush(stdout);
       }

       barrier();

       put_direct(dim1, dim2);
 
       send_pack(dim1, dim2);

    }

  }

  printf("[%d] Benchmark complete\n", myrank);
  fflush(stdout);

  DCMF_Messager_finalize ();

  return 0;
}
