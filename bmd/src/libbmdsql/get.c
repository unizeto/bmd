#include <bmd/libbmdsql/get.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdsql/history.h>
#include <bmd/libbmdsql/class.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/************************************************/
/*		GET FUNCTIONS				*/
/************************************************/
/** @note by WSZ
js_ret->symkeyAlreadySet to adres zmiennej (do funkcji przekazuje sie &zmienna).
Przy pierwszym wywylaniu tej funkcji (dla pierwszego datagramu w datagramsecie) zmienna ta MUSI byc ustawiona na 0.
Kazde wywolanie funkcji dla tego samego datagramsetu opiera sie na tej samej zmiennej symkeyAlreadySet (funkcja sama modyfikuje odpowiednio wartosc zmiennej symkeyAlreadySet w kolejnych wywolaniach).
Wszysto po to , aby klucz symetryczny do deszyfrowania lobow po stronie klienckiej zawarty byl w pierszym POPRAWNYM
datagramie datagramsetu (a nie w datagramie o indeksie 0, jak to bylo do tej pory)
*/
long _bmdPrepareDtgForGetRequest(	bmdDatagram_t *dtg,
					JS_return_t *js_ret,
					long i,
					bmdDatagram_t **tmpbmdDatagram)
{
GenBuf_t *tmp_buf		= NULL;
GenBuf_t *tmp1			= NULL;

/****************************************************************/
/*	klucz symetryczny przekazywany w pierszym datagramie	*/
/****************************************************************/
bmd_crypt_ctx_t *first_ctx=NULL;

	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(js_ret==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/********************************************************/
	/*	przygotowanie datagramu dla requestow LOBOWYCH	*/
	/********************************************************/
	if( (dtg->datagramType==BMD_DATAGRAM_GET_ONLY_FILE) ||
	    (dtg->datagramType==BMD_DATAGRAM_TYPE_GET_LO) ||
	    (dtg->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO) ||
	    (dtg->datagramType==BMD_DATAGRAM_GET_CGI_LO) ||
		(dtg->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO))
	{
		/******************************************************************************************************/
		/*	klucz pobierany jest dla kazdego datagramu w zbiorze - kazdy datagram moze miec inny klucz	*/
		/*	pobranie DER klucza symetrycznego z datagramu									*/
		/******************************************************************************************************/
		BMD_FOK(bmd2_datagram_get_symkey((*tmpbmdDatagram),&tmp_buf));

		/******************************************************************************************************************/
		/*	zdekodowanie pobranego klucza z wykorzystaniem kontekstu serwera i ustawienie trybu pracy na strumieniowy	*/
		/******************************************************************************************************************/
		BMD_FOK(bmd_decode_bmdKeyIV(tmp_buf,NULL,js_ret->server_ctx,&(js_ret->lob_ctx_server[i])));
		js_ret->lob_ctx_server[i]->sym->sym_params=BMD_SYM_MULTI_ROUND;

		if(js_ret->symkeyForUserAlreadySet == 0)
		{
			/************************************************************************************/
			/*	wygenerowanie nowego kontekstu ktory bedzie szyfrowal LOBA - dla uzytkownika	*/
			/************************************************************************************/
			BMD_FOK(bmd_set_ctx_sym(&(js_ret->lob_ctx_user),BMD_CRYPT_ALGO_DES3,NULL,NULL));
			js_ret->lob_ctx_user->sym->sym_params=BMD_SYM_MULTI_ROUND;

			/******************************************************************/
			/*	zakodowanie kontekstu do DER i umieszczenie go w datagramie	*/
			/******************************************************************/
			BMD_FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,js_ret->user_cert_ctx,
										js_ret->lob_ctx_user->sym->key,
										js_ret->lob_ctx_user->sym->IV,NULL,&tmp1));

			BMD_FOK(bmd2_datagram_update_symkey(tmp1,tmpbmdDatagram));
			js_ret->symkeyForUserAlreadySet=1; /*klucz symetryczny zostal ustawiony, wiec przy kolejnych wywolaniach funkcji juz nie bedzie*/
		}
		else /* dla pozostalych datagramow - po pobraniu*/
		{

			/************************************************************/
			/*	skasowanie metadanej systemowej - klucz symetryczny	*/
			/************************************************************/
			BMD_FOK(bmd2_datagram_del_symkey(*tmpbmdDatagram));
		}
		free_gen_buf(&tmp_buf); free_gen_buf(&tmp1);
	}
	else if((dtg->datagramType==BMD_DATAGRAM_TYPE_GET) ||
		(dtg->datagramType==BMD_DATAGRAM_TYPE_GET_PKI) ||
		(dtg->datagramType==BMD_DATAGRAM_GET_CGI) ||
		(dtg->datagramType==BMD_DATAGRAM_DIR_GET_DETAIL))
	{

		GenBuf_t *deciphered_buf=NULL,*tmp1=NULL,*tmp=NULL;
		bmd_crypt_ctx_t *sym_ctx=NULL;

		/********************************************************************************************************************************/
		/*	dla kazdego datagramu ze zbioru pobranie,zdekodowanie klucza do struktury, zdeszyfrowanie symetryczne protocolData	*/
		/********************************************************************************************************************************/
		BMD_FOK(bmd2_datagram_get_symkey(*tmpbmdDatagram,&tmp));
		BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,js_ret->server_ctx,&sym_ctx));
		free_gen_buf(&tmp);

		BMD_FOK(bmd_symmetric_decrypt((*tmpbmdDatagram)->protocolData, 0, &sym_ctx,&deciphered_buf,NULL));

		/************************************************************************************************************************/
		/*	dla pierwszego datagramu - utworzenie kontekstu, zakodowanie DER i umieszczenie go w datagramie pierwszym	*/
		/************************************************************************************************************************/
		if(i==0)
		{
			BMD_FOK(bmd_set_ctx_sym(&first_ctx,BMD_CRYPT_ALGO_DES3,NULL,NULL));
			BMD_FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,js_ret->user_cert_ctx,
										first_ctx->sym->key,
										first_ctx->sym->IV,NULL,&tmp1));
			BMD_FOK(bmd2_datagram_update_symkey(tmp1,tmpbmdDatagram));
		}
		else
		{
			BMD_FOK(bmd2_datagram_del_symkey(*tmpbmdDatagram));
		}

		free_gen_buf(&((*tmpbmdDatagram)->protocolData));

		(*tmpbmdDatagram)->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
		(*tmpbmdDatagram)->protocolData->buf=(char *)malloc(deciphered_buf->size+2);
		memset((*tmpbmdDatagram)->protocolData->buf, 0, deciphered_buf->size+1);
		memcpy((*tmpbmdDatagram)->protocolData->buf, deciphered_buf->buf, deciphered_buf->size);
		(*tmpbmdDatagram)->protocolData->size=deciphered_buf->size;

		free_gen_buf(&deciphered_buf);
		free_gen_buf(&tmp1);
		bmd_ctx_destroy(&sym_ctx);
		bmd_ctx_reinit(&first_ctx); /* zeby mozna bylo uzywac ponownie kontekstu z tym samym kluczem symetrycznym */
	}

	/************/
	/* porzadki */
	/************/
	free_gen_buf(&tmp_buf);
	free_gen_buf(&tmp1);

	return BMD_OK;
}

