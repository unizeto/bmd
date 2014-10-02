/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#ifndef	_DVCSRequestInformation_H_
#define	_DVCSRequestInformation_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_common/ServiceType.h>
#include <bmd/libbmdasn1_common/Nonce.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct DVCSTime;
struct GeneralNames;
struct PolicyInformation;
struct Extensions;

/* DVCSRequestInformation */
typedef struct DVCSRequestInformation {
	INTEGER_t	*version	/* DEFAULT 1 */;
	ServiceType_t	 service;
	Nonce_t	*nonce	/* OPTIONAL */;
	struct DVCSTime	*requestTime	/* OPTIONAL */;
	struct GeneralNames	*requester	/* OPTIONAL */;
	struct PolicyInformation	*requestPolicy	/* OPTIONAL */;
	struct GeneralNames	*dvcs	/* OPTIONAL */;
	struct GeneralNames	*dataLocations	/* OPTIONAL */;
	struct Extensions	*extensions	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DVCSRequestInformation_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DVCSRequestInformation;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/DVCSTime.h>
#include <bmd/libbmdasn1_common/GeneralNames.h>
#include <bmd/libbmdasn1_common/PolicyInformation.h>
#include <bmd/libbmdasn1_common/Extensions.h>

#endif	/* _DVCSRequestInformation_H_ */