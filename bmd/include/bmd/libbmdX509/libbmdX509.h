#ifndef _LIBBMDX509_INCLUDED_
#define _LIBBMDX509_INCLUDED_

#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/common/bmd-openssl.h>

#define ERR_X509_CONVERT_INTEGER_T_TO_STRING		-200
#define ERR_X509_GETTING_VALIDITY_NB_TIME		-201
#define ERR_X509_GETTING_VALIDITY_NA_TIME		-202
#define ERR_X509_UNRECOGNIZED_ATTR_TYPE			-203
#define ERR_X509_DECODE_CERTIFICATE_DATA		-204
#define ERR_X509_CONVERT_INTEGER_T_TO_LONG		-205
#define ERR_X509_VERSION_FIELD_NOT_PRESENT		-206
#define ERR_X509_CONVERT_NAME_T_TO_STRING		-207
#define ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE		-208
#define ERR_X509_GET_SURNAME_FROM_RDNSEQUENCE		-209
#define ERR_X509_GET_GIVENNAME_FROM_RDNSEQUENCE		-210
#define ERR_X509_GET_COMMONNAME_FROM_RDNSEQUENCE	-211
#define ERR_X509_CERTIFICATE_EXT_NOT_PRESENT		-212
#define ERR_X509_UNRECOGNIZED_EXT_TYPE			-213
#define ERR_X509_UNDEFINED_EXTENSION_TYPE		-214
#define ERR_X509_CHECK_EXT_TYPE				-215
#define ERR_X509_EXT_CRL_DISTP_NOT_FOUND		-216
#define ERR_X509_GET_ORGANIZATION_FROM_RDNSEQUENCE	-217
#define ERR_X509_GET_COUNTRY_FROM_RDNSEQUENCE -218
#define ERR_X509_GET_LOCALITY_FROM_RDNSEQUENCE -218
#define ERR_X509_GET_ORGANIZATIONALUNIT_FROM_RDNSEQUENCE		-218
#define ERR_X509_GET_SERIALNUMBER_FROM_RDNSEQUENCE				-219
#define ERR_X509_GET_PSEUDONYM_FROM_RDNSEQUENCE					-220
#define ERR_X509_PSEUDONYM_WITH_NAME							-221
#define ERR_X509_ORGANISATION_WITHOUT_ADDRESS					-222
#define ERR_X509_NO_EXTENSIONS									-223
#define ERR_X509_SUBJECT_KEY_IDENTIFIER_EXISTS					-224
#define ERR_X509_KEY_USAGE_ABSENT								-225
#define ERR_X509_DECODE_KEY_USAGE								-226
#define ERR_X509_NOT_SUPPORTED_EXTENDED_KEY_USAGE				-227
#define ERR_X509_DECODE_BASIC_CONSTRAINTS						-228
#define ERR_X509_BASIC_CONSTRAINTS_TRUE							-229
#define ERR_X509_NOT_SUPPORTED_CRITICAL_EXTENSION				-230
#define ERR_X509_ISSUER_NOT_CORRECT								-231
#define ERR_X509_WRONG_SIZE_OF_BYTES							-232
#define ERR_X509_NON_REPUDATION_BIT_NOT_SET						-233
#define ERR_X509_WRONG_CERTIFICATE_VERSION						-234
#define ERR_X509_WRONG_CERTIFICATE_SIGNING_ALGHORITM			-235
#define ERR_X509_WRONG_SUBJECT_PUBLIC_KEY_SIGNING_ALGHORITM		-236
#define ERR_X509_WRONG_CERTIFICATE_CATEGORY						-237
#define ERR_X509_DECODE_EXTENDED_KEY_USAGE						-238

#define OID_X509_ID_CE_CRLDISTRIBUTIONPOINTS 2,5,29,31

