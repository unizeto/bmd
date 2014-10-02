#ifndef LIBBMDXADES_H_
#define LIBBMDXADES_H_

#include <libxml/tree.h>
#ifndef WIN32
	#include <xmlsec/xmlsec.h>
	#include <xmlsec/xmltree.h>
	#include <xmlsec/xmldsig.h>
#endif
#include <bmd/common/bmd-sds.h>

#ifdef WIN32
	#ifdef LIBBMDXADES_EXPORTS
		#define LIBBMDXADES_SCOPE __declspec(dllexport)
	#else
		#define LIBBMDXADES_SCOPE __declspec(dllimport)
	#endif
#else
	#define LIBBMDXADES_SCOPE extern
#endif 

typedef enum LIBBMDXADES_DIGEST_METHOD
{
	LIBBMDXADES_DIGEST_METHOD_SHA1,
	LIBBMDXADES_DIGEST_METHOD_MD5
} LIBBMDXADES_DIGEST_METHOD_t;

long xades_init(void);
long xades_destroy(void);

long xades_get_certificate_from_doc (xmlDocPtr doc, char ** cert, long * cert_len);
long xades_verify(const char *buffer, const long buffer_len, GenBuf_t *timestamp);
long verify_fields(const char *buffer, const long buffer_len, GenBuf_t *timestamp);
LIBBMDXADES_SCOPE long _getCertificateFromXML(xmlDocPtr *document, GenBuf_t ** certyfikat);
long _getInfoFromXML(const xmlDocPtr *document, char **FormSerialNumber, char **FormIssuerName);
long _compareSerials(char **cert, char **form);
long _compareIssuerNames(char **cert, char **form);
long _getInfoFromCertificate(GenBuf_t **certyfikat, char **CertSerialNumber, char **CertIssuerName);
long _getCertificateDigest(GenBuf_t **certifikat, LIBBMDXADES_DIGEST_METHOD_t method, char **basedHash);
long _getDigestAndMethod(const xmlDocPtr *document, char **hash, LIBBMDXADES_DIGEST_METHOD_t *metoda);
long isCertificateValid(GenBuf_t **certyfikat, GenBuf_t *timestamp);
time_t _to_seconds(const char *date);

#define XADES_CERT_TAG BAD_CAST "X509Certificate"

#endif /*LIBBMDXADES_H_*/
