/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined ARMCI_H_INCLUDED
#define ARMCI_H_INCLUDED

/*************************
      Data Structures
*************************/

typedef enum 
{
   ARMCI_ACC_INT,
   ARMCI_ACC_FLT,
   ARMCI_ACC_DBL,
   ARMCI_ACC_CPL,
   ARMCI_ACC_DCPL
} armci_datatype_t;

typedef enum
{
   ARMCI_FETCH_AND_ADD,
   ARMCI_FETCH_AND_ADD_LONG,
   ARMCI_SWAP,
   ARMCI_SWAP_LONG
} armci_rmw_op_t;

typedef struct {
   void **src_ptr_ar; 
   void **dst_ptr_ar; 
   int bytes;         
   int ptr_ar_len;    
} armci_giov_t;

typedef int armci_size_t;

typedef void* armci_hdl_t;

/*************************
      Macros
*************************/

#define ARMCI_SET_AGGREGATE_HANDLE(handle) { /*do nothing*/ }

#define ARMCI_UNSET_AGGREGATE_HANDLE(handle) { /*do nothing*/ }

/*************************
      Functions
*************************/

void ARMCI_INIT_HANDLE(armci_hdl_t* handle);

int ARMCI_Init_args(int *argc, char ***argv);

int ARMCI_Init();

int ARMCI_Finalize();

void ARMCI_Error(char *message, 
                 int code);

int ARMCI_Malloc(void* ptr[], 
                 armci_size_t bytes);

void* ARMCI_Malloc_local(armci_size_t bytes);

int ARMCI_Free(void *address);

int ARMCI_Free_local(void *address);

int ARMCI_Put(void* src,  
              void* dst, 
              int bytes, 
              int proc);

int ARMCI_PutS(void* src_ptr, 
               int src_stride_ar[], 
               void* dst_ptr, 
               int dst_stride_ar[], 
               int count[], 
               int stride_levels, 
               int proc);

int ARMCI_NbPutS(void* src_ptr, 
                 int src_stride_ar[], 
                 void* dst_ptr, 
                 int dst_stride_ar[], 
                 int count[], 
                 int stride_levels, 
                 int proc,
                 armci_hdl_t* handle);

int ARMCI_PutV(armci_giov_t *dsrc_arr, 
               int arr_len, 
               int proc);

int ARMCI_Get(void *src, 
              void *dst, 
              int bytes,
              int proc);

int ARMCI_GetS(void *src_ptr, 
               int src_stride_ar[], 
               void* dst_ptr,
               int dst_stride_ar[], 
               int count[], 
               int stride_levels, 
               int proc);

int ARMCI_NbGetS(void *src_ptr, 
                 int src_stride_ar[], 
                 void* dst_ptr, 
                 int dst_stride_ar[], 
                 int count[], 
                 int stride_levels, 
                 int proc, 
                 armci_hdl_t* handle);

int ARMCI_GetV(armci_giov_t *dsrc_arr,
               int arr_len,
               int proc);

int ARMCI_Acc(int datatype, 
              void *scale, 
              void* src, 
              void* dst, 
              int bytes, 
              int proc);

int ARMCI_AccS(int datatype, 
               void *scale, 
               void* src_ptr,
               int src_stride_ar[], 
               void* dst_ptr, 
               int dst_stride_ar[], 
               int count[],  
               int stride_levels, 
               int proc);

int ARMCI_NbAccS(int datatype,  
                 void *scale,
                 void* src_ptr,
                 int src_stride_ar[],
                 void* dst_ptr,
                 int dst_stride_ar[],
                 int count[],
                 int stride_levels,
                 int proc,
                 armci_hdl_t* handle);

int ARMCI_AccV(int datatype, 
               void *scale, 
               armci_giov_t *dsrc_arr, 
               int arr_len, 
               int proc);

int ARMCI_Rmw(int op, 
              void *ploc, 
              void *prem, 
              int value, 
              int proc);

int ARMCI_Wait(armci_hdl_t* handle);

int ARMCI_WaitAll();

int ARMCI_Test(armci_hdl_t* handle);

void ARMCI_Fence(int proc);

void ARMCI_AllFence();

int ARMCI_Barrier();

int ARMCI_Create_mutexes(int num);

int ARMCI_Destroy_mutexes(void);

void ARMCI_Lock(int mutex, int proc);

void ARMCI_Unlock(int mutex, int proc);


/*************************************
       Prototypes for Unused Functions
**************************************/

/**********Setup functions**********/

void ARMCI_Cleanup(void);

/**********Communication************/

void armci_read_strided(void *ptr, int stride_levels,
                           int stride_arr[], int count[], char *buf);

void armci_write_strided(void *ptr, int stride_levels,
                           int stride_arr[], int count[], char *buf);

int ARMCI_PutS_flag_dir(       /* put with flag that uses direct put */
                void *src_ptr,        /* pointer to 1st segment at source*/
                int src_stride_arr[], /* array of strides at source */
                void* dst_ptr,        /* pointer to 1st segment at destination*/
                int dst_stride_arr[], /* array of strides at destination */
                int count[],          /* number of segments at each stride
                                         levels: count[0]=bytes*/
                int stride_levels,    /* number of stride levels */
                int *flag,            /* pointer to remote flag */
                int val,              /* value to set flag upon completion of
                                         data transfer */
                int proc              /* remote process(or) ID */
                );