typedef enum X509CertAttrType{X509_ATTR_UNDEFINED,	/* nieznana wersja atrybutu */
			X509_ATTR_VERSION,		/* wersja certyfikatu */
			X509_ATTR_SN,			/* numer seryjny certyfikatu */
			X509_ATTR_SUBJECT_SN,	/*Numer seryjny z podmiotu certyfikatu*/
			X509_ATTR_ISSUER_SN,	/*Numer seryjny certyfikatu wystawcy*/
			X509_ATTR_SUBJECT_DN,		/* nazwa wyrozniona wlasciciela certyfikatu */
			X509_ATTR_ISSUER_DN,		/* nazwa wyrozniona wystawcy certyfikatu */
			X509_ATTR_SIGALG_OID,		/* OID algorytmu sluzacego do podpisu certyfikatu */
			X509_ATTR_VALIDITY_NB,		/* czas waznosci certyfikatu (notBefore) */
			X509_ATTR_VALIDITY_NA,		/* czas waznosci certyfikatu (notAfter) */
			X509_ATTR_SUBJECT_SURNAME,	/* nazwisko wlasciciela certyfikatu */
			X509_ATTR_SUBJECT_GIVENNAME,	/* imie wlasciciela certyfikatu */
			X509_ATTR_SUBJECT_COMMONNAME,	/* nazwa potoczna wlasciciela certyfikatu */
			X509_ATTR_SUBJECT_ORGANIZATION,	/* nazwa organizacji, do której należy właściciel certyfikatu */
			X509_ATTR_ISSUER_ORGANIZATION, /* nazwa organizacji, do której należy wystawca certyfikatu */
			X509_ATTR_SUBJECTPUBLICKEYINFO_ALG_OID, /*oid algorytmu klucza publicznego (by WSZ)*/
			X509_ATTR_ISSUER_COMMONNAME,	/* nazwa potoczna wystawcy certyfikatu */
			X509_ATTR_EXT_CRL_DISPOINTS,	/* lista punktow dystrybucyjnych CRLi */
			X509_ATTR_COUNTRY,
			X509_ATTR_ISSUER_COUNTRY,
			X509_ATTR_LOCALITY,
			X509_ATTR_ISSUER_LOCALITY,
			X509_ATTR_ORGANIZATIONALUNIT,
			X509_ATTR_ISSUER_ORGANIZATIONALUNIT
			} X509CertAttrType_t;

typedef enum X509CertExtType{CERT_EXT_UNDEFINED, 	/* nieznane rozszerzenie */
			CERT_EXT_CRL_DP			/* lista punktow dystrybucyjnych CRLa */
			} X509CertExtType_t;

