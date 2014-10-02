/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_TBSRequest_H_
#define	_TBSRequest_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Version.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct GeneralName;
struct Extensions;
struct Request;

/* TBSRequest */
typedef struct TBSRequest {
	Version_t	*version	/* DEFAULT 0 */;
	struct GeneralName	*requestorName	/* OPTIONAL */;
	struct requestList {
		A_SEQUENCE_OF(struct Request) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} requestList;
	struct Extensions	*requestExtensions	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} TBSRequest_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_TBSRequest;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/GeneralName.h>
#include <bmd/libbmdasn1_common/Extensions.h>
#include <bmd/libbmdasn1_common/Request.h>

#endif	/* _TBSRequest_H_ */