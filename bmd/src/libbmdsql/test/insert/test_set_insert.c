/****************************************
 *					*
 * Test operacji Insert 		*
 *					*
 ***************************************/
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>

#include<bmd/libbmdsql/proofs.h>
#include<bmd/libbmdsql/dict.h>
#include<bmd/libbmdsql/js.h>

#include<bmd/libbmdutils/libbmdutils.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdpkcs12/libbmdpkcs12.h>
#include<pthread.h>

#define OID_CMS_ID_DATA		1,2,840,113549,1,7,1
#define TIMESTAMP_FILENAME	"../resources/timestamp_response.tsr"
#define CRL_FILENAME		"../resources/crl.crl"
#define CERT_FILENAME		"../resources/CA.cer"
#define SIG_FILENAME		"../resources/plik.sig"


extern long _GLOBAL_debug_level;

/* Prototypy */
int read_GenBuf_from_file(char *filename, GenBuf_t **gbuf);


int main(int argc, char *argv[])
{
int err=0;
unsigned long int 	oid_additional_metadata_undefined[]	= {1,2,616,1,113527,4,3,4,0};
unsigned long int 	oid_additional_metadata_imie[] 		= {1,2,616,1,113527,4,3,3,30};
unsigned long int 	oid_additional_metadata_nazwisko[] 	= {1,2,616,1,113527,4,3,3,31};
unsigned long int	oid_additional_metadata_data_przyjecia[]= {1,2,616,1,113527,4,3,3,37};
unsigned long int 	oid_usr_metadata_keyword[]		= {OID_USR_METADATA_KEYWORDS};
unsigned long int 	oid_usr_metadata_data_description[]	= {OID_USR_METADATA_DATA_DESCRIPTION};
unsigned long int 	oid_usr_metadata_software_types[]	= {OID_USR_METADATA_SOFTWARE};
unsigned long int 	oid_usr_metadata_doc_types[]		= {OID_USR_METADATA_DOCUMENT_TYPE};
unsigned long int 	oid_sys_metadata_groups[]            	= {OID_RFC3281_GROUP};
unsigned long int 	oid_sys_metadata_roles[]		= {OID_RFC3281_ROLE};
unsigned long int 	oid_sys_metadata_sec_categories[]    	= {OID_SYS_METADATA_SEC_CAT};
unsigned long int 	oid_sys_metadata_sec_levels[]        	= {OID_SYS_METADATA_BMD_SEC_LEVELS};
unsigned long int 	oid_pki_metadata_timestamp[]        	= {OID_PKI_METADATA_TIMESTAMP};
unsigned long int 	oid_pki_metadata_signature[]        	= {OID_PKI_METADATA_SIGNATURE};
unsigned long int	oid_pki_metadata_certificate[]		= {OID_PKI_METADATA_QUALIFIEDCERT};
unsigned long int	oid_pki_metadata_crl[]			= {OID_PKI_METADATA_CRL};

unsigned long int 	oid_usr_metadata_cases_case_type[]		= {OID_USR_METADATA_CASES_CASE_TYPE};
unsigned long int 	oid_usr_metadata_cases_case_code[]		= {OID_USR_METADATA_CASES_CASE_CODE};
unsigned long int 	oid_usr_metadata_cases_case_code_passwd[]	= {OID_USR_METADATA_CASES_CASE_CODE_PASSWD};
unsigned long int 	oid_usr_metadata_cases_case_title[]		= {OID_USR_METADATA_CASES_CASE_TITLE};
unsigned long int 	oid_usr_metadata_cases_case_description[]	= {OID_USR_METADATA_CASES_CASE_DESCRIPTION};
unsigned long int 	oid_usr_metadata_cases_parent_id[]		= {OID_USR_METADATA_CASES_PARENT_ID};
unsigned long int 	oid_usr_metadata_cases_relation_type[]		= {OID_USR_METADATA_CASES_RELATION_TYPE};


void *db_conn_info			= NULL;
GenBuf_t *bmdDatagramDerEncoded		= NULL;
_GLOBAL_debug_level = 2;

GenBuf_t *timestampFile			= NULL;
GenBuf_t *crlFile			= NULL;
GenBuf_t *certFile			= NULL;
GenBuf_t *sigFile			= NULL;

bmd_crypt_ctx_t *sym_ctx		= NULL;
GenBuf_t *enc				= NULL;

bmdDatagram_t *dg1;
bmdDatagram_t *dg2;
bmdDatagram_t *dg3 = NULL;
bmdDatagramSet_t *set1=NULL;
bmdDatagramSet_t *set2=NULL;
bmdDatagramSet_t *set3=NULL;

unsigned long int SQLQueryOffset	= 1;
unsigned long int SQLQueryLimit		= 3;
int licznik = 0;

bmd_crypt_ctx_t *bmd_ctx		= NULL;
GenBuf_t *tmp				= NULL;



	char *server_info_postgres =	"hostaddr='127.0.0.1' dbname='bmd2' port='5432' user='bmd2' password=12345678";
	char *server_info_oracle = 	"(DESCRIPTION = (ADDRESS_LIST = (ADDRESS = (PROTOCOL = TCP)(HOST = 127.0.0.1)(PORT = 1521))) "
					"(CONNECT_DATA = (SID = BMD) (SERVER = DEDICATED)))";
	char *server_info = server_info_postgres;

	bmd_init();
	/* wczytaj slowniki do cache bazodanowego */
	printf("Zrzucam slowniki... ");
	err=dump_all_dictionaries(server_info);
	if(err < 0){	PRINT_DEBUG("ERR[%s:%i:%s] Err=%i\n", __FILE__, __LINE__, __FUNCTION__, err); return err; }
	printf("[done]\n");
	/*Wczytaj znacznik czasu do GenBuf'a*/
	printf("Wczytuje znacznik do GenBuf_a... ");
	err = read_GenBuf_from_file(TIMESTAMP_FILENAME, &timestampFile);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading timestamp file: %s. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, TIMESTAMP_FILENAME, err);
		return err;
	}
	printf("[done] dlugosc = %d\n", timestampFile->size);
	/*Wczytaj CRLa do GenBuf'a*/
	printf("Wczytuje CRLa do GenBuf_a... ");
	err = read_GenBuf_from_file(CRL_FILENAME, &crlFile);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading CRL file: %s. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, CRL_FILENAME, err);
		return err;
	}
	/*Wczytaj Certyfikat do GenBuf'a*/
	printf("Wczytuje certyfikat do GenBuf_a... ");
	err = read_GenBuf_from_file(CERT_FILENAME, &certFile);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading cert file: %s. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, CERT_FILENAME, err);
		return err;
	}
	/*Wczytaj Podpis do GenBuf'a*/
	printf("Wczytuje podpis do GenBuf_a... ");
	err = read_GenBuf_from_file(SIG_FILENAME, &sigFile);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading signature file: %s. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, SIG_FILENAME, err);
		return err;
	}
	bmd_db_connect(server_info, &db_conn_info);

char *filename = NULL;

for(licznik=0; licznik < 1; licznik++)
{
	err = _PR2_create_bmdDatagram_set(&set1);
	if(err<0){ printf("Create set err\n"); return err; }
	dg1 = (bmdDatagram_t *) malloc (sizeof(bmdDatagram_t));
	PR_bmdDatagram_init(dg1);
	dg1->datagramType = BMD_DATAGRAM_TYPE_INSERT;

	printf("Wczytuje plik do datagramu... ");
	/* zawartosc pliku */
	asprintf(&filename, "plik%i.txt", licznik);
	err = read_GenBuf_from_file(filename, &(dg1->protocolData));
	if(err<0){ printf("Brak pliku %s!\n", filename); return err; }
	printf("[done]\n");

	/*dg1->protocolData = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolData->buf= (char *) malloc (sizeof(char) * 5);
	memset(dg1->protocolData->buf, 0, 5);
	strcpy(dg1->protocolData->buf, "dupa");
	dg1->protocolData->size=5;*/

	/* nazwa pliku */
	dg1->protocolDataFilename = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolDataFilename->buf= (unsigned char *) malloc (sizeof(char) * (1+strlen(filename)));
	memset(dg1->protocolDataFilename->buf, 0, (1+strlen(filename)));
	strcpy((char *)dg1->protocolDataFilename->buf, filename);
	dg1->protocolDataFilename->size=(1+strlen(filename));

#ifdef aaa
	/* Klucz symetryczny */
	FOK(bmd_set_ctx_sym(&sym_ctx,BMD_CRYPT_ALGO_DES3,NULL,NULL));
	FOK(bmd_symmetric_encrypt(dg1->protocolData,&sym_ctx,&enc,NULL));
	free_gen_buf(&(dg1->protocolData));
	set_gen_buf2(enc->buf,enc->size,&(dg1->protocolData));
	free_gen_buf(&enc);
	/* 1. Certyfikat = pfx */
	FOK(bmd_set_ctx_file("marua.pfx","12345678",8,&bmd_ctx));
	FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,bmd_ctx,
				sym_ctx->sym->key,sym_ctx->sym->IV,NULL,&tmp));
	FOK(bmd2_datagram_set_symkey(tmp,&dg1));
	bmd_ctx_destroy(&sym_ctx);
#endif

	/* owner pliku */
	dg1->protocolDataOwner = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolDataOwner->buf= (char *) malloc (sizeof(char) * 9);
	memset(dg1->protocolDataOwner->buf, 0, 4);
	strcpy(dg1->protocolDataOwner->buf, "szu");
	dg1->protocolDataOwner->size=4;

	PR2_bmdDatagram_add_metadata(oid_additional_metadata_imie,

				"Jan",
				1+strlen("Jan"),
				ADDITIONAL_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_additional_metadata_nazwisko,

				"Kowalski",
				1+strlen("Kowalski"),
				ADDITIONAL_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_additional_metadata_undefined,

				"undefined2",
				1+strlen("undefined1"),
				ADDITIONAL_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_additional_metadata_data_przyjecia,

				"2007-02-13 11:40:05",
				1+strlen("2007-02-13 11:40:05"),
				ADDITIONAL_METADATA,
				dg1,0,0,0);
	/* keyword 1*/
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_keyword,

				"slowo kluczowe 1",
				1+strlen("slowo kluczowe 1"),
				USER_METADATA,
				dg1,0,0,0);
	/* description 1*/
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_data_description,

				"opis 1",
				1+strlen("opis 1"),
				USER_METADATA,
				dg1,0,0,0);
	/* software type 1*/
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_software_types,

				"1.2.616.1.113527.4.3.1.5.3",
				1+strlen("1.2.616.1.113527.4.3.1.5.3"),
				USER_METADATA,
				dg1,0,0,0);
	/* doc type 1*/
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_doc_types,

				"1.2.616.1.113527.4.3.1.4.1",
				1+strlen("1.2.616.1.113527.4.3.1.4.1"),
				USER_METADATA,
				dg1,0,0,0);
	/* group 1*/
	PR2_bmdDatagram_add_metadata(oid_sys_metadata_groups,

				"1.2.616.1.113527.4.3.2.3.1",
				1+strlen("1.2.616.1.113527.4.3.2.3.1"),
				SYS_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_sys_metadata_sec_categories,

				"1.2.616.1.113527.4.3.2.1.1",
				1+strlen("1.2.616.1.113527.4.3.2.1.1"),
				SYS_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_sys_metadata_sec_levels,

				"1.2.616.1.113527.4.3.2.4.2",
				1+strlen("1.2.616.1.113527.4.3.2.4.2"),
				SYS_METADATA,
				dg1,0,0,0);
/*	PR2_bmdDatagram_add_metadata(oid_sys_metadata_roles,
				sizeof(oid_sys_metadata_roles),
				"1.2.616.1.113527.4.3.1.8.1",
				1+strlen("1.2.616.1.113527.4.3.1.8.1"),
				SYS_METADATA,
				dg1,0,0,0);*/
/*	PR2_bmdDatagram_add_metadata(oid_sys_metadata_roles,
				sizeof(oid_sys_metadata_roles),
				"1.2.616.1.113527.4.3.1.8.2",
				1+strlen("1.2.616.1.113527.4.3.1.8.2"),
				SYS_METADATA,
				dg1,0,0,0);
*/
	PR2_bmdDatagram_add_metadata(oid_sys_metadata_roles,

				"administrator",
				1+strlen("administrator"),
				SYS_METADATA,
				dg1,0,0,0);

	/* Dane dotyczace sprawy */
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_type,

				"high priority case",
				1+strlen("high priority case"),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_code,

				"2006/12/31-CaseCode1",
				1+strlen("2006/12/31-CaseCode1"),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_code_passwd,

				"12345678",
				1+strlen("12345678"),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_title,

				"Tytul sprawy: Notatka sluzbowa",
				1+strlen("Tytul sprawy: Notatka sluzbowa"),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_case_description,

				"Notatka sluzbowa ze spotkana w sprawie BAE.",
				1+strlen("Notatka sluzbowa ze spotkana w sprawie BAE."),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_parent_id,
				sizeof(oid_usr_metadata_cases_parent_id),
				"1",
				1+strlen("1"),
				USER_METADATA,
				dg1,0,0,0);
	PR2_bmdDatagram_add_metadata(oid_usr_metadata_cases_relation_type,

				"1.2.616.1.113527.4.3.1.7.2",
				1+strlen("1.2.616.1.113527.4.3.1.7.2"),
				USER_METADATA,
				dg1,0,0,0);

	/* Metadane PKI */
/*	PR2_bmdDatagram_add_metadata(oid_pki_metadata_timestamp,
				sizeof(oid_pki_metadata_timestamp),
				timestampFile->buf,
				timestampFile->size,
				PKI_METADATA,
				dg1,0,0,0);
*//*	PR2_bmdDatagram_add_metadata(oid_pki_metadata_crl,
				sizeof(oid_pki_metadata_crl),
				crlFile->buf,
				crlFile->size,
				PKI_METADATA,
				dg1,0,0,0);
*//*	PR2_bmdDatagram_add_metadata(oid_pki_metadata_signature,
				sizeof(oid_pki_metadata_signature),
				sigFile->buf,
				sigFile->size,
				PKI_METADATA,
				dg1,0,0,0);
*//*	PR2_bmdDatagram_add_metadata(oid_pki_metadata_certificate,
				sizeof(oid_pki_metadata_certificate),
				certFile->buf,
				certFile->size,
				PKI_METADATA,
				dg1,0,0,0);
*/
/*	PR2_bmdDatagram_print(dg1, WITHOUT_TIME);*/

	err = _PR2_add_bmdDatagram_to_set(dg1, set1);
	if(err<0){
		printf("Error in adding into set\n");
		return err;
	}
	PR_bmdDatagramSet_print(set1, WITHOUT_TIME);

	GenBuf_t *out = NULL;
/*	err=PR2_bmdDatagram_serialize(dg1,&out);
	if(err<0){
		printf("Error in datagram\n");
		return err;
	}
*/
	err = PR2_bmdDatagramSet_serialize(set1, &out);
	if(err<0){
		printf("Error in encoding set\n");
		return err;
	}


/*
	err=_PR2_bmdDatagramSet_der_encode(set1,&out);
	if(err<0){
		printf("Error in encoding set\n");
		return err;
	}
*/

	/********************************************************/
	/* CLUE TESTU	WRZUCAM DATAGRAM			*/
	/********************************************************/
