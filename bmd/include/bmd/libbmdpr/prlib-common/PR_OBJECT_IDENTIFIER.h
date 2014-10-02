#ifndef _PR_OBJECT_IDENTIFIER_included_
#define _PR_OBJECT_IDENTIFIER_included_

#include <bmd/libbmdpr/prlib-common/PR_lib.h>

#if defined(__cplusplus)
extern "C" {
#endif

LIBBMDPR_SCOPE long PR_OBJECT_IDENTIFIER_cmp(void *oid1, int sizeoid1, void *oid2, int sizeoid2);

#if defined(__cplusplus)
}
#endif
#endif
