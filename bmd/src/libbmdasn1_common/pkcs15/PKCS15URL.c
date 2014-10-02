/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15URL.h>

static asn_TYPE_member_t asn_MBR_PKCS15URL_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15URL, choice.url),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"url"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15URL, choice.urlWithDigest),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PKCS15urlWithDigest,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"urlWithDigest"
		},
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15URL_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 0, 0, 0 }, /* url at 81 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 1, 0, 0 } /* urlWithDigest at 83 */
};
static asn_CHOICE_specifics_t asn_SPC_PKCS15URL_specs_1 = {
	sizeof(struct PKCS15URL),
	offsetof(struct PKCS15URL, _asn_ctx),
	offsetof(struct PKCS15URL, present),
	sizeof(((struct PKCS15URL *)0)->present),
	asn_MAP_PKCS15URL_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15URL = {
	"PKCS15URL",
	"PKCS15URL",
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
	asn_MBR_PKCS15URL_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15URL_specs_1	/* Additional specs */
};
