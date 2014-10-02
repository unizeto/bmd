/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/Target.h>

static asn_TYPE_member_t asn_MBR_Target_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Target, choice.targetName),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_GeneralName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"targetName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Target, choice.targetGroup),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_GeneralName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"targetGroup"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Target, choice.targetCert),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_TargetCert,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"targetCert"
		},
};
static asn_TYPE_tag2member_t asn_MAP_Target_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* targetName at 220 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* targetGroup at 221 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* targetCert at 223 */
};
static asn_CHOICE_specifics_t asn_SPC_Target_specs_1 = {
	sizeof(struct Target),
	offsetof(struct Target, _asn_ctx),
	offsetof(struct Target, present),
	sizeof(((struct Target *)0)->present),
	asn_MAP_Target_tag2el_1,
	3,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_Target = {
	"Target",
	"Target",
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
	asn_MBR_Target_1,
	3,	/* Elements count */
	&asn_SPC_Target_specs_1	/* Additional specs */
};

