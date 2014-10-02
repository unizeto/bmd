
#include <bmd/libbmdsql/timestamp.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>
#include<bmd/libbmdsql/proofs.h>
#include<bmd/libbmdglobals/libbmdglobals.h>


#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

/**********************************************************/
/* zmiana domyślnej (wybranej/aktualnej) roli użytkownika */
/**********************************************************/
long JS_bmdDatagramSet_to_conservateTimestamps(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
char *SQLQueryFull 		= NULL;
char *SQLQueryLimits		= NULL;
char *SelectQueryColumns	= NULL;
char *lo_oid			= NULL;
char *fk_crypto_objects		= NULL;
char *fk_objects		= NULL;
char *timestamp_type		= NULL;
char *timestamp_id		= NULL;
char *new_timestamp_id		= NULL;
GenBuf_t *plainDataGenBuf	= NULL;
GenBuf_t *timestampedDataGenBuf	= NULL;
void *SelectResult 		= NULL;
long fetched_counter1		= 0;
long err			= 0;
long no_tsa_cert_number		= 0;

/*na potrzeby synchronizacji*/
long retVal = 0;
void *selectConservedResult = NULL;
char *selectConservedCommand = NULL;
char *conservedValue = NULL;
long fetchParameter = 0;
ConservationConfig_t ConservationConfig;


	PRINT_INFO("LIBBMDSQLINF Kernel system selecting datagram\n");
	/************************/
	/* WALIDACJA PARAMETROW */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponseSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bmdJSResponseSet != NULL)	{       BMD_FOK(BMD_ERR_PARAM3);        }

	/***********************************/
	/* stworzenie datagramu odpowiedzi */
	/***********************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));

	/*********************************************************************/
	/* sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania */
	/*********************************************************************/
	if (bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus<BMD_OK)
	{
		PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[0]), bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus);
		return BMD_OK;
	}

	BMD_FOK(GenerateSearchQuery(		hDB,
						bmdJSRequestSet->bmdDatagramSetTable[0],
						0,
						0,
						&SQLQueryFull,
						&SQLQueryLimits,
						&SelectQueryColumns,
						req));

	/***************************/
	/* wykonanie zapytania SQL */
	/***************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQueryFull, NULL, NULL, &SelectResult));

	free0(SQLQueryFull);
	free0(SQLQueryLimits);
	free0(SelectQueryColumns);

	/***********************************************/
	/* ponowne oznakowanie czasem wybranych danych */
	/***********************************************/
	fetched_counter1 = 0;
	while(err>=0)
	{
		if (_GLOBAL_interrupt==1)
		{
			break;
		}
		free0(lo_oid);
		free0(fk_crypto_objects);
		free0(fk_objects);
		free0(timestamp_id);
		free0(new_timestamp_id);

		/********************************************/
		/* pobierz raster i klucz glowny znacznikow */
		/* oraz obcy do crypto_objects              */
		/********************************************/
		/*ponizsza funkcja nie odwoluje sie do bazy tylko pracuje na wyniku z ostatniego zapytania (iterator zapytania)*/
		err = prfGetTimestampKeyData(hDB, &fetched_counter1, SelectResult, &lo_oid, &fk_crypto_objects, &fk_objects, &timestamp_id, &timestamp_type);
		if (err==BMD_ERR_NODATA)
		{
			PRINT_NOTICE("LIBBMDSQLNOT No timestamps for conservation\n");
			if (fetched_counter1!=0) err=BMD_OK;
			break;
		}
		BMD_FOK(err);


		/***********************************
		Synchronizacja procesow - znacznik zakonserwowany w miedzyczasie przez inny proces
		nie moze byc juz ponownie konserwowany (zaburzenie lancucha konserwacji)
		***********************************/

		if(timestamp_id == NULL)
		{
			bmd_db_result_free(&SelectResult);
			free0(lo_oid);
			free0(fk_crypto_objects);
			free0(fk_objects);
			free0(timestamp_id);
			free0(timestamp_type);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		/***************************
		(synchronizacja) rozpoczecie transakcji na konserwacje pojedynczego znacznika czasu
		***************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_SERIALIZABLE));

		/**********************************************************************
		(synchronizacja) zalozenie blokady na wiersz odpowiadajacy konserwowanemu znacznikowi
		**********************************************************************/
		BMD_FOK_TRANS(LockPrfTimestampsRow(hDB, timestamp_id));

		/************************
		(synchronizacja) sprawdzenie, czy w miedzyczasie znacznik zostal zakonserwowany przez inny proces
		pobranie aktualnej wartosci pola conserved
		************************/
		asprintf(&selectConservedCommand, "SELECT conserved FROM prf_timestamps WHERE id=%s;", timestamp_id);
		retVal=bmd_db_execute_sql(hDB, selectConservedCommand, NULL, NULL, &selectConservedResult);
		free0(selectConservedCommand);
		if(retVal < 0)
		{
			bmd_db_result_free(&SelectResult);
			free0(lo_oid);
			free0(fk_crypto_objects);
			free0(fk_objects);
			free0(timestamp_id);
			free0(timestamp_type);
			BMD_FOK_TRANS(retVal);
		}

		fetchParameter=0;
		retVal=bmd_db_result_get_value(hDB, selectConservedResult, 0, 0, &conservedValue,FETCH_ABSOLUTE, &fetchParameter);
		bmd_db_result_free(&selectConservedResult);
		if( retVal<0 || conservedValue == NULL)
		{
			bmd_db_result_free(&SelectResult);
			free0(lo_oid);
			free0(fk_crypto_objects);
			free0(fk_objects);
			free0(timestamp_id);
			free0(timestamp_type);
			BMD_FOK_TRANS(retVal);
		}

		/**************************************
		(synchronizacja) jesli znacznik zostal juz zakonserwowany przez inny proces to nie bedzie ponownie konserwowany
		**************************************/
		retVal=strcmp(conservedValue, "0");
		free0(conservedValue);
		if(retVal != 0)
		{
			BMD_FOK_TRANS(bmd_db_end_transaction(hDB));
			continue;
		}


		/**********************************/
		/* zaimportowanie znacznika czasu */
		/**********************************/
		BMD_FOK_TRANS(prfImportTimestampToGenBuf(hDB, lo_oid, &plainDataGenBuf));

		/******************************/
		/* ponowne oznakowanie czasem */
		/******************************/
		if (strcmp(timestamp_type, OID_PKI_METADATA_TIMESTAMP)==0)
		{
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( plainDataGenBuf, MAIN_TS, PLAIN_DATA, &timestampedDataGenBuf);
			}
		}
		else if (strcmp(timestamp_type, OID_PKI_METADATA_TIMESTAMP_FROM_SIG)==0)
		{
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( plainDataGenBuf, SIGN_TS, PLAIN_DATA, &timestampedDataGenBuf);
			}
		}
		else if (strcmp(timestamp_type, OID_PKI_METADATA_TIMESTAMP_FROM_DVCS)==0)
		{
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( plainDataGenBuf, DVCS_TS, PLAIN_DATA, &timestampedDataGenBuf);
			}
		}

		/*************************************/
		/* umieszczenie oznakowanego w bazie */
		/*************************************/
		if (timestampedDataGenBuf!=NULL)
		{
			bmdconf_get_value_int(_GLOBAL_StdConservate_conf,"plugin_StdConservate","no_tsa_cert_number", &no_tsa_cert_number);
			if(no_tsa_cert_number < 0)
			{
				/*Domyslnie nie ma byc ustawione*/
				no_tsa_cert_number = 0;
			}
			memset(&ConservationConfig, 0, sizeof(ConservationConfig_t));
			ConservationConfig.no_tsa_cert_number = 0;

			BMD_FOK_TRANS(prfParseAndInsertTimestamp(	hDB,
									fk_crypto_objects,
									fk_objects,
									0,
									timestampedDataGenBuf,
									timestamp_type,
									&new_timestamp_id, 0, &ConservationConfig));

			/***********************************************/
			/* oznaczenie timestampa jako zakonserwowanego */
			/***********************************************/
			BMD_FOK_TRANS(prfUpdateConservedTimestampStatus(hDB, timestamp_id, new_timestamp_id));
		}

		/***********************
		(synchronizacja) zakonczenie transakcji
		***********************/
		BMD_FOK_TRANS(bmd_db_end_transaction(hDB));

		free_gen_buf(&plainDataGenBuf);
		free_gen_buf(&timestampedDataGenBuf);
		free0(lo_oid);
		free0(fk_crypto_objects);
		free0(fk_objects);
		free0(timestamp_id);
		free0(timestamp_type);
	}

	BMD_FOK(PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[0]), bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100, err));

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&SelectResult);
	free_gen_buf(&plainDataGenBuf);
	free_gen_buf(&timestampedDataGenBuf);
	free0(lo_oid);
	free0(fk_crypto_objects);
	free0(fk_objects);
	free0(timestamp_id);
	free0(timestamp_type);

	return BMD_OK;
}


