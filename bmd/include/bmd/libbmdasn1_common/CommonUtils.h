#ifndef _ASN1_COMMON_COMMONUTILS_INCLUDED_
#define _ASN1_COMMON_COMMONUTILS_INCLUDED_

#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-const.h>
#include <bmd/libbmdasn1_common/TimeStampResp.h>
#include <bmd/libbmdasn1_common/TSTInfo.h>
#include <bmd/libbmdasn1_common/CertificateList.h>
#include <bmd/libbmdasn1_common/PostalAddress.h>
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/libbmdasn1_common/CRLDistributionPoints.h>
#include <bmd/libbmdasn1_common/EmailAddress.h>
#include <bmd/libbmdasn1_common/ClassList.h>
#include <bmd/libbmdasn1_common/ExtKeyUsageSyntax.h>
#include <bmd/libbmdasn1_common/BasicConstraints.h>

#define CLASS_LIST_UNMARKED_STR	"unmarked"
#define CLASS_LIST_UNCLASSIFIED_STR	"unclassified"
#define CLASS_LIST_RESTRICTED_STR	"restricted"
#define CLASS_LIST_CONFIDENTIAL_STR	"confidential"
#define CLASS_LIST_SECRET_STR		"secret"
#define CLASS_LIST_TOP_SECRET_STR	"topsecret"

#define OID_ID_AT_COMMONNAME_LDAP_PREFIX			"CN="
#define OID_ID_AT_SURNAME_LDAP_PREFIX				"SN="
#define OID_ID_AT_SERIALNUMBER_LDAP_PREFIX			"serialNumber="
#define OID_ID_AT_COUNTRY_LDAP_PREFIX				"C="
#define OID_ID_AT_LOCALITY_LDAP_PREFIX				"L="
#define OID_ID_AT_STATEORPROVINCE_LDAP_PREFIX			"S="
#define OID_ID_AT_ORGANIZATION_LDAP_PREFIX			"O="
#define OID_ID_AT_ORGANIZATIONALUNIT_LDAP_PREFIX		"OU="
#define OID_ID_AT_TITLE_LDAP_PREFIX				"Title="
#define OID_ID_AT_DESCRIPTION_LDAP_PREFIX			"description="
#define OID_ID_AT_GIVENNAME_LDAP_PREFIX				"G="
#define OID_ID_AT_GENERATIONQUALIFIER_LDAP_PREFIX		"GenerQualifier="
#define OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER_LDAP_PREFIX	"DNQualifier="
#define OID_ID_AT_PSEUDONYM_LDAP_PREFIX				"Pseudonym="
#define OID_ID_AT_POSTALADDESS_LDAP_PREFIX			"postalAddress="
#define OID_ID_AT_EMAILADDRESS_LDAP_PREFIX			"emailAddress="

#define OID_ID_AT_COMMONNAME				"2.5.4.3"
#define OID_ID_AT_SURNAME				"2.5.4.4"
#define OID_ID_AT_SERIALNUMBER			"2.5.4.5"
#define OID_ID_AT_COUNTRY				"2.5.4.6"
#define OID_ID_AT_LOCALITY				"2.5.4.7"
#define OID_ID_AT_STATEORPROVINCE			"2.5.4.8"
#define OID_ID_AT_ORGANIZATION			"2.5.4.10"
#define OID_ID_AT_ORGANIZATIONALUNIT		"2.5.4.11"
#define OID_ID_AT_TITLE					"2.5.4.12"
#define OID_ID_AT_DESCRIPTION				"2.5.4.13"
/*FIXME - sprawdzic klase OIDa */
#define OID_ID_AT_POSTALADDRESS			"2.5.4.16"
#define OID_ID_AT_GIVENNAME				"2.5.4.42"
#define OID_ID_AT_GENERATIONQUALIFIER		"2.5.4.43"
#define OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER	"2.5.4.46"
#define OID_ID_AT_PSEUDONYM				"2.5.4.65"
#define OID_ID_AT_EMAILADDRESS			"1.2.840.113549.1.9.1"

#define OID_ID_AT_COMMONNAME_LONG				2,5,4,3
#define OID_ID_AT_SURNAME_LONG				2,5,4,4
#define OID_ID_AT_SERIALNUMBER_LONG				2,5,4,5
#define OID_ID_AT_COUNTRY_LONG				2,5,4,6
#define OID_ID_AT_LOCALITY_LONG				2,5,4,7
#define OID_ID_AT_STATEORPROVINCE_LONG			2,5,4,8
#define OID_ID_AT_ORGANIZATION_LONG				2,5,4,10
#define OID_ID_AT_ORGANIZATIONALUNIT_LONG			2,5,4,11
#define OID_ID_AT_TITLE_LONG					2,5,4,12
#define OID_ID_AT_DESCRIPTION_LONG				2,5,4,13
/*FIXME - sprawdzic klase OIDa */
#define OID_ID_AT_POSTALADDRESS_LONG			2,5,4,16
#define OID_ID_AT_GIVENNAME_LONG				2,5,4,42
#define OID_ID_AT_GENERATIONQUALIFIER_LONG		2,5,4,43
#define OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER_LONG	2,5,4,46
#define OID_ID_AT_PSEUDONYM_LONG				2,5,4,65
#define OID_ID_AT_EMAILADDRESS_LONG				1,2,840,113549,1,9,1

/*#define NO_MEMORY		-1
#define ERR_NO_MEMORY		-1
*/

#define ERR_ASN1_UNRECOGNIZED_ATTRIBUTE_OID		-208
#define ERR_ASN1_DECODE_DIRECTORY_STRING_DATA		-209
#define ERR_ASN1_UNRECOGNIZED_TIME_TYPE			-210
#define ERR_ASN1_DER_ENCODING				-211
#define ERR_ASN1_NAME_HAS_NO_RDNSEQUENCE_FIELD		-212
#define ERR_ASN1_CLONE_RDNSEQUENCE			-213
#define ERR_ASN1_RDNSEQUENCE_UNRECOGNIZED_ATTR_TYPE	-214
#define ERR_ASN1_GET_ATTVALUE_FROM_RDNSEQUENCE		-215
#define ERR_ASN1_CLONE_ATTRVALUE			-216
#define ERR_ASN1_ATTRIBUTE_TYPE_NOT_PRESENT		-217
#define ERR_ASN1_DECODE_POSTAL_ADDRESS_DATA		-218
#define ERR_ASN1_DECODE_CRL_DISTPOINTS_DATA		-219
#define ERR_ASN1_DECODE_IA5STRING_DATA			-220
#define ERR_ASN1_OBJECT_IDENTIFIER_SET_ARCS		-221
#define ERR_ASN1_STRING2OID				-222
#define ERR_ASN1_STRING2OCTET_STRING			-223
#define ERR_ASN1_ENCODE_IA5STRING			-224
#define ERR_ASN1_DER_ENCODE_FAILED			-225
#define ERR_ASN1_DECODE_UTF8STRING			-226
#define ERR_ASN1_DECODE_INTEGER				-227
#define ERR_ASN1_CONVERT_INTEGER_TO_LONG		-228
/*#define UNCOMPARABLE_ARGUMENTS				-3*/

#define OID_SHA1_WITH_RSA_STR					"1.2.840.113549.1.1.5"
#define OID_DSA_WITH_SHA1_STR					"1.2.840.10040.4.3"
#define OID_ECDSA_WITH_SHA1_STR					"1.2.840.10045.4.1"

#define OID_RSA_ENCRYPTION_PUB_KEY_STR			"1.2.840.113549.1.1.1"
#define OID_DSA_PUB_KEY_STR						"1.2.840.10040.4.1"


typedef enum RDNSequenceAttrType{RDNSEQ_ATTR_UNDEFINED,	/* nieznana wersja atrybutu */
			RDNSEQ_ATTR_SURNAME,		/* nazwisko podmiotu OID=2.5.4.4 */
			RDNSEQ_ATTR_GIVENNAME,		/* imie podmiotu OID=2.5.4.42 */
			RDNSEQ_ATTR_COMMONNAME,		/* nazwa potoczna podmiotu OID=2.5.4.3 */
			RDNSEQ_ATTR_ORGANIZATION,	/* nazwa organizacji podmiotu OID=2.5.4.10 */
			RDNSEQ_ATTR_COUNTRY,	/*panstwo, OID=2.5.4.6*/
			RDNSEQ_ATTR_LOCALITY,	/*OID=2.5.4.7*/
			RDNSEQ_ATTR_ORGANIZATIONALUNIT,	/*OID=2.5.4.11*/
			RDNSEQ_ATTR_SERIALNUMBER, /*2.5.4.5*/
			RDNSEQ_ATTR_PSEUDONYM, /*2.5.4.65*/
			RDNSEQ_ATTR_STATE, /*2.5.4.8*/
			RDNSEQ_ATTR_POSTALADDRESS /*2.5.4.16*/
			} RDNSequenceAttrType_t;


