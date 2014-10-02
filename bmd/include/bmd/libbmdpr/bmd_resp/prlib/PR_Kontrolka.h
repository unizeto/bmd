#ifndef _PR_Kontrolka_included_
#define _PR_Kontrolka_included_

#include <bmd/libbmdpr/prlib-common/PR_lib.h>

#if defined(__cplusplus)
extern "C" {
#endif
   
    int bmd_kontrolka_init(Kontrolka_t **kontrolka);
    int bmd_kontrolka_create(BMD_attr_t *attr,Kontrolka_t **kontrolka);
    int bmd_kontrolka_der_encode(Kontrolka_t *kontrolka,GenBuf_t **output);
    int bmd_kontrolka_free(Kontrolka_t **kontrolka);
    
#if defined(__cplusplus)
}
#endif
#endif