/******************************************************************************************/
/*	wypelnianie datagramsetu w odpowiedzi na zadanie pobierania danyc hz bazy danych	*/
/******************************************************************************************/
long JS_bmdDatagramSet_to_get(	void *hDB,
				bmdDatagramSet_t *bmdJSRequestSet,
				EnumWithProtocolData_t WithProtocolData,
				EnumWithPkiMetadata_t WithPkiMetadata,
				EnumWithAddMetadata_t WithAddMetadata,
				EnumWithSysMetadata_t WithSysMetadata,
				EnumWithActionMetadata_t WithActionMetadata,
				LobsInDatagram_t LobsInDatagram,
				GetInCGIMode_t CGIMode,
				bmdDatagramSet_t **bmdJSResponseSet,
				JS_return_t *js_ret,
				server_request_data_t *req)

{
long status	= 0;
long i		= 0;

	/** by WSZ  - musi byc flaga wyzerowana przy obsludze kazdego nowego datagramsetu pobierania */
	if(js_ret != NULL)
	{
		js_ret->symkeyForUserAlreadySet=0;
	}

	PRINT_INFO("LIBBMDSQLINF Getting data from database\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	/************************************************/
	/*	alokacja pamieci na struktury lobowe	*/
	/************************************************/
	if (LobsInDatagram==LOB_NOT_IN_DATAGRAM)
	{
		js_ret->oid=(char **)malloc(sizeof(char*)*(bmdJSRequestSet->bmdDatagramSetSize+2));
		memset(js_ret->oid,0,sizeof(char*)*(bmdJSRequestSet->bmdDatagramSetSize+1));

		js_ret->lob_ctx_server=(bmd_crypt_ctx_t **)malloc(sizeof(bmd_crypt_ctx_t *) * \
		(bmdJSRequestSet->bmdDatagramSetSize+2));
		memset(js_ret->lob_ctx_server,0,sizeof(bmd_crypt_ctx_t *)*(bmdJSRequestSet->bmdDatagramSetSize+1));

		js_ret->lob_ctx_user=NULL;
	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/************************************************/
	/*	obsluga poszczegolnych datagramow	*/
	/************************************************/
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

		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType + 100;

		/**************************************/
		/* rozpoczecie transakcji bazy danych */
		/**************************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/********************************/
		/* obsluga pojedynczego zadania */
		/********************************/
		status=JS_bmdDatagram_to_get(	hDB,
						bmdJSRequestSet->bmdDatagramSetTable[i],
						WithProtocolData,
						WithPkiMetadata,
						WithAddMetadata,
						WithSysMetadata,
						WithActionMetadata,
						LobsInDatagram,
						CGIMode,
						&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),
						js_ret,
						i,req);

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

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
			status=bmd_db_end_transaction(hDB);
			if(status < BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
				bmd_db_rollback_transaction(hDB,NULL);
			}
		}

	}

	return BMD_OK;
}


/************************************************************************************/
/*	wypelnianie datagramu w odpowiedzi na zadanie pobierania danyc hz bazy danych	*/
/************************************************************************************/
long JS_bmdDatagram_to_get(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				EnumWithProtocolData_t WithProtocolData,
				EnumWithPkiMetadata_t WithPkiMetadata,
				EnumWithAddMetadata_t WithAddMetadata,
				EnumWithSysMetadata_t WithSysMetadata,
				EnumWithActionMetadata_t WithActionMetadata,
				LobsInDatagram_t LobsInDatagram,
				GetInCGIMode_t CGIMode,
				bmdDatagram_t **bmdJSResponse,
				JS_return_t *js_ret,
				long i,
				server_request_data_t *req)
{
char *CryptoObjectNrTemp	= NULL;
char *CryptoObjectNr		= NULL;
char *SQLQuery			= NULL;

long retVal				= 0;
long iter				= 0;
char* symkeyQueryTemplate	= "SELECT symkey FROM crypto_objects_history WHERE id=%s AND location_id=%s;";
char* symkeyOid			= NULL;
GenBuf_t* symkeyLob		= NULL;
char* currentVersionFileId	= NULL;
char* escapeTemp		= NULL;
char* userClassId		= NULL;


	if(bmdJSRequest==NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponse == NULL)
		{ BMD_FOK(BMD_ERR_PARAM10); }

	if ((bmdJSRequest->protocolDataFilename!=NULL) && (bmdJSRequest->datagramType!=BMD_DATAGRAM_GET_ONLY_FILE))
	{
		/**********************************************************************************/
		/* sprawdzenie czy uzytkownik ma prawa do pliku, ktorego podpis chce zweryfikowac */
		/**********************************************************************************/
		BMD_FOK(bmd_db_escape_string(hDB, bmdJSRequest->protocolDataFileId->buf, STANDARD_ESCAPING, &escapeTemp));
			
		asprintf(&SQLQuery, "SELECT fk_crypto_objects FROM prf_signatures WHERE id='%s' AND location_id='%s';",
					escapeTemp,_GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &CryptoObjectNrTemp));
		free0(SQLQuery);
		
		BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, CryptoObjectNrTemp,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				NULL/*fileType*/,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/));
		//BMD_FOK(CheckIfCryptoObjectExist(hDB, dtg_tmp, 0, &CryptoObjectNr, NULL, &location_id, NULL, NULL, NULL, NULL, req));
		free0(CryptoObjectNrTemp);


		/*********************************************/
		/* wypelnienie datagramu zadanymi metadanymi */
		/*********************************************/
		BMD_FOK(FillFromAllMetadata(	hDB,
						CryptoObjectNr,
						_GLOBAL_bmd_configuration->location_id,
						WithPkiMetadata,
						WITHOUT_ADD_METADATA,
						WITHOUT_SYS_METADATA,
						WITHOUT_ACTION_METADATA,
						*bmdJSResponse,req));
		
		/****************************************************/
		/* w przypadku zadania pobrania pliku z bazy danych */
		/****************************************************/
		if (WithProtocolData==WITH_PROTOCOL_DATA)
		{
			js_ret->cur_index=i;
			BMD_FOK(FillFromProtocolData( hDB, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id, LobsInDatagram,
						CURRENT_FILE_VERSION, *bmdJSResponse, js_ret));

			/*************************************************************/
			/* przygotowanie datagramu wraz z umieszczeniem w nim kluczy */
			/*************************************************************/
			BMD_FOK(_bmdPrepareDtgForGetRequest(bmdJSRequest, js_ret, i, bmdJSResponse));

		}
	}
	else if(bmdJSRequest->datagramType == BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO)
	{
		if(bmdJSRequest->protocolDataFileId==NULL)
			{ BMD_FOK(BMD_ERR_PARAM2); }
	
		for(iter=0; iter<bmdJSRequest->no_of_sysMetaData; iter++)
		{
			if(bmdJSRequest->sysMetaData[iter]->OIDTableBuf != NULL)
			{
				if(strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
				{
					if(bmdJSRequest->sysMetaData[iter]->AnySize > 0 && bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
					{
						currentVersionFileId=(char*)calloc(bmdJSRequest->sysMetaData[iter]->AnySize+1, sizeof(char));
						if(currentVersionFileId == NULL)
							{ BMD_FOK(NO_MEMORY); }
						memcpy(currentVersionFileId, bmdJSRequest->sysMetaData[iter]->AnyBuf, bmdJSRequest->sysMetaData[iter]->AnySize);
					}
				}
			}
		}
		
		if(currentVersionFileId == NULL)
		{
			BMD_FOK(BMD_ERR_CURRENT_VERSION_FILE_ID_NOT_SPECIFIED);
		}
		
		retVal=VerifyPermissionsForArchiveCryptoObject(	hDB, req, currentVersionFileId, bmdJSRequest->protocolDataFileId->buf, 
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				NULL/*fileType*/,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/);
		if(retVal < BMD_OK)
		{
			free(currentVersionFileId);
			BMD_FOK(retVal);
		}
		
		/*********************************************/
		/* wypelnienie datagramu zadanymi metadanymi */
		/*********************************************/
		BMD_FOK(FillFromAllMetadata(	hDB,
						CryptoObjectNr,
						_GLOBAL_bmd_configuration->location_id,
						WithPkiMetadata,
						WithAddMetadata,
						WithSysMetadata,
						WithActionMetadata,
						*bmdJSResponse,req));

		BMD_FOK(bmd_db_escape_string(hDB, bmdJSRequest->protocolDataFileId->buf, STANDARD_ESCAPING, &escapeTemp));
		
		//pobranie symkey z crypto_objects_history , pobranie loba symkeya i updatowanie w datagramie.
		asprintf(&SQLQuery, symkeyQueryTemplate, escapeTemp, _GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		if(SQLQuery == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		retVal=ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &symkeyOid);
		free(SQLQuery); SQLQuery=NULL;
		if(retVal < BMD_OK)
		{
			BMD_FOK(retVal);
		}
		
		retVal=bmd_db_import_blob(hDB, symkeyOid, &symkeyLob),
		free(symkeyOid); symkeyOid=NULL;
		if(retVal < BMD_OK)
		{
			BMD_FOK(retVal);
		}
		
		retVal=bmd2_datagram_update_symkey(symkeyLob, bmdJSResponse);
		free_gen_buf(&symkeyLob);
		if(retVal < BMD_OK)
		{
			BMD_FOK(retVal);
		}
		
		/****************************************************/
		/* w przypadku zadania pobrania pliku z bazy danych */
		/****************************************************/
		if (WithProtocolData==WITH_PROTOCOL_DATA)
		{
			js_ret->cur_index=i;
			BMD_FOK(FillFromProtocolData( hDB, bmdJSRequest->protocolDataFileId->buf, _GLOBAL_bmd_configuration->location_id, LobsInDatagram, ARCHIVE_FILE_VERSION, *bmdJSResponse, js_ret));

			/*************************************************************/
			/* przygotowanie datagramu wraz z umieszczeniem w nim kluczy */
			/*************************************************************/
			BMD_FOK(_bmdPrepareDtgForGetRequest(bmdJSRequest, js_ret, i, bmdJSResponse));

		}
	}
	else
	{
		/************************************************************************************************/
		/* sprawdzenie czy plik istnieje lub czy uzytkownik ma do niego uprawnienia i zwrócenie jego id */
		/************************************************************************************************/
		if(bmdJSRequest->protocolDataFileId != NULL)
		{
			BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req,
				bmdJSRequest->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				NULL/*fileType*/,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/));
		//BMD_FOK(CheckIfCryptoObjectExist(hDB, bmdJSRequest, 0, &CryptoObjectNr, &file_type, &location_id, NULL, NULL, NULL, NULL, req));
		}
		else
		{
			BMD_FOK(VerifyPermissionsForCryptoObjectByMetadata(hDB, req,
				bmdJSRequest,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				NULL/*fileType*/,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/));
		}


		/*********************************************/
		/* wypelnienie datagramu zadanymi metadanymi */
		/*********************************************/
		BMD_FOK(FillFromAllMetadata(	hDB,
						CryptoObjectNr,
						_GLOBAL_bmd_configuration->location_id,
						WithPkiMetadata,
						WithAddMetadata,
						WithSysMetadata,
						WithActionMetadata,
						*bmdJSResponse,req));
		
		/****************************************************/
		/* w przypadku zadania pobrania pliku z bazy danych */
		/****************************************************/
		if (WithProtocolData==WITH_PROTOCOL_DATA)
		{
			js_ret->cur_index=i;
			BMD_FOK(FillFromProtocolData( hDB, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id, LobsInDatagram,
				CURRENT_FILE_VERSION, *bmdJSResponse, js_ret));

			/*************************************************************/
			/* przygotowanie datagramu wraz z umieszczeniem w nim kluczy */
			/*************************************************************/
			BMD_FOK(_bmdPrepareDtgForGetRequest(bmdJSRequest, js_ret, i, bmdJSResponse));

		}
		
		if(_GLOBAL_std_session_configuration != NULL &&
			_GLOBAL_std_session_configuration->enable_first_get_notice == 1 &&
			_GLOBAL_std_session_configuration->notice_for_user_class_id != NULL &&
			WithProtocolData==WITH_PROTOCOL_DATA)
		{
			BMD_FOK(DetermineUserClassForRequest(bmdJSRequest, req, &userClassId));
			if(strcmp(userClassId, _GLOBAL_std_session_configuration->notice_for_user_class_id) == 0)
			{
				BMD_FOK(noticeFirstTimeActivity(hDB, TRANSACTION_ALREADY_STARTED, ACTIVITY_SUCCEED, CryptoObjectNr, _GLOBAL_std_session_configuration->file_get_string, NULL, _GLOBAL_std_session_configuration->notice_metadata_oid));
			}
			free(userClassId); userClassId = NULL;
		}
	}

	/************/
	/* porzadki */
	/************/
	free0(CryptoObjectNr);

	return BMD_OK;
}
