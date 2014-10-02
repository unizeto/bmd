/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/AttributeValue.h>

int
AttributeValue_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_ANY.check_constraints;
	return td->check_constraints(td, sptr, app_errlog, app_key);
}

/*
 * This type is implemented using ANY,
 * so here we adjust the DEF accordingly.
 */
static void
AttributeValue_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_ANY.free_struct;
	td->print_struct   = asn_DEF_ANY.print_struct;
	td->ber_decoder    = asn_DEF_ANY.ber_decoder;
	td->der_encoder    = asn_DEF_ANY.der_encoder;
	td->xer_decoder    = asn_DEF_ANY.xer_decoder;
	td->xer_encoder    = asn_DEF_ANY.xer_encoder;
	td->uper_decoder   = asn_DEF_ANY.uper_decoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_ANY.per_constraints;
	td->elements       = asn_DEF_ANY.elements;
	td->elements_count = asn_DEF_ANY.elements_count;
	td->specifics      = asn_DEF_ANY.specifics;
}

void
AttributeValue_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
AttributeValue_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
AttributeValue_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
AttributeValue_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
AttributeValue_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
AttributeValue_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_TYPE_descriptor_t asn_DEF_AttributeValue = {
	"AttributeValue",
	"AttributeValue",
	AttributeValue_free,
	AttributeValue_print,
	AttributeValue_constraint,
	AttributeValue_decode_ber,
	AttributeValue_encode_der,
	AttributeValue_decode_xer,
	AttributeValue_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

