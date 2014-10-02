#ifndef _AC_SignatureValue_INCLUDED_
#define _AC_SignatureValue_INCLUDED_

/* Kody bledow */
#include <bmd/libbmdasn1_core/BIT_STRING.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/pki/AA_pki.h>



#if defined(__cplusplus)
extern "C" {
#endif
	int AC_SignatureValue_create(ANY_t *DerInput, BIT_STRING_t *SignatureValue, ConfigData_t *ConfigData);
	int AC_SignatureValue_create_alloc(ANY_t *DerInput, BIT_STRING_t *SignatureValue, ConfigData_t *ConfigData);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_SignatureValue_INCLUDED_*/
