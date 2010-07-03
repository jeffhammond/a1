#include "bench.h"

void flush_put() {

   if(myrank == 0) {

      DCMF_Request_t put_req[nranks];
      DCMF_Callback_t put_done, put_ack;
      int done_count, ack_count;
      int dest, i;

      put_done.function = done;
      put_done.clientdata = (void *) &done_count;
      put_ack.function = done;
      put_ack.clientdata = (void *) &ack_count;

      char buffer[50];
      sprintf(buffer,"%20s %30s","Flush Latency (us)", "Flush Restart Latency(us)");
      printf("%s \n", buffer);
      fflush(stdout);

      barrier();

      /***********************
      * warmup               *
      ***********************/
       for(i=0; i<SKIP; i++) {

         ack_count = nranks-1;
         for(dest=0; dest<nranks; dest++) { 
            if(dest != myrank) {        
                 DCMF_Put(&put_reg,
                      &put_req[dest],
                      put_done,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      dest,
                      1,
                      &memregion[myrank],
                      &memregion[dest],
                      0,
                      0,
                      put_ack);
              
            }
         }
         while(ack_count) DCMF_Messager_advance();

       }

       /***********************
       * start timer          *
       ***********************/
       ack_count = (nranks-1)*ITERATIONS;
       t_start = DCMF_Timebase();

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {

         done_count = nranks-1;
         for(dest=0; dest<nranks; dest++) {
            if(dest != myrank) {
                 DCMF_Put(&put_reg,
                     &put_req[dest],
                     put_done,
                     DCMF_SEQUENTIAL_CONSISTENCY,
                     dest,
                     1,
                     &memregion[myrank],
                     &memregion[dest],
                     0,
                     0,
                     put_ack);

             }
         }
         while(done_count) DCMF_Messager_advance();

       }

       t_stop = DCMF_Timebase();
       while(ack_count) DCMF_Messager_advance();
       /***********************
       * stop timer          *
       ***********************/
       t_usec = ((t_stop-t_start)/clockMHz);
       printf("%20.0f ", t_usec/(ITERATIONS));

       /***********************
       * start timer          *
       ***********************/
       ack_count = (nranks-1)*ITERATIONS;
       t_start = DCMF_Timebase();

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {

         done_count = nranks-1;
         for(dest=0; dest<nranks; dest++) {
            if(dest != myrank) {
                 DCMF_Restart(&put_req[dest]);
            }
         }
         while(done_count) DCMF_Messager_advance();

       }

       t_stop = DCMF_Timebase();
       while(ack_count) DCMF_Messager_advance();
       /***********************
       * stop timer          *
       ***********************/
       t_usec = ((t_stop-t_start)/clockMHz);
       printf("%20.0f \n", t_usec/(ITERATIONS));

       barrier();
 
     } else {
 
       barrier();

       barrier();

     }
}

void flush_multicast() {

   int i;
   barrier(); 

   if(myrank == 0) {

      char buffer[50];
      sprintf(buffer,"%20s %20s","Flush Latency (us)","Flush Restart Latency (us)");
      printf("%s \n", buffer);
      fflush(stdout);

       for(i=0; i<SKIP; i++) {       
           mc_active=1;
           DCMF_Multicast (&mc_info);
           while(mc_active) DCMF_Messager_advance();     
       }
 
       t_start = DCMF_Timebase();
       /***********************
       * start timer          *
       ***********************/
       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
           mc_active=1;
           DCMF_Multicast (&mc_info);
           while(mc_active) DCMF_Messager_advance();
       }       
       t_stop = DCMF_Timebase();
       /***********************
       * stop timer          *
       ***********************/
       t_usec = ((t_stop-t_start)/clockMHz);
       printf("%20.0f ", t_usec/(ITERATIONS));
       fflush(stdout);

       t_start = DCMF_Timebase();
       /***********************
       * start timer          *
       ***********************/
       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
           mc_active=1;
           DCMF_Restart (mc_req);
           while(mc_active) DCMF_Messager_advance();
       }
       t_stop = DCMF_Timebase();
       /***********************
       * stop timer          *
       ***********************/
       t_usec = ((t_stop-t_start)/clockMHz);
       printf("%20.0f \n", t_usec/(ITERATIONS));
       fflush(stdout);

   } else {

       mc_rcv_active=SKIP+ITERATIONS*2;
       while(mc_rcv_active) DCMF_Messager_advance();

   }

}

void flush_send(unsigned int size) {

  barrier(); 

  if(myrank == 0) {
    int i,j;
    DCMF_Request_t snd_req[nranks];
    DCMF_Callback_t snd_callback;
    char snd_buffer[size];
    unsigned int snd_active;

    char buffer[50];
    sprintf(buffer,"%20s","Flush Latency (us)");
    printf("%s \n", buffer);
    fflush(stdout); 

    snd_callback.function = done;
    snd_callback.clientdata = (void *) &snd_active;

    for(i=0; i<SKIP; i++) {
       snd_active = nranks-1;
       for(j=0; j<nranks; j++) {
           if(j != myrank) {
               DCMF_Send (&snd_reg,
                     &snd_req[j],
                     snd_callback,
                     DCMF_SEQUENTIAL_CONSISTENCY,
                     j,
                     size,
                     snd_buffer,
                     snd_msginfo,
                     1);
           }
       }
       while(snd_active) DCMF_Messager_advance();
    }

    /***********************
    * start timer          *
    ***********************/ 
    t_start = DCMF_Timebase();
    for(i=0; i<ITERATIONS; i++) {
       snd_active = nranks-1;
       for(j=0; j<nranks; j++) {
           if(j != myrank) {
               DCMF_Send (&snd_reg,
                     &snd_req[j],
                     snd_callback,
                     DCMF_SEQUENTIAL_CONSISTENCY,
                     j,
                     size,
                     snd_buffer,
                     snd_msginfo,
                     1);
           }
       }
       while(snd_active) DCMF_Messager_advance();
    }
    t_stop = DCMF_Timebase();
    /***********************
    * stop timer          *
    ***********************/
    t_usec = ((t_stop-t_start)/clockMHz);
    printf("%20.0f \n", t_usec/(ITERATIONS));
    fflush(stdout);

  } else {

    snd_rcv_active = SKIP+ITERATIONS;
    while(snd_rcv_active) DCMF_Messager_advance();

  }

  barrier();

}

int main ()
{
  DCMF_Messager_initialize();

  init();

  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

  memregion_init(1);

  put_init(DCMF_DEFAULT_PUT_PROTOCOL, DCMF_TORUS_NETWORK);

  barrier();

  if(myrank == 0) {
     printf("Number of processes : %d\n", nranks);
     printf("Latency (usec) of Flush with Puts\n");
     fflush(stdout);
  }

  flush_put();

  memregion_finalize();

  multicast_init(DCMF_MEMFIFO_DMA_MSEND_PROTOCOL, 240);

  if(myrank == 0) {
     printf("Latency (usec) of Flush with Multicast\n");
     fflush(stdout);
  }

  flush_multicast();

  send_init(DCMF_DEFAULT_SEND_PROTOCOL, DCMF_TORUS_NETWORK);

  if(myrank == 0) {
     printf("Latency (usec) of Flush with Send\n");
     fflush(stdout);
  }

  flush_send(1);

  barrier();

  if(myrank == 0) {
    printf("Benchmark complete\n");
    fflush(stdout);
  }

  DCMF_Messager_finalize ();

  return 0;
}
