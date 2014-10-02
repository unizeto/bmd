/****************************************
 *					*
 * Test operacji Select		*
 *					*
 ***************************************/

#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdsql/js.h>
#include<bmd/libbmdsql/dict.h>

#include<bmd/libbmdutils/libbmdutils.h>

#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdpkcs12/libbmdpkcs12.h>

#include<pthread.h>
#define OID_CMS_ID_DATA			1,2,840,113549,1,7,1

extern long _GLOBAL_debug_level;


int main()
{
int err=0;
unsigned long int 	oid_additional_metadata_regon[] 	= {1,2,616,1,113527,4,3,3,1};
unsigned long int 	oid_additional_metadata_nip[] 		= {1,2,616,1,113527,4,3,3,2};
unsigned long int 	oid_additional_metadata_undefined[]	= {1,2,616,1,113527,4,3,3,0};
/*unsigned long int 	data_sprzedazy[] = { 1,2,616,1,113527,4,3,3,16};*/
unsigned long int	oid_additional_metadata_data_przyjecia[]= {1,2,616,1,113527,4,3,3,37,1};
unsigned long int	oid_additional_metadata_data_przyjecia2[]= {1,2,616,1,113527,4,3,3,37,2};
unsigned long int 	oid_usr_metadata_keyword[]		= {OID_USR_METADATA_KEYWORDS};
unsigned long int 	oid_usr_metadata_data_description[]	= {OID_USR_METADATA_DATA_DESCRIPTION};
unsigned long int 	oid_usr_metadata_software_types[]	= {OID_USR_METADATA_SOFTWARE};
unsigned long int 	oid_usr_metadata_doc_types[]		= {OID_USR_METADATA_DOCUMENT_TYPE};
unsigned long int 	oid_sys_metadata_groups[]            	= {OID_RFC3281_GROUP};
unsigned long int 	oid_sys_metadata_roles[]		= {OID_RFC3281_ROLE};
unsigned long int 	oid_sys_metadata_sec_categories[]    	= {OID_SYS_METADATA_SEC_CAT};
unsigned long int 	oid_sys_metadata_sec_levels[]        	= {OID_SYS_METADATA_BMD_SEC_LEVELS};
unsigned long int 	oid_sys_metadata_crypto_objects_timestamp[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP};
unsigned long int 	oid_sys_metadata_crypto_objects_filename[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME};
unsigned long int 	oid_sys_metadata_crypto_objects_filesize[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE};


unsigned long int 	oid_pki_metadata_timestamp[]        	= {OID_PKI_METADATA_TIMESTAMP};
unsigned long int 	oid_pki_metadata_signature[]        	= {OID_PKI_METADATA_SIGNATURE};

unsigned long int 	oid_usr_metadata_cases_case_type[]		= {OID_USR_METADATA_CASES_CASE_TYPE};
unsigned long int 	oid_usr_metadata_cases_case_code[]		= {OID_USR_METADATA_CASES_CASE_CODE};
unsigned long int 	oid_usr_metadata_cases_case_code_passwd[]	= {OID_USR_METADATA_CASES_CASE_CODE_PASSWD};
unsigned long int 	oid_usr_metadata_cases_case_title[]		= {OID_USR_METADATA_CASES_CASE_TITLE};
unsigned long int 	oid_usr_metadata_cases_case_description[]	= {OID_USR_METADATA_CASES_CASE_DESCRIPTION};
unsigned long int 	oid_usr_metadata_cases_parent_id[]		= {OID_USR_METADATA_CASES_PARENT_ID};
unsigned long int 	oid_usr_metadata_cases_relation_type[]		= {OID_USR_METADATA_CASES_RELATION_TYPE};

unsigned long int oid_sys_metadata_crypto_objects_filesize_less[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE,1};
unsigned long int oid_sys_metadata_crypto_objects_filesize_more[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE,2};
unsigned long int oid_sys_metadata_crypto_objects_timestamp_less[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP,1};
unsigned long int oid_sys_metadata_crypto_objects_timestamp_more[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP,2};



void *db_conn_info			= NULL;
GenBuf_t *bmdDatagramDerEncoded		= NULL;

bmdDatagram_t *dg1 = NULL;
bmdDatagram_t *dg2 = NULL;
bmdDatagram_t *dg3 = NULL;
bmdDatagramSet_t *set1=NULL;
bmdDatagramSet_t *set2=NULL;
bmdDatagramSet_t *set3=NULL;

unsigned long int SQLQueryOffset	= 0;
unsigned long int SQLQueryLimit		= 20;

_GLOBAL_debug_level = 2;
char *server_info_postgres =	"hostaddr='127.0.0.1' dbname='bmd2' port='5432' user='bmd2' password='12345678'";
char *server_info_oracle = 	"(DESCRIPTION = (ADDRESS_LIST = (ADDRESS = (PROTOCOL = TCP)(HOST = 127.0.0.1)(PORT = 1521)))"
				"(CONNECT_DATA = (SID = BMD) (SERVER = DEDICATED)))";
char *server_info = server_info_oracle;

	/* wczytaj slowniki do cache bazodanowego */
/*	err=dump_all_dictionaries(server_info_postgres);*/
	err=dump_all_dictionaries(server_info);
	if(err < 0){ PRINT_DEBUG("ERR[%s:%i:%s] Err=%i\n", __FILE__, __LINE__, __FUNCTION__, err); return(err); }

/*	bmd_db_connect(server_info_postgres, &db_conn_info);*/
	bmd_db_connect(server_info, &db_conn_info);
	ShowDatabaseInfo(db_conn_info);
/*	err = CheckSQLScript(db_conn_info, "1.0.1", "generic_BMD Oracle");*/
/*	err = CheckSQLScript(db_conn_info, "1.0.1", "generic BMD");*/
	if(err<0){ printf("CheckSQLScript err=%i\n", err); return err; }

	err = PR_create_bmdDatagram_set(&set1); if(err<0){ printf("Create set err\n"); return err; }
	dg1 = (bmdDatagram_t *) malloc (sizeof(bmdDatagram_t));
	PR_bmdDatagram_init(dg1);
	/*dg1->datagramType = BMDDatagram__datagramType_requestSelectBmdDataFull;*/
	dg1->datagramType = BMD_DATAGRAM_TYPE_SELECT;

	/* owner pliku */
	dg1->protocolDataOwner = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolDataOwner->buf = (char *) malloc (sizeof(char) * 100);
	memset(dg1->protocolDataOwner->buf, 0, 100);
	/*strcpy(dg1->protocolDataOwner->buf, "cn=LEVEL, ou=LEVEL, o=LEVEL, l=Szczecin, sp=Zachodniopomorskie, c=PL5");*/
	/*strcpy(dg1->protocolDataOwner->buf, "cn=LEVEL, ou=LEVEL, o=LEVEL, l=Szczecin, sp=Zachodniopomorskie, c=PL5");*/
	strcpy(dg1->protocolDataOwner->buf, "%");
	dg1->protocolDataOwner->size=100;

	/* limit i offset */
	dg1->protocolDataFileId = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolDataFileId->buf= (char *) malloc (sizeof(char) * 9);
	memset(dg1->protocolDataFileId->buf, 0, 9);
	strcpy(dg1->protocolDataFileId->buf, "0|10");
	dg1->protocolDataFileId->size=9;

	dg1->datagramStatus = /*BMD_REQ_DB_SELECT_ALL_SYSMTD |*/ BMD_REQ_DB_SELECT_LIKE_QUERY;
	/*dg1->datagramStatus = BMD_REQ_DB_SELECT_ALL_SYSMTD;*/
	/*	BMD_REQ_DB_SELECT_ILIKE_QUERY
		BMD_REQ_DB_SELECT_LIKE_QUERY
		BMD_REQ_DB_SELECT_ILIKE_QUERY	*/

/*	PR_bmdDatagram_add_metadata(oid_additional_metadata_regon,
				sizeof(oid_additional_metadata_regon),
				"regon1",
				1+strlen("regon1"),
				ADDITIONAL_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_additional_metadata_nip,
				sizeof(oid_additional_metadata_nip),
				"nip1",
				1+strlen("nip1"),
				ADDITIONAL_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_additional_metadata_undefined,
				sizeof(oid_additional_metadata_undefined),
				"undefined1",
				1+strlen("undefined1"),
				ADDITIONAL_METADATA,
				dg1);
*/
	PR_bmdDatagram_add_metadata(oid_additional_metadata_data_przyjecia,
				sizeof(oid_additional_metadata_data_przyjecia),
				"2007-02-13 11:40:06",
				1+strlen("2007-02-13 11:40:06"),
				ADDITIONAL_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_additional_metadata_data_przyjecia2,
				sizeof(oid_additional_metadata_data_przyjecia2),
				"2005-02-13 11:40:04",
				1+strlen("2005-02-13 11:40:04"),
				ADDITIONAL_METADATA,
				dg1);

	PR_bmdDatagram_add_metadata(oid_sys_metadata_crypto_objects_filename,
				sizeof(oid_sys_metadata_crypto_objects_filename),
				"plik",
				1+strlen("plik"),
				SYS_METADATA,
				dg1);

/*
unsigned long int oid_sys_metadata_crypto_objects_filesize_more[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE,1};
unsigned long int oid_sys_metadata_crypto_objects_filesize_less[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE,2};
unsigned long int oid_sys_metadata_crypto_objects_timestamp_more[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP,1};
unsigned long int oid_sys_metadata_crypto_objects_timestamp_less[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP,2};
*/
/*	PR_bmdDatagram_add_metadata(oid_sys_metadata_crypto_objects_timestamp_more,
				sizeof(oid_sys_metadata_crypto_objects_timestamp_more),
				"2007-03-03",
				1+strlen("2007-03-03"),
				SYS_METADATA,
				dg1);
*/
	PR_bmdDatagram_add_metadata(oid_sys_metadata_roles,
				sizeof(oid_sys_metadata_roles),
				"administrator",
				1+strlen("administrator"),
				SYS_METADATA,
				dg1);

	PR_bmdDatagram_add_metadata(oid_usr_metadata_keyword,
				sizeof(oid_usr_metadata_keyword),
				"slowo kluczowe 1",
				1+strlen("slowo kluczowe 1"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_data_description,
				sizeof(oid_usr_metadata_data_description),
				"opis 1",
				1+strlen("opis 1"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_software_types,
				sizeof(oid_usr_metadata_software_types),
				"1.2.616.1.113527.4.3.1.5.3",
				1+strlen("1.2.616.1.113527.4.3.1.5.3"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_doc_types,
				sizeof(oid_usr_metadata_doc_types),
				"1.2.616.1.113527.4.3.1.4.1",
				1+strlen("1.2.616.1.113527.4.3.1.4.1"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_sys_metadata_groups,
				sizeof(oid_sys_metadata_groups),
				"1.2.616.1.113527.4.3.2.3.1",
				1+strlen("1.2.616.1.113527.4.3.2.3.1"),
				SYS_METADATA,
				dg1);

	/* Oganieczanie warupnkow zapytania dot. spraw */
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_type,
				sizeof(oid_usr_metadata_cases_case_type),
				"high priority case",
				1+strlen("high priority case"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_code,
				sizeof(oid_usr_metadata_cases_case_code),
				"2006/12/31-CaseCode1",
				1+strlen("2006/12/31-CaseCode1"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_code_passwd,
				sizeof(oid_usr_metadata_cases_case_code_passwd),
				"12345678",
				1+strlen("12345678"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_title,
				sizeof(oid_usr_metadata_cases_case_title),
				"Tytul sprawy: Notatka sluzbowa",
				1+strlen("Tytul sprawy: Notatka sluzbowa"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_description,
				sizeof(oid_usr_metadata_cases_case_description),
				"Notatka sluzbowa ze spotkana w sprawie BAE.",
				1+strlen("Notatka sluzbowa ze spotkana w sprawie BAE."),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_parent_id,
				sizeof(oid_usr_metadata_cases_parent_id),
				"1",
				1+strlen("1"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_usr_metadata_cases_relation_type,
				sizeof(oid_usr_metadata_cases_relation_type),
				"1.2.616.1.113527.4.3.1.7.2",
				1+strlen("1.2.616.1.113527.4.3.1.7.2"),
				USER_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_sys_metadata_sec_categories,
				sizeof(oid_sys_metadata_sec_categories),
				"1.2.616.1.113527.4.3.2.1.1",
				1+strlen("1.2.616.1.113527.4.3.2.1.1"),
				SYS_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_sys_metadata_sec_levels,
				sizeof(oid_sys_metadata_sec_levels),
				"1.2.616.1.113527.4.3.2.4.2",
				1+strlen("1.2.616.1.113527.4.3.2.4.2"),
				SYS_METADATA,
				dg1);
	PR_bmdDatagram_add_metadata(oid_pki_metadata_timestamp,
				sizeof(oid_pki_metadata_timestamp),
				"timestamp 1",
				1+strlen("timestamp 1"),
				PKI_METADATA,
				dg1);

/*	PR_bmdDatagram_print(dg1);*/
	err = PR_add_bmdDatagram_to_set(dg1, set1);
	if(err<0){ printf("Error in adding into set\n"); return err; }
	PR_bmdDatagramSet_print(set1, WITHOUT_TIME);

	printf("Selektuje pierwszy datagram\n");
	err = JS_main(db_conn_info, set1, &set2, NULL, NULL, NULL);
	if(err<0){ PRINT_DEBUG("TESTERR[%s:%i%s] JS Main zakonczyl sie z bledem!!! Kod bledu = %i\n", err); return err; }

	printf("Po JS main\n");
	printf("test 1 err = %i set = %p\n", err, set2);

	printf("Pobralem pierwszy i mam ilosc wszystkich datagramow = %i\n", set2->bmdDatagramSetSize);
	printf("Pokaz odpowiedz\n");
/*	PR_bmdDatagramSet_print(set2, WITHOUT_TIME);*/

//	PR_bmdDatagramSet_serialize(set2, &bmdDatagramDerEncoded);
	printf("TEST1 BEGIN ********************************************************************************\n");
//	PR_bmdDatagramSet_deserialize(bmdDatagramDerEncoded, &set3);
	PR_bmdDatagramSet_print(set2, WITHOUT_TIME);
	printf("TEST1 END **********************************************************************************\n");

//	free(bmdDatagramDerEncoded->buf);
//	free(bmdDatagramDerEncoded);
	PR_bmdDatagramSet_free(set1, 1); set1=NULL;
	PR_bmdDatagramSet_free(set2, 1); set2=NULL;
//	PR_bmdDatagramSet_free(set3, 1); set3=NULL;
	bmd_db_disconnect(&db_conn_info);
	return err;
}

