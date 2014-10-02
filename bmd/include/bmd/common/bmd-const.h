#ifndef _BMD_CONST_INCLUDED_
#define _BMD_CONST_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

/*
	na kompilatorach gcc w wersji 3.4.4 oraz 3.4.6 nie ma rzutowania const
	pod każdym innym kompilatorem rzutuje na const
*/

#ifdef WIN32
	#define CONST_CAST
#else
	#if (__GNUC__ == 3 && __GNUC_MINOR__ == 4)
		#if __GNUC_PATCHLEVEL__ == 4 || __GNUC_PATCHLEVEL__ == 3
			#define CONST_CAST
		#elif __GNUC_PATCHLEVEL__ == 6
			#define CONST_CAST const
		#else
			#define CONST_CAST const
		#endif
	#else
		#if (__GNUC__ == 3 && __GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ == 6)
			#define CONST_CAST
		#else
			#define CONST_CAST const
		#endif
	#endif
#endif

#define BMD_FOK_BREAK(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
		status=bmd_fok_status;\
		break;\
	}\
}

#define BMD_FOK_CONTINUE(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
		continue;\
	}\
}

#define BMD_FOK_CONTINUE_NP(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		continue;\
	}\
}

#define BMD_FOK(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
		return bmd_fok_status;\
	}\
}


#define BMD_FOK_NP(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		return bmd_fok_status;\
	}\
}

#define BMD_FOK_CHG(x,y)\
{\
	long bmd_fok_status = 0, bmd_fok_ret = 0;\
	bmd_fok_status=x;\
	bmd_fok_ret=y;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_ret),bmd_fok_ret);\
		return bmd_fok_ret;\
	}\
}

#define BMD_FOK_CHG_NP(x,y)\
{\
	long bmd_fok_status = 0, bmd_fok_ret = 0;\
	bmd_fok_status=x;\
	bmd_fok_ret=y;\
	if(bmd_fok_status<BMD_OK)\
	{\
		return bmd_fok_ret;\
	}\
}

#define BMD_FOK_TRANS(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR(" %s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
		bmd_db_rollback_transaction(hDB,NULL);\
		return bmd_fok_status;\
	}\
}

#define BMD_FOK_DB(x)\
	{\
		long bmd_fok_status = 0;\
		bmd_fok_status=x;\
		if(bmd_fok_status<BMD_OK)\
		{\
			PRINT_ERROR(" %s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
			bmd_db_disconnect(&hDB);\
			return bmd_fok_status;\
		}\
}


#define BMD_FOK_THREAD(x)\
		{\
			long bmd_fok_status = 0;\
			bmd_fok_status=x;\
			if(bmd_fok_status<BMD_OK)\
			{\
				bmd_thread_return_t *bmd_fok_thread_return	= NULL;\
				PRINT_ERROR(" %s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
				bmd_fok_thread_return = malloc(sizeof(bmd_thread_return_t));\
				if(bmd_fok_thread_return == NULL)\
				{\
					PRINT_ERROR(" %s. Error: %i\n",GetErrorMsg(NO_MEMORY), NO_MEMORY);\
					return NULL;\
				}\
				bmd_fok_thread_return->tws_status = bmd_fok_status;\
				return (void *)bmd_fok_thread_return;\
			}\
		}\

#define BMD_FOK_NO_RETURN(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status<BMD_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
	}\
}\

#define BMD_MEMORY_GUARD(x)\
{\
	twl_memory_guard = twl_memory_guard + x;\
	if(twl_max_memory != 0 && twl_memory_guard > twl_max_memory)\
	{\
		PRINT_ERROR("Memory limit %li bytes exceeded with %li bytes. Error code %i\n",twl_max_memory,\
		twl_memory_guard, MAX_MEMORY_EXCEEDED);\
		BMD_FOK(MAX_MEMORY_EXCEEDED);\
	}\
}\

#define BMD_CODE_PAGE_CP1250	1
#define BMD_CODE_PAGE_ISO88592	2
#define BMD_CODE_PAGE_UTF8	3

#define BMD_BN_BYTE_SIZE	4

#define BMD_SHOW_PROGRESS	1
#define BMD_HIDE_PROGRESS	0


/* parsowanie pliku kontrolki.conf i zwiazane z tym definicje */
#define TARGET_FORM_SEND	1
#define TARGET_FORM_SEARCH	2
#define TARGET_FORM_GRID	3
#define TARGET_FORM_SEND_SEARCH	4
#define TARGET_FORM_SEARCH_GRID	5
#define TARGET_FORM_ALL		6

#define CONTROL_VISIBLE	    1
#define CONTROL_UNVISIBLE   0

#define CONTROL_CONTENT_TEXT    1
#define CONTROL_CONTENT_NUMBER  2
#define CONTROL_CONTENT_TIME	3
#define CONTROL_CONTENT_MONEY	4

#define CONTROL_CONNECTION_ONLYCHOOSE	0
#define CONTROL_CONNECTION_CHOOSECHOOSE 1
#define CONTROL_CONNECTION_ABSENT	2

#define CONTROL_VALUE_SOURCE_CONTROLVALUE   0
#define CONTROL_VALUE_SOURCE_ASSOCDATA	    1

#define CONTROL_INHERITANCE_PRESENT 0
#define CONTROL_INHERITANCE_ABSENT  1

