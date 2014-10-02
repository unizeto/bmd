#define _WINSOCK2API_
#include<bmd/libbmdsql/proofs.h>
#include<bmd/libbmdsql/select.h>
#include<bmd/common/bmd-oids.h>
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include<bmd/libbmdglobals/libbmdglobals.h>

#include <assert.h>
#include <sys/types.h>
#ifndef WIN32
	#include <sys/wait.h>
	#include <sys/ipc.h>
	#include <semaphore.h>
	#include <sys/mman.h>
#endif
#include <errno.h>
#include <signal.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif


/**
Funkcja LockPrfTimestampsRow() naklada lock (typu exclusive) na wskazany wiewsz w tabeli prf_timestamps.
Funkcja musi byc wywolywana wewnatrz transakcji - w przeciwnym razie zalozenie lock nie jest mozliwe.
Lock jest zdejmowany automatycznie po zakonczeniu transakcji (commit) badz wykonaniu rollback.

@param db_connection_handle to handler polaczenia do bazy danych
@param TimestampId to identyfikator wiersza, ktory ma zostac zablokowany

@retval BMD_OK jesli zalozona blokada
@retval <0 jesli wystapil blad
*/
long LockPrfTimestampsRow(	void *db_connection_handle,
				char *TimestampId	)
{
char *SQLQuery		= NULL;
char *tmpString		= NULL;

	if (db_connection_handle==NULL)		{ BMD_FOK(BMD_ERR_PARAM1); }
	if (TimestampId==NULL)		{ BMD_FOK(BMD_ERR_PARAM2); }

	asprintf(&SQLQuery, "SELECT id FROM prf_timestamps WHERE id=%s FOR UPDATE OF prf_timestamps;", TimestampId);
	if (SQLQuery==NULL)
	{
		BMD_FOK(NO_MEMORY);
	}
	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(db_connection_handle, SQLQuery, &tmpString));
	free(SQLQuery);
	free(tmpString);

	return BMD_OK;
}



/* Ta funkcja podpieta bedzie do INPUT FILTERA. Kazdy nowy dokument ma byc timestampowany!!! */
// long prfTimestampGenBuf(	void *hDB,
// 				GenBuf_t *timestamped_data,
// 				GenBuf_t **timestamp_genbuf,char *ts_server,long ts_port,
// 				server_request_data_t *req)
// {
// 	PRINT_INFO("LIBBMDSQLINF Timestamping data.\n");
// 	if (hDB == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (timestamped_data == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (*timestamp_genbuf != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if ( ts_server == NULL )		{	BMD_FOK(BMD_ERR_PARAM4);	}
//
// 	PRINT_INFO("LIBBMDSQLINF Timestamping data with %s:%li\n",ts_server,ts_port);
//
// 	BMD_FOK(TimestampGenBuf(timestamped_data, timestamp_genbuf,ts_server, ts_port));
//
// 	return BMD_OK;
// }

long prfMaintainAllTimestamps(	void *hDB,
				long years_before_expiration,
				long months_before_expiration,
				long days_before_expiration,
				long hours_before_expiration,
				long minutes_before_expiration,
				long seconds_before_expiration,
				TimestampType_t TimestampType,

				ConservationConfig_t *twf_ConservationConfig,

				struct GenBufList *list,
				const char* timestamp_metadata_oid_str)
{
long err					= 0;
long twl_max_conservation_rounds_count		= 0;
long twl_iteration_count			= 0;
long fetched_counter1 				= 0;
long i						= 0;
char *lo_oid					= NULL;
char *fk_crypto_objects				= NULL;
char *fk_objects				= NULL;
char *timestamp_type				= NULL;
char *timestamp_id      			= NULL;
char *new_timestamp_id				= NULL;
void *query_result				= NULL;
char *tmp					= NULL;
GenBuf_t *plainDataGenBuf			= NULL;
GenBuf_t *timestampedDataGenBuf			= NULL;
char *objects_ids				= NULL;
char *timestamp_certValidTo			= NULL;


/*na potrzeby synchronizacji*/
long retVal					= 0;
void *selectConservedResult			= NULL;
char *selectConservedCommand			= NULL;
char *conservedValue				= NULL;
long fetchParameter				= 0;
long int timestamps_count			= 0;

GenBuf_t *ts_buf				= NULL;
GenBuf_t *tmpGenBuf				= NULL;

	PRINT_INFO("LIBBMDSQLINF Maintaining all timestamps\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (years_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (months_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (days_before_expiration < 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (hours_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (minutes_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (seconds_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM7);	}


	if(twf_ConservationConfig == NULL)					{	BMD_FOK(BMD_ERR_PARAM11);	}

	if(twf_ConservationConfig->round_size < 0)				{	BMD_FOK(BMD_ERR_PARAM11);	}
	if(twf_ConservationConfig->check_timestamp_validity < 0 ||
	twf_ConservationConfig->check_timestamp_validity > 1)			{	BMD_FOK(BMD_ERR_PARAM11);	}
	if(twf_ConservationConfig->round_sleep < 0)				{	BMD_FOK(BMD_ERR_PARAM11);	}
	if(twf_ConservationConfig->max_conservation_rounds_count < 0)		{	BMD_FOK(BMD_ERR_PARAM11);	}


	PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation round size: %li\n", twf_ConservationConfig->round_size);
	PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation check timestamp validity: %li\n", twf_ConservationConfig->check_timestamp_validity);
	PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation round sleep: %li\n", twf_ConservationConfig->round_sleep);
	PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation max conservation rounds count: %li\n", twf_ConservationConfig->max_conservation_rounds_count);


	if (list!=NULL)
	{
		asprintf(&objects_ids,"SELECT fk_crypto_objects, fk_objects FROM prf_timestamps WHERE (fk_crypto_objects=%s",list->gbufs[0]->buf );
		if (objects_ids==NULL)	{	BMD_FOK(NO_MEMORY);	}

		for (i=1; i<list->size; i++)
		{
			if (strcmp((char *)list->gbufs[i]->buf,"0")!=0)
			{
				free0(tmp); tmp = NULL;
				asprintf(&tmp, "%s", objects_ids);
				if(tmp == NULL)			{		BMD_FOK(NO_MEMORY);		}
				free0(objects_ids); objects_ids = NULL;
				asprintf(&objects_ids,"%s OR fk_crypto_objects=%s",tmp,list->gbufs[i]->buf);
				if(objects_ids == NULL)		{		BMD_FOK(NO_MEMORY);		}

			}

			free0(tmp); tmp = NULL;
			asprintf(&tmp, "%s", objects_ids);
			if(tmp == NULL)				{		BMD_FOK(NO_MEMORY);		}
			free0(objects_ids); objects_ids = NULL;
			asprintf(&objects_ids,"%s)%s%s",tmp,_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
			_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
			if(objects_ids == NULL)			{		BMD_FOK(NO_MEMORY);		}
		}


	}

	/************************************************/
	/* wybranie rekordów do odświeżenia timestamp'a */
	/************************************************/
	/*BMD_FOK(prfSelectTimestampsForConservation(	hDB,
							years_before_expiration,	months_before_expiration,
							days_before_expiration,		hours_before_expiration,
							minutes_before_expiration,	seconds_before_expiration,
							&query_result,
							objects_ids,
							timestamp_metadata_oid_str));*/

	/***********************************************/
	/* ponowne oznakowanie czasem wybranych danych */
	/***********************************************/
	fetched_counter1 = 0;
	while(err>=0)
	{
		PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation max_conservation_rounds_count: %li twl_max_conservation_rounds_count: %li\n",
		twf_ConservationConfig->max_conservation_rounds_count, twl_max_conservation_rounds_count);


		if (_GLOBAL_interrupt==1)
		{
			break;
		}
		

		/********************************************************/
		/*      wybranie rekordów do odświeżenia timestamp'a    */
		/********************************************************/
		//BMD_FOK(prfSelectTimestampsForConservation(	hDB,	years_before_expiration,months_before_expiration,						days_before_expiration,hours_before_expiration,minutes_before_expiration,	seconds_before_expiration,	&query_result,	twf_ConservationConfig->round_size,	objects_ids,timestamp_metadata_oid_str));

		fetched_counter1 = 0;
		twl_iteration_count = 0;
		while(err >= 0)
		{/*while(err>=0) drugi*/
			
			free0(lo_oid);
			free0(timestamp_id);
			free0(fk_objects);
			free0(fk_crypto_objects);

			/************************************************/
			/*	pobierz raster i klucz glowny znacznikow	*/
			/*	oraz obcy do crypto_objects			*/
			/********************************************/
			/*ponizsza funkcja nie odwoluje sie do bazy tylko pracuje na wyniku z ostatniego zapytania (iterator zapytania)*/
			err = prfGetTimestampKeyData(hDB, &fetched_counter1, query_result,&lo_oid, &fk_crypto_objects, &fk_objects, &timestamp_id,
			&timestamp_type);
			free0(timestamp_type); /*nie jest wykorzystane*/
			if (err==BMD_ERR_NODATA)
			{
				if(timestamps_count == 0)
	                        {
                	                PRINT_NOTICE("LIBBMDSQLNOT No timestamps for conservation\n");
				}

				PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation iteration count: %li timestamps_count: %li\n",
				twl_iteration_count, timestamps_count);
				if( (twl_iteration_count < twf_ConservationConfig->round_size && twf_ConservationConfig->round_size > 0)
				|| twl_iteration_count == 0)
				{
					//err = NO_SQL_DATA_RETURNED;
				}
				else
				{
					err = BMD_OK;
				}

				break;	
			}
			BMD_FOK(err);

			/*to wprowadzone przez TWo w celu sprawdzenia serwera znacznika czasu*/
			if(twf_ConservationConfig->check_timestamp_validity == 1 && timestamps_count== 0 &&
			_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin != NULL)
			{
				BMD_FOK(set_gen_buf2("test", (long)strlen("test"), &tmpGenBuf));


				BMD_FOK(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( tmpGenBuf, TimestampType,
				PLAIN_DATA, &ts_buf));	


				BMD_FOK(prfParseTimestampCartValidTo(ts_buf, &timestamp_certValidTo, twf_ConservationConfig));
				PRINT_VDEBUG("LIBBMDSQLVDEBUG Timestamp certificate valid to: %s\n", timestamp_certValidTo);

				free_gen_buf(&tmpGenBuf);
				free_gen_buf(&ts_buf);
				free(timestamp_certValidTo);
				timestamp_certValidTo = NULL;
			/*TODO*/
			}

			/***********************************
			Synchronizacja procesow - znacznik zakonserwowany w miedzyczasie przez inny proces
			nie moze byc juz ponownie konserwowany (zaburzenie lancucha konserwacji)
			***********************************/

			if(timestamp_id == NULL)
			{
				bmd_db_result_free(&query_result);
				free0(lo_oid);
				free0(fk_crypto_objects);
				free0(fk_objects);
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
			if(selectConservedCommand == NULL)		{		BMD_FOK(NO_MEMORY);		}
			retVal=bmd_db_execute_sql(hDB, selectConservedCommand, NULL, NULL, &selectConservedResult);
			free0(selectConservedCommand);
			if(retVal < 0)
			{
				bmd_db_result_free(&query_result);
				free0(lo_oid);
				free0(fk_crypto_objects);
				free0(fk_objects);
				free0(timestamp_id);
				BMD_FOK_TRANS(retVal);
			}

			fetchParameter=0;
			retVal=bmd_db_result_get_value(hDB, selectConservedResult, 0, 0, &conservedValue,FETCH_ABSOLUTE, &fetchParameter);
			bmd_db_result_free(&selectConservedResult);
			if( retVal<0 || conservedValue==NULL )
			{
				bmd_db_result_free(&query_result);
				free0(lo_oid);
				free0(fk_crypto_objects);
				free0(fk_objects);
				free0(timestamp_id);
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
			free(lo_oid); lo_oid=NULL;

			/******************************/
			/* ponowne oznakowanie czasem */
			/******************************/
			if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
			{
				_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( plainDataGenBuf, TimestampType, PLAIN_DATA,
				&timestampedDataGenBuf);
			}

			if (timestampedDataGenBuf!=NULL)
			{
				/*************************************/
				/* umieszczenie oznakowanego w bazie */
				/*************************************/
				BMD_FOK_TRANS(prfParseAndInsertTimestamp(	hDB,
								fk_crypto_objects,
								fk_objects,
								INSERT_BMD,
								timestampedDataGenBuf,
								timestamp_metadata_oid_str,
								&new_timestamp_id,
								-1,
								twf_ConservationConfig));

				/***********************************************/
				/* oznaczenie timestampa jako zakonserwowanego */
				/***********************************************/
				BMD_FOK_TRANS(prfUpdateConservedTimestampStatus(hDB, timestamp_id, new_timestamp_id));
				free0(new_timestamp_id);
			}

			/***********************
        	        (synchronizacja) zakonczenie transakcji
                	***********************/
			BMD_FOK_TRANS(bmd_db_end_transaction(hDB));

			free_gen_buf(&plainDataGenBuf);
			free_gen_buf(&timestampedDataGenBuf);
			timestamps_count++;
			twl_iteration_count++;
		}/*while(err>=0) drugi*/
	

		bmd_db_result_free(&query_result);

		twl_max_conservation_rounds_count++;
		if(twl_max_conservation_rounds_count >= twf_ConservationConfig->max_conservation_rounds_count &&
		twf_ConservationConfig->max_conservation_rounds_count > 0)
		{
			/*TODO*/
			break;
		}

		if(twf_ConservationConfig->round_sleep > 0 && err >= BMD_OK)
		{
			PRINT_VDEBUG("LIBBMDSQLVDEBUG Sleeping for %li seconds\n", twf_ConservationConfig->round_sleep);
#ifndef WIN32
			sleep(twf_ConservationConfig->round_sleep);
#else
			Sleep(1000 * twf_ConservationConfig->round_sleep);
#endif /*ifndef WIN32*/
		}
	}/*while(err>=0) pierwszy*/
	err = BMD_OK;

	BMD_FOK(prfTimestampClearConservationId(hDB));


	/****************************************************/
	/* przerwanie nastepuje, gdy process konserwacji    */
	/* dostaje sygnal o zakonczeniu dzialania aplikacji */
	/****************************************************/
	if (_GLOBAL_interrupt==1)
	{
		PRINT_NOTICE("LIBBMDSQLINF Timestamp conservation interrupted\n");
	}
	if (query_result!=NULL)
	{
		bmd_db_result_free(&query_result);
	}

	/************/
	/* porzadki */
	/************/
	free0(fk_crypto_objects);
	free0(fk_objects);
	free0(timestamp_id);
	free_gen_buf(&plainDataGenBuf);
	free_gen_buf(&timestampedDataGenBuf);
	free0(objects_ids);
	

	return BMD_OK;
}

long prfTimestampClearConservationId( void *hDB)
{
	void *query_result		= NULL;
	char *SQLQuery			= NULL;
	int err				= 0;

	if (hDB == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}


	asprintf(&SQLQuery, "UPDATE prf_timestamps SET conserved = 0 WHERE conserved = -1;");	
	if(SQLQuery == NULL)
	{
		BMD_FOK(NO_MEMORY);
	}
	
	
	err = bmd_db_execute_sql(hDB, SQLQuery, NULL, NULL, &query_result);
	PRINT_VDEBUG("LIBBMDSQLVDEBUG Execute update timestamps conservation id result: %i\n", err);
	if(err < 0 && err != BMD_ERR_NODATA)
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

/*long prfSelectTimestampsForConservation(	void *hDB,
						long years_before_expiration, long  months_before_expiration,
						long days_before_expiration, long   hours_before_expiration,
						long minutes_before_expiration, long seconds_before_expiration,
						void **query_result,

						long round_size,

						char *objects_ids,
						const char* timestamp_metadata_oid_str)
{
long err		= 0;
char *SQLQuery 		= NULL;
char *CurrentTimestamp	= NULL;

	PRINT_INFO("LIBBMDSQLINF Selecting timestamps for conservation\n");

	
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (years_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (months_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (days_before_expiration < 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (hours_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (minutes_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (seconds_before_expiration < 0)	{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (query_result == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (*query_result != NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}

	if(round_size < 0)			{	BMD_FOK(BMD_ERR_PARAM9);	}

	PRINT_VDEBUG("LIBBMDSQLVDEBUG Conservation timestamp select round size: %li\n", round_size);


	CurrentTimestamp=bmd_db_get_actualtime_sql(hDB);
	if (CurrentTimestamp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	if (objects_ids!=NULL)
	{
		asprintf(&SQLQuery, "SELECT timestamp, id, fk_crypto_objects, fk_objects, timestamp_type FROM prf_timestamps "
			"WHERE conserved=0 AND timestamp_type = \'%s\'"
			"AND timestampcertvalidto < %s::timestamp + '%li year %li month %li day %li hour %li minute %li second'::interval"
			"%s%s"
			"AND fk_crypto_objects IN (%s);",
			timestamp_metadata_oid_str, CurrentTimestamp,
			years_before_expiration, months_before_expiration, days_before_expiration,
			hours_before_expiration, minutes_before_expiration, seconds_before_expiration,
			_GLOBAL_bmd_configuration->location_id==NULL?"":"AND location_id=",
			_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
			objects_ids);
	}
	else
	{
		if(round_size > 0)
		{
			asprintf(&SQLQuery, "SELECT timestamp, id, fk_crypto_objects, fk_objects, timestamp_type FROM prf_timestamps "
				"WHERE conserved=0 AND timestamp_type = \'%s\'"
				"AND timestampcertvalidto < %s::timestamp + '%li year %li month %li day %li hour %li minute %li second'::interval %s%s LIMIT %li;",
				timestamp_metadata_oid_str, CurrentTimestamp,
				years_before_expiration, months_before_expiration, days_before_expiration,
				hours_before_expiration, minutes_before_expiration, seconds_before_expiration, 
				 _GLOBAL_bmd_configuration->location_id==NULL?"":"AND location_id=",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
				round_size);
		}
		else
		{
			asprintf(&SQLQuery, "SELECT timestamp, id, fk_crypto_objects, fk_objects, timestamp_type FROM prf_timestamps "
				"WHERE conserved=0 AND timestamp_type = \'%s\'"
				"AND timestampcertvalidto < %s::timestamp + '%li year %li month %li day %li hour %li minute %li second'::interval"
				"%s%s;",
				timestamp_metadata_oid_str, CurrentTimestamp,
				years_before_expiration, months_before_expiration, days_before_expiration,
				hours_before_expiration, minutes_before_expiration, seconds_before_expiration,
				_GLOBAL_bmd_configuration->location_id==NULL?"":"AND location_id=",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		}
	}


	if (SQLQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}

	free0(CurrentTimestamp);

	err = bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, query_result);
	if (err<0)
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}

	free0(SQLQuery);

	return BMD_OK;
}*/

long prfGetTimestampKeyData(	void *hDB,
				long *fetched_counter,
				void *query_result,
				char **lo_oid,
				char **fk_crypto_objects,
				char **fk_objects,
				char **timestamp_id,
				char **timestamp_type)
{
long err			= 0;
db_row_strings_t *rowstruct	= NULL;

	PRINT_INFO("LIBBMDSQLINF Getting timestamp keys' data\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (fetched_counter == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*fetched_counter < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (query_result == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (lo_oid == NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*lo_oid != NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (fk_crypto_objects == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (*fk_crypto_objects != NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (fk_objects == NULL)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (*fk_objects != NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (timestamp_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (*timestamp_id != NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}

	err = bmd_db_result_get_row(hDB, query_result, 0, 0, &rowstruct, FETCH_NEXT,fetched_counter);
	if (err==BMD_ERR_NODATA)
	{
		bmd_db_row_struct_free(&rowstruct);
		/*Tu nie moze byc BMD_FOK. To jest sytuacja gdy nie ma znacznikow czasu*/
		return err;
	}
	BMD_FOK(err);

	asprintf(lo_oid, "%s",rowstruct->colvals[0]);
	asprintf(timestamp_id, "%s",rowstruct->colvals[1]);
	asprintf(fk_crypto_objects, "%s",rowstruct->colvals[2]);
	asprintf(fk_objects, "%s",rowstruct->colvals[3]);
	asprintf(timestamp_type, "%s",rowstruct->colvals[4]);
	bmd_db_row_struct_free(&rowstruct);

	return BMD_OK;
}

long prfImportTimestampToGenBuf(	void *hDB,
					char *lo_oid,
					GenBuf_t **plainDataGenBuf)
{
	PRINT_INFO("LIBBMDSQLINF Importing timestamp to genbuf\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (lo_oid==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (plainDataGenBuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*plainDataGenBuf != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(bmd_db_import_blob(hDB, lo_oid, plainDataGenBuf));

	return BMD_OK;
}

long prfUpdateConservedTimestampStatus(	void *hDB,
					char *timestamp_id,
					char *new_timestamp_id)
{
void *query_result	= NULL;
char *SQLQuery 		= NULL;

	PRINT_INFO("LIBBMDSQLINF Updating conserved timestamp status\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_id == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (new_timestamp_id == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "UPDATE prf_timestamps SET conserved=%s WHERE id=%s ;", new_timestamp_id, timestamp_id);
	if (SQLQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &query_result));

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	return BMD_OK;
}


long prfParseAndInsertTimestamp(	void *hDB,
					char *crypto_objects_id,
					char *objects_id,
					const WhoInsertIt_t timestamp_user_or_system,
					GenBuf_t *timestamp_genbuf,
					const char* timestamp_metadata_oid_str,
					char **new_timestamp_id,
					long twf_conserved,
					ConservationConfig_t *twf_ConservationConfig)
{
long timestamp_version		= 0;
long err			= 0;
char *timestamp_policy_oid	= NULL;
char *timestamp_serialNumber	= NULL;
char *timestamp_genTime		= NULL;
char *timestamp_certValidTo	= NULL;
char *timestamp_tsaDN		= NULL;
char *timestamp_tsaSN		= NULL;
PKIStandardEnum_t timestamp_standard = STANDARD_UNKNOWN;

	PRINT_INFO("LIBBMDSQLINF Parsing and inserting timestamp\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (objects_id==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (timestamp_user_or_system < 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (new_timestamp_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if ((*new_timestamp_id) != NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM9);	}

	/****************************************/
	/*	operacje na znaczniku czasu	*/
	/****************************************/
	BMD_FOK(prfParseTimestampVersion(timestamp_genbuf, &timestamp_version, twf_ConservationConfig));
	BMD_FOK(prfParseTimestampPolicyOID(timestamp_genbuf, &timestamp_policy_oid, twf_ConservationConfig));
	BMD_FOK(prfParseTimestampSN(timestamp_genbuf, &timestamp_serialNumber, twf_ConservationConfig));
	prfParseTimestampTsaSN(timestamp_genbuf, &timestamp_tsaSN, twf_ConservationConfig);
	err =prfParseTimestampTsaDN(timestamp_genbuf, &timestamp_tsaDN, twf_ConservationConfig);
	if(err == BMD_TIMESTAMP_NO_TSA_FIELD)
	{
		asprintf(&timestamp_tsaDN," ");
	}
	else
	{
		BMD_FOK(err);
	}
	BMD_FOK(prfParseTimestampGenTime(timestamp_genbuf, &timestamp_genTime, twf_ConservationConfig));
	prfParseTimestampCartValidTo(timestamp_genbuf, &timestamp_certValidTo, twf_ConservationConfig);
	BMD_FOK(prfParseTimestampStandard(timestamp_genbuf, &timestamp_standard));

	/*********************************************/
	/* wstawianie znacznika czasu do bazy danych */
	/*********************************************/
	BMD_FOK(prfInsertTimestamp(	hDB,
					crypto_objects_id,
					objects_id,
					timestamp_version,
					timestamp_policy_oid,
					timestamp_serialNumber,
					timestamp_genTime,
					timestamp_certValidTo,
					timestamp_tsaDN,
					timestamp_tsaSN,
					timestamp_standard,
					timestamp_user_or_system,
					timestamp_genbuf,
					timestamp_metadata_oid_str,
					new_timestamp_id,
					twf_conserved));

	/************/
	/* porzadki */
	/************/
	free0(timestamp_policy_oid);
	free0(timestamp_serialNumber);
	free0(timestamp_genTime);
	free0(timestamp_certValidTo);
	free0(timestamp_tsaDN);
	free0(timestamp_tsaSN);

	return BMD_OK;
}

long prfParseTimestampVersion(	GenBuf_t *timestamp_genbuf,
				long *timestamp_version,
				ConservationConfig_t *twf_ConservationConfig)
{
char *AttributeString = NULL;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp's version\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_version == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_version < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_VERSION, &AttributeString, timestamp_version, twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;

}

long prfParseTimestampPolicyOID(	GenBuf_t *timestamp_genbuf,
					char **timestamp_policy_oid,
					ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong = 0;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp's policy oid\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_policy_oid == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_policy_oid != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_POLICY_OID, timestamp_policy_oid, &AttributeLong, twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;
}

long prfParseTimestampSN(	GenBuf_t *timestamp_genbuf,
				char **timestamp_SN,
				ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong = 0;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp's serial number\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_SN == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_SN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_SN, timestamp_SN, &AttributeLong, twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;
}

long prfParseTimestampTsaSN(	GenBuf_t *timestamp_genbuf,
				char **timestamp_tsa_SN,
				ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong = 0;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp's tsa serial number\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_tsa_SN == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_tsa_SN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_TSA_CERT_SN, timestamp_tsa_SN, &AttributeLong, twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;
}

long prfParseTimestampTsaDN(	GenBuf_t *timestamp_genbuf,
				char **timestamp_tsa_DN,
				ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong	= 0;
long status		= 0;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp's tsa dn\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_tsa_DN == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_tsa_DN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	status = TSGetAttr(timestamp_genbuf, TS_ATTR_TSA_DN, timestamp_tsa_DN, &AttributeLong, twf_ConservationConfig->no_tsa_cert_number);
	if(status == BMD_TIMESTAMP_NO_TSA_FIELD)
	{
		return BMD_TIMESTAMP_NO_TSA_FIELD;
	}
	BMD_FOK(status);

	return BMD_OK;
}

long prfParseTimestampGenTime(	GenBuf_t *timestamp_genbuf,
				char **timestamp_genTime,
				ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong	= 0;

	PRINT_INFO("LIBBMDSQLINF Parsing generate time from timestamp \n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genTime==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_genTime != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_GENTIME, timestamp_genTime, &AttributeLong, twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;
}

long prfParseTimestampCartValidTo(	GenBuf_t *timestamp_genbuf,
					char **timestamp_certValidTo,
					ConservationConfig_t *twf_ConservationConfig)
{
long AttributeLong = 0;

	PRINT_INFO("LIBBMDSQLINF Parsing certificate validity time from timestamp \n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_certValidTo == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_certValidTo != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_ConservationConfig == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(TSGetAttr(timestamp_genbuf, TS_ATTR_TSA_CERT_VALID_NA, timestamp_certValidTo, &AttributeLong,
	twf_ConservationConfig->no_tsa_cert_number));

	return BMD_OK;
}

long prfParseTimestampStandard(	GenBuf_t *timestamp_genbuf,
						PKIStandardEnum_t *timestamp_standard)
{
	PRINT_INFO("LIBBMDSQLINF Parsing timestamp standard\n");
	/* TODO */
	/****************************************************************************************/
	/* TU UMIESCIC FUNKCJE PARSUJACE STANDARD FORMATU ZNACZNIKA, BLEDY Z UJEMNYMI KODAMI	*/
	/****************************************************************************************/


	/************************/
	/* walidacja parametrow */
	/************************/
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (timestamp_standard == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*timestamp_standard < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	*timestamp_standard = STANDARD_CMS;
/*	*timestamp_standard = STANDARD_PKCS7;
	*timestamp_standard = STANDARD_XADES;*/

	return BMD_OK;
}


long prfInsertTimestamp(	void *hDB,
				char *crypto_objects_id,
				char *objects_id,
				const long timestamp_version,
				const char *timestamp_policy_oid,
				const char *timestamp_serialNumber,
				const char *timestamp_genTime,
				const char *timestamp_certValidTo,
				const char *timestamp_tsaDN,
				const char *timestamp_tsaSN,
				const PKIStandardEnum_t timestamp_standard,
				const WhoInsertIt_t timestamp_userorsystem,
				GenBuf_t *timestamp_genbuf,
                    		const char* timestamp_metadata_oid_str,
				char **new_timestamp_id,
				long twf_conserved)
{
char *timestamp_lobjId	= NULL;

	PRINT_INFO("LIBBMDSQLINF Parsing timestamp standard\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (timestamp_version < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (timestamp_policy_oid == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (timestamp_serialNumber == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (timestamp_genTime == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (timestamp_tsaDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (timestamp_standard < 0)		{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (timestamp_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM11);	}
	if (timestamp_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM12);	}
	if (timestamp_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM12);	}
	if (timestamp_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM12);	}
	if (new_timestamp_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM13);	}
	if ((*new_timestamp_id) != NULL)	{	BMD_FOK(BMD_ERR_PARAM13);	}

	BMD_FOK(bmd_db_export_blob(hDB, timestamp_genbuf, &timestamp_lobjId));

	BMD_FOK(prfInsertIntoTimestampsTable(	hDB,
						crypto_objects_id,
						objects_id,
						timestamp_version,
						timestamp_policy_oid,
						timestamp_serialNumber,
						timestamp_genTime,
						timestamp_certValidTo,
						timestamp_tsaDN,
						timestamp_tsaSN,
						timestamp_standard,
						timestamp_userorsystem,
						timestamp_lobjId,
						timestamp_metadata_oid_str,
						new_timestamp_id,
						twf_conserved));

	/************/
	/* porzadki */
	/************/
	free0(timestamp_lobjId);

	return BMD_OK;
}


long prfInsertIntoTimestampsTable(	void *hDB,
					char *crypto_objects_id,
					char *objects_id,
					const long timestamp_version,
					const char *timestamp_policy_oid,
					const char *timestamp_serialNumber,
					const char *timestamp_genTime,
					const char *timestamp_certValidTo,
					const char *timestamp_tsaDN,
					const char *timestamp_tsaSN,
					const PKIStandardEnum_t timestamp_standard,
					const WhoInsertIt_t timestamp_userorsystem,
					char *timestamp_lobjId,
			                const char* timestamp_metadata_oid_str,
					char **new_timestamp_id,
					long twf_conserved)
{
long err				= 0;
char *SQLQuery				= NULL;
char *SequenceNextVal			= NULL;
void *query_result			= NULL;
char *dbcomp_timestamp_genTime		= NULL;
char *dbcomp_timestamp_certValidTo	= NULL;
long fetched_counter			= 0;

	PRINT_INFO("LIBBMDSQLINF Inserting timestamp info to database\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (timestamp_version <= 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (timestamp_policy_oid == 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (timestamp_serialNumber == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (timestamp_genTime == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (timestamp_tsaDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (timestamp_standard < 0)		{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (timestamp_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM11);	}
	if (timestamp_lobjId == NULL)		{	BMD_FOK(BMD_ERR_PARAM12);	}
	if (new_timestamp_id == NULL)		{	BMD_FOK(BMD_ERR_PARAM13);	}
	if ((*new_timestamp_id) != NULL)	{	BMD_FOK(BMD_ERR_PARAM13);	}

	/**************************************************/
	/* pobranie kolejnego wolnego id tabeli timestamp */
	/**************************************************/
	if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
	{
		SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "prf_timestamps_id_seq");
		if (SequenceNextVal==NULL)	{	BMD_FOK(NO_MEMORY);	}

		asprintf(&SQLQuery,"SELECT %s FROM prf_timestamps_id_seq;",SequenceNextVal);
		if (SQLQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql(hDB,SQLQuery,NULL,NULL,&query_result));
		BMD_FOK(bmd_db_result_get_value(hDB, query_result, 0, 0,new_timestamp_id, FETCH_NEXT, &fetched_counter));
	}
	else
	{
		if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
		{
			void *hRes=NULL;
			long sf=0;
			bmd_db_execute_sql(hDB,"SELECT prf_timestamps_id_seq.nextval FROM DUAL",NULL,NULL,&hRes);
			bmd_db_result_get_value(hDB,hRes,0,0,new_timestamp_id,FETCH_NEXT,&sf);
			bmd_db_result_free(&hRes);
		}
		else
		{
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
		}
	}
	if (new_timestamp_id==NULL)
	{
		BMD_FOK(-12345678);
	}

	bmd_db_result_free(&query_result);

	free(SQLQuery); SQLQuery=NULL;

	/*****************************************/
	/* ustawienie odpowiedniego formatu daty */
	/*****************************************/
	BMD_FOK(bmd_db_date_to_timestamp(timestamp_genTime, &dbcomp_timestamp_genTime));
	BMD_FOK(bmd_db_date_to_timestamp(timestamp_certValidTo, &dbcomp_timestamp_certValidTo));

	/****************************************************/
	/* wstawienie nowej pozycja w tabeli prf_timestamps */
	/****************************************************/
	asprintf(&SQLQuery,"INSERT INTO prf_timestamps (id%s, fk_crypto_objects, fk_objects, version, policyoid,serialNumber,genTime, timestampCertValidTo,tsaDN, tsaSN,standard, userorsystem, conserved, timestamp, timestamp_type)"
				"VALUES ( %s%s%s, %s, %s, %li, \'%s\', "
				"\'%s\', "
				"%s, %s, "
				"\'%s\', \'%s\', "
				"%i, %i, %li, %s, \'%s\');",
				_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
				(*new_timestamp_id),
				_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
				crypto_objects_id,
				objects_id,
				timestamp_version,
				timestamp_policy_oid,
				timestamp_serialNumber,
				dbcomp_timestamp_genTime,
				dbcomp_timestamp_certValidTo!=NULL?dbcomp_timestamp_certValidTo:"'2000-01-01 00:00:00+01'",
				timestamp_tsaDN,
				timestamp_tsaSN!=NULL?timestamp_tsaSN:"<Not present>",
				timestamp_standard,
				timestamp_userorsystem,
				twf_conserved,
				timestamp_lobjId,
				timestamp_metadata_oid_str);

	free(SequenceNextVal); SequenceNextVal=NULL;
	free(dbcomp_timestamp_genTime); dbcomp_timestamp_genTime=NULL;
	free(dbcomp_timestamp_certValidTo); dbcomp_timestamp_certValidTo=NULL;
	if (SQLQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}

	err = bmd_db_execute_sql(	hDB, SQLQuery,NULL,NULL, &query_result);
	if (err<BMD_OK)
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}

	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}


/**
*Zapis danych o nowym poswiadczeniu podpisu elektronicznego w bazie danych
*
*@param[in] void *hDB		- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] long signature_id	- identyfikator wewnetrzny wstawionego juz wczesniej podpisu, do ktorego odwoluje sie poswiadczenie
*@param[in] GenBuf_t *dvcs_genbuf		- genbuf zawierajacy poswiadczenie zwrocone przez dvcs
*@param[in] char *verify_status			- lancuch znakowy z zaznaczeniem, czy weryfikacja podpisu powiodla sie i kiedy nastapila proba weryfikacji
*
*@retval BMD_OK 			- zakończenie pomyślne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval NO_MEMORY			- blad zasobow pamieci
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawna wartość identyfikatora wewnętrznego podpisu
*@retval BMD_ERR_PARAM3			- niepoprawna wartość identyfikatora wewnętrznego loba poswiadczenia
*@retval BMD_ERR_PARAM4			- pusty łańcuch znakowy, w którym powinien znalezc sie statusweryfikacji
*/
long prfInsertDVCS(	void *hDB,
			char *cryptoObjectId,
			char *signature_id,
			GenBuf_t *dvcs_genbuf,
			char *verify_status,
			bmd_crypt_ctx_t *sym_ctx,
			char **dvcs_id)
{
char *dvcs_lobjId		= NULL;
GenBuf_t *dvcs_genbuf_enc	= NULL;
char *verify_msg		= NULL;

	PRINT_INFO("LIBBMDSQLINF Inserting DVCS to database\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (cryptoObjectId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (dvcs_genbuf == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (dvcs_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (dvcs_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (verify_status==NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	/**********************************/
	/* szyfrowanie poswiadczenia dvcs */
	/**********************************/
	BMD_FOK(bmdpki_symmetric_encrypt(sym_ctx, dvcs_genbuf, BMDPKI_SYM_LAST_CHUNK, &dvcs_genbuf_enc));

	/**************************************************************************/
	/* wyeksportowanie genbufa z poswiadczeniem do bazy dabych w postaci loba */
	/**************************************************************************/
	BMD_FOK(bmd_db_export_blob(hDB, dvcs_genbuf_enc, &dvcs_lobjId));

	/*******************************************************/
	/* zapisanie informacji o poswiadczeniu w bazie danych */
	/*******************************************************/
	asprintf(&verify_msg, "%s", verify_status);
	BMD_FOK(prfInsertIntoDVCSTable(	hDB, cryptoObjectId, signature_id, dvcs_lobjId, verify_msg, dvcs_id));

	/************/
	/* porzadki */
	/************/
	free0(dvcs_lobjId);
	free_GenBuf(&dvcs_genbuf_enc);

	return BMD_OK;
}


/**
*Zapis danych o nowym poswiadczeniu podpisu elektronicznego w tabeli prf_dvcs
*
*@param[in] void *hDB		- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] long signature_id	- identyfikator wewnetrzny wstawionego juz wczesniej podpisu, do ktorego odwoluje sie poswiadczenie
*@param[in] long dvcs_lobjId	- identyfikator wewnetrzny loba - ciala podpisu umieszczinego w bazie danych
*@param[in] char *verify_status			- lancuch znakowy z zaznaczeniem, czy weryfikacja podpisu powiodla sie i kiedy nastapila proba weryfikacji
*
*@retval BMD_OK 			- zakończenie pomyślne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval NO_MEMORY			- blad zasobow pamieci
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawna wartość identyfikatora wewnętrznego podpisu
*@retval BMD_ERR_PARAM3			- niepoprawna wartość identyfikatora wewnętrznego loba poswiadczenia
*/
long prfInsertIntoDVCSTable(	void *hDB,
				char *cryptoObjectId,
				char *signature_id,
				char *dvcs_lobjId,
                       		char *verify_status,
				char **dvcs_id)

{
long fetched_counter	= 0;
long rows		= 0;
long cols		= 0;
long status		= 0;
char *SQLQuery		= NULL;
char *SequenceNextVal	= NULL;
char *ans		= NULL;
void *query_result	= NULL;

	PRINT_INFO("LIBBMDSQLINF Inserting dvcs info to database\n");
	/************************************/
	/* walidacja parametrów wejściowych */
	/************************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (cryptoObjectId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_id ==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (dvcs_lobjId == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	/**************************************************/
	/* pobranie kolejnego wolnego id tabeli timestamp */
	/**************************************************/
	/* MS_ORACLE - TODO SEKWENCJA */
	SequenceNextVal=bmd_db_get_sequence_nextval(	hDB, "prf_dvcs_id_seq");

	asprintf(&SQLQuery,"SELECT %s FROM prf_dvcs_id_seq;",SequenceNextVal);

	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,&rows, &cols, &query_result));

	status=bmd_db_result_get_value(hDB, query_result, 0, 0, &ans, FETCH_NEXT, &fetched_counter);
	if (status<BMD_OK || ans==NULL)
	{
		return status;
	}
	asprintf(dvcs_id, "%s", ans);
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	/********************************************************/
	/*	wstawienie nowej pozycji w tabeli prf_dvcs	*/
	/********************************************************/
	asprintf(&SQLQuery,	"INSERT INTO prf_dvcs (id%s, fk_crypto_objects, fk_prf_signatures, dvcs, verify_status, dvcs_type) VALUES ( %s%s%s, %s, %s, %s, '%s', '%s');",
					_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
					ans,
					_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
					cryptoObjectId,
					signature_id,
					dvcs_lobjId,
					verify_status,
					OID_PKI_METADATA_DVCS);
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &query_result));

	/************************/
	/*	porzadki	*/
	/************************/
	free0(SQLQuery);
	free0(SequenceNextVal);
	free0(ans);
	bmd_db_result_free(&query_result);

	return BMD_OK;
}

