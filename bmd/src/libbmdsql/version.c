#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdlob/libbmdlob.h>
#include <bmd/libbmdsql/update.h> /*LockCryptoObjectsRow*/


long versionInsertNewFileVersionIntoDatabase(	void* hDB,
					bmdnet_handler_ptr handler,
					bmdDatagram_t* bmdJSRequest,
					bmdDatagram_t* bmdJSResponse,
					char** lobId,
					char **hash_as_hex_string,
					insert_data_t *data,
					u_int64_t *lob_size_for_db,
					server_request_data_t *req)
{
long status 			= 0;
long lob_chunk_size		= 0;
u_int64_t remaining_bytes	= 0;
u_int64_t lob_size		= 0;
u_int64_t counted_lob_size	= 0;
long last_lob_chunk		= -1;
struct lob_transport *lob	= NULL;
void *blob_locator		= NULL;
bmd_crypt_ctx_t *hash_ctx	= NULL;
bmd_crypt_ctx_t *lob_sym_ctx	= NULL;
GenBuf_t *der_lob_symkey	= NULL;
GenBuf_t *hash_buf		= NULL;

GenBuf_t gb;
GenBuf_t *gb_ptr		= &gb;

ts_info_t ts_info;
long	iterat = 0;				//zmienna zalozona na potrzeby ominiecia restrykcjnosci warningow o stalej petli

	if (req || bmdJSResponse)	//warunek podany dla ominiecia restrykcyjnosci warnigow o nie uzywanych paramtrach
	{	
		PRINT_INFO("LIBBMDSQLINF Starting InsertNewFileVersionIntoDatabase.\n");
	}
	memset( &gb, 0, sizeof( gb ) );
	
	PRINT_INFO("LIBBMDSQLINF Inserting LOB into database.\n");


	BMD_FOK_CHG(bmd_set_ctx_hash(&hash_ctx, BMD_HASH_ALGO_SHA1), LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR);
	hash_ctx->hash->hash_params=BMD_HASH_MULTI_ROUND;

	BMD_FOK_CHG(bmd2_datagram_get_symkey(bmdJSRequest, &der_lob_symkey), LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR);

	/* stworzenie wlasciwego kontekstu z zaznaczeniem ze szyfrowanie jest wielorundowe */
	BMD_FOK_CHG(bmd_decode_bmdKeyIV(der_lob_symkey,NULL,data->serverPrivateKey,&lob_sym_ctx),
			LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR);
	free_gen_buf(&der_lob_symkey);
	lob_sym_ctx->sym->sym_params=BMD_SYM_MULTI_ROUND;
	
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


	/* wiele iteracji */
	/************************************************************************/
	/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
	/************************************************************************/
	if (bmdJSRequest->datagramStatus==BMD_OK)
	{

		if( last_lob_chunk == 0 )
		{
			BMD_FOK_CHG(bmd_db_export_blob_begin(hDB, lobId, &blob_locator),
					LIBBMDSQL_INSERT_START_LOB_TRANSPORT_TO_DB_ERR);
			status=bmd_db_export_blob_continue(hDB, gb_ptr, blob_locator, LOB_FIRST_PIECE);
		}
		else
		{
			/* dla Oracle i innych zreszta baz w tym przypadku wystarczy normalny import BLOB'a
				dla Oracle jest blad bo tylko pierwszy PIECE wchodzil nie bylo kolejnych i zaraz
				szedl commit
				*/
			status=bmd_db_export_blob(hDB,gb_ptr,lobId);
		}
		if(status<BMD_OK)
		{
			BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
					LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);

			BMD_FOK(LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR);
		}
	}

	free_gen_buf(&gb_ptr);
	/* Strumieniowe wstawianie LOBow do bazy */
	if (last_lob_chunk==0)
	{
		while(iterat == 0)
		{

			status=recv_lob_chunk( &lob, &gb_ptr, 0, LARGE_FILE_DB_CHUNK_SIZE+1024 );
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
				if (bmdJSRequest->datagramStatus==BMD_OK)
				{
					status=bmd_db_export_blob_continue(hDB, gb_ptr, blob_locator, LOB_LAST_PIECE);
					free_gen_buf(&gb_ptr);
					if(status<BMD_OK)
					{
						PRINT_ERROR("LIBBMDSQLERR Error in iterate export LOB into database - LAST_PIECE. Error=%li\n", status);
						BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
								LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);

						BMD_FOK(LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR);
					}
				}

				break;
			}
			else
			{ /* ostatni fragment strumienia */
				PRINT_INFO("LIBBMDSQLINF Next piece of stream!\n");
				/************************************************************************/
				/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
				/************************************************************************/
				if (bmdJSRequest->datagramStatus==BMD_OK)
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
			}
		} /* end while */
	} /* last_lob_chunk == 0 */
	/************************************************************************/
	/*	jesli wczesniej nie wykryto bledu w datagramie zadania		*/
	/************************************************************************/
	if (bmdJSRequest->datagramStatus==BMD_OK)
	{
		if( last_lob_chunk == 0 )
		{
			BMD_FOK_CHG(bmd_db_export_blob_end(hDB, &blob_locator),
					LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR);
		}
	}
	else
	{
		BMD_FOK(bmdJSRequest->datagramStatus);
		/*Na wszelki wypadek*/
		return BMD_ERR_OP_FAILED;
	}

	// finalizacja obliczania skrotu z bloba
	BMD_FOK(get_lob_hash(&lob,&hash_buf));
	BMD_FOK(bmd_genbuf_hex_dump(hash_buf,hash_as_hex_string,0,1));

	*lob_size_for_db=counted_lob_size;

	/************************************************************************/
	/*	Znaczymy czasem BLOBa i umieszczamy znacznik w datagramie	*/
	/************************************************************************/
	
	/************************************************************************************/
	/*	znakujemy zwyklym znacznikiem czasu	*/
	/************************************************************************************/
	/*if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
	{

		BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hash_buf, MAIN_TS, HASH_DATA, &ts_buf));
		if (ts_buf!=NULL)
		{
			BMD_FOK(PR2_bmdDatagram_add_metadata(timestamp_metadata_oid, ts_buf->buf,ts_buf->size, PKI_METADATA, bmdJSRequest,0,0,0, 1));
			ts_buf->buf = NULL; ts_buf->size = 0;
			free_gen_buf(&ts_buf);
		}
	}*/


	/******************************************************************************/
	/*	sprawdzamy poprawnosc podpisu pod dokumentem na podstawe jego skrotu	*/
	/******************************************************************************/
	/*if(_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature)
	{

		status=_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature(bmdJSRequest, hash_buf);
		if (status<BMD_OK)
		{

			bmd_db_delete_blob(hDB, (*lobId));
			BMD_FOK(status);
		}
		else
		{

			free_lob_transport( &lob );
			bmd_ctx_destroy(&hash_ctx);
			bmd_ctx_destroy(&lob_sym_ctx);
			return status;

		}

	}*/

	free_lob_transport( &lob );
	bmd_ctx_destroy(&hash_ctx);
	bmd_ctx_destroy(&lob_sym_ctx);


	return bmdJSRequest->datagramStatus;
}


