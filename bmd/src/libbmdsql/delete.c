#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/update.h>

#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

/************************************************/
/*		SEND AVIZO FUNCTIONS		*/
/************************************************/
/**
*Obsluzenie zadania ponownej wysylki awizo na poziomie datagramsetu
*
*@param[in] void *hDB		- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] bmdDatagramSet_t *bmdJSRequestSet	- wskaznik na datagram zadania
*@param[in] bmdDatagramSet_t **bmdJSResponseSet	- wskaznik na datagram odpowiedzi
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval NO_MEMORY			- blad zasobow pamieci
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*@retval BMD_ERR_PARAM4			- niepoprawny wskaznik na GenBuf'a zawierajacego id grupy, do ktorej nalezy uzytkownik
*/
#ifndef WIN32
long JS_bmdDatagramSet_to_send_avizo(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
#else /*WIN32*/
long JS_bmdDatagramSet_to_send_avizo(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req,
					HANDLE *threadHandler)
#endif /*ifndef WIN32*/
{
long err		= 0;
long i			= 0;
long status		= 0;
char *hash		= NULL;
char *tmp_id		= NULL;


	PRINT_INFO("LIBBMDSQLINF Sending avizo (datagramset)\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	for(i=0; i < bmdJSRequestSet->bmdDatagramSetSize; i++, err=0)
	{
		free0(tmp_id);

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

		/************************************************************************/
		/*	sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania	*/
		/************************************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}

		// sparametryzowana wysylka awizo moze odbywac sie na podstawie bmdId albo na podstawie wartosci metadanych
		if(bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType == BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS)
		{
			if(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId == NULL) // zadanie wywolane z podaniem bmdId
			{
				status = VerifyPermissionsForCryptoObjectByMetadata(hDB, req,
					bmdJSRequestSet->bmdDatagramSetTable[i],
					CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
					NULL/*fileName*/,
					NULL/*fileType*/,
					&tmp_id/*pointingId*/,
					NULL/*correspondingId*/,
					NULL/*forGrant*/,
					NULL/*additionalMetadataCount*/,
					NULL/*additionalMetadataValues*/);
			}
			else // zadanie wywolane z podaniem metadanych opisujacych dokument (nie bmdId)
			{
				status = VerifyPermissionsForCryptoObject(hDB, req,
					bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId->buf,
					CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
					NULL/*fileName*/,
					NULL/*fileType*/,
					NULL/*pointingId*/,
					NULL/*correspondingId*/,
					NULL/*forGrant*/,
					NULL/*additionalMetadataCount*/,
					NULL/*additionalMetadataValues*/);

				tmp_id = strdup(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId->buf);
			}
		}
		else // dla innych typow zadan wysylka awizo moze odbyc sie tylko na podstawie bmdId
		{
			if(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId == NULL)
				{ BMD_FOK(BMD_ERR_PARAM2); }

			status = VerifyPermissionsForCryptoObject(hDB, req,
				bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				NULL/*fileType*/,
				NULL/*pointingId*/,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/);

			tmp_id = strdup(bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		}
		
		//status=CheckIfCryptoObjectExist( hDB, bmdJSRequestSet->bmdDatagramSetTable[i], 0, &tmp_id, NULL, &tmp_location, NULL, NULL, NULL, NULL, req);
		if (status<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR %s. Error=%li\n",GetErrorMsg(status), status);
			err=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}


// 		/************************************************************************/
// 		/*	przepisanie id pliku z datagramu requesta do datagramu responsa	*/
// 		/************************************************************************/
// 		asprintf(&tmp_id,"%s",(char *)bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileId->buf);
// 		if (tmp_id==NULL)
// 		{
// 			PRINT_ERROR("LIBBMDSQLERR Memory error. Error=%i\n",NO_MEMORY);
// 			err=NO_MEMORY;
// 			PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),
// 									bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100,
// 									err);
// 			continue;
// 		}
//
// 		/************************************************/
// 		/* przepisanie id lokalizacji pliku z datagramu */
// 		/************************************************/
// 		asprintf(&tmp_location,"%s",(char *)bmdJSRequestSet->bmdDatagramSetTable[i]->protocolDataFileLocationId->buf);
// 		if (tmp_location==NULL)
// 		{
// 			PRINT_ERROR("LIBBMDSQLERR Memory error. Error=%i\n",NO_MEMORY);
// 			err=NO_MEMORY;
// 			PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),
// 									bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100,
// 									err);
// 			continue;
// 		}
//

		status = set_gen_buf2( tmp_id, (long)strlen(tmp_id), &((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFileId));
		if (status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Memory error. Error=%i\n",NO_MEMORY);
			err=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}

		status = set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFileLocationId));
		if (status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Memory error. Error=%i\n",NO_MEMORY);
			err=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}

		/******************************************************/
		/*	pobranie hasha pliku na podstawienie id pliku	*/
		/******************************************************/
		status=GetHashFromId(hDB, tmp_id, _GLOBAL_bmd_configuration->location_id, &hash);
		if (status<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Error in getting recipient's e-mail\n");
			err=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}

		/******************************************/
		/*	dolaczenie hasha do datagaramu	*/
		/******************************************/
		status=bmd_add_hash_to_dtg_as_data(hash,&((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
		if (status<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Error adding hash to datagram. Error=%li\n",status);
			err=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			continue;
		}

		/************************************************/
		/*	ustawienie statusu bledu datagramu	*/
		/************************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus =(long ) err;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;

		/************************************************************/
		/*	pobranie metadanych pliku na podstawienie id pliku	*/
		/************************************************************/
		BMD_FOK(FillFromAllMetadata(	hDB,
						tmp_id,
						_GLOBAL_bmd_configuration->location_id,
						WITHOUT_PKI_METADATA,
						WITH_ADD_METADATA,
						WITH_SYS_METADATA,
						WITHOUT_ACTION_METADATA,
						(*bmdJSResponseSet)->bmdDatagramSetTable[i],req));

		// skopiowanie pamametrow wysylki awizo z zadania do odpowiedzi (bo wysylka odbywa sie na podstawie odpowiedzi)
		if(bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType == BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS)
		{
			BMD_FOK( CopySendAdviceParameters(bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i]) );
		}
	}

	/************************************************/
	/*	wysylka maili z awizo do klientow	*/
	/************************************************/
	if(_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post!=NULL)
	{

// 		if ((char *)_GLOBAL_bmd_configuration->additional_db_conninfo!=NULL)
// 		{
// #ifndef WIN32
//
// 			status=_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
// 																_GLOBAL_bmd_configuration->additional_db_conninfo,
// 																0,0,1,req);
// #else /*WIN32*/
//
// 			status=_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
// 																_GLOBAL_bmd_configuration->additional_db_conninfo,
// 																0,0,1,req, threadHandler);
// #endif /*ifndef WIN32*/
//
// 		}
// 		else
// 		{
#ifndef WIN32

			status=_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
																_GLOBAL_bmd_configuration->db_conninfo,
																0,0,1,req);
#else /*WIN32*/

			status=_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
																_GLOBAL_bmd_configuration->db_conninfo,
																0,0,1,req, threadHandler);

#endif /*ifndef WIN32*/
// 		}


		if (status<BMD_OK)
		{
			for(i=0; i<(long)bmdJSRequestSet->bmdDatagramSetSize; i++, err=0)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
			}

		}

		/* rejestracja odbywa sie na zewnatrz w js.c
		// zarejestrowanie akcji w bazie danych
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			for(i=0; i<(long)bmdJSRequestSet->bmdDatagramSetSize; i++)
			{
				status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
				if (status<BMD_OK)
				{
					(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
				}
			}
		}
		*/


	}
	else
	{
		BMD_FOK(PLUGIN_SESSION_NOT_RUNNING);
	}

	/************/
	/* porzadki */
	/************/
	free0(hash);
	free0(tmp_id);

	return BMD_OK;
}

