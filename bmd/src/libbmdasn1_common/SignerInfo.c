/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SignerInfo.h>

static asn_TYPE_member_t asn_MBR_SignerInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SignerInfo, version),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_CMSVersion,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"version"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerInfo, sid),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_SignerIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerInfo, digestAlgorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_DigestAlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"digestAlgorithm"
		},
	{ ATF_POINTER, 1, offsetof(struct SignerInfo, signedAttrs),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SignedAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signedAttrs"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerInfo, signatureAlgorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SignatureAlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signatureAlgorithm"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerInfo, signature),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_SignatureValue,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signature"
		},
	{ ATF_POINTER, 1, offsetof(struct SignerInfo, unsignedAttrs),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UnsignedAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"unsignedAttrs"
		},
};
static ber_tlv_tag_t asn_DEF_SignerInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SignerInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 0 }, /* version at 85 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 5, 0, 0 }, /* signature at 90 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 2 }, /* issuerAndSerialNumber at 94 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 2, -1, 1 }, /* digestAlgorithm at 87 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 4, -2, 0 }, /* signatureAlgorithm at 89 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 1 }, /* subjectKeyIdentifier at 95 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 3, -1, 0 }, /* signedAttrs at 88 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 6, 0, 0 } /* unsignedAttrs at 91 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SignerInfo_specs_1 = {
	sizeof(struct SignerInfo),
	offsetof(struct SignerInfo, _asn_ctx),
	asn_MAP_SignerInfo_tag2el_1,
	8,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SignerInfo = {
	"SignerInfo",
	"SignerInfo",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SignerInfo_tags_1,
	sizeof(asn_DEF_SignerInfo_tags_1)
		/sizeof(asn_DEF_SignerInfo_tags_1[0]), /* 1 */
	asn_DEF_SignerInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_SignerInfo_tags_1)
		/sizeof(asn_DEF_SignerInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SignerInfo_1,
	7,	/* Elements count */
	&asn_SPC_SignerInfo_specs_1	/* Additional specs */
};

