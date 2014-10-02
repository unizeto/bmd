/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/DistributionPoint.h>

static asn_TYPE_member_t asn_MBR_DistributionPoint_1[] = {
	{ ATF_POINTER, 3, offsetof(struct DistributionPoint, distributionPoint),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_DistributionPointName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"distributionPoint"
		},
	{ ATF_POINTER, 2, offsetof(struct DistributionPoint, reasons),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ReasonFlags,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"reasons"
		},
	{ ATF_POINTER, 1, offsetof(struct DistributionPoint, cRLIssuer),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cRLIssuer"
		},
};
static ber_tlv_tag_t asn_DEF_DistributionPoint_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DistributionPoint_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* distributionPoint at 239 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* reasons at 240 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* cRLIssuer at 241 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DistributionPoint_specs_1 = {
	sizeof(struct DistributionPoint),
	offsetof(struct DistributionPoint, _asn_ctx),
	asn_MAP_DistributionPoint_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DistributionPoint = {
	"DistributionPoint",
	"DistributionPoint",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DistributionPoint_tags_1,
	sizeof(asn_DEF_DistributionPoint_tags_1)
		/sizeof(asn_DEF_DistributionPoint_tags_1[0]), /* 1 */
	asn_DEF_DistributionPoint_tags_1,	/* Same as above */
	sizeof(asn_DEF_DistributionPoint_tags_1)
		/sizeof(asn_DEF_DistributionPoint_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DistributionPoint_1,
	3,	/* Elements count */
	&asn_SPC_DistributionPoint_specs_1	/* Additional specs */
};

