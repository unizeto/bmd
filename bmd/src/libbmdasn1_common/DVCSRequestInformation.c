/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/DVCSRequestInformation.h>

static int asn_DFL_2_set_1(void **sptr) {
	INTEGER_t *st = *sptr;
	
	if(!st) {
		st = (*sptr = CALLOC(1, sizeof(*st)));
		if(!st) return -1;
	}
	
	/* Install default value 1 */
	return asn_long2INTEGER(st, 1);
}
static asn_TYPE_member_t asn_MBR_DVCSRequestInformation_1[] = {
	{ ATF_POINTER, 1, offsetof(struct DVCSRequestInformation, version),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		asn_DFL_2_set_1,	/* DEFAULT 1 */
		"version"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DVCSRequestInformation, service),
		(ASN_TAG_CLASS_UNIVERSAL | (10 << 2)),
		0,
		&asn_DEF_ServiceType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"service"
		},
	{ ATF_POINTER, 7, offsetof(struct DVCSRequestInformation, nonce),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_Nonce,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nonce"
		},
	{ ATF_POINTER, 6, offsetof(struct DVCSRequestInformation, requestTime),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_DVCSTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"requestTime"
		},
	{ ATF_POINTER, 5, offsetof(struct DVCSRequestInformation, requester),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"requester"
		},
	{ ATF_POINTER, 4, offsetof(struct DVCSRequestInformation, requestPolicy),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PolicyInformation,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"requestPolicy"
		},
	{ ATF_POINTER, 3, offsetof(struct DVCSRequestInformation, dvcs),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dvcs"
		},
	{ ATF_POINTER, 2, offsetof(struct DVCSRequestInformation, dataLocations),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dataLocations"
		},
	{ ATF_POINTER, 1, offsetof(struct DVCSRequestInformation, extensions),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Extensions,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extensions"
		},
};
static ber_tlv_tag_t asn_DEF_DVCSRequestInformation_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DVCSRequestInformation_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 1 }, /* version at 150 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 2, -1, 0 }, /* nonce at 152 */
    { (ASN_TAG_CLASS_UNIVERSAL | (10 << 2)), 1, 0, 0 }, /* service at 151 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 3, 0, 0 }, /* timeStampToken at 101 */
    { (ASN_TAG_CLASS_UNIVERSAL | (24 << 2)), 3, 0, 0 }, /* genTime at 99 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 4, 0, 0 }, /* requester at 154 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 5, 0, 0 }, /* requestPolicy at 155 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 6, 0, 0 }, /* dvcs at 156 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 7, 0, 0 }, /* dataLocations at 157 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 8, 0, 0 } /* extensions at 158 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DVCSRequestInformation_specs_1 = {
	sizeof(struct DVCSRequestInformation),
	offsetof(struct DVCSRequestInformation, _asn_ctx),
	asn_MAP_DVCSRequestInformation_tag2el_1,
	10,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DVCSRequestInformation = {
	"DVCSRequestInformation",
	"DVCSRequestInformation",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DVCSRequestInformation_tags_1,
	sizeof(asn_DEF_DVCSRequestInformation_tags_1)
		/sizeof(asn_DEF_DVCSRequestInformation_tags_1[0]), /* 1 */
	asn_DEF_DVCSRequestInformation_tags_1,	/* Same as above */
	sizeof(asn_DEF_DVCSRequestInformation_tags_1)
		/sizeof(asn_DEF_DVCSRequestInformation_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DVCSRequestInformation_1,
	9,	/* Elements count */
	&asn_SPC_DVCSRequestInformation_specs_1	/* Additional specs */
};
