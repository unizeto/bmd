/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/TargetEtcChain.h>

static int
memb_chain_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, app_errlog, app_key);
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_chain_3[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_CertEtcToken,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_chain_tags_3[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_chain_specs_3 = {
	sizeof(struct chain),
	offsetof(struct chain, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_chain_3 = {
	"chain",
	"chain",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_chain_tags_3,
	sizeof(asn_DEF_chain_tags_3)
		/sizeof(asn_DEF_chain_tags_3[0]), /* 1 */
	asn_DEF_chain_tags_3,	/* Same as above */
	sizeof(asn_DEF_chain_tags_3)
		/sizeof(asn_DEF_chain_tags_3[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_chain_3,
	1,	/* Single element */
	&asn_SPC_chain_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_TargetEtcChain_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct TargetEtcChain, target),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_CertEtcToken,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"target"
		},
	{ ATF_POINTER, 2, offsetof(struct TargetEtcChain, chain),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_chain_3,
		memb_chain_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chain"
		},
	{ ATF_POINTER, 1, offsetof(struct TargetEtcChain, pathProcInput),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PathProcInput,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pathProcInput"
		},
};
static ber_tlv_tag_t asn_DEF_TargetEtcChain_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_TargetEtcChain_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 1 }, /* extension at 147 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, -1, 0 }, /* chain at 105 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 1 }, /* certificate at 137 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 2, -1, 0 }, /* pathProcInput at 106 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 0, 0, 0 }, /* esscertid at 138 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 0, 0, 0 }, /* pkistatus at 139 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 0, 0, 0 }, /* assertion at 140 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 0, 0, 0 }, /* crl at 141 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 0, 0, 0 }, /* ocspcertstatus at 142 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 0, 0, 0 }, /* oscpcertid at 143 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 0, 0, 0 }, /* oscpresponse at 144 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 0, 0, 0 } /* capabilities at 145 */
};
static asn_SEQUENCE_specifics_t asn_SPC_TargetEtcChain_specs_1 = {
	sizeof(struct TargetEtcChain),
	offsetof(struct TargetEtcChain, _asn_ctx),
	asn_MAP_TargetEtcChain_tag2el_1,
	12,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_TargetEtcChain = {
	"TargetEtcChain",
	"TargetEtcChain",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_TargetEtcChain_tags_1,
	sizeof(asn_DEF_TargetEtcChain_tags_1)
		/sizeof(asn_DEF_TargetEtcChain_tags_1[0]), /* 1 */
	asn_DEF_TargetEtcChain_tags_1,	/* Same as above */
	sizeof(asn_DEF_TargetEtcChain_tags_1)
		/sizeof(asn_DEF_TargetEtcChain_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_TargetEtcChain_1,
	3,	/* Elements count */
	&asn_SPC_TargetEtcChain_specs_1	/* Additional specs */
};

