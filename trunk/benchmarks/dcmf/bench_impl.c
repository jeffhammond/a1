/* 
* The following is a notice of limited availability of the code, and disclaimer
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
* met:
* 
* - Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
* 
* - Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer listed
*   in this license in the documentation and/or other materials
*   provided with the distribution.
* 
* - Neither the name of the copyright holders nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
* 
* The copyright holders provide no reassurances that the source code
* provided does not infringe any patent, copyright, or any other
* intellectual property rights of third parties.  The copyright holders
* disclaim any liability to any recipient for claims brought against
* recipient by any third party for infringement of that parties
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

#include "bench.h"

/***************************************
*  Generic Init function               *
****************************************/
void init() {
    nranks = DCMF_Messager_size();
    myrank = DCMF_Messager_rank();
    DCMF_Hardware (&hw);
    clockMHz = (double)hw.clockMHz;
}

/***************************************
*  Generic Callback function           *
****************************************/
void done(void *clientdata, DCMF_Error_t *error) {

     --(*((unsigned *) clientdata));

}

/***************************************
*  Receive Done Callback function           *
****************************************/
void rcv_done(void *clientdata, DCMF_Error_t *error) {

     --(*((unsigned *) clientdata));
} 

/***************************************
*  Non-contiguous Callback function           *
****************************************/
void done_noncontig(void *clientdata, DCMF_Error_t *error) {

     int i;
     char *data;
     struct noncontig_header *header = (struct noncontig_header *) clientdata;

     data = (char *) clientdata + sizeof(struct noncontig_header);  
     for(i=0; i<header->d1; i++) {
         memcpy(header->vaddress + i*header->stride, data + i*header->d2, header->d2);
     }    
         
     --snd_rcv_noncontig_active;
}

/***************************************
*  Control Callback function           *
****************************************/
void ctrl_recv(void *clientdata, const DCMF_Control_t *info, size_t peer) {
     memregion[peer] = (DCMF_Memregion_t *) malloc (sizeof(DCMF_Memregion_t));
     memcpy(memregion[peer], info, sizeof(DCMF_Memregion_t));
     --(*((unsigned *) clientdata));
}

/****************************************
*  Send Recv Short Callback function    *
****************************************/
void ack_rcv(void *clientdata, const DCQuad *msginfo,
                 unsigned count, size_t peer, const char *src,
                 size_t bytes) {

     --(*((unsigned *) clientdata));

}

/****************************************
*  Send Recv Short Callback function    *
****************************************/
void snd_rcv_short(void *clientdata, const DCQuad *msginfo,
                 unsigned count, size_t peer, const char *src,
                 size_t bytes) {

     memcpy(target + target_index, src, bytes);
     target_index = target_index + bytes;

     --(*((unsigned *) clientdata));

}

/****************************************
*  Timed Send Recv Short Callback function    *
****************************************/
void timed_snd_rcv_short(void *clientdata, const DCQuad *msginfo,
                 unsigned count, size_t peer, const char *src,
                 size_t bytes) {

     t_start = DCMF_Timebase();
     t_stop = DCMF_Timebase();

     while(((t_stop-t_start)/clockMHz)/1000000 < 1.0) t_stop = DCMF_Timebase();

     printf("[%d] Count = %d Start time : %lld End time : %lld \n", 
           myrank, *((unsigned *) clientdata), t_start, t_stop); 
     fflush(stdout);

     --(*((unsigned *) clientdata));
}

/**************************************************
*  Send Recv Short Noncontig Callback function    *
***************************************************/
void snd_rcv_noncontig_short(void *clientdata, const DCQuad *msginfo,
                 unsigned count, size_t peer, const char *src,
                 size_t bytes) {

     int i;
     char *data;
     struct noncontig_header *header = (struct noncontig_header *) src;

     data = (char *) src + sizeof(struct noncontig_header);
     for(i=0; i<header->d1; i++) {
         memcpy(header->vaddress + i*header->stride, data + i*header->d2, header->d2);
     }

     --(*((unsigned *) clientdata));

}

/**************************************************
*  Send Recv Short Manytomany Callback function    *
***************************************************/
void snd_rcv_manytomany_short(void *clientdata, const DCQuad *msginfo,
                 unsigned count, size_t peer, const char *src,
                 size_t bytes) {

     m2m_header = (struct noncontig_header *) malloc (sizeof(struct noncontig_header));

     memcpy(m2m_header, src, bytes);

     --(*((unsigned *) clientdata));
}