#if defined(__cplusplus)
extern "C" {
#endif

/****************
 * ONE CALL API *
 ***************/
/* API, w ktorym za pomoca jednej funkcji realizowana sa wszystkie operacjie pobierania 
   parametrow certyfikatu */

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej wybrane za pomoca X509CertAttrType jego atrybuty.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param X509CertAttrType - wybrany atrybut certyfikatu. Aktualnie obslugiwane sa:
 * 		X509_ATTR_VERSION,		- wersja certyfikatu (long)
 *			X509_ATTR_SN,			- numer seryjny certyfikatu (string)
 *			X509_ATTR_SUBJECT_DN,		- nazwa wyrozniona DN wlasciciela certyfikatu (string)
 *			X509_ATTR_ISSUER_DN		- nazwa wyrozniona DN wystawcy certyfikatu (string)
 *			X509_ATTR_SIGALG_OID		- OID algorytmu sluzacego do podpisu certyfikatu (string)
 *			X509_ATTR_VALIDITY_NB,		- czas waznosci certyfikatu (notBefore) (string)
 *			X509_ATTR_VALIDITY_NA		- czas waznosci certyfikatu (notAfter) (string)
 *			X509_ATTR_SUBJECT_SURNAME	- nazwisko wlasciciela certyfikatu (string)
 *			X509_ATTR_SUBJECT_COMMONNAME	- nazwa potoczna wlasciciela certyfikatu (string)
 *			X509_ATTR_COUNTRY,
 *			X509_ATTR_ISSUER_COUNTRY,
 *			X509_ATTR_LOCALITY,
 *			X509_ATTR_ISSUER_LOCALITY,
 *			X509_ATTR_ORGANIZATIONALUNIT,
 *			X509_ATTR_ISSUER_ORGANIZATIONALUNIT
 * @param AttributeString - jesli pobierany atrybut jest stringiem to jest on alokowany tutaj
 * @param AttributeLong   - jesli pobierany atrybut jest longiem to jest on alokowany tutaj
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note - funkcja alokuje pamiec pod string AttributeString.
 */
long X509CertGetAttr(GenBuf_t *GenBuf, X509CertAttrType_t X509CertAttrType, char **AttributeString, long *AttributeLong);

/************
 * EASY API *
 ***********/

long GetIssuerEMailFromX509Certificate(char *pfx_file, char *pfx_pass, char **e_mail);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej wersje.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param version - zwracana wersja CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 */
long GetVersionFromX509Certificate(GenBuf_t *GenBuf, long *version);

/** Funkcja ustala kategorie certyfikatu na potrzeby Rozporzadzenia w sprawie okreslenia warunkow technicznych.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param category - zwracana kategoria
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
*/
long CheckCertificateSubjectInX509Certificate(GenBuf_t *GenBuf, long *category, CertExtConfig_t *twf_CertExtConfig);

/** Funkcja ustala poprawnosc wystawcy certyfikatu na potrzeby Rozporzadzenia w sprawie okreslenia warunkow technicznych.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param twf_CertExtConfig - struktura z konfiguracja
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
*/
long CheckCertificateIssuer(GenBuf_t *GenBuf, CertExtConfig_t *twf_CertExtConfig);

/** Funkcja sprawdza potrzebne rozszerzenia na potrzeby Rozporzadzenia w sprawie okreslenia warunkow technicznych.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
*/
long CheckCertificateExtensionsInX509Certificate(GenBuf_t *GenBuf, CertExtConfig_t *twf_CertExtConfig);


/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej numer seryjny.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SerialNumber - zwracany numer seryjny certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SerialNumber
 */
long GetSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej numer seryjny podmiotu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SerialNumber - zwracany numer seryjny podmiotu certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SerialNumber
 */
long GetSubjectSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej numer seryjny wystawcy.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SerialNumber - zwracany numer seryjny wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SerialNumber
 */
long GetIssuerSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwe wyrozniona DN wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectDN - zwracana nazwa wyrozniona DN wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectDN
 */
long GetSubjectDNFromX509Certificate(GenBuf_t *GenBuf, char **SubjectDN);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwisko - Surname - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectSurname - zwracana nazwa wyrozniona DN wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectSurname
 */
long GetSubjectSurnameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectSurname);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej imie - GivenName - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectGivenName - zwracana nazwa wyrozniona DN wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectGivenName
 */
long GetSubjectGivenNameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectGivenName);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwe potoczna - CommonName - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectCommonName - zwracana nazwa potoczna wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectCommonName
 */
long GetSubjectCommonNameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectCommonName);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwę organizacji - Organization - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectOrganization - zwracana nazwa potoczna wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectOrganization
 */
long GetSubjectOrganizationFromX509Certificate(GenBuf_t *GenBuf, char **SubjectOrganization);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwę organizacji - Organization - wystawcy certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectOrganization - zwracana nazwa potoczna wystawcy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectOrganization
 */
long GetIssuerOrganizationFromX509Certificate(GenBuf_t *GenBuf, char **IssuerOrganization);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwę organizacji - Organization - wystawcy certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectOrganization - zwracana nazwa potoczna wystawcy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectOrganization
 */
long GetSubjectOrganizationFromX509Certificate(GenBuf_t *GenBuf, char **SubjectOrganization);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej nazwe wyrozniona DN wystawcy certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param IssuerDN - zwracana nazwa wyrozniona DN wystawcy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod IssuerDN
 */
long GetIssuerDNFromX509Certificate(GenBuf_t *GenBuf, char **IssuerDN);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej OID algorytmu uzytego do podpisu certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SignatureAlgorithmOid - zwracana OID algorytmu uzytego do podpisu certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SignatureAlgorithmOid
 */
long GetAlgorithmOIDFromX509Certificate(GenBuf_t *GenBuf, char **SignatureAlgorithmOid);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej date waznosci certyfikatu (notBefore).
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param ValidityNB - data waznosci certyfikatu (notBefore)
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod ValidityNB
 */
