#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdsql/cases.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdsql/group.h>
#include <bmd/libbmdsql/user.h>
#include <bmd/libbmdsql/timestamp.h>
#include <bmd/libbmdlob/libbmdlob.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif
/* TODO zastanowic sie dokladnie kiedy nalezy wychodic returnem i zamykac watek, a kiedy cofac transakcje	*/
/* TODO umozliwic _szybkie_ dodawanie additional metadata							*/
/* TODO umozliwic skladanie i pobieranie + selekcje bez metadanych						*/

#define ERR_UNRECOGNIZED_PKI_METADATA_OID -701

long PR_bmdDatagram_SQL_Insert_Err(	bmdDatagram_t *bmdJSResponse,
						long dtg_type,
						long err_code)
{
	PR_bmdDatagram_init(bmdJSResponse);
	bmdJSResponse->datagramType=dtg_type;
	bmdJSResponse->datagramStatus=err_code;

	return BMD_OK;
}

long PR_bmdDatagram_SQL_Insert_Ok(	bmdDatagram_t *bmdJSResponse,
						long dtg_type)
{
	/** @bug to kasowalo hasha z datagramu!! */
	/* PR_bmdDatagram_init(bmdJSResponse); */
	bmdJSResponse->datagramType=dtg_type;
	bmdJSResponse->datagramStatus=BMD_OK;
	return BMD_OK;
}

long PR_bmdDatagram_SQL_Nop_Ok(bmdDatagram_t *bmdJSResponse,long dtg_type)
{
	bmdJSResponse->datagramType	= dtg_type;
	bmdJSResponse->datagramStatus	= BMD_OK;
	return BMD_OK;
}


/************************************************/
/*		INSERT FUNCTIONS		*/
/************************************************/
#ifndef WIN32
long JS_bmdDatagramSet_to_insert(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					LobsInDatagram_t LobsInDatagram,
					bmdnet_handler_ptr handler,
					insert_data_t *data,
					server_request_data_t *req)
#else /*WIN32*/
long JS_bmdDatagramSet_to_insert(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					LobsInDatagram_t LobsInDatagram,
					bmdnet_handler_ptr handler,
					insert_data_t *data,
					server_request_data_t *req,
					HANDLE *threadHandler
					)
#endif /*ifndef WIN32*/
{
long i			= 0;
long status		= 0;
long interrupt		= 0;

	/************************************************/
	/*	przygotowanie datagramu odpowiedzi	*/
	/************************************************/
//__debugbreak();
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	//#ifdef WIN32
	//		status=WaitForSingleObject(_GLOBAL_bmd_configuration->hInsertMutex,INFINITE);
	//		PRINT_DEBUG("MSILEWICZ_THR_DEBUG WaitForSingleObject %li\n",status);
	//#endif
	/*************************************/
	/* obsluga poszczegolnych datagramow */
	/*************************************/
	for(i=0; i < bmdJSRequestSet->bmdDatagramSetSize; i++)
	{

		/********************************************************************************************
		* Poczatek transakcji									    *
		********************************************************************************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/****************************************/
		/*	wrzucenie pliku do bazy danych	*/
		/****************************************/
		if (interrupt==0)
		{

			status=JS_bmdDatagram_to_insert(	hDB,
								bmdJSRequestSet->bmdDatagramSetTable[i],
								&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),
								LobsInDatagram,
								handler,
								data,
								req);

			if (status==LOB_CANCELED_BY_CLIENT)
			{
				Q_DONE("Inserting canceled by client\n")
				PRINT_WARNING("LIBBMDSQLWRN Sending files canceled by client. Status: %li\n",status);
				interrupt=1;
			}

		}
		else
		{
			status=LOB_CANCELED_BY_CLIENT;
		}


		/************************************************/
		/*	przygotowanie datagramu odpowiedzi	*/
		/************************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus= status;

		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFilename=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFilename->buf=(char *)malloc(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFilename->size+2);
		memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFilename->buf,bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFilename->buf,bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFilename->size+1);
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFilename->size=bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFilename->size;

		/************************************************/
		/* przepisanie zadajacego do odpowiedzi serwera */
		/************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner!=NULL)
		{
			if (bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->buf!=NULL)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf=(char*)malloc(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->size+2);
				memset((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf, 0,bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->size+1);
				memmove((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->size);
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->size=bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataOwner->size;
			}
		}

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
			}
		}

		if ((*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
		}
		else
		{
			bmd_db_end_transaction(hDB);
		}
	}

	//#ifdef WIN32
	//		ReleaseMutex(_GLOBAL_bmd_configuration->hInsertMutex);
	//#endif
	/*************************************/
	/* wysylanie awizo w postaci e-maili */
	/*************************************/
	if(_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post)
	{
#ifndef WIN32
			_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
												(char *)_GLOBAL_bmd_configuration->db_conninfo, 1,1,0, req);
#else /*WIN32*/
			_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
												(char *)_GLOBAL_bmd_configuration->db_conninfo, 1,1,0, req, threadHandler);
#endif /*ifndef WIN32*/
	}

	return BMD_OK;
}

long JS_bmdDatagram_to_insert(	void *hDB,
				bmdDatagram_t * bmdJSRequest,
				bmdDatagram_t ** bmdJSResponse,
				LobsInDatagram_t LobsInDatagram,
				bmdnet_handler_ptr handler,
				insert_data_t *data,
				server_request_data_t *req)
{
long status			= 0;
u_int64_t lob_size		= 0;
char *lobjId			= NULL;
char *MaxId			= NULL;
char *hash_as_hex_string	= NULL;
char *crypto_object_id		= NULL;
char *dvcs			= NULL;
char *correspondingId		= NULL; //na potrzeby tworzenia linka (jedyna metadata systemowa, ktora jest kopiowana)
GenBuf_t gb;
char *newHashHexString		= NULL; // skrot zmienia sie po podmianie pdf w przypadku umieszczania znacznika czasu wewnatrz PAdES 
long iter			= 0;



	memset( &gb, 0, sizeof( gb ) );

	PRINT_NOTICE("LIBBMDSQLINF Begining database transaction\n");

	// przy wstawianu, wartosci metadanych dodatkowych nie sa potrzebne (dwa ostatnie parametry sa ignorowane)
	BMD_FOK(VerifyAdditionalMetadataTypeAndValue(bmdJSRequest, req, NULL, 0));

	// aktualizacja czasu ostatniego wstawienia (na potrzeby procesu lob remover odpowiedzialnego za usuwanie zastapianych lobow)
	if(_GLOBAL_bmd_configuration != NULL && _GLOBAL_bmd_configuration->clear_plugin_conf.bmd_update_last_insertion_time != NULL)
	{
		_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_update_last_insertion_time();
	}
	
	/****************************************************************************************
	* 1. Na poczatek wstawiamy large object z pola protocol data. W zamian otrzymujemy 	*
	* Oid large objecta - lobjId, ktory posluzy do dalszej pracy na tabelach		*
	****************************************************************************************/
	status=InsertLOBandTimestampIntoDatabase(	hDB,
							&bmdJSRequest,
							*bmdJSResponse,
							LobsInDatagram,
							&lobjId,
							&correspondingId,
							handler,
							&hash_as_hex_string,
							data,
							&lob_size,
							req);
	/*********************************************************************/
	/* jesli wczesniej nie wykryto bledu w datagramie zadania	     */
	/* dopiero tutaj, poniewaz konieczne jest odebranie lobow od klienta,*/
	/* zeby mozna bylo mu odeslac blad w datagramie odpowiedzi - w tym   */
	/* wypadku nie zapisujemy odebranych danych w bazie danych	     */
	/*********************************************************************/
	if ((status<BMD_OK) && (status!=LOB_CANCELED_BY_CLIENT))	{	BMD_FOK(status);	}

	if (status==LOB_CANCELED_BY_CLIENT)
	{
		if (lobjId>0)
		{
			BMD_FOK(bmd_db_delete_blob(hDB,lobjId));
		}
		BMD_FOK(LOB_CANCELED_BY_CLIENT);
	}



	/**********************************/
	/* zapamietanie odpowiedzi DVCS'a */
	/******************************************/
	asprintf(&dvcs, "%li", status);


	/********************************************************************************************
	* 2. pobierz id nowododanego elementu. Jest to ID obiektu do ktorego dalej dodawac bedziemy *
	* kolejne metadane 									    *
	********************************************************************************************/
	//BMD_FOK(bmd_db_get_max_entry(hDB, "id", "crypto_objects", &MaxId));
	void *queryResult = NULL;
	long fetched_counter=0;
	BMD_FOK(bmd_db_execute_sql(hDB, "SELECT nextval('crypto_objects_id_seq')", NULL, NULL, &queryResult));
	BMD_FOK(bmd_db_result_get_value(hDB, queryResult, 0, 0, &MaxId, FETCH_ABSOLUTE, &fetched_counter));
	bmd_db_result_free(&queryResult);
	

	/****************************************************************************************
	* 3. wstawienie wiersza crypto_objects wraz z metadanymi systemowymi
	****************************************************************************************/
	BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_CryptoObjects(hDB, bmdJSRequest, MaxId, correspondingId, lobjId, lob_size, hash_as_hex_string, req));
	free(correspondingId); correspondingId=NULL;


	/********************************************************************************************
	* 5. Wstaw metadane dodatkowe								    *
	********************************************************************************************/
	status = JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(hDB, bmdJSRequest, MaxId,req);
	if(status<BMD_OK && status != ERR_JS_NO_ADDITIONAL_METADATA_PRESENT)
	{
		BMD_FOK(status);
	}



	/****************************************************************/
	/* jesli nie tworzymy linka, wrzucamy metadane dodatkowe, i pki */
	/****************************************************************/
	if (bmdJSRequest->datagramType!=BMD_DATAGRAM_LINK_CREATE)
	{
		/********************************************************************************************
		* 6. Wstaw metadane PKI									    *
		********************************************************************************************/
		status = JS_PrepareAndExecuteSQLQuery_Insert_PkiMetaData(hDB, bmdJSRequest, MaxId, data,req, &newHashHexString);
		if(status<BMD_OK && status != ERR_JS_NO_PKI_METADATA_PRESENT)
		{
			BMD_FOK(status);
		}
	}


	/*W srodku hash_as_hex_string jest kopiowany do nowego bufora.*/
	BMD_FOK(SetHashValueAndFileIdDatagram((newHashHexString == NULL ? hash_as_hex_string : newHashHexString), &crypto_object_id, MaxId, LobsInDatagram, *bmdJSResponse, data,req));



	/******************************************************************************************/
	/*	ustawienie wlasciciela datagramu odpowiedzi na wlasciciela datagramu wejsciowego	*/
	/******************************************************************************************/
	if (bmdJSRequest->protocolDataOwner!=NULL)
	{
		if (bmdJSRequest->protocolDataOwner->buf!=NULL)
		{
			if ((*bmdJSResponse)->protocolDataOwner==NULL)
			{
				(*bmdJSResponse)->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
				(*bmdJSResponse)->protocolDataOwner->buf=(char*)malloc(bmdJSRequest->protocolDataOwner->size+2);
				memset((*bmdJSResponse)->protocolDataOwner->buf, 0, bmdJSRequest->protocolDataOwner->size+1);
				memcpy((*bmdJSResponse)->protocolDataOwner->buf, bmdJSRequest->protocolDataOwner->buf,bmdJSRequest->protocolDataOwner->size);
				(*bmdJSResponse)->protocolDataOwner->size=bmdJSRequest->protocolDataOwner->size;
			}
		}
	}

	// przy wstawianiu metadane dodatkowe sa podawane w samym zadaniu, dlatego zbednym narzutem byloby ich wyciaganie z bazy jesli mozna przekopiowac z zadania
	// BMD_FOK(FillFromAdditionalMetaData(hDB, MaxId, _GLOBAL_bmd_configuration->location_id, *bmdJSResponse, req));
	//
	for(iter=0; iter<bmdJSRequest->no_of_additionalMetaData; iter++)
	{
		GenBuf_t* metadataValue = NULL;
		BMD_FOK(set_gen_buf2(bmdJSRequest->additionalMetaData[iter]->AnyBuf, bmdJSRequest->additionalMetaData[iter]->AnySize, &metadataValue));
		BMD_FOK(AddElementToMetaDataTable(&((*bmdJSResponse)->additionalMetaData),
						&((*bmdJSResponse)->no_of_additionalMetaData),
						bmdJSRequest->additionalMetaData[iter]->OIDTableBuf,
						metadataValue,
						bmdJSRequest->additionalMetaData[iter]->myId,
						bmdJSRequest->additionalMetaData[iter]->ownerType,
						bmdJSRequest->additionalMetaData[iter]->ownerId,
						1));
		free_gen_buf(&metadataValue);
	}



	/************/
	/* porzadki */
	/************/
	free0(crypto_object_id);
	free0(lobjId);
	free0(MaxId);
	free0(hash_as_hex_string);
	free0(newHashHexString);




	if (dvcs!=NULL)
	{
		status=atol(dvcs);
		if (status>BMD_OK)
		{
			free(dvcs); dvcs=NULL;
			return status;
		}
	}




	free0(dvcs);




	return BMD_OK;
}


/*
correspondingId - w przypadku tworzenia linka na wyjsciu otrzymamy correspoondingId skopiowane z dokumentu wskazywanego;
w innych przypadkach wartosc jest nieistotna (najlepiej wtedy przekazac NULL)
*/
long InsertLOBandTimestampIntoDatabase(	void *hDB,
					bmdDatagram_t **bmdJSRequest,
					bmdDatagram_t * bmdJSResponse,
					LobsInDatagram_t LobsInDatagram,
					char **lobjId,
					char **correspondingId,
					bmdnet_handler_ptr handler,
					char **hash_as_hex_string,
					insert_data_t *data,
					u_int64_t *lob_size_for_db,
					server_request_data_t *req)
{
long i				= 0;
long status 			= 0;
long lob_chunk_size		= 0;
u_int64_t remaining_bytes	= 0;
u_int64_t lob_size		= 0;
u_int64_t counted_lob_size	= 0;
long last_lob_chunk		= -1;
long corresponding_id_present	= 0;
struct lob_transport *lob	= NULL;
#ifndef BLOB_IN_BYTEA
void *blob_locator		= NULL;
#endif
bmd_crypt_ctx_t *hash_ctx	= NULL;
bmd_crypt_ctx_t *lob_sym_ctx	= NULL;
GenBuf_t *der_lob_symkey	= NULL;
GenBuf_t *hash_buf		= NULL;
GenBuf_t *ts_buf		= NULL;
GenBuf_t *deciphered_buf	= NULL;
char timestamp_metadata_oid[]	= {OID_PKI_METADATA_TIMESTAMP};
GenBuf_t gb;
GenBuf_t *gb_ptr		= &gb;

char *corresponding_id		= NULL;
char *pointing_id		= NULL;
//char *pointing_location_id	= NULL;

char *file_type			= NULL;
char* forGrant			= NULL;

bmdDatagram_t *dtg		= NULL;
char *id			= NULL;
char *location_id		= NULL;
long addMetadataCount		= 0;
char **addMetadata		= NULL;
char **addMetadataOid		= NULL;
ts_info_t ts_info;

// ponizej zmienne potrzebne do sprawdzenia powielonych linkow
bmdStringArray_t* idArray	= NULL;
char* ownerIdentityId		= NULL;
char* ownerGroupId			= NULL;
long linksCount				= 0;

#ifdef BLOB_IN_BYTEA
GenBuf_t *large_binary_buf		= NULL;
char * buf_ptr			= NULL;
#endif

	memset( &gb, 0, sizeof( gb ) );
	/****************************************************************************************
	* 1. Na poczatek wstawiamy large object z pola protocol data. W zamian otrzymujemy 	*
	* Oid large objecta - lobjId, ktory posluzy do dalszej pracy na tabelach		*
	****************************************************************************************/
	PRINT_INFO("LIBBMDSQLINF Inserting LOB into database.\n");


	if(LobsInDatagram == LOB_IN_DATAGRAM)
	{
		if (	((*bmdJSRequest)->datagramType==BMD_DATAGRAM_DIR_CREATE) ||
			((*bmdJSRequest)->datagramType==BMD_DATAGRAM_LINK_CREATE))
		{
			/****************************************************/
			/* wypelnienie wartosci domyslnych w nowym katalogu */
			/****************************************************/
			if ((*bmdJSRequest)->protocolData == NULL)
			{
				set_gen_buf2 ((char*)"<no description>", (long)strlen("<no description>"), &((*bmdJSRequest)->protocolData));
			}

			if ((*bmdJSRequest)->datagramType==BMD_DATAGRAM_DIR_CREATE)
			{
				gb.buf=(char *)"dir";
				gb.size=(long)strlen("dir");
			}
			else if ((*bmdJSRequest)->datagramType==BMD_DATAGRAM_LINK_CREATE)
			{
				gb.buf=(char *)"link";
				gb.size=(long)strlen("link");
			}

			bmd_datagram_add_metadata_2((char *)OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE,&gb,bmdJSRequest, 0);
		}
		else
		{
			if((*bmdJSRequest)->protocolData == NULL)	{	BMD_FOK(LIBBMDSQL_INSERT_REQUIRED_LOB_NOT_PRESENT_IN_REQUEST_ERR);	}
		}


		/************************************************************************/
		/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
		/*	dopiero tutaj, poniewaz konieczne jest odebranie lobow od klienta,*/
		/*	zeby mozna bylo mu odeslac blad w datagramie odpowiedzi - w tym	*/
		/*	wypadku nie zapisujemy odebranych daych w bazie danych		*/
		/************************************************************************/
		if ((*bmdJSRequest)->datagramStatus==BMD_OK ||
			 (*bmdJSRequest)->datagramStatus==BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS)
		{
			BMD_FOK_CHG(bmd_db_export_blob(hDB, (*bmdJSRequest)->protocolData, lobjId),
					LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR);
		}
		


		/****************************************************************/
		/*	wydobycie klucza symetrycznego i jego zdekodowanie	*/
		/****************************************************************/
		BMD_FOK_CHG(bmd2_datagram_get_symkey((*bmdJSRequest),&der_lob_symkey),
				LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR);


		BMD_FOK_CHG(bmd_decode_bmdKeyIV(der_lob_symkey,NULL,data->serverPrivateKey,&lob_sym_ctx),
				LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR);


		free_gen_buf(&der_lob_symkey);


		/****************************************************************/
		/*	zdeszyfrowanie zawartosci i policzenie skrotu z niej	*/
		/****************************************************************/
		BMD_FOK_CHG(bmd_symmetric_decrypt((*bmdJSRequest)->protocolData, 0, &lob_sym_ctx,&deciphered_buf,NULL), LIBBMDSQL_INSERT_DECRYPT_PROTOCOL_DATA_ERR);

		BMD_FOK_CHG(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1), LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR);

		BMD_FOK_CHG(bmd_hash_data(deciphered_buf,&hash_ctx,&hash_buf,NULL), LIBBMDSQL_INSERT_SET_HASH_ERR);

		free_gen_buf(&deciphered_buf);


		/****************************************/
		/*	dodanie skrotu do datagramu	*/
		/****************************************/
		bmd_genbuf_hex_dump(hash_buf,hash_as_hex_string,0,1);
		//BMD_FOK(PR2_bmdDatagram_add_metadata(hash_oid,  (char *)(*hash_as_hex_string), (long)strlen(*hash_as_hex_string),ADD_METADATA, (*bmdJSRequest),0,0,0, 1));

		
		/************************************/
		/*	oznaczenie czasem zawartosci	*/
		/************************************/
		/******************************************************************/
		/*	sprawdzamy, czy plik jest plikiem towarzyszacym	lub linkiem */
		/******************************************************************/
		for(i=0; i<(*bmdJSRequest)->no_of_sysMetaData; i++)
		{

			if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)
			{
				(*bmdJSRequest)->sysMetaData[i]->AnyBuf[(*bmdJSRequest)->sysMetaData[i]->AnySize]='\0';
				asprintf(&corresponding_id, "%s", (*bmdJSRequest)->sysMetaData[i]->AnyBuf);
				if (corresponding_id==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
			else if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)
			{
				(*bmdJSRequest)->sysMetaData[i]->AnyBuf[(*bmdJSRequest)->sysMetaData[i]->AnySize]='\0';
				asprintf(&pointing_id, "%s", (*bmdJSRequest)->sysMetaData[i]->AnyBuf);
				if (pointing_id==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
// 			else if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)
// 			{
// 				(*bmdJSRequest)->sysMetaData[i]->AnyBuf[(*bmdJSRequest)->sysMetaData[i]->AnySize]='\0';
// 				asprintf(&pointing_location_id, "%s", (*bmdJSRequest)->sysMetaData[i]->AnyBuf);
// 				if (pointing_location_id==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 			}

		}


		if ((*bmdJSRequest)->datagramType==BMD_DATAGRAM_LINK_CREATE)
		{
			if (pointing_id==NULL)		{	BMD_FOK(LIBBMDSQL_INSERT_NO_POINTING_ID_IN_LINK_CREATION);	}
			//if (pointing_location_id==NULL)	{	BMD_FOK(LIBBMDSQL_INSERT_NO_POINTING_LOCATION_ID_IN_LINK_CREATION);	}


			BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, pointing_id, 
//				((*bmdJSRequest)->datagramStatus==BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS ? CONSIDER_CURRENT_GROUP : CONSIDER_GROUPS_GRAPH),
				CONSIDER_ONLY_OWNER,
				NULL, &file_type, &id, correspondingId, &forGrant,
				&addMetadataCount, &addMetadata));

			if(strcmp(forGrant, "0") == 0)
			{
				BMD_FOK(LIBBMDSQL_LINK_CREATE_FORBIDDEN);
			}

			free(forGrant); forGrant=NULL;

			/***********************************************/
			/* pobranie listy oidow metadanych dodatkowych */
			/***********************************************/
			//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
			BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataCount, &addMetadataOid));

			/**********************************************/
			/* usuniecie metadanych dodatkowych z zadania */
 			/**********************************************/
			for (i=0; i<(*bmdJSRequest)->no_of_additionalMetaData; i++)
			{
				if((*bmdJSRequest)->additionalMetaData[i])
				{
					if ((*bmdJSRequest)->additionalMetaData[i]->OIDTableBuf)
					{
						free0((*bmdJSRequest)->additionalMetaData[i]->OIDTableBuf);
					}
					if ((*bmdJSRequest)->additionalMetaData[i]->AnyBuf)
					{
						(*bmdJSRequest)->additionalMetaData[i]->AnyBuf=NULL;
						(*bmdJSRequest)->additionalMetaData[i]->AnySize=0;
					}
					free0((*bmdJSRequest)->additionalMetaData[i]);
				}
			}
			free0((*bmdJSRequest)->additionalMetaData);
			(*bmdJSRequest)->no_of_additionalMetaData=0;


			/*****************************************************************/
			/* przepisanie metadanych pliku wskazywanego do metadanych linka */
			/*****************************************************************/
			for (i=0; i<addMetadataCount; i++)
			{
				if (strlen(addMetadata[i])>0)
				{
					BMD_FOK(bmd_datagram_add_metadata_char(	addMetadataOid[i], addMetadata[i], bmdJSRequest));
				}
			}


			if (strcmp(file_type, "link")==0)
			{
				for(i=0; i<(*bmdJSRequest)->no_of_sysMetaData; i++)
				{
					if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)
					{
						free0((*bmdJSRequest)->sysMetaData[i]->AnyBuf);
						asprintf(&((*bmdJSRequest)->sysMetaData[i]->AnyBuf), "%s", id);
						if ((*bmdJSRequest)->sysMetaData[i]->AnyBuf==NULL)	{	BMD_FOK(NO_MEMORY);	}
						(*bmdJSRequest)->sysMetaData[i]->AnySize=(long)strlen((*bmdJSRequest)->sysMetaData[i]->AnyBuf);

					}
/*					else if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)
					{
						free0((*bmdJSRequest)->sysMetaData[i]->AnyBuf);
						asprintf(&((*bmdJSRequest)->sysMetaData[i]->AnyBuf), "%s", location_id);
						if ((*bmdJSRequest)->sysMetaData[i]->AnyBuf==NULL)	{	BMD_FOK(NO_MEMORY);	}
						(*bmdJSRequest)->sysMetaData[i]->AnySize=(long)strlen((*bmdJSRequest)->sysMetaData[i]->AnyBuf);
					}*/
				}
			}

			// sprawdzenie, czy link juz istnieje w systemie
			if(_GLOBAL_bmd_configuration == NULL)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }
			if(req == NULL || req->ua == NULL || req->ua->identityId == NULL || req->ua->identityId->buf == NULL)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }
			status=bmdStringArray_Create(&idArray);
			if(status < 0)
				{ BMD_FOK(NO_MEMORY); }
			status=bmdStringArray_AddItem(idArray, id);
			if(status < 0)
			{
				bmdStringArray_DestroyList(&idArray);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			status=GetOwnerFromRequestDatagram(hDB, *bmdJSRequest, req->ua->identityId->buf, &ownerIdentityId, &ownerGroupId);
			if(status < BMD_OK)
			{
				bmdStringArray_DestroyList(&idArray);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			status=IgnoreDuplicateLinks(hDB, _GLOBAL_bmd_configuration->location_id, req->ua->identityId->buf, ownerIdentityId, ownerGroupId, &idArray);
			free(ownerIdentityId); ownerIdentityId=NULL;
			free(ownerGroupId); ownerGroupId=NULL;
			if(status < 0)
			{
				PRINT_ERROR("LIBBMDSQLERR Error in ignoring duplicated links (%li)\n", status);
				bmdStringArray_DestroyList(&idArray);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			
			status=bmdStringArray_GetItemsCount(idArray, &linksCount);
			bmdStringArray_DestroyList(&idArray);
			if(status < 0)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }

			if(linksCount == 0) // stwierdzono duplikat, wiec pominieto tworzenie
			{
				PRINT_NOTICE("LIBBMDSQL No link left to create after ignoring duplicate\n");
				BMD_FOK(LIBBMDSQL_NO_LINK_CREATED);
			}
			linksCount=0;
			// sprawdzenie, czy link juz istnieje w systemie (koniec)

			bmd_datagram_free(&dtg);
			free0(id);
			free0(location_id);
			free0(file_type);

		}


		/******************************************************************************/
		/*	jesli wsadzamy plik towarzyszacy - znakujemy go innym znacznikiem czasu	*/
		/******************************************************************************/
		if (corresponding_id_present==1)
		{

			if ((*bmdJSRequest)->datagramType!=BMD_DATAGRAM_LINK_CREATE)
			{

				if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
				{
					if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
					{
						BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hash_buf, ADD_TS, HASH_DATA, &ts_buf));
						if (ts_buf!=NULL)
						{
							BMD_FOK(PR2_bmdDatagram_add_metadata(timestamp_metadata_oid, ts_buf->buf,ts_buf->size, PKI_METADATA, (*bmdJSRequest),0,0,0, 1));
							ts_buf->buf = NULL; ts_buf->size = 0;/*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
							free_gen_buf(&ts_buf);
						}
					}
				}
			}
		}
		/************************************************************************************/
		/*	jesli wsadzamy plik nie towarzyszacy - znakujemy zwyklym znacznikiem czasu	*/
		/************************************************************************************/
		else
		{

			if ((*bmdJSRequest)->datagramType!=BMD_DATAGRAM_LINK_CREATE)
			{

				if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
				{
					if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
					{
						BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hash_buf, MAIN_TS, HASH_DATA, &ts_buf));
						if (ts_buf!=NULL)
						{
							BMD_FOK(PR2_bmdDatagram_add_metadata(timestamp_metadata_oid, ts_buf->buf,ts_buf->size, PKI_METADATA, (*bmdJSRequest),0,0,0, 1));
							ts_buf->buf = NULL;ts_buf->size=0;/*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
							free_gen_buf(&ts_buf);
						}
					}
				}

			}
		}
		free_gen_buf(&hash_buf);
		*lob_size_for_db=(*bmdJSRequest)->protocolData->size;


	}
	else /* LOB NOT IN DATAGRAM - przesylamy go strumieniowo */
	{
		PRINT_INFO("LIBBMDSQLINF Inserting streamed LOB into database.\n");
		BMD_FOK_CHG(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1),
				LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR);

		hash_ctx->hash->hash_params=BMD_HASH_MULTI_ROUND;

		BMD_FOK_CHG(bmd2_datagram_get_symkey((*bmdJSRequest),&der_lob_symkey),
				LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR);

		/* stworzenie wlasciwego kontekstu z zaznaczeniem ze szyfrowanie jest wielorundowe */
		BMD_FOK_CHG(bmd_decode_bmdKeyIV(der_lob_symkey,NULL,data->serverPrivateKey,&lob_sym_ctx),
				LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR);

		lob_sym_ctx->sym->sym_params=BMD_SYM_MULTI_ROUND;
		free_gen_buf(&der_lob_symkey);
		BMD_FOK_CHG(new_lob_transport( &lob, &handler, 0, lob_sym_ctx, hash_ctx, &ts_info, CALC_HASH|TS_PLAIN_DATA),
				LIBBMDSQL_INSERT_CREATE_LOB_TRANSPORT_INFO_ERR);

		gb_ptr=0;

		/* Tu zaczyna sie obsluga strumienia danych.
		 * Filozofia umieszczania plikow w bazie danych Oracle jest nieco przekombinowana.
		 * Na poczatku nalezy okreslic czy wielkosc pliku jest na tyle mala, ze mozna go umiescic
		 * bez uzycia funkcji strumieniowych - tylko w jednej iteracji.
		 * Jesli plik jest wiekszy niz krytyczny rozmiar chunka bazodanowego wowczas rozpoczynamy
		 * przetwarzanie strumieniowe. Musimy umiec rozpoznac pierwszy i ostatni fragment strumienia
		 * oraz fragmenty posrednie.
		 */

		/* Pobieramy dane z LOBa i patrzymy czy jest mniejszy czy wiekszy niz bazodanowy chunksize */
		/* Juz po pierwszym wywolaniu funkcji recv_lob_chunk mozemy odczytac calkowity rozmiar pliku
		 * ze struktury lob->tfs
		 */
		status=recv_lob_chunk( &lob, &gb_ptr, 0, LARGE_FILE_DB_CHUNK_SIZE+1024);
		//PRINT_ERROR("BMDSERVERINF ROL_DBG LOB recv size %li\n",gb_ptr->size);
		if (status==LOB_CANCELED_BY_CLIENT)
		{
			free_lob_transport( &lob );

			BMD_FOK(bmd_ctx_destroy(&hash_ctx));
			BMD_FOK(bmd_ctx_destroy(&lob_sym_ctx));
			return LOB_CANCELED_BY_CLIENT;
		}
		last_lob_chunk=status;

		BMD_FOK_CHG(status,
				LIBBMDSQL_INSERT_RECEIVELOB_CHUNK_ERR);

		lob_chunk_size = sizeof(char) * LARGE_FILE_DB_CHUNK_SIZE;
		remaining_bytes = lob->tfs - gb_ptr->size;	/* Zaczynamy odliczac ilosc danych pozostalych do wyslania */
		counted_lob_size = gb_ptr->size;		/* Zaczynamy zliczac rozmiar LOBa */
		lob_size = lob->tfs;				/* Rozmiar pliku bez paddingu */
#ifdef BLOB_IN_BYTEA
		large_binary_buf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if (large_binary_buf == NULL)
		{
			PRINT_ERROR("LIBBMDSQL ERR Memory error. Error=%i\n",NO_MEMORY);
			return NO_MEMORY;
		}
		//PRINT_ERROR("BMDSERVERINF ROL_DBG LOB lob_size %li\n",(long)lob_size);
		large_binary_buf->size = (size_t) lob_size + 8;//gb_ptr->size;
		large_binary_buf->buf = (char *) calloc (sizeof(char) * large_binary_buf->size,sizeof(char));
		if (large_binary_buf->buf == NULL)
		{
			PRINT_ERROR("LIBBMDSQL ERR Memory error for large BUFF. Error=%i\n",NO_MEMORY);
			return NO_MEMORY;
		}
		buf_ptr = large_binary_buf->buf; //ustawiamy wskaźnik poczatek wolnego obszaru w buforze
#endif
		/* wiele iteracji */
		/************************************************************************/
		/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
		/************************************************************************/
		if ((*bmdJSRequest)->datagramStatus==BMD_OK)
		{
#ifndef BLOB_IN_BYTEA
			if( last_lob_chunk == 0 )
			{
				BMD_FOK_CHG(bmd_db_export_blob_begin(hDB, lobjId, &blob_locator),
					    	LIBBMDSQL_INSERT_START_LOB_TRANSPORT_TO_DB_ERR);
				status=bmd_db_export_blob_continue(hDB, gb_ptr, blob_locator, LOB_FIRST_PIECE);
			}
			else
			{
				/* dla Oracle i innych zreszta baz w tym przypadku wystarczy normalny import BLOB'a
				   dla Oracle jest blad bo tylko pierwszy PIECE wchodzil nie bylo kolejnych i zaraz
				   szedl commit
				 */
				status=bmd_db_export_blob(hDB,gb_ptr,lobjId);
			}
			if(status<BMD_OK)
			{
				BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
						LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);

				BMD_FOK(LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR);
			}
#else
			if( last_lob_chunk == 0 )
			{
				assert (gb_ptr->size > 0);
				if (gb_ptr->size > 0)
				{
					memcpy(buf_ptr, gb_ptr->buf,gb_ptr->size);
					buf_ptr = buf_ptr+gb_ptr->size;	//przesuwamy wskaznik bufora na poczatek wolnej przestrzeni
				}
			}
			else
			{
				/************************************************************************/
				/*	zapisujemy bufor do tblob i kasujemy bufor	*/
				/************************************************************************/
				//PRINT_INFO("LIBBMDSQLINF data size (buf_ptr - large_binary_buf->buf): %i .\n",(int) (buf_ptr - large_binary_buf->buf));
				PRINT_INFO("LIBBMDSQLINF data size (large_binary_buf->size): %li .\n", large_binary_buf->size);

				//PRINT_ERROR("BMDSERVERINF ROL_DBG LOB lob_size %li\n",(long)gb_ptr->size);
				large_binary_buf->size = (size_t) gb_ptr->size; //lob_size + 8;//
				free(large_binary_buf->buf);
				large_binary_buf->buf = gb_ptr->buf;
				//assert (buf_ptr - large_binary_buf->buf <= large_binary_buf->size);
				BMD_FOK_CHG(bmd_db_export_blob(hDB, large_binary_buf, lobjId),
							LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR);
				buf_ptr = NULL;
				large_binary_buf->buf = NULL;
				//free_gen_buf(&large_binary_buf);
				//TODO sprawdzic jak zwalniane jest gb_ptr->buf;
			}
#endif
		}
		free_gen_buf(&gb_ptr);
		/* Strumieniowe wstawianie LOBow do bazy */
		if (last_lob_chunk==0)
		{
			while(1)
			{

				status=recv_lob_chunk( &lob, &gb_ptr, 0, LARGE_FILE_DB_CHUNK_SIZE+1024 );
				//PRINT_ERROR("BMDSERVERINF ROL_DBG LOB recv size %li\n",gb_ptr->size);
				if (status==LOB_CANCELED_BY_CLIENT)
				{
					PRINT_INFO("LIBBMDSQLINF File transfer cancelled by client\n");
					free_lob_transport( &lob );

					BMD_FOK(bmd_ctx_destroy(&hash_ctx));
					BMD_FOK(bmd_ctx_destroy(&lob_sym_ctx));
					return LOB_CANCELED_BY_CLIENT;
				}

				if(status<0)
				{
					PRINT_ERROR("LIBBMDSQLERR Recieve lob chunk failed! Error=%li\n", status);
					break; /* Brak danych do odczytu z gniazda */
				}
				counted_lob_size += gb_ptr->size;
				remaining_bytes -= gb_ptr->size;

				/* Ostatnia paczka danych jest rozpoznawana po statusie funkcji recv_lob_chunk. Jesli jest
				* on rowny jeden wowczas funkcje bmd_db_export_blob_continue z flaga LOB_LAST_PIECE.
				*/
				//PRINT_ERROR("MSILEWICZ_ORACLE_TEST czy to pierwszy czy ostatni %li\n",status);
				if(status == 1)
				{

					PRINT_INFO("LIBBMDSQLINF Last piece of stream!\n");
					/************************************************************************/
					/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
					/************************************************************************/
					if ((*bmdJSRequest)->datagramStatus==BMD_OK)
					{
#ifndef BLOB_IN_BYTEA
						status=bmd_db_export_blob_continue(hDB, gb_ptr, blob_locator, LOB_LAST_PIECE);
						free_gen_buf(&gb_ptr);
						if(status<BMD_OK)
						{
							PRINT_ERROR("LIBBMDSQLERR Error in iterate export LOB into database - LAST_PIECE. Error=%li\n", status);
							BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
									LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);

							BMD_FOK(LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR);
						}
#else
						assert (gb_ptr->size > 0);
						memcpy(buf_ptr, gb_ptr->buf,gb_ptr->size);
						buf_ptr = buf_ptr + gb_ptr->size;	//przesuwamy wskaznik bufora na poczatek wolnej przestrzeni
						free_gen_buf(&gb_ptr);
#endif
					}

					break;
				}
				else
				{ /* jeszcze nie ostatni fragment strumienia */
					PRINT_INFO("LIBBMDSQLINF Next piece of stream!\n");
#ifndef BLOB_IN_BYTEA
					/************************************************************************/
					/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
					/************************************************************************/
					if ((*bmdJSRequest)->datagramStatus==BMD_OK)
					{
						status=bmd_db_export_blob_continue(hDB, gb_ptr, blob_locator, LOB_NEXT_PIECE);
						free_gen_buf(&gb_ptr);
						if(status<BMD_OK)
						{
							BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
									LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);

							BMD_FOK(LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR);
						}
					}
#else
					assert (gb_ptr->size > 0);
					memcpy(buf_ptr, gb_ptr->buf,gb_ptr->size);
					buf_ptr = buf_ptr + gb_ptr->size;	//przesuwamy wskaznik bufora na poczatek wolnej przestrzeni
					free_gen_buf(&gb_ptr);
#endif
				}
			} /* end while */
		} /* last_lob_chunk == 0 */
		/************************************************************************/
		/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
		/************************************************************************/
/*		PRINT_INFO("LIBBMDSQLINF RaOle DEBUG 4.\n");
		PRINT_INFO("LIBBMDSQLINF RaOle DEBUG (*bmdJSRequest)->datagramStatus %li.\n",(*bmdJSRequest)->datagramStatus);
		PRINT_INFO("LIBBMDSQLINF RaOle DEBUG last_lob_chunk %li.\n",last_lob_chunk);
*/
		if ((*bmdJSRequest)->datagramStatus==BMD_OK)
		{
			if( last_lob_chunk == 0 )
			{
#ifndef BLOB_IN_BYTEA
				BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
						LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);
#else
				/************************************************************************/
				/*	zapisujemy bufor do tblob i kasujemy bufor	*/
				/************************************************************************/
				//PRINT_INFO("LIBBMDSQLINF data size (buf_ptr - large_binary_buf->buf): %i .\n",(int) (buf_ptr - large_binary_buf->buf));
				PRINT_INFO("LIBBMDSQLINF data size (large_binary_buf->size): %li .\n", large_binary_buf->size);
				assert (buf_ptr - large_binary_buf->buf <= large_binary_buf->size);
				//TODO - ten warunek chyba nigdy nie jest spelniony - wywalic
				if (buf_ptr == large_binary_buf->buf)	// pobrano plik w jednym kawalku
				{
					large_binary_buf->size = gb_ptr->size;
					PRINT_ERROR("LIBBMDSQLINF data size (gb_ptr->size): %li .\n", gb_ptr->size);
					PRINT_ERROR("LIBBMDSQLINF data size (counted_lob_size): %li .\n", (long)counted_lob_size);
				}
				else
				{
					large_binary_buf->size = counted_lob_size;
				}
				//PRINT_ERROR("LIBBMDSQLINF data size (large_binary_buf->size): %li .\n", large_binary_buf->size);
				BMD_FOK_CHG(bmd_db_export_blob(hDB, large_binary_buf, lobjId),
							LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR);
				PRINT_INFO("LIBBMDSQLDBG identyfikator rastra (lobjId): %s .\n",*lobjId);
				buf_ptr = NULL;
				free_gen_buf(&large_binary_buf);
#endif

			}
		}
		else
		{
			BMD_FOK((*bmdJSRequest)->datagramStatus);
			/*Na wszelki wypadek*/
			return BMD_ERR_OP_FAILED;
		}
		/******************************************************************/
		/*	Obliczamy skrot z BLOBa i umieszczamy go w datagramie.	*/
		/*	Tak na prawde finalizujemy liczenie hasha				*/
		/******************************************************************/
		BMD_FOK(get_lob_hash(&lob,&hash_buf));
		BMD_FOK(bmd_genbuf_hex_dump(hash_buf,hash_as_hex_string,0,1));


		//BMD_FOK_CHG(PR2_bmdDatagram_add_metadata(	hash_oid,
/*
									(char *)(*hash_as_hex_string),
									(long)strlen(*hash_as_hex_string),
									ADDITIONAL_METADATA,
									(*bmdJSRequest),
									0,
									0,
									0,
									1),
									LIBBMDSQL_INSERT_ADD_HASH_TO_DATAGRAM_ERR);*/
		*lob_size_for_db=counted_lob_size;
		//PRINT_ERROR("BMDSERVERINF ROL_DBG counted_lob_size %li\n",(long)counted_lob_size);

		/************************************************************************/
		/*	Znaczymy czasem BLOBa i umieszczamy znacznik w datagramie	*/
		/************************************************************************/
		/******************************************************/
		/*	sprawdzamy, czy plik jest plikiem towarzyszacym	*/
		/******************************************************/
		for(i=0; i<(*bmdJSRequest)->no_of_sysMetaData; i++)
		{
			if(strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)
			{
				if((*bmdJSRequest)->sysMetaData[i]->AnyBuf != NULL)
				{
					if(strcmp((*bmdJSRequest)->sysMetaData[i]->AnyBuf, "") != 0)
						{ corresponding_id_present=1; }
				}
			}
		}

		/******************************************************************************/
		/*	jesli wsadzamy plik towarzyszacy - znakujemy go innym znacznikiem czasu	*/
		/******************************************************************************/
		if (corresponding_id_present==1)
		{
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
				{
					BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hash_buf, ADD_TS, HASH_DATA, &ts_buf));
					if (ts_buf!=NULL)
					{
						BMD_FOK(PR2_bmdDatagram_add_metadata(timestamp_metadata_oid, ts_buf->buf,ts_buf->size, PKI_METADATA, (*bmdJSRequest),0,0,0, 1));

					ts_buf->buf = NULL; ts_buf->size = 0;/*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
					free_gen_buf(&ts_buf);
					}
				}
			}
		}
		/************************************************************************************/
		/*	jesli wsadzamy plik nie towarzyszacy - znakujemy zwyklym znacznikiem czasu	*/
		/************************************************************************************/
		else
		{

			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
				{
					BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hash_buf, MAIN_TS, HASH_DATA, &ts_buf));
					if (ts_buf!=NULL)
					{
						BMD_FOK(PR2_bmdDatagram_add_metadata(timestamp_metadata_oid, ts_buf->buf,ts_buf->size, PKI_METADATA, (*bmdJSRequest),0,0,0, 1));
						ts_buf->buf = NULL; ts_buf->size = 0;/*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
						free_gen_buf(&ts_buf);
					}
				}
			}
		}

		/******************************************************************************/
		/*	sprawdzamy poprawnosc podpisu pod dokumentem na podstawe jego skrotu	*/
		/******************************************************************************/
		if(_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature)
		{
			status=_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature((*bmdJSRequest), hash_buf);
			if (status<BMD_OK)
			{

				bmd_db_delete_blob(hDB, (*lobjId));
				BMD_FOK(status);
			}
			else
			{
				free_lob_transport( &lob );
				bmd_ctx_destroy(&hash_ctx);
				bmd_ctx_destroy(&lob_sym_ctx);
				PRINT_INFO("LIBBMDSQLINF RaOle DEBUG status = %li.\n", status);
				PRINT_INFO("LIBBMDSQLINF RaOle DEBUG DtgStatus = %li.\n", (*bmdJSRequest)->datagramStatus);
				return status;

			}

		}
		free_lob_transport( &lob );
		bmd_ctx_destroy(&hash_ctx);
		bmd_ctx_destroy(&lob_sym_ctx);
	}

	PRINT_INFO("LIBBMDSQLINF Inserting LOB into database finished.\n");
	return (*bmdJSRequest)->datagramStatus;
}

long SetHashValueAndFileIdDatagram(	char *hash_as_hex_string,
					char **crypto_object_id,
					char *MaxId,
					LobsInDatagram_t LobsInDatagram,
					bmdDatagram_t *bmdJSResponse,
					insert_data_t *data,
					server_request_data_t *req)
{
long crypto_object_id_length = 0;

    /* tutaj przekazujemy policzony HASH w polu protocolData */
	if(LobsInDatagram != LOB_IN_DATAGRAM)
	{
		set_gen_buf2((char *)hash_as_hex_string,(long)strlen(hash_as_hex_string),&(bmdJSResponse->protocolData));
	}

	asprintf(crypto_object_id,"%s",MaxId);
	crypto_object_id_length=(long)strlen(*crypto_object_id);
	BMD_FOK(set_gen_buf2((char *)(*crypto_object_id),crypto_object_id_length,&(bmdJSResponse->protocolDataFileId)));

	if (_GLOBAL_bmd_configuration->location_id!=NULL)
	{
		BMD_FOK(set_gen_buf2((char *)(_GLOBAL_bmd_configuration->location_id),(long)strlen(_GLOBAL_bmd_configuration->location_id),&(bmdJSResponse->protocolDataFileLocationId)));
	}

	return BMD_OK;
}


/**
* @author WSZ
* Funkcja GetOwnerFromRequestDatagram() pomocna jest przy wstawianiu pliku/linku do archiwum.
* Pozwala na wyciagniecie z datagramu zadania identyfikatora tozsamosci przypisywanego wlasiciela,
* oraz identyfikatora przypisywanej grupy. W przypadku pobierania tozsamosci wlasciciela,
* automatycznie sprawdzone zostaje istnienie takiej tozsamosci w systemie.
*
* @param[in] hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param[in] requestDatagram to datagram analizowanego zadania
* @param[in] creatorIdentityId to identyfikator tozsamosci tworcy pliku. Parametr wymagany jest,
*	gdy pobierany jest identyfikator tozsamosci wlasciciela. Jesli w zadaniu brakuje informacji o wlasicielu,
*	wlascicielem automatycznie zostaje tworca. Sprawdzenie istnienia tozsamosci wykonywane jest tylko wtedy,
*	gdy sa rozne tozsamosci tworcy i wlasciciela.
* @param[out] ownerIdentityId to adres wskaznika, do ktorego przypisany zostanie wydobyty (alokowany) identyfikator tozsamosci wlasciciela.
*	Jesli ta informacja nie jest potrzebna, nalezy podac NULL.
* @param[out] ownerGroupId to adres wskaznika, do ktorego przypisany zostanie wydobyty (alokowany) identyfikator grupy.
*	Jesli ta informacja nie jest potrzebna, nalezy podac NULL.
*
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, a odpowiednia wartosc ujemna w przypadku bledu.
*/

long GetOwnerFromRequestDatagram(void* hDB, bmdDatagram_t* requestDatagram, char* creatorIdentityId, char** ownerIdentityId, char** ownerGroupId)
{
char* tempOwnerIdentityId		= NULL;
char* tempOwnerGroupId			= NULL;
long i							= 0;
long retVal						= 0;

	PRINT_INFO("LIBBMDSQLINFO Getting owner from request...\n");

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(ownerIdentityId != NULL)
	{
		if(creatorIdentityId == NULL)
			{ BMD_FOK(BMD_ERR_PARAM3); }
		if(*ownerIdentityId != NULL)
			{ BMD_FOK(BMD_ERR_PARAM4); }
	}
	if(ownerGroupId != NULL)
	{
		if(*ownerGroupId != NULL)
			{ BMD_FOK(BMD_ERR_PARAM5); }
	}
	
	if(ownerIdentityId != NULL)
	{
		/***
		wstawiamy dokument/tworzymy link
		jesli w datagramie mamy id tozsamosci uzytkownika, ta tozsamosc staje sie wlascicielem wstawianego obiektu
		jesli w datagramie mamy id certyfikatu uzytkownika (a nie tozsamosci), to tlumaczymy na id tozsamosci
		jesli nie ma zadnego z powyzszych, tworca automatycznie staje sie wlascicielem
		***/
		for(i=0; i<requestDatagram->no_of_sysMetaData; i++)
		{
			if(requestDatagram->sysMetaData == NULL)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }
			if(requestDatagram->sysMetaData[i] == NULL)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }
			if(requestDatagram->sysMetaData[i]->OIDTableBuf == NULL)
				{ BMD_FOK(BMD_ERR_OP_FAILED); }
			

			if( strcmp(requestDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY)==0 )
			{
				if(requestDatagram->sysMetaData[i]->AnyBuf == NULL)
					{ BMD_FOK(BMD_ERR_OP_FAILED); }
				requestDatagram->sysMetaData[i]->AnyBuf[requestDatagram->sysMetaData[i]->AnySize]='\0';
				tempOwnerIdentityId=strdup(requestDatagram->sysMetaData[i]->AnyBuf);
				if(tempOwnerIdentityId == NULL)
					{ BMD_FOK(NO_MEMORY); }
				break;
			}
			else if (strcmp(requestDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ID)==0)
			{
				if(requestDatagram->sysMetaData[i]->AnyBuf == NULL)
					{ BMD_FOK(BMD_ERR_OP_FAILED); }
				requestDatagram->sysMetaData[i]->AnyBuf[requestDatagram->sysMetaData[i]->AnySize]='\0';
				BMD_FOK(id2identify(hDB, (char *)requestDatagram->sysMetaData[i]->AnyBuf, &tempOwnerIdentityId));

				break;
			}
		}

		//jesli nie wskazywany w zadaniu owner, wowczas domyslnie owner=creator
		if(tempOwnerIdentityId == NULL)
		{
			tempOwnerIdentityId=strdup(creatorIdentityId);
			if(tempOwnerIdentityId == NULL)
			{
				BMD_FOK(NO_MEMORY);
			}
			PRINT_INFO("Default: Owner same as creator\n");
		}
		else
		{
			if(strcmp(creatorIdentityId, tempOwnerIdentityId) != 0)
			{
				retVal=CheckIfIdentityExists(hDB, tempOwnerIdentityId);
				if(retVal < BMD_OK)
				{
					free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
					BMD_FOK(retVal);
				}
			}
		}
	}

	if(ownerGroupId != NULL)
	{
		for(i=0; i<requestDatagram->no_of_sysMetaData; i++)
		{
			if(requestDatagram->sysMetaData == NULL)
			{
				free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(requestDatagram->sysMetaData[i] == NULL)
			{
				free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(requestDatagram->sysMetaData[i]->OIDTableBuf == NULL)
			{
				free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			if( strcmp(requestDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID)==0 )
			{
				if(requestDatagram->sysMetaData[i]->AnyBuf == NULL)
				{
					free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
					BMD_FOK(BMD_ERR_OP_FAILED);
				}
				requestDatagram->sysMetaData[i]->AnyBuf[requestDatagram->sysMetaData[i]->AnySize]='\0';
				tempOwnerGroupId=strdup(requestDatagram->sysMetaData[i]->AnyBuf);
				if(tempOwnerGroupId == NULL)
				{
					free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
					BMD_FOK(NO_MEMORY);
				}
				break;
			}
		}
		
		if(tempOwnerGroupId == NULL) // nie znaleziono w zadaniu
		{
			free(tempOwnerIdentityId); tempOwnerIdentityId=NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	
	// przypisanie wynikow
	if(ownerIdentityId != NULL)
	{
		*ownerIdentityId = tempOwnerIdentityId;
		tempOwnerIdentityId=NULL;
	}
	if(ownerGroupId != NULL)
	{
		*ownerGroupId = tempOwnerGroupId;
		tempOwnerGroupId = NULL;
	}
	
	return BMD_OK;
}


/**
* @brief funkcja wstawia wiersz crypto_obejcts (wraz z wypelenieniem metadanych systemowych)
* @param correspondingId[in] nalezy podac tylko, gdy tworzony link (to jedyna metadata systemowa kopiowana z dokumentu wskazywanego)
*	a w innych przypadkach podac NULL
*/
long JS_PrepareAndExecuteSQLQuery_Insert_CryptoObjects(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char* NewCryptoObjectId,
							char* correspondingId,
							char *lobjId,
							u_int64_t lob_size,
							char* hashValueHexString,
							server_request_data_t *req)
{
long retVal		= 0;
char *symkey_loid	= NULL;
void *query_result	= NULL;
char *SQLQuery		= NULL;
char *CurrentTimestamp	= NULL;
char *filename		= NULL;
char *tmp		= NULL;
GenBuf_t *tmp_symkey	= NULL;
char* ownerIdentityId	= NULL;
char* creatorIdentityId	= NULL;
char* hashValue		= NULL;

char* escapeTemp			= NULL;
char* temp_transaction_id		= NULL;
char* temp_visible			= NULL;
char* temp_present			= NULL;
char* temp_pointing_id			= NULL;
char* temp_pointing_location_id		= NULL;
char* temp_file_type			= NULL;
char* temp_corresponding_id		= NULL;
char* temp_fk_group			= NULL;
char* temp_fk_sec_category		= NULL;
char* temp_fk_sec_level			= NULL;
char* temp_for_grant			= NULL;
char* temp_expiration_date		= NULL;
char* temp_for_timestamping		= NULL;

	PRINT_INFO("LIBBMDSQLINF Preparing and executing sql query (JS)\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest != NULL &&
		( bmdJSRequest->protocolDataFilename      == NULL
		|| bmdJSRequest->protocolDataFilename->buf == NULL
		|| bmdJSRequest->protocolDataOwner         == NULL
		|| bmdJSRequest->protocolDataOwner->buf    == NULL))
									{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(NewCryptoObjectId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if (lobjId == NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (lob_size<0)							{	BMD_FOK(BMD_ERR_PARAM6);	}

	if(req == NULL || req->ua == NULL || req->ua->identityId == NULL || req->ua->identityId->size <= 0)
		{ BMD_FOK(BMD_ERR_PARAM8); }
	else
	{
		creatorIdentityId=strdup(req->ua->identityId->buf);
		if(creatorIdentityId == NULL)
			{ BMD_FOK(NO_MEMORY); }
	}

	if(bmdJSRequest->datagramType == BMD_DATAGRAM_TYPE_INSERT ||
		bmdJSRequest->datagramType == BMD_DATAGRAM_TYPE_INSERT_LO)
	{
		if(hashValueHexString != NULL || strlen(hashValueHexString) != 40)
			{ asprintf(&hashValue, " '%s' ", hashValueHexString); }
		else
			{ BMD_FOK(BMD_ERR_PARAM7); }
		
	}
	else
	{
		hashValue=strdup("NULL");
	}

	if(hashValue == NULL)
		{ BMD_FOK(NO_MEMORY); }



	if(bmdJSRequest->protocolData!=NULL)
	{
		lob_size=bmdJSRequest->protocolData->size;
	}

	CurrentTimestamp=bmd_db_get_actualtime_sql(hDB);
	if(CurrentTimestamp==NULL)			{	BMD_FOK(NO_MEMORY);	}

	/***
	wstawiamy dokument/tworzymy link
	jesli w datagramie mamy id tozsamosci uzytkownika, ta tozsamosc staje sie wlascicielem wstawianego obiektu
	jesli w datagramie mamy id certyfikatu uzytkownika (a nie tozsamosci), to tlumaczymy na id tozsamosci
	jesli nie ma zadnego z powyzszych, tworca automatycznie staje sie wlascicielem
	***/
	BMD_FOK(GetOwnerFromRequestDatagram(hDB, bmdJSRequest, creatorIdentityId, &ownerIdentityId, NULL/*ownerGroupId nie jest potrzebne*/));
	// escapowanie wartosci
	tmp=ownerIdentityId; ownerIdentityId=NULL;
	BMD_FOK(bmd_db_escape_string(hDB, tmp, STANDARD_ESCAPING, &ownerIdentityId));
	free(tmp); tmp=NULL;

	tmp=(char *)malloc(sizeof(char)*bmdJSRequest->protocolDataFilename->size+2);
	memset(tmp,0,sizeof(char)*bmdJSRequest->protocolDataFilename->size+1);
	memmove(tmp,bmdJSRequest->protocolDataFilename->buf,bmdJSRequest->protocolDataFilename->size);

 	BMD_FOK(bmd_db_escape_string(hDB, tmp, STANDARD_ESCAPING, &filename));
	//asprintf(&filename, "%s", tmp);
	if (filename==NULL)	{	BMD_FOK(NO_MEMORY);	}
	free0(tmp);



// 	if (	(bmdJSRequest->datagramType!=BMD_DATAGRAM_DIR_CREATE) &&
// 		(bmdJSRequest->datagramType!=BMD_DATAGRAM_LINK_CREATE) )
// 	{
		BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest,&tmp_symkey));
		BMD_FOK(bmd_db_export_blob(hDB,tmp_symkey,&symkey_loid));
		free_gen_buf(&tmp_symkey);
// 	}
// 	else
// 	{
// 		asprintf(&symkey_loid, "0");
// 	}

	// pozostale metadane systemowe
	BMD_FOK(JS_Prepare_Insert_SysMetaData(hDB, bmdJSRequest, NewCryptoObjectId, correspondingId, req,
							&temp_transaction_id,
							&temp_visible,
							&temp_present,
							&temp_pointing_id,
							&temp_pointing_location_id,
							&temp_file_type,
							&temp_corresponding_id,
							&temp_fk_group,
							&temp_fk_sec_category,
							&temp_fk_sec_level,
							&temp_for_grant,
							&temp_expiration_date,
							&temp_for_timestamping));

	BMD_FOK(bmd_db_escape_string(hDB, temp_transaction_id, STANDARD_ESCAPING, &escapeTemp));
	free(temp_transaction_id); temp_transaction_id = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_visible, STANDARD_ESCAPING, &escapeTemp));
	free(temp_visible); temp_visible = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_present, STANDARD_ESCAPING, &escapeTemp));
	free(temp_present); temp_present = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_pointing_id, STANDARD_ESCAPING, &escapeTemp));
	free(temp_pointing_id); temp_pointing_id = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_pointing_location_id, STANDARD_ESCAPING, &escapeTemp));
	free(temp_pointing_location_id); temp_pointing_location_id = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_file_type, STANDARD_ESCAPING, &escapeTemp));
	free(temp_file_type); temp_file_type = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_corresponding_id, STANDARD_ESCAPING, &escapeTemp));
	free(temp_corresponding_id); temp_corresponding_id = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_fk_group, STANDARD_ESCAPING, &escapeTemp));
	free(temp_fk_group); temp_fk_group = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_fk_sec_category, STANDARD_ESCAPING, &escapeTemp));
	free(temp_fk_sec_category); temp_fk_sec_category = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_fk_sec_level, STANDARD_ESCAPING, &escapeTemp));
	free(temp_fk_sec_level); temp_fk_sec_level = escapeTemp; escapeTemp = NULL;

	BMD_FOK(bmd_db_escape_string(hDB, temp_for_grant, STANDARD_ESCAPING, &escapeTemp));
	free(temp_for_grant); temp_for_grant = escapeTemp; escapeTemp = NULL;

	if(temp_expiration_date != NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, temp_expiration_date, STANDARD_ESCAPING, &escapeTemp));
		free(temp_expiration_date); temp_expiration_date = escapeTemp; escapeTemp = NULL;
		BMD_FOK(bmdQuoteSqlConditionValue(temp_expiration_date, &escapeTemp));
		free(temp_expiration_date); temp_expiration_date = escapeTemp; escapeTemp = NULL;
	}

	BMD_FOK(bmd_db_escape_string(hDB, temp_for_timestamping, STANDARD_ESCAPING, &escapeTemp));
	free(temp_for_timestamping); temp_for_timestamping = escapeTemp; escapeTemp = NULL;


#ifdef WIN32
	if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
	{
		retVal = asprintf(&SQLQuery,
				"INSERT INTO crypto_objects (id, name, filesize, raster, symkey, hash_value, creator, fk_owner, insert_date, "
				"transaction_id, visible, present, pointing_id, pointing_location_id, file_type, corresponding_id, fk_group, fk_sec_categories, fk_sec_levels, for_grant, expiration_date, for_timestamping %s) "
				"VALUES ('%s', \'%s\', %li, '%s', '%s', %s, '%s', '%s', %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, '%s'  %s%s);",
				_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
				NewCryptoObjectId, filename,
				(long)lob_size, lobjId,
				symkey_loid, hashValue, creatorIdentityId, ownerIdentityId, CurrentTimestamp,
				temp_transaction_id,
				temp_visible,
				temp_present,
				temp_pointing_id,
				temp_pointing_location_id,
				temp_file_type,
				temp_corresponding_id,
				temp_fk_group,
				temp_fk_sec_category,
				temp_fk_sec_level,
				temp_for_grant,
				(temp_expiration_date != NULL ? temp_expiration_date : "NULL"),
				temp_for_timestamping,
				_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	}
	else
	{
		if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
		{
			char **bv_list=NULL;
			char *bss=NULL;

			retVal = asprintf(&SQLQuery,
					 "INSERT INTO crypto_objects (id, name, filesize, raster, symkey, hash_value, creator, fk_owner, insert_date, "
					"transaction_id, visible, present, pointing_id, pointing_location_id, file_type, corresponding_id, "
					"fk_group, fk_sec_categories, fk_sec_levels, for_grant, expiration_date, for_timestamping %s) "
					 "VALUES ('%s',:1,:2,:3,:4,:5,:6,:7,%s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, '%s' %s%s);",
					_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
					 NewCryptoObjectId,
					 CurrentTimestamp,
					temp_transaction_id,
					temp_visible,
					temp_present,
					temp_pointing_id,
					temp_pointing_location_id,
					temp_file_type,
					temp_corresponding_id,
					temp_fk_group,
					temp_fk_sec_category,
					temp_fk_sec_level,
					temp_for_grant,
					(temp_expiration_date != NULL ? temp_expiration_date : "NULL"),
					temp_for_timestamping,
					_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

			bv_list=(char **)malloc(7*sizeof(char *));
			bv_list[0]=filename;
			asprintf(&bss,"%li",(long)lob_size);
			bv_list[1]=bss;
			bv_list[2]=lobjId;
			bv_list[3]=symkey_loid;
			bv_list[4]=hashValue;
			bv_list[5]=creatorIdentityId;
			bv_list[6]=ownerIdentityId;
			BMD_FOK(bmd_db_link_bind_var_list(bv_list,7,hDB));
			free0(bss);
			free0(bv_list);
		}
	}
#else
	retVal = asprintf(&SQLQuery,	"INSERT INTO crypto_objects (id, name, filesize, raster, symkey, hash_value, creator, fk_owner, insert_date, "
				"transaction_id, visible, present, pointing_id, pointing_location_id, file_type, corresponding_id, fk_group, fk_sec_categories, fk_sec_levels, for_grant, expiration_date, " "for_timestamping %s) "
				"VALUES ('%s', \'%s\', %lli, '%s', '%s', %s, '%s', '%s', %s, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, '%s' %s%s);",
					_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
					NewCryptoObjectId, filename,
					(long long)lob_size, lobjId,
					symkey_loid, hashValue, creatorIdentityId, ownerIdentityId, CurrentTimestamp,
					temp_transaction_id,
					temp_visible,
					temp_present,
					temp_pointing_id,
					temp_pointing_location_id,
					temp_file_type,
					temp_corresponding_id,
					temp_fk_group,
					temp_fk_sec_category,
					temp_fk_sec_level,
					temp_for_grant,
					(temp_expiration_date != NULL ? temp_expiration_date : "NULL"),
					temp_for_timestamping,
					_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
#endif

	free(temp_transaction_id); temp_transaction_id = NULL;
	free(temp_visible); temp_visible = NULL;
	free(temp_present); temp_present = NULL;
	free(temp_pointing_id); temp_pointing_id = NULL;
	free(temp_pointing_location_id); temp_pointing_location_id = NULL;
	free(temp_file_type); temp_file_type = NULL; 
	free(temp_corresponding_id); temp_corresponding_id = NULL;
	free(temp_fk_group); temp_fk_group = NULL;
	free(temp_fk_sec_category); temp_fk_sec_category = NULL;
	free(temp_fk_sec_level); temp_fk_sec_level = NULL;
	free(temp_for_grant); temp_for_grant = NULL;
	free(temp_expiration_date); temp_expiration_date = NULL;
	free(temp_for_timestamping); temp_for_timestamping = NULL;

	free(filename); filename=NULL;
	free(creatorIdentityId); creatorIdentityId=NULL;
	free(ownerIdentityId); ownerIdentityId = NULL;
	free(CurrentTimestamp); CurrentTimestamp=NULL;
	free(tmp); tmp=NULL;
	free_gen_buf(&tmp_symkey);
	free(symkey_loid); symkey_loid=NULL;
	free(hashValue); hashValue=NULL;

	if(retVal == -1)
		{ BMD_FOK(NO_MEMORY); }

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

/****
 * Funkcja odpowiada za wstawienie dodatkowych metadanych (np. dotyczacych e-faktur) do roznych tabel. W obecnej wersji
 * wszystkie metadane wstawiane sa do jednej tabeli - additional_metadata
 */
long JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								char *MaxId,
								server_request_data_t *req)
{
long db_type			= -1;
long i				= 0;
void *database_result		= NULL;
char *tmp 			= NULL;
char *addMetadataColumn		= NULL;
char *addMetadataValue		= NULL;
char *updateQuery		= NULL;

	PRINT_INFO("LIBBMDSQLINF Inserting additional metadata(JS)\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (MaxId==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (bmdJSRequest->no_of_additionalMetaData <= 0) //{	BMD_FOK(ERR_JS_NO_ADDITIONAL_METADATA_PRESENT);	}
	{
		PRINT_INFO("%s. Info: %li\n",GetErrorMsg(ERR_JS_NO_ADDITIONAL_METADATA_PRESENT),(long)ERR_JS_NO_ADDITIONAL_METADATA_PRESENT);\
		return ERR_JS_NO_ADDITIONAL_METADATA_PRESENT;
	}	
	

	db_type=bmd_db_connect_get_db_type(hDB);

	asprintf(&updateQuery, "UPDATE crypto_objects SET ");
	if (updateQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}

	/******************************************************/
	/*	przegladanie wszystkich metadanych dodatkowych	*/
	/******************************************************/
	for(i=0; i<bmdJSRequest->no_of_additionalMetaData; i++)
	{
		/*****************************************/
		/* ustalenie nazwy kolumny dla metadanej */
		/*****************************************/
		asprintf(&addMetadataColumn, "amv_%s", bmdJSRequest->additionalMetaData[i]->OIDTableBuf);
		if (addMetadataColumn==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_str_replace(&addMetadataColumn, ".", "_"));

		/********************************/
		/* ustalenie wartości metadanej */
		/********************************/
		asprintf(&addMetadataValue, "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
		if (addMetadataValue==NULL)	{	BMD_FOK(NO_MEMORY);	}

		// escapowane wartosci metadanej
		tmp=addMetadataValue; addMetadataValue=NULL;
		BMD_FOK(bmd_db_escape_string(hDB, tmp, STANDARD_ESCAPING, &addMetadataValue));
		
		/****************************************/
		/* polaczenie nazw kolumn z wartosciami */
		/****************************************/
		free0(tmp);
		if (i==0)	asprintf(&tmp, 	"%s%s='%s'", updateQuery, addMetadataColumn, addMetadataValue);
		else		asprintf(&tmp, 	"%s, %s='%s'", updateQuery, addMetadataColumn, addMetadataValue);

		if (tmp==NULL)			{	BMD_FOK(NO_MEMORY);	}

		free0(updateQuery);
		asprintf(&updateQuery, "%s", tmp);
		if (updateQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}
	}

	/*************************/
	/* zakonczenie zapytania */
	/*************************/
	free0(tmp);
	asprintf(&tmp, 	"%s WHERE id=%s;", updateQuery, MaxId);
	if (tmp==NULL)			{	BMD_FOK(NO_MEMORY);	}

	free0(updateQuery);
	asprintf(&updateQuery, "%s", tmp);
	if (tmp==NULL)			{	BMD_FOK(NO_MEMORY);	}

 	BMD_FOK_CHG(bmd_db_execute_sql(hDB, updateQuery, NULL, NULL, &database_result), BMD_VERIFY_REQUEST_INVALID_ADD_METADATA);

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(tmp);
	free0(addMetadataColumn);
	free0(addMetadataValue);
	free0(updateQuery);

	return BMD_OK;
}


/**
* @brief Funkcja wydobywa z zadania metadane systemowe na potrzeby wstawienia nowego wiersza crypto_objects
* @param correspondingId[in] nalezy podac tylko, gdy tworzony link (to jedyna metadata systemowa kopiowana z dokumentu wskazywanego)
*	a w innych przypadkach podac NULL
* @param out_expiration_date[out] moze zostac na wyjsciu NULL, co oznacza, ze nalezy ustawic NULL dla tej kolumny
*/
long JS_Prepare_Insert_SysMetaData(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char *MaxId,
							char* correspondingId,
							server_request_data_t *req,
							char** out_transaction_id,
							char** out_visible,
							char** out_present,
							char** out_pointing_id,
							char** out_pointing_location_id,
							char** out_file_type,
							char** out_corresponding_id,
							char** out_fk_group,
							char** out_fk_sec_category,
							char** out_fk_sec_level,
							char** out_for_grant,
							char** out_expiration_date,
							char** out_for_timestamping)
{
long i				= 0;
long IsSecCategoryTypeFlag	= 0;
long IsSecLevelTypeFlag		= 0;
char * SecCategoryTypeId	= NULL;
char * SecLevelTypeId		= NULL;
char *SQLQuery			= NULL;
char *filename			= NULL;
char **ans			= NULL;
char *file_group_name		= NULL;
char *tmp			= NULL;
long ansCount			= 0;
char* escapeTemp			= NULL;

char* expirationYear		= NULL;
char* expirationMonth		= NULL;
char* expirationDay		= NULL;
char* expirationHour		= NULL;
char* expirationMinute		= NULL;
char* expirationSecond		= NULL;

char* temp_TransactionId	= NULL;
char* temp_Visible		= NULL;
char* temp_Present		= NULL;
char* temp_PointingId		= NULL;
char* temp_PointingLocationId	= NULL;
char* temp_FileType		= NULL;
char* temp_CorrespondingId	= NULL;
char* temp_GroupId		= NULL;
char* temp_SecCategoryId	= NULL;
char* temp_SecLevelId		= NULL;
char* temp_ForGrant		= NULL;
char* temp_ExpirationDate	= NULL;
char* temp_ForTimestamping	= NULL;


	PRINT_INFO("LIBBMDSQLINF Inserting system metadata (JS)\n");

	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (MaxId==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(out_transaction_id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6);}
	if(*out_transaction_id != NULL)
		{ BMD_FOK(BMD_ERR_PARAM6);}
	if(out_visible == NULL)
		{ BMD_FOK(BMD_ERR_PARAM7);}
	if(*out_visible != NULL)
		{ BMD_FOK(BMD_ERR_PARAM7);}
	if(out_present == NULL)
		{ BMD_FOK(BMD_ERR_PARAM8);}
	if(*out_present != NULL)
		{ BMD_FOK(BMD_ERR_PARAM8);}
	if(out_pointing_id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM9);}
	if(*out_pointing_id != NULL)
		{ BMD_FOK(BMD_ERR_PARAM9);}
	if(out_pointing_location_id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM10);}
	if(*out_pointing_location_id != NULL)
		{ BMD_FOK(BMD_ERR_PARAM10);}
	if(out_file_type == NULL)
		{ BMD_FOK(BMD_ERR_PARAM11);}
	if(*out_file_type != NULL)
		{ BMD_FOK(BMD_ERR_PARAM11);}
	if(out_corresponding_id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM12);}
	if(*out_corresponding_id != NULL)
		{ BMD_FOK(BMD_ERR_PARAM12);}
	if(out_fk_group == NULL)
		{ BMD_FOK(BMD_ERR_PARAM13);}
	if(*out_fk_group != NULL)
		{ BMD_FOK(BMD_ERR_PARAM13);}
	if(out_fk_sec_category == NULL)
		{ BMD_FOK(BMD_ERR_PARAM14);}
	if(*out_fk_sec_category != NULL)
		{ BMD_FOK(BMD_ERR_PARAM14);}
	if(out_fk_sec_level == NULL)
		{ BMD_FOK(BMD_ERR_PARAM15);}
	if(*out_fk_sec_level != NULL)
		{ BMD_FOK(BMD_ERR_PARAM15);}
	if(out_for_grant == NULL)
		{ BMD_FOK(BMD_ERR_PARAM16);}
	if(*out_for_grant != NULL)
		{ BMD_FOK(BMD_ERR_PARAM16);}
	if(out_expiration_date == NULL)
		{ BMD_FOK(BMD_ERR_PARAM17);}
	if(*out_expiration_date != NULL)
		{ BMD_FOK(BMD_ERR_PARAM17);}
	if(out_for_timestamping == NULL)
		{ BMD_FOK(BMD_ERR_PARAM18);}
	if(*out_for_timestamping != NULL)
		{ BMD_FOK(BMD_ERR_PARAM18);}

	/************************************************/
	/*	przeszukiwanie metadanych systemowych	*/
	/************************************************/
	for(i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/****************************************************/
		/* poszukiwania metadanych systemowych w datagramie */
		/****************************************************/
		if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf,OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID))
		{
			temp_PointingId=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_PointingId == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_PointingId, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_PointingId, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));

		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf,OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID))
		{
			temp_PointingLocationId=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_PointingLocationId == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_PointingLocationId, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_PointingLocationId, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));

		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf,OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID))
		{
			temp_TransactionId=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_TransactionId == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_TransactionId, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_TransactionId, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE))
		{
			temp_Visible=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_Visible == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_Visible, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_Visible, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf,OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT))
		{
			temp_Present=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_Present == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_Present, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_Present, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE))
		{
			temp_FileType=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_FileType == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_FileType, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_FileType, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID))
		{
			temp_CorrespondingId=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_CorrespondingId == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_CorrespondingId, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_CorrespondingId, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID))
		{
			temp_GroupId=(char *)malloc(sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			if(temp_GroupId == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memset(temp_GroupId, 0, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize+1));
			memcpy(temp_GroupId, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT))
		{
			temp_ForGrant=(char *)calloc(bmdJSRequest->sysMetaData[i]->AnySize+1, sizeof(char));
			if(temp_ForGrant == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memcpy(temp_ForGrant, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE))
		{
			temp_ExpirationDate=(char *)calloc(bmdJSRequest->sysMetaData[i]->AnySize+1, sizeof(char));
			if(temp_ExpirationDate == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			memcpy(temp_ExpirationDate, bmdJSRequest->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdJSRequest->sysMetaData[i]->AnySize));
		}

		/************************************************************/
		/*	jesli metadana jest metadana kategorii bezpieczenstwa	*/
		/************************************************************/
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_SEC_CAT))
		{
			//SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id);
			BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityDictionary, 4, bmdJSRequest->sysMetaData[i]->AnyBuf, 3, &SecCategoryTypeId), LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND);

			IsSecCategoryTypeFlag=1;
		}
		/************************************************************************/
		/*	jesli metadana jest metadana kategorii poziomu bezpieczenstwa	*/
		/************************************************************************/
		else if(!strcmp( bmdJSRequest->sysMetaData[i]->OIDTableBuf,OID_SYS_METADATA_BMD_SEC_LEVELS))
		{
			//SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id);
			BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityDictionary, 2, bmdJSRequest->sysMetaData[i]->AnyBuf, 1, &tmp), LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND);

			//SELECT id, type, name, priority FROM sec_levels;
			BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityLevelsDictionary, 0, tmp, 3, &SecLevelTypeId), LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND);
			free0(tmp);
			IsSecLevelTypeFlag=1;
		}
	}

	if (temp_TransactionId!=NULL)
	{
		free(temp_Visible); temp_Visible = NULL;
		temp_Visible= strdup("0");
		if(temp_Visible == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	if ((temp_Visible==NULL) && (temp_TransactionId==NULL))
	{
		temp_Visible = strdup("1");
		if(temp_Visible == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	if (temp_Present==NULL)
	{
		temp_Present = strdup("1");
		if(temp_Present == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	if (temp_FileType==NULL)
	{
		temp_FileType = strdup("");
		if(temp_FileType == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}

	if(correspondingId != NULL)
	{
		free(temp_CorrespondingId); temp_CorrespondingId = NULL;
		temp_CorrespondingId = strdup(correspondingId);
		if(temp_CorrespondingId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	else if(temp_CorrespondingId==NULL)
	{
		temp_CorrespondingId = strdup("0");
		if(temp_CorrespondingId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}

	if (temp_PointingId==NULL)
	{
		temp_PointingId = strdup(MaxId);
		if(temp_PointingId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	if (temp_PointingLocationId==NULL)
	{
		temp_PointingLocationId = strdup(_GLOBAL_bmd_configuration->location_id);
		if(temp_PointingLocationId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	if(temp_TransactionId == NULL)
	{
		temp_TransactionId = strdup("");
		if(temp_TransactionId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}

	if(temp_ForGrant == NULL)
	{
		temp_ForGrant=strdup("1");
		if(temp_ForGrant == NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else if(strcmp(temp_ForGrant, "0") != 0 && strcmp(temp_ForGrant, "1") != 0)
	{
		BMD_FOK(LIBBMDSQL_INSERT_INCORRECT_FOR_GRANT_VALUE);
	}

	if( strcmp(temp_FileType, "dir")==0 && bmdJSRequest->datagramType!=BMD_DATAGRAM_DIR_CREATE )
		{ BMD_FOK(LIBBMDSQL_INSERT_UNALLOWABLE_DIR_FILE_TYPE); }

	if( strcmp(temp_FileType, "link")==0 && bmdJSRequest->datagramType!=BMD_DATAGRAM_LINK_CREATE &&
		bmdJSRequest->datagramType!=BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS )
		{ BMD_FOK(LIBBMDSQL_INSERT_UNALLOWABLE_LINK_FILE_TYPE); }

	if(strcmp(temp_FileType, "link")==0 && strcmp(temp_FileType, "dir")==0)
	{
		temp_ForTimestamping = strdup("0");
		if(temp_ForTimestamping == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}
	else
	{
		if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
		{
			temp_ForTimestamping = strdup("0");
			if(temp_ForTimestamping == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
		}
		else if(_GLOBAL_std_conservate_configuration->asynchronous_timestamping	== 1 &&
				_GLOBAL_std_conservate_configuration->enable_main_ts		== 0 &&
				_GLOBAL_std_conservate_configuration->enable_additional_ts	== 0 &&
				_GLOBAL_std_conservate_configuration->enable_signature_ts	== 0 )
		{
			temp_ForTimestamping = strdup("0");
			if(temp_ForTimestamping == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
		}
		else
		{
			free(temp_Visible);
			temp_Visible = strdup("0");
			if(temp_Visible == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
			temp_ForTimestamping = strdup("1");
			if(temp_ForTimestamping == NULL)
				{ BMD_FOK(BMD_ERR_MEMORY); }
		}
			
	}

	if(bmdJSRequest->datagramType==BMD_DATAGRAM_LINK_CREATE || bmdJSRequest->datagramType==BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS)
	{
		if(temp_ExpirationDate != NULL)
		{
			BMD_FOK(bmdParseSqlTimestamp(temp_ExpirationDate, &expirationYear, &expirationMonth, &expirationDay, &expirationHour, &expirationMinute, &expirationSecond));
			BMD_FOK(bmdValidateSqlTimestamp(expirationYear, expirationMonth, expirationDay, expirationHour, expirationMinute, expirationSecond, LATER_THAN_NOW));
			free(expirationYear); expirationYear=NULL;
			free(expirationMonth); expirationMonth=NULL;
			free(expirationDay); expirationDay=NULL;
			free(expirationHour); expirationHour=NULL;
			free(expirationMinute); expirationMinute=NULL;
			free(expirationDay); expirationDay=NULL;
			free(expirationSecond); expirationSecond=NULL;
		}
	}
	if(temp_ExpirationDate == NULL)
	{
		// temp_ExpirationDate pozostaje NULL na wyjsciu
	}
	/******************************************************************************/
	/* sprawdzenie, czy w bazie znajduje się katalog o tej nazwie na tym poziomie */
	/******************************************************************************/
	if (bmdJSRequest->datagramType==BMD_DATAGRAM_DIR_CREATE)
	{
		BMD_FOK(bmd_datagram_get_filename(bmdJSRequest, &filename));
		// escapowanie wartosci
		escapeTemp=filename; filename=NULL;
		BMD_FOK(bmd_db_escape_string(hDB, escapeTemp, STANDARD_ESCAPING, &filename));
		free(escapeTemp); escapeTemp=NULL;
		
		BMD_FOK(bmd_db_escape_string(hDB, temp_CorrespondingId, STANDARD_ESCAPING, &escapeTemp));
		

		asprintf(&SQLQuery, 	"SELECT id FROM crypto_objects WHERE file_type='dir' AND name='%s' AND corresponding_id='%s'%s%s;",
					filename,
					escapeTemp,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free0(filename);
		free0(escapeTemp);

		ExecuteSQLQueryWithAnswersKnownDBConnection(hDB, SQLQuery, &ansCount, &ans);

		for (i=0; i<ansCount; i++)
		{
			free0(ans[i]);
		}
		free0(ans);
		free0(SQLQuery);

		if (ansCount>1)	{	BMD_FOK(LIBBMDSQL_INSERT_DIRECTORY_ALREADY_EXIST); }
	}

	/**********************************************************************************************/
	/* okreslenie, czy plik wstawiany jest w grupie wstawiajacego, czy jako link dla kogos innego */
	/**********************************************************************************************/
	if (temp_GroupId==NULL)
	{
		temp_GroupId = strdup(req->ua->user_chosen_group->buf);
		if (temp_GroupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}
	else
	{
		/************************************************/
		/* sprawdzenie, czy grupa o podanym id istnieje */
		/************************************************/
		BMD_FOK(bmd_db_escape_string(hDB, temp_GroupId, STANDARD_ESCAPING, &escapeTemp));
		
		
		BMD_FOK(CheckIfGroupExist(escapeTemp, &file_group_name));
		free(escapeTemp); escapeTemp=NULL;
		free0(file_group_name);
	}

	/* musza byc obecne oraz dany sec_level odpowiada danej sec categ */
	/******************************************************************
	* UWAGA:
	* W zwiazku z tak przyjeta implementacja, jesli zaistnieje sytuacja,
	* ze crypto_object bedzie mial wiecej niz 1 clearance to nalezy podawac
	* po kolei w sys_metadata security_category nr1, security level nr1,
	* security category nr2, security level nr2 itd.
	* Jeli ktos pod na poczatku 2krotnie security category, a potem security
	* level 2 razy, to licznik IsSecCategoryTypeFlag == 2 i IsSecLevelTypeFlag ==2
	* i nie wstawiam nic. Wiem ze mozna by porownywac zawartosc obu licznikow,
	* ale to w przyszlych wersjach protokolu sie uszczegolowi
	* jesli bedzie taka potrzeba.
	*
	*******************************************************************/
	if ((IsSecCategoryTypeFlag ==1) && (IsSecLevelTypeFlag == 1))
	{
		temp_SecCategoryId = SecCategoryTypeId; SecCategoryTypeId = NULL;
		temp_SecLevelId = SecLevelTypeId; SecLevelTypeId = NULL;
	}
	else
	{
		/*  fk_sec_categories=1 (undefined) oraz fk_sec_levels=0 (priorytet 0 dla unmarked) */
		temp_SecCategoryId = strdup("1");
		if(temp_SecCategoryId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
		temp_SecLevelId = strdup("0");
		if(temp_SecLevelId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	}

	// ustawienie wynikow
	*out_transaction_id = temp_TransactionId;
	temp_TransactionId = NULL;

	*out_visible = temp_Visible;
	temp_Visible = NULL;

	*out_present = temp_Present;
	temp_Present = NULL;

	*out_pointing_id = temp_PointingId;
	temp_PointingId = NULL;

	*out_pointing_location_id = temp_PointingLocationId;
	temp_PointingLocationId = NULL;

	*out_file_type = temp_FileType;
	temp_FileType = NULL;

	*out_corresponding_id = temp_CorrespondingId;
	temp_CorrespondingId = NULL;

	*out_fk_group = temp_GroupId;
	temp_GroupId = NULL;

	*out_fk_sec_category = temp_SecCategoryId;
	temp_SecCategoryId = NULL;

	*out_fk_sec_level = temp_SecLevelId;
	temp_SecLevelId = NULL;

	*out_for_grant = temp_ForGrant;
	temp_ForGrant = NULL;

	*out_expiration_date = temp_ExpirationDate;
	temp_ExpirationDate = NULL;

	*out_for_timestamping = temp_ForTimestamping;
	temp_ForTimestamping = NULL;

	return BMD_OK;
}


/**
*Wprowadzenie metadanych PKI pobranych z datagramu do bazy danych
*
*@param[in] void *hDB	- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] bmdDatagram_t *bmdJSRequest	- wskaznik do datagramu zadania
*@param[in] long MaxId		- wewnetrzny identyfikator crypto_objecta, do ktorego przypisane sa metadane
*@param[in] insert_data_t *data		- wskaznik do struktury zawierajacej wszystkie informacje niezbedne do wstawiania danych do archiwum
*
*/
long JS_PrepareAndExecuteSQLQuery_Insert_PkiMetaData(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char *MaxId,
							insert_data_t *data,
							server_request_data_t *req,
							char** newHashHexString)
{
long i					= 0;
long timestamp_dvcs_insert		= 0;
long no_tsa_cert_number			= 0;
char *SQLQuery				= NULL;
char *timestamp_id			= NULL;
char *signature_id			= NULL;
char *dvcs_id				= NULL;
char *metadataId			= NULL;
char *loOid				= NULL;

bmd_crypt_ctx_t *sym_ctx		= NULL;
bmd_crypt_ctx_t *sym_ctx2		= NULL;
bmd_crypt_ctx_t *timestamp_hash_ctx	= NULL;

GenBuf_t *der_symkey			= NULL;
GenBuf_t *timestamp_hash_buf		= NULL;
GenBuf_t *ts_buf			= NULL;
GenBuf_t *tmpGenBuf			= NULL;
ConservationConfig_t ConservationConfig;

char* padesHashHexValue		= NULL;
char* padesHashAlgorithm	= NULL;


	PRINT_INFO("LIBBMDSQLINF Inserting pki metadata (JS)\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (MaxId==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (bmdJSRequest->no_of_pkiMetaData <= 0)	{	BMD_FOK_NP(ERR_JS_NO_PKI_METADATA_PRESENT);	}
	if(newHashHexString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }
	if(*newHashHexString != NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }



	/************************************************************************/
	/*	przeszukanie listy metadanych PKI w poszukiwaniu podpisow pliku	*/
	/************************************************************************/
	for (i=0; i<bmdJSRequest->no_of_pkiMetaData; i++)
	{
		/************************************************/
		/*	wydobycie wartosci binarnej obiektu PKI	*/
		/************************************************/
		asprintf(&metadataId, "%li", bmdJSRequest->pkiMetaData[i]->myId);

		/* czy poswiadczenie dvcs ma byc znakowane czasem*/
		timestamp_dvcs_insert = bmdJSRequest->pkiMetaData[i]->ownerType;

		BMD_FOK(set_gen_buf2(bmdJSRequest->pkiMetaData[i]->AnyBuf, bmdJSRequest->pkiMetaData[i]->AnySize, &tmpGenBuf));

		/************************************************/
		/*	jesli metadana PKI jest podpisem pliku	*/
		/************************************************/
		if(!strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE))
		{
			/**********************************************************************/
			/* wydobycie klucza symetrycznego z datagramu i stworzenie wlasciwego */
			/* kontekstu z zaznaczeniem ze szyfrowanie jest jednorundowe          */
			/**********************************************************************/
			if (bmdJSRequest->datagramType==BMD_DATAGRAM_ADD_SIGNATURE)
			{
				asprintf(&SQLQuery, 	"SELECT symkey FROM crypto_objects WHERE id=%s%s%s;",
								MaxId,
								_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
								_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
				if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

				BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &loOid));
				BMD_FOK(bmd_db_import_blob(hDB, loOid, &der_symkey));
				free(SQLQuery); SQLQuery=NULL;
				free(loOid); loOid=NULL;
			}
			else
			{
				BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest,&der_symkey));
			}

			BMD_FOK(bmd_decode_bmdKeyIV(der_symkey,NULL,data->serverPrivateKey,&sym_ctx));
			sym_ctx->sym->sym_params=BMD_SYM_SINGLE_ROUND;
			free_gen_buf(&der_symkey);

			/*************************************************************/
			/*	umieszczenie podpisu razem z jego szyfrowaniem	     */
			/*************************************************************/
			/*bmd_save_buf(tmpGenBuf, "/tmp/sig_buf");*/
			free0(signature_id);
			BMD_FOK(prfParseAndInsertSignature(	hDB, MaxId, INSERT_USER, sym_ctx, tmpGenBuf, &signature_id));
			bmd_ctx_destroy(&sym_ctx);

			/********************************************************/
			/*	Sprawdzam czy nalezy oznaczyc czasem podpis	*/
			/********************************************************/
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				// dla dodawania podpisu pades, mimo wlaczonego "asynchronicznego znakowania czasem" ma byc wykonane zwykle znakowanie (synchroniczne)
				if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0 ||
					(bmdJSRequest->datagramType==BMD_DATAGRAM_ADD_SIGNATURE && _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 1) )
				{
					BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( tmpGenBuf, SIGN_TS, PLAIN_DATA, &ts_buf));
					if(ts_buf!=NULL)
					{
						bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
						if(no_tsa_cert_number < 0)
						{
							/*Domyslnie nie ma byc ustawione*/
							no_tsa_cert_number = 0;
						}
						memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
						ConservationConfig.no_tsa_cert_number = 0;

						BMD_FOK(prfParseAndInsertTimestamp(	hDB, MaxId, signature_id, INSERT_USER, ts_buf,
						OID_PKI_METADATA_TIMESTAMP_FROM_SIG, &timestamp_id, 0, &ConservationConfig));
						free_gen_buf(&ts_buf);
					}
					bmd_ctx_destroy(&timestamp_hash_ctx);
					free_gen_buf(&timestamp_hash_buf);
				}
			}
		}
		else if(!strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_XADES))
		{

			/********************************************************************************/
			/*	wydobycie klucza symetrycznego z datagramu i stworzenie wlasciwego 	*/
			/*	kontekstu z zaznaczeniem ze szyfrowanie jest jednorundowe	 	*/
			/********************************************************************************/
			BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest,&der_symkey));
			BMD_FOK(bmd_decode_bmdKeyIV(der_symkey,NULL,data->serverPrivateKey,&sym_ctx));

			sym_ctx->sym->sym_params=BMD_SYM_SINGLE_ROUND;
			free_gen_buf(&der_symkey);

			/*************************************************************/
			/*	umieszczenie podpisu razem z jego szyfrowaniem	     */
			/*************************************************************/

			//Tutaj trzeba bedzie wstawic akcje zwiazana ze wstawianiem XADESa

			BMD_FOK(prfParseAndInsertSignatureinXades(hDB, MaxId, INSERT_USER, sym_ctx, tmpGenBuf, &signature_id));
			bmd_ctx_destroy(&sym_ctx);

			/********************************************************/
			/*	Sprawdzam czy nalezy oznaczyc czasem podpis	*/
			/********************************************************/

			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0)
				{
					BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( tmpGenBuf, SIGN_TS, PLAIN_DATA, &ts_buf));
					if(ts_buf!=NULL)
					{
						bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
						if(no_tsa_cert_number < 0)
						{
							/*Domyslnie nie ma byc ustawione*/
							no_tsa_cert_number = 0;
						}
						memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
						ConservationConfig.no_tsa_cert_number = 0;

						BMD_FOK(prfParseAndInsertTimestamp(	hDB, MaxId, signature_id, INSERT_USER, ts_buf,
						OID_PKI_METADATA_TIMESTAMP_FROM_SIG,&timestamp_id, 0, &ConservationConfig));
						free_gen_buf(&ts_buf);
					}
					bmd_ctx_destroy(&timestamp_hash_ctx);
					free_gen_buf(&timestamp_hash_buf);
				}
			}
		}
		/**** podpis pkcs#7 wyodrebniony z PAdES ****/
		else if(!strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_PADES))
		{

			/********************************************************************************/
			/*	wydobycie klucza symetrycznego z datagramu i stworzenie wlasciwego 	*/
			/*	kontekstu z zaznaczeniem ze szyfrowanie jest jednorundowe	 	*/
			/********************************************************************************/
			// przy dodawaniu podpisu pades realizowane wstawianiem nowej wersji sprawia,
			// ze datagram zadania uzupelniony jest o nowo wygenerowany klucz symetryczny
			// i nie trzeba pobierac symkey z bazy
			BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest,&der_symkey));
			BMD_FOK(bmd_decode_bmdKeyIV(der_symkey,NULL,data->serverPrivateKey,&sym_ctx));
			free_gen_buf(&der_symkey);
			sym_ctx->sym->sym_params=BMD_SYM_SINGLE_ROUND;

			/*************************************************************/
			/*	umieszczenie podpisu razem z jego szyfrowaniem	     */
			/*************************************************************/

			BMD_FOK(prfParseAndInsertSignatureinPades(hDB, MaxId, INSERT_USER, sym_ctx, tmpGenBuf, &signature_id));
			bmd_ctx_destroy(&sym_ctx);

			// wyszukiwanie skrotu z pdf - musi byc wydrebniony w przypadku PAdES
			BMD_FOK(GetPadesHashValueAndAlgorithm(bmdJSRequest, &padesHashHexValue, &padesHashAlgorithm));
			
			// wstawienie skrotu z podpisanej czesci pdf oraz oidu zastosowanej funkcji skrotu do tabeli prf_signatures
			BMD_FOK(InsertPadesHashToDatabase(hDB, padesHashHexValue, padesHashAlgorithm, signature_id));
			free(padesHashHexValue); padesHashHexValue=NULL;
			free(padesHashAlgorithm); padesHashAlgorithm=NULL;
			
			/********************************************************/
			/*	Sprawdzam czy nalezy oznaczyc czasem podpis	*/
			/********************************************************/

			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin != NULL && _GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_plugin_timestamp_pades != NULL)
			{
				// dla dodawania podpisu pades, mimo wlaczonego "asynchronicznego znakowania czasem" ma byc wykonane zwykle znakowanie (synchroniczne)
				if(_GLOBAL_std_conservate_configuration == NULL || _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 0 ||
					(bmdJSRequest->datagramType==BMD_DATAGRAM_ADD_SIGNATURE && _GLOBAL_std_conservate_configuration->asynchronous_timestamping == 1) )
				{
					if(_GLOBAL_std_conservate_configuration->insert_timestamp_into_pdf == 0)
					{
						BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( tmpGenBuf, SIGN_TS, PLAIN_DATA, &ts_buf));
						if(ts_buf!=NULL)
						{
							bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
							if(no_tsa_cert_number < 0)
							{
								/*Domyslnie nie ma byc ustawione*/
								no_tsa_cert_number = 0;
							}
							memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
							ConservationConfig.no_tsa_cert_number = 0;

							BMD_FOK(prfParseAndInsertTimestamp(	hDB, MaxId, signature_id, INSERT_USER, ts_buf,
							OID_PKI_METADATA_TIMESTAMP_FROM_SIG,&timestamp_id, 0, &ConservationConfig));
							free_gen_buf(&ts_buf);
						}
						bmd_ctx_destroy(&timestamp_hash_ctx);
						free_gen_buf(&timestamp_hash_buf);
					}
					else
					{
						BMD_FOK(SynchronousPadesTimestamping(hDB, MaxId, newHashHexString));
					}
				}
			}
		}
		/******************************************************/
		/*	jesli metadana PKI jest poswiadczeniem podpisu	*/
		/******************************************************/
		else if(!strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_DVCS))
		{

			/******************************************************************************/
			/*	wydobycie klucza symetrycznego z datagramu i stworzenie wlasciwego 	*/
			/*	kontekstu z zaznaczeniem ze szyfrowanie jest jednorundowe	 		*/
			/******************************************************************************/
			if (bmdJSRequest->datagramType==BMD_DATAGRAM_ADD_SIGNATURE)
			{
				asprintf(&SQLQuery, 	"SELECT symkey FROM crypto_objects WHERE id=%s%s%s;",
								MaxId,
								_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
								_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
				if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

				BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &loOid));
				BMD_FOK(bmd_db_import_blob(hDB, loOid, &der_symkey));
				free(SQLQuery); SQLQuery=NULL;
				free(loOid); loOid=NULL;
			}
			else
			{
				BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest,&der_symkey));
			}
			BMD_FOK(bmd_decode_bmdKeyIV(der_symkey,NULL,data->serverPrivateKey,&sym_ctx2));

			sym_ctx2->sym->sym_params=BMD_SYM_SINGLE_ROUND;
			free_gen_buf(&der_symkey);

			/************************************************/
			/*	umieszczenie szyfrogramu poswiadzcenia	*/
			/************************************************/
			free(dvcs_id); dvcs_id=NULL;
			if (metadataId<0)
			{
				BMD_FOK(prfInsertDVCS(	hDB, MaxId, signature_id, tmpGenBuf, metadataId, sym_ctx2, &dvcs_id));
			}
			else
			{
				BMD_FOK(prfInsertDVCS(	hDB, MaxId, signature_id, tmpGenBuf, metadataId,sym_ctx2, &dvcs_id));
			}
			bmd_ctx_destroy(&sym_ctx2);

			/*********************************************/
			/* Sprawdzam czy nalezy oznaczyc czasem dvcs */
			/*********************************************/
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( tmpGenBuf, DVCS_TS, PLAIN_DATA, &ts_buf));
				if(ts_buf != NULL)
				{
					bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
					if(no_tsa_cert_number < 0)
					{
						/*Domyslnie nie ma byc ustawione*/
						no_tsa_cert_number = 0;
					}
					memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
					ConservationConfig.no_tsa_cert_number = 0;

					free(timestamp_id); timestamp_id=NULL;
					BMD_FOK(prfParseAndInsertTimestamp(	hDB, MaxId, dvcs_id, INSERT_USER, ts_buf,
					OID_PKI_METADATA_TIMESTAMP_FROM_DVCS,&timestamp_id, 0, &ConservationConfig));
					free_gen_buf(&ts_buf);
				}
				free_gen_buf(&timestamp_hash_buf);
			}
		}
		/********************************************************/
		/*	jesli metadana PKI jest znacznikiem czasu pliku	*/
		/********************************************************/
		else if(!strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_TIMESTAMP))
		{
			free0(timestamp_id);
			bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
			if(no_tsa_cert_number < 0)
			{
				/*Domyslnie nie ma byc ustawione*/
				no_tsa_cert_number = 0;
			}
			memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
			ConservationConfig.no_tsa_cert_number = 0;

			BMD_FOK(prfParseAndInsertTimestamp(	hDB, MaxId, MaxId, INSERT_USER, tmpGenBuf, OID_PKI_METADATA_TIMESTAMP, &timestamp_id, 0,
			&ConservationConfig));
		}

		free_gen_buf(&tmpGenBuf);
	}

	free(signature_id); signature_id=NULL;
	free(dvcs_id); dvcs_id=NULL;
	free(metadataId); metadataId=NULL;
	free(timestamp_id); timestamp_id=NULL;

	return BMD_OK;
}

