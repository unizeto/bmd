/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SigPolicy/CommitmentType.h>

static asn_TYPE_member_t asn_MBR_CommitmentType_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CommitmentType, identifier),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_CommitmentTypeIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"identifier"
		},
	{ ATF_POINTER, 2, offsetof(struct CommitmentType, fieldOfApplication),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_FieldOfApplication,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"fieldOfApplication"
		},
	{ ATF_POINTER, 1, offsetof(struct CommitmentType, semantics),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_DirectoryString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"semantics"
		},
};
static ber_tlv_tag_t asn_DEF_CommitmentType_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CommitmentType_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 }, /* identifier at 120 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 }, /* fieldOfApplication at 121 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 2, 0, 0 } /* semantics at 122 */
};
static asn_SEQUENCE_specifics_t asn_SPC_CommitmentType_specs_1 = {
	sizeof(struct CommitmentType),
	offsetof(struct CommitmentType, _asn_ctx),
	asn_MAP_CommitmentType_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CommitmentType = {
	"CommitmentType",
	"CommitmentType",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CommitmentType_tags_1,
	sizeof(asn_DEF_CommitmentType_tags_1)
		/sizeof(asn_DEF_CommitmentType_tags_1[0]), /* 1 */
	asn_DEF_CommitmentType_tags_1,	/* Same as above */
	sizeof(asn_DEF_CommitmentType_tags_1)
		/sizeof(asn_DEF_CommitmentType_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_CommitmentType_1,
	3,	/* Elements count */
	&asn_SPC_CommitmentType_specs_1	/* Additional specs */
};

