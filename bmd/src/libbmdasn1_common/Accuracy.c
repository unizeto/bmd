/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXTSP"
 * 	found in "tsp.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/Accuracy.h>

static int
memb_millis_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 1 && value <= 999)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_micros_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 1 && value <= 999)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_Accuracy_1[] = {
	{ ATF_POINTER, 3, offsetof(struct Accuracy, seconds),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"seconds"
		},
	{ ATF_POINTER, 2, offsetof(struct Accuracy, millis),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_millis_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"millis"
		},
	{ ATF_POINTER, 1, offsetof(struct Accuracy, micros),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_micros_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"micros"
		},
};
static ber_tlv_tag_t asn_DEF_Accuracy_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Accuracy_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 0 }, /* seconds at 154 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 }, /* millis at 155 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 2, 0, 0 } /* micros at 156 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Accuracy_specs_1 = {
	sizeof(struct Accuracy),
	offsetof(struct Accuracy, _asn_ctx),
	asn_MAP_Accuracy_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Accuracy = {
	"Accuracy",
	"Accuracy",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Accuracy_tags_1,
	sizeof(asn_DEF_Accuracy_tags_1)
		/sizeof(asn_DEF_Accuracy_tags_1[0]), /* 1 */
	asn_DEF_Accuracy_tags_1,	/* Same as above */
	sizeof(asn_DEF_Accuracy_tags_1)
		/sizeof(asn_DEF_Accuracy_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Accuracy_1,
	3,	/* Elements count */
	&asn_SPC_Accuracy_specs_1	/* Additional specs */
};

