/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_ResponseData_H_
#define	_ResponseData_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Version.h>
#include <bmd/libbmdasn1_common/ResponderID.h>
#include <bmd/libbmdasn1_core/GeneralizedTime.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Extensions;
struct SingleResponse;

/* ResponseData */
typedef struct ResponseData {
	Version_t	*version	/* DEFAULT 0 */;
	ResponderID_t	 responderID;
	GeneralizedTime_t	 producedAt;
	struct responses {
		A_SEQUENCE_OF(struct SingleResponse) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} responses;
	struct Extensions	*responseExtensions	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ResponseData_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ResponseData;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/Extensions.h>
#include <bmd/libbmdasn1_common/SingleResponse.h>

#endif	/* _ResponseData_H_ */
