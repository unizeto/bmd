#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <string.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdsql/common.h>


/**
 * Funkcja pobiera wartość pola z XML'a na podstawie
 * jego XPatha 
 *
 * @param[in] doc - uchwyt do XMLa
 * @param[in] ctx - kontekst dla mechanizmu wyszukiwania po XPath
 * @param[in] xpath - Xpath
 * @param[out] value - odczytana wartość
 *
 * @return status zakończenia 
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_PARAM4 - niepoprawny czwarty parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - brak węzła o zadanej wartości XPath
*
*/

long getXpathValue( xmlDocPtr doc, xmlChar* xpath, char** value ) {

/* --------------------------------------------- */

	xmlXPathContextPtr	       ctx;
	xmlXPathObjectPtr   result  = NULL;
	xmlNodeSetPtr       nodeset = NULL;
	xmlChar*            keyword = NULL;

/* --------------------------------------------- */



if ( doc == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
}

if ( xpath == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
}

if ( value == NULL || *value != NULL ) {
	PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
	return BMD_ERR_PARAM3;
}

ctx = xmlXPathNewContext( doc );

if (ctx == NULL) {
        PRINT_ERROR("Błąd tworzenia kontekstu xmlXpath. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
}

ctx->namespaces = xmlGetNsList( doc, xmlDocGetRootElement(doc));
ctx->nsNr = 0;

if ( ctx->namespaces != NULL ) {
	while ( ctx->namespaces[ctx->nsNr] != NULL ) {
		ctx->nsNr++;
	}
}

result = xmlXPathEvalExpression(xpath, ctx);

if (result && !xmlXPathNodeSetIsEmpty(result->nodesetval)) {
	nodeset = result->nodesetval;
	keyword = xmlNodeListGetString( doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);

	if ( keyword != NULL ) {
		asprintf(value, "%s", keyword);
	}
	xmlFree(keyword);
}
else {
	PRINT_ERROR("Nie odnaleziono węzła o zadanej wartości XPath. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
}

xmlXPathFreeContext(ctx);
xmlXPathFreeObject (result);

return BMD_OK;

}

/**
* Funkcja sprawdza poprawność strukturalną dokumentu XML
*
* @param[in] buff_tBuff - bufor z dokumentem
* @param[out] hXmlDoc - uchwyt do poprawnie
                        sparsowanego dokumentu
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błądy parsowania dokumentu
* @retval BMD_OK - dokument sparsowany prawidłowo
*
*/


int MessageParse( GenBuf_t* const buff, xmlDocPtr* const hXmlDoc ) {

if ( buff == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
}

if ( hXmlDoc == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
}

*hXmlDoc = xmlParseMemory( (const char*)buff->buf, buff->size );

if ( *hXmlDoc == NULL ) {
	return BMD_ERR_OP_FAILED;
}

 return BMD_OK;

}



/**
* Funkcja pobiera identyfikator szablonu e-Awiza
*
* @param[in] hXmlDoc - uchwyt do dokumentu XML
* @param[out] templateId - identyfikator szablonu 

* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błądy parsowania dokumentu
* @retval BMD_OK - dokument sparsowany prawidłowo
*
*/


long getTemplateID(xmlDocPtr const hXmlDoc, char** const templateId) {



/* --------------------------------- */

	/* char* title = NULL; */
	long longRet = 0;

/* --------------------------------- */



if ( hXmlDoc == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
}

if ( templateId == NULL || *templateId != NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
}

longRet = getXpathValue( hXmlDoc, (xmlChar*)"//TEMPLATEID", templateId);

if ( longRet != BMD_OK ) {
	/*	getXpathValue( hXmlDoc, (xmlChar*)"//TITLE", &title); */
	/* pozostała część procedury wyszukiwania odpowiedniego 
          szablonu */
	/* na razie zakładamy że zawsze podajemy id szablonu */
	/* pozostałe sytuacje do ponownego rozpatrzenia */
	return BMD_ERR_OP_FAILED;
}

return BMD_OK;

}


/**
*  Funkcja sprawdza czy szablon o zadanym identyfikatorze
*  znajduje się w pamięci cache.
* 
* @param[in] cacheId - uchwyt do węzła z pamięci cache
* @param[in] templateId - identyfikator szablonu 

* @retval FOUND_IN_QUEUE - znaleziono węzeł
* @retval NFOUND_IN_QUEUE - nie znaleziono węzła
*
*/

long cmpCacheNode(const void* const cacheId, const void* const templateId) {

 if (((cacheNode_t*)cacheId)->id == atol((char*)templateId)) {
	return FOUND_IN_QUEUE;
 }

 return NFOUND_IN_QUEUE;

}


/**
* Funkcja pobiera szablon z pamieci cache jesli on
* sie tam znajduje badz pobiera go z bazy danych
*
* @param[in] templateId - identyfikator szablonu
* @param[in] template - uchwyt do struktury z szablonem 
*
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_OK - zakonczono z sukcesem
* 
*/

long downloadTemplateID(char* const templateId, void** const template, void* dbHandler) {


 /* ----------------------------------- */

	long		longRet = 0;
	union sigval	value;

 /* ----------------------------------- */



 if ( templateId == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
 }

 if ( dbHandler == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
 }

 longRet = FindInQueue( &_GLOBAL_myCache, cmpCacheNode, templateId, template );

 if (!(longRet == FOUND_IN_QUEUE)) {
	if ((_GLOBAL_awizoConfig.mailqueue != 1) || (_GLOBAL_awizoConfig.maxmailqueue == 1)) {
		longRet = getDataFromDB( templateId, dbHandler, template );
		if ( longRet != BMD_OK) return longRet;
	}

	/* **************************************************** */
	/*  sygnał odnośnie dodania szablonu do pamięci cache   */
	/* **************************************************** */

	   sem_wait(&(_GLOBAL_shptr->clientMutex));

	   value.sival_int = atoi(templateId);
	   sigqueue( getpid(), SIGRTMIN+4, value); 
 }

 return BMD_OK;

}



/**
*  Funkcja pobiera szablon i powiązane z nim załączniki
*  z bazy danych 
*
* @param[in] templateId - identyfikator szablonu
* @param[in] template - uchwyt do struktury z szablonem 
* 
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_ERR_OP_FAILED - błędy zapisu do bazy i dekodowania z base64
*
*/

long getDataFromDB(char* const templateId, void* dbHandler, void** const template ) {

/* ----------------------------------------------------- */

	long			longRet = 0;
	long			rows = 0;
	long			cols = 0;
	void*			result = NULL;
	long			fetched = 0;
	db_row_strings_t*	rowstruct = NULL;
	attchNode_t* 		attchNode_tNode = NULL;
	queue_t*		attachments = NULL;
	cacheNode_t*		cacheNode_tNode = NULL;
	char*			content = NULL;
	size_t			contentLenght;
	char* 			stringSQL = NULL;

/* ------------------------------------------------------ */



 if ( templateId == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
 }

 if ( dbHandler == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
 }

 /* ******************************************** */
 /*   Pobieranie załączników z bazy danych       */
 /* ******************************************** */

 if ( asprintf( &stringSQL, "SELECT filename, mimetype, disposition, content FROM mailing_email_attachments WHERE templateid = %s;", templateId ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 if ( bmd_db_execute_sql( dbHandler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	return BMD_ERR_OP_FAILED;
 } 

 free0(stringSQL);
 attachments = (queue_t*)calloc(1, sizeof(queue_t));

 if (attachments == NULL){
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 InitQueue( attachments );

 do {
	longRet = bmd_db_result_get_row( dbHandler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );
	
	if (longRet == 0) {
		contentLenght = strlen(rowstruct->colvals[3]);
		content = (char*)spc_base64_decode( (unsigned char*)(rowstruct->colvals[3]), &contentLenght, 0, (longRet=0, (int*)&longRet));

		if ( longRet != 0){
			PRINT_ERROR("Błąd dekodowania base64. Error = %ld\n", longRet);
			bmd_db_result_free( &result );
			bmd_db_row_struct_free( &rowstruct );
			return BMD_ERR_OP_FAILED;
		}

		/* ************************************************************** */
		/*    Utworzenie węzła przechowywującego załączniki               */
		/* ************************************************************** */

		if ( CreateAttchNode( &attchNode_tNode, rowstruct->colvals[0], rowstruct->colvals[1], \
					rowstruct->colvals[2], content, contentLenght ) < 0 ){
			PRINT_ERROR("Błąd przy zapisie zalacznika do pamieci cache. Error = %d\n", BMD_ERR_OP_FAILED);
			bmd_db_result_free( &result );
			bmd_db_row_struct_free( &rowstruct );
			free0(content);
			return BMD_ERR_OP_FAILED;
		}

		free0(content);

		/* *********************************************** */
		/*    Dodanie węzła z załącznikami do kolejki      */
		/* *********************************************** */

		if ( InsertToQueue( attachments, attchNode_tNode ) < 0 ) {
			PRINT_ERROR("Błąd dodawania elementu do listy. Error = %d\n", BMD_ERR_OP_FAILED);
			bmd_db_result_free( &result );
			bmd_db_row_struct_free( &rowstruct );
			return BMD_ERR_OP_FAILED;
		}

		bmd_db_row_struct_free( &rowstruct );
	}

 }while(longRet == 0);

 bmd_db_result_free( &result );
 fetched = 0;

 /* ******************************************** */
 /*   Pobieranie szablonu z bazy danych          */
 /* ******************************************** */

 if ( asprintf( &stringSQL, "SELECT * FROM mailing_email_template WHERE id = %s;", templateId ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_MEMORY;
 }

 if ( bmd_db_execute_sql( dbHandler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_OP_FAILED;
 } 

 free0(stringSQL);
 longRet = bmd_db_result_get_row( dbHandler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

 if ( longRet < 0 ) {
	PRINT_ERROR("Błąd pobierania danych z bazy. Error = %d\n", BMD_ERR_OP_FAILED);
	bmd_db_result_free( &result );
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_OP_FAILED;
 }

 bmd_db_result_free( &result );

 if ( _GLOBAL_myCache.ulCount > _GLOBAL_awizoConfig.maxcachesize) {

  /* ************************************************************* */
  /*   Usunięcie wezla z pamieci cache w przypadku przepełnienia   */
  /* ************************************************************* */

   longRet = DeleteFromQueue(&_GLOBAL_myCache, (void**)&(_GLOBAL_myCache.head), DestroyCacheNode);

   if ( longRet != BMD_OK ) {
	PRINT_ERROR("Błąd usuwania węzła z pamięci cache. Error = %d\n", BMD_ERR_OP_FAILED);
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_OP_FAILED;
   } 

 }

 /* ************************************ */
 /*   Utworzenie węzła pamięci cache     */
 /* ************************************ */

 longRet = CreateCacheNode( &cacheNode_tNode, atol(rowstruct->colvals[0]), rowstruct->colvals[4], \
				rowstruct->colvals[5], rowstruct->colvals[6], rowstruct->colvals[8], \
				rowstruct->colvals[9], rowstruct->colvals[10], rowstruct->colvals[12], \
				rowstruct->colvals[13], rowstruct->colvals[14], rowstruct->colvals[15], \
				attachments );

 bmd_db_row_struct_free( &rowstruct );

 if (longRet < 0 ) {
	PRINT_ERROR("Błąd zapisu węzła cache. Error = %d\n", BMD_ERR_OP_FAILED);
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_OP_FAILED;
 }

 /* ************************************ */
 /*   Dodanie wezla do pamieci cache     */
 /* ************************************ */

 longRet = InsertToQueue( &_GLOBAL_myCache, cacheNode_tNode );

 if (longRet != BMD_OK){
	PRINT_ERROR("Błąd wstawiania węzła do pamięci cache. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 if (template) *template = _GLOBAL_myCache.tail;

 return BMD_OK;

}



/**
 * Funkcja przygotowuje e-Awizo do wysyłki, wczytuje grafikę
 * w postaci załączników
 *
 * @param[in] smtpParam_tParams - parametry dla e-Awiza
 * @param[in] message           - struktura opisująca wiadomość
 * @param[in] graphicsDir       - katalog z plikami graficznymi
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd przy dołączeniu załącznika
 *
*/


long mailPrepare( void* const cacheNode, bmd_smime_msg** const message )

{

   /* --------------------------------------- */

      long            longRet         =    0;
      cacheNode_t*    cacheNode_tNode = NULL;
      queueNode_t*    attchQueueNode  = NULL;
      attchNode_t*    attchNode       = NULL;

    /* -------------------------------------- */




 if ( cacheNode == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
 }

 if ( message == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
 }

 longRet = bmd_smime_create(message);

 if (longRet != 0) {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
 }


 cacheNode_tNode = (cacheNode_t*)(((queueNode_t*)cacheNode)->node);
 attchQueueNode = (cacheNode_tNode->_attachments)->head;

 for(; attchQueueNode != NULL; attchQueueNode=attchQueueNode->prev) {
	attchNode = (attchNode_t*)(attchQueueNode->node);

	if (strcmp(attchNode->disposition, "inline") == 0){
	 	longRet = bmd_smime_add_attachment_from_buf( *message, attchNode->content, attchNode->size, attchNode->filename, BMD_SMIME_HIDDEN_ATTACHMENT );

		if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd przy dołączeniu załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
        		return BMD_ERR_OP_FAILED;
		}
	}
	else{
		longRet = bmd_smime_add_attachment_from_buf( *message, attchNode->content, attchNode->size, attchNode->filename, BMD_SMIME_SHOW_ATTACHMENT );

		if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd przy dołączeniu załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
        		return BMD_ERR_OP_FAILED;
		}
	} 
     }

    return BMD_OK;
}


/**
 * Funkcja pobiera dane odnośnie nadawcy i odbiorcy wiadomości 
 * email.
 *
 * @param[in]  message       - struktura opisująca wiadomość
 * @param[out] sender        - nadawca wiadomości email
 * @param[out] receiver      - odbiorca wiadomości email
 *
 * @retval BMD_ERR_PARAM1    - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2    - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3    - niepoprawny trzeci parametr
 * @retval BMD_ERR_PARAM4    - niepoprawny czwarty parametr
 * @retval BMD_ERR_MEMORY    - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd odczytu nadawcy/odbiorcy 
 *                             wiadomości email.
 *
*/


long getMsgSenderReceiver( xmlDocPtr message, char** const sender, char** const receiver, void* dbHandler )

{

   /* -------------------------------------- */

       long          longRet         =    0;
       char*         stringSender    = NULL;
       char*         stringSQL       = NULL;

    /* ------------------------------------- */


    if (message == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (sender == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    if (receiver == NULL) {
	PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
	return BMD_ERR_PARAM3;
    }

    if (dbHandler == NULL) {
	PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
	return BMD_ERR_PARAM4;
    }

    /* ****************************************************** */
    /*   Pobranie id nadawcy wiadomości email z komunikatu    */
    /* ****************************************************** */

    if (getXpathValue( message, (xmlChar*)"//SENDER", &stringSender) != BMD_OK) {
	PRINT_ERROR("Błąd odczytu identyfikatora nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    /* ****************************************************** */
    /*   Pobranie nadawcy o danym id z bazy                   */
    /* ****************************************************** */

    if ( asprintf( &stringSQL, "SELECT name FROM mailing_message_senders WHERE id = %s;", stringSender ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
    }

    free0(stringSender);

    longRet = ExecuteSQLQueryWithAnswerKnownDBConnection( dbHandler, stringSQL, sender);

    free0(stringSQL);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Błąd odczytu nadawcy wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    /* ***************************************************** */
    /*   Pobranie odbiorcy wiadomości email z komunikatu     */
    /* ***************************************************** */

    if (getXpathValue( message, (xmlChar*)"//RECEIVER", receiver) != BMD_OK) {
	PRINT_ERROR("Błąd odczytu odbiorcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }


    return BMD_OK;
}



/**
 * Funkcja usuwa węzeł wiadomości przechowywanej w kolejce
 *
 * @param[in]  msgNode       - struktura opisująca wiadomość
 *
*/

void DestroyMsgNode( void* msgNode ) {
	free0( msgNode );
}



long prepareAwizo(void* const cacheNode, xmlDocPtr message, void* dbHandler, mail_send_info_t** const mail_info ){


 /* ------------------------------------------- */

     xmlDocPtr                             res;
     xmlDocPtr                             doc;
     xsltStylesheetPtr            templateFile;
     cacheNode_t*       cacheNode_tNode = NULL;
     const char*        params          = NULL;
     long               longRet         =    0;
     bmd_smime_msg*     sMime           = NULL;
     xmlChar*           doc_txt_ptr     = NULL;
     int                doc_txt_len     =    0;
     char*              msgBody         = NULL;
     char*              sender          = NULL;
     char*              receiver        = NULL;
     GenBuf_t*          tresc           = NULL;

 /* ------------------------------------------ */


    if (cacheNode == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (message == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    if (dbHandler == NULL) {
	PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
	return BMD_ERR_PARAM3;
    }

    if (mail_info == NULL) {
	PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
	return BMD_ERR_PARAM4;
    }

    cacheNode_tNode = (cacheNode_t*)(((queueNode_t*)cacheNode)->node);

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;

    doc = xmlParseMemory((const char*)(cacheNode_tNode->xsltohtml), strlen(cacheNode_tNode->xsltohtml));

    if (doc == NULL) {
	PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    templateFile = xsltParseStylesheetDoc(doc);

    if (templateFile == NULL) {
	PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    res = xsltApplyStylesheet( templateFile, message, &params);

    if (res == NULL) {
	PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
        xsltFreeStylesheet(templateFile);
	return BMD_ERR_OP_FAILED;
    }

    if ( xsltSaveResultToString ( &doc_txt_ptr, &doc_txt_len, res, templateFile ) < 0 ) {
        PRINT_ERROR("Błąd zapisu wiadomości do bufora. Error = %d\n", BMD_ERR_OP_FAILED);
        xsltFreeStylesheet(templateFile);
        xmlFreeDoc(res);
        return BMD_ERR_OP_FAILED;
     }

    xmlFreeDoc(res);
    xsltFreeStylesheet(templateFile);

    msgBody=(char *)calloc(doc_txt_len+2, sizeof(char));

    if (msgBody == NULL){
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        free0( doc_txt_ptr );
	return BMD_ERR_MEMORY;
    }

    memmove(msgBody,doc_txt_ptr,doc_txt_len);
    free0( doc_txt_ptr );

    longRet = mailPrepare(cacheNode, &sMime);

    if (longRet != BMD_OK){
	PRINT_ERROR("Błąd dodawania załączników. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    longRet = getMsgSenderReceiver( message, &sender, &receiver, dbHandler );

    if (longRet != BMD_OK){
	PRINT_ERROR("Błąd odczytu nadawcy/odbiorcy eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    longRet = bmd_smime_set_header(sMime, sender, receiver, cacheNode_tNode->title);

    if (longRet != BMD_OK){
	PRINT_ERROR("Nie można dodać nagłówka. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    longRet = set_gen_buf2((char*)msgBody, strlen(msgBody), &tresc);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Nie można wygenerować treści wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    longRet = bmd_smime_set_body(sMime, tresc, BODY_TYPE_TEXT_HTML, BODY_CHARSET_UTF8 );

    if (longRet != BMD_OK) {
	PRINT_ERROR("Nie można dodać treści wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    longRet = bmd_smime_sign(sMime, _GLOBAL_awizoConfig.ctx);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Nie można podpisać wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    *mail_info = (mail_send_info_t*)calloc (1, sizeof(mail_send_info_t));

    if ( *mail_info == NULL ) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
    }

    (*mail_info)->recipient_to = receiver;
    (*mail_info)->reverse_path = sender;

    asprintf( &((*mail_info)->server_port), "%s:%s", _GLOBAL_awizoConfig.smtpAddr, _GLOBAL_awizoConfig.smtpPort );

    (*mail_info)->with_auth=1;

    (*mail_info)->auth_data.username = strdup(_GLOBAL_awizoConfig.user);
    (*mail_info)->auth_data.password = strdup(_GLOBAL_awizoConfig.password);

    (*mail_info)->mail_body= strdup((char*)(sMime->smime->buf));
    (*mail_info)->mail_to_send = 1;

    bmd_smime_destroy(&sMime);


   return BMD_OK;
}



long prepareMessageNode(GenBuf_t inputMsg, message_t** outputMsg){


    if (outputMsg == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    *outputMsg = (message_t*)calloc(1, sizeof(message_t));

    if (*outputMsg == NULL){
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
    }

    memcpy((*outputMsg)->message, inputMsg.buf, inputMsg.size);
    (*outputMsg)->msgSize = inputMsg.size;

    return BMD_OK;

}



long prepareAndSendAwizoSet( void* dbase_handler, char** response ) {


  /* ----------------------------------------- */

     queueNode_t*       iterator   = NULL;
     long               longRet    =    0;
     char*              templateId = NULL;
     void*              template   = NULL;
     mail_send_info_t*  mail_info  = NULL;
     long               i          =   -1;
     xmlDocPtr                   hMessage;
     GenBuf_t                       bufor;

  /* ------------------------------------------ */ 



 for(iterator = _GLOBAL_myXmlQueue.head, i=-1; iterator; iterator=iterator->prev, i++) {

   /* ********************* */
   /* parsowanie komunikatu */
   /* ********************* */

	bufor.buf = ((message_t*)(iterator->node))->message;
	bufor.size = ((message_t*)(iterator->node))->msgSize;

	longRet = MessageParse( &bufor, &hMessage );

  	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd parsowania otrzymanego komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
  	}

   /* ******************** */
   /* pobranie id szablonu */
   /* ******************** */ 

	longRet = getTemplateID( hMessage,  &templateId);

	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd odczytu id szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

   /* ******************** */
   /*  pobranie szablonu   */
   /* ******************** */

	longRet = downloadTemplateID(templateId, &template, dbase_handler);

	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd pobierania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
        }

	free0(templateId);

   /* ******************** */
   /*  tworzenie eAwiza    */
   /* ******************** */

	longRet = prepareAwizo(template, hMessage, dbase_handler, &mail_info );

	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd tworzenia eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
        }

        memcpy(&(_GLOBAL_awizoConfig.awizoTable[i+1]), mail_info, sizeof(mail_send_info_t));

  }

  if ( i > -1) {
	longRet = bmd_send_envoice_avizo(_GLOBAL_awizoConfig.awizoTable, i+1, response);

	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd w trakcie wysyłki eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
        }

	/* ************************** */
	/* zwolnienie struktury maili */
	/* ************************** */

        freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, _GLOBAL_myXmlQueue.ulCount);

  }

  return BMD_OK;

}


long freeMailSendInfo(mail_send_info_t* mi, long const count) {

 /* ----------- */

    long i = 0;

 /* ----------- */


   if ( mi == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
   }

   if ( count < 0 ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
   }

   for (i=0; i<count; i++) {
     free0(mi[i].server_port);
     free0(mi[i].recipient_to);
     free0(mi[i].reverse_path);
     free0(mi[i].recipient_cc);
     free0(mi[i].recipient_bcc);
     free0(mi[i].auth_data.username);
     free0(mi[i].auth_data.password);
     free0(mi[i].mail_body);
   }

 return BMD_OK;

}





long sendAwizo(char const * const awizoText) {


  /* ----------------------------------------- */

     long 	            longRet    =    0;
     char* 	            templateId = NULL;
     void*                  template   = NULL;
     char*                  response   = NULL;
     mail_send_info_t*      mail_info  = NULL;
     void*               dbase_handler = NULL;
     message_t*             xmlMessage = NULL;
     union sigval	                value;
     GenBuf_t		                 plik;
     xmlDocPtr	                     hMessage;

 /* ------------------------------------------ */ 




if ((_GLOBAL_shptr->config).stopSending == 0) {

  plik.buf = (char*)calloc(strlen(awizoText)+2, sizeof(char));
  memcpy(plik.buf, awizoText, strlen(awizoText));
  plik.size = strlen(awizoText);


 /* ****************************************** */
 /*  Parsowanie otrzymanego komunikatu xml     */
 /* ****************************************** */ 

  longRet = MessageParse( &plik, &hMessage );

  if ( longRet != BMD_OK ) {
	PRINT_ERROR("Błąd parsowania otrzymanego komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
  }

 /* *************************************** */
 /*   sprawdzenie poprawności id szablonu   */
 /* *************************************** */

  longRet = getTemplateID( hMessage,  &templateId);

  if ( longRet != BMD_OK ) {
	PRINT_ERROR("Błąd odczytu id szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
  }

 /* ******************************************************** */
 /*   pobranie szablonu z pamięci cache lub z bazy danych    */
 /* ******************************************************** */

 longRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, _GLOBAL_awizoConfig.dbPort, _GLOBAL_awizoConfig.dbName, \
			   _GLOBAL_awizoConfig.dbUser, _GLOBAL_awizoConfig.dbPassword, &dbase_handler);
 
 if ( longRet < 0 ){
	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }


 longRet = downloadTemplateID(templateId, &template, dbase_handler);

  if ( longRet != BMD_OK ) {
	PRINT_ERROR("Błąd pobierania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
  }

  /* ******************************************************** */
  /* Jeżeli kolejkowanie komunikatów jest wyłączone to nie    */
  /* synchronizujemy dostępu do pamięci dzielonej.            */
  /* Awizo jest wysyłane od razu.                             */
  /* ******************************************************** */


  if (_GLOBAL_awizoConfig.mailqueue == 1) {


	/* ******************************************************************** */
	/*   Synchronizacja dosŧepu do zapisu i odczytu komunikatów xmlowych.   */
	/* ******************************************************************** */


	   sem_wait(&(_GLOBAL_shptr->clientMutex));

	   if (ElementCount(&_GLOBAL_myXmlQueue)+1 >= _GLOBAL_awizoConfig.maxmailqueue) {


	   /* ********************************************* */
	   /* zapakowanie komunikatu do struktury message_t */
	   /* ********************************************* */


	      longRet = prepareMessageNode( plik, &xmlMessage);

              if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd inicjowania struktury xmlMessage. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
              }


           /* ***************************** */
           /* dodanie komunikatu do kolejki */
           /* ***************************** */

              InsertToQueue( &_GLOBAL_myXmlQueue, xmlMessage );
 
	   /* **************************************************** */
	   /* uruchomienie procesu wysyłki e-Awiza w jednej sesji  */
	   /* **************************************************** */

	      PRINT_INFO("AWIZOJMSSERVERINF SENT %ld AWIZOS FROM MAILQUEUE.\n", _GLOBAL_myXmlQueue.ulCount );

	      longRet = prepareAndSendAwizoSet( dbase_handler, &response );

		if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd wysyłki zakolejkowanych wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

	      free0(response);

	   /* ************************************* */
	   /*   Wyczyszczenie kolejki wiadomości    */
           /* ************************************* */

              sigqueue( getpid(), SIGRTMIN+7, value);
	   }
	   else {

	   /* ***************************************** */
	   /*   Zapis komunikatu do pamięci dzielonej   */
           /* ***************************************** */

           memcpy((_GLOBAL_shptr->buf).message, plik.buf, plik.size );
           (_GLOBAL_shptr->buf).msgSize = plik.size;

	   /* ***************************************** */
	   /*  Zapis komunikatu do kolejki komunikatów  */
           /* ***************************************** */

	   sigqueue( getpid(), SIGRTMIN+6, value);

	   }
  }
 else {

    longRet = prepareAwizo(template, hMessage, dbase_handler, &mail_info );

    if (longRet != BMD_OK) {
	PRINT_ERROR("Błąd komponowania wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    memcpy(&(_GLOBAL_awizoConfig.awizoTable[0]), mail_info, sizeof(mail_send_info_t));

    longRet = bmd_send_envoice_avizo(_GLOBAL_awizoConfig.awizoTable, 1, &response);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Błąd wysyłania wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    free0(response);

    freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, 1);

  }

 bmd_db_disconnect(&dbase_handler);

 return BMD_OK;

}

 if ((_GLOBAL_shptr->config).stopSending == 1) {
   PRINT_INFO("AWIZOJMSSERVERINF USER STOPED AWIZO SENDING PROCESS.\n" );
   return BMD_OK;
 }

 PRINT_INFO("AWIZOJMSSERVERINF ILLEGAL STOP SENDING PARAM!!\n" );
 return BMD_ERR_OP_FAILED;

}



long mailQueueFunc(void)
{

   /* ----------------------------------------- */

      long          	longElapsedTime =    0;
      struct sigaction      structSigsignTimer;

   /* ----------------------------------------- */


      structSigsignTimer.sa_handler = queueTimer;
      structSigsignTimer.sa_flags = 0;

      sigaction( SIGALRM, &structSigsignTimer, NULL);


      for(;;)
       {
	  longElapsedTime = alarm((_GLOBAL_shptr->config).sendingtime);
          pause();

	  kill( getppid(), SIGALRM );

       }

    return 0;

}