long GetValidityNBFromX509Certificate(GenBuf_t *GenBuf, char **ValidityNB);
long GetValidityNBFromX509Certificate_time(GenBuf_t *GenBUf,time_t *time_NB);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej date waznosci certyfikatu (notAfter).
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param ValidityNA - data waznosci certyfikatu (notAfter)
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod ValidityNA
 */
long GetValidityNAFromX509Certificate(GenBuf_t *GenBuf, char **ValidityNA);
long GetValidityNAFromX509Certificate_time(GenBuf_t *GenBuf, time_t *time_NA);
/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej liste punktow dostepowych z CRLami oddzielone separatorem "|".
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param CRLDistPointsString - lista z punktami dystrybucyjnymi CRLi, punkty
 * 				sa oddzielone separatorem "|"
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod CRLDistPointsString
 */
long GetCRLDistPointsFromX509Certificate(GenBuf_t *GenBuf, char **CRLDistPointsString);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej panstwo - Country - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectCountry - zwracana nazwa panstwa wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectCountry
 */
long GetSubjectCountryFromX509Certificate(GenBuf_t *GenBuf, char **SubjectCountry);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej panstwo - Country - wystawcy certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param SubjectCountry - zwracana nazwa panstwa wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod SubjectCountry
 */
long GetIssuerCountryFromX509Certificate(GenBuf_t *GenBuf, char **IssuerCountry);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej lokalizację - Locality - wlasciciela certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param Locality - zwracana lokalizacja wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod Locality
 */
long GetSubjectLocalityFromX509Certificate(GenBuf_t *GenBuf, char **Locality);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej lokalizację - Locality - wystawcy certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param Locality - zwracana lokalizacja wystawcy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod Locality
 */
long GetIssuerLocalityFromX509Certificate(GenBuf_t *GenBuf, char **Locality);

long GetSubjectOrganizationalUnitFromX509Certificate(GenBuf_t *GenBuf, char **OrganizationalUnit);
long GetIssuerOrganizationalUnitFromX509Certificate(GenBuf_t *GenBuf, char **OrganizationalUnit);

/** Funkcja GetSubjectPublicKeyInfoAlgorithmX509Certificate() parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej oid algorytmu klucza publicznego podmiotu
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param alg_oid - zwracana lokalizacja wlasciciela
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad
 * @note UWAGA!: funkcja alokuje pamiec pod alg_oid
 */
long GetSubjectPublicKeyInfoAlgorithmX509Certificate(GenBuf_t *GenBuf, char **alg_oid);

/************
 * FAST API *
 ***********/
/** Funkcja parsuje zakodowany w DER certyfikat i wyciaga
 * z niej strukture Certificate_t, ktora jest uzywana przez dalsze
 * funkcje parsujace.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat X509 zakodowany w DER
 * @param Certificate - struktura zawierajca dane dot certyfikatu X509
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 */
long GetCertificateStructFromX509Certificate(GenBuf_t *GenBuf, Certificate_t **Certificate);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.version 
 *  wersje certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param version - wersja certyfikatu w postaci longa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 */
long GetVersionFromCertificate(Certificate_t *Certificate, long *version);

/** Funkcja wyciaga z pola Certificate->subject potrzebne pola na potrzeby Rozporzadzenia w sprawie okreslenia
 * warunkow technicznych. Okresla tez kategorie certyfikatu.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 * @param category - zwracana kategoria
 * @param twf_CertExtConfig - struktura konfiguracyjna
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
*/
long CheckCertificateSubject(Certificate_t *Certificate, long *category, CertExtConfig_t *twf_CertExtConfig);


/** Funkcja wyciaga z pola Certificate->extensions potrzebne pola na potrzeby Rozporzadzenia w sprawie okreslenia
 * warunkow technicznych.
 *
 * @param GenBuf bufor generyczny zawierajacy certyfikat zakodowany w DER
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
*/
long CheckCertificateExtensions(Certificate_t *Certificate, CertExtConfig_t *twf_CertExtConfig);