/***************************************
*  Send Recv Callback function           *
****************************************/
DCMF_Request_t* snd_rcv(void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
          size_t sndlen, size_t *rcvlen, char **rcvbuf, DCMF_Callback_t *cb_done) {

     *rcvlen = sndlen;
     *rcvbuf = target + target_index;
     target_index = target_index + sndlen;

     cb_done->function = rcv_done;
     cb_done->clientdata = clientdata;  

     rcv_req_index++;
     return &rcv_req[rcv_req_index-1];
}

/***************************************
*  Timed Send Recv Callback function           *
****************************************/
DCMF_Request_t* timed_snd_rcv(void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
          size_t sndlen, size_t *rcvlen, char **rcvbuf, DCMF_Callback_t *cb_done) {

     target = (char *) malloc (sndlen);

     *rcvlen = sndlen;
     *rcvbuf = target;
     cb_done->function = done;
     cb_done->clientdata = (void *) &snd_rcv_active;

     t_start = DCMF_Timebase();
     t_stop = DCMF_Timebase();

     while(((t_stop-t_start)/clockMHz)/1000000 < 1.0) t_stop = DCMF_Timebase();

     printf("[%d] Count = %d Start time : %lld End time %lld \n",
           myrank, *((unsigned *) clientdata), t_start, t_stop);
     fflush(stdout);

     return &snd_rcv_req;
}

/***************************************
*  Send Recv Noncontig Callback function  
****************************************/
DCMF_Request_t* snd_rcv_noncontig(void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
          size_t sndlen, size_t *rcvlen, char **rcvbuf, DCMF_Callback_t *cb_done) {

     snd_rcv_noncontig_buffer = (char *) malloc (sndlen);

     *rcvlen = sndlen;
     *rcvbuf = snd_rcv_noncontig_buffer;
     cb_done->function = done_noncontig;
     cb_done->clientdata = (void *) snd_rcv_noncontig_buffer;

     return &snd_rcv_noncontig_req;
}

/***************************************
*  Multicast Recv Callback function    *
****************************************/
DCMF_Request_t* mc_recv(const DCQuad *info, unsigned count, unsigned peer, unsigned sndlen, 
             unsigned conn_id, void *arg, unsigned *rcvlen, char **rcvbuf, 
             unsigned *pipewidth, DCMF_Callback_t *cb_done) {

     mc_rcv_buffer = (char *) malloc (sndlen);

     *rcvbuf = mc_rcv_buffer;
     *rcvlen = sndlen;
     *pipewidth = sndlen;

     cb_done->function = mc_done;
     cb_done->clientdata = (void *) &mc_rcv_active;

     return &mc_rcv_req;     
}

/***************************************
*  Multicast Done Callback function    *
****************************************/
void mc_done(void *clientdata, DCMF_Error_t *error) {
     --(*((unsigned *) clientdata));
}

/***************************************
*  Manytomany Recv Callback function    *
****************************************/
DCMF_Request_t* m2m_recv_callback(unsigned conn_id, void *arg, char **rcvbuf, unsigned **rcvlens,
             unsigned **rcvdispls, unsigned **rcvcounters, unsigned *nranks, 
             unsigned *rankIndex, DCMF_Callback_t *cb_done) {

     int i;

     printf("Inside manytomany recv callback \n");
     fflush(stdout);

     *rcvbuf = (char *) m2m_header->vaddress;
     *rcvlens = (unsigned *) malloc (sizeof(unsigned) * m2m_header->d1);
     *rcvdispls = (unsigned *) malloc (sizeof(unsigned) * m2m_header->d1);
     *rcvcounters = (unsigned *) malloc (sizeof(unsigned) * m2m_header->d1);
     cb_done = (DCMF_Callback_t *) malloc (sizeof(DCMF_Callback_t));
      
     for (i=0; i<m2m_header->d1; i++) {
        *rcvlens[i] = m2m_header->d2; 
        *rcvdispls[i] = i*m2m_header->stride;
     }  

     *nranks = 2;
     *rankIndex = 0;

     cb_done->function = done;
     cb_done->clientdata = (void *) &m2m_rcv_active;

     printf("Completed manytomany recv callback \n");
     fflush(stdout);

     return &m2m_rcv_req;
}

