#include "dcmf.h"
#include "dcmf_globalcollectives.h"
#include "dcmf_multisend.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define ITERATIONS 100
#define SKIP 10
#define MAX_MSG_SIZE 1024*1024*1
#define VECTOR 16 
#define MAX_BUF_SIZE MAX_MSG_SIZE*(ITERATIONS+SKIP)

#define MAX_DIM 1024
#define ITERS 100

/***************************************
*  Header for noncontig transfers      *
***************************************/

struct noncontig_header {
   void *vaddress;
   int dist;
   int stride;
   int d1;
   int d2;
   char flag;
};

/***************************************
*  Global Helper variables             *
***************************************/
int nranks, myrank;
unsigned long long t_start, t_stop;
double t_sec, t_usec, t_msec;
double t_max, t_avg, t_min;
double clockMHz; 
DCMF_Hardware_t hw;
double clockMHz;

/***************************************
*  Global DCMF structures for Window   *
***************************************/
DCMF_Memregion_t **memregion;
void **vaddress;
char *window;

/**********************************************
* Global DCMF structures for Put              *
**********************************************/
DCMF_Put_Configuration_t put_conf;
DCMF_Protocol_t put_reg;
DCMF_Callback_t put_done, put_ack;
int put_count, ack_count;

/**********************************************
* Global DCMF structures for Send      *
**********************************************/
DCMF_Send_Configuration_t snd_conf;
DCMF_Callback_t snd_callback;
DCMF_Protocol_t snd_reg, snd_noncontig_reg;
DCMF_Request_t snd_rcv_req, snd_rcv_noncontig_req;
int snd_rcv_active, snd_active, snd_rcv_noncontig_active;
char *snd_rcv_buffer, *snd_rcv_noncontig_buffer;
DCQuad *snd_msginfo;

/**********************************************
* Global DCMF structures for Multisend      *
**********************************************/

DCMF_Multicast_Configuration_t mc_conf;
DCMF_Multicast_t mc_info;
DCMF_MulticastRecv_t mc_rcv_info;
DCMF_Protocol_t mc_reg;
DCMF_Request_t *mc_req, mc_rcv_req;
DCMF_Callback_t mc_callback, mc_rcv_callback;
DCMF_Opcode_t *mc_opcodes;
unsigned int *mc_ranks;
int mc_active, mc_rcv_active;
void **connectionlist;
char *mc_rcv_buffer, *mc_snd_buffer;
DCQuad *mc_msginfo;

/***************************************
*  Global DCMF structures for Barrier  *
****************************************/
DCMF_GlobalBarrier_Configuration_t gb_conf;
DCMF_Protocol_t gb_reg;
DCMF_Request_t *gb_req;
DCMF_Callback_t gb_callback;
int gb_active;

/***************************************
*  Global DCMF structures for Allreduce  *
****************************************/
DCMF_GlobalAllreduce_Configuration_t gar_conf;
DCMF_Protocol_t gar_reg;
DCMF_Request_t *gar_req;
DCMF_Callback_t gar_callback;
int gar_active;

/***************************************
*  Global DCMF structures for Control  *
****************************************/
DCMF_Control_Configuration_t ctrl_conf;
DCMF_Protocol_t ctrl_reg;
DCMF_Request_t ctrl_req;
DCMF_Callback_t ctrl_callback;
int ctrl_active;

/***************************************
*  Generic init                        *
****************************************/
void init();

/***************************************
*  Generic Callback function           *
****************************************/
void done(void *, DCMF_Error_t *); 

/***************************************
*  Control Callback function           *
****************************************/
void ctrl_recv(void *, const DCMF_Control_t *, size_t); 

/***************************************
*  Multicast Recv Callback function         *
****************************************/
DCMF_Request_t* mc_recv(const DCQuad *, unsigned, unsigned, unsigned,\
             unsigned , void *, unsigned *, char **,\
             unsigned *, DCMF_Callback_t *);

/***************************************
*  Multicast Done Callback function    *
****************************************/
void mc_done(void *, DCMF_Error_t *); 

/***************************************
*  Configuring and registering Put     *
****************************************/
void put_init (DCMF_Put_Protocol, DCMF_Network);

/****************************************
* Configuring and Registering Send *
*****************************************/
void send_init(DCMF_Send_Protocol, DCMF_Network);

/****************************************
* Configuring and Registering Send Noncontig *
*****************************************/
void send_noncontig_init(DCMF_Send_Protocol, DCMF_Network);

/****************************************
* Configuring and Registering Multicast *
*****************************************/
void multicast_init(DCMF_Multicast_Protocol, unsigned int size);

/*********************************************
* Configuring and Registering Global Barrier *
**********************************************/
void barrier_init(DCMF_GlobalBarrier_Protocol); 

/*********************************************
* Configuring and Registering Global Allreduce*
**********************************************/
void allreduce_init(DCMF_GlobalAllreduce_Protocol);

/**********************************************
* Configuring and Registering Control Protocol*
**********************************************/
void control_init(DCMF_Control_Protocol, DCMF_Network);

/**********************************************
* Creating memory region                      *
**********************************************/
void memregion_init(unsigned long long);

/**********************************************
* Exchange memory region information          *
**********************************************/
void memregion_xchange();

/**********************************************
* Exchange memory region information          *
**********************************************/
void address_xchange();

/**********************************************
* Global Barrier                              *
**********************************************/
void barrier();

/**********************************************
* Global Allreduce                            *
**********************************************/
void allreduce(int, char *, char *, unsigned, DCMF_Dt, DCMF_Op);

/**********************************************
* Destroy memory region                       *
**********************************************/
void memregion_finalize();

 
