/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/RecipientInfo.h>

static asn_TYPE_member_t asn_MBR_RecipientInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct RecipientInfo, choice.ktri),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_KeyTransRecipientInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ktri"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RecipientInfo, choice.kari),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_KeyAgreeRecipientInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"kari"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RecipientInfo, choice.kekri),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_KEKRecipientInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"kekri"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RecipientInfo, choice.pwri),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PasswordRecipientInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pwri"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RecipientInfo, choice.ori),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OtherRecipientInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ori"
		},
};
static asn_TYPE_tag2member_t asn_MAP_RecipientInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 }, /* ktri at 148 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* kari at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* kekri at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* pwri at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* ori at 152 */
};
static asn_CHOICE_specifics_t asn_SPC_RecipientInfo_specs_1 = {
	sizeof(struct RecipientInfo),
	offsetof(struct RecipientInfo, _asn_ctx),
	offsetof(struct RecipientInfo, present),
	sizeof(((struct RecipientInfo *)0)->present),
	asn_MAP_RecipientInfo_tag2el_1,
	5,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_RecipientInfo = {
	"RecipientInfo",
	"RecipientInfo",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_RecipientInfo_1,
	5,	/* Elements count */
	&asn_SPC_RecipientInfo_specs_1	/* Additional specs */
};