/************************************/
/*		DELETE FUNCTIONS		*/
/************************************/
/**
*Obsluzenie zadania usuwania plikow z bazy danych na poziomie datagramsetu
*
*@param[in] void *hDB		- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] bmdDatagramSet_t *bmdJSRequestSet	- wskaznik na datagram zadania
*@param[in] bmdDatagramSet_t **bmdJSResponseSet	- wskaznik na datagram odpowiedzi
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval NO_MEMORY			- blad zasobow pamieci
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*@retval BMD_ERR_PARAM4			- niepoprawny wskaznik na GenBuf'a zawierajacego id grupy, do ktorej nalezy uzytkownik
*/
long JS_bmdDatagramSet_to_delete(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long i					= 0;
long status				= 0;

	PRINT_INFO("LIBBMDSQLINF Deleting file(s) (datagramset)\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/********************************************************/
	/*	obsluga poszczegolnych datagramow z zadaniami	*/
	/********************************************************/
	for(i=0; i<(int)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/************************************************************************/
		/*	sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania	*/
		/************************************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus);
			continue;
		}

		/************************************************/
		/*	obsluzenie pojedynczego datagramu	*/
		/************************************************/
		status=DeleteDataSlot(	hDB,
					bmdJSRequestSet->bmdDatagramSetTable[i],
					&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),
					req);

		/************************************************/
		/*	ustawienie statusu bledu datagramu	*/
		/************************************************/
/*
		PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, status);
*/

		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = \
		bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;


	}

	return BMD_OK;
}


