/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_BuiltInStandardAttributes_H_
#define	_BuiltInStandardAttributes_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/NetworkAddress.h>
#include <bmd/libbmdasn1_common/TerminalIdentifier.h>
#include <bmd/libbmdasn1_common/OrganizationName.h>
#include <bmd/libbmdasn1_common/NumericUserIdentifier.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CountryName;
struct AdministrationDomainName;
struct PrivateDomainName;
struct PersonalName;
struct OrganizationalUnitNames;

/* BuiltInStandardAttributes */
typedef struct BuiltInStandardAttributes {
	struct CountryName	*country_name	/* OPTIONAL */;
	struct AdministrationDomainName	*administration_domain_name	/* OPTIONAL */;
	NetworkAddress_t	*network_address	/* OPTIONAL */;
	TerminalIdentifier_t	*terminal_identifier	/* OPTIONAL */;
	struct PrivateDomainName	*private_domain_name	/* OPTIONAL */;
	OrganizationName_t	*organization_name	/* OPTIONAL */;
	NumericUserIdentifier_t	*numeric_user_identifier	/* OPTIONAL */;
	struct PersonalName	*personal_name	/* OPTIONAL */;
	struct OrganizationalUnitNames	*organizational_unit_names	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BuiltInStandardAttributes_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BuiltInStandardAttributes;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/CountryName.h>
#include <bmd/libbmdasn1_common/AdministrationDomainName.h>
#include <bmd/libbmdasn1_common/PrivateDomainName.h>
#include <bmd/libbmdasn1_common/PersonalName.h>
#include <bmd/libbmdasn1_common/OrganizationalUnitNames.h>

#endif	/* _BuiltInStandardAttributes_H_ */
