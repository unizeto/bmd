/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15KeyManagementInfo.h>

static asn_TYPE_member_t asn_MBR_Member_2[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15KeyManagementInfo__Member, keyId),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_PKCS15Identifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PKCS15KeyManagementInfo__Member, keyInfo),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_PKCS15keyInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyInfo"
		},
};
static ber_tlv_tag_t asn_DEF_Member_tags_2[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Member_tag2el_2[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 0, 0, 0 }, /* keyId at 306 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 }, /* ktri at 148 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 }, /* passwordInfo at 303 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* kari at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 1, 0, 0 }, /* kekri at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 1, 0, 0 }, /* pwri at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 1, 0, 0 } /* ori at 152 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Member_specs_2 = {
	sizeof(struct PKCS15KeyManagementInfo__Member),
	offsetof(struct PKCS15KeyManagementInfo__Member, _asn_ctx),
	asn_MAP_Member_tag2el_2,
	7,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_Member_2 = {
	"SEQUENCE",
	"SEQUENCE",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Member_tags_2,
	sizeof(asn_DEF_Member_tags_2)
		/sizeof(asn_DEF_Member_tags_2[0]), /* 1 */
	asn_DEF_Member_tags_2,	/* Same as above */
	sizeof(asn_DEF_Member_tags_2)
		/sizeof(asn_DEF_Member_tags_2[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Member_2,
	2,	/* Elements count */
	&asn_SPC_Member_specs_2	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_PKCS15KeyManagementInfo_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Member_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_PKCS15KeyManagementInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_PKCS15KeyManagementInfo_specs_1 = {
	sizeof(struct PKCS15KeyManagementInfo),
	offsetof(struct PKCS15KeyManagementInfo, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15KeyManagementInfo = {
	"PKCS15KeyManagementInfo",
	"PKCS15KeyManagementInfo",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PKCS15KeyManagementInfo_tags_1,
	sizeof(asn_DEF_PKCS15KeyManagementInfo_tags_1)
		/sizeof(asn_DEF_PKCS15KeyManagementInfo_tags_1[0]), /* 1 */
	asn_DEF_PKCS15KeyManagementInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_PKCS15KeyManagementInfo_tags_1)
		/sizeof(asn_DEF_PKCS15KeyManagementInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_PKCS15KeyManagementInfo_1,
	1,	/* Single element */
	&asn_SPC_PKCS15KeyManagementInfo_specs_1	/* Additional specs */
};