int ARMCI_PutS_flag(
                void *src_ptr,        /* pointer to 1st segment at source*/
                int src_stride_arr[], /* array of strides at source */
                void* dst_ptr,        /* pointer to 1st segment at destination*/
                int dst_stride_arr[], /* array of strides at destination */
                int count[],          /* number of segments at each stride
                                         levels: count[0]=bytes*/
                int stride_levels,    /* number of stride levels */
                int *flag,            /* pointer to remote flag */
                int val,              /* value to set flag upon completion of
                                         data transfer */
                int proc              /* remote process(or) ID */
                );

void ARMCI_Copy(void *src, void *dst, int n);

/************ Locality Functions **********************************/
typedef int armci_domain_t;
#define ARMCI_DOMAIN_SMP 0        
int armci_domain_nprocs(armci_domain_t domain, int id);
int armci_domain_id(armci_domain_t domain, int glob_proc_id);
int armci_domain_glob_proc_id(armci_domain_t domain, int id, int loc_proc_id);
int armci_domain_my_id(armci_domain_t domain);
int armci_domain_count(armci_domain_t domain);
int armci_domain_same_id(armci_domain_t domain, int proc);

/*********** Group Functions ************************************/
typedef struct {
} ARMCI_Group;

typedef struct {
} armci_clus_t;

typedef struct {
}armci_grp_attr_t;

void ARMCI_Group_create(int n, int *pid_list, ARMCI_Group *group_out);
void ARMCI_Group_create_child(int n, int *pid_list, ARMCI_Group *group_out,
                              ARMCI_Group *group_parent);
void ARMCI_Group_free(ARMCI_Group *group);
int  ARMCI_Group_rank(ARMCI_Group *group, int *rank);
void ARMCI_Group_size(ARMCI_Group *group, int *size);
void ARMCI_Group_set_default(ARMCI_Group *group);
void ARMCI_Group_get_default(ARMCI_Group *group_out);
void ARMCI_Group_get_world(ARMCI_Group *group_out);
ARMCI_Group * ARMCI_Get_ft_group();

int ARMCI_Malloc_group(void *ptr_arr[], armci_size_t bytes,ARMCI_Group *group);
int ARMCI_Free_group(void *ptr, ARMCI_Group *group);

armci_grp_attr_t *ARMCI_Group_getattr(ARMCI_Group *grp);

/*********** Group Functions based on MPI ****************************/

void armci_msg_gop_init();
int  armci_msg_nproc();
int  armci_msg_me();

void armci_msg_gop_scope(int scope, void *x, int n, char* op, int type);
void armci_msg_sel_scope(int scope, void *x, int n, char* op, int type, int contribute);
void armci_msg_llgop(long long *x, int n, char* op);
void armci_msg_bcast(void* buffer, int len, int root);
void armci_msg_barrier();
void armci_msg_dgop(double *x, int n, char* op);
void armci_msg_fgop(float *x, int n, char* op);
void armci_msg_igop(int *x, int n, char* op);
void armci_msg_lgop(long *x, int n, char* op);
void armci_msg_bintree(int scope, int* Root, int *Up, int *Left, int *Right);
void armci_exchange_address(void *ptr_ar[], int n);

void armci_msg_group_igop(int *x, int n, char* op,ARMCI_Group *group);
void armci_msg_group_lgop(long *x, int n, char* op,ARMCI_Group *group);
void armci_msg_group_llgop(long long *x, int n, char* op,ARMCI_Group *group);
void armci_msg_group_fgop(float *x, int n, char* op,ARMCI_Group *group);
void armci_msg_group_dgop(double *x, int n,char* op,ARMCI_Group *group);
void armci_msg_group_bcast_scope(int scope, void *buf, int len,
                                        int root, ARMCI_Group *group);
void armci_msg_group_barrier(ARMCI_Group *group);
void armci_msg_group_gop_scope(int scope, void *x, int n, char* op,
                                      int type, ARMCI_Group *group);
void armci_grp_clus_brdcst(void *buf, int len, int grp_master,
                                  int grp_clus_nproc,ARMCI_Group *mastergroup);
void armci_exchange_address_grp(void *ptr_arr[], int n, ARMCI_Group *group);

void armci_msg_bcast_scope(int scope, void* buffer, int len, int root);
void armci_msg_brdcst(void* buffer, int len, int root);
void armci_msg_snd(int tag, void* buffer, int len, int to);
void armci_msg_rcv(int tag, void* buffer, int buflen, int *msglen, int from);
int  armci_msg_rcvany(int tag, void* buffer, int buflen, int *msglen);


/**********Checkpointing************/

typedef struct {
}armci_ckpt_ds_t;

void ARMCI_Ckpt_create_ds(armci_ckpt_ds_t *ckptds, int count);
int ARMCI_Ckpt_init(char *filename, ARMCI_Group *grp, int savestack, int saveheap, armci_ckpt_ds_t *ckptds);
int ARMCI_Ckpt(int rid);
void ARMCI_Ckpt_finalize(int rid);

/************* Memory *******************************************/
int ARMCI_Same_node(int proc);

int ARMCI_Uses_shm();
void ARMCI_Set_shm_limit(unsigned long shmemlimit);

int ARMCI_Uses_shm_grp(ARMCI_Group *group);

int ARMCI_Absolute_id(ARMCI_Group *group,int group_rank);

#endif /* ARMCI_H_INCLUDED */
