#include "bench.h"
#include "float.h"

void put_static(int pick) {

      unsigned int size, i, dst;
      DCMF_Request_t put_req[ITERATIONS];
      DCMF_Callback_t put_done, put_ack;
      int done_count, ack_count;
      DCMF_NetworkCoord_t myaddr, dstaddr; 
      DCMF_Network ntwk;

      DCMF_Messager_rank2network(myrank, DCMF_TORUS_NETWORK, &myaddr);

      dstaddr.network = myaddr.network;
      dstaddr.torus.x = (myaddr.torus.x+4)%8;
      dstaddr.torus.y = (myaddr.torus.y+4)%8;
      dstaddr.torus.z = (myaddr.torus.z+4)%8;
      dstaddr.torus.t = myaddr.torus.t;
 
      DCMF_Messager_network2rank(&dstaddr, &dst, &ntwk);
 
      put_done.function = done;
      put_done.clientdata = (void *) &done_count;
      put_ack.function = done;
      put_ack.clientdata = (void *) &ack_count;
     
      if(myrank == 0) {
        char buffer[50];
        sprintf(buffer,"%20s  %20s  %20s  %20s  %20s", "Msg Size", "Max Latency (us)",\
               "Min Latency (us)", "Avg Latency (us)", "Latency at 0 (usec)");
        printf("%s \n", buffer);
        fflush(stdout);
      }


      for(size=16*1024; size<=MAX_MSG_SIZE; size*=2) {

         barrier();

//         if((myaddr.torus.x)%pick == 0) { 
         if(myrank == 0) { 

           /***********************
           * start timer          *
           ***********************/
           t_start = DCMF_Timebase();

           ack_count = ITERATIONS;
           for(i=0; i<ITERATIONS; i++) {
               DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_SEQUENTIAL_CONSISTENCY,
                  dst,
                  size,
                  memregion[myrank],
                  memregion[dst],
                  0,
                  size,
                  put_ack);
            }
            while(ack_count) DCMF_Messager_advance();
            t_stop = DCMF_Timebase();

            /***********************
            * stop timer          *
            ***********************/
            t_usec = ((t_stop-t_start)/clockMHz);
            t_usec = t_usec/ITERATIONS;

            barrier();    
 
            allreduce(-1, (char *) &t_usec, (char *) &t_max, 1, DCMF_DOUBLE, DCMF_MAX);
            allreduce(-1, (char *) &t_usec, (char *) &t_min, 1, DCMF_DOUBLE, DCMF_MIN);
            allreduce(-1, (char *) &t_usec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);

            barrier();

            if(myrank == 0) {
              t_avg = t_avg/(nranks/pick);
              printf("%20d %20.0f  %20.0f  %20.0f %20.0f\n", size, t_max, t_min, t_avg, t_usec);
              fflush(stdout);
            }

         } else {

            double d_min, d_max, d_avg;
            d_max = DBL_MAX;
            d_min = DBL_MIN;
            d_avg = 0;           
 
            barrier();

            allreduce(-1, (char *) &d_min, (char *) &t_max, 1, DCMF_DOUBLE, DCMF_MAX);
            allreduce(-1, (char *) &d_max, (char *) &t_min, 1, DCMF_DOUBLE, DCMF_MIN);
            allreduce(-1, (char *) &d_avg, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);
            
            barrier(); 

         }
      }
}

