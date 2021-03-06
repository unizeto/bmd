/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/CertEtcToken.h>

static asn_TYPE_member_t asn_MBR_CertEtcToken_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.certificate),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Certificate,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"certificate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.esscertid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ESSCertID,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"esscertid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.pkistatus),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PKIStatusInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pkistatus"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.assertion),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ContentInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"assertion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.crl),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_CertificateList,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"crl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.ocspcertstatus),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CertStatus,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ocspcertstatus"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.oscpcertid),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_CertID,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"oscpcertid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.oscpresponse),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCSPResponse,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"oscpresponse"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.capabilities),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SMIMECapabilities,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"capabilities"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertEtcToken, choice.extension),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Extension,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension"
		},
};
static asn_TYPE_tag2member_t asn_MAP_CertEtcToken_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 9, 0, 0 }, /* extension at 147 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* certificate at 137 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* esscertid at 138 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* pkistatus at 139 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* assertion at 140 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* crl at 141 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* ocspcertstatus at 142 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* oscpcertid at 143 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* oscpresponse at 144 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 } /* capabilities at 145 */
};
static asn_CHOICE_specifics_t asn_SPC_CertEtcToken_specs_1 = {
	sizeof(struct CertEtcToken),
	offsetof(struct CertEtcToken, _asn_ctx),
	offsetof(struct CertEtcToken, present),
	sizeof(((struct CertEtcToken *)0)->present),
	asn_MAP_CertEtcToken_tag2el_1,
	10,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_CertEtcToken = {
	"CertEtcToken",
	"CertEtcToken",
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
	asn_MBR_CertEtcToken_1,
	10,	/* Elements count */
	&asn_SPC_CertEtcToken_specs_1	/* Additional specs */
};