/**
* @author WSz
* Funkcja znakuje czasem (podpis) i umieszcza znacznik wewnatrz podpisanego pdf w trybie synchronicnym (znakowanie bezposrednio przy wstawianiu do archiwum)
*/
long SynchronousPadesTimestamping(const void *hDB, const char* cryptoObjectId, char **newHashHexString)
{
long retVal			= 0;
long iter			= 0;
char *queryTemplate	= "SELECT raster, symkey FROM crypto_objects WHERE id='%s' AND location_id='%s' ; ";
char *sqlQuery		= NULL;
void* queryResult			= NULL;
long rowsCount				= 0;
db_row_strings_t *rowStruct	= NULL;
long fetched				= 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(cryptoObjectId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(strlen(cryptoObjectId) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(newHashHexString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*newHashHexString != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	if(_GLOBAL_std_conservate_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_std_conservate_configuration->enable_signature_ts == 0)
		{ return BMD_OK; }
	if(_GLOBAL_std_conservate_configuration->insert_timestamp_into_pdf == 0)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	retVal = asprintf(&sqlQuery, queryTemplate, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(retVal == -1)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	
	retVal = bmd_db_execute_sql((void*)hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	BMD_FOK(retVal);
	
	if(rowsCount == 0)
	{
		bmd_db_result_free(&queryResult);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	retVal = bmd_db_result_get_row((void*)hDB, queryResult, iter, 0, &rowStruct, FETCH_ABSOLUTE, &fetched);
	if(retVal < BMD_OK)
	{
		bmd_db_result_free(&queryResult);
		BMD_FOK(retVal);
	}
	
	if(rowStruct->size != 2 || rowStruct->colvals == NULL)
	{
		bmd_db_row_struct_free(&rowStruct);
		bmd_db_result_free(&queryResult);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	retVal = TimestampAndReplacePades(hDB, cryptoObjectId, rowStruct->colvals[0], rowStruct->colvals[1], newHashHexString);
	bmd_db_row_struct_free(&rowStruct);
	bmd_db_result_free(&queryResult);
	BMD_FOK(retVal);
	
	return BMD_OK;
}


/**
* @author WSz
* Funkcja narzedziowa znakujaca czasem podpisany pdf (podpis) umieszczajac znacznik wewnatrz pdf.
* @param[out] newHashHexString to skrot (hex) z pdf uzupelnionego o znacznik czasu dla podpisu
*/
long TimestampAndReplacePades(const void *hDB, const char* cryptoObjectId, const char* padesRaster, const char* symkeyRaster, char **newHashHexString)
{
long retVal								= 0;
char *newRasterNumber					= NULL;
char *newTimestampId					= NULL;
bmd_crypt_ctx_t *symmetricCtx			= NULL;
bmd_crypt_ctx_t *serverCtxPtr			= NULL;
bmd_crypt_ctx_t *hashCtx				= NULL;
GenBuf_t *symkeyBuf						= NULL;
GenBuf_t *tempBuf						= NULL;
GenBuf_t *padesForTimestamping			= NULL;
GenBuf_t *timestampedPades				= NULL;
GenBuf_t *timestamp						= NULL;
GenBuf_t *encTimestampedPades			= NULL;
GenBuf_t *hashValue						= NULL;
char *hexHashValue						= NULL;
char *signatureId						= NULL;

char *sigSelectTemplate			= "SELECT id FROM prf_signatures WHERE fk_crypto_objects='%s' AND location_id='%s' AND standard='%li' LIMIT 1; ";
char *updateQueryTemplate		= "UPDATE crypto_objects SET raster='%s', hash_value='%s' WHERE id='%s' AND location_id='%s' ; ";
char *insertTemplate			= "INSERT INTO rep_ts_bill (crypto_object_id, lob_id) VALUES ('%s', '%s') ; ";
char *sqlQuery			= NULL;
long rowsCount			= 0;
void *queryResult		= NULL;
long fetched			= 0;

ConservationConfig_t ConservationConfig;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(cryptoObjectId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(strlen(cryptoObjectId) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(padesRaster == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(strlen(padesRaster) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(symkeyRaster == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(strlen(symkeyRaster) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(newHashHexString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(*newHashHexString != NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
		
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->crypto_conf == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_plugin_timestamp_pades == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	if(_GLOBAL_std_conservate_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	if(_GLOBAL_std_conservate_configuration->enable_signature_ts == 0)
		{ return BMD_OK; }
	
	if(_GLOBAL_std_conservate_configuration->insert_timestamp_into_pdf == 0)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	serverCtxPtr=_GLOBAL_bmd_configuration->crypto_conf->server_ctx;
	memset(&ConservationConfig, 0 , sizeof(ConservationConfig_t));

	// pobranie id wyciagnietego podpisu PKCS#7
	retVal = asprintf(&sqlQuery, sigSelectTemplate, cryptoObjectId, _GLOBAL_bmd_configuration->location_id, STANDARD_PADES);
	if(retVal == -1)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	
	retVal = bmd_db_execute_sql((void*)hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	if(retVal < BMD_OK)
		{ BMD_FOK(retVal); }
	
	// jesli niepodpisany pdf, to nie ma zadnego podpisu pades
	if(rowsCount == 0)
	{
		bmd_db_result_free(&queryResult);
		return BMD_OK;
	}
	
	retVal = bmd_db_result_get_value((void*)hDB, queryResult, 0, 0, &signatureId, FETCH_ABSOLUTE, &fetched);
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
		{ BMD_FOK(retVal); }
	
	// pobranie pdf
	retVal = bmd_db_import_blob((void*)hDB, (char*)padesRaster, &tempBuf);
	if(retVal < BMD_OK)
	{
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// pobranie loba symkey
	retVal = bmd_db_import_blob((void*)hDB, (char*)symkeyRaster, &symkeyBuf);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&tempBuf);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// deszyfrowanie symkey
	retVal = bmd_decode_bmdKeyIV(symkeyBuf, NULL, serverCtxPtr, &symmetricCtx);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&symkeyBuf);
		free_gen_buf(&tempBuf);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// zdeszyfrowanie pdf do oznakowania
	retVal = bmdpki_symmetric_decrypt(symmetricCtx, tempBuf, BMDPKI_SYM_LAST_CHUNK, &padesForTimestamping);
	free_gen_buf(&tempBuf);
	bmd_ctx_destroy(&symmetricCtx);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&symkeyBuf);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	retVal = _GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_plugin_timestamp_pades(padesForTimestamping, &timestampedPades, &timestamp);
	free_gen_buf(&padesForTimestamping);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&symkeyBuf);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// przygotowanie kontesktu do szfrowania
	retVal = bmd_decode_bmdKeyIV(symkeyBuf, NULL, serverCtxPtr, &symmetricCtx);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free_gen_buf(&symkeyBuf);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// szyfrowanie oznakowanego czasem pdf'a
	retVal = bmdpki_symmetric_encrypt(symmetricCtx, timestampedPades, BMDPKI_SYM_LAST_CHUNK, &encTimestampedPades);
	bmd_ctx_destroy(&symmetricCtx);
	free_gen_buf(&symkeyBuf);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// wstawienie oznakowanego Pades do bazy
	retVal = bmd_db_export_blob((void*)hDB, encTimestampedPades, &newRasterNumber);
	free_gen_buf(&encTimestampedPades);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// odlozone usuniecie starego padesa
	//retVal = bmd_db_delete_blob((void*)hDB, (char*)padesRaster);
	retVal = asprintf(&sqlQuery, insertTemplate, cryptoObjectId, padesRaster);
	if(retVal == -1)
	{
		free(newRasterNumber); newRasterNumber = NULL;
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// przepisanie do tabeli rep_ts_bill informacji potrzebnych do usuniecia podmienionego loba pdf
	retVal = bmd_db_execute_sql((void*)hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
	{
		free(newRasterNumber); newRasterNumber = NULL;
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	// wyliczenie skrotu z oznakowanego pdf
	retVal = bmd_set_ctx_hash(&hashCtx, BMD_HASH_ALGO_SHA1);
	if(retVal < BMD_OK)
	{
		free(newRasterNumber); newRasterNumber = NULL;
		free_gen_buf(&timestamp);
		free_gen_buf(&timestampedPades);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
		
	retVal = bmd_hash_data(timestampedPades, &hashCtx, &hashValue, NULL);
	bmd_ctx_destroy(&hashCtx);
	free_gen_buf(&timestampedPades);
	if(retVal < BMD_OK)
	{
		free(newRasterNumber); newRasterNumber = NULL;
		free_gen_buf(&timestamp);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	retVal = bmd_genbuf_hex_dump(hashValue, &hexHashValue, 0,1);
	free_gen_buf(&hashValue);
	if(retVal < BMD_OK)
	{
		free(newRasterNumber); newRasterNumber = NULL;
		free_gen_buf(&timestamp);
		free(signatureId); signatureId = NULL;
		BMD_FOK(retVal);
	}
	
	retVal = asprintf(&sqlQuery, updateQueryTemplate, newRasterNumber, hexHashValue, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	free(newRasterNumber); newRasterNumber = NULL;
	if(retVal == -1)
	{
		free(hexHashValue); hexHashValue = NULL;
		free_gen_buf(&timestamp);
		free(signatureId); signatureId = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	retVal = bmd_db_execute_sql((void*)hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
	{
		free(hexHashValue); hexHashValue = NULL;
		free_gen_buf(&timestamp);
		free(signatureId); signatureId = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	// wstawienie znacznika do bazy
	retVal = prfParseAndInsertTimestamp((void*)hDB,
				(char*)cryptoObjectId,
				signatureId,
				INSERT_BMD,
				timestamp,
				OID_PKI_METADATA_TIMESTAMP_FROM_SIG,
				&newTimestampId,
				0,
				&ConservationConfig);
	free(newTimestampId);
	free(signatureId); signatureId = NULL;
	free_gen_buf(&timestamp);
	if(retVal < BMD_OK)
	{
		free(hexHashValue); hexHashValue = NULL;
		BMD_FOK(retVal);
	}
	
	*newHashHexString = hexHashValue;
	hexHashValue = NULL;
	
	return BMD_OK;
}


long JS_bmdDatagramSet_to_deleteReplacedLobs(bmdDatagramSet_t *requestSet,
											bmdDatagramSet_t **responseSet,
											server_request_data_t *req)
{
long retVal					= 0;
void *hDB					= NULL;
long deletedCounter			= 0;
char *deletedCounterString	= NULL;
long enable_lobs_removing	= 0;

	PRINT_INFO("LIBBMDSQLINF Deleting replaced lobs\n");

	if(requestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestSet->bmdDatagramSetSize <= 0 || requestSet->bmdDatagramSetTable == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(responseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*responseSet != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_StdClear_conf == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, responseSet));
	
	BMD_FOK(PR_bmdDatagram_PrepareResponse(	&((*responseSet)->bmdDatagramSetTable[0]), BMD_DATAGRAM_TYPE_DELETE_REPLACED_LOBS_RESPONSE, BMD_OK));
	
	BMD_FOK(bmdconf_get_value_int(_GLOBAL_StdClear_conf, "plugin_StdClear_lobsRemover", "enable_lobs_removing", &enable_lobs_removing));
	if(enable_lobs_removing <= 0)
	{
		Q_DENY("LOBSREMOVER Lobs removing is disabled\n");
		BMD_FOK(set_gen_buf2("0", strlen("0"), &((*responseSet)->bmdDatagramSetTable[0]->protocolData) ));
		return BMD_OK;
	}
	
	BMD_FOK(bmd_db_connect(_GLOBAL_bmd_configuration->db_conninfo, &hDB));
	retVal = _GLOBAL_bmd_configuration->clear_plugin_conf.bmd_start_lobs_remover(hDB, &deletedCounter);
	bmd_db_disconnect(&hDB);
	
	(*responseSet)->bmdDatagramSetTable[0]->datagramStatus = retVal;
	
	if(asprintf(&deletedCounterString, "%li", deletedCounter) == -1)
	{
		BMD_FOK(BMD_ERR_MEMORY);
	}
	BMD_FOK(set_gen_buf2(deletedCounterString, strlen(deletedCounterString), &((*responseSet)->bmdDatagramSetTable[0]->protocolData) ));
	free(deletedCounterString); deletedCounterString = NULL;
	
	BMD_FOK(retVal);
	
	return BMD_OK;
}