void put_dynamic(int pick) {

      unsigned int size, i, dst;
      DCMF_Request_t put_req[ITERATIONS];
      DCMF_Callback_t put_done, put_ack;
      int done_count, ack_count;
      DCMF_NetworkCoord_t myaddr, dstaddr;
      DCMF_Network ntwk;

      DCMF_Messager_rank2network(myrank, DCMF_TORUS_NETWORK, &myaddr);

      dstaddr.network = myaddr.network;
      dstaddr.torus.x = (myaddr.torus.x+4)%8;
      dstaddr.torus.y = (myaddr.torus.y+4)%8;
      dstaddr.torus.z = (myaddr.torus.z+4)%8;
      dstaddr.torus.t = myaddr.torus.t;

      DCMF_Messager_network2rank(&dstaddr, &dst, &ntwk);

      put_done.function = done;
      put_done.clientdata = (void *) &done_count;
      put_ack.function = done;
      put_ack.clientdata = (void *) &ack_count;

      if(myrank == 0) {
        char buffer[50];
        sprintf(buffer,"%20s  %20s  %20s  %20s %20s", "Msg Size", "Max Latency (us)",\
               "Min Latency (us)", "Avg Latency (us)", "Latency at 0 (usec)");
        printf("%s \n", buffer);
        fflush(stdout);
      }

      for(size=16*1024; size<=MAX_MSG_SIZE; size*=2) {

         barrier();

//       if((myaddr.torus.x)%pick == 0) {
         if(myrank == 0) {

           /***********************
           * start timer          *
           ***********************/
           t_start = DCMF_Timebase();

           ack_count = ITERATIONS;
           for(i=0; i<ITERATIONS; i++) {
               DCMF_Put(&put_reg,
                  &put_req[i],
                  put_done,
                  DCMF_RELAXED_CONSISTENCY,
                  dst,
                  size,
                  memregion[myrank],
                  memregion[dst],
                  0,
                  size,
                  put_ack);
           }
           while(ack_count) DCMF_Messager_advance();
           t_stop = DCMF_Timebase();

           /***********************
           * stop timer          *
           ***********************/
           t_usec = ((t_stop-t_start)/clockMHz);
           t_usec = t_usec/ITERATIONS;

           barrier();

           allreduce(-1, (char *) &t_usec, (char *) &t_max, 1, DCMF_DOUBLE, DCMF_MAX);
           allreduce(-1, (char *) &t_usec, (char *) &t_min, 1, DCMF_DOUBLE, DCMF_MIN);
           allreduce(-1, (char *) &t_usec, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);

           barrier();

           if(myrank == 0) {
              t_avg = t_avg/(nranks/pick);
              printf("%20d %20.0f  %20.0f  %20.0f %20.0f\n", size, t_max, t_min, t_avg, t_usec);
              fflush(stdout);
           }
         } else {

            double d_min, d_max, d_avg;
            d_max = DBL_MAX;
            d_min = DBL_MIN;
            d_avg = 0;

            barrier();

            allreduce(-1, (char *) &d_min, (char *) &t_max, 1, DCMF_DOUBLE, DCMF_MAX);
            allreduce(-1, (char *) &d_max, (char *) &t_min, 1, DCMF_DOUBLE, DCMF_MIN);
            allreduce(-1, (char *) &d_avg, (char *) &t_avg, 1, DCMF_DOUBLE, DCMF_SUM);

            barrier();

         }             
      }
}

int main ()
{
  DCMF_Messager_initialize();

  init();
 
  if(nranks != 512) {
    if(myrank==0) {
      printf("This test requires 512 processes \n");
      fflush(stdout);
      return -1;
    } 
  }

  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  allreduce_init(DCMF_DEFAULT_GLOBALALLREDUCE_PROTOCOL);

  control_init(DCMF_DEFAULT_CONTROL_PROTOCOL, DCMF_DEFAULT_NETWORK);

  memregion_init(MAX_MSG_SIZE*2);

  put_init(DCMF_DEFAULT_PUT_PROTOCOL, DCMF_TORUS_NETWORK);

  barrier();
  
  if(myrank == 0) {
     printf("Put Latency (usec) with static routing - one pair \n");
     fflush(stdout);
  }
  put_static(512);

  /* if(myrank == 0) {
     printf("Put Latency (usec) with static routing - 50p saturation \n");
     fflush(stdout);
  }
  put_static(2);

  if(myrank == 0) {
     printf("Put Latency (usec) with static routing - 25p saturation \n");
     fflush(stdout);
  }
  put_static(4);*/

  if(myrank == 0) {
     printf("Put Latency (usec) with dynamic routing - one pair\n");
     fflush(stdout);
  }
  put_dynamic(512);

  /*if(myrank == 0) {
     printf("Put Latency (usec) with dynamic routing - 50p saturation \n");
     fflush(stdout);
  }
  put_dynamic(2);

  if(myrank == 0) {
     printf("Put Latency (usec) with dynamic routing - 25p saturation \n");
     fflush(stdout);
  }
  put_dynamic(4);*/

  barrier();

  if(myrank == 0) {
    printf("[%d] Benchmark complete\n", myrank);
    fflush(stdout);
  }  

  memregion_finalize(); 
 
  DCMF_Messager_finalize ();

  return 0;
}
