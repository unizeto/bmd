/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/Dss-Parms.h>

static asn_TYPE_member_t asn_MBR_Dss_Parms_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Dss_Parms, p),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"p"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Dss_Parms, q),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"q"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Dss_Parms, g),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"g"
		},
};
static ber_tlv_tag_t asn_DEF_Dss_Parms_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Dss_Parms_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 2 }, /* p at 60 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, -1, 1 }, /* q at 61 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 2, -2, 0 } /* g at 62 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Dss_Parms_specs_1 = {
	sizeof(struct Dss_Parms),
	offsetof(struct Dss_Parms, _asn_ctx),
	asn_MAP_Dss_Parms_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Dss_Parms = {
	"Dss-Parms",
	"Dss-Parms",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Dss_Parms_tags_1,
	sizeof(asn_DEF_Dss_Parms_tags_1)
		/sizeof(asn_DEF_Dss_Parms_tags_1[0]), /* 1 */
	asn_DEF_Dss_Parms_tags_1,	/* Same as above */
	sizeof(asn_DEF_Dss_Parms_tags_1)
		/sizeof(asn_DEF_Dss_Parms_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Dss_Parms_1,
	3,	/* Elements count */
	&asn_SPC_Dss_Parms_specs_1	/* Additional specs */
};

