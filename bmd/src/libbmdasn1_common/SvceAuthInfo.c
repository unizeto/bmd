/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/SvceAuthInfo.h>

static asn_TYPE_member_t asn_MBR_SvceAuthInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SvceAuthInfo, service),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_GeneralName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"service"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SvceAuthInfo, ident),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_GeneralName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ident"
		},
	{ ATF_POINTER, 1, offsetof(struct SvceAuthInfo, authInfo),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"authInfo"
		},
};
static ber_tlv_tag_t asn_DEF_SvceAuthInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SvceAuthInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 2, 0, 0 }, /* authInfo at 243 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 1 }, /* otherName at 118 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, -1, 0 }, /* otherName at 118 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 0, 0, 1 }, /* rfc822Name at 119 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, -1, 0 }, /* rfc822Name at 119 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 0, 0, 1 }, /* dNSName at 120 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 1, -1, 0 }, /* dNSName at 120 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 0, 0, 1 }, /* directoryName at 122 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 1, -1, 0 }, /* directoryName at 122 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 0, 0, 1 }, /* ediPartyName at 123 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 1, -1, 0 }, /* ediPartyName at 123 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 0, 0, 1 }, /* uniformResourceIdentifier at 124 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 1, -1, 0 }, /* uniformResourceIdentifier at 124 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 0, 0, 1 }, /* iPAddress at 125 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 1, -1, 0 }, /* iPAddress at 125 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 0, 0, 1 }, /* registeredID at 126 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 1, -1, 0 } /* registeredID at 126 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SvceAuthInfo_specs_1 = {
	sizeof(struct SvceAuthInfo),
	offsetof(struct SvceAuthInfo, _asn_ctx),
	asn_MAP_SvceAuthInfo_tag2el_1,
	17,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SvceAuthInfo = {
	"SvceAuthInfo",
	"SvceAuthInfo",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SvceAuthInfo_tags_1,
	sizeof(asn_DEF_SvceAuthInfo_tags_1)
		/sizeof(asn_DEF_SvceAuthInfo_tags_1[0]), /* 1 */
	asn_DEF_SvceAuthInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_SvceAuthInfo_tags_1)
		/sizeof(asn_DEF_SvceAuthInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SvceAuthInfo_1,
	3,	/* Elements count */
	&asn_SPC_SvceAuthInfo_specs_1	/* Additional specs */
};

