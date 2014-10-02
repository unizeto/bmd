#ifndef _LIBBMDPKCS11_INCLUDED_
#define _LIBBMDPKCS11_INCLUDED_

#include <bmd/common/bmd-common.h>
#include <bmd/common/bmd-crypto_ds.h>
#ifdef WIN32
#include<pkcs11/cryptoki.h>
#else
#define CK_PTR *
#define CK_DEFINE_FUNCTION(returnType, name) returnType name
#define CK_DECLARE_FUNCTION(returnType, name) returnType name
#define CK_DECLARE_FUNCTION_POINTER(returnType, name) returnType (* name)
#define CK_CALLBACK_FUNCTION(returnType, name) returnType (* name)
#ifndef NULL_PTR
#define NULL_PTR 0
#endif

#endif

#ifdef WIN32
	#ifdef LIBBMDPKCS11_EXPORTS
		#define LIBBMDPKCS11_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDPKCS11_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDPKCS11_SCOPE
		#endif
	#endif
#else
	#define LIBBMDPKCS11_SCOPE
#endif				// ifdef _WIN32

#if defined(__cplusplus)
extern "C" {
#endif
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_finalize(pkcs11_crypto_info_t *pkcs11Info);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_init(const char *pkcs11_library,pkcs11_crypto_info_t *info);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_get_all_certs(pkcs11_crypto_info_t *info,pkcs11_cert_t ***cert_list);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_get_privkey(pkcs11_cert_t *cert_info,char **pin,long pin_length,pkcs11_crypto_info_t *info);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_remember_pin(pkcs11_crypto_info_t *pkcs11Info, char *pin, long pinLen);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_clear_pin(pkcs11_crypto_info_t *pkcs11Info);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_sign(pkcs11_crypto_info_t *info,GenBuf_t *input,GenBuf_t **output);
	LIBBMDPKCS11_SCOPE long bmd_pkcs11_decrypt(pkcs11_crypto_info_t *info,GenBuf_t *input,GenBuf_t **output);
#if defined(__cplusplus)
}
#endif
#endif
