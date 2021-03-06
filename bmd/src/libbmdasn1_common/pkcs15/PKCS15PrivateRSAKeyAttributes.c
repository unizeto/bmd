/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15PrivateRSAKeyAttributes.h>

static asn_TYPE_member_t asn_MBR_PKCS15PrivateRSAKeyAttributes_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15PrivateRSAKeyAttributes, value),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_PKCS15RSAPrivateKeyObjectValue,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"value"
		},
	{ ATF_POINTER, 1, offsetof(struct PKCS15PrivateRSAKeyAttributes, keyInfo),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_PKCS15RSAPrivateKeyInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyInfo"
		},
};
static ber_tlv_tag_t asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15PrivateRSAKeyAttributes_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, 0, 0 }, /* reference at 209 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 1 }, /* value at 219 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, -1, 0 } /* paramsAndOps at 211 */
};
static asn_SEQUENCE_specifics_t asn_SPC_PKCS15PrivateRSAKeyAttributes_specs_1 = {
	sizeof(struct PKCS15PrivateRSAKeyAttributes),
	offsetof(struct PKCS15PrivateRSAKeyAttributes, _asn_ctx),
	asn_MAP_PKCS15PrivateRSAKeyAttributes_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15PrivateRSAKeyAttributes = {
	"PKCS15PrivateRSAKeyAttributes",
	"PKCS15PrivateRSAKeyAttributes",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1,
	sizeof(asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1)
		/sizeof(asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1[0]), /* 1 */
	asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1,	/* Same as above */
	sizeof(asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1)
		/sizeof(asn_DEF_PKCS15PrivateRSAKeyAttributes_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_PKCS15PrivateRSAKeyAttributes_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15PrivateRSAKeyAttributes_specs_1	/* Additional specs */
};