#define TYPE_USER_CHOOSABLE_EDITABLE 	10 /* kontrolka ComboBox - wybor z listy plus wpisanie swojego */
#define TYPE_USER_CHOOSABLE_ONLY		11 /* kontrolka typu Choice - wybor jednego z listy */
#define TYPE_USER_EDITABLE_SHORT 		12 /* jednowierszowa kontrolka edycyjna */
#define TYPE_USER_EDITABLE_LONG  		13 /* pole tekstowe */
#define TYPE_USER_CHOOSABLE_TREE_COMBO	14 /* combobox z wartosciami wybieralnymi z drzewa !!!!!!!  DEPRECATED  !!!!!!*/ 
#define TYPE_USER_CHOOSABLE_DATE		15 /* combobox z wartosciami wybieralnymi z kalendarza */

#define TYPE_USER_LIST  16
/* ponizej odmiany umozliwiajace podanie kilku wartosci */
#define TYPE_USER_CHOOSABLE_EDITABLE_LIST    TYPE_USER_CHOOSABLE_EDITABLE + TYPE_USER_LIST
#define TYPE_USER_CHOOSABLE_ONLY_LIST        TYPE_USER_CHOOSABLE_ONLY     + TYPE_USER_LIST
#define TYPE_USER_EDITABLE_SHORT_LIST        TYPE_USER_EDITABLE_SHORT     + TYPE_USER_LIST
#define TYPE_USER_EDITABLE_LONG_LIST         TYPE_USER_EDITABLE_LONG      + TYPE_USER_LIST
#define TYPE_USER_CHOOSABLE_DATE_LIST        TYPE_USER_CHOOSABLE_DATE     + TYPE_USER_LIST


#define INTERNAL_SOURCE_FLAG_ABSENT 0
#define INTERNAL_SOURCE_FLAG_FILE   1
#define INTERNAL_SOURCE_FLAG_BMD    2

#define RANGE_ENABLED_TRUE		1
#define RANGE_ENABLED_FALSE		0

#define SPLIT_TO_WORDS_TRUE		1
#define SPLIT_TO_WORDS_FALSE	0

#define MANDATORY_CONTROL	1
#define NON_MANDATORY_CONTROL	0

#define BMD_REQ_DB_SELECT_ONLY_GROUP		0
#define BMD_REQ_DB_SELECT_ONLY_OWNER		1
#define BMD_REQ_DB_SELECT_ONLY_CREATOR		2
#define BMD_REQ_DB_SELECT_CREATOR_OR_OWNER	4
#define BMD_REQ_DB_SELECT_CREATOR_OR_OWNER_OR_GROUPS	8
#define BMD_REQ_DB_SELECT_SKIP_AUTHORIZATION	16

#define BMD_REQ_DB_SELECT_ALL_SYSMTD		32

#define BMD_REQ_DB_SELECT_ILIKE_QUERY		64	/* zapytanie ignorujace wielkosc liter i dopasowujace podlancuchy */
#define BMD_REQ_DB_SELECT_LIKE_QUERY		128	/* zapytanie nie ignorujace wielkosc liter i dopasowujace podlancuchy */
#define BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY	256	/* zapytanie ignorujace wielkosc liter ale dopasowujace dokladnie */
#define BMD_REQ_DB_SELECT_EQUAL_QUERY		512	/* zapytanie dopasowujace doslawnie, przez operator '=' */

/***********************************************************/
/* stałe do budoania dowolnych zapytań wyszukujących w bmd */
/***********************************************************/
#define BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE	64
#define BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN	128
#define BMD_REQ_DB_SELECT_ADVANCED_PERC_END	256
#define BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH	512
#define BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY	1024
#define BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY	2048
#define BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR	4096
#define BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND	8192

#define POSITIVE_STATEMENT		0
#define NEGATIVE_STATEMENT		1

#define	BMD_FORM		0
#define BMD_FORM_SEND		1
#define BMD_FORM_SEARCH		2
#define BMD_FORM_GRID		3
#define BMD_FORM_UNVISIBLE	4
#define BMD_FORM_UPLOAD		5
#define BMD_FORM_HISTORY	6
#define BMD_FORM_UPDATE		7

/**************************************************************************************/
#define MAX_FILE_SIZE 2000000000

#define KLIENT_CONF_OK				100

#define MAX_READERS 100
#define MAX_READER_NAME 64

#define PARSE_CORRECT	0
#define OK 		0

#define OVECCOUNT 30

#define PLAIN_PKI_METADATA	0
#define SIGNATURE_PKI_METADATA	1
#define TIMESTAMP_PKI_METADATA	2
#define SIG_TS_PKI_METADATA	3

#define REGEXP_SIZE		300	/* do 100 dopasowan */
#define REGEXP_SIZE_VALUES	3000	/* do 1000 dopasowan */

#define TRUE_KONTROLKI	1
#define FALSE_KONTROLKI	0

#define EQUAL	0

#define PCRE_GET_SUBSTRING_OK(x)	( (x!=PCRE_ERROR_NOMEMORY) && (x!=PCRE_ERROR_NOSUBSTRING) )

#define CommonName_OID 		0x55,0x04,0x03
#define Organization_OID	0x55,0x04,0xA

#define LARGE_FILE_DB_CHUNK_SIZE 1024*256

#define ASN1_DATAGRAM_HDR_SIZE 9

#define BMD_RESP_DB_INSERT_ERR	-1
#define BMD_RESP_DB_SELECT_ERR	-2
#define BMD_RESP_DB_GET_ERR		-3
#define BMD_RESP_DB_DELETE_ERR	-4
#define BMD_RESP_DB_UPDATE_ERR	-5

#define BMD_ADD_METADATA_COLNAME_AM_VALUE			"amv"
#define BMD_ADD_METADATA_COLNAME_FK_AM_TYPES		"fkamt"

#define BMD_MAX_SERIALISATION_TYPE	2

#define SIGUSR 			35
#define SIGDICTRELOAD		36
#define SIGCONFRELOAD		37
#define SIGALLRELOAD		38

#if defined(__cplusplus)
}
#endif

#endif


