/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15Label.h>

int
PKCS15Label_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	const UTF8String_t *st = (const UTF8String_t *)sptr;
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	size = UTF8String_length(st);
	if((ssize_t)size < 0) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: UTF-8: broken encoding (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	if((size <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using UTF8String,
 * so here we adjust the DEF accordingly.
 */
static void
PKCS15Label_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_UTF8String.free_struct;
	td->print_struct   = asn_DEF_UTF8String.print_struct;
	td->ber_decoder    = asn_DEF_UTF8String.ber_decoder;
	td->der_encoder    = asn_DEF_UTF8String.der_encoder;
	td->xer_decoder    = asn_DEF_UTF8String.xer_decoder;
	td->xer_encoder    = asn_DEF_UTF8String.xer_encoder;
	td->uper_decoder   = asn_DEF_UTF8String.uper_decoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_UTF8String.per_constraints;
	td->elements       = asn_DEF_UTF8String.elements;
	td->elements_count = asn_DEF_UTF8String.elements_count;
	td->specifics      = asn_DEF_UTF8String.specifics;
}

void
PKCS15Label_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
PKCS15Label_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
PKCS15Label_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
PKCS15Label_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
PKCS15Label_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
PKCS15Label_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	PKCS15Label_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_PKCS15Label_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (12 << 2))
};
asn_TYPE_descriptor_t asn_DEF_PKCS15Label = {
	"PKCS15Label",
	"PKCS15Label",
	PKCS15Label_free,
	PKCS15Label_print,
	PKCS15Label_constraint,
	PKCS15Label_decode_ber,
	PKCS15Label_encode_der,
	PKCS15Label_decode_xer,
	PKCS15Label_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PKCS15Label_tags_1,
	sizeof(asn_DEF_PKCS15Label_tags_1)
		/sizeof(asn_DEF_PKCS15Label_tags_1[0]), /* 1 */
	asn_DEF_PKCS15Label_tags_1,	/* Same as above */
	sizeof(asn_DEF_PKCS15Label_tags_1)
		/sizeof(asn_DEF_PKCS15Label_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

