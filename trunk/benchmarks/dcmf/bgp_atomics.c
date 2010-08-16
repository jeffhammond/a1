
#include <assert.h>
#include <bpcore/bgp_atomic_ops.h>

_BGP_Atomic global_atomic;
#define A1DI_GLOBAL_ATOMIC_ACQUIRE() while(!_bgp_test_and_set(&global_atomic, 1))
#define A1DI_GLOBAL_ATOMIC_RELEASE() do{ global_atomic.atom = 0; }while(0)

int main()
{
    printf("before A1DI_GLOBAL_ATOMIC_ACQUIRE\n");
    A1DI_GLOBAL_ATOMIC_ACQUIRE();
    printf("after A1DI_GLOBAL_ATOMIC_ACQUIRE\n");
    printf("pretending to do something which requires thread serialization\n");
    printf("before A1DI_GLOBAL_ATOMIC_RELEASE\n");
    A1DI_GLOBAL_ATOMIC_RELEASE();
    printf("after A1DI_GLOBAL_ATOMIC_RELEASE\n");

    return(0);
}
