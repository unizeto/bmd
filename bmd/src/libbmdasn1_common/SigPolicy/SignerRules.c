/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SigPolicy/SignerRules.h>

static int asn_DFL_5_set_1(void **sptr) {
	CertRefReq_t *st = *sptr;
	
	if(!st) {
		st = (*sptr = CALLOC(1, sizeof(*st)));
		if(!st) return -1;
	}
	
	/* Install default value 1 */
	return asn_long2INTEGER(st, 1);
}
static int asn_DFL_6_set_0(void **sptr) {
	CertInfoReq_t *st = *sptr;
	
	if(!st) {
		st = (*sptr = CALLOC(1, sizeof(*st)));
		if(!st) return -1;
	}
	
	/* Install default value 0 */
	return asn_long2INTEGER(st, 0);
}
static asn_TYPE_member_t asn_MBR_SignerRules_1[] = {
	{ ATF_POINTER, 1, offsetof(struct SignerRules, externalSignedData),
		(ASN_TAG_CLASS_UNIVERSAL | (1 << 2)),
		0,
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"externalSignedData"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerRules, mandatedSignedAttr),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_CMSAttrs,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mandatedSignedAttr"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SignerRules, mandatedUnsignedAttr),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_CMSAttrs,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mandatedUnsignedAttr"
		},
	{ ATF_POINTER, 3, offsetof(struct SignerRules, mandatedCertificateRef),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CertRefReq,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		asn_DFL_5_set_1,	/* DEFAULT 1 */
		"mandatedCertificateRef"
		},
	{ ATF_POINTER, 2, offsetof(struct SignerRules, mandatedCertificateInfo),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CertInfoReq,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		asn_DFL_6_set_0,	/* DEFAULT 0 */
		"mandatedCertificateInfo"
		},
	{ ATF_POINTER, 1, offsetof(struct SignerRules, signPolExtensions),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_SignPolExtensions,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signPolExtensions"
		},
};
static ber_tlv_tag_t asn_DEF_SignerRules_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SignerRules_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (1 << 2)), 0, 0, 0 }, /* externalSignedData at 137 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 1 }, /* mandatedSignedAttr at 141 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 2, -1, 0 }, /* mandatedUnsignedAttr at 143 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 3, 0, 0 }, /* mandatedCertificateRef at 145 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 4, 0, 0 }, /* mandatedCertificateInfo at 147 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 5, 0, 0 } /* signPolExtensions at 150 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SignerRules_specs_1 = {
	sizeof(struct SignerRules),
	offsetof(struct SignerRules, _asn_ctx),
	asn_MAP_SignerRules_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SignerRules = {
	"SignerRules",
	"SignerRules",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SignerRules_tags_1,
	sizeof(asn_DEF_SignerRules_tags_1)
		/sizeof(asn_DEF_SignerRules_tags_1[0]), /* 1 */
	asn_DEF_SignerRules_tags_1,	/* Same as above */
	sizeof(asn_DEF_SignerRules_tags_1)
		/sizeof(asn_DEF_SignerRules_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SignerRules_1,
	6,	/* Elements count */
	&asn_SPC_SignerRules_specs_1	/* Additional specs */
};

