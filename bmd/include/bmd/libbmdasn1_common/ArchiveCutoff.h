/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_ArchiveCutoff_H_
#define	_ArchiveCutoff_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/GeneralizedTime.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArchiveCutoff */
typedef GeneralizedTime_t	 ArchiveCutoff_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ArchiveCutoff;
asn_struct_free_f ArchiveCutoff_free;
asn_struct_print_f ArchiveCutoff_print;
asn_constr_check_f ArchiveCutoff_constraint;
ber_type_decoder_f ArchiveCutoff_decode_ber;
der_type_encoder_f ArchiveCutoff_encode_der;
xer_type_decoder_f ArchiveCutoff_decode_xer;
xer_type_encoder_f ArchiveCutoff_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _ArchiveCutoff_H_ */
