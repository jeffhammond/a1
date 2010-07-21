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

#define MAXNODES   8192
#define MAXMSGSIZE 1024

int    send_active[2];
int    recv_active[2];

DCMF_Request_t send_req[2] __attribute ((__aligned__(16)));
DCMF_Request_t recv_req[2] __attribute ((__aligned__(16)));

char recv_buf[MAXMSGSIZE*26+1] __attribute ((__aligned__(16)));
unsigned recv_len[27];
unsigned recv_displ[27];
unsigned recv_counter[2][27];


char send_buf[MAXMSGSIZE*26] __attribute ((__aligned__(16)));
unsigned send_ranks[26];
unsigned send_len[26];
unsigned send_displ[26];
unsigned send_idx[26];
unsigned send_counter[2][26];
unsigned permutation[26];

unsigned  rank, size;

void    cb_Manytomany_senddone (void * clientdata, DCMF_Error_t *error)
{
  send_active[(unsigned)clientdata]--;
}

void   cb_Manytomany_recvdone (void * clientdata, DCMF_Error_t *error)
{
  recv_active[(unsigned)clientdata]--;
}

DCMF_Request_t* cb_recv_Manytomany (unsigned              connid,
				   void                 * arg,
				   char                ** recvbuf,
				   unsigned            ** recvlens,
				   unsigned            ** recvdispls,
				   unsigned            ** recvcounters,
				   unsigned             * numranks,
				   unsigned             * ridx,
				   DCMF_Callback_t* const cb_info)
{
  * recvbuf             =  (char *) recv_buf;
  * recvlens            =  recv_len;
  * recvdispls          =  recv_displ;
  * recvcounters        =  recv_counter[connid];
  
  *numranks             =  27;
  *ridx                 =  26;

  cb_info->function   = cb_Manytomany_recvdone;
  cb_info->clientdata = (void *) connid;

  return &recv_req[connid];
}

int main()
{
  unsigned sendlen;
  unsigned i,j,k,idx;
  DCMF_Protocol_t  protocol;
  DCMF_Manytomany_Configuration_t mconfig;
  DCMF_Callback_t cb_done[2];
  DCMF_NetworkCoord_t myaddr, dstaddr; 
  DCMF_Network ntwk, temp;

  DCMF_Messager_initialize();

  init();
    
  barrier_init(DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL);

  if(nranks != 512) {
     if(myrank == 0) {
        printf("This tests requires and 8x8x8 torus \n");
        fflush(stdout);
     }
     return -1; 
  }

  mconfig.protocol       = DCMF_MEMFIFO_DMA_M2M_PROTOCOL;
  mconfig.cb_recv        = cb_recv_Manytomany;
  mconfig.arg            = NULL;
  mconfig.nconnections   = 2;

  DCMF_Manytomany_register (&protocol, &mconfig);

  cb_done[0].function = cb_Manytomany_senddone;
  cb_done[0].clientdata = (void *) 0;
  cb_done[1].function = cb_Manytomany_senddone;
  cb_done[1].clientdata = (void *) 1;

  for (i=0; i<MAXMSGSIZE; i++) {
      send_buf[i] = '*';
  }

  DCMF_Messager_rank2network(myrank, DCMF_TORUS_NETWORK, &myaddr);
  dstaddr.network = myaddr.network;
  dstaddr.torus.t = myaddr.torus.t;

  idx=0;
  for(i=-1; i<=1; i++) {
    for(j=-1; j<=1; j++) {
      for(k=-1; k<=1; k++) {
	  dstaddr.torus.x = (myaddr.torus.x+i+8)%8;
	  dstaddr.torus.y = (myaddr.torus.y+j+8)%8;
	  dstaddr.torus.z = (myaddr.torus.z+k+8)%8;  
          DCMF_Messager_network2rank(&dstaddr, &temp, &ntwk); 
          if(temp != myrank) {
               send_ranks[idx] = myrank;
               send_idx[idx] = (-i + 1) + (-j + 1)*3 + (-k + 1)*9;
               idx++;
          }
      }
    }
  }

  if(idx != 26) {
    if(rank == 0) {
        printf("rank calculation is erroneous \n");
        fflush(stdout);
    }
    return -1; 
  }

  for(i=0; i<26; i++)  
    permutation[i] = i;

  recv_len[26] = 1;
  recv_displ[26] = MAXMSGSIZE*26;

  for (sendlen=1; sendlen<=MAXMSGSIZE; sendlen*=2) {

  for(i=0; i<26; i++) {
    send_len[i] = sendlen;
    send_displ[i] = i*sendlen;
    recv_len[i] = sendlen;
    recv_displ[i] = i*sendlen;
  }  
  
  for (i=0; i<MAXMSGSIZE*26; i++) {
      recv_buf[i] = '_';   
  }
  
  send_active[0] = 1;
  recv_active[0] = 1;

  DCMF_Manytomany (&protocol, &send_req[0], cb_done[0],
		   DCMF_MATCH_CONSISTENCY, 0, 1, 0, send_idx, (char *) send_buf, 
		   send_len, send_displ, send_counter[0], 
		   send_ranks, permutation, 26);
  
  while (send_active[0] > 0 || recv_active[0] > 0) DCMF_Messager_advance();

  printf("[%d] After manytomany 1 \n", myrank);
  fflush(stdout);

  barrier();

  send_active[1] = 1;
  recv_active[1] = 1;

  DCMF_Manytomany (&protocol, &send_req[1], cb_done[1],
                   DCMF_MATCH_CONSISTENCY, 1, 1, 0, send_idx, (char *) send_buf,     
                   send_len, send_displ, send_counter[1],
                   send_ranks, permutation, 26);

  while (send_active[1] > 0 || recv_active[1] > 0) DCMF_Messager_advance();

  printf("[%d] After manytomany 2 \n", myrank);
  fflush(stdout);

  barrier();

  int niter = 2;
  unsigned long long start, time;  

  start = DCMF_Timebase();
  for (i = 0; i < niter; i++) {    
    int conn = i % 2;
    send_active[conn] = 1;
    recv_active[conn] = 1;

    DCMF_Manytomany (&protocol, &send_req[conn], cb_done[conn],
                   DCMF_MATCH_CONSISTENCY, conn, 1, 0, send_idx, (char *) send_buf,     
                   send_len, send_displ, send_counter[conn],
                   send_ranks, permutation, 26);

    while (send_active[conn] > 0 || recv_active[conn] > 0) DCMF_Messager_advance();
  }

  time = DCMF_Timebase() - start;
  if(myrank == 0) {
    printf ("Time For Many-to-many with size %d = %d cycles, buffer = %s \n", sendlen, (unsigned)(time / niter), recv_buf);
    fflush(stdout);
  }

  }

  return 0;
}
