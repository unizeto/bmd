/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ShmDictionaries"
 * 	found in "./dict_asn1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/ShmDicts/ShmDlNode.h>

static asn_TYPE_member_t asn_MBR_ShmDlNode_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlNode, i),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"i"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlNode, localizationId),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"localizationId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlNode, oidName),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"oidName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlNode, sqlCastString),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sqlCastString"
		},
};
static ber_tlv_tag_t asn_DEF_ShmDlNode_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ShmDlNode_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 0, 0, 3 }, /* i at 33 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 1, -1, 2 }, /* localizationId at 34 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 2, -2, 1 }, /* oidName at 35 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 3, -3, 0 } /* sqlCastString at 36 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ShmDlNode_specs_1 = {
	sizeof(struct ShmDlNode),
	offsetof(struct ShmDlNode, _asn_ctx),
	asn_MAP_ShmDlNode_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ShmDlNode = {
	"ShmDlNode",
	"ShmDlNode",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ShmDlNode_tags_1,
	sizeof(asn_DEF_ShmDlNode_tags_1)
		/sizeof(asn_DEF_ShmDlNode_tags_1[0]), /* 1 */
	asn_DEF_ShmDlNode_tags_1,	/* Same as above */
	sizeof(asn_DEF_ShmDlNode_tags_1)
		/sizeof(asn_DEF_ShmDlNode_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ShmDlNode_1,
	4,	/* Elements count */
	&asn_SPC_ShmDlNode_specs_1	/* Additional specs */
};
