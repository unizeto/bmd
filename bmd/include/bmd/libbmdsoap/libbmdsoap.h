#ifndef _LIBBMDSOAP_INCLUDED_
#define _LIBBMDSOAP_INCLUDED_

#include "bmd230H.h"
#include "bmd_wsse.h"

#include <signal.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdzlib/libbmdzlib.h>
#include <bmd/libbmdxslt/libbmdxslt.h>
#include <bmd/libbmd/libbmd_internal.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#include <sys/types.h>
#include <sys/wait.h>

#define FILE_ID_OID "1.2.616.1.113527.4.3.3.23"
#define PACK 262144

#define SIGUSR 35

#define SOAP_FOK(x) \
	{\
		long status;\
		status=x;\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
			char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define SOAP_FOK_TRANSACTION_INSERT(x)	\
	{\
		int status;\
		char *twe_error_status = NULL;\
		char *twe_soap_error_status = NULL;\
		status=x;\
		if(status<0)\
		{\
			if(_GLOBAL_twg_transaction_dir_name != NULL)\
			{\
				asprintf(&twe_error_status, "Error in stopping Transaction. Please remove %s manually. "\
				"Error from file insert: %s (%i) \n", _GLOBAL_twg_transaction_dir_name, GetErrorMsg(status),status);\
			}\
			else\
			{\
				asprintf(&twe_error_status, "Error in stopping Transaction. No transaction directory. "\
				"Error from file insert: %s (%i) \n", GetErrorMsg(status),status);\
			}\
			asprintf(&twe_soap_error_status, "Error in stopping Transaction. "\
			"Error from file insert: %s (%i) \n", GetErrorMsg(status),status);\
			if(twe_error_status != NULL)\
			{\
				PRINT_SOAP(twe_error_status);\
			}\
			else\
			{\
				PRINT_SOAP("Insufficient memory")\
			}\
			if(twe_soap_error_status != NULL)\
			{\
				return soap_receiver_fault(soap,"Error",twe_soap_error_status);\
			}\
			else\
			{\
				return soap_receiver_fault(soap,"Error","Insufficient memory");\
			}\
		}\
	}\


#define SOAP_FOK_LOGOUT_TRANSACTION(x) \
	{\
		long status;\
		status=x;\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
			twfun_mark_error_in_transaction(transaction_dir_name, status);\
			char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
			return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}


#define SOAP_FOK_LOGOUT(x) \
	{\
		long status;\
		status=x;\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
			char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
			return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define SOAP_FOK_LOGOUT_EXT(x) \
	{\
		long status;\
		status=x;\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
			exit(0);\
		}\
	}

#define SOAP_FOK_LOGOUT_CD(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_LOGOUT_CD %li\n", status);\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			chdir(_GLOBAL_current_dir);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define SOAP_FOK_LOGOUT_CD_TMP(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_LOGOUT_CD_TMP %li\n", status);\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			chdir(_GLOBAL_current_dir);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define SOAP_FOK_LOGOUT_TMP(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_LOGOUT_TMP %li\n", status);\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			chdir(_GLOBAL_current_dir);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define SOAP_FOK_LOGOUT_TMP_EXT(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_LOGOUT_TMP_EXT %li\n", status);\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			chdir(_GLOBAL_current_dir);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
			exit(BMD_OK);\
		}\
	}

#define SOAP_FOK_LOGOUT_TMP_EXT_1(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_LOGOUT_TMP_EXT_1 %li\n", status);\
		if(status<0)\
		{\
			twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, status);\
			PRINT_ERROR("Error: %li\n", status);\
			chdir(_GLOBAL_current_dir);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			bmd_logout(&bi, deserialisation_max_memory);\
			bmd_end();\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
			char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
			return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
			exit(BMD_OK);\
		}\
	}

