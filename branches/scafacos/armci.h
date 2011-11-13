int ARMCI_Init();
void ARMCI_Finalize();
void ARMCI_Cleanup();
void ARMCI_Error(char *msg, int code);

int ARMCI_Malloc(void * ptr_arr[], int bytes);
int ARMCI_Free(void * ptr);

void ARMCI_Barrier();

void ARMCI_Fence(int proc);
void ARMCI_AllFence();

int ARMCI_Put(void *src, void *dst, int bytes, int proc);
