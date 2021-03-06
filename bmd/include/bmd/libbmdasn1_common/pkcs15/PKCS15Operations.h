/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15Operations_H_
#define	_PKCS15Operations_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/BIT_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PKCS15Operations {
	PKCS15Operations_compute_checksum	= 0,
	PKCS15Operations_compute_signature	= 1,
	PKCS15Operations_verify_checksum	= 2,
	PKCS15Operations_verify_signature	= 3,
	PKCS15Operations_encipher	= 4,
	PKCS15Operations_decipher	= 5,
	PKCS15Operations_hash	= 6,
	PKCS15Operations_generate_key	= 7
} PKCS15Operations_e;

/* PKCS15Operations */
typedef BIT_STRING_t	 PKCS15Operations_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15Operations;
asn_struct_free_f PKCS15Operations_free;
asn_struct_print_f PKCS15Operations_print;
asn_constr_check_f PKCS15Operations_constraint;
ber_type_decoder_f PKCS15Operations_decode_ber;
der_type_encoder_f PKCS15Operations_encode_der;
xer_type_decoder_f PKCS15Operations_decode_xer;
xer_type_encoder_f PKCS15Operations_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15Operations_H_ */