/** Funkcja wyciaga z pola Certificate->tbsCertificate.serialNumber 
 *  numer seryjny certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param serialNumber - numer seryjny certyfikatu w postaci stringu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h oraz libbmdX509.h
 * @note jesli ktos chcialby serialNumber w postaci long lub lub jakiejkolwiek innej
 * to dajcie mi znak
 * @note funkcja automatycznie alokuje pamiec na string serialNumber
 */
long GetSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject.serialNumber 
 *  numer seryjny certyfikatu wystawcy.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param serialNumber - numer seryjny podmiotu certyfikatu w postaci stringu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h oraz libbmdX509.h
 * @note jesli ktos chcialby serialNumber w postaci long lub lub jakiejkolwiek innej
 * to dajcie mi znak
 * @note funkcja automatycznie alokuje pamiec na string serialNumber
 */
long GetSubjectSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.issuer.serialNumber 
 *  numer seryjny certyfikatu wystawcy.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param serialNumber - numer seryjny certyfikatu wystawcy w postaci stringu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h oraz libbmdX509.h
 * @note jesli ktos chcialby serialNumber w postaci long lub lub jakiejkolwiek innej
 * to dajcie mi znak
 * @note funkcja automatycznie alokuje pamiec na string serialNumber
 */
long GetIssuerSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber);


/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject 
 *  nazwe wyrozniona DN wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param SubjectDN - nazwa wyrozniona DN wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string SubjectDN
 */
long GetSubjectDNFromCertificate(Certificate_t *Certificate, char **SubjectDN);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject 
 *  nazwisko - Surname - wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param SubjectSurname - nazwisko wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string SubjectSurname
 */
long GetSubjectSurnameFromCertificate(Certificate_t *Certificate, char **SubjectSurname);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject 
 *  imie - GivenName - wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param SubjectGivenName - nazwisko wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string SubjectGivenName
 */
long GetSubjectGivenNameFromCertificate(Certificate_t *Certificate, char **SubjectGivenName);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject 
 *  nazwe potoczna - CommonName - wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param SubjectCommonName - nazwe potoczna wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string SubjectCommonName
 */
long GetSubjectCommonNameFromCertificate(Certificate_t *Certificate, char **SubjectCommonName);

long GetSubjectOrganizationFromCertificate(Certificate_t *Certificate, char **SubjectOrganization);
long GetIssuerOrganizationFromCertificate(Certificate_t *Certificate, char **IssuerOrganization);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.issuer
 *  nazwe wyrozniona DN wystawcy certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param IssuerDN - nazwa wyrozniona DN wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string IssuerDN
 */
long GetIssuerDNFromCertificate(Certificate_t *Certificate, char **IssuerDN);

/** Funkcja wyciaga z pola TBSCertificate->tbsCertificate.validity.notAfter
 *  czas wygasniecia waznosci certyfikatu
 *  umieszczonego w kopercie CMS wraz z wygenerowanym znacznikiem czasu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param TsaCertValidityNA - czas uplyniecia waznosci certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h oraz libbmdX509.h
 * @note jesli ktos chcialby TsaCertValidityNA w postaci char* lub lub jakiejkolwiek innej
 * to dajcie mi znak
 */
long GetValidityNAFromCertificate(Certificate_t *Certificate, char **CertValidityNA);

/** Funkcja wyciaga z pola TBSCertificate->tbsCertificate.validity.notAfter
 *  czas wygasniecia waznosci certyfikatu
 *  umieszczonego w kopercie CMS wraz z wygenerowanym znacznikiem czasu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param TsaCertValidityNB - czas, po ktorym certyfikat zacznie byc wazny
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h oraz libbmdX509.h
 * @note jesli ktos chcialby TsaCertValidityNB w postaci char* lub lub jakiejkolwiek innej
 * to dajcie mi znak
 */
long GetValidityNBFromCertificate(Certificate_t *Certificate, char **CertValidityNB);

/** Funkcja wyciaga z pola TBSCertificate->tbsCertificate.signature
 *  OID algorytmu uzytego do podpisu certyfikatu
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param SignatureAlgorithmOid - OID algorytmu uzytego do podpisu certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string SignatureAlgorithmOid
 */
