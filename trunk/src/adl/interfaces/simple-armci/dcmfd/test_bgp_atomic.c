#include <stdio.h>
#include <bpcore/ppc450_inlines.h>

int main()
{
    uint32_t data = 1000;
    uint32_t * clientdata = &data;
    uint32_t * atomic = (uint32_t *) clientdata;
    register uint32_t temp;

    int i;
    for (i=0;i<100;i++)
    {
        _bgp_msync();
        do {
            temp = _bgp_LoadReserved( atomic );
            --temp;
        } while( !_bgp_StoreConditional( atomic, temp ) );
    }

    printf(" data        = %u \n", data );
    printf(" &data       = %p \n", &data );
    printf(" *clientdata = %u \n", (*clientdata) );
    printf(" clientdata  = %p \n", clientdata );
    printf(" *atomic     = %u \n", (*atomic) );
    printf(" atomic      = %p \n", atomic );

    return 0;
}
