/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_CommitmentType_H_
#define	_CommitmentType_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/SigPolicy/CommitmentTypeIdentifier.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct DirectoryString;

/* CommitmentType */
typedef struct CommitmentType {
	CommitmentTypeIdentifier_t	 identifier;
	struct DirectoryString	*fieldOfApplication	/* OPTIONAL */;
	struct DirectoryString	*semantics	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CommitmentType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CommitmentType;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/SigPolicy/FieldOfApplication.h>
#include <bmd/libbmdasn1_common/DirectoryString.h>

#endif	/* _CommitmentType_H_ */