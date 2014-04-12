
#include <assert.h>

int ARMCIX_Mode_set(int mode, void *ptr, ARMCI_Group *group) {
  assert (0);
  return 0;
}

int ARMCIX_Mode_get(void *ptr) {
  assert (0);
  return 0;
}

int ARMCIX_Group_split(ARMCI_Group *parent, int color, int key, ARMCI_Group *new_group) 
{
  assert (0);
  return 0;
}

int ARMCIX_Group_dup(ARMCI_Group *parent, ARMCI_Group *new_group)
{
  assert (0);
}

armcix_mutex_hdl_t ARMCIX_Create_mutexes_hdl(int count, ARMCI_Group *pgroup)
{
  assert (0);
  return 0;  
}

int  ARMCIX_Destroy_mutexes_hdl(armcix_mutex_hdl_t hdl)
{
  assert (0);
  return 0;
}

void ARMCIX_Lock_hdl(armcix_mutex_hdl_t hdl, int mutex, int proc)
{
  assert (0);
  return;
}

int  ARMCIX_Trylock_hdl(armcix_mutex_hdl_t hdl, int mutex, int proc)
{
  assert (0);
  return;
}
void ARMCIX_Unlock_hdl(armcix_mutex_hdl_t hdl, int mutex, int proc)
{
  assert (0);
}
