/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_RecipientInfo_H_
#define	_RecipientInfo_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/KeyTransRecipientInfo.h>
#include <bmd/libbmdasn1_common/KeyAgreeRecipientInfo.h>
#include <bmd/libbmdasn1_common/KEKRecipientInfo.h>
#include <bmd/libbmdasn1_common/PasswordRecipientInfo.h>
#include <bmd/libbmdasn1_common/OtherRecipientInfo.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum RecipientInfo_PR {
	RecipientInfo_PR_NOTHING,	/* No components present */
	RecipientInfo_PR_ktri,
	RecipientInfo_PR_kari,
	RecipientInfo_PR_kekri,
	RecipientInfo_PR_pwri,
	RecipientInfo_PR_ori,
} RecipientInfo_PR;

/* RecipientInfo */
typedef struct RecipientInfo {
	RecipientInfo_PR present;
	union RecipientInfo_u {
		KeyTransRecipientInfo_t	 ktri;
		KeyAgreeRecipientInfo_t	 kari;
		KEKRecipientInfo_t	 kekri;
		PasswordRecipientInfo_t	 pwri;
		OtherRecipientInfo_t	 ori;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RecipientInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RecipientInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _RecipientInfo_H_ */