#include <bmd/libbmdsql/history.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdlog/bmd_logs.h>


//#define SQL_CLEARANCE_AMBIGUITY_ERR -11
/*#define BMD_ERR_NODATA		-606*/
//#define ERR_NO_USER_WITH_GIVEN_ID	-800

#define OID_RFC3281_ROLES 1,2,3,4
#include "../klient/vs2005/klient/VersionNo.h"

long JS_bmdDatagramSet_to_getMetaDataWithTime(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long i						= 0;
long j						= 0;
long status					= 0;
bmdDatagramSet_t **tmp_dtg_set	= NULL;

	PRINT_INFO("LIBBMDSQLINF Getting file(s) history(datagramset)\n");
	/************************/
	/* walidacja parametrów */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/******************************************************/
	/*	zaalokowanie pamieci na datagramy odpowiedzi	*/
	/******************************************************/
	tmp_dtg_set=(bmdDatagramSet_t **)malloc(sizeof(bmdDatagramSet_t *)*bmdJSRequestSet->bmdDatagramSetSize+2);
	memset(tmp_dtg_set, 0, sizeof(bmdDatagramSet_t *)*bmdJSRequestSet->bmdDatagramSetSize+1);

	for(i=0; i<(int)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{

		/************************************************************************/
 		/*	sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania	*/
 		/************************************************************************/
 		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
 		{
			BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, &(tmp_dtg_set[i])));
 			PR_bmdDatagram_PrepareResponse(&((tmp_dtg_set[i])->bmdDatagramSetTable[0]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus);
			BMD_FOK(bmd_datagram_add_to_set((tmp_dtg_set[i])->bmdDatagramSetTable[0], bmdJSResponseSet));
 			continue;
 		}

		/***************************/
		/* rozpoczynamy transakcje */
		/***************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_SERIALIZABLE));

		/******************************************/
		/*	obsluzenie pojedynczego datagramu	*/
		/******************************************/
		status=JS_bmdDatagram_to_getMetaDataWithTime(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], &(tmp_dtg_set[i]), req);

		/******************************************/
		/*	ustawienie statusu błędu datagramu	*/
		/******************************************/
		if (status<BMD_OK)
		{
			BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, &(tmp_dtg_set[i])));
 			PR_bmdDatagram_PrepareResponse(&((tmp_dtg_set[i])->bmdDatagramSetTable[0]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, status);
			BMD_FOK(bmd_datagram_add_to_set((tmp_dtg_set[i])->bmdDatagramSetTable[0], bmdJSResponseSet));
		}
		else
		{
			for (j=0; j<(int)(tmp_dtg_set[i])->bmdDatagramSetSize; j++)
			{

				BMD_FOK(bmd_datagram_add_to_set(tmp_dtg_set[i]->bmdDatagramSetTable[j], bmdJSResponseSet));

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

	return BMD_OK;

}

long JS_bmdDatagram_to_getMetaDataWithTime(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long i			= 0;
long ans2_count		= 0;
char **ans2		= NULL;
long ans_count		= 0;
char **ans		= NULL;
char *SQLQuery		= NULL;
char *CryptoObjectNr	= NULL;
char *file_type		= NULL;
char *file_name		= NULL;
long addMetadataNumber	= 0;
char **addMetadataOid	= NULL;
long addMetadataCount	= 0;
char **addMetadata	= NULL;
char* creatorCertificate	= NULL;
long retVal			= 0;

	PRINT_INFO("LIBBMDSQLINF Getting metadata history (datagramset)\n");
	/************************/
	/* walidacja parametrów */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSRequest->protocolDataFileId==NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	/*********************************************************************/
	/* sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania */
	/*********************************************************************/
	if (bmdJSRequest->datagramStatus<BMD_OK)
	{
		PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[0]), bmdJSRequest->datagramType+100, bmdJSRequest->datagramStatus);
		return BMD_OK;
	}

	/***********************************************/
	/* sprawdzanie czy plik do skasowania istnieje */
	/***********************************************/
	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, bmdJSRequest->protocolDataFileId->buf, 
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				&file_name,
				&file_type,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				&addMetadataCount,
				&addMetadata));
	//BMD_FOK(CheckIfCryptoObjectExist(hDB, bmdJSRequest, 0, &CryptoObjectNr, &file_type, &location_id, &file_name, NULL,  &addMetadataCount, &addMetadata, req));

	/********************************************/
	/* okreslenie liczby metadanych dodatkowych */
	/********************************************/
	//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataNumber, &addMetadataOid));

	/********************************************************/
	/* zliczenie ilości potrzebnych datagramow na odpowiedz */
	/********************************************************/
	asprintf(&SQLQuery, 	"SELECT id FROM add_metadata_history WHERE fk_crypto_objects=%s%s%s ORDER BY insert_date DESC;;",
				CryptoObjectNr,
				_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	ExecuteSQLQueryWithAnswersKnownDBConnection(hDB, SQLQuery, &ans_count, &ans);
	free0(SQLQuery);

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(ans_count+1, bmdJSResponseSet));

	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType=bmdJSRequest->datagramType+100;
	BMD_FOK(set_gen_buf2(CryptoObjectNr, (long)strlen(CryptoObjectNr), &((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFileId)));
	BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFileLocationId)));
	BMD_FOK(set_gen_buf2(file_name, (long)strlen(file_name), &((*bmdJSResponseSet)->bmdDatagramSetTable[0]->protocolDataFilename)));


	for (i=0; i<addMetadataCount; i++)
	{
		if ((addMetadata[i]!=NULL) && (strlen(addMetadata[i])>0))
		{
			BMD_FOK(bmd_datagram_add_metadata_char(	addMetadataOid[i], addMetadata[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[0])));
		}
		free0(addMetadataOid[i]);
		free0(addMetadata[i]);
	}
	free0(addMetadataOid);
	free0(addMetadata);

	/***************************************************/
	/* wypelenienie kolejnych datagramu historii pliku */
	/***************************************************/
	for (i=1; i<ans_count+1; i++)
	{
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequest->datagramType+100;

		/*********************************************************/
		/* obsluzenie pojedynczego wiersza tabeli crypto_objects */
		/*********************************************************/
		BMD_FOK(set_gen_buf2(CryptoObjectNr, (long)strlen(CryptoObjectNr), &((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFileId)));
		BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFileLocationId)));
		BMD_FOK(set_gen_buf2(file_name, (long)strlen(file_name), &((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataFilename)));
		BMD_FOK(FillFromAdditionalMetaDataHistory(hDB, ans[i-1], _GLOBAL_bmd_configuration->location_id, (*bmdJSResponseSet)->bmdDatagramSetTable[i-1], (*bmdJSResponseSet)->bmdDatagramSetTable[i],req));

		free0(ans[i-1]);
	}
	free0(ans);

	/**********************************************************/
	/* wypelnienie pierwszego - najmniej aktualnego datagramu */
	/**********************************************************/
	/** WSZ todo - bylo SELECT owner_user_dn, insert_date||'|File insertion' FROM crypto_objects WHERE...
	*   teraz daje id tozsamosci tworcy, ale docelowo powinny byc czytelne informacje
	*/
	asprintf(&SQLQuery, 	"SELECT creator, to_char(insert_date, 'YYYY-MM-DD HH24:MI:SS')||'|File insertion' FROM crypto_objects WHERE id=%s%s%s;",
				CryptoObjectNr,
				_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
				_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection(hDB, SQLQuery, &ans2_count, &ans2));
	free0(SQLQuery);

	(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolData->size=(long)strlen(ans2[1]);
	(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolData->buf=(char *)malloc(strlen(ans2[1])+2);
	memset((*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolData->buf, 0, strlen(ans2[1])+1);
	memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolData->buf, ans2[1], strlen(ans2[1]));

	retVal = getElementWithCondition(_GLOBAL_UsersDictionary, 2, ans2[0], 5, &creatorCertificate);
	if(retVal == BMD_OK)
	{
		(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolDataOwner->size=(long)strlen(creatorCertificate);
		(*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolDataOwner->buf=(char *)calloc(strlen(creatorCertificate)+1, sizeof(char));
		memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[ans_count]->protocolDataOwner->buf, creatorCertificate, strlen(creatorCertificate));
	}
	
	free(creatorCertificate); creatorCertificate=NULL;
	free0(ans2[0]);
	free0(ans2[1]);
	free0(ans2);

	/************/
	/* porzadki */
	/************/
	free0(SQLQuery);
	free0(CryptoObjectNr);
	free0(file_type);
	free0(file_name);

	return BMD_OK;
}