long GetAlgorithmOIDFromCertificate(Certificate_t *Certificate, char **SignatureAlgorithmOid);

/** Funkcja parsuje zakodowana w DER strukture certyfikatu i wyciaga
 * z niej rozszerzenie zawierające liste punktow dystrybucyjnych CRLi w postaci stringu
 * oddzielonego separatorem "|".
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param CRLDistPointsString - lista punktow dystrybucyjnych CRLi w postaci stringu
 * oddzielonego separatorem "|" 
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note UWAGA!: funkcja alokuje pamiec pod CRLDistPointsString
 */
long GetExtCRLDistPointsFromCertificate(Certificate_t *Certificate, char **CRLDistPointsString);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.extensions wzkaznik na rozszerzenia certyfikatu.
 *
 * @param Certificate - struktura certyfikatu X509
 * @param Extensions - wskaznik na rozszerzenia certyfikatu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdX509.h oraz libbmdcms.h
 * @note - UWAGA: kopiowany jest tylko adres do rozszerzen, nie zwalniac Certficate bo
 *		wskaznik bedzie pokazywał na zwolnione miejsce w pamięci!!!
 */
long GetExtensionsFromCertificate(Certificate_t *Certificate, Extensions_t **Extensions);

/** Funkcja wyciaga z pola rozszerzen rozszerzenie certyfikatu o wskazanym X509CertExtType typie.
 *
 * @param Extensions - wskaznik na rozszerzenia CRLa
 * @param X509CertExtType -  wybrane rozszerzenie Certyfikatu. Aktualnie obslugiwane sa:
 * 			CERT_EXT_CRL_DP,		- lista punktow dystrybucyjnych CRLa w postaci 
 * 							  stringu oddzielonego separatorem "|" (string)
 * @param ExtensionString - jesli pobierane rozszerzenie jest stringiem to jest on alokowany tutaj
 * @param ExtensionLong   - jesli pobierane rozszerzenie jest longiem to jest on alokowany tutaj
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h oraz libbmdcms.h
 */
long GetCertificateExtension(Extensions_t *Extensions, X509CertExtType_t X509CertExtType, 
				char **ExtensionString, long *ExtensionLong);

/** Funkcja wyciaga z pola rozszerzen liste odnosnikow do CRLi oddzielona separatorem "|".
 *
 * @param Extensions - wskaznik na rozszerzenia CRLa
 * @param CRLDistPoints - numer seryjny bazowego CRLa lub "0" dla delta-CRL
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h oraz libbmdcms.h
 * @note - funkcja alokuje pamiec pod string CRLDistPoints
 */
long GetCRLDistPointsFromCRLExtension(Extensions_t *Extensions, char **CRLDistPoints);

/** Funkcja zamienia OID typu rozszerzenia na typ wyliczeniowy X509CertExtType_t.
 *
 * @param Oid - wskaznik OID typu rozszerzenia CRLa
 * @param X509CertExtType - typ rozszerzenia CRLa (patrz na X509CertExtType w libbmdX509.h)
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h oraz libbmdcms.h
 */
long GetCertExtType(OBJECT_IDENTIFIER_t *Oid, X509CertExtType_t *X509CertExtType);

/** Funkcja zamienia wartosc rozszerzenia OCTET_STRING_t na string zawierajacy liste punktow
 * dystrybucyjnych CRL oddzielonych searatorem "|".
 *
 * @param ExtensionValue - wskaznik na wartosc atrybutu
 * @param string - string zawierajacy liste punktow dystrybucyjnych CRL 
 *			oddzielonych searatorem "|"
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h oraz libbmdcms.h
 * @note - funkcja alokuje pamiec pod string
 */
long ExtensionCRLDistPoints2string(OCTET_STRING_t *ExtensionValue, char **string);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject
 *  nazwe panstwa wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param Country - panstwo wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string Country
 */
long GetSubjectCountryFromCertificate(Certificate_t *Certificate, char **Country);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.issuer
 *  nazwe panstwa wystawcy certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param Country - panstwo wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string Country
 */
