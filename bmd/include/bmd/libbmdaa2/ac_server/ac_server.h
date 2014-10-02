#ifndef _AC_server_INCLUDED_
#define _AC_server_INCLUDED_

#include<time.h>
#ifndef _WIN32
#include<unistd.h>
#endif /* ifndef _WIN32 */

#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/db/AA_db.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdasn1_common/AttributeCertificate.h>
#include <bmd/libbmdaa2/ac_server/ac_server.h>

#define ERR_AASRV_DECODE_X509_CERTIFICATE_DATA	-100
#define ERR_AASRV_UNKNOWN_AC_HOLDER_SOURCE	-101
#define ERR_AASRV_BN_ONE			-102
#define ERR_AASRV_BN_HEX2BN			-103
#define ERR_AASRV_WRONG_SERIAL_FILE		-104
#define ERR_AASRV_BN_ADD			-105
#define ERR_AASRV_OPEN_FILE			-106

#if defined(__cplusplus)
extern "C" {
#endif
	long ST_GenAttributeCert( GenBuf_t *X509Certificate, certificate_attributes_t **UserAttributes, ConfigData_t *ConfigDataStruct);

	int AA_GenAttributeCert(GenBuf_t *X509Certificate, GenBuf_t **AttrCertificate, ConfigData_t *ConfigDataStruct);
	/* Funkcje obslugujace poszczegolne pola certyfikatu */
	int AA_AttributeCertificate_set_Issuer(ConfigData_t *ConfigData,
				AttributeCertificate_t *AC);
	int AA_AttributeCertificate_set_signatureAlgorithmIdentifier(ConfigData_t *ConfigData,
				AttributeCertificate_t *AC);
	int AA_AttributeCertificate_set_serialNumber(GenBuf_t *AcSerialGenBuf,
				AttributeCertificate_t *AC);
	int AA_AttributeCertificate_set_attrCertValidityPeriod(ConfigData_t *ConfigData,
				AttributeCertificate_t *AC);
	int AA_AttributeCertificate_set_attributes(ConfigData_t *ConfigData,
				AttributesSequence_t *AttributesSequence,
				AttributeCertificate_t *AC);
	int AA_AttributeCertificate_set_signatureValue(ConfigData_t *ConfigData,
				AttributeCertificate_t *AC);
	int AA_Create_baseCertificateID(Certificate_t *Certificate,
				ConfigData_t *ConfigData,
				IssuerSerial_t **IssuerSerial);

#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_server_INCLUDED_*/
