#ifndef _AC_RDNSequence_INCLUDED_
#define _AC_RDNSequence_INCLUDED_

#include <bmd/libbmdasn1_common/RDNSequence.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_RDNSequenceSetAddValue(DNValueType_t DNValueType, const char *DNValue, 
					RDNSequence_t *RDNSequence);
	int AC_RDNSequence_clone_pointers(RDNSequence_t *src, RDNSequence_t *dest);	
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_RDNSequence_INCLUDED_*/

