/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15DigestInfoWithDefault.h>

static int
memb_digest_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	size = st->size;
	
	if((size >= 8 && size <= 128)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_PKCS15DigestInfoWithDefault_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15DigestInfoWithDefault, digestAlg),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"digestAlg"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15DigestInfoWithDefault, digest),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		memb_digest_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"digest"
		},
};
static ber_tlv_tag_t asn_DEF_PKCS15DigestInfoWithDefault_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15DigestInfoWithDefault_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 1, 0, 0 }, /* digest at 87 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 } /* digestAlg at 86 */
};
static asn_SEQUENCE_specifics_t asn_SPC_PKCS15DigestInfoWithDefault_specs_1 = {
	sizeof(struct PKCS15DigestInfoWithDefault),
	offsetof(struct PKCS15DigestInfoWithDefault, _asn_ctx),
	asn_MAP_PKCS15DigestInfoWithDefault_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15DigestInfoWithDefault = {
	"PKCS15DigestInfoWithDefault",
	"PKCS15DigestInfoWithDefault",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PKCS15DigestInfoWithDefault_tags_1,
	sizeof(asn_DEF_PKCS15DigestInfoWithDefault_tags_1)
		/sizeof(asn_DEF_PKCS15DigestInfoWithDefault_tags_1[0]), /* 1 */
	asn_DEF_PKCS15DigestInfoWithDefault_tags_1,	/* Same as above */
	sizeof(asn_DEF_PKCS15DigestInfoWithDefault_tags_1)
		/sizeof(asn_DEF_PKCS15DigestInfoWithDefault_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_PKCS15DigestInfoWithDefault_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15DigestInfoWithDefault_specs_1	/* Additional specs */
};
