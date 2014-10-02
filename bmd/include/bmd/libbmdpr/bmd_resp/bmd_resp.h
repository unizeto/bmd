#ifndef _bmd_resp_included_
#define _bmd_resp_included_

#include <stdlib.h>
#include <string.h>

#include <bmd/libbmdpr/prlib-common/PR_lib.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>

#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolka.h>
#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolki.h>

#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdasn1_common/bmdgroup_utils.h>

#if defined(__cplusplus)
extern "C" {
#endif
    LIBBMDPR_SCOPE long bmd_kontrolki_serialize(BMD_attr_t **attrs,GenBuf_t **output);
    LIBBMDPR_SCOPE long bmd_kontrolki_deserialize(GenBuf_t *input,BMD_attr_t ***attrs);
#if defined(__cplusplus)
}
#endif
#endif
