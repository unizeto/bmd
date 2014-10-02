#ifndef _AC_client_INCLUDED_
#define _AC_client_INCLUDED_

#include<time.h>
#ifndef _WIN32
#include<unistd.h>
#endif /* ifndef _WIN32 */

#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdasn1_common/AttributeCertificate.h>
#include <bmd/libbmdasn1_common/IetfAttrSyntax.h>
#include <bmd/libbmdasn1_common/RoleSyntax.h>
#include <bmd/libbmdasn1_common/Clearance.h>
#include <bmd/libbmdasn1_common/UnizetoAttrSyntax.h>

#include <bmd/libbmdaa2/wrappers/AC_Clearance.h>
#include <bmd/libbmdaa2/db/common/AC_db_UnizetoAttrSyntax.h>

#include <bmd/libbmdutils/libbmdutils.h>


typedef enum ACParamType{AC_ATTR_UNDEFINED,		/* nieznana wersja atrybutu */
			AC_PARAM_VERSION,		/* wersja certyfikatu */
			AC_PARAM_HOLDER_SN,		/* numer seryjny cartyfikatu PKI wlasiciela AC */
			AC_PARAM_HOLDER_DN,		/* nazwa wyrozniona DN posiadacza cartyfikatu PKI bedacego wlascicielem AC */
			AC_PARAM_SN,			/* numer seryjny certyfikatu atrybutów */
			AC_PARAM_ISSUER_DN,		/* nazwa wyrozniona DN wystawcy certyfikatu AC */
			AC_PARAM_VALIDITY_NB,		/* Waznosc certyfikatu NotBefore */
			AC_PARAM_VALIDITY_NA,		/* Waznosc certyfikatu NotAfter */
			AC_PARAM_GROUP_POLICYAUTH,	/* Policy Authority atrybutu Grupa */
			AC_PARAM_GROUPS,		/* lista grup */
			AC_PARAM_ROLE_ROLEAUTH,		/* Role Authority dla atrybutu Rola */
			AC_PARAM_ROLES,			/* lista roli */
			AC_PARAM_CLEARANCE_POLICYID,	/* Oid polityki atrybutu Clearance */
			AC_PARAM_CLEARANCES,		/* lista clearances */
			AC_PARAM_UNIZETO_POLICYAUTH	/* PolicyAuthority dla atrybutu UnizetoAttrSyntax */
			} ACParamType_t;

#define ERR_AC_UNRECOGNIZED_PARAM_TYPE			-100
#define ERR_AC_VERSION_FIELD_NOT_PRESENT		-101
#define ERR_AC_CONVERT_INTEGER_T_TO_LONG		-102
#define ERR_AC_DECODE_CERTIFICATE_DATA			-103
#define ERR_AC_NO_BASE_CERTIFIACTE_ID_PRESENT		-104
#define ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING		-105
#define ERR_AC_CONVERT_GENERALIZEDTIME_T_TO_STRING	-106
#define ERR_AC_NO_V2FORM_ISSUERNAME_PRESENT		-107
#define ERR_AC_UNKNOWN_AC_ISSUER_FORMAT			-108
#define ERR_AC_ATTRIBUTE_NOT_FOUND_IN_AC		-109
#define ERR_AC_DECODE_IETFATTRSYNTAX			-110
#define ERR_AC_POLICYAUTHORITY_NOT_IN_GROUPS		-111
#define ERR_AC_GET_ATTRIBUTE_FROM_AC			-112
#define ERR_AC_DECODE_ROLESYNTAX			-113
#define ERR_AC_ROLEAUTHORITY_NOT_IN_ROLES		-114
#define ERR_AC_DECODE_CLEARANCE				-115
#define ERR_AC_SECURITY_CATEGORIES_NOT_IN_CLEARANCES	-116
#define ERR_AC_CLASSLIST_NOT_IN_CLEARANCES		-117
#define ERR_AC_CONVERT_CLEARANCE_T_TO_STRING		-118
#define ERR_AC_CONVERT_SECURITYCATEGORY_VALUE_TO_STRING	-119
#define ERR_AC_DECODE_UNIZETOATTRSYNTAX			-120
#define ERR_AC_POLICYAUTHORITY_NOT_IN_UNIZETOATTRSYNTAX	-121
#define ERR_AC_UNIZETO_UDEF_SLOT_STX			-122