/***************************************
*  Configuring and registering Put     *
****************************************/
void put_init (DCMF_Put_Protocol protocol, DCMF_Network network) {
     DCMF_Result result;
     put_conf.protocol = protocol;
     put_conf.network = network;
     result = DCMF_Put_register(&put_reg, &put_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Put Registration failed with error %d \n", myrank, result);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering Get            *
**********************************************/

void get_init (DCMF_Get_Protocol protocol, DCMF_Network network) {
     DCMF_Get_Configuration_t get_conf;
     DCMF_Result result;

     get_conf.protocol = protocol;
     get_conf.network = network;

     result = DCMF_Get_register(&get_reg, &get_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Get Registration failed with error %d \n", myrank, result);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering Send            *
**********************************************/
void send_init(DCMF_Send_Protocol protocol, DCMF_Network network) {
     DCMF_Result result;
     snd_msginfo = (DCQuad *) malloc (sizeof(DCQuad));
 
     snd_conf.protocol = protocol;
     snd_conf.network = network;
     snd_conf.cb_recv_short = snd_rcv_short;
     snd_conf.cb_recv_short_clientdata = (void *) &snd_rcv_active;
     snd_conf.cb_recv = snd_rcv;
     snd_conf.cb_recv_clientdata = (void *) &snd_rcv_active;

     result = DCMF_Send_register(&snd_reg, &snd_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Send registration failed \n", myrank);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering Send            *
**********************************************/
void timed_send_init(DCMF_Send_Protocol protocol, DCMF_Network network) {

     DCMF_Result result;
     DCMF_Send_Configuration_t timed_snd_conf;
     snd_msginfo = (DCQuad *) malloc (sizeof(DCQuad));

     timed_snd_conf.protocol = protocol;
     timed_snd_conf.network = network;
     timed_snd_conf.cb_recv_short = timed_snd_rcv_short;
     timed_snd_conf.cb_recv_short_clientdata = (void *) &snd_rcv_active;
     timed_snd_conf.cb_recv = timed_snd_rcv;
     timed_snd_conf.cb_recv_clientdata = (void *) &snd_rcv_active;

     result = DCMF_Send_register(&timed_snd_reg, &timed_snd_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Send registration failed \n", myrank);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering Ack             *
**********************************************/
void ack_init() {
     DCMF_Result result;

     snd_conf.protocol = DCMF_EAGER_SEND_PROTOCOL;
     snd_conf.network = DCMF_TORUS_NETWORK;
     snd_conf.cb_recv_short = ack_rcv;
     snd_conf.cb_recv_short_clientdata = (void *) &ack_rcv_active;
     snd_conf.cb_recv = NULL;
     snd_conf.cb_recv_clientdata = NULL;

     result = DCMF_Send_register(&ack_reg, &snd_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Send registration failed \n", myrank);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering noncontig Send  *
**********************************************/
void send_noncontig_init(DCMF_Send_Protocol protocol, DCMF_Network network) {

     DCMF_Result result;
     snd_msginfo = (DCQuad *) malloc (sizeof(DCQuad));
   
     snd_conf.protocol = protocol;
     snd_conf.network = network; 
     snd_conf.cb_recv_short = snd_rcv_noncontig_short;
     snd_conf.cb_recv_short_clientdata = (void *) &snd_rcv_noncontig_active;
     snd_conf.cb_recv = snd_rcv_noncontig;
     snd_conf.cb_recv_clientdata = (void *) &snd_rcv_noncontig_active;

     result = DCMF_Send_register(&snd_noncontig_reg, &snd_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Send registration failed \n", myrank);
         fflush(stdout);
     }
}

/******************************************************
* Configuring and Registering manytomany header Send  *
*******************************************************/
void send_manytomany_init(DCMF_Send_Protocol protocol, DCMF_Network network) {

     DCMF_Result result;
     snd_msginfo = (DCQuad *) malloc (sizeof(DCQuad));

     snd_conf.protocol = protocol;
     snd_conf.network = network;
     snd_conf.cb_recv_short = snd_rcv_manytomany_short;
     snd_conf.cb_recv_short_clientdata = (void *) &snd_rcv_manytomany_active;
     snd_conf.cb_recv = NULL;
     snd_conf.cb_recv_clientdata = NULL;

     result = DCMF_Send_register(&snd_manytomany_reg, &snd_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Send registration failed \n", myrank);
         fflush(stdout);
     }
}

/**********************************************
* Configuring and Registering Multiacst       *
**********************************************/
void multicast_init(DCMF_Multicast_Protocol protocol, unsigned int size) {
     DCMF_Result result;
     connectionlist = (void **) malloc(sizeof(void*)*nranks);

     mc_conf.protocol = protocol;
     mc_conf.cb_recv = mc_recv;
     mc_conf.clientdata = NULL;
     mc_conf.connectionlist = connectionlist;
     mc_conf.nconnections = nranks;
     result = DCMF_Multicast_register(&mc_reg, &mc_conf);
     if(result != DCMF_SUCCESS) {
         printf("[%d] Multicast registration failed \n", myrank);
         fflush(stdout);
     }

     if(myrank == 0) {

       mc_ranks = (unsigned *) malloc(sizeof(unsigned int) * (nranks-1));
       mc_opcodes = (DCMF_Opcode_t *) malloc(sizeof(DCMF_Opcode_t) * (nranks-1)); 
       mc_snd_buffer = (char *) malloc(size);
       mc_req = (DCMF_Request_t *) malloc(sizeof(DCMF_Request_t));
       mc_msginfo = (DCQuad *) malloc (sizeof(DCQuad));

       mc_callback.function  = mc_done;
       mc_callback.clientdata = (void *) &mc_active;

       int i, idx=0;
       for(i=0; i<size; i++) 
            mc_snd_buffer[i] = 's'; 

       for(i=0; i<nranks; i++) {
         if(myrank != i) {
            mc_ranks[idx] = i;
            mc_opcodes[idx] = DCMF_PT_TO_PT_SEND;
            idx++;
         }
       }

       mc_info.registration  = &mc_reg;
       mc_info.request       = mc_req;
       mc_info.cb_done       = mc_callback;
       mc_info.consistency   = DCMF_SEQUENTIAL_CONSISTENCY;
       mc_info.connection_id = 0;
       mc_info.bytes         = size;
       mc_info.src           = mc_snd_buffer;
       mc_info.nranks        = nranks-1;
       mc_info.ranks         = mc_ranks;
       mc_info.opcodes       = mc_opcodes;
       mc_info.msginfo       = mc_msginfo;
       mc_info.count         = 1;
       mc_info.op            = DCMF_UNDEFINED_OP;
       mc_info.dt            = DCMF_UNDEFINED_DT;
       mc_info.flags         = 0;   
 
     }

     if(protocol == DCMF_DPUT_DMA_MSEND_PROTOCOL && myrank != 0) {

       mc_rcv_buffer = (char *) malloc (size); 

       mc_rcv_callback.function = done;
       mc_rcv_callback.clientdata = (void *) &mc_rcv_active;

       mc_rcv_info.registration = &mc_reg;
       mc_rcv_info.request = &mc_rcv_req;
       mc_rcv_info.cb_done = mc_rcv_callback; 
       mc_rcv_info.connection_id = 0;
       mc_rcv_info.bytes = size;
       mc_rcv_info.dst = mc_rcv_buffer;
       mc_rcv_info.pwidth = size;
       mc_rcv_info.op = DCMF_UNDEFINED_OP;
       mc_rcv_info.dt = DCMF_UNDEFINED_DT;

       result = DCMF_Multicast_postrecv (&mc_rcv_info);
       if(result != DCMF_SUCCESS) {
          printf("[%d] Receive pre-post failed \n", myrank);
          fflush(stdout);
       } else { 
          printf("[%d] Receive pre-post successful \n", myrank);
          fflush(stdout);  
       }
     }
}

/**********************************************
* Configuring and Registering ManytoMany ops  *
**********************************************/

void manytomany_init(DCMF_Manytomany_Protocol protocol) {

     DCMF_Result result;
 
     m2m_conf.protocol = protocol;
     m2m_conf.cb_recv = m2m_recv_callback;
     m2m_conf.arg = NULL; 
     m2m_conf.nconnections = 1;    
       
     result = DCMF_Manytomany_register(&m2m_reg, &m2m_conf); 
     if(result != DCMF_SUCCESS) {
        printf("[%d] Manytomany registration failed with status %d \n",
                 myrank, result);
        fflush(stdout);
      }     
}

/**********************************************
* Configuring and Registering Global Barrier *
**********************************************/
void barrier_init(DCMF_GlobalBarrier_Protocol protocol) {
  gb_conf.protocol = protocol;
  DCMF_Result result;  
  result = DCMF_GlobalBarrier_register(&gb_reg, &gb_conf);
  if(result != DCMF_SUCCESS) {
     printf("[%d] Global Barrier registration failed with status %d \n",
                 myrank, result);
     fflush(stdout);
  }

  gb_callback.function = done;
  gb_callback.clientdata = (void *) &gb_active;
}

/**********************************************
* Configuring and Registering Global Barrier *
**********************************************/
void allreduce_init(DCMF_GlobalAllreduce_Protocol protocol) {
  gar_conf.protocol = protocol;
  DCMF_Result result;
  result = DCMF_GlobalAllreduce_register(&gar_reg, &gar_conf);
  if(result != DCMF_SUCCESS) {
     printf("[%d] Global Allreduce registration failed with status %d \n",
                 myrank, result);
     fflush(stdout);
  }

  gar_callback.function = done;
  gar_callback.clientdata = (void *) &gar_active;
}

/**********************************************
* Configuring and Registering Control Protocol*
**********************************************/
void control_init(DCMF_Control_Protocol protocol, DCMF_Network network) {
  ctrl_conf.protocol = protocol;
  ctrl_conf.network = network;
  ctrl_conf.cb_recv = ctrl_recv;
  ctrl_conf.cb_recv_clientdata = (void *) &ctrl_active;
  DCMF_Result result;
  result = DCMF_Control_register(&ctrl_reg, &ctrl_conf);
  if(result != DCMF_SUCCESS) {
     printf("[%d] Control registration failed with status %d \n",
                 myrank, result);
     fflush(stdout);
  }
}

/**********************************************
* Creating memory region                      *
**********************************************/
void memregion_init(unsigned long long size) {
  window = (char *) malloc (size);
  memregion = (DCMF_Memregion_t **) malloc (sizeof(DCMF_Memregion_t*)*nranks);
  memregion[myrank] = (DCMF_Memregion_t *) malloc (sizeof(DCMF_Memregion_t));
  DCMF_Result result;
  size_t out;
  result = DCMF_Memregion_create (memregion[myrank], &out,
                 size, window, 0);
  if(result != DCMF_SUCCESS || out != size) {
     printf("[%d] Memory creation failed with status %d\
                 and size %d \n", myrank, result, out);
     fflush(stdout);
  }
  memregion_xchange();
}

/**********************************************
* Exchange memory region information          *
**********************************************/
void memregion_xchange() {
  int i;
  ctrl_active = nranks-1;

  for(i=0; i<nranks; i++) {
    if(i != myrank) {
      DCMF_Control(&ctrl_reg, DCMF_SEQUENTIAL_CONSISTENCY,
              i, (DCMF_Control_t *) memregion[myrank]);
    }
  }

  while(ctrl_active) DCMF_Messager_advance();
}

/**********************************************
* Exchange memory region information          *
**********************************************/
void address_xchange() {
  int i,count=0;
  DCMF_Request_t snd_req[nranks-1];
  DCMF_Callback_t snd_callback;
  unsigned int address;
  memcpy(&address, &vaddress[myrank], sizeof(unsigned int));
  snd_msginfo = (DCQuad *) malloc (sizeof(DCQuad));

  snd_callback.function = done;
  snd_callback.clientdata = (void *) &snd_active;

  snd_active = nranks-1;
  snd_rcv_active = nranks-1;
  for(i=0; i<nranks; i++) {
    if(i != myrank) { 
      DCMF_Send(&snd_reg, 
                &snd_req[count],
                snd_callback, 
                DCMF_SEQUENTIAL_CONSISTENCY,
                i, 
                sizeof(unsigned int),
                (char *) &address, 
                snd_msginfo, 
                1);
      count++;
    }
  }
  while(snd_active || snd_rcv_active) DCMF_Messager_advance();
}

/**********************************************
* Global Barrier                              *
**********************************************/
void barrier() {
  gb_active = 1;
  if(!gb_req) {
     gb_req = (DCMF_Request_t *) malloc(sizeof(DCMF_Request_t));
     DCMF_GlobalBarrier(&gb_reg, gb_req, gb_callback);
  } else {
     DCMF_Restart(gb_req);
  }
  while(gb_active) DCMF_Messager_advance();
}

/**********************************************
* Global Allreduce                            *
**********************************************/
void allreduce(int root, char *sbuffer, char *rbuffer, unsigned count, DCMF_Dt dt, DCMF_Op op) {
  gar_active = 1;
  gar_req = (DCMF_Request_t *) malloc(sizeof(DCMF_Request_t));
  DCMF_GlobalAllreduce(&gar_reg, gar_req, gar_callback, DCMF_SEQUENTIAL_CONSISTENCY, root,\
                  sbuffer, rbuffer, count, dt, op);
  while(gb_active) DCMF_Messager_advance();
}

/**********************************************
* Destrouing memory region                    *
**********************************************/
void memregion_finalize() {
  DCMF_Memregion_destroy(memregion[myrank]);
  free (memregion);
  free (window);
}
