/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXTSP"
 * 	found in "tsp.asn1"
 */

#ifndef	_PKIStatusInfo_H_
#define	_PKIStatusInfo_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/PKIStatus.h>
#include <bmd/libbmdasn1_common/PKIFailureInfo.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct PKIFreeText;

/* PKIStatusInfo */
typedef struct PKIStatusInfo {
	PKIStatus_t	 status;
	struct PKIFreeText	*statusString	/* OPTIONAL */;
	PKIFailureInfo_t	*failInfo	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKIStatusInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKIStatusInfo;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/PKIFreeText.h>

#endif	/* _PKIStatusInfo_H_ */
