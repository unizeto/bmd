/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "SecureMimeMessageV3"
 * 	found in "smimev3.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SMIMECapability.h>

static asn_TYPE_member_t asn_MBR_SMIMECapability_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SMIMECapability, capabilityID),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"capabilityID"
		},
	{ ATF_OPEN_TYPE | ATF_POINTER, 1, offsetof(struct SMIMECapability, parameters),
		-1 /* Ambiguous tag (ANY?) */,
		0,
		&asn_DEF_ANY,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"parameters"
		},
};
static ber_tlv_tag_t asn_DEF_SMIMECapability_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SMIMECapability_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 } /* capabilityID at 35 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SMIMECapability_specs_1 = {
	sizeof(struct SMIMECapability),
	offsetof(struct SMIMECapability, _asn_ctx),
	asn_MAP_SMIMECapability_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SMIMECapability = {
	"SMIMECapability",
	"SMIMECapability",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SMIMECapability_tags_1,
	sizeof(asn_DEF_SMIMECapability_tags_1)
		/sizeof(asn_DEF_SMIMECapability_tags_1[0]), /* 1 */
	asn_DEF_SMIMECapability_tags_1,	/* Same as above */
	sizeof(asn_DEF_SMIMECapability_tags_1)
		/sizeof(asn_DEF_SMIMECapability_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SMIMECapability_1,
	2,	/* Elements count */
	&asn_SPC_SMIMECapability_specs_1	/* Additional specs */
};

