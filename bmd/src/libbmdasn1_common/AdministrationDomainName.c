/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#include <bmd/libbmdasn1_core/asn_internal.h>

#include <bmd/libbmdasn1_common/AdministrationDomainName.h>

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
memb_numeric_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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
	
	if((size <= 16)
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
memb_printable_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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
	
	if((size <= 16)
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

static asn_TYPE_member_t asn_MBR_AdministrationDomainName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AdministrationDomainName, choice.numeric),
		(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),
		0,
		&asn_DEF_NumericString,
		memb_numeric_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numeric"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AdministrationDomainName, choice.printable),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_printable_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"printable"
		},
};
static ber_tlv_tag_t asn_DEF_AdministrationDomainName_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (2 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AdministrationDomainName_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (18 << 2)), 0, 0, 0 }, /* numeric at 410 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 } /* printable at 411 */
};
static asn_CHOICE_specifics_t asn_SPC_AdministrationDomainName_specs_1 = {
	sizeof(struct AdministrationDomainName),
	offsetof(struct AdministrationDomainName, _asn_ctx),
	offsetof(struct AdministrationDomainName, present),
	sizeof(((struct AdministrationDomainName *)0)->present),
	asn_MAP_AdministrationDomainName_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_AdministrationDomainName = {
	"AdministrationDomainName",
	"AdministrationDomainName",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	CHOICE_outmost_tag,
	asn_DEF_AdministrationDomainName_tags_1,
	sizeof(asn_DEF_AdministrationDomainName_tags_1)
		/sizeof(asn_DEF_AdministrationDomainName_tags_1[0]), /* 1 */
	asn_DEF_AdministrationDomainName_tags_1,	/* Same as above */
	sizeof(asn_DEF_AdministrationDomainName_tags_1)
		/sizeof(asn_DEF_AdministrationDomainName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AdministrationDomainName_1,
	2,	/* Elements count */
	&asn_SPC_AdministrationDomainName_specs_1	/* Additional specs */
};

