/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ShmDictionaries"
 * 	found in "./dict_asn1.asn1"
 */

#ifndef	_ShmRoleNodeDescr_H_
#define	_ShmRoleNodeDescr_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShmRoleNodeDescr__roleActions {
	A_SEQUENCE_OF(OCTET_STRING_t) list;
	asn_struct_ctx_t _asn_ctx;
} ShmRoleNodeDescr__roleActions_t;

/* ShmRoleNodeDescr */
typedef struct ShmRoleNodeDescr {
	OCTET_STRING_t	 roleId;
	OCTET_STRING_t	 roleName;
	ShmRoleNodeDescr__roleActions_t roleActions;
	INTEGER_t	 actionsCount;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ShmRoleNodeDescr_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ShmRoleNodeDescr;

#ifdef __cplusplus
}
#endif

#endif	/* _ShmRoleNodeDescr_H_ */