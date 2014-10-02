/****************************** PKL *************************************/
/*	Program sluzacy do przeszyfrowania zawartosci bazy danych	*/
/****************************** PKL *************************************/

#include <bmd/libbmdsql/common.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdts/libbmdts.h>
#include <stdlib.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>



#define DEBUG(fmt,args...)\
	{\
		fflush(stdout);\
		printf(fmt,##args );\
		fflush(stdout);\
	}

#define RENC_FOK(x)\
	{\
		int stat;\
		stat=x;\
		if (stat<BMD_OK)\
		{\
			printf("FAILED (%i)\n",stat);\
			return stat;\
		}\
		printf("OK\n");\
	}

#define RENC_FOK_DB(x)\
	{\
		int stat;\
		stat=x;\
		if (stat<BMD_OK)\
		{\
			printf("FAILED (%i)\n",stat);\
			bmd_db_disconnect(&hDB);\
			return stat;\
		}\
		printf("OK\n");\
	}


int parse_command_line(int argc, char *argv[], char ***params, int switcher_no, char *switchers[])
{
FILE *ptr	= NULL;
int i		= 0;
int j		= 0;

	if (argc!=(switcher_no*2+1))
	{
		printf("\n----------- Rencryption software for BMD 1.7.1 (Unizeto Technologies SA) ------------\n");
		printf("executive parameters:\n");
		printf("\t%s level\t\tdebug level\n",switchers[0]);
		printf("\t%s file\t\t\tbmd config file\n",switchers[1]);
		printf("\t%s .pfx file\t\tcertificate for rencrypting from\n",switchers[2]);
		printf("\t%s .pfx file\t\tcertificate for rencrypting to\n",switchers[3]);
		printf("\t%s password\t\tpassword for certificate for rencrypting from\n",switchers[4]);
		printf("\t%s password\t\tpassword for certificate for rencrypting to\n",switchers[5]);
		printf("\t%s id number\tfile's id number to rencrypt database from\n",switchers[6]);
		printf("\t%s id number\tfile's id number to rencrypt database to\n",switchers[7]);
		printf("-------------------------------------------------------------------------------------\n\n");
		printf("Error: Invalid parameters\n");
		return -1;
	}
	/********************************/
	/*	pasowanie linii polecen	*/
	/********************************/
	(*params)=(char **)malloc(switcher_no*sizeof(char*));
	for (i=1; i<argc; i++)
	{
		for (j=0; j<switcher_no; j++)
		{
			if (strcmp(argv[i],switchers[j])==0)
			{
				asprintf(&((*params)[j]),"%s",argv[i+1]);
				break;
			}
		}
	}

	/************************************************/
	/*	sprawdzenie poprawnosci parametrow	*/
	/************************************************/
	for (i=0; i<strlen((*params)[0]); i++)
	{
		if ((((*params)[0][i]<'0') || ((*params)[0][i]>'9')) && (*params)[0][i]!='-')
		{
			PRINT_ERROR("Invalid debug level format\n");
			return -2;
		}
	}

	_GLOBAL_debug_level = atoi((*params)[0]);
	if ((ptr=fopen((*params)[1],"r"))==NULL)
	{
		PRINT_ERROR("Cannot open configuration file\n");
		return -3;
	}
	else fclose(ptr);

	if ((ptr=fopen((*params)[2],"r"))==NULL)
	{
		PRINT_ERROR("Cannot open pfx file %s\n",(*params)[2]);
		return -4;
	}
	else fclose(ptr);

	if ((ptr=fopen((*params)[3],"r"))==NULL)
	{
		PRINT_ERROR("Cannot open pfx file %s\n",(*params)[3]);
		return -5;
	}
	else fclose(ptr);

	return 0;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
int main(int argc, char *argv[])
{
long switcher_no				= 8;
long ans_count				= 0;
long i					= 0;
char *switchers[] 			= {"-d","-c","-p","-b","-pp","-pb", "-id_from", "-id_to"};
char *lobid					= NULL;
char **params				= NULL;
char *SQLQuery				= NULL;
char *ans2					= NULL;
char *ans3					= NULL;
void *hDB		= NULL;
GenBuf_t *symkey_encrypted		= NULL;
bmd_crypt_ctx_t *symkey_decrypted	= NULL;
GenBuf_t *symkey_rencrypted		= NULL;
bmd_crypt_ctx_t *oldctx			= NULL;
bmd_crypt_ctx_t *newctx			= NULL;
char **ans					= NULL;

	/****************************/
	/* parsowanie linii polecen */
	/****************************/
	DEBUG("parsing command line...                      ");
	RENC_FOK(parse_command_line(argc,argv,&params,switcher_no,switchers));

	for (i=0; i<switcher_no; i++)
	{
		if ((params[i]==NULL) || (strcmp(params[i],"")==0))
		{
			PRINT_ERROR("RENCRYPTIONERR Invalid parameter value given at %li position\n",i);
			for (i=0; i<switcher_no; i++)
			{
				if (params[i])	{free(params[i]); params[i]=NULL;}
			}
			free(params); params=NULL;
			return -1;
		}
	}

	/***************************/
	/* stworzenie konfiguracji */
	/***************************/
	DEBUG("creating configuration struture...           ");
	//RENC_FOK(bmd_server_configuration_create(&_GLOBAL_bmd_configuration));

	/*************************************/
	/* pobranie konfiguracji serwera BMD */
	/*************************************/
	DEBUG("reading configuration values...              ");
//	RENC_FOK(bmd_get_server_configuration(params[1],_GLOBAL_bmd_configuration, TIME_FIRST));

	/****************************/
	/* polaczenie z baza danych */
	/****************************/
	DEBUG("initializing database library...             ");
	if( _GLOBAL_bmd_configuration->db_library == NULL )
	{
		RENC_FOK(bmd_db_init("libbmddb_postgres.so"));
	}
	else
	{
		RENC_FOK(bmd_db_init(_GLOBAL_bmd_configuration->db_library));
	}

	memset(&hDB,0,sizeof(char *)+2);
	DEBUG("connecting to database...                    ");
	RENC_FOK(bmd_db_connect(_GLOBAL_bmd_configuration->db_conninfo, &hDB));

	/****************************************************/
	/* wyszukanie oidow wszystkich kluczy symetrycznych */
	/****************************************************/
	asprintf(&SQLQuery, "SELECT symkey FROM crypto_objects WHERE id>=%s AND id <=%s;", params[6], params[7]);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	DEBUG("retrieving cryto objects id list...          ");
	RENC_FOK_DB(ExecuteSQLQueryWithAnswersKnownDBConnection(	hDB, SQLQuery, &ans_count, &ans));
	free(SQLQuery); SQLQuery=NULL;

	printf("\n\n\n");
	for (i=0; i<ans_count; i++)
	{
		/***********************/
		/* wyzerowanie pamieci */
		/***********************/
		bmd_ctx_destroy(&newctx);
		bmd_ctx_destroy(&oldctx);
		free(ans2); ans2=NULL;
		free(ans3); ans3=NULL;
		free(lobid); lobid=NULL;
		free_gen_buf(&symkey_encrypted);
		free_gen_buf(&symkey_rencrypted);

		/****************************************************/
		/* sprawdzenie, czy lob znajduje sie w bazie danych */
		/****************************************************/
		asprintf(&SQLQuery,"SELECT DISTINCT loid FROM pg_largeobject WHERE loid=%s;", ans[i]);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_DB(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &ans2));
		free(SQLQuery); SQLQuery=NULL;

		DEBUG("importing and rencrypting symkey's lob (%.7li/%.7li) (lob id:%.7s)...          ", 	i+1, ans_count, ans2);
		/****************************************/
		/*	importuj zaszyfrowany symkey	*/
		/****************************************/
		RENC_FOK_DB(bmd_db_import_blob(hDB, ans2, &symkey_encrypted));

		/***********************************/
		/* zdeszyfrowanie pobranego klucza */
		/***********************************/
		DEBUG("preparing decrypting certificate...                                                   ");
		RENC_FOK_DB(bmd_set_ctx_file(params[2], params[4], strlen(params[4]), &(oldctx)));

		DEBUG("decrypting private key with decrypting certificate...                                 ");
		RENC_FOK_DB(bmd_decode_bmdKeyIV(symkey_encrypted, NULL, oldctx, &symkey_decrypted));

		/*******************************/
		/* zaszyfrowanie nowym kluczem */
		/*******************************/
		DEBUG("preparing encrypting certificate...                                                   ");
		RENC_FOK_DB(bmd_set_ctx_file(params[3], params[5], strlen(params[5]), &(newctx)));

		DEBUG("encrypting private key with encrypting certificate...                                 ");
		RENC_FOK_DB(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA, BMD_CRYPT_ALGO_DES3, newctx, symkey_decrypted->sym->key, symkey_decrypted->sym->IV, NULL, &symkey_rencrypted));

		/****************************************/
		/* zapisanie nowego loba w bazie danych */
		/****************************************/
		DEBUG("exporting new symmetric key to data base...                                           ");
		RENC_FOK_DB(bmd_db_export_blob(hDB, symkey_rencrypted, &lobid));

		/****************************************/
		/* zmiana id symkeya w crypto_objectach */
		/****************************************/
		asprintf(&SQLQuery,"UPDATE crypto_objects SET symkey=%s WHERE symkey=%s;",lobid,ans2);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &ans3);
		free(ans3); ans3=NULL;
		free(SQLQuery); SQLQuery=NULL;

		/*********************************************/
		/* usuniecie poprzedniego loba z bazy danych */
		/*********************************************/
		asprintf(&SQLQuery,"SELECT lo_unlink(%s);",ans2);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &ans3);
		free(ans3); ans3=NULL;
		free(SQLQuery); SQLQuery=NULL;

		bmd_ctx_destroy(&symkey_decrypted);

		printf("\n");
	}

	bmd_db_disconnect(&hDB);

	/************/
	/* porzadki */
	/************/
	free(hDB); hDB=NULL;
	bmd_ctx_destroy(&newctx); newctx=NULL;
	bmd_ctx_destroy(&oldctx); oldctx=NULL;
	free_gen_buf(&symkey_encrypted);
	free_gen_buf(&symkey_rencrypted);
	free(ans); ans=NULL;
	free(ans2); ans2=NULL;
	free(ans3); ans3=NULL;

	/************/
	/* porzadki */
	/************/
	for (i=0; i<switcher_no; i++)
	{
		free(params[i]);params[i]=NULL;
	}

	free(params); params=NULL;
	return BMD_OK;
}