/**
*Usuwanie wszystkich znacznikow czasu powiazanych z crypto objectem o podanym id
*
*@param[in] void *hDB	- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] char *CryptoObjectNr	- id poszukiwanego crypto object'a w postaci lancucha znakowego
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*/
long DeleteTimestamps(	void *hDB,
			char *CryptoObjectNr,
			char *location_id)
{
/*long rows				= 0;
long cols				= 0;*/
long status		= 0;
long i			= 0;
long fetched_counter	= 0;

char *SQLQuery		= NULL;
char *tmp		= NULL;
void *database_result	= NULL;

	PRINT_INFO("LIBBMDSQLINF Deleting file's timestamps\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*********************************************************************/
	/* pobieranie oida odpowiedniego loba przypisanego do crypto_objecta */
	/*********************************************************************/
	asprintf(&SQLQuery, "SELECT timestamp FROM prf_timestamps WHERE fk_crypto_objects=%s%s%s;",
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	/******************************************/
	/* wykonanie polecenie odszukania w bazie */
	/******************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));

	/*********************************************/
	/* przypisanie wynikow wyszukiwania do listy */
	/*********************************************/
	i=0;
	while(1)
	{
		status=bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp, FETCH_NEXT, &fetched_counter);
		if(status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
		{
			break;
		}

		/*********************************/
		/* usuwanie poszczegolnych lobow */
		/*********************************/
		status=bmd_db_delete_blob(hDB,tmp);
		if (status<BMD_OK)
		{
			PRINT_WARNING("LIBBDMSQLWRN Unable to erase timestamp's blob %s\n",tmp);
		}

		free0(tmp);
		i++;
	}

// 	/************************************************/
// 	/*	usuwanie informacji o znacznikach czasu	*/
// 	/************************************************/
// 	asprintf(&SQLQuery, "DELETE FROM prf_timestamps WHERE (fk_crypto_objects=%s AND timestamp_type = '1.2.616.1.113527.4.4.2' AND location_id=%s)"
// 								     "OR (fk_crypto_objects IN (SELECT id FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s) AND timestamp_type = '1.2.616.1.113527.4.4.8' AND location_id=%s)"
// 								     "OR (fk_crypto_objects IN (SELECT id FROM prf_dvcs WHERE fk_prf_signatures IN (SELECT id FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s) AND dvcs_type = '%s' AND location_id=%s) AND  timestamp_type = '1.2.616.1.113527.4.4.10' AND location_id=%s);", CryptoObjectNr, location_id, CryptoObjectNr, location_id, location_id, CryptoObjectNr, location_id, OID_PKI_METADATA_DVCS, location_id, location_id);
// 	#if 0
// 	asprintf(&SQLQuery, "DELETE FROM prf_timestamps WHERE (fk_crypto_objects=%s AND timestamp_type = '1.2.616.1.113527.4.4.2') OR (fk_crypto_objects=%s AND timestamp_type = '1.2.616.1.113527.4.4.8') OR (fk_crypto_objects IN (SELECT id FROM prf_dvcs WHERE fk_prf_signatures IN (SELECT id FROM prf_signatures WHERE fk_crypto_objects=%s)) AND  timestamp_type = '1.2.616.1.113527.4.4.10');", CryptoObjectNr, CryptoObjectNr, CryptoObjectNr);
// 	#endif
// 	/************************************************/
// 	/*	wykonanie polecenie odszukania w bazie	*/
// 	/************************************************/
// 	bmd_db_result_free(&database_result);
// 	status=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
// 	if(status<0)
// 	{
// 		PRINT_WARNING("LIBBMDSQLWRN Unable to delete timestamps' information\n");
// 	}
//
	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(SQLQuery);
	free0(tmp);

	return BMD_OK;
}


/**
*Usuwanie wszystkich poswiadczen podpisow powiazanych z crypto objectem o podanym id
*
*@param[in] void *hDB	- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] char *CryptoObjectNr	- id poszukiwanego crypto object'a w postaci lancucha znakowego
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*/
long DeleteDVCS(	void *hDB,
			char *CryptoObjectNr,
			char *location_id)
{
long status		= 0;
long i			= 0;
long fetched_counter	= 0;
char *SQLQuery		= NULL;
char *tmp		= NULL;
void *database_result	= NULL;

	PRINT_INFO("LIBBMDSQLINF Deleting file's signatures' DVCS\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*********************************************************************/
	/* pobieranie oida odpowiedniego loba przypisanego do crypto_objecta */
	/*********************************************************************/
	asprintf(&SQLQuery, "SELECT dvcs FROM prf_dvcs WHERE fk_crypto_objects=%s%s%s;",
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	/******************************************/
	/* wykonanie polecenie odszukania w bazie */
	/******************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));

	/*********************************************/
	/* przypisanie wynikow wyszukiwania do listy */
	/*********************************************/
	i=0;
	while(1)
	{
		status=bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp, FETCH_NEXT, &fetched_counter);
		if(status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
		{
			break;
		}

		/****************************************/
		/*	usuwanie poszczegolnych lobow	*/
		/****************************************/
		status=bmd_db_delete_blob(hDB,tmp);
		if (status<BMD_OK)
		{
			PRINT_WARNING("LIBBDMSQLWRN Unable to erase timestamp's blob\n");
		}
		free0(tmp);
		i++;
	}

// 	/************************************************/
// 	/*	usuwanie informacji o znacznikach czasu	*/
// 	/************************************************/
// 	asprintf(&SQLQuery, "DELETE FROM prf_dvcs WHERE fk_prf_signatures IN (SELECT id FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s) AND dvcs_type = '%s' AND location_id=%s;", CryptoObjectNr, location_id, OID_PKI_METADATA_DVCS, location_id);
//
// 	/************************************************/
// 	/*	wykonanie polecenie odszukania w bazie	*/
// 	/************************************************/
// 	bmd_db_result_free(&database_result);
// 	status=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
// 	if(status<0)
// 	{
// 		PRINT_WARNING("LIBBMDSQLWRN Unable to delete dvcs' information\n");
// 	}

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(SQLQuery);
	free0(tmp);

	return BMD_OK;
}


/**
*Usuwanie wszystkich podpisow powiazanych z crypto objectem o podanym id
*
*@param[in] void *hDB	- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] char *CryptoObjectNr	- id poszukiwanego crypto object'a w postaci lancucha znakowego
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*/
long DeleteSignatures(	void *hDB,
			char *CryptoObjectNr,
			char *location_id)
{
long status		= 0;
long i			= 0;
long fetched_counter	= 0;
char *SQLQuery		= NULL;
char *tmp		= NULL;
void *database_result	= NULL;

	PRINT_INFO("LIBBMDSQLINF Deleting file's signatures\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*********************************************************************/
	/* pobieranie oida odpowiedniego loba przypisanego do crypto_objecta */
	/*********************************************************************/
	asprintf(&SQLQuery, "SELECT signature FROM prf_signatures WHERE fk_crypto_objects=%s%s%s;",
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	/******************************************/
	/* wykonanie polecenie odszukania w bazie */
	/******************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));

	/*********************************************/
	/* przypisanie wynikow wyszukiwania do listy */
	/*********************************************/
	i=0;
	while(1)
	{
		status=bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp, FETCH_NEXT, &fetched_counter);
		if(status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
		{
			break;
		}

		/*********************************/
		/* usuwanie poszczegolnych lobow */
		/*********************************/
		status=bmd_db_delete_blob(hDB,tmp);
		if (status<BMD_OK)
		{
			PRINT_WARNING("LIBBDMSQLWRN Unable to erase timestamp's blob\n");
		}
		free0(tmp);
		i++;
	}

// 	/************************************************/
// 	/*	usuwanie informacji o znacznikach czasu	*/
// 	/************************************************/
// 	asprintf(&SQLQuery, "DELETE FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s;", CryptoObjectNr, location_id);
//
// 	/************************************************/
// 	/*	wykonanie polecenie odszukania w bazie	*/
// 	/************************************************/
// 	bmd_db_result_free(&database_result);
// 	status=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
// 	if(status<0)
// 	{
// 		PRINT_WARNING("LIBBMDSQLWRN Unable to delete signatures' information\n");
// 	}

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(SQLQuery);
	free0(tmp);

	return BMD_OK;
}


/**
*Usuwanie crypto objecta o podanym id
*
*@param[in] void *hDB	- wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@param[in] char *CryptoObjectNr	- id poszukiwanego crypto object'a w postaci lancucha znakowego
*
*@retval BMD_OK 			- zakonczenie pomyslne.
*@retval BMD_ERR_OP_FAILED		- nieudana operacja
*@retval BMD_ERR_PARAM1			- niepoprawny wskaznik do uchwytu ustanowionego polaczenia z baza danych
*@retval BMD_ERR_PARAM2			- niepoprawny wskaznik do lancucha znakowego z id crypto objecta
*/
long DeleteCryptoObjects(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				char *CryptoObjectNr,
				char *location_id,
				server_request_data_t *req)
{
long status			= 0;
long i				= 0;
long fetched_counter		= 0;
long error			= 0;
char *SQLQuery			= NULL;
char *tmp			= NULL;
void *database_result		= NULL;
char *ans			= NULL;
db_row_strings_t *db_row=NULL;

	PRINT_INFO("LIBBMDSQLINF Deleting file's signatures\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*********************************************************************/
	/* pobieranie oida odpowiedniego loba przypisanego do crypto_objecta */
	/*********************************************************************/
	asprintf(&SQLQuery, 	"SELECT raster, symkey FROM crypto_objects WHERE id=%s%s%s;",
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	/******************************************/
	/* wykonanie polecenie odszukania w bazie */
	/******************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));


	/*********************************************/
	/* przypisanie wynikow wyszukiwania do listy */
	/*********************************************/
	i=0;
	while(1)
	{
		status=bmd_db_result_get_row(hDB,database_result,i,0,&db_row,FETCH_NEXT,&fetched_counter);
		if(status==BMD_ERR_NODATA)
		{
			if( i == 0 )
				error=BMD_SQL_DELETE_FILE_NOT_FOUND;
			break;
		}
		status=bmd_db_delete_blob(hDB,db_row->colvals[0]);
		if (status<BMD_OK)
		{
			PRINT_WARNING("LIBBDMSQLWRN Unable to erase file's blob\n");
		}
		status=bmd_db_delete_blob(hDB,db_row->colvals[1]);
		if (status<BMD_OK)
		{
			PRINT_WARNING("LIBBDMSQLWRN Unable to erase symkey's blob\n");
		}
		bmd_db_row_struct_free(&db_row);
		i++;
	}

	/***********************************************************************************/
	/* jesli dokumenty maja byc usuwane z archiwum peranentnie bez pozostawiania sladu */
	/***********************************************************************************/
	if ((_GLOBAL_bmd_configuration->enable_delete_history==0) || (bmdJSRequest==NULL))
	{
		/*******************************************/
		/* usuwanie informacji o znacznikach czasu */
		/*******************************************/
		asprintf(&SQLQuery, 	"DELETE FROM crypto_objects WHERE id=%s%s%s;",
					CryptoObjectNr,
					location_id==NULL?"":" AND location_id=",
					location_id==NULL?"":location_id);

		/**********************************************/
		/* wykonanie polecenie usuniecia z bazy bazie */
		/**********************************************/
		bmd_db_result_free(&database_result);
		status=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
		if(status<0)
		{
			PRINT_WARNING("LIBBMDSQLWRN Unable to delete crypto object's information\n");
		}
	}
	else
	{
		/*******************************************************/
		/* ukrycie dotychczasowych metadanych opisujacych plik */
		/*******************************************************/
		BMD_FOK(JS_PrepareAndExecuteSQLQuery_Hide_AdditionalMetadata(hDB, CryptoObjectNr, location_id,req));

		/******************************************/
		/* wprowadzenie nowego wiersza metadanych */
		/******************************************/
		//PRINT_ERROR("ROL_DBG bmdJSRequest ptr %p\n",bmdJSRequest);
		//PRINT_ERROR("ROL_DBG bmdJSRequest->protocolData ptr %p\n",bmdJSRequest->protocolData);

		//TODO - zastanowic sie jak elegancko rozpoznac, ze mamy do czynienia z plikiem dodatkowym/ukrytym
		/****************************************************************/
		/* rozpoznanie, czy to plik dodatkowy - tak troche workaroudowo */
		/* jesli bmdJSRequest->protocolData jest ustawiony to pewnie    */
		/* jest to plik dodatkowy                                       */
		/****************************************************************/
		if (bmdJSRequest->protocolData)
		{
			BMD_FOK(set_gen_buf2(	(char*)_GLOBAL_bmd_configuration->default_delete_update_reason,
					(int)strlen(_GLOBAL_bmd_configuration->default_delete_update_reason),
					&(bmdJSRequest->protocolData)));

			BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadataHistory( hDB, bmdJSRequest, CryptoObjectNr, location_id, 0, NULL));
		}
	}

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(SQLQuery);
	free0(tmp);
	free0(ans);

	return error;
}


long DeleteCorespondingDocs(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				char *CryptoObjectNr,
				char *location_id,
				server_request_data_t *req)
{
long i			= 0;
long ans_count		= 0;
char **ans		= NULL;
char *SQLQuery		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*****************************************/
	/* wyszukanie plikow korespondencujacych */
	/*****************************************/
	asprintf(&SQLQuery, "SELECT id FROM crypto_objects WHERE corresponding_id=%s%s%s;",
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	BMD_FOK_NP(ExecuteSQLQueryWithAnswersKnownDBConnection(	hDB, SQLQuery, &ans_count, &ans));

	/*****************************************/
	/* usuwanie dokumentow o znalezionych id */
	/*****************************************/

	for (i=0; i<ans_count; i++)
	{
		DeleteDocFromDatabase(hDB, bmdJSRequest, ans[i], location_id, req);
		free0(ans[i]);
	}

	/************/
	/* porzadki */
	/************/
	free0(ans);
	free0(SQLQuery);

	return BMD_OK;
}



long DeleteLinkFromDatabase(void *hDB, char* cryptoObjectId, server_request_data_t *req)
{
long retVal		= 0;
void* queryResult	= NULL;
char* sqlQuery		= NULL;

db_row_strings_t* rowStruct	= NULL;
long fetchedCounter		= 0;
char* cryptoObjectLobId		= NULL;
char* symKeyLobId		= NULL;

char* selectTemplate = "SELECT raster, symkey FROM crypto_objects WHERE id=%s AND location_id=%s;";
char* deleteTemplate = "DELETE FROM crypto_objects WHERE id=%s AND location_id=%s;";

	if(hDB == NULL)
		{ return -1; }
	if(cryptoObjectId == NULL)
		{ return -2; }
	if(strlen(cryptoObjectId) <= 0)
		{ return -3; }
	if(req == NULL)
		{ /*ok, nie jest wykorzystywany w kodzie*/ }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -5; }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ return -6; }

	asprintf(&sqlQuery, selectTemplate, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(sqlQuery == NULL)
		{ return -11; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
		{ return -12; }

	retVal=bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowStruct, FETCH_ABSOLUTE, &fetchedCounter);
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
		{ return -13; }

	cryptoObjectLobId=strdup(rowStruct->colvals[0]);
	symKeyLobId=strdup(rowStruct->colvals[1]);
	bmd_db_row_struct_free(&rowStruct);

	if(cryptoObjectLobId == NULL || symKeyLobId == NULL)
	{
		free(cryptoObjectLobId); cryptoObjectLobId=NULL;
		free(symKeyLobId); symKeyLobId=NULL;
		return -14;
	}

	retVal=bmd_db_delete_blob(hDB, cryptoObjectLobId);
	free(cryptoObjectLobId); cryptoObjectLobId=NULL;
	if(retVal < BMD_OK)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		return -15;
	}

	retVal=bmd_db_delete_blob(hDB, symKeyLobId);
	free(symKeyLobId); symKeyLobId=NULL;
	if(retVal < BMD_OK)
		{ return -16; }

	asprintf(&sqlQuery, deleteTemplate, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(sqlQuery == NULL)
		{ return -17; }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	bmd_db_result_free(&queryResult);
	if(retVal < BMD_OK)
		{ return -18; }

	return 0;
}



long DeleteLinks(void *hDB, char *CryptoObjectNr, server_request_data_t *req)
{

char* selectTemplate="SELECT id FROM crypto_objects WHERE file_type='link' AND pointing_id=%s AND location_id=%s;";
char* sqlQuery		= NULL;
void* queryResult	= NULL;
long retVal		= 0;

db_row_strings_t* rowStruct	= NULL;
long fetchedCounter		= 0;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(CryptoObjectNr == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }


	asprintf(&sqlQuery, selectTemplate, CryptoObjectNr,_GLOBAL_bmd_configuration->location_id);
	if(sqlQuery==NULL)
		{ BMD_FOK(NO_MEMORY); }

	retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;

	//nie ma linkow do wskazanego obiektu
	if(retVal == BMD_ERR_NODATA)
		{ return BMD_OK; }
	else if(retVal < BMD_OK)
		{ BMD_FOK(retVal); }

	for(;;)
	{
		retVal=bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowStruct, FETCH_NEXT, &fetchedCounter);
		if(retVal == BMD_ERR_NODATA)
			{ break; }
		else if(retVal < BMD_OK)
		{
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}

		retVal=DeleteLinkFromDatabase(hDB, rowStruct->colvals[0], req);
		bmd_db_row_struct_free(&rowStruct);
		if(retVal < 0)
		{
			bmd_db_result_free(&queryResult);
			PRINT_ERROR("Error: Unable to delete link from database (%li)", retVal);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}

	bmd_db_result_free(&queryResult);

	return BMD_OK;
}


long DeleteOtherData(		void *hDB,
				char *CryptoObjectNr,
				char *location_id,
				char *tableName,
				server_request_data_t *req)
{
char *SQLQuery		= NULL;
void *database_result	= NULL;

	PRINT_INFO("LIBBMDSQLINF Deleting file's aditional metadata\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&SQLQuery, "DELETE FROM %s WHERE fk_crypto_objects=%s%s%s;",
				tableName,
				CryptoObjectNr,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/****************************************/
	/* wykonanie polecenia usuwania w bazie */
	/****************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&database_result);
	free0(SQLQuery);

	return BMD_OK;
}

long DeleteDocFromDatabase(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				char *CryptoObjectNr,
				char *location_id,
				server_request_data_t *req)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************/
	/* usuwaie danych z archiwum */
	/*****************************/
	DeleteTimestamps(hDB, CryptoObjectNr, location_id);
	DeleteDVCS(hDB, CryptoObjectNr, location_id);
	DeleteSignatures(hDB, CryptoObjectNr, location_id);
	DeleteCorespondingDocs(hDB, bmdJSRequest, CryptoObjectNr, location_id,req);
	DeleteLinks(hDB, CryptoObjectNr, req);

	if (_GLOBAL_bmd_configuration->enable_delete_history==0)
	{
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_delete_action_register)
		{
			_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_delete_action_register(hDB, CryptoObjectNr, location_id);
		}
	}

	BMD_FOK(DeleteCryptoObjects(hDB, bmdJSRequest, CryptoObjectNr, location_id,req));

	return BMD_OK;
}



/************************************************************************************************/
/* Przeprowadzenie operacji usuwania crypto objecta wraz z powiazanymi z nim lobami,            */
/* podpisami, poswiadzeniami tych podpisow oraz znacznikow czasu wszystkich wartosci dowodowych */
/************************************************************************************************/
long DeleteDataSlot(	void *hDB,
			bmdDatagram_t *bmdJSRequest,
			bmdDatagram_t **bmdJSResponse,
			server_request_data_t *req)
{
char *CryptoObjectNr	= NULL;
char *ans		= NULL;
char *SQLQuery		= NULL;
char* fileType		= NULL;
long retVal		= 0;
long permissionsType	= 0; 

	PRINT_INFO("LIBBMDSQLINF Deleting file\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdJSRequest->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequest->protocolDataFileId->buf == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	


	/************/
	/* usuwanie */
	/************/

	// w konfiguracji ustawione, kto moze usuwac dokument
	/*if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->file_removing_permission_type == NULL)
		{ permissionsType = CONSIDER_CREATOR_OR_OWNER; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "CREATOR_OR_OWNER") == 0)
		{ permissionsType = CONSIDER_CREATOR_OR_OWNER; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "CREATOR") == 0)
		{ permissionsType = CONSIDER_ONLY_CREATOR; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "OWNER") == 0)
		{ permissionsType = CONSIDER_ONLY_OWNER; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "CREATOR_OR_OWNER_OR_GROUPS") == 0)
		{ permissionsType = CONSIDER_CREATOR_OR_OWNER_OR_GROUPS; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "CREATOR_OR_OWNER_OR_CURRENT_GROUP") == 0)
		{ permissionsType = CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "GROUPS") == 0)
		{ permissionsType = CONSIDER_GROUPS_GRAPH; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "CURRENT_GROUP") == 0)
		{ permissionsType = CONSIDER_CURRENT_GROUP; }
	else if(strcmp(_GLOBAL_bmd_configuration->file_removing_permission_type, "ANY") == 0)
		{ permissionsType = SKIP_AUTHORIZATION; }
	else
		{ permissionsType = CONSIDER_CREATOR_OR_OWNER; }*/
	

	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));
	BMD_FOK_TRANS(VerifyPermissionsForCryptoObject(hDB, req,
				bmdJSRequest->protocolDataFileId->buf,
				(bmdJSRequest->datagramType==BMD_DATAGRAM_DIR_DELETE ? CONSIDER_ONLY_DIRECTORY|permissionsType : permissionsType),
				NULL, &fileType, &CryptoObjectNr, NULL, NULL, NULL, NULL));
	BMD_FOK_TRANS(FillFromCryptoObjects(hDB, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id, WITHOUT_SYS_METADATA, *bmdJSResponse,req));

	/************************************************************/
	/* w przypadku usuwania katalogu sprawdzamy, czy jest pusty */
	/************************************************************/
	if (bmdJSRequest->datagramType==BMD_DATAGRAM_DIR_DELETE)
	{
		asprintf(&SQLQuery, 	"SELECT count(id) FROM crypto_objects WHERE corresponding_id=%s AND location_id=%s;",
					CryptoObjectNr,
					_GLOBAL_bmd_configuration->location_id);
		BMD_FOK_TRANS(ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &ans));

		if ((ans==NULL) || (strcmp(ans, "0")!=0))
		{
			BMD_FOK_TRANS(BMD_SQL_DELETE_DIR_NOT_EMPTY_ERROR);
		}
	}

	//jesli podany jest w zadaniu identyfikator linka, to ma byc usuniety tylko link
	if(strcmp(fileType, "link") == 0)
	{
		retVal=DeleteLinkFromDatabase(hDB, bmdJSRequest->protocolDataFileId->buf, req);
		if(retVal < 0)
		{
			PRINT_ERROR("ERROR: Unable to delete link from database (%li)", retVal);
			BMD_FOK_TRANS(BMD_ERR_OP_FAILED);
		}
	}
	//dla 'prawdziwego' dokumentu maja byc usuniete wszystkie jego linki, dokumenty powiazane itd. 
	else
	{
		BMD_FOK_TRANS(DeleteDocFromDatabase(hDB, bmdJSRequest, bmdJSRequest->protocolDataFileId->buf, _GLOBAL_bmd_configuration->location_id, req));
	}

	BMD_FOK(bmd_db_end_transaction(hDB));

	/************/
	/* porzadki */
	/************/
	free(fileType); fileType=NULL;
	free(CryptoObjectNr); CryptoObjectNr=NULL;
	free(SQLQuery); SQLQuery=NULL;
	free(ans); ans=NULL;

	return BMD_OK;
}