long versionInsertSymKeyIntoDatabase(void* hDB, bmdDatagram_t* bmdJSRequest, char** symKeyLobId)
{

long retVal		= 0;
GenBuf_t* tempSymKey	= NULL;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(symKeyLobId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	

	BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest, &tempSymKey));

	retVal=bmd_db_export_blob(hDB, tempSymKey, symKeyLobId);
	free_gen_buf(&tempSymKey);
	if(retVal < BMD_OK)
	{
		*symKeyLobId=NULL;
		BMD_FOK(retVal);
	}

	return BMD_OK;
}



long versionInsertToCryptoObjectsHistory(void* hDB, char* cryptoObjectId)
{

long retVal		= 0;
char* selectTemplate	= "SELECT filesize, raster, symkey, insert_date FROM crypto_objects WHERE id='%s' AND location_id='%s';";
char* insertTemplate	= "INSERT INTO crypto_objects_history (id, location_id, fk_crypto_object, filesize, raster, symkey, insert_date) VALUES(nextval('crypto_objects_history_id_seq'), '%s', '%s', '%s', '%s', '%s', '%s');";
char* sqlQuery		= NULL;
void* queryResult	= NULL;
db_row_strings_t* rowStruct	= NULL;
long fetchedCounter	= 0;

char* select_fileSize		= NULL;
char* select_raster		= NULL;
char* select_symkey		= NULL;
char* select_insert_date	= NULL;
char* escapeTemp		= NULL;

	if(hDB == NULL)
		{ return -1; }
	if(cryptoObjectId == NULL)
		{ return -2; }
	if(strlen(cryptoObjectId) <= 0)
		{ return -3; }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -4; }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ return -5; }

	retVal=bmd_db_escape_string(hDB, cryptoObjectId, STANDARD_ESCAPING, &escapeTemp);
	if(retVal < BMD_OK)
		{ return -30; }
	asprintf(&sqlQuery, selectTemplate, escapeTemp, _GLOBAL_bmd_configuration->location_id);
	free(escapeTemp); escapeTemp=NULL;
	if(sqlQuery == NULL)
		{ return -11; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
	{
		PRINT_ERROR("LIBBMDSQL Error in executing sql query (%li)\n", retVal);
		return -12;
	}

	retVal=bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowStruct, FETCH_ABSOLUTE, &fetchedCounter);
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
		{ return -13; }

	if(rowStruct->size != 4) //cztery kolumny
	{
		bmd_db_row_struct_free(&rowStruct);
		return -14;
	}

	select_fileSize=strdup(rowStruct->colvals[0]);
	select_raster=strdup(rowStruct->colvals[1]);
	select_symkey=strdup(rowStruct->colvals[2]);
	select_insert_date=strdup(rowStruct->colvals[3]);
	bmd_db_row_struct_free(&rowStruct);

	if(select_fileSize == NULL || select_raster == NULL || select_symkey == NULL || select_insert_date == NULL)
	{
		free(select_fileSize);
		free(select_raster);
		free(select_symkey);
		free(select_insert_date);
		return -15;
	}

	retVal=bmd_db_escape_string(hDB, cryptoObjectId, STANDARD_ESCAPING, &escapeTemp);
	if(retVal < BMD_OK)
	{
		free(select_fileSize);
		free(select_raster);
		free(select_symkey);
		free(select_insert_date);
		return -31;
	}
	asprintf(&sqlQuery, insertTemplate, _GLOBAL_bmd_configuration->location_id, escapeTemp, select_fileSize, select_raster, select_symkey, select_insert_date);
	free(escapeTemp); escapeTemp=NULL;
	free(select_fileSize); select_fileSize=NULL;
	free(select_raster); select_raster=NULL;
	free(select_symkey); select_symkey=NULL;
	free(select_insert_date); select_insert_date=NULL;

	if(sqlQuery == NULL)
		{ return -16; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery=NULL;

	if(retVal < BMD_OK)
	{
		PRINT_ERROR("LIBBMDSQL Error in executing sql query (%li)\n", retVal);
		return -17;
	}

	return 0;
}



long versionUpdateCryptoObjectSystemMetadata(void* hDB, char* cryptoObjectId, char* fileSize, char* raster, char* symkey, char* hashValueHexString)
{

char* updateTemplate	= "UPDATE crypto_objects SET filesize='%s', raster='%s', symkey='%s', insert_date=now(), hash_value='%s' WHERE id='%s' AND location_id='%s';";

char* sqlQuery		= NULL;
long retVal		= 0;
void* queryResult	= NULL;
char* escapeTemp	= NULL;


	if(hDB == NULL)
		{ return -1; }
	if(cryptoObjectId == NULL)
		{ return -2; }
	if(strlen(cryptoObjectId) <= 0)
		{ return -3; }
	if(fileSize == NULL)
		{ return -4; }
	if(strlen(fileSize) <= 0)
		{ return -5; }
	if(raster == NULL)
		{ return -6; }
	if(strlen(raster) <= 0)
		{ return -7; }
	if(symkey == NULL)
		{ return -8; }
	if(strlen(symkey) <= 0)
		{ return -9; }
	if(hashValueHexString == NULL)
		{ return -10; }
	if(strlen(hashValueHexString) <= 0)
		{ return -11; }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -12; }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ return -13; }

	retVal=bmd_db_escape_string(hDB, cryptoObjectId, STANDARD_ESCAPING, &escapeTemp);
	if(retVal < BMD_OK)
		{ return -30; }
	asprintf(&sqlQuery, updateTemplate, fileSize, raster, symkey, hashValueHexString, escapeTemp, _GLOBAL_bmd_configuration->location_id);
	free(escapeTemp); escapeTemp=NULL;
	if(sqlQuery == NULL)
		{ return -21; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery=NULL;
	
	if(retVal < BMD_OK)
	{
		PRINT_ERROR("LIBBMDSQL Error in executing sql query (%li)\n", retVal);
		return -22;
	}

	return 0;
}



long versionLinkProofsToCryptoObjectsHistoryEntry(void *hDB, char* cryptoObjectId)
{
char* signaturesQuery="UPDATE prf_signatures SET fk_older_version=currval('crypto_objects_history_id_seq'), fk_crypto_objects=NULL WHERE fk_crypto_objects='%s' AND location_id='%s';";
char* timestampsQuery="UPDATE prf_timestamps SET fk_older_version=currval('crypto_objects_history_id_seq'), fk_crypto_objects=NULL WHERE fk_crypto_objects='%s' AND location_id='%s';";
char* dvcsQuery="UPDATE prf_dvcs SET fk_older_version=currval('crypto_objects_history_id_seq'), fk_crypto_objects=NULL WHERE fk_crypto_objects='%s' AND location_id='%s';";

long retVal		= 0;
void* queryResult	= NULL;
char* sqlQuery		= NULL;
char* escapedCryptoObjectId	= NULL;

	if(hDB == NULL)
		{ return -1; }
	if(cryptoObjectId == NULL)
		{ return -2; }
	if(strlen(cryptoObjectId) <= 0)
		{ return -3; }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -4; }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ return -5; }

	retVal=bmd_db_escape_string(hDB, cryptoObjectId, STANDARD_ESCAPING, &escapedCryptoObjectId);
	if(retVal < BMD_OK)
		{ return -30; };
		
	asprintf(&sqlQuery, signaturesQuery, escapedCryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(sqlQuery == NULL)
	{
		free(escapedCryptoObjectId);
		return -11;
	}

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
	{
		free(escapedCryptoObjectId);
		return -12;
	}

	asprintf(&sqlQuery, timestampsQuery, escapedCryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(sqlQuery == NULL)
	{
		free(escapedCryptoObjectId);
		return -13;
	}

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
	{
		free(escapedCryptoObjectId);
		return -14;
	}

	asprintf(&sqlQuery, dvcsQuery, escapedCryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	free(escapedCryptoObjectId); escapedCryptoObjectId=NULL;
	if(sqlQuery == NULL)
		{ return -15; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
		{ return -16; }

	return 0;
}


long JS_bmdDatagram_to_updateFileVersion(void *hDB,
				bmdDatagram_t* bmdJSRequest,
				bmdDatagram_t** bmdJSResponse,
				bmdnet_handler_ptr handler,
				insert_data_t *data,
				server_request_data_t *req)
{
long retVal			= 0;

u_int64_t lobSize		= 0;
char* lobSizeString		= NULL;

char *newVersionLobId		= NULL;
char* symKeyLobId		= NULL;

char *hashValueHexString	= NULL;
char *fileType			= NULL;



	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequest->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponse == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*bmdJSResponse == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(handler == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(data == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }


	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, bmdJSRequest->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER,
				NULL/*fileName*/,
				&fileType,
				NULL/*pointingId*/,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL, NULL));


	if(strcmp(fileType, "dir")==0 || strcmp(fileType, "link")==0)
	{
		//tylko "prawdziwy dokument" moze byc modyfikowany
		free(fileType); fileType=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	free(fileType); fileType=NULL;


	//umieszczenie w bazie danych loba nowej wersji pliku
	retVal=versionInsertNewFileVersionIntoDatabase(hDB, handler, bmdJSRequest, *bmdJSResponse, &newVersionLobId,
					&hashValueHexString, data, &lobSize, req);


	
	// jesli wczesniej nie wykryto bledu w datagramie zadania
	// dopiero tutaj, poniewaz konieczne jest odebranie lobow od klienta,
	// zeby mozna bylo mu odeslac blad w datagramie odpowiedzi - w tym
	// wypadku nie zapisujemy odebranych danych w bazie danych
	if (retVal<BMD_OK && retVal!=LOB_CANCELED_BY_CLIENT)
	{
		PRINT_ERROR("LIBBMDSQL Unable to insert new version of file (status=%li)\n", retVal);
		BMD_FOK(retVal);
	}

	if (retVal==LOB_CANCELED_BY_CLIENT)
	{
		BMD_FOK(LOB_CANCELED_BY_CLIENT);
	}

	//umieszczenie w bazie danych loba klucza symetrycznego, ktorym zaszyfrowana jest nowa wersja pliku
	retVal=versionInsertSymKeyIntoDatabase(hDB, bmdJSRequest, &symKeyLobId);
	if(retVal < BMD_OK)
	{
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to insert symetric key to database (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//blokada, aby nie pomieszac rownoleglych operacji wstawiania nowej wersji
	retVal=LockCryptoObjectsRow(hDB, bmdJSRequest->protocolDataFileId->buf);
	if(retVal < BMD_OK)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to create row lock (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//wstawienie nowego rekordu do tabeli histori wersji pliku (crypto_objects_history)
	retVal=versionInsertToCryptoObjectsHistory(hDB, bmdJSRequest->protocolDataFileId->buf);
	if(retVal < BMD_OK)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to insert record to crypto_objects' history (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//asprintf(&lobSizeString, "%llu", lobSize);
	if(lobSizeString == NULL)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		BMD_FOK(NO_MEMORY);
	}

	//uaktualnienie metadanych systemowych dla aktualizowanego crypto_object
	retVal=versionUpdateCryptoObjectSystemMetadata(hDB, bmdJSRequest->protocolDataFileId->buf,
			lobSizeString, newVersionLobId/*raster*/, symKeyLobId, hashValueHexString);
	free(symKeyLobId); symKeyLobId=NULL;
	free(newVersionLobId); newVersionLobId=NULL;
	free(hashValueHexString); hashValueHexString=NULL;
	free(lobSizeString); lobSizeString=NULL;

	if(retVal < 0)
	{
		PRINT_ERROR("LIBBMDSQL Unable to update crypto objects (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//wkazanie (we wpisach dotychczasowej wartosci dowodowej) wersji przeniesionej do historii
	retVal=versionLinkProofsToCryptoObjectsHistoryEntry(hDB, bmdJSRequest->protocolDataFileId->buf);
	if(retVal < 0)
	{
		PRINT_ERROR("LIBBMDSQL Unable to link proofs for older version (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	// przepisanie do odpowiedzi identyfikatora aktualizowanego pliku
	free_gen_buf( &((*bmdJSResponse)->protocolDataFileId) );
	set_gen_buf2(bmdJSRequest->protocolDataFileId->buf, bmdJSRequest->protocolDataFileId->size , &((*bmdJSResponse)->protocolDataFileId) );

	/*W srodku hash_as_hex_string jest kopiowany do nowego bufora.*/
//	BMD_FOK(SetHashValueAndFileIdDatagram(hash_as_hex_string, &crypto_object_id, MaxId, LOB_NOT_IN_DATAGRAM, *bmdJSResponse, data,req));

	/******************************************************************************************/
	/*	ustawienie wlasciciela datagramu odpowiedzi na wlasciciela datagramu wyjsciowego	*/
	/******************************************************************************************/
// 	if (bmdJSRequest->protocolDataOwner!=NULL)
// 	{
// 		if (bmdJSRequest->protocolDataOwner->buf!=NULL)
// 		{
// 			if ((*bmdJSResponse)->protocolDataOwner==NULL)
// 			{
// 				(*bmdJSResponse)->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
// 				(*bmdJSResponse)->protocolDataOwner->buf=(char*)malloc(bmdJSRequest->protocolDataOwner->size+2);
// 				memset((*bmdJSResponse)->protocolDataOwner->buf, 0, bmdJSRequest->protocolDataOwner->size+1);
// 				memcpy((*bmdJSResponse)->protocolDataOwner->buf, bmdJSRequest->protocolDataOwner->buf,bmdJSRequest->protocolDataOwner->size);
// 				(*bmdJSResponse)->protocolDataOwner->size=bmdJSRequest->protocolDataOwner->size;
// 			}
// 		}
// 	}

	return BMD_OK;
}



long JS_bmdDatagramSet_to_updateNewVersion(void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					bmdnet_handler_ptr handler,
					insert_data_t *data,
					server_request_data_t *req)
{

long status	= 0;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequestSet->bmdDatagramSetSize != 1)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(handler == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(data == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }
	


	// przygotowanie datagramu odpowiedzi
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	
	// poczatek transakcji
	BMD_FOK(bmd_db_start_transaction(hDB, BMD_TRANSACTION_LEVEL_NONE));

	// obsluzenie datagramu zadania
	status=JS_bmdDatagram_to_updateFileVersion(hDB,
			bmdJSRequestSet->bmdDatagramSetTable[0], &((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
			handler, data, req);
	

	// przygotowanie datagramu odpowiedzi
 	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus = status;
 	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType = BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO_RESPONSE;

	// przepisanie nazwy pliku
// 	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFilename=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
// 	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFilename->buf=(char *)calloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFilename->size+1, sizeof(char));
// 	memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFilename->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFilename->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFilename->size+1);
// 	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFilename->size=bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFilename->size;

	// przepisanie zadajacego do odpowiedzi serwera
// 	if(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner != NULL)
// 	{
// 		if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf!=NULL)
// 		{
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->buf=(char*)calloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+1, sizeof(char));
// 			memmove((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size);
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->size=bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size;
// 		}
// 	}

	// zarejestrowanie akcji w bazie danych
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], (*bmdJSResponseSet)->bmdDatagramSetTable[0], req);
		if(status<BMD_OK)
		{
			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus = status;
		}
	}

	if ((*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus<BMD_OK)
	{
		bmd_db_rollback_transaction(hDB, NULL);
	}
	else
	{
		bmd_db_end_transaction(hDB);
	}
	
	return BMD_OK;
}


/**
* @author WSZ
* Funkcja GetArchiveVersionIds() wyszukuje archiwalne wersje pliku fileId. Jesli plik fileId nie ma zadnej starszej wersji,
* wowczas na wyjsciu otrzymuje sie pusta liste archiveIds (i opcjonalnie insertDate), czyli zawierajaca 0 elementow.
* W wynikowej liscie nie jest uwzgledniony plik fileId (tylko jego starsze wersje).
* @param[in] hDB to przygotowane wczesniej polaczenie bazodanowe
* @param[in] fileId to identyfikator pliku (nie moze byc link), ktorego wersje archiwalne maja byc znalezione
* @param[out] archiveIds to alokowana lista identyfikatorow znalezionych wersji archiwalnych pliku fileId
* @param[out] insertDate to alokowana lista z datami wstawienia do achiwum znalezionych wersji archiwalnych pliku fileId. Parametr jest opcjonalny - mozna podac NULL.
* Kolejnosc dat w liscie insertDate odpowiada kolejnosci identyfikatorow w liscie archiveIds.
* @return W przypadku sukcesu operacji funkcja zwraca 0, a w przypadku niepoprawnego wykonania wartosc ujemna.
*/
long GetArchiveVersionIds(void *hDB, char* fileId, bmdStringArray_t** archiveIds, bmdStringArray_t** insertDates)
{
long retVal							= 0;
long iter							= 0;

bmdStringArray_t *tempArchiveIds	= NULL;
bmdStringArray_t *tempInsertDates	= NULL;

long rowsCount						= 0;
char* sqlQuery						= NULL;
void* queryResult					= NULL;
db_row_strings_t* rowStruct			= NULL;
long fetchedCounter					= 0;
char *queryTemplate="SELECT id, to_char(insert_date, 'YYYY-MM-DD HH24:MI:SS') FROM crypto_objects_history WHERE fk_crypto_object=%s AND location_id=%s ORDER BY insert_date ASC;";
char* escapeTemp					= NULL;

	if(hDB == NULL)
		{ return -1; }
	if(fileId == NULL)
		{ return -2; }
	if(strlen(fileId) <= 0)
		{ return -3; }
	if(archiveIds == NULL)
		{ return -4;}
	if(*archiveIds != NULL)
		{ return -5; }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -6; }


	retVal=bmdStringArray_Create(&tempArchiveIds);
	if(retVal < 0)
		{ return -21; }
	
	if(insertDates != NULL)
	{
		retVal=bmdStringArray_Create(&tempInsertDates);
		if(retVal < 0)
		{
			bmdStringArray_DestroyList(&tempArchiveIds);
			return -22;
		}
	}

	retVal=bmd_db_escape_string(hDB, fileId, STANDARD_ESCAPING, &escapeTemp);
	if(retVal < BMD_OK)
	{
		bmdStringArray_DestroyList(&tempInsertDates);
		bmdStringArray_DestroyList(&tempArchiveIds);
		return -30;
	}
	asprintf(&sqlQuery, queryTemplate, escapeTemp, _GLOBAL_bmd_configuration->location_id);
	free(escapeTemp); escapeTemp=NULL;
	if(sqlQuery == NULL)
	{
		bmdStringArray_DestroyList(&tempInsertDates);
		bmdStringArray_DestroyList(&tempArchiveIds);
		return -23;
	}

	retVal=bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
	{
		bmdStringArray_DestroyList(&tempInsertDates);
		bmdStringArray_DestroyList(&tempArchiveIds);
		return -24;
	}

	for(iter=0; iter<rowsCount; iter++)
	{
		retVal=bmd_db_result_get_row(hDB, queryResult, iter, 0, &rowStruct, FETCH_ABSOLUTE, &fetchedCounter);
		if(retVal < BMD_OK)
		{
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempInsertDates);
			bmdStringArray_DestroyList(&tempArchiveIds);
			return -25;
		}

		retVal=bmdStringArray_AddItem(tempArchiveIds, rowStruct->colvals[0]);
		if(retVal < 0)
		{
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempInsertDates);
			bmdStringArray_DestroyList(&tempArchiveIds);
			return -26;
		}

		if(tempInsertDates != NULL)
		{
			retVal=bmdStringArray_AddItem(tempInsertDates, rowStruct->colvals[1]);
			if(retVal < 0)
			{
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				bmdStringArray_DestroyList(&tempInsertDates);
				bmdStringArray_DestroyList(&tempArchiveIds);
				return -27;
			}
		}
		
		bmd_db_row_struct_free(&rowStruct);
	}
	bmd_db_result_free(&queryResult);

	*archiveIds=tempArchiveIds;
	tempArchiveIds=NULL;
	
	if(insertDates != NULL)
	{
		*insertDates=tempInsertDates;
		tempInsertDates=NULL;
	}
	
	return 0;
}


long JS_bmdDatagram_to_getVersionsHistory(void *hDB, bmdDatagram_t* requestDatagram, bmdDatagram_t* responseDatagram, server_request_data_t *req)
{
long iter							= 0;
long retVal							= 0;
bmdStringArray_t* archiveIds		= NULL;
bmdStringArray_t* insertDates		= NULL;
long archiveVersionsCount			= 0;
char* fileId						= NULL;

char* tempArchiveIdPtr					= NULL;
char* tempInsertDatePtr					= NULL;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram->datagramType != BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_FILE_ID_NOT_SPECIFIED); }
	if(requestDatagram->protocolDataFileId->buf == NULL || requestDatagram->protocolDataFileId->size <= 0)
		{ BMD_FOK(BMD_ERR_FILE_ID_NOT_SPECIFIED); }
	if(responseDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	
	
	// przekopiowania protocolDatFileId do odpowiedzi
	free_gen_buf( &(responseDatagram->protocolDataFileId) );
	set_gen_buf2( requestDatagram->protocolDataFileId->buf, requestDatagram->protocolDataFileId->size, &(responseDatagram->protocolDataFileId) );
	
	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, requestDatagram->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS, NULL, NULL, &fileId, NULL, NULL, NULL, NULL));
	
	retVal=GetArchiveVersionIds(hDB, fileId, &archiveIds, &insertDates);
	free(fileId); fileId=NULL;
	if(retVal < 0)
	{
		PRINT_ERROR("Unable to get information about archive versions of specified file. (%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	retVal=bmdStringArray_GetItemsCount(archiveIds, &archiveVersionsCount);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&insertDates);
		bmdStringArray_DestroyList(&archiveIds);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	for(iter=0; iter < archiveVersionsCount; iter++)
	{
		retVal=bmdStringArray_GetItemPtr(archiveIds, iter, &tempArchiveIdPtr);
		if(retVal < 0)
		{
			bmdStringArray_DestroyList(&insertDates);
			bmdStringArray_DestroyList(&archiveIds);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_ID, tempArchiveIdPtr, &responseDatagram);
		tempArchiveIdPtr=NULL;
		if(retVal < BMD_OK)
		{
			bmdStringArray_DestroyList(&insertDates);
			bmdStringArray_DestroyList(&archiveIds);
			BMD_FOK(retVal);
		}
		
		retVal=bmdStringArray_GetItemPtr(insertDates, iter, &tempInsertDatePtr);
		if(retVal < 0)
		{
			bmdStringArray_DestroyList(&insertDates);
			bmdStringArray_DestroyList(&archiveIds);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP, tempInsertDatePtr, &responseDatagram);
		tempInsertDatePtr=NULL;
		if(retVal < BMD_OK)
		{
			bmdStringArray_DestroyList(&insertDates);
			bmdStringArray_DestroyList(&archiveIds);
			BMD_FOK(retVal);
		}
	}
	
	bmdStringArray_DestroyList(&insertDates);
	bmdStringArray_DestroyList(&archiveIds);
	
	return BMD_OK;
}


long JS_bmdDatagramSet_to_getVersionsHistory(void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{

long iter			= 0;
long status			= 0;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	

	// przygotowanie datagramu odpowiedzi
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	
	for(iter=0; iter < bmdJSRequestSet->bmdDatagramSetSize; iter++)
	{
		status=JS_bmdDatagram_to_getVersionsHistory(hDB, bmdJSRequestSet->bmdDatagramSetTable[iter], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus = status;
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramType = BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY_RESPONSE;
	}


	// przepisanie zadajacego do odpowiedzi serwera
// 	if(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner != NULL)
// 	{
// 		if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf!=NULL)
// 		{
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->buf=(char*)calloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+1, sizeof(char));
// 			memmove((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size);
// 			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataOwner->size=bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size;
// 		}
// 	}

	// zarejestrowanie akcji w bazie danych
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		for(iter=0; iter < bmdJSRequestSet->bmdDatagramSetSize; iter++)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[iter], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
			if(status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus = status;
			}
		}
	}

	return BMD_OK;
}
