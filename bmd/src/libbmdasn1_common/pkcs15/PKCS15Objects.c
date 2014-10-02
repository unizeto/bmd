/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15Objects.h>

static asn_TYPE_member_t asn_MBR_PKCS15Objects_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15Objects, choice.privateKeys),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_PKCS15PrivateKeys,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"privateKeys"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15Objects, choice.certificates),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_PKCS15Certificates,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"certificates"
		},
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15Objects_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* privateKeys at 181 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 1, 0, 0 } /* certificates at 183 */
};
static asn_CHOICE_specifics_t asn_SPC_PKCS15Objects_specs_1 = {
	sizeof(struct PKCS15Objects),
	offsetof(struct PKCS15Objects, _asn_ctx),
	offsetof(struct PKCS15Objects, present),
	sizeof(((struct PKCS15Objects *)0)->present),
	asn_MAP_PKCS15Objects_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15Objects = {
	"PKCS15Objects",
	"PKCS15Objects",
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
	asn_MBR_PKCS15Objects_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15Objects_specs_1	/* Additional specs */
};
