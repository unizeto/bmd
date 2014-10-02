/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/ExtensionAttribute.h>

static int
memb_extension_attribute_type_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 256)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_ExtensionAttribute_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ExtensionAttribute, extension_attribute_type),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_extension_attribute_type_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension-attribute-type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ExtensionAttribute, extension_attribute_value),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ANY,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension-attribute-value"
		},
};
static ber_tlv_tag_t asn_DEF_ExtensionAttribute_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ExtensionAttribute_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* extension-attribute-type at 475 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* extension-attribute-value at 477 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ExtensionAttribute_specs_1 = {
	sizeof(struct ExtensionAttribute),
	offsetof(struct ExtensionAttribute, _asn_ctx),
	asn_MAP_ExtensionAttribute_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ExtensionAttribute = {
	"ExtensionAttribute",
	"ExtensionAttribute",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ExtensionAttribute_tags_1,
	sizeof(asn_DEF_ExtensionAttribute_tags_1)
		/sizeof(asn_DEF_ExtensionAttribute_tags_1[0]), /* 1 */
	asn_DEF_ExtensionAttribute_tags_1,	/* Same as above */
	sizeof(asn_DEF_ExtensionAttribute_tags_1)
		/sizeof(asn_DEF_ExtensionAttribute_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ExtensionAttribute_1,
	2,	/* Elements count */
	&asn_SPC_ExtensionAttribute_specs_1	/* Additional specs */
};

