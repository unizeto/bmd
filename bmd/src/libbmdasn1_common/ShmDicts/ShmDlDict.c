/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ShmDictionaries"
 * 	found in "./dict_asn1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/ShmDicts/ShmDlDict.h>

static asn_TYPE_member_t asn_MBR_ShmDlDict_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlDict, dictName),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dictName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ShmDlDict, nodes),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ShmDl,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nodes"
		},
};
static ber_tlv_tag_t asn_DEF_ShmDlDict_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ShmDlDict_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 0, 0, 0 }, /* dictName at 44 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 } /* nodes at 46 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ShmDlDict_specs_1 = {
	sizeof(struct ShmDlDict),
	offsetof(struct ShmDlDict, _asn_ctx),
	asn_MAP_ShmDlDict_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ShmDlDict = {
	"ShmDlDict",
	"ShmDlDict",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ShmDlDict_tags_1,
	sizeof(asn_DEF_ShmDlDict_tags_1)
		/sizeof(asn_DEF_ShmDlDict_tags_1[0]), /* 1 */
	asn_DEF_ShmDlDict_tags_1,	/* Same as above */
	sizeof(asn_DEF_ShmDlDict_tags_1)
		/sizeof(asn_DEF_ShmDlDict_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ShmDlDict_1,
	2,	/* Elements count */
	&asn_SPC_ShmDlDict_specs_1	/* Additional specs */
};
