/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#ifndef	_Data_H_
#define	_Data_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_common/DigestInfo.h>
#include <bmd/libbmdasn1_common/MessageInfo.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Data_PR {
	Data_PR_NOTHING,	/* No components present */
	Data_PR_message,
	Data_PR_messageImprint,
	Data_PR_certs1,
	Data_PR_messageInfo,
} Data_PR;

/* Forward declarations */
struct TargetEtcChain;

/* Data */
typedef struct Data {
	Data_PR present;
	union Data_u {
		OCTET_STRING_t	 message;
		DigestInfo_t	 messageImprint;
		struct certs1 {
			A_SEQUENCE_OF(struct TargetEtcChain) list;
			
			/* Context for parsing across buffer boundaries */
			asn_struct_ctx_t _asn_ctx;
		} certs1;
		MessageInfo_t	 messageInfo;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Data_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Data;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/TargetEtcChain.h>

#endif	/* _Data_H_ */