/************************************************/
/*		        NOP      		*/
/************************************************/
long PR_bmdDatagram_SQL_Ok(bmdDatagram_t *bmdJSResponse,long dtg_type)
{
long err=0;
	/** @bug to kasowalo hasha z datagramu!! */
	/* PR_bmdDatagram_init(bmdJSResponse); */
	bmdJSResponse->datagramType = dtg_type;
	bmdJSResponse->datagramStatus = BMD_OK;
	return err;
}

long PR_bmdDatagram_SQL_Err(bmdDatagram_t *bmdJSResponse,long dtg_type,long err_code)
{
long err=0;

	PR_bmdDatagram_init(bmdJSResponse);
	bmdJSResponse->datagramType = dtg_type;
	bmdJSResponse->datagramStatus = err_code;
	return err;
}


long JS_bmdDatagram_to_Nop(	bmdDatagram_t * bmdJSRequest,
				bmdDatagram_t ** bmdJSResponse)
{
	PRINT_INFO("LIBBMDSQLINF Datagram noop\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if(bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*bmdJSResponse) == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (bmdJSRequest->protocolVersion/100==(*bmdJSResponse)->protocolVersion/100)
	{
		PRINT_NOTICE("LIBBMDPKIINF Client's protocol version match\n");
		PR_bmdDatagram_PrepareResponse(bmdJSResponse, bmdJSRequest->datagramType+100, BMD_OK);
	}
	else
	{
		PRINT_ERROR("LIBBMDPKIERR Client's protocol version mismatch\n");
		PR_bmdDatagram_PrepareResponse(bmdJSResponse, bmdJSRequest->datagramType+100, VERSION_NUMBER_ERR);
	}
	return BMD_OK;
}

long JS_bmdDatagramSet_to_Nop(	bmdDatagramSet_t *bmdJSRequestSet,
				bmdDatagramSet_t ** bmdJSResponseSet)

{
long err = 0;
long i = 0;
long status=0;

	PRINT_INFO("LIBBMDSQLINF DatagramSet noop\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if (bmdJSRequestSet==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	status=PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet);
	BMD_FOK(status);

	for(i=0; i<(long)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{

		err=JS_bmdDatagram_to_Nop(bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
		if(err<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Error in datagram to nop. Error=%li.\n", err);
			continue;
		}
	}

	return BMD_OK;
}

