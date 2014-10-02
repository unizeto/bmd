#ifndef _AC_pki_INCLUDED_
#define _AC_pki_INCLUDED_

/* Kody bledow */
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
/* Konfiguracja serwera */
#include <bmd/libbmdaa2/configuration/AA_config.h>

#include<openssl/evp.h>
#include<openssl/pem.h>
#include<openssl/err.h>
#include<openssl/rand.h>

#define SIGNATURE_INPUT_READ_BUF_SIZE	128

#if defined(__cplusplus)
extern "C" {
#endif
	/************************
	* Funkcje OpenSSL	*
	***********************/
	long ReadEvpPkeyFromPEM_private(char *keyfilename, char *password, EVP_PKEY **key);
	long SignBuffer(ANY_t *AnyBuf, ACSignatureEnum_t SignatureAlgorithm, 
				char *keyfilename, char *password, GenBuf_t **signaturebuf);
	void print_openssl_errors(void);

#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_pki_INCLUDED_*/
