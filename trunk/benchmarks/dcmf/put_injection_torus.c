#include "bench.h"

void put_injection_nocallback() {

      DCMF_Request_t put_req[ITERATIONS+SKIP];
      DCMF_Request_t put_flush;
      DCMF_Callback_t put_done, put_ack;
      DCMF_Callback_t put_flush_done, put_flush_ack;
      int flush_done_count, flush_ack_count;
      unsigned int msgsize, i, dst;

      put_done.function = NULL;
      put_done.clientdata = NULL;
      put_ack.function = NULL;
      put_ack.clientdata = NULL;

      put_flush_done.function = done;
      put_flush_done.clientdata = (void *) &flush_done_count;
      put_flush_ack.function = done;
      put_flush_ack.clientdata = (void *) &flush_ack_count;

      if(myrank == 0) {
        char buffer[50];
        sprintf(buffer,"%20s  %20s", "Msg Size", "Injection Rt (kbps)");
        printf("%s \n", buffer);
        fflush(stdout);
      }

      barrier();

      for(msgsize=1; msgsize<MAX_MSG_SIZE; msgsize*=2) {

      t_sec = 0;
      for(dst=0; dst<nranks; dst++) {

      if(dst != myrank) {

       /***********************
       * warmup               *
       ***********************/
       for(i=0; i<SKIP; i++) {
          DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  dst,
                  msgsize,
                  memregion[myrank],
                  memregion[dst],
                  i*msgsize,
                  i*msgsize,
                  put_ack);
       }

       flush_done_count = 1;
       flush_ack_count = 1;
       DCMF_Put( &put_reg,
               &put_flush,
               put_flush_done,
               DCMF_SEQUENTIAL_CONSISTENCY,
               dst,
               1,
               memregion[myrank],
               memregion[dst],
               0,
               0,
               put_flush_ack);
       while(flush_done_count) DCMF_Messager_advance();

       /***********************
       * start timer          *
       ***********************/

       t_start = DCMF_Timebase();

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
            DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  dst,
                  msgsize,
                  memregion[myrank],
                  memregion[dst],
                  i*msgsize,
                  i*msgsize,
                  put_ack);
         }         

         flush_done_count = 1;
         flush_ack_count += 1;
         DCMF_Put( &put_reg,
               &put_flush,
               put_flush_done,
               DCMF_SEQUENTIAL_CONSISTENCY,
               1,
               1,
               memregion[myrank],
               memregion[dst],
               0,
               0,
               put_flush_ack);
          while(flush_done_count) DCMF_Messager_advance(); 
          t_stop = DCMF_Timebase();
          t_sec += ((t_stop-t_start)/clockMHz)/1000000;

          /***********************
          * stop timer          *
          ***********************/

          while(flush_ack_count) DCMF_Messager_advance();

          }
 
          }

          barrier();
          t_sec = t_sec/(nranks - 1); 
          allreduce(-1, (char *) &t_sec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);
          barrier();

          if(myrank == 0) {
             t_avg = t_avg/nranks;
             printf("%20d %20.0f \n", msgsize, ((ITERATIONS)*msgsize)/(t_avg*1024));         
             fflush(stdout);
          } 

      }
  
}  

void put_injection_callback() {

      DCMF_Request_t put_req[ITERATIONS+SKIP];
      DCMF_Callback_t put_done, put_ack;
      int done_count, ack_count;
      unsigned int msgsize, i, dst;

      put_done.function = done;
      put_done.clientdata = (void *) &done_count;
      put_ack.function = done;
      put_ack.clientdata = (void *) &ack_count;

      if(myrank == 0) {
        char buffer[50];
        sprintf(buffer,"%20s  %20s", "Msg Size", "Injection Rt (kbps)");
        printf("%s \n", buffer);
        fflush(stdout);
      }
 
      barrier();

      for(msgsize=1; msgsize<MAX_MSG_SIZE; msgsize*=2) {

      t_sec = 0;
      for(dst=0; dst<nranks; dst++) {

      if(dst != myrank) {

       /***********************
       * warmup               *
       ***********************/
       done_count = SKIP;
       ack_count = SKIP;
       for(i=0; i<SKIP; i++) {
          DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  dst,
                  msgsize,
                  memregion[myrank],
                  memregion[dst],
                  i*msgsize,
                  i*msgsize,
                  put_ack);
       }
       while(done_count) DCMF_Messager_advance();

       /***********************
       * start timer          *
       ***********************/

       t_start = DCMF_Timebase();

       done_count = ITERATIONS;
       ack_count += ITERATIONS;

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
            DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  dst,
                  msgsize,
                  memregion[myrank],
                  memregion[dst],
                  i*msgsize,
                  i*msgsize,
                  put_ack);
         }

         while(done_count) DCMF_Messager_advance();
         t_stop = DCMF_Timebase();
         t_sec += ((t_stop-t_start)/clockMHz)/1000000;

         /***********************
         * stop timer          *
         ***********************/

         while(ack_count) DCMF_Messager_advance();
 
         }

         }

         barrier();
         t_sec = t_sec/(nranks-1);
         allreduce(-1, (char *) &t_sec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);
         barrier();

         if(myrank == 0) {
            t_avg = t_avg/nranks;
            printf("%20d %20.0f \n", msgsize, ((ITERATIONS)*msgsize)/(t_avg*1024));
            fflush(stdout);
         }

       }
 
} 

int main ()
{
  DCMF_Messager_initialize();

  init();

  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  allreduce_init(DCMF_DEFAULT_GLOBALALLREDUCE_PROTOCOL);

  control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

  memregion_init(MAX_BUF_SIZE);

  put_init(DCMF_DEFAULT_PUT_PROTOCOL, DCMF_TORUS_NETWORK);

  barrier();

  if(myrank == 0) {
     printf("Put Injection Rate (kbps) with static routing and callback \n");
     fflush(stdout);
  }
  put_injection_callback();

  if(myrank == 0) {
     printf("Put Injection Rate (kbps) with static routing and nocallback \n");
     fflush(stdout);
  }
  put_injection_nocallback();

  barrier();

  if(myrank == 0) {
    printf("[%d] Benchmark Complete \n", myrank);
    fflush(stdout);
  }

  memregion_finalize(); 
 
  DCMF_Messager_finalize ();

  return 0;
}
