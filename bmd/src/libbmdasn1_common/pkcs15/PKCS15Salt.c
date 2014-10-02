/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "PKIX1Explicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15Salt.h>

static asn_TYPE_member_t asn_MBR_PKCS15Salt_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15Salt, choice.specified),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"specified"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15Salt, choice.otherSource),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"otherSource"
		},
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15Salt_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 0, 0, 0 }, /* specified at 343 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 } /* otherSource at 345 */
};
static asn_CHOICE_specifics_t asn_SPC_PKCS15Salt_specs_1 = {
	sizeof(struct PKCS15Salt),
	offsetof(struct PKCS15Salt, _asn_ctx),
	offsetof(struct PKCS15Salt, present),
	sizeof(((struct PKCS15Salt *)0)->present),
	asn_MAP_PKCS15Salt_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15Salt = {
	"PKCS15Salt",
	"PKCS15Salt",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_PKCS15Salt_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15Salt_specs_1	/* Additional specs */
};

