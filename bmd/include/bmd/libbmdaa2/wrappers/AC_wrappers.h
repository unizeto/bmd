#ifndef _AC_WRAPPERS_INCLUDED_
#define _AC_WRAPPERS_INCLUDED_

#define ERR_AA_TIMEERR					-400
#define ERR_AA_DER_ENCODE_FAILED			-401
#define ERR_AA_LOADING_CERTIFICATE			-402
#define ERR_AA_LOADING_PRIVKEY				-403
#define ERR_AA_DIGEST_FAILED				-404
#define ERR_AA_OPEN_FILE				-405
#define ERR_AA_READ_FILE				-405
#define ERR_AA_ASN1_SET_ADD				-406
#define ERR_AA_UNRECOGNIZED_DN_TYPE			-407
#define ERR_AA_FUNCTION_NOT_IMPLEMENTED			-408
#define ERR_AA_UNRECOGNIZED_GENERALNAME_TYPE		-409
#define ERR_AA_UNRECOGNIZED_NAME_TYPE			-410
#define ERR_AA_CREATE_OCTET_STRING_FROM_GROUP		-411
#define ERR_AA_WRONG_GENBUF_WITH_OID_STRING		-412
#define ERR_AA_CREATE_OBJECT_IDENTIFIER_FROM_GROUP	-413
#define ERR_AA_CREATE_UTF8STRING_FROM_GROUP		-414
#define ERR_AA_IETFATTRIBUTEVALUE_NOT_COMPATIBLE	-415
#define ERR_AA_UNSUPPORTED_GROUP_TYPE			-416
#define ERR_AA_ATTRIBUTESDB_RECORD_NOT_FOUND		-417
#define ERR_AA_ATTRIBUTESDB_GROUPS_NOT_FOUND		-418
#define ERR_AA_ATTRIBUTESDB_ROLES_NOT_FOUND		-419
#define ERR_AA_ATTRIBUTESDB_CLEARANCES_NOT_FOUND	-420
#define ERR_AA_ATTRIBUTESDB_UNIZETOS_NOT_FOUND		-421
#define ERR_AA_ATTRIBUTESDB_UNIZETO_SLOTS_NOT_FOUND	-422
#define ERR_AA_ATTRIBUTESDB_UNIZETO_UDEF_SLOT_STX	-423
#define ERR_AA_READ_PEM_PRIVATE_KEY			-424
#define ERR_AA_OPENSSL_UNKNOWN_DIGEST			-425
#define ERR_AA_OPENSSL_EVP_INIT_STAGE_FAILED		-426
#define ERR_AA_OPENSSL_EVP_UPDATE_STAGE_FAILED		-427
#define ERR_AA_OPENSSL_EVP_FINAL_STAGE_FAILED		-428

typedef enum DNValueType {DN_VALUE_UNDEFINED,		/* nieznany element nazwy wyroznionej DN */
			DN_VALUE_COMMONNAME,
			DN_VALUE_SURNAME,
			DN_VALUE_SERIALNUMBER,
			DN_VALUE_COUNTRY,
			DN_VALUE_LOCALITY,
			DN_VALUE_STATEORPROVINCE,
			DN_VALUE_ORGANIZATION,
			DN_VALUE_ORGANIZATIONALUNIT,
			DN_VALUE_TITLE,
			DN_VALUE_DESCRIPTION,
			DN_VALUE_GIVENNAME,
			DN_VALUE_GENERATIONQUALIFIER,
			DN_VALUE_DISTINGUISHEDNAMEQUALIFIER,
			DN_VALUE_PSEUDONYM
			} DNValueType_t;

#ifndef WIN32
#include <bmd-common.h>
#else
#include <bmd/common/bmd-common.h>
#endif
#include <bmd/libbmdutils/libbmdutils.h>

#include <bmd/libbmdaa2/wrappers/AC_AlgorithmIdentifier.h>
#include <bmd/libbmdaa2/wrappers/AC_OBJECT_IDENTIFIER.h>
#include <bmd/libbmdaa2/wrappers/AC_OBJECT_IDENTIFIER.h>
#include <bmd/libbmdaa2/wrappers/AC_AttCertValidityPeriod.h>
#include <bmd/libbmdaa2/wrappers/AC_IA5String.h>
#include <bmd/libbmdaa2/wrappers/AC_RoleSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_IetfAttrSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_SecurityCategory.h>
#include <bmd/libbmdaa2/wrappers/AC_ClassList.h>
#include <bmd/libbmdaa2/wrappers/AC_Clearance.h>
#include <bmd/libbmdaa2/wrappers/AC_Slot.h>
#include <bmd/libbmdaa2/wrappers/AC_Attribute.h>
#include <bmd/libbmdaa2/wrappers/AC_GeneralizedTime.h>
#include <bmd/libbmdaa2/wrappers/AC_GeneralName.h>
#include <bmd/libbmdaa2/wrappers/AC_UnizetoAttrSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_AttributeTypeAndValue.h>
#include <bmd/libbmdaa2/wrappers/AC_RelativeDistinguishedName.h>
#include <bmd/libbmdaa2/wrappers/AC_RDNSequence.h>
#include <bmd/libbmdaa2/wrappers/AC_Name.h>
#include <bmd/libbmdaa2/wrappers/AC_SignatureValue.h>
#endif /*_AC_WRAPPERS_INCLUDED_*/

