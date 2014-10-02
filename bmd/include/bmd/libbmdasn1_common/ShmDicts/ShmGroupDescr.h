/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ShmDictionaries"
 * 	found in "./dict_asn1.asn1"
 */

#ifndef	_ShmGroupDescr_H_
#define	_ShmGroupDescr_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShmGroupDescr__depGroups {
	A_SEQUENCE_OF(OCTET_STRING_t) list;
		
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ShmGroupDescr__depGroups_t;

/* ShmGroupDescr */
typedef struct ShmGroupDescr {
	OCTET_STRING_t	 mainGroup;
	ShmGroupDescr__depGroups_t *depGroups;
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ShmGroupDescr_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ShmGroupDescr;

#ifdef __cplusplus
}
#endif

#endif	/* _ShmGroupDescr_H_ */
