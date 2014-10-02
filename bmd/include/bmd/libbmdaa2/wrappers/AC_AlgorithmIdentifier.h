#ifndef _AC_AlgorithmIdentifier_INCLUDED_
#define _AC_AlgorithmIdentifier_INCLUDED_

#include <bmd/libbmdasn1_common/AlgorithmIdentifier.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_AlgorithmIdentifier_set_AlgorithmOid(OBJECT_IDENTIFIER_t *Oid, 
					AlgorithmIdentifier_t *AlgorithmIdentifier);
	int AC_AlgorithmIdentifier_set_AlgorithmParameters(ANY_t *Parameters, 
					AlgorithmIdentifier_t *AlgorithmIdentifier);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_AlgorithmIdentifier_INCLUDED_*/
