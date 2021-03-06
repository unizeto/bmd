/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/CertID.h>

static asn_TYPE_member_t asn_MBR_CertID_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CertID, hashAlgorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"hashAlgorithm"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertID, issuerNameHash),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"issuerNameHash"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertID, issuerKeyHash),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"issuerKeyHash"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CertID, serialNumber),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_CertificateSerialNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"serialNumber"
		},
};
static ber_tlv_tag_t asn_DEF_CertID_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CertID_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 3, 0, 0 }, /* serialNumber at 75 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 1, 0, 1 }, /* issuerNameHash at 73 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 2, -1, 0 }, /* issuerKeyHash at 74 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 } /* hashAlgorithm at 72 */
};
static asn_SEQUENCE_specifics_t asn_SPC_CertID_specs_1 = {
	sizeof(struct CertID),
	offsetof(struct CertID, _asn_ctx),
	asn_MAP_CertID_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CertID = {
	"CertID",
	"CertID",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CertID_tags_1,
	sizeof(asn_DEF_CertID_tags_1)
		/sizeof(asn_DEF_CertID_tags_1[0]), /* 1 */
	asn_DEF_CertID_tags_1,	/* Same as above */
	sizeof(asn_DEF_CertID_tags_1)
		/sizeof(asn_DEF_CertID_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_CertID_1,
	4,	/* Elements count */
	&asn_SPC_CertID_specs_1	/* Additional specs */
};