long GetIssuerCountryFromCertificate(Certificate_t *Certificate, char **Country);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject
 *  pole lokalizacji wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param Locality - lokalizacja wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string Locality
 */
long GetSubjectLocalityFromCertificate(Certificate_t *Certificate, char **Locality);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.issuer
 *  pole lokalizacji wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param Locality - lokalizacja wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string Locality
 */
long GetIssuerLocalityFromCertificate(Certificate_t *Certificate, char **Locality);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.subject
 *  nazwe jednostki organizacyjnej wlasciciela certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param OrganizationalUnit - nazwa jednostki organizacyjnej właściciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string OrganizationalUnit
 */
long GetSubjectOrganizationalUnitFromCertificate(Certificate_t *Certificate, char **OrganizationalUnit);

/** Funkcja wyciaga z pola Certificate->tbsCertificate.issuer
 *  nazwe jednostki organizacyjnej wystawcy certyfikatu.
 *
 * @param Certificate - struktura zawierajca dane dot certyfikatu
 * @param OrganizationalUnit - nazwa jednostki organizacyjnej wystawcy certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdX509.h
 * @note funkcja automatycznie alokuje pamiec na string OrganizationalUnit
 */
long GetIssuerOrganizationalUnitFromCertificate(Certificate_t *Certificate, char **OrganizationalUnit);

/** Funkcja GetSubjectPublicKeyInfoAlgorithmFromCertificate() wyciaga z pola Certificate->tbsCertificate.subjectPublicKeyInfo
 *  oid algorytmu klucza publicznego wlasciciela certyfikatu.
 *
 * @param certificate - struktura zawierajca dane dot certyfikatu
 * @param oid - oid algorytmu klucza publicznego wlasciciela certyfikatu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad 
 * @note funkcja automatycznie alokuje pamiec na OBJECT_IDENTIFIER oid
 */
long GetSubjectPublicKeyInfoAlgorithmFromCertificate(Certificate_t *certificate, OBJECT_IDENTIFIER_t **oid);

long GetIssuerCommonNameFromX509Certificate(GenBuf_t *GenBuf, char **IssuerCommonName);
long GetIssuerCommonNameFromCertificate(Certificate_t *Certificate, char **IssuerCommonName);
long GetCrlDistributionPoints(GenBuf_t *cert_buf,char ***crl_dp);
long GetPubKeyFromCert(GenBuf_t *cert_buf,GenBuf_t **pub_key);
long GetPubKeyInfoFromCert(GenBuf_t *cert_buf,AlgorithmIdentifier_t **Algorithm);

void get_CommonName_from_subject(X509 *cert,char **cn);
void get_CommonName_from_issuer(X509 *cert,char **cn_issuer);
void get_Organization_from_subject(X509 *cert,char **organization);
void get_Serial_from_certificate(X509 *cert,char **serial);
void get_base_info_from_cert(X509 *cert,char **cn,char **organization,char **cn_issuer,char **serial,long *cert_type);

/**
funkcja X509_is_non_repudiation() pobiera bufor z certyfikatem (zakodowanym w DER)
i zwraca:
@retval 1 , jesli uzycie klucza non-repudiation jest ustawione
@retval 0 , jesli uzycie klucza non-repudiation NIE jest ustawione
@retval	-1, jesli certyfikat nie posiada roszerzenia KeyUsage
@retval inna_wartość_ujemna w przypadku błędu:
-11 jesli za certificate podano NULL
-12 jesli bufor jest pusty (NULL)
-13 jesli bufor jest zerowego rozmiaru
-14 jesli bufor zawiera niepoprawna strukture certyfikatu
-15 jesli certyfikat nie posiada rozszerzen
-16 jesli nie mozna zdekodowac jednego z roszerzen certyfikatu
-17 jesli wystawca certyfikatu zmienil podejscie w "interpretacji" BIT STRING
*/
long X509_is_non_repudiation(GenBuf_t *certificate, CertExtConfig_t *twf_CertExtConfig);

long X509Certificate_free_ptr(Certificate_t **Certificate);
#if defined(__cplusplus)
}
#endif

#endif /*_LIBBMDX509_INCLUDED_*/
