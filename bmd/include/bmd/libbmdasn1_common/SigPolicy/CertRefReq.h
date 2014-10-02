/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_CertRefReq_H_
#define	_CertRefReq_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CertRefReq {
	CertRefReq_signerOnly	= 1,
	CertRefReq_fullPath	= 2
} CertRefReq_e;

/* CertRefReq */
typedef ENUMERATED_t	 CertRefReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CertRefReq;
asn_struct_free_f CertRefReq_free;
asn_struct_print_f CertRefReq_print;
asn_constr_check_f CertRefReq_constraint;
ber_type_decoder_f CertRefReq_decode_ber;
der_type_encoder_f CertRefReq_encode_der;
xer_type_decoder_f CertRefReq_decode_xer;
xer_type_encoder_f CertRefReq_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _CertRefReq_H_ */
