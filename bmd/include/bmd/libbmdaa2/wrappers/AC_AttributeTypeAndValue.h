#ifndef _AC_AttributeTypeAndValue_INCLUDED_
#define _AC_AttributeTypeAndValue_INCLUDED_

#include <bmd/libbmdasn1_common/AttributeTypeAndValue.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#define OID_STR_COMMONNAME			"2.5.4.3"
#define OID_STR_SURNAME				"2.5.4.4"
#define OID_STR_SERIALNUMBER			"2.5.4.5"
#define OID_STR_COUNTRY				"2.5.4.6"
#define OID_STR_LOCALITY			"2.5.4.7"
#define OID_STR_STATEORPROVINCE			"2.5.4.8"
#define OID_STR_ORGANIZATION			"2.5.4.10"
#define OID_STR_ORGANIZATIONALUNIT		"2.5.4.11"
#define OID_STR_TITLE				"2.5.4.12"
#define OID_STR_DESCRIPTION			"2.5.4.13"
#define OID_STR_POSTALADDRESS			"2.5.4.16"
#define OID_STR_GIVENNAME			"2.5.4.42"
#define OID_STR_GENERATIONQUALIFIER		"2.5.4.43"
#define OID_STR_DISTINGUISHEDNAMEQUALIFIER	"2.5.4.46"
#define OID_STR_PSEUDONYM			"2.5.4.65"
#define OID_STR_EMAILADDRESS			"1.2.840.113549.1.9.1"

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_CreateAttributeTypeAndValueFromString(DNValueType_t DNValueType, 
				const char *DNValue, 
				AttributeTypeAndValue_t **AttributeTypeAndValue);
	int AC_AttributeTypeAndValue_init(AttributeTypeAndValue_t *AttributeTypeAndValue);
		
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_AttributeTypeAndValue_INCLUDED_*/

