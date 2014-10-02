/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/pkcs15/PKCS15Usage.h>

static int
memb_extKeyUsage_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, app_errlog, app_key);
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_extKeyUsage_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_extKeyUsage_tags_3[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_extKeyUsage_specs_3 = {
	sizeof(struct PKCS15Usage__extKeyUsage),
	offsetof(struct PKCS15Usage__extKeyUsage, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_extKeyUsage_3 = {
	"extKeyUsage",
	"extKeyUsage",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_extKeyUsage_tags_3,
	sizeof(asn_DEF_extKeyUsage_tags_3)
		/sizeof(asn_DEF_extKeyUsage_tags_3[0]), /* 1 */
	asn_DEF_extKeyUsage_tags_3,	/* Same as above */
	sizeof(asn_DEF_extKeyUsage_tags_3)
		/sizeof(asn_DEF_extKeyUsage_tags_3[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_extKeyUsage_3,
	1,	/* Single element */
	&asn_SPC_extKeyUsage_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_PKCS15Usage_1[] = {
	{ ATF_POINTER, 2, offsetof(struct PKCS15Usage, keyUsage),
		(ASN_TAG_CLASS_UNIVERSAL | (3 << 2)),
		0,
		&asn_DEF_KeyUsage,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyUsage"
		},
	{ ATF_POINTER, 1, offsetof(struct PKCS15Usage, extKeyUsage),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_extKeyUsage_3,
		memb_extKeyUsage_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extKeyUsage"
		},
};
static ber_tlv_tag_t asn_DEF_PKCS15Usage_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_PKCS15Usage_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (3 << 2)), 0, 0, 0 }, /* keyUsage at 176 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 } /* extKeyUsage at 177 */
};
static asn_SEQUENCE_specifics_t asn_SPC_PKCS15Usage_specs_1 = {
	sizeof(struct PKCS15Usage),
	offsetof(struct PKCS15Usage, _asn_ctx),
	asn_MAP_PKCS15Usage_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_PKCS15Usage = {
	"PKCS15Usage",
	"PKCS15Usage",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PKCS15Usage_tags_1,
	sizeof(asn_DEF_PKCS15Usage_tags_1)
		/sizeof(asn_DEF_PKCS15Usage_tags_1[0]), /* 1 */
	asn_DEF_PKCS15Usage_tags_1,	/* Same as above */
	sizeof(asn_DEF_PKCS15Usage_tags_1)
		/sizeof(asn_DEF_PKCS15Usage_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_PKCS15Usage_1,
	2,	/* Elements count */
	&asn_SPC_PKCS15Usage_specs_1	/* Additional specs */
};

