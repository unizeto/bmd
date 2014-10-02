#ifndef _PR_Kontrolki_included_
#define _PR_Kontrolki_included_

#include <bmd/libbmdpr/prlib-common/PR_lib.h>

#if defined(__cplusplus)
extern "C" {
#endif

    long bmd_kontrolki_init(Kontrolki_t **kontrolki);
    long bmd_kontrolki_create(Kontrolka_t *kontrolka,Kontrolki_t **kontrolki);
    long bmd_kontrolki_der_encode(Kontrolki_t *kontrolki,GenBuf_t **output);
    long bmd_kontrolki_der_decode(GenBuf_t *input,Kontrolki_t **kontrolki);
    long bmd_kontrolki_free(Kontrolki_t **kontrolki);

#if defined(__cplusplus)
}
#endif
#endif