#if defined(__cplusplus)
extern "C" {
#endif

/* konwertery OIDowe */
long OBJECT_IDENTIFIER_cmp(	OBJECT_IDENTIFIER_t *oidt, char *pOIDTab);
long OID_to_ulong_alloc2(OBJECT_IDENTIFIER_t *oid, long **arcs);
long OID_to_ulong2(OBJECT_IDENTIFIER_t *oid, long **arcs, long arc_type_size, long arc_slots);
long ulong_to_str_of_OID2(long *OIDtable, long OIDtable_size, char **OIDstring);
long OID_cmp2(void *oid1, long sizeoid1, void *oid2, long sizeoid2);

/* konwertery struktur do stringow */
long OBJECT_IDENTIFIER2string(OBJECT_IDENTIFIER_t *OBJECT_IDENTIFIER, char **OidString);
long INTEGER2string(INTEGER_t *Integer, char **string);
long AnotherName2string(AnotherName_t *AnotherName, char **string);
long OCTET_STRING2string(OCTET_STRING_t *OCTET_STRING, char **string);
long ORAddress2string(ORAddress_t *ORAddress, char **string);
long DirectoryString2string(DirectoryString_t *DirectoryString, char **string);
long AttributeTypeAndValue2string(AttributeTypeAndValue_t *AttributeTypeAndValue, char **string);
long RelativeDistinguishedName2string(RelativeDistinguishedName_t *RelativeDistinguishedName, char **string);
long RDNSequence2string(RDNSequence_t *RDNSequence, char **string);
long RDNSequence2sortedString(RDNSequence_t *RDNSequence, char **string);
long Time2string(Time_t *Time, char **string);
long GeneralizedTime2string(GeneralizedTime_t *GeneralizedTime, char **string);
long UTCTime2string(UTCTime_t *UTCTime, char **string);
long Name2string(Name_t *Name, char **string);
long Name2sortedString(Name_t *Name, char **string);
long GetAttrFromRDNSequence(RDNSequence_t *RDNSequence, RDNSequenceAttrType_t RDNSequenceAttrType, char **AttributeString);
long AttributeValue2String(AttributeValue_t *AttributeValue, char **string);
long EDIPartyName2string(EDIPartyName_t *EDIPartyName, char **string);
long GeneralName2string(GeneralName_t *GeneralName, char **string);
long GeneralNames2string(GeneralNames_t *GeneralNames, char **string);
long CRLDistributionPoints2string(CRLDistributionPoints_t *CRLDistributionPoints, char **string);
long DistributionPointName2string(DistributionPointName_t *DistributionPointName, char **string);
long Any2UTF8String2String(ANY_t *Any, char **string);
long Any2INTEGER2String(ANY_t *Any, char **string);
long ClassList2string(ClassList_t *ClassList, char **string);


/* konwertery, klonery i inne bajery */
long asn_cloneContent(asn_TYPE_descriptor_t *td, void *src, void **dst);
long alloc_EncodeBuffer(asn_enc_rval_t rv, size_t twf_kn_size, GenBuf_t **buf);
long free_GenBuf(GenBuf_t **GenBuf);
long free_GenBufList(GenBufList_t **GenBufList);
long AttributeValue2DirectoryString(AttributeValue_t *AttributeValue, DirectoryString_t **DirectoryString);
long AttributeValue2PostalAddress(AttributeValue_t *AttributeValue, PostalAddress_t **PostalAddress);
long AttributeValue2IA5String(AttributeValue_t *AttributeValue, IA5String_t **IA5String);
long ExtensionValue2CRLDistributionPoints(OCTET_STRING_t *ExtensionValue, CRLDistributionPoints_t **CRLDistributionPoints);
long Name2RDNSequence(Name_t *Name, RDNSequence_t **RDNSequence);
long GetAttributeValueFromRDNSequence(RDNSequence_t *RDNSequence, RDNSequenceAttrType_t RDNSequenceAttrType,
				AttributeValue_t **AttributeValue);
long GetValueFromAttributeTypeAndValue(AttributeTypeAndValue_t *AttributeTypeAndValue,
				RDNSequenceAttrType_t RDNSequenceAttrType,
				AttributeValue_t **AttributeValue);

long prepare_digest_info(GenBuf_t *digest,GenBuf_t **output);

/*!!! wykomentowane przez WSZ: zeby bylo wiadomo, ze pisalem wlasna implementacje tej funkcji*/
/*long String2OID(const char *String, OBJECT_IDENTIFIER_t *oid);*/
long String2OID_alloc(const char *String, OBJECT_IDENTIFIER_t **oid);
long String2IA5String2ANY(const char *String, ANY_t *any);
long String2UTF8String2ANY(const char *String, ANY_t *any);
long String2UTF8String2ANY_alloc(const char *String, ANY_t **any);
long String2INTEGER2ANY(const char *String, ANY_t *any);
long String2INTEGER2ANY_alloc(const char *String, ANY_t **any);
long String2OCTET_STRING2ANY(const char *String,	ANY_t *any);
long String2OCTET_STRING2ANY_alloc(const char *String, ANY_t **any);
int CU_buff_composer_reentrant(const void *encoder_buffer, size_t size, void *out_buff);
long CutString2GenBufList_alloc(const char *String, GenBufList_t **GenBufList);
long String2GenBuf_alloc(const char *String, GenBuf_t **GenBuf);

/*added by WSZ*/
long bmd_strsep(char *src, char sep, char*** dest, long *N);
long bmd_strsep_destroy(char*** src, long N);
long String2OID(char *str, OBJECT_IDENTIFIER_t *oid);
/*added by WSZ: end*/

long asn1_encode(void *td,void *src, GenBuf_t *twf_prefix, GenBuf_t **der);

#if defined(__cplusplus)
}
#endif
#endif /*_ASN1_COMMON_COMMONUTILS_INCLUDED_*/


