/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/ResponderID.h>

static asn_TYPE_member_t asn_MBR_ResponderID_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ResponderID, choice.byName),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Name,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"byName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResponderID, choice.byKey),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_KeyHash,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"byKey"
		},
};
static asn_TYPE_tag2member_t asn_MAP_ResponderID_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 0, 0, 0 }, /* byName at 109 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 1, 0, 0 } /* byKey at 110 */
};
static asn_CHOICE_specifics_t asn_SPC_ResponderID_specs_1 = {
	sizeof(struct ResponderID),
	offsetof(struct ResponderID, _asn_ctx),
	offsetof(struct ResponderID, present),
	sizeof(((struct ResponderID *)0)->present),
	asn_MAP_ResponderID_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_ResponderID = {
	"ResponderID",
	"ResponderID",
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
	asn_MBR_ResponderID_1,
	2,	/* Elements count */
	&asn_SPC_ResponderID_specs_1	/* Additional specs */
};

