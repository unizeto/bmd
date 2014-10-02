/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/UnizetoAttrSyntax.h>

static asn_TYPE_member_t asn_MBR_slots_4[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Slot,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_slots_tags_4[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_slots_specs_4 = {
	sizeof(struct UnizetoAttrSyntax__slots),
	offsetof(struct UnizetoAttrSyntax__slots, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_slots_4 = {
	"slots",
	"slots",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_slots_tags_4,
	sizeof(asn_DEF_slots_tags_4)
		/sizeof(asn_DEF_slots_tags_4[0]), /* 1 */
	asn_DEF_slots_tags_4,	/* Same as above */
	sizeof(asn_DEF_slots_tags_4)
		/sizeof(asn_DEF_slots_tags_4[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_slots_4,
	1,	/* Single element */
	&asn_SPC_slots_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_UnizetoAttrSyntax_1[] = {
	{ ATF_POINTER, 1, offsetof(struct UnizetoAttrSyntax, policyAuthority),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"policyAuthority"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct UnizetoAttrSyntax, syntaxId),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"syntaxId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct UnizetoAttrSyntax, slots),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_slots_4,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"slots"
		},
};
static ber_tlv_tag_t asn_DEF_UnizetoAttrSyntax_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_UnizetoAttrSyntax_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 1, 0, 0 }, /* syntaxId at 68 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 1 }, /* policyAuthority at 67 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 2, -1, 0 } /* slots at 69 */
};
static asn_SEQUENCE_specifics_t asn_SPC_UnizetoAttrSyntax_specs_1 = {
	sizeof(struct UnizetoAttrSyntax),
	offsetof(struct UnizetoAttrSyntax, _asn_ctx),
	asn_MAP_UnizetoAttrSyntax_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_UnizetoAttrSyntax = {
	"UnizetoAttrSyntax",
	"UnizetoAttrSyntax",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_UnizetoAttrSyntax_tags_1,
	sizeof(asn_DEF_UnizetoAttrSyntax_tags_1)
		/sizeof(asn_DEF_UnizetoAttrSyntax_tags_1[0]), /* 1 */
	asn_DEF_UnizetoAttrSyntax_tags_1,	/* Same as above */
	sizeof(asn_DEF_UnizetoAttrSyntax_tags_1)
		/sizeof(asn_DEF_UnizetoAttrSyntax_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_UnizetoAttrSyntax_1,
	3,	/* Elements count */
	&asn_SPC_UnizetoAttrSyntax_specs_1	/* Additional specs */
};
