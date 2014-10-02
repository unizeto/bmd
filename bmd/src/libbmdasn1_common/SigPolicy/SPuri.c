/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "SigPolicyID"
 * 	found in "ws.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SigPolicy/SPuri.h>

int
SPuri_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_IA5String.check_constraints;
	return td->check_constraints(td, sptr, app_errlog, app_key);
}

/*
 * This type is implemented using IA5String,
 * so here we adjust the DEF accordingly.
 */
static void
SPuri_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_IA5String.free_struct;
	td->print_struct   = asn_DEF_IA5String.print_struct;
	td->ber_decoder    = asn_DEF_IA5String.ber_decoder;
	td->der_encoder    = asn_DEF_IA5String.der_encoder;
	td->xer_decoder    = asn_DEF_IA5String.xer_decoder;
	td->xer_encoder    = asn_DEF_IA5String.xer_encoder;
	td->uper_decoder   = asn_DEF_IA5String.uper_decoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_IA5String.per_constraints;
	td->elements       = asn_DEF_IA5String.elements;
	td->elements_count = asn_DEF_IA5String.elements_count;
	td->specifics      = asn_DEF_IA5String.specifics;
}

void
SPuri_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	SPuri_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
SPuri_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	SPuri_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
SPuri_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	SPuri_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
SPuri_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	SPuri_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
SPuri_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	SPuri_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
SPuri_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	SPuri_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_SPuri_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (22 << 2))
};
asn_TYPE_descriptor_t asn_DEF_SPuri = {
	"SPuri",
	"SPuri",
	SPuri_free,
	SPuri_print,
	SPuri_constraint,
	SPuri_decode_ber,
	SPuri_encode_der,
	SPuri_decode_xer,
	SPuri_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SPuri_tags_1,
	sizeof(asn_DEF_SPuri_tags_1)
		/sizeof(asn_DEF_SPuri_tags_1[0]), /* 1 */
	asn_DEF_SPuri_tags_1,	/* Same as above */
	sizeof(asn_DEF_SPuri_tags_1)
		/sizeof(asn_DEF_SPuri_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};