#if defined(__cplusplus)
extern "C" {
#endif


/*
 * One Call API
 */
long ACGetParam(GenBuf_t *GenBuf, ACParamType_t ACParamType, char **AttributeString, long *AttributeLong);

/**
 * Easy API
 */
long GetVersionFromAC(GenBuf_t *GenBuf, long *version);
long GetHolderDNFromAC(GenBuf_t *GenBuf, char **holderDN);
long GetHolderSNFromAC(GenBuf_t *GenBuf, char **holderSN);
long GetSNFromAC(GenBuf_t *GenBuf, char **SN);
long GetSNFromAC(GenBuf_t *GenBuf, char **SN);
long GetIssuerDNFromAC(GenBuf_t *GenBuf, char **IssuerDN);
long GetValidityNBFromAC(GenBuf_t *GenBuf, char **ValidityNB);
long GetValidityNAFromAC(GenBuf_t *GenBuf, char **ValidityNA);
long GetAttrGroupPolicyAuthorityFromAC(GenBuf_t *GenBuf, char **AttrGroupPolicyAuthority);
long GetAttrGroupGroupsFromAC(GenBuf_t *GenBuf, char **Groups);
long GetAttrRoleRolesFromAC(GenBuf_t *GenBuf, char **Roles);
long GetAttrClearancePolicyIdFromAC(GenBuf_t *GenBuf, char **AttrClearancePolicyId);
long GetAttrUnizetoPolicyAuthorityFromAC(GenBuf_t *GenBuf, char **PolicyAuthority);
long GetAttrUnizetoUnizetosFromAC(GenBuf_t *GenBuf, UnizetoAttrSyntaxInList_t **UAS);


/**
 * Fast API
 */
long GetACStructFromAC(GenBuf_t *GenBuf, AttributeCertificate_t **AC);
long FreeACStruct(AttributeCertificate_t **AC);
/* Pola certyfikatu */
long GetVersionFromACStruct(AttributeCertificate_t *AC, long *version);
long GetHolderDNFromACStruct(AttributeCertificate_t *AC, char **holderDN);
long GetHolderSNFromACStruct(AttributeCertificate_t *AC, char **holderSN);
long GetSNFromACStruct(AttributeCertificate_t *AC, char **SN);
long GetIssuerDNFromACStruct(AttributeCertificate_t *AC, char **issuerDN);
long GetValidityNBFromACStruct(AttributeCertificate_t *AC, char **ValidityNB);
long GetValidityNAFromACStruct(AttributeCertificate_t *AC, char **ValidityNA);

/* Grupy */
long GetAttrGroupPolicyAuthorityFromACStruct(AttributeCertificate_t *AC, char **AttrGroupPolicyAuthority);
long GetAttrGroupGroupsFromACStruct(AttributeCertificate_t *AC, GenBufList_t **GenBufList);

/* Role */
long GetAttrRoleRoleAuthorityFromACStruct(AttributeCertificate_t *AC, char **AttrRoleRoleAuthority);
long GetAttrRoleRoleAuthorityFromAC(GenBuf_t *GenBuf, char **AttrRoleRoleAuthority);
long GetAttrRoleRolesFromACStruct(AttributeCertificate_t *AC, GenBufList_t **GenBufList);

/* Clearance */
long GetAttrClearancePolicyIdFromACStruct(AttributeCertificate_t *AC, char **PolicyId);
long GetAttrClearanceClearancesFromACStruct(AttributeCertificate_t *AC, GenBufList_t **GenBufList);
long GetAttrClearanceClearancesFromAC(GenBuf_t *GenBuf, char **Clearances);

/* UnizetoAttrSyntax */
long GetAttrUnizetoPolicyAuthorityFromACStruct(AttributeCertificate_t *AC, char **PolicyAuthority);
long GetAttrUnizetoUnizetosFromACStruct(AttributeCertificate_t *AC, UnizetoAttrSyntaxInList_t **UnizetoAttrSyntaxInList);
long UnizetoAttrSyntax2UnizetoAttrSyntaxIn(UnizetoAttrSyntax_t *UnizetoAttrSyntax, UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn);
long Slot2SlotIn(Slot_t *Slot, SlotIn_t **SlotIn);

/**
 * Utils
 */
long GetAttributeFromACStruct(	AttributeCertificate_t *AC, Attribute_t **Attribute, char *attribute_oid);
long GenBufList2string(GenBufList_t *GenBufList, char **string);

/** Funkcja wybiera z listy typu UnzietoAttributeSyntax pierwszy napotkany atrybut o wskazanym przez UASTypeOID OIDzie składni	*/
/** oraz wskazanym przez SlotOID typie slotów											*/
long UnizetoAttrSyntaxInList2GenBufList(UnizetoAttrSyntaxInList_t *UAS, GenBufList_t **GenBufList, char *UASTypeOID, char *SlotOID);
long GenBufList_clone(GenBufList_t *in, GenBufList_t **out);
long GenBuf_clone(GenBuf_t *in, GenBuf_t **out);


/** Do CommonUtils */

#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_client_INCLUDED_*/
