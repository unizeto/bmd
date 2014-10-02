/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/PostalCode.h>

static int permitted_alphabet_table_2[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*                  */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*                  */
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/* .                */
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,	/* 0123456789       */
};

static int check_permitted_alphabet_2(const void *sptr) {
	int *table = permitted_alphabet_table_2;
	/* The underlying type is NumericString */
	const NumericString_t *st = (const NumericString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;
	
	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int permitted_alphabet_table_3[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*                  */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*                  */
1,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,	/* .      '() +,-./ */
1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,	/* 0123456789:  = ? */
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/*  ABCDEFGHIJKLMNO */
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,	/* PQRSTUVWXYZ      */
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/*  abcdefghijklmno */
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_3(const void *sptr) {
	int *table = permitted_alphabet_table_3;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;
	
	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int
memb_numeric_code_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	size = st->size;
	
	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_2(st)) {
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
memb_printable_code_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_consume_bytes_f *app_errlog, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;
	
	if(!sptr) {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	size = st->size;
	
	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_ERRLOG(app_errlog, app_key,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_PostalCode_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PostalCode, choice.numeric_code),
		(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),
		0,
		&asn_DEF_NumericString,
		memb_numeric_code_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numeric-code"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PostalCode, choice.printable_code),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_printable_code_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"printable-code"
		},
};
static asn_TYPE_tag2member_t asn_MAP_PostalCode_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (18 << 2)), 0, 0, 0 }, /* numeric-code at 539 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 } /* printable-code at 540 */
};
static asn_CHOICE_specifics_t asn_SPC_PostalCode_specs_1 = {
	sizeof(struct PostalCode),
	offsetof(struct PostalCode, _asn_ctx),
	offsetof(struct PostalCode, present),
	sizeof(((struct PostalCode *)0)->present),
	asn_MAP_PostalCode_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_PostalCode = {
	"PostalCode",
	"PostalCode",
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
	asn_MBR_PostalCode_1,
	2,	/* Elements count */
	&asn_SPC_PostalCode_specs_1	/* Additional specs */
};

