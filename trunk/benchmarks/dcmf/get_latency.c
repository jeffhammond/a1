#include "bench.h"

DCMF_Protocol_t get_protocol;

void get_init (DCMF_Get_Protocol protocol, DCMF_Network network) {
     DCMF_Get_Configuration_t get_conf;
     DCMF_Result result;
     get_conf.protocol = protocol;
     get_conf.network = network;
     result = DCMF_Get_register(&get_protocol, &get_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Get Registration failed with error %d \n", myrank, result);
         fflush(stdout);
     }
}

void get_static_nocallback() {

   if(myrank == 0) {  

      DCMF_Request_t get_req[ITERATIONS+SKIP];
      DCMF_Request_t get_flush;
      DCMF_Callback_t get_done;
      DCMF_Callback_t get_flush_done;
      int flush_done_count;
      int msgsize, i;

      get_done.function = NULL;
      get_done.clientdata = NULL;

      get_flush_done.function = done;
      get_flush_done.clientdata = (void *) &flush_done_count;

      char buffer[50];
      sprintf(buffer,"%20s  %20s", "Msg Size", "Injection Rt (kbps)");
      printf("%s \n", buffer);
      fflush(stdout);

      barrier();

      for(msgsize=1; msgsize<MAX_MSG_SIZE; msgsize*=2) {

       /***********************
       * warmup               *
       ***********************/
       for(i=0; i<SKIP; i++) {
          DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
       }

       flush_done_count = 1;
       DCMF_Get( &get_protocol,
               &get_flush,
               get_flush_done,
               DCMF_SEQUENTIAL_CONSISTENCY,
               1,
               1,
               &memregion[myrank],
               &memregion[myrank+1],
               0,
               0);
       while(flush_done_count) DCMF_Messager_advance();

       /***********************
       * start timer          *
       ***********************/

       t_start = DCMF_Timebase();

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
            DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
         }         

         flush_done_count = 1;
         DCMF_Get( &get_protocol,
               &get_flush,
               get_flush_done,
               DCMF_SEQUENTIAL_CONSISTENCY,
               1,
               1,
               &memregion[myrank],
               &memregion[myrank+1],
               0,
               0);
          while(flush_done_count) DCMF_Messager_advance(); 
          t_stop = DCMF_Timebase();

          /***********************
          * stop timer          *
          ***********************/

          t_usec = ((t_stop-t_start)/clockMHz);
          printf("%20d %20.0f \n", msgsize, t_usec/(ITERATIONS));         

      }
  
      barrier();

   } else {

      barrier();
   
      barrier();

   }
}  

void get_static_callback() {

   if(myrank == 0) {

      DCMF_Request_t get_req[ITERATIONS+SKIP];
      DCMF_Callback_t get_done;
      int done_count;
      int msgsize, i;

      get_done.function = done;
      get_done.clientdata = (void *) &done_count;

      char buffer[50];
      sprintf(buffer,"%20s  %20s", "Msg Size", "Injection Rt (kbps)");
      printf("%s \n", buffer);
      fflush(stdout);

      barrier();

      for(msgsize=1; msgsize<MAX_MSG_SIZE; msgsize*=2) {

       /***********************
       * warmup               *
       ***********************/
       done_count = SKIP;
       for(i=0; i<SKIP; i++) {
          DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
       }
       while(done_count) DCMF_Messager_advance();

       /***********************
       * start timer          *
       ***********************/

       t_start = DCMF_Timebase();
       done_count = ITERATIONS;

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
            DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
         }

         while(done_count) DCMF_Messager_advance();
         t_stop = DCMF_Timebase();

         /***********************
         * stop timer          *
         ***********************/

         t_usec = ((t_stop-t_start)/clockMHz);
         printf("%20d %20.0f \n", msgsize, t_usec/(ITERATIONS));

       }

       barrier();
 
     } else {
 
       barrier();

       barrier();

     }
}

void get_dynamic_callback() {

   if(myrank == 0) {

      DCMF_Request_t get_req[ITERATIONS+SKIP];
      DCMF_Callback_t get_done;
      int done_count;
      int msgsize, i;

      get_done.function = done;
      get_done.clientdata = (void *) &done_count;

      char buffer[50];
      sprintf(buffer,"%20s  %20s", "Msg Size", "Injection Rt (kbps)");
      printf("%s \n", buffer);
      fflush(stdout);

      barrier();

      for(msgsize=1; msgsize<MAX_MSG_SIZE; msgsize*=2) {

       /***********************
       * warmup               *
       ***********************/
       done_count = SKIP;
       for(i=0; i<SKIP; i++) {
          DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_RELAXED_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
       }
       while(done_count) DCMF_Messager_advance();

       /***********************
       * start timer          *
       ***********************/

       t_start = DCMF_Timebase();
       done_count = ITERATIONS;

       for(i=SKIP; i<ITERATIONS+SKIP; i++) {
            DCMF_Get(&get_protocol,
                  &get_req[i],
                  get_done,
                  DCMF_RELAXED_CONSISTENCY,
                  1,
                  msgsize,
                  &memregion[myrank],
                  &memregion[myrank+1],
                  i*msgsize,
                  i*msgsize);
         }

         while(done_count) DCMF_Messager_advance();
         t_stop = DCMF_Timebase();

         /***********************
         * stop timer          *
         ***********************/ 

         t_usec = ((t_stop-t_start)/clockMHz);
         printf("%20d %20.0f \n", msgsize, t_usec/(ITERATIONS));

       }

       barrier();

     } else {

       barrier();

       barrier();

     }
}

int main ()
{
  DCMF_Messager_initialize();

  init();

  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

  memregion_init(MAX_BUF_SIZE*nranks);

  get_init(DCMF_DEFAULT_GET_PROTOCOL, DCMF_TORUS_NETWORK);

  barrier();

  if(myrank == 0) {
     printf("Get Latency (usec) with static routing and nocallback \n");
     fflush(stdout);
  }
  get_static_nocallback();

  if(myrank == 0) {
     printf("Get Latency (usec) with static routing and callback \n");
     fflush(stdout);
  }
  get_static_callback();

  if(myrank == 0) {
     printf("Get Latency (usec) with dynamic routing and callback \n");
     fflush(stdout);
  }
  get_dynamic_callback();

  barrier();

  printf("[%d] Benchmark complete\n", myrank);
  fflush(stdout);

  memregion_finalize(); 
 
  DCMF_Messager_finalize ();

  return 0;
}
