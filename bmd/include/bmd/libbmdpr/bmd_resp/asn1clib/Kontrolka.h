/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "BMDResponse"
 * 	found in "bmd_resp.asn1"
 */

#ifndef	_Kontrolka_H_
#define	_Kontrolka_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/ENUMERATED.h>
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Kontrolka__wxType {
	Kontrolka__wxType_typeUserChoosableEditable	= 10,
	Kontrolka__wxType_typeUserChoosableOnly	= 11,
	Kontrolka__wxType_typeUserEditableShort	= 12,
	Kontrolka__wxType_typeUserEditableLong	= 13
} Kontrolka__wxType_e;
typedef enum Kontrolka__visibility {
	Kontrolka__visibility_unvisible	= 0,
	Kontrolka__visibility_visible	= 1
} Kontrolka__visibility_e;
typedef enum Kontrolka__targetForm {
	Kontrolka__targetForm_sendForm	= 1,
	Kontrolka__targetForm_searchForm	= 2,
	Kontrolka__targetForm_gridForm	= 3,
	Kontrolka__targetForm_sendSearchForm	= 4,
	Kontrolka__targetForm_searchGridForm	= 5,
	Kontrolka__targetForm_allForm	= 6
} Kontrolka__targetForm_e;
typedef enum Kontrolka__contentType {
	Kontrolka__contentType_textContent	= 1,
	Kontrolka__contentType_numberContent	= 2,
	Kontrolka__contentType_timeContent	= 3
} Kontrolka__contentType_e;
typedef enum Kontrolka__connectType {
	Kontrolka__connectType_onlyChoose	= 0,
	Kontrolka__connectType_chooseChoose	= 1,
	Kontrolka__connectType_absent	= 2
} Kontrolka__connectType_e;
typedef enum Kontrolka__valueSource {
	Kontrolka__valueSource_fromControlValue	= 0,
	Kontrolka__valueSource_fromControlAssocData	= 1
} Kontrolka__valueSource_e;
typedef enum Kontrolka__inheritancePresent {
	Kontrolka__inheritancePresent_absent	= 0,
	Kontrolka__inheritancePresent_present	= 1
} Kontrolka__inheritancePresent_e;
typedef enum Kontrolka__splitToWords {
	Kontrolka__splitToWords_splitToWordsFALSE	= 0,
	Kontrolka__splitToWords_splitToWordsTRUE	= 1
} Kontrolka__splitToWords_e;
typedef enum Kontrolka__rangeEnabled {
	Kontrolka__rangeEnabled_rangeEnabledFALSE	= 0,
	Kontrolka__rangeEnabled_rangeEnabledTRUE	= 1
} Kontrolka__rangeEnabled_e;
typedef enum Kontrolka__mandatory {
	Kontrolka__mandatory_nonMandatory	= 0,
	Kontrolka__mandatory_mandatory	= 1
} Kontrolka__mandatory_e;
typedef enum Kontrolka__isUpdateEnabled {
	Kontrolka__isUpdateEnabled_nonUpdateEnabled	= 0,
	Kontrolka__isUpdateEnabled_updateEnabled	= 1
} Kontrolka__isUpdateEnabled_e;
typedef enum Kontrolka__isHistoryEnabled {
	Kontrolka__isHistoryEnabled_nonhistoryEnabled	= 0,
	Kontrolka__isHistoryEnabled_historyEnabled	= 1
} Kontrolka__isHistoryEnabled_e;

/* Kontrolka */
typedef struct Kontrolka {
	OCTET_STRING_t	 label;
	ENUMERATED_t	 wxType;
	ENUMERATED_t	 visibility;
	ENUMERATED_t	 targetForm;
	INTEGER_t	 seqNumber;
	struct Kontrolka__valuesList {
		A_SEQUENCE_OF(OCTET_STRING_t) list;

		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} valuesList;
	struct Kontrolka__priorityList {
		A_SEQUENCE_OF(INTEGER_t) list;

		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} priorityList;
	INTEGER_t	 sizeX;
	INTEGER_t	 sizeY;
/*pkl	OBJECT_IDENTIFIER_t	 oid;*/
	OCTET_STRING_t	oid;
	ENUMERATED_t	 contentType;
	OCTET_STRING_t	 regexp;
	INTEGER_t	 superiorControl;
	INTEGER_t	 inferiorControl;
	ENUMERATED_t	 connectType;
	ENUMERATED_t	 valueSource;
	OCTET_STRING_t	*tooltip	/* OPTIONAL */;
	OCTET_STRING_t	*depValueList	/* OPTIONAL */;
	ENUMERATED_t	 inheritancePresent;
	OCTET_STRING_t	 controlName;
	ENUMERATED_t	 splitToWords;
	ENUMERATED_t	 rangeEnabled;
	ENUMERATED_t	 mandatory;
	ENUMERATED_t	 isUpdateEnabled;
	ENUMERATED_t	 isHistoryEnabled;

	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Kontrolka_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_wxType_3;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_visibility_8;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_targetForm_11;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_contentType_26;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_connectType_33;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_valueSource_37;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_inheritancePresent_42;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_splitToWords_46;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_rangeEnabled_49;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_mandatory_52;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_isUpdateEnabled_55;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_isHistoryEnabled_58;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_Kontrolka;

#ifdef __cplusplus
}
#endif

#endif	/* _Kontrolka_H_ */
