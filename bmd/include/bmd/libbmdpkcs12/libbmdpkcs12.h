#ifndef _LIBBMDPKCS12_INCLUDED_
#define _LIBBMDPKCS12_INCLUDED_

#include <bmd/common/bmd-common.h>
#include <bmd/common/bmd-openssl.h>

#ifdef WIN32
	#ifdef LIBBMDPKCS12_EXPORTS
		#define LIBBMDPKCS12_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDPKCS12_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDPKCS12_SCOPE
		#endif
	#endif
#else
	#define LIBBMDPKCS12_SCOPE
#endif // ifdef WIN32



#if defined(__cplusplus)
extern "C" {
#endif
LIBBMDPKCS12_SCOPE long bmd_parse_p12_file(char *filename,char *pass,long passlen,RSA **publicKey,RSA **privateKey,GenBuf_t **cert);
LIBBMDPKCS12_SCOPE long bmd_check_p12_pass(GenBuf_t *p12_buf,long *flag);
long _bmd_binary_content_to_BIO(GenBuf_t *input,BIO *out_bio);

LIBBMDPKCS12_SCOPE long bmd_parse_p12_fileInMem(GenBuf_t *file_buf,char *pass,long passlen,RSA **publicKey,RSA **privateKey,GenBuf_t **cert);
#if defined(__cplusplus)
}
#endif
#endif