#ifdef aaa
	/*insert_data_t data = { 0, "http://192.168.128.81/tsserver/", 80, bmd_ctx };*/
	/*2008.02.17 Tomasz Wojciechowski Zmodyfikowane na potrzeby znakowania czasem podpisu.*/
	insert_data_t data = { 0, "time.certum.pl", 80, "time.certum.pl", 80, bmd_ctx };
	/*\2008.02.17 Tomasz Wojciechowski Zmodyfikowane na potrzeby znakowania czasem podpisu.*/
	/*insert_data_t data = { 0, NULL, -1, bmd_ctx };*/
	printf("Przed JS main... ");
	err = JS_main(db_conn_info, set1, &set2, NULL, NULL, &data);
	if(err<0){
		PRINT_DEBUG("TESTERR[%s:%i%s] JS Main zakonczyl sie z bledem!!! Kod bledu = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("po JS main\n");
	PR_bmdDatagramSet_print(set2, WITHOUT_TIME);

{
	#define CRL_SIZE 20
	GenBuf_t CRLGenBuf;
	CRLGenBuf.buf = (char *) malloc (sizeof(char) * CRL_SIZE);
	strcpy(CRLGenBuf.buf, "PRzykladowy CRL");
	CRLGenBuf.size = CRL_SIZE;

	err = main_insert_CRL(db_conn_info, &CRLGenBuf);
	if(err < 0){
		PRINT_DEBUG("TESTERR[%s:%i%s] main_insert_CRL zakonczyl sie z bledem!!! Kod bledu = %i\n", err);
		return err;
	}
}

	bmd_ctx_destroy(&bmd_ctx);
#endif

/*	free(bmdDatagramDerEncoded->buf);
	free(bmdDatagramDerEncoded);
*/

	if(set1) { PR2_bmdDatagramSet_free(&set1); }
	if(set2) { PR2_bmdDatagramSet_free(&set2); }


/*	PR_bmdDatagramSet_free(set3, 1); set3=NULL;*/
}
	bmd_db_disconnect(&db_conn_info);
	free_gen_buf(&timestampFile);
	free_gen_buf(&crlFile);
	free_gen_buf(&certFile);
	free_gen_buf(&sigFile);
	if(filename) {free(filename); filename=NULL;}
	bmd_end();
	return err;
}

#define INDEX_OPT_CONF_FILE  1
#define INDEX_CONF_FILE 0

int main_insert_CRL(void *db_conn_info, GenBuf_t *CRLGenBuf)
{
int err = 0;
CrlTypeEnum_t CrlTypeEnum = TYPE_CRL;

	err = prfParseAndInsertCrl(db_conn_info, CRLGenBuf, CrlTypeEnum);
	if(err<0){
		printf("ERR[%s:%i:%s] Parse and insert CRL error. Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	/* i to samo jako ARL. */
	err = prfParseAndInsertCrl(db_conn_info, CRLGenBuf, TYPE_ARL);
	if(err<0){
		printf("ERR[%s:%i:%s] Parse and insert ARL error. Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

int read_GenBuf_from_file(char *filename, GenBuf_t **gbuf)
{
int err=0, fd=0, readed=0;
struct stat filestat;
	*gbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*gbuf==NULL) return -1;

	err = stat(filename, &filestat);
	if (err) return -2;

	fd = open(filename, O_RDONLY, 0666);
	if(fd==-1) return -2;

	(*gbuf)->buf = (unsigned char *) malloc (sizeof(unsigned char) * filestat.st_size + 2);
	memset((*gbuf)->buf, 0 , filestat.st_size + 1);
	if((*gbuf)->buf==NULL) return -1;
	(*gbuf)->size=filestat.st_size;

	if ( (readed=read(fd,(*gbuf)->buf,(*gbuf)->size)) == -1 ){
		return -3;
	}
	close(fd);
	return err;
}



