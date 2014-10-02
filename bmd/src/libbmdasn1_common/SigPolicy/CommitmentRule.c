/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SigPolicy/CommitmentRule.h>

static asn_TYPE_member_t asn_MBR_CommitmentRule_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CommitmentRule, selCommitmentTypes),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SelectedCommitmentTypes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"selCommitmentTypes"
		},
	{ ATF_POINTER, 6, offsetof(struct CommitmentRule, signerAndVeriferRules),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_SignerAndVerifierRules,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signerAndVeriferRules"
		},
	{ ATF_POINTER, 5, offsetof(struct CommitmentRule, signingCertTrustCondition),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_SigningCertTrustCondition,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signingCertTrustCondition"
		},
	{ ATF_POINTER, 4, offsetof(struct CommitmentRule, timeStampTrustCondition),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_TimestampTrustCondition,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"timeStampTrustCondition"
		},
	{ ATF_POINTER, 3, offsetof(struct CommitmentRule, attributeTrustCondition),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_AttributeTrustCondition,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"attributeTrustCondition"
		},
	{ ATF_POINTER, 2, offsetof(struct CommitmentRule, algorithmConstraintSet),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_AlgorithmConstraintSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"algorithmConstraintSet"
		},
	{ ATF_POINTER, 1, offsetof(struct CommitmentRule, signPolExtensions),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_SignPolExtensions,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signPolExtensions"
		},
};
static ber_tlv_tag_t asn_DEF_CommitmentRule_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CommitmentRule_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 }, /* selCommitmentTypes at 98 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 }, /* signerAndVeriferRules at 100 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 2, 0, 0 }, /* signingCertTrustCondition at 102 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 3, 0, 0 }, /* timeStampTrustCondition at 104 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 4, 0, 0 }, /* attributeTrustCondition at 109 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 5, 0, 0 }, /* algorithmConstraintSet at 111 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 6, 0, 0 } /* signPolExtensions at 113 */
};
static asn_SEQUENCE_specifics_t asn_SPC_CommitmentRule_specs_1 = {
	sizeof(struct CommitmentRule),
	offsetof(struct CommitmentRule, _asn_ctx),
	asn_MAP_CommitmentRule_tag2el_1,
	7,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CommitmentRule = {
	"CommitmentRule",
	"CommitmentRule",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CommitmentRule_tags_1,
	sizeof(asn_DEF_CommitmentRule_tags_1)
		/sizeof(asn_DEF_CommitmentRule_tags_1[0]), /* 1 */
	asn_DEF_CommitmentRule_tags_1,	/* Same as above */
	sizeof(asn_DEF_CommitmentRule_tags_1)
		/sizeof(asn_DEF_CommitmentRule_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_CommitmentRule_1,
	7,	/* Elements count */
	&asn_SPC_CommitmentRule_specs_1	/* Additional specs */
};

