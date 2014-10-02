/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "BMDGroup"
 * 	found in "bmdGroup.asn1"
 */

#ifndef	_BMDGroup_H_
#define	_BMDGroup_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct BMDGroupPairList;

typedef struct BMDGroup__levels {
	A_SEQUENCE_OF(struct BMDGroupPairList) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BMDGroup__levels_t;

/* BMDGroup */
typedef struct BMDGroup {
	OCTET_STRING_t	 groupName;
	OCTET_STRING_t	 groupNameOID;
	BMDGroup__levels_t *levels;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BMDGroup_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BMDGroup;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/BMDGroupPairList.h>

#endif	/* _BMDGroup_H_ */
