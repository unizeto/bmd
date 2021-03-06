/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/RevocationInfoChoices.h>

static asn_TYPE_member_t asn_MBR_RevocationInfoChoices_1[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_RevocationInfoChoice,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_RevocationInfoChoices_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (17 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_RevocationInfoChoices_specs_1 = {
	sizeof(struct RevocationInfoChoices),
	offsetof(struct RevocationInfoChoices, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
asn_TYPE_descriptor_t asn_DEF_RevocationInfoChoices = {
	"RevocationInfoChoices",
	"RevocationInfoChoices",
	SET_OF_free,
	SET_OF_print,
	SET_OF_constraint,
	SET_OF_decode_ber,
	SET_OF_encode_der,
	SET_OF_decode_xer,
	SET_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RevocationInfoChoices_tags_1,
	sizeof(asn_DEF_RevocationInfoChoices_tags_1)
		/sizeof(asn_DEF_RevocationInfoChoices_tags_1[0]), /* 1 */
	asn_DEF_RevocationInfoChoices_tags_1,	/* Same as above */
	sizeof(asn_DEF_RevocationInfoChoices_tags_1)
		/sizeof(asn_DEF_RevocationInfoChoices_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_RevocationInfoChoices_1,
	1,	/* Single element */
	&asn_SPC_RevocationInfoChoices_specs_1	/* Additional specs */
};