#define SOAP_FOK_TMP(x) \
	{\
		long status;\
		status=x;\
		PRINT_VDEBUG("SOAPSERVERVDEBUG SOAP_FOK_TMP %li\n", status);\
		if(status<0)\
		{\
			PRINT_ERROR("Error: %li\n", status);\
			remove_dir(tmp_location);\
			if(tmp_location != NULL)\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by status handling\n", tmp_location);\
			}\
			else\
			{\
				PRINT_VDEBUG("SOAPSERVERVDEBUG NULL temporary directory\n");\
			}\
			PRINT_SOAP("Error:\t\t%s (%li) \n",GetErrorMsg(status),status);\
        		char *tag=NULL;\
			asprintf(&tag, "<error>%s</error>", GetErrorMsg(status));\
        		return soap_receiver_fault(soap,GetErrorMsg(status),tag);\
		}\
	}

#define PRINT_SOAP(fmt,args...) \
{\
		___debug_printtime();\
		printf("[%i]\t",getpid()); \
		printf(fmt,##args);\
		fflush(stdout); \
}

/************************************************************************/
/*	Struktura przechowujaca konfiguracje escapowania pliku CSV	*/
/************************************************************************/
struct CSV_escape_conf {
	long tws_enable_escaping;
	char *tws_csv_delimiter;
	char *tws_char_to_escape;
	char *tws_escape_char;
	char *tws_m_value_escape;

	long tws_enable_string_escaping;
	long tws_enable_number_escaping;
	long tws_enable_date_escaping;

	char *src_delimeter;
	char *dst_delimeter;
};

typedef struct CSV_escape_conf CSV_escape_conf_t;


/************************************/
/*	pobranie pliku z bazy danych	*/
/************************************/
long getFileFromBMD(	bmd_info_t *bi,
				bmdDatagramSet_t *req_dtg_set,
				bmdDatagramSet_t **resp_dtg_set,
				char *dest_local);

/******************************************/
/*	wyszukiwanie plikow w archiwum	*/
/******************************************/
long sendRequestToBMD(	bmd_info_t *bi,
			bmdDatagramSet_t *req_dtg_set,
			long twf_max_datagram_in_set_transmission,
			bmdDatagramSet_t **resp_dtg_set,
			long twf_free_datagrams_flag,
			long deserialisation_max_memory);
/******************************/
/*	stworzenie datagramu	*/
/******************************/
long createDatagram(	char *userClassId,
			char **oids_table,
			char **values_table,
			long oids_count,
			long datagramType,

			long status,
			long sql_operator,

			bmdDatagramSet_t **dtg_set);

/******************************************************/
/* ustawienie roli i grupy logujacego sie uzytkownika */
/******************************************************/
long chooseClientRole(	char *roleId,
			char *groupId,
			bmd_info_t **bi,
			long deserialisation_max_memory);

/**********************************************************/
/* przygotowanie struktur do zalogowania do serwer BMD    */
/* oraz zalogowanie sie do bmd w imieniu uzytkownika soap */
/**********************************************************/
long PrepareDataForBMDLoginAndRequest(	bmd_info_t **bi,
					char* binaryCert,
					long binaryCertLen,
					char *roleId,
					char *groupId,
					long deserialisation_max_memory);

/************************************************/
/*	sprawdzenie, czy string zawiera podstring	*/
/************************************************/
long isSubstring(	char *string,
			char *substring);

/************************/
/*	skopiowanie pliku	*/
/************************/
long fileCopy(	char *source,
			char *destination);

/******************************/
/*	przeniesienie pliku	*/
/******************************/
long fileMove(	char *source,
			char *destination);

/******************************************************************************/
/*	pobranie z bazy danych pliku i zapisanie go we wskazanej lokalizacji	*/
/******************************************************************************/
long insertFileToBMD(	bmd_info_t *bi,
				bmdDatagramSet_t *req_dtg_set,
				bmdDatagramSet_t **resp_dtg_set,
				char *dest_local);

/******************************************************/
/*	stworzenie datagramu zadania pobrania pliku	*/
/******************************************************/
long createDatagramToInsertFile(	char *userClassId,
					char **oids_table,
					char **values_table,
					long oids_count,
					char *filename,
					bmdDatagramSet_t **dtg_set);

/******************************/
/*	delete temporary files	*/
/******************************/
long deleteTempFiles(	char **zip_list,
				long zip_list_size);

long ConvertMtdsList(	struct bmd230__mtdsValues *mtds,
				char ***oids_table,
				char ***values_table,
				long *oids_count);

void *dime_write_open( struct soap*, const char*, const char*, const char* );
void  dime_write_close( struct soap*, void* );
int   dime_write( struct soap*, void*, const char*, size_t );

void *dime_read_open( struct soap*, void*, const char*, const char*, const char* );
size_t dime_read( struct soap*, void*, char*, size_t);
void dime_read_close(struct soap*, void* );

long MtdsListToCSV (	struct soap *twf_soap,
			MetaDataBuf_t **twf_metadata_array_struct,
			long twf_metadata_count,
			BMD_attr_t **twf_form,
			long twf_from_count,

			char **twf_tmp_csv_file,

			long *twf_first_run_flag,

			struct bmd230__mtdsValues *twf_formMtds,
			long twf_formMtds_number,

			CSV_escape_conf_t *twf_CSV_escape_conf);

long FillResponseWithMetadata( struct soap *soapStruct,
				bmdDatagram_t *responseDatagram,
				struct bmd230__mtdsValues *formMtds,
				long includeSYSMetadataFlag, long includeADDMetadataFlag, long includeACTIONMetadataFlag,
				struct bmd230__mtdsValues **responseMetadata );


long MtdsListToResponse (	struct soap *twf_soap,
				MetaDataBuf_t **twf_metadata_array_struct,
				long twf_metadata_count,

				struct bmd230__mtdsValues *twf_result_mtds,

				struct bmd230__mtdsValues *twf_formMtds,
				long twf_formMtds_number,

				long *twf_mtd_result_count);

long CharEscape(	char *twf_value,
			long twf_value_count,
			char **twf_escaped_value,
			long twf_value_type,
			CSV_escape_conf_t *twf_CSV_escape_conf);


long bmdsoapGenerateDateTimeString(struct bmd230__DateTime* dateTimeStruct, char** dateTimeString);


long twfun_make_transaction_directory(	char *twf_transaction_dir_name,
					long twf_error_if_failure);

long twfun_check_transaction_directory( char *twf_temporary_dir,
					char *twf_transaction_id,
					long twf_start_transaction_required,
					char **twf_transaction_dir_name);

long twfun_mark_start_of_insert(	char *twf_transaction_dir_name,
					char *twf_transaction_id);

long twfun_unmark_start_of_insert(	char *twf_transaction_dir_name,
					char *twf_transaction_id,
					long twf_pid);

long twfun_mark_error_in_transaction(	char *twf_transaction_dir_name,
					long twf_error_code);


#endif

