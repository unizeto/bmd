#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <string.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmddb/libbmddb.h>

#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmd/libbmd_internal.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdcurl/libbmdcurl.h>


/**
 * Funkcja wyszukuje podnapisy pasujące do podanego wzorca
 * 
 * @param[in] string - analizowany łańcuch
 * @param[in] pattern - zadany wzorzec
 * @param[out] stringTable - tablica podnapisów
 *
 * @return status zakończenia 
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błędy PCRE
*
*/

int getSubstring(const char* const string, const char* const pattern, char*** const stringTable)
{


  /* ----------------------------------------- */

     pcre*        re                   = NULL;
     const char*  error                = NULL;
     const char*  stringPtr            = NULL;
     int          erroffset            =    0;
     int          intRet               =    0;
     int          ile                  =    0;
     int          i                    =    0;
     int          ovector[OVECTOR_NUM] =   {};

  /* ----------------------------------------- */
 


   if ( string == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
      return BMD_ERR_PARAM1;
    }

   if ( pattern == NULL )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
      return BMD_ERR_PARAM2;
    }

   if ( stringTable == NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
      return BMD_ERR_PARAM3;
    }

   re = pcre_compile( pattern, 0, &error, &erroffset, NULL);

   if ( re == NULL )
    {
      PRINT_ERROR("Błąd inicjowania PCRE. Error = %d\n", BMD_ERR_OP_FAILED);
      return BMD_ERR_OP_FAILED;
    }

   for( ile=0; (intRet=pcre_exec(re, NULL, string, strlen(string),\
        ovector[1], 0, ovector, OVECTOR_NUM))>0; ile+=intRet);


   *stringTable = ( char** )calloc( ile+1, sizeof( char* ) );

   if ( *stringTable == NULL )
    {
      PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
      return BMD_ERR_MEMORY;
    }

   ovector[1] = 0;

   for( i=0; (intRet=pcre_exec(re, NULL, string, strlen(string),\
        ovector[1], 0, ovector, OVECTOR_NUM))>0; i++)
    {
      intRet = pcre_get_substring( string, ovector, intRet, 0, &stringPtr );
      intRet = asprintf( &((*stringTable)[i]), "%s", stringPtr );

      if ( intRet == -1 )
       {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
       }

      pcre_free_substring( stringPtr );
    }

   pcre_free(re);

 return BMD_OK;

}

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
	PRINT_INFO("Nie odnaleziono węzła o zadanej wartości XPath : '%s'\n", (char*)xpath);
	xmlXPathFreeContext(ctx);
	xmlXPathFreeObject (result);
	return BMD_ERR_OP_FAILED;
}

xmlXPathFreeContext(ctx);
xmlXPathFreeObject (result);

 if (*value == NULL) {
	PRINT_INFO("Nie zdefiniowano wartości dla danego węzła : '%s'\n", (char*)xpath);
	return BMD_ERR_OP_FAILED;
 } 
	 

return BMD_OK;

}


/**
 * Funkcja pobiera wartość atrybutu dla danego tagu XML
 *  
 *
 * @param[in] doc - uchwyt do XMLa
 * @param[in] xpath - xpath
 * @param[in] attrName - nazwa atrybutu
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


long getXpathAttrValue( xmlDocPtr doc, xmlChar* xpath, xmlChar* attrName, char** value ) {


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

if ( attrName == NULL ) {
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

	keyword = xmlGetProp(nodeset->nodeTab[0], attrName);

	if ( keyword != NULL ) {
		asprintf(value, "%s", keyword);
	}
	xmlFree(keyword);
}
else {
	PRINT_INFO("Nie odnaleziono węzła o zadanej wartości XPath : '%s'\n", (char*)xpath);
	return BMD_ERR_OP_FAILED;
}

xmlXPathFreeContext(ctx);
xmlXPathFreeObject (result);

return BMD_OK;

}

/**
 * Funkcja porównuje nazwy przechowywanych ustawień serwera archiwum
 *
 * @param[in]  elem1 -- nazwa pochodząca z kolejki
 * @param[in]  elem2 -- nazwa poszukiwana
 *
*/

long checkServerName(const void* const elem1, \
			const void* const elem2) {

 return  (strcmp(((bmdserver_t*)elem1)->name, (char*)elem2) == 0) ? 1 : 0;

}


/**
 * Funkcja pobiera załączniki z archiwum lub z zewnętrzengo źródła
 *
 * @param[in]  doc -- uchwyt do komunikatu w postaci xml
 * @param[in]  message -- struktura przechowywująca wiadomość email
 *
*/

long getAttachments(xmlDocPtr doc, bmd_smime_msg* const message) {



/* --------------------------------------- */

    xmlXPathObjectPtr   result     = NULL;
    xmlNodeSetPtr       nodeset    = NULL;
    queueNode_t*        queueNode  = NULL;
    xmlChar*            attchSrc   = NULL;
    xmlChar*            attchName  = NULL;
    xmlChar*		attchGroup = NULL;
    xmlChar*            attchValue = NULL;
    xmlChar*            attchBmd   = NULL;
    GenBuf_t*           bmdFile    = NULL;
    long                longRet    = 	0;
    long                size       =    0;
    long                i          =    0;
    xmlXPathContextPtr                ctx;

/* --------------------------------------- */



if ( doc == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
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

result = xmlXPathEvalExpression((xmlChar*)"//ATTACHMENTS/ATTACH", ctx);

if (result && !xmlXPathNodeSetIsEmpty(result->nodesetval)) {
	nodeset = result->nodesetval;
	size = (nodeset) ? nodeset->nodeNr : 0;

	for(i = 0; i < size; ++i) {
		attchSrc = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"type");

		if (attchSrc != NULL) {
			if (strcmp((char*)attchSrc, "bmd") == 0) {

			 /* ********************************** */
			 /*    Pobranie parametrów serwera     */
			 /* ********************************** */

			 attchGroup = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"group");

			 if (attchGroup == NULL){
				PRINT_ERROR("Nie okreslono grupy uzytkownika BMD. Error = %d\n", BMD_ERR_OP_FAILED);
				xmlXPathFreeObject (result);
				xmlXPathFreeContext(ctx);
				xmlFree(attchSrc);
				attchSrc = NULL;
				return BMD_ERR_OP_FAILED; 

			 }

			xmlFree(attchSrc);
			attchSrc = NULL;

			 attchBmd = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"source");

			 if (attchBmd == NULL){
				PRINT_ERROR("Nie okreslono nazwy archiwum. Error = %d\n", BMD_ERR_OP_FAILED);
				xmlXPathFreeObject (result);
				xmlXPathFreeContext(ctx);
				xmlFree(attchGroup);
				attchGroup = NULL;
				return BMD_ERR_OP_FAILED; 

			 }

			 longRet = FindInQueue( &(_GLOBAL_awizoConfig.bmdservers), checkServerName,\
						(char*)attchBmd, (void**)(&queueNode));

			 xmlFree(attchBmd);
			 attchBmd=NULL;

			 /* ********************************** */
			 /*   Funkcja pobrania dokumentu jako  */
			 /*     załącznika z archiwum          */
			 /* ********************************** */

			 attchValue = xmlNodeListGetString( doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			 PRINT_INFO("Import file with given id = %ld\n", atol((char*)attchValue));

			 longRet = getFileByIdFromBAE((bmdserver_t*)(queueNode->node), atol((char*)attchValue), NULL, (char*)attchGroup, &bmdFile);

			xmlFree(attchValue);
			xmlFree(attchGroup);
			attchValue=NULL;
			attchGroup=NULL;

			if ( (longRet != BMD_OK) || bmdFile == NULL ) {
				PRINT_ERROR("Błąd pobierania pliku z archiwum. Error = %d\n", BMD_ERR_OP_FAILED);
				xmlXPathFreeObject (result);
				xmlXPathFreeContext(ctx);
				return BMD_ERR_OP_FAILED; 
			}

			attchName = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"name");

			longRet = bmd_smime_add_attachment_from_buf( message, bmdFile->buf, bmdFile->size, (char*)attchName, BMD_SMIME_SHOW_ATTACHMENT );

			free_gen_buf(&bmdFile);

			if (longRet != BMD_OK) {
				PRINT_ERROR("Błąd przy dołączeniu załącznika : %s. Error = %d\n", (char*)attchName ,BMD_ERR_OP_FAILED);
				xmlXPathFreeObject (result);
				xmlXPathFreeContext(ctx);
				xmlFree(attchName);
				attchName=NULL;
				return BMD_ERR_OP_FAILED;
			}

			xmlFree(attchName);
			attchName=NULL;

			}
			else if (strcmp((char*)attchSrc, "url") == 0) {

				xmlFree(attchSrc);
				attchSrc = NULL;

				attchValue = xmlNodeListGetString( doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);

				longRet = SimpleCurlGet((char*)attchValue, &bmdFile);

				if ( longRet != BMD_OK ) {
					PRINT_ERROR("Błąd pobierania pliku z zadanego adresu : %s. Error = %d\n", (char*)attchValue, BMD_ERR_OP_FAILED);
					xmlXPathFreeObject (result);
					xmlXPathFreeContext(ctx);
					xmlFree(attchValue);
					attchValue = NULL;
					return BMD_ERR_OP_FAILED; 
				}

				xmlFree(attchValue);
				attchValue = NULL;

				attchName = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"name");

				longRet = bmd_smime_add_attachment_from_buf( message, bmdFile->buf, bmdFile->size, (char*)attchName, BMD_SMIME_SHOW_ATTACHMENT );

				xmlFree(attchName);
				attchName = NULL;
				free_gen_buf(&bmdFile);

				if (longRet != BMD_OK) {
					PRINT_ERROR("Błąd przy dołączeniu załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
					xmlXPathFreeObject(result);
					xmlXPathFreeContext(ctx);
					return BMD_ERR_OP_FAILED;
				}


			}
		}

	}
}
else {
	PRINT_INFO("Nie odnaleziono węzła o zadanej wartości XPath : //ATTACHMENTS/ATTACH\n");
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


long getTemplateId(xmlDocPtr const hXmlDoc, void* dbHandler, char** const templateId) {



/* --------------------------------- */

	long  longRet      =    0;
	char* templateName = NULL;
	char* senderName   = NULL;
	char* stringSQL    = NULL;
	char* stringLang   = NULL;
	char* stringLangC  = NULL;
	char* stringTpVer  = NULL;
	char* stringTpVerC = NULL;
	char* stringTmp    = NULL;

/* --------------------------------- */


if ( hXmlDoc == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
}

if ( templateId == NULL || *templateId != NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
}

/* ************************************************ */
/*     Pobranie wartości wymaganych z komunikatu    */
/* ************************************************ */

longRet = getXpathValue( hXmlDoc, (xmlChar*)"//TEMPLATENAME", &templateName);

if ( longRet != BMD_OK ) {
	PRINT_ERROR("Nie określono nazwy szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
}

longRet = getXpathValue( hXmlDoc, (xmlChar*)"//SENDER", &senderName);

if ( longRet != BMD_OK ) {
	PRINT_ERROR("Nie określono nazwy nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(templateName);
	return BMD_ERR_OP_FAILED;
}

/* ************************************************ */
/*     Pobranie wartości opcjonalnych z komunikatu  */
/* ************************************************ */


longRet = asprintf(&stringTmp, "SELECT mailing_email_template.id FROM mailing_message_senders JOIN mailing_email_template ON mailing_message_senders.id = mailing_email_template.messagesenderid WHERE mailing_email_template.deleted is NULL AND mailing_email_template.name = '%s' AND mailing_message_senders.name = '%s'", templateName, senderName);

if (longRet == -1) {

	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	free0(templateName);
	free0(senderName);

	return BMD_ERR_MEMORY;
} 

free0(templateName);
free0(senderName);

getXpathValue( hXmlDoc, (xmlChar*)"//LANGUAGE", &stringLang);


if ( stringLang != NULL) {
	if(asprintf(&stringLangC, "AND mailing_email_template.lang = '%s'", stringLang) == -1) {

		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		free0(stringTmp);
		return BMD_ERR_MEMORY;
	}

	free0(stringLang);
}
else if(asprintf(&stringLangC, " ") == -1) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	
	free0(stringTmp);
	return BMD_ERR_MEMORY;
}

getXpathValue( hXmlDoc, (xmlChar*)"//TEMPLATEVERSION", &stringTpVer);

if ( stringTpVer != NULL) {
	if(asprintf(&stringTpVerC, "AND mailing_email_template.version = '%s'", stringTpVer ) == -1) {

		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		free0(stringLangC);
		free0(stringTmp);
		return BMD_ERR_MEMORY;
	}

	free0(stringTpVer);

}
else if(asprintf(&stringTpVerC, " ") == -1) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	free0(stringLangC);
	free0(stringTmp);
	return BMD_ERR_MEMORY;
}

if(asprintf(&stringSQL, "%s %s %s;", stringTmp, stringLangC, stringTpVerC) == -1) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);

	free0(stringTmp);
	free0(stringLangC);
	free0(stringTpVerC);

	return BMD_ERR_MEMORY;
}

free0(stringTmp);
free0(stringLangC);
free0(stringTpVerC);

longRet = ExecuteSQLQueryWithAnswerKnownDBConnection( dbHandler, stringSQL, templateId);
free0(stringSQL);

if (longRet != BMD_OK) {
	PRINT_ERROR("Błąd identyfikatora szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
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

	long		longRet  = 0;

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
	PRINT_INFO("AWIZOSOAPSERVERINF get template from database\n"); 
	longRet = getDataFromDB( templateId, dbHandler, template );
		if ( longRet != BMD_OK) return longRet;
 }
 else {
	PRINT_INFO("AWIZOSOAPSERVERINF get template from cache\n"); 

 }
	PRINT_INFO("TEMPLATE ID : %s.\n", templateId);

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

   /* --------------------------------------------------- */

	long			longRet         =    0;
	long			rows            =    0;
	long			cols            =    0;
	void*			result          = NULL;
	long			fetched         =    0;
	db_row_strings_t*	rowstruct       = NULL;
	attchNode_t* 		attchNode_tNode = NULL;
	queue_t*		attachments     = NULL;
	cacheNode_t*		cacheNode_tNode = NULL;
	char*			content         = NULL;
	char* 			stringSQL       = NULL;
	size_t			         contentLenght;

   /* --------------------------------------------------- */



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

 if ( asprintf( &stringSQL, "SELECT filename, mimetype, disposition, content FROM mailing_email_attachments WHERE deleted is NULL AND templateid = %s;", templateId ) < 0) {
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
	}
	bmd_db_row_struct_free( &rowstruct );

 }while(longRet == 0);

 bmd_db_result_free( &result );
 fetched = 0;

 /* ******************************************** */
 /*   Pobieranie szablonu z bazy danych          */
 /* ******************************************** */

 if ( asprintf( &stringSQL, "SELECT id,name,lang,version,xsltohtml,xsltotext,sign FROM mailing_email_template WHERE id = %s;", templateId ) < 0) {
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
 //bmd_db_row_struct_free( &rowstruct );
 longRet = bmd_db_result_get_row( dbHandler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

 if ( longRet < 0 ) {
	PRINT_ERROR("Błąd pobierania danych z bazy. Error = %d\n", BMD_ERR_OP_FAILED);
	bmd_db_result_free( &result );
	DestroyQueue( attachments, DestroyAttchNode);
	return BMD_ERR_OP_FAILED;
 }

 bmd_db_result_free( &result );

 if ( _GLOBAL_myCache.ulCount > (_GLOBAL_shptr->config).maxcachesize) {

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

 longRet = CreateCacheNode( &cacheNode_tNode, atol(rowstruct->colvals[0]), rowstruct->colvals[1], \
				rowstruct->colvals[2], rowstruct->colvals[3], rowstruct->colvals[4], \
				rowstruct->colvals[5], atol(rowstruct->colvals[6]), attachments );

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

 PRINT_INFO("AWIZOSOAPSERVERINF count of elements in cache : %ld.\n", _GLOBAL_myCache.ulCount );

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

	  
	  
   /* -------------------------------------- */




 //if ( cacheNode == NULL ) {
//	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
//	return BMD_ERR_PARAM1;
 //}

 if ( message == NULL ) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
 }

 longRet = bmd_smime_create(message);

 if (longRet != 0) {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
 }
  return BMD_OK;
}


/**
 * Pobranie ustawień konfiguracyjnych dla danego wystawcy
 * 
 *
 * @param[in] sender		- nazwa wystawcy
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd pobierania danych z bazy
 *
*/


long getSenderConfiguration(const char* const sender){


  /* ------------------------------------------- */

      char*               stringSQL     = NULL;
      void*               result        = NULL;
      db_row_strings_t*   rowstruct     = NULL;
      long                rows          =    0;
      long                cols          =    0;
      long                intRet        =    0;
      long                fetched       =    0;
      size_t              contentLenght =    0;
      char*               content       = NULL;
      long                longRet       =    0;
      GenBuf_t                        certFile;

  /* ------------------------------------------- */



 if ( sender == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
 }

 if ( (strcmp((_GLOBAL_shptr->config).sender, sender) != 0) || (_GLOBAL_shptr->config).mailqueue == 0 ) {

 	if ( asprintf( &stringSQL, "SELECT smtpaddr, smtpport, smtpuser, smtppassword, pfxfile, pfxpin, maximagesize, awizoaction, allowimageabsent, smsapiurl, smsapierror FROM mailing_message_senders WHERE name = '%s';", sender) < 0) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
 	}

 	if ( bmd_db_execute_sql(_GLOBAL_awizoConfig.dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
		PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
		free0(stringSQL);
		return BMD_ERR_OP_FAILED;
 	} 

	free0(stringSQL);

 	intRet = bmd_db_result_get_row(_GLOBAL_awizoConfig.dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

 	if ( intRet < 0 ) {
		PRINT_ERROR("Błąd pobierania danych z bazy. Error = %d\n", BMD_ERR_OP_FAILED);
		bmd_db_result_free( &result );
		return BMD_ERR_OP_FAILED;
 	}

 	strcpy((_GLOBAL_shptr->config).smtpAddr, rowstruct->colvals[0]);
 	strcpy((_GLOBAL_shptr->config).smtpPort, rowstruct->colvals[1]);
 	strcpy((_GLOBAL_shptr->config).smtpUser, rowstruct->colvals[2]);
 	strcpy((_GLOBAL_shptr->config).smtpPswd, rowstruct->colvals[3]);

 	/* -------------------------- */
 	/*   pfx download             */
 	/* -------------------------- */
 
	bmd_ctx_destroy(&(_GLOBAL_awizoConfig.ctx));
 	contentLenght = strlen(rowstruct->colvals[4]);

	if (contentLenght > 0) {

	 	content = (char*)spc_base64_decode((unsigned char*)(rowstruct->colvals[4]), &contentLenght, 0, (longRet=0, (int*)&longRet));

 		if (longRet != 0) {
			PRINT_ERROR("Błąd dekodowania base64. Error = %ld\n", longRet);
			bmd_db_result_free( &result );
			bmd_db_row_struct_free( &rowstruct );
			return BMD_ERR_OP_FAILED;
 		}

 		certFile.buf = content;
 		certFile.size = contentLenght;
 
	 	intRet = bmd_set_ctx_fileInMem(&certFile, rowstruct->colvals[5], strlen(rowstruct->colvals[5]), &(_GLOBAL_awizoConfig.ctx));

 		if (intRet != BMD_OK) {
			PRINT_ERROR("Błąd w trakcie ustawiania kontekstu do podpisywania wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
			bmd_db_result_free( &result );
			bmd_db_row_struct_free( &rowstruct );
			free0(content);
			return BMD_ERR_OP_FAILED;
 		}

		free0(content);
	}

 	(_GLOBAL_shptr->config).maxImageSize = atoi(rowstruct->colvals[6]);
 	(_GLOBAL_shptr->config).awizoAction = atoi(rowstruct->colvals[7]);
 	(_GLOBAL_shptr->config).allowImageAbsent = atoi(rowstruct->colvals[8]);
 	strcpy((_GLOBAL_shptr->config).sms_api_url, rowstruct->colvals[9]);
	strcpy((_GLOBAL_shptr->config).sms_api_error, rowstruct->colvals[10]);

 	bmd_db_result_free( &result );
 	bmd_db_row_struct_free( &rowstruct );
 }

  return BMD_OK;

}

/**
 * Pobranie informacji odnośnie nadawcy i odbiorcy wiadomoci email
 * 
 *
 * @param[in] message           - struktura opisująca wiadomość
 * @param[in] dbHandler		- uchwyt do bazy danych
 * @param[out] sender		- nadawca wiadomości
 * @param[out] receiver		- odbiorca wiadomości
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd przy dołączeniu załącznika
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
    /*   Pobranie nazwy nadawcy wiadomości email z komunikatu    */
    /* ****************************************************** */

    if (getXpathValue( message, (xmlChar*)"//SENDER", &stringSender) != BMD_OK) {
	PRINT_ERROR("Błąd odczytu nazwy nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    if (getSenderConfiguration(stringSender) != BMD_OK) {
	PRINT_ERROR("Błąd odczytu ustawień konfiguracyjnych z bazy danych. Error = %d\n", BMD_ERR_OP_FAILED );
	return BMD_ERR_OP_FAILED;
    }

    strcpy((_GLOBAL_shptr->config).sender, stringSender);

    /* ****************************************************** */
    /*   Pobranie adresu email nadawcy o zadanym name z bazy  */
    /* ****************************************************** */

    if ( asprintf( &stringSQL, "SELECT email FROM mailing_message_senders WHERE name = '%s';", stringSender ) < 0) {
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

    if (getXpathValue( message, (xmlChar*)"//RECIPIENT_EMAIL", receiver) != BMD_OK) {
	PRINT_INFO("Błąd odczytu odbiorcy wiadomości email\n");
	return BMD_ERR_OP_FAILED;
    }


    return BMD_OK;
}




/**
 * Funkcja dokonuje ewentualnej zmiany roli i grupy po stronie
 * archiwum
 *
 * @param[in] roleId		- identyfikator roli
 * @param[in] groupId		- identyfikator grupy
 * @param[out] bi		- struktura PKI
 * @param[out] deserialisation_max_memory - maksymalny rozmiar pamięci dla deserializacji
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd przy dołączeniu załącznika
 *
*/


long chooseClientRole(	char *roleId,
			char *groupId,
			bmd_info_t **bi,
			long deserialisation_max_memory)
{
GenBuf_t *der_form			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (	(roleId==NULL) &&
		(groupId==NULL))	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	/**********************/
	/* stworzenie zadania */
	/**********************/
	BMD_FOK_NP(bmd_datagram_create(BMD_DATAGRAM_TYPE_CHOOSE_ROLE,&dtg));

	if (roleId!=NULL)
	{
		BMD_FOK_NP(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_ROLE_ID, roleId, &dtg));
	}

	if (groupId!=NULL)
	{
		BMD_FOK_NP(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_GROUP_ID, groupId, &dtg));
	}

	BMD_FOK_NP(bmd_datagram_add_to_set(dtg,&dtg_set));

	/********************/
	/* wyslanie zadania */
	/********************/
	BMD_FOK_NP(bmd_send_request(*bi,dtg_set, 100, &resp_dtg_set,1, deserialisation_max_memory));

	/************************/
	/* odebranie odpowiedzi */
	/************************/
	BMD_FOK_NP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/******************************************/
	/* pobranie nowego formularza z datagramu */
	/******************************************/
	free_BMD_attr_list(&((*bi)->form_info.user_form));
	free_BMD_attr_list(&((*bi)->form_info.user_send_form));
	free_BMD_attr_list(&((*bi)->form_info.user_search_form));
	free_BMD_attr_list(&((*bi)->form_info.user_view_form));
	free_BMD_attr_list(&((*bi)->form_info.user_unvisible_form));
	free_BMD_attr_list(&((*bi)->form_info.user_update_form));
	free_BMD_attr_list(&((*bi)->form_info.user_history_form));

	BMD_FOK(bmd2_datagram_get_form(resp_dtg_set->bmdDatagramSetTable[0], &der_form));
	BMD_FOK(bmd_kontrolki_deserialize(der_form, &((*bi)->form_info.user_form)));

	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), SEND_KONTROLKI, &((*bi)->form_info.user_send_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), GRID_KONTROLKI, &((*bi)->form_info.user_view_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), SEARCH_KONTROLKI, &((*bi)->form_info.user_search_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), UNVISIBLE_KONTROLKI, &((*bi)->form_info.user_unvisible_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), UPDATE_KONTROLKI, &((*bi)->form_info.user_update_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), HISTORY_KONTROLKI, &((*bi)->form_info.user_history_form)));

	/************/
	/* porzadki */
	/************/
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&der_form);

	return BMD_OK;

}






/**
 * Funkcja pobiera dokument z archiwum na podstawie jego id
 *
 * @param[in] srvParam		- parametry serwera BMD
 * @param[in] id		- id pliku do pobrania
 * @param[in] roleId		- identyfikator roli
 * @param[in] groupId		- identyfikator grupy
 * @param[out] message		- sparsowany dokument xml
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd przy dołączeniu załącznika
 *
*/


long getFileByIdFromBAE(bmdserver_t* srvParam, \
			long id, \
			char *roleId, \
			char *groupId, \
			GenBuf_t** gb)
{

/* ------------------------------------------------- */

    long max_datagram_in_set_transmission   =  100;
    long deserialisation_max_memory         =    0;
    bmd_info_t *bi                          = NULL;
    long intRet				   =    0;
    GenBuf_t* cert_owner                    = NULL;
    lob_request_info_t* li                  = NULL;
    GenBuf_t *cert_login_as2                = NULL;
    char* path                              = NULL;
    bmdDatagram_t* dtg                      = NULL;
    bmdDatagramSet_t* dtg_set               = NULL;
    bmdDatagramSet_t* resp_dtg_set          = NULL;
    GenBuf_t                         cert_login_as;
 
/* ------------------------------------------------- */


 /************************************/
 /*     stworzenie struktury BMD     */
 /************************************/


  intRet=bmd_info_create(&bi);

  if(intRet < BMD_OK) {
	PRINT_ERROR("Błąd tworzenia struktury wymiany komunikatów. Error = %d\n", BMDSOAP_SERVER_BMD_INIT_ERROR);
	bmd_end();
	return BMDSOAP_SERVER_BMD_INFO_CREATE_ERROR;
  }


 /******************************************/
 /*   przygotowanie zestawu do logowania   */
 /******************************************/

  intRet=bmd_info_login_set(srvParam->addr, atoi(srvParam->port),NULL,-1,NULL,NULL,BMDNET_PROXY_NONE,NULL,&bi);

  if(intRet < BMD_OK) {
	PRINT_ERROR("Błąd tworzenia struktury logowania. Error = %d\n", BMDSOAP_SERVER_LOGIN_SET_ERROR);
	bmd_end();
	return BMDSOAP_SERVER_LOGIN_SET_ERROR;
  }

 /******************************************************/
 /*  przygotowanie struktur certyfikatu logujacego     */
 /******************************************************/

 intRet=bmd_info_set_credentials_pfx(srvParam->pfx,srvParam->pfx_pass, strlen(srvParam->pfx_pass),&bi);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Błąd ustawień uprawnień. Error = %d\n", BMDSOAP_SERVER_PFX_SET_ERROR);
	bmd_end();
	return BMDSOAP_SERVER_PFX_SET_ERROR;
 }

 intRet=bmd_info_set_bmd_cert(srvParam->bmd_cert,&bi);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Błąd ustawień certyfikatu serwera. Error = %d\n", BMDSOAP_SERVER_BMD_CERT_SET_ERROR);
	bmd_end();
	return BMDSOAP_SERVER_BMD_CERT_SET_ERROR;
 }

 intRet = bmd_load_binary_content(srvParam->der,&cert_login_as2);
 cert_login_as.buf=(char *)cert_login_as2->buf;
 cert_login_as.size=cert_login_as2->size; 

 if(cert_login_as.buf == NULL || cert_login_as.size <= 0) {
	PRINT_ERROR("Get owner from certificate error. Error %li\n", intRet );
        bmd_end();
        return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
 }

 /******************************************/
 /*    wydobycie podmiotu z certyfikatu    */
 /******************************************/

 intRet=get_owner_from_cert(&cert_login_as,&cert_owner);

 if(intRet < BMD_OK) {
 	PRINT_ERROR("Get owner from certificate error. Error %li\n", intRet);
        bmd_end();
	return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
 }

 PRINT_INFO("User:\t\t%s\n",cert_owner->buf);

 intRet=bmd_info_set_cert_login_as(&cert_login_as,&bi);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Cert login failed. Error %li\n", intRet);
	bmd_end();
	return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
 }

 /******************************/
 /* zalogowanie do serwera BMD */
 /******************************/

 intRet = bmd_login(&bi);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Login failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /************************************/
 /* ewentualna zmiana grupy lub roli */
 /************************************/

 if ((roleId!=NULL) || (groupId!=NULL)) {
	intRet = chooseClientRole( roleId, groupId, &bi, deserialisation_max_memory);

	if(intRet < BMD_OK) {
		bmd_end();
		return intRet;
	}
 }

 /******************************/
 /*    stworzenie zadania      */
 /******************************/

 intRet = bmd_lob_request_info_create(&li);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Request create failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 asprintf(&path,"soap_get_file_by_id_%li.tmp",(long)getpid());

 intRet =  bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Filename create failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /******************************/
 /*    stworzenie datagramu    */
 /******************************/

 intRet = bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Datagram create failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /************************************/
 /*      usatwienie id w datagramie  */
 /************************************/

 intRet = bmd_datagram_set_id((const long *)&id,&dtg);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Datagram set id failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /************************************************/
 /*   dodanie datagramu do zestawu datagramow    */
 /************************************************/

 intRet = bmd_datagram_add_to_set(dtg,&dtg_set);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Add datagram to set failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /************************************/
 /*    wyslanie zadania lobowego     */
 /************************************/

 intRet = bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);


 if(intRet < BMD_OK) {
	PRINT_ERROR("Bmd request failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }


 if(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus < BMD_OK) {
	PRINT_ERROR("Bmd request failed. Error %li\n", resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus );
	bmd_end();
        return intRet;
 }

/*
 intRet = bmd_load_binary_content(path,&gb);
*/

intRet = bmd_load_binary_content(path,gb);

 if(intRet < BMD_OK) {
	PRINT_ERROR("Fetch content failed. Error %li\n", intRet);
	bmd_end();
        return intRet;
 }

 /******************/
 /*    porzadki    */
 /******************/

 bmd_logout(&bi, deserialisation_max_memory);
 bmd_info_destroy(&bi);

 unlink(path);
 free0(path);

 bmd_datagramset_free(&dtg_set);
 bmd_datagramset_free(&resp_dtg_set);
 bmd_lob_request_info_destroy(&li);

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



/**
 * Funkcja przygotowuje awizo do wysyłki
 *
 * @param[in] cacheNode		- węzeł pamięci cache, w P1 NULL
 * @param[in] message		- parsowana wiadomość
 * @param[in] dbHandler		- uchwyt do bazy danych
 * @param[in] mail_info		- struktura przechowywująca maile
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd przy dołączeniu załącznika
 *
*/



long prepareAwizo(void* const cacheNode, xmlDocPtr message, void* dbHandler, \
			char* const msgID, mail_send_info_t** const mail_info ){

printf("prepareAwizo\n");
 /*  ------------------------------------------ */

     cacheNode_t*       cacheNode_tNode = NULL;
   
     long               longRet         =    0;
   
   
     int                doc_txt_len     =    0;
     char*              msgBodyTmp      = NULL;
	 char*              msgBody         = NULL;
     char*              sender          = NULL;
     char*              receiver        = NULL;
     GenBuf_t*          tresc           = NULL;
     char*              attrValue       = NULL; 
     char*              title			= NULL;
     bmd_smime_msg*     sMime           = NULL;
     xmlDocPtr                             doc;

 /*  ------------------------------------------ */

	// w P1 cacheNode ma warttość NULL ponieważ nie korzystamy z szablonów
    //if (cacheNode == NULL) {
	//	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	//	return BMD_ERR_PARAM1;
    //}

    if (message == NULL) {
		PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
    }

    if (dbHandler == NULL) {
		PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
    }

    if (msgID == NULL) {
		PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
    }

    if (mail_info == NULL) {
		PRINT_ERROR("Niepoprawny piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
    }

    //cacheNode_tNode = (cacheNode_t*)(((queueNode_t*)cacheNode)->node);

    longRet = getXpathValue( message, (xmlChar*)"//METADATA", &msgBodyTmp );

    if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd odczytu wartości atrybutu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }

	longRet = getXpathAttrValue( message, (xmlChar*)"//METADATA", (xmlChar*)"source_type", &attrValue );

    if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd odczytu wartości atrybutu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }

	if (strcmp(attrValue, "inline") != 0){
		PRINT_ERROR("Nie rozpoznany rodzaj zrodla metadanych. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }

	doc_txt_len = strlen(msgBodyTmp);
	
    msgBody=(char *)calloc(doc_txt_len+2, sizeof(char));

    if (msgBody == NULL){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        free0( msgBodyTmp );
		return BMD_ERR_MEMORY;
    }

    memmove(msgBody,msgBodyTmp,doc_txt_len);
 	
    /* ************************************ */
    /*   Dodanie załączników typu inline    */
    /*           (not visible)              */
    /* ************************************ */

    longRet = mailPrepare(cacheNode, &sMime);

    if (longRet != BMD_OK){
		PRINT_ERROR("Błąd przygotawania wiadomości / załączników jawnych. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }
	
    longRet = getMsgSenderReceiver( message, &sender, &receiver, dbHandler );


	if (longRet != BMD_OK){
		PRINT_ERROR("Błąd odczytu nadawcy/odbiorcy eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }

    getXpathValue( message, (xmlChar*)"//TITLE", &title);

    
	if (title == NULL) {
    	longRet = bmd_smime_set_header2(sMime, sender, receiver, "Awizo", msgID);
    }
    else {
		longRet = bmd_smime_set_header2(sMime, sender, receiver, title, msgID);
    }

    if (longRet != BMD_OK){
		PRINT_ERROR("Nie można dodać nagłówka. Error = %d\n", BMD_ERR_OP_FAILED);

		return BMD_ERR_OP_FAILED;
    }

    longRet = set_gen_buf2((char*)msgBody, strlen(msgBody), &tresc);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Nie można wygenerować treści wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);

	free0(msgBody);
	return BMD_ERR_OP_FAILED;
    }

    free0(msgBody);

    longRet = bmd_smime_set_body(sMime, tresc, BODY_TYPE_TEXT_PLAIN, BODY_CHARSET_UTF8 );

    if (longRet != BMD_OK) {
	PRINT_ERROR("Nie można dodać treści wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
	
	return BMD_ERR_OP_FAILED;
    }

    free_gen_buf(&tresc);

    /* ********************************************* */
    /*     Dokonanie sprawdzenia czy podpisywać      */
    /*                  czy tez nie.                 */
    /* W P1 nie podpisujemy, jak coś się zmieni TODO */
    /* ********************************************* */

		longRet = bmd_smime_noSign(sMime);
		if (longRet != 0) {
			PRINT_ERROR("Błąd komponowania wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
			
			return BMD_ERR_OP_FAILED;
        } 

    *mail_info = (mail_send_info_t*)calloc(1, sizeof(mail_send_info_t));

    if ( *mail_info == NULL ) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		
		return BMD_ERR_MEMORY;
    }

    (*mail_info)->recipient_to = receiver;
    (*mail_info)->reverse_path = sender;

    asprintf(&((*mail_info)->server_port), "%s:%s", (_GLOBAL_shptr->config).smtpAddr, (_GLOBAL_shptr->config) .smtpPort);

    (*mail_info)->with_auth=1;

    (*mail_info)->auth_data.username = strdup((_GLOBAL_shptr->config).smtpUser);
    (*mail_info)->auth_data.password = strdup((_GLOBAL_shptr->config).smtpPswd);

    (*mail_info)->mail_body= strdup((char*)(sMime->smime->buf));
    (*mail_info)->mail_to_send = 1;

    bmd_smime_destroy(&sMime);

    free0(title);

   return BMD_OK;
}


/**
 * Funkcja przygotowuje identyfikator wiadomości
 *
 * @param[in] hMessage - uchwyt do przetwarzanej wiadomości
 * @param[out] msgID - skomponowany identyfikator wiadomości 
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd komponowania identyfikatora wiadomości
 *
*/


long prepareMessageId(xmlDocPtr hMessage, char** const msgID) {

printf("prepareMessageId\n");
 /* ------------------------------- */

    long                     value;
    struct timeval             now;
    struct tm                 czas;
    char*           sender  = NULL;
    char*     templateName  = "Testowa nazwa nigdy nieuzytej formatki";
    long            longRet =    0;

 /* -------------------------------- */

    if (hMessage == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (msgID == NULL || *msgID != NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    gettimeofday(&now,NULL);
    localtime_r(&(now.tv_sec),&czas);
    srand(now.tv_usec+getpid()+now.tv_sec);
    value=(1+(long) (100.0*rand()/(RAND_MAX+1.0)))*1000+czas.tm_sec*100+getpid()*10+now.tv_usec; 


    /*************************************************/
    /*  Pobranie nazw wystawcy i wybranego szablonu  */
    /*************************************************/

    longRet = getXpathValue( hMessage, (xmlChar*)"//SENDER", &sender);

     if ( longRet != BMD_OK ) {
		PRINT_ERROR("Nie określono nazwy wystawcy. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
     }

     //longRet = getXpathValue( hMessage, (xmlChar*)"//TEMPLATENAME", &templateName);

     //if ( longRet != BMD_OK ) {
	//PRINT_ERROR("Nie określono nazwy szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	//free0(sender);
	//return BMD_ERR_OP_FAILED;
     //}

     asprintf( msgID, "%s#%s#%ld.%ld.%ld", sender, templateName, value, now.tv_sec, now.tv_usec );

     free0(sender);
     //free0(templateName);
printf("prepareMessageId END\n");
    return BMD_OK;

}








/**
 * Funkcja nadaje identyfikator dla otrzymanego komunikatu 
 *
 * @param[in] hMessage - uchwyt do przetwarzanej wiadomości
 * @param[out] msgID - skomponowany identyfikator wiadomości 
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd komponowania identyfikatora wiadomości
 *
*/



long createMessageId(xmlDocPtr hMessage, message_t* const outputMsg ){

 /* ------------------- */

    char* msgID = NULL;

 /* ------------------- */

    if (hMessage == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (outputMsg == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    prepareMessageId(hMessage, &msgID);
	*msgID = "1";
    memcpy(outputMsg->messageID, msgID, strlen(msgID));
    free0(msgID);

    return BMD_OK; 

}


/**
 * Przygotowanie węzła komunikatu przed umieszczeniem go w kolejce 
 *
 * @param[in] inputMsg - bufor przechowywujący komunikat
 * @param[in]  hMessage - uchwyt do sparsowanego komunikatu xml
 * @param[out] outputMsg - struktura przechowywująca przygotowywaną
 *                         wiadomość
 *
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd komponowania identyfikatora wiadomości
 *
*/


long prepareMessageNode(GenBuf_t inputMsg, xmlDocPtr hMessage, message_t** outputMsg){

 /* --------------------- */
 
    long  longRet =    0;

 /* --------------------- */


    if (hMessage == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    if (outputMsg == NULL) {
	PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
	return BMD_ERR_PARAM3;
    }

    *outputMsg = (message_t*)calloc(1, sizeof(message_t));

    if (*outputMsg == NULL){
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
    }

    memcpy((*outputMsg)->message, inputMsg.buf, inputMsg.size);
    (*outputMsg)->msgSize = inputMsg.size;


    longRet = createMessageId(hMessage, *outputMsg );

    if (longRet != BMD_OK){
	PRINT_ERROR("Błąd nadawania identyfikatora komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }


    return BMD_OK;

}


/**
 * Zapis raportu odnośnie danego komunikatu w bazie danych
 *
 * @param[in]
 * @param[out]
 *
 *
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd komponowania identyfikatora wiadomości
 *
*/



long saveRaportToDb(xmlDocPtr msg, void* dbHandler,\
			char* msgID,\
			long templateID,\
			long composition_status,\
			char* awizo_info,\
			long msg_type)
{


/* ------------------------------ */

    long   longRet        =    0;
    char*  sender         = NULL;
    char*  recipient	  = NULL;
    char*  templateName   = NULL;
    char*  seq_next_val   = NULL;
    char*  stringSQL      = NULL;
    void*  query_result   = NULL;

/* ------------------------------ */




    if (msg == NULL) {
	PRINT_ERROR("Niepoprawny uchwyt wiadomosci. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (dbHandler == NULL) {
	PRINT_ERROR("Niepoprawny uchwyt do bazy danych. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    if (msgID == NULL) {
	PRINT_ERROR("Niepoprawny identyfikator wiadomosci. Error = %d\n", BMD_ERR_PARAM3);
	return BMD_ERR_PARAM3;
    }

    if (awizo_info == NULL) {
	PRINT_ERROR("Niepoprawny adres zmiennej. Error = %d\n", BMD_ERR_PARAM6);
	return BMD_ERR_PARAM6;
    }


    /* **************************** */
    /*  Pobierz nadawce wiadomości  */
    /* **************************** */


    longRet = getXpathValue( msg, (xmlChar*)"//SENDER", &sender);

    if ( longRet != BMD_OK ) {
	PRINT_ERROR("Nie określono nadawcy w komunikacie. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }


    /* ************************* */
    /*  Pobierz nazwę szablonu   */
    /* ************************* */

    longRet = getXpathValue( msg, (xmlChar*)"//TEMPLATENAME", &templateName);

    if ( longRet != BMD_OK ) {
	PRINT_ERROR("Nie określono odbiorcy w komunikacie. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(sender);
	return BMD_ERR_OP_FAILED;
    }

    /* ***************************** */
    /*  Pobierz adresata wiadomości  */
    /* ***************************** */

    if (msg_type == modeEMAIL) {

	//seq_next_val = bmd_db_get_sequence_nextval( dbHandler, "raports_id_seq");

	//if ( seq_next_val == NULL ) {
	//	PRINT_ERROR("Błąd odczytu kolejnego indeksu. Error = %d\n", BMD_ERR_OP_FAILED);
	//	free0(sender);
	//	free0(templateName);
	//	return BMD_ERR_OP_FAILED;
	//}

    	longRet = getXpathValue( msg, (xmlChar*)"//RECIPIENT_EMAIL", &recipient);

    	if (recipient == NULL) {
		asprintf(&recipient, "UNKNOWN");
    	}

	if (asprintf( &stringSQL, "INSERT INTO raports(msgid, msg_sender, msg_recipient, composition_status, template_name, msg_type, awizo_info, template_id) VALUES('%s', '%s', '%s', %ld, '%s', %d, '%s', %ld);", msgID, sender, recipient, composition_status, templateName, modeEMAIL, awizo_info, (long)templateID) < 0 ) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
    	}

        longRet = bmd_db_execute_sql( dbHandler, stringSQL,NULL,NULL, &query_result);

        free0(stringSQL);
	free0(recipient);
	free0(seq_next_val);
	bmd_db_result_free(&query_result);

    	if (longRet != BMD_OK){
		PRINT_ERROR("Błąd zapisu danych raportu do bazy. Error = %ld\n", longRet);
		return longRet;
    	}

    } 

 else if (msg_type == modeSMS) {

	//seq_next_val = bmd_db_get_sequence_nextval( dbHandler, "raports_id_seq");

	//if ( seq_next_val == NULL ) {
	//	PRINT_ERROR("Błąd odczytu kolejnego indeksu. Error = %d\n", BMD_ERR_OP_FAILED);
	//	free0(sender);
	//	free0(templateName);
	//	return BMD_ERR_OP_FAILED;
	//}

	longRet = getXpathValue( msg, (xmlChar*)"//RECIPIENT_SMS", &recipient);

    	if (recipient == NULL) {
		asprintf(&recipient, "UNKNOWN");
    	}

	if (asprintf( &stringSQL, "INSERT INTO raports(msgid, msg_sender, msg_recipient, composition_status, template_name, msg_type, awizo_info, template_id) VALUES('%s', '%s', '%s', %ld, '%s', %d, '%s', %ld);", msgID, sender, recipient, composition_status, templateName, modeSMS, awizo_info, templateID) < 0 ) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
    	}

        longRet = bmd_db_execute_sql( dbHandler, stringSQL,NULL,NULL, &query_result);

        free0(stringSQL);
	free0(recipient);
	free0( seq_next_val );
	bmd_db_result_free(&query_result);

    	if (longRet != BMD_OK){
		PRINT_ERROR("Błąd zapisu danych raportu do bazy. Error = %ld\n", longRet);
		return longRet;
    	}

    }
 else {

	//seq_next_val = bmd_db_get_sequence_nextval( dbHandler, "raports_id_seq");

	//if ( seq_next_val == NULL ) {
	//	PRINT_ERROR("Błąd odczytu kolejnego indeksu. Error = %d\n", BMD_ERR_OP_FAILED);
	//	free0(sender);
	//	free0(templateName);
	//	return BMD_ERR_OP_FAILED;
	//}

	if (asprintf( &stringSQL, "INSERT INTO raports(msgid, msg_sender, msg_recipient, composition_status, template_name, msg_type, awizo_info, template_id) VALUES('%s', '%s', '%s', %ld, '%s', %d, '%s', %ld);", msgID, sender, "UNKNOWN", composition_status, templateName, modeNONE, awizo_info, (long)templateID) < 0 ) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
    	}

        longRet = bmd_db_execute_sql( dbHandler, stringSQL,NULL,NULL, &query_result);

        free0(stringSQL);
	free0(seq_next_val);
	bmd_db_result_free(&query_result);

    	if (longRet != BMD_OK){
		PRINT_ERROR("Błąd zapisu danych raportu do bazy. Error = %ld\n", longRet);
		return longRet;
    	}

 }

    free0(sender);
    free0(templateName);

    return BMD_OK;

}


long updateRaportInDb( void* dbHandler,\
			char* msgID,\
			awizo_mode_t mode,
			char* awizo_info,\
			char sending_status)
{

/* ---------------------------- */

    long   longRet      =    0;
    char*  sendingTime  = NULL;
    long   attemptCount =    0;
    char*  stringSQL    = NULL;
    char*  value        = NULL;
    void*  query_result = NULL;
    struct timeval          tv;
    struct tm*          tm_ptr;

/* ---------------------------- */




    if (dbHandler == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    if (msgID == NULL) {
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }

    if ( asprintf( &stringSQL, "SELECT attempt_of_sending_awizo FROM raports WHERE msgid='%s' AND msg_type=%d;", msgID, mode) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
    }

    longRet = ExecuteSQLQueryWithAnswerKnownDBConnection( dbHandler, stringSQL, &value);
    free0(stringSQL);

    if (longRet != BMD_OK) {
	PRINT_ERROR("Błąd odczytu danych raportu z bazy. Error = %d\n", BMD_ERR_NODATA);
	free0(value);
	return BMD_ERR_NODATA;
    }

    attemptCount = atol(value);
    free0(value);

    gettimeofday(&tv, 0);
    tm_ptr = (struct tm*)localtime(&(tv.tv_sec));

    asprintf( &sendingTime, "%04d-%02d-%02d %02d:%02d:%02d.%06li", 1900+tm_ptr->tm_year, \
                            tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, \
                            tm_ptr->tm_sec,(long)tv.tv_usec );

    if (asprintf( &stringSQL, "UPDATE raports SET sending_status=%c,sending_time='%s',attempt_of_sending_awizo=%ld, awizo_info='%s' WHERE msgid='%s' AND msg_type=%d;", sending_status, sendingTime, ++attemptCount, awizo_info, msgID, mode) < 0 ) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	free0(sendingTime);
	return BMD_ERR_MEMORY;
    }

    free0(sendingTime);

    longRet = bmd_db_execute_sql( dbHandler, stringSQL,NULL,NULL, &query_result);

    bmd_db_result_free( &query_result );
    free0(stringSQL);
    free0(value);


    if (longRet != BMD_OK){
	PRINT_ERROR("Błąd zapisu danych raportu do bazy. Error = %d\n", BMD_ERR_NODATA);
	return BMD_ERR_NODATA;
    }

    return BMD_OK;

}


/**
 * Przygotowanie i wysyłka awiz zgromadzonych w kolejce 
 *
 * @param[in]  dbase_handler - uchwyt do bazy danych
 * @param[out] response      - tablica ze statusami przetworzonych
 *                             awiz
 *
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED - błąd komponowania identyfikatora wiadomości
 *
*/



long prepareAndSendAwizoSet( void* dbase_handler, char** response ) {


  /* ------------------------------------------ */

     queueNode_t*       iterator        = NULL;
     long               longRet         =    0;
     long 		smsSendStatus   =    0;
     //long 		emailSendStatus =    0;
     char*              templateId      = NULL;
     void*              template        = NULL;
     mail_send_info_t*  mail_info       = NULL;
     char*		msgID	        = NULL;
     long               i               =   -1;
     xmlDocPtr                        hMessage;
     GenBuf_t                            bufor;
     awizo_mode_t            mode =  modeEMAIL;	/* domyslnie wysylka emaili, nie smsow */

  /* ------------------------------------------ */ 



  /* Alokacja tablicy maili */

  _GLOBAL_awizoConfig.awizoTable = (mail_send_info_t*)calloc((_GLOBAL_shptr->config).maxmailqueue, sizeof(mail_send_info_t));

  if (_GLOBAL_awizoConfig.awizoTable == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
  }

 //for(iterator = _GLOBAL_myXmlQueue.head, i=-1; iterator; iterator=iterator->prev, i++) {
 for(iterator = _GLOBAL_myXmlQueue.head, i=-1; iterator; iterator=iterator->prev) {


   /* ********************* */
   /* parsowanie komunikatu */
   /* ********************* */

	bufor.buf = ((message_t*)(iterator->node))->message;
	bufor.size = ((message_t*)(iterator->node))->msgSize;
	msgID = ((message_t*)(iterator->node))->messageID;

	longRet = MessageParse( &bufor, &hMessage );

  	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd parsowania otrzymanego komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
		PRINT_INFO("MsgID : %s.\n", msgID);
		continue;
  	}

  /* **************************************** */ 
  /*  sprawdzenie trybu pracy wysylki awiz    */
  /* **************************************** */

   mode = getAwizoMode(hMessage);

   if (mode == modeNONE){
	PRINT_ERROR("Nie określono trybu pracy eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
	PRINT_INFO("MsgID : %s\n", msgID);

	saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, msgID, -1, -1, "Awizo mode not selected", modeNONE);
	xmlFreeDoc(hMessage);
	continue;
   }


       if (mode == modeSMS || mode == modeSMSEMAIL) {
		saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, -1, atol(templateId), 1, "Prepared successfully", modeSMS);

		smsSendStatus = sendSMS(template, hMessage, _GLOBAL_awizoConfig.dbase_handler, -1);

		if (smsSendStatus != BMD_OK) {
			PRINT_ERROR("Błąd wysyłki wiadomości SMS. Error = %d\n", BMD_ERR_OP_FAILED);
			PRINT_INFO("MsgID : %s.\n", msgID);

			updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, -1, modeSMS, "SMS send error", '0');
		}
		else {
			/* ********************************************* */
			/*  Zapis statusu po prawidłowej wysyłce SMSów   */
			/* ********************************************* */

			updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, -1, modeSMS, "SMS was successfully send", '1');
		}

	}

    /* ************************************* */
    /*    tylko w przypadku wysylki maili    */
    /* ************************************* */

   if (mode == modeEMAIL || mode == modeSMSEMAIL) {

   /* ******************** */
   /*  tworzenie eAwiza    */
   /* ******************** */

	longRet = prepareAwizo(template, hMessage, dbase_handler, -1, &mail_info );

	if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd tworzenia eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
		PRINT_INFO("MsgID : %s.\n", msgID);
		saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, -1, atol(templateId), longRet, "eAwizo prepared failed", modeEMAIL);
		free0(templateId);
     		xmlFreeDoc(hMessage);
     		template = NULL;
		continue;

        }
	else {
   		/* ******************************************** */
   		/*  Zapisz status o prawidłowym skomponowaniu   */
   		/*   eAwiza do tabeli raportów bazy danych      */
   		/* ******************************************** */

        	saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, -1, atol(templateId), 1, "Prepared successfully", modeEMAIL);

        	memcpy(&(_GLOBAL_awizoConfig.awizoTable[i+1]), mail_info, sizeof(mail_send_info_t));
		i++;
	}
   }

     free0(templateId);
     xmlFreeDoc(hMessage);
     template = NULL;
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

  free0(_GLOBAL_awizoConfig.awizoTable);

  for(iterator = _GLOBAL_myXmlQueue.head, i=0; iterator; iterator=iterator->prev) {
	bufor.buf = ((message_t*)(iterator->node))->message;
	bufor.size = ((message_t*)(iterator->node))->msgSize;
	msgID = ((message_t*)(iterator->node))->messageID;

	longRet = MessageParse( &bufor, &hMessage );

  	if ( longRet == BMD_OK ) {
		mode = getAwizoMode(hMessage);

		if (mode == modeEMAIL || mode == modeSMSEMAIL){
			updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, msgID, modeEMAIL, ((*response)[i]=='1') ? "eAwizo send successfully" : "eAwizo send failed", (*response)[i]);
			i++;
		}
  	}

	xmlFreeDoc(hMessage);
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



long mailQueueTimerFunc(void)
{

   /* ----------------------------------------- */

      long          	longElapsedTime =    0;
      struct sigaction      structSigsignTimer;
      sigset_t 				  iset;

   /* ----------------------------------------- */

	

      sigfillset(&iset);
      structSigsignTimer.sa_handler = queueTimer;
      structSigsignTimer.sa_flags = SA_SIGINFO | SA_RESTART;
      structSigsignTimer.sa_mask = iset;

      sigaction(SIGALRM, &structSigsignTimer, NULL);

      for(;;)
       {
	  longElapsedTime = alarm((_GLOBAL_shptr->config).sendingtime);
          pause();

	  kill( 0, SIGUSR2 );

       }

    return 0;

}



long mailQueueFunc(void)
{

  /* ------------------------------------- */

      long 	   longRet         =    0; 
      char*        response        = NULL; 
      message_t*   shMessage       = NULL;
      int          sval            =    0;

   /* ------------------------------------ */


 for(;;) {

	if ((_GLOBAL_shptr->config).stopSending == 1) {
		PRINT_INFO("AWIZOSOAPSERVERINF User suspended awizo sending process" );
		goto IDLE;
        }

	sem_getvalue(&(_GLOBAL_shptr->clientMutex), &sval);

	switch(_GLOBAL_shptr->awizoAction) {

		case queueTime : {

  			if (sval <= 0){
				continue;
			}
			else{
				sem_wait(&(_GLOBAL_shptr->clientMutex));
			}
/*
			if ((_GLOBAL_shptr->config).stopSending == 0) {
*/
			/* **************************************************** */
			/* uruchomienie procesu wysyłki e-Awiza w jednej sesji  */
			/* **************************************************** */

			PRINT_INFO("AWIZOSOAPSERVERINF sent %ld awizos from mailqueue.\n", _GLOBAL_myXmlQueue.ulCount );

			longRet = prepareAndSendAwizoSet(_GLOBAL_awizoConfig.dbase_handler, &response );

			if (longRet != BMD_OK) {
				PRINT_ERROR("Błąd wysyłki zakolejkowanych wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			free0(response);
	
			/* ************************************* */
			/*   Wyczyszczenie kolejki wiadomości    */
			/* ************************************* */

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
			PRINT_INFO("AWIZOSOAPSERVERINF clear mailqueue\n");

			if (DestroyQueue(&_GLOBAL_myXmlQueue, DestroyMsgNode) != BMD_OK) {
				PRINT_ERROR("Błędy w trakcie czyszczenia kolejki komunikatów. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
			
			/* ********************************************************** */
			/* Dodać tutaj wywoływanie funkcji wysyłki awiz z bazy danych */
			/* ********************************************************** */


			longRet = sendAwizoSetFromDataBase();

			if (longRet != BMD_OK) {
				PRINT_ERROR("Błąd wysyłki zakolejkowanych wiadomości z bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			goto IDLE;

		}

		case cacheClear : {

  			if (sval <= 0){
				continue;
			}
			else{
				sem_wait(&(_GLOBAL_shptr->clientMutex));
			}

 			PRINT_INFO("AWIZOSOAPSERVERINF current cache size : %ld.\n", _GLOBAL_myCache.ulCount );
 			PRINT_INFO("AWIZOSOAPSERVERINF clear cache\n");

 			if (DestroyQueue(&_GLOBAL_myCache, DestroyCacheNode) != BMD_OK) {
				PRINT_ERROR("Błędy w trakcie czyszczenia pamięci cache. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			PRINT_INFO("AWIZOSOAPSERVERINF current cache size : %ld.\n", _GLOBAL_myCache.ulCount );

			goto IDLE;
		}

		case queueClear : {

  			if (sval <= 0){
				continue;
			}
			else{
				sem_wait(&(_GLOBAL_shptr->clientMutex));
			}

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
 			PRINT_INFO("AWIZOSOAPSERVERINF clear mailqueue\n");
 
			if (DestroyQueue(&_GLOBAL_myXmlQueue, DestroyMsgNode) != BMD_OK) {
				PRINT_ERROR("Błędy w trakcie czyszczenia kolejki komunikatów. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
		
			goto IDLE;
		}

		case queueNextMail : {

			//sem_wait(&(_GLOBAL_shptr->clientMutex));

			PRINT_INFO("AWIZOSOAPSERVERINF add message to msgqueue\n"); 

			shMessage = (message_t*)calloc(1, sizeof(message_t));

			if (shMessage == NULL) {
				PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
			}

			memcpy(shMessage, &(_GLOBAL_shptr->buf), sizeof(message_t));

			/* *********************** */
			/* Wstawianie do kolejki   */
			/* *********************** */

			InsertToQueue(&_GLOBAL_myXmlQueue, shMessage);

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

			/* ********************************************************************* */
			/* Sprawdzenie ilości elementów w kolejce i uruchomienie procesu wysyłki */
			/* ********************************************************************* */

			if (ElementCount(&_GLOBAL_myXmlQueue)+1 > (_GLOBAL_shptr->config).maxmailqueue) {

			PRINT_INFO("AWIZOSOAPSERVERINF Sent %ld awizos from mailqueue.\n", _GLOBAL_myXmlQueue.ulCount );
			
			longRet = prepareAndSendAwizoSet( _GLOBAL_awizoConfig.dbase_handler, &response );

			if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd wysyłki zakolejkowanych wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
			}
			
			/* **************************************** */
			/*    W tym miejscu można wywołać funkcje   */
			/*         pobierającą status awiza         */
			/*     przekazanego do serwera pocztowego   */ 
			/* **************************************** */

			free0(response);

			/* ************************************* */
			/*   Wyczyszczenie kolejki wiadomości    */
			/* ************************************* */

 			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
			PRINT_INFO("AWIZOSOAPSERVERINF clear mailqueue\n");

			if (DestroyQueue(&_GLOBAL_myXmlQueue, DestroyMsgNode) != BMD_OK) {
				PRINT_ERROR("Błędy w trakcie czyszczenia kolejki komunikatów. Error = %d\n", BMD_ERR_OP_FAILED);
			}

			PRINT_INFO("AWIZOSOAPSERVERINF current mailqueue size : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

     			}
		}
	IDLE :
	      default : {

				sem_wait(&(_GLOBAL_shptr->awizoActionMutex));
				_GLOBAL_shptr->awizoAction = queueNothing;
				sem_post(&(_GLOBAL_shptr->awizoActionMutex));
			
				sem_post(&(_GLOBAL_shptr->clientMutex));
				sleep(1);	

				if (_GLOBAL_shptr->awizoAction == queueNothing) {
					sigemptyset(&_GLOBAL_mask_set);
 					sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);

					pause();
				}

			}

    }

 }

 return 0;

}


/**
 * Funkcja sprawdza, w jakim trybie ma pracować wysyłka awiz dla danego komunikatu.
 *
 * @param message Sparsowany komunikat.
 *
 * @retval modeEMAIL Wysyłka emaili (w komunikacie znaleziono email odbiorcy)
 * @retval modeSMS Wysyłka smsów (w komunikacie znaleziono numer odbiorcy)
 * @retval modeSMSEMAIL Wysyłka smsów i emaili (w komunikacie znaleziono zarówno email jak i numer telefonu odbiorcy).
 */
awizo_mode_t getAwizoMode( xmlDocPtr message)
{
	printf("getAwizoMode\n");
	char *stringEMAIL 	= NULL;
	char *stringSMS 	= NULL;
	int email 			= 0;
	int sms 			= 0;

	if (message == NULL) {
	PRINT_ERROR("Niepoprawny pierwszy parametr - komunikat XML. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }

    /* sprawdzenie wystepowania pola RECEIVER_EMAIL (tryb email) */
    if (getXpathValue( message, (xmlChar*)"//RECIPIENT_EMAIL", &stringEMAIL) == BMD_OK) {
		email = 1;
		free(stringEMAIL);
    }

    /* sprawdzenie wystepowania pola RECEIVER_SMS (tryb sms) */
	if (getXpathValue( message, (xmlChar*)"//RECIPIENT_SMS", &stringSMS) == BMD_OK) {
		sms = 1;
		free(stringSMS);
	}

	if (email && sms) {
		PRINT_NOTICE("Praca w trybie sms i email.\n");
		return modeSMSEMAIL;
	} else if (sms) {
		PRINT_NOTICE("Praca w trybie sms.\n");
		return modeSMS;
	} else if (email) {
		PRINT_NOTICE("Praca w trybie email.\n");
		return modeEMAIL;
	} else {
		PRINT_NOTICE("Nie określono trybu sms/email.\n");
		return modeNONE;
	}
}



long sendSMS(void* const cacheNode, xmlDocPtr message, void* dbHandler, \
			char* const msgID)
{
	cacheNode_t*       cacheNode_tNode = NULL;
	const char*        params          = NULL;
	long               longRet         =    0;
	xmlChar*           doc_txt_ptr     = NULL;
	int                doc_txt_len     =    0;
	char*              smsBody         = NULL;
	char*              receiver        = NULL;
	char*              attrValue       = NULL;
	char*		source		= NULL;
	char*              group           = NULL;
	char*		file		= NULL;
	queueNode_t*       queueNode	= NULL;
	xmlDocPtr                     fileFromBmd;
	xsltStylesheetPtr            templateFile;
	GenBuf_t*                         bmdFile;
	xmlDocPtr                             res;
	xmlDocPtr                             doc;



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

    if (msgID == NULL) {
	PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
	return BMD_ERR_PARAM4;
    }

    cacheNode_tNode = (cacheNode_t*)(((queueNode_t*)cacheNode)->node);

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;


    /*parsujemy szablon smsow*/
    doc = xmlParseMemory((const char*)(cacheNode_tNode->xsltotext), strlen(cacheNode_tNode->xsltotext));
    if (doc == NULL) {
	PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }

    templateFile = xsltParseStylesheetDoc(doc);
    if (templateFile == NULL) {
	PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
    }


    /*sprawdzamy zrodlo pobierania metadanych - bmd czy sam komunikat*/
    longRet = getXpathAttrValue( message, (xmlChar*)"//METADATA", (xmlChar*)"source_type", &attrValue );
    if ( longRet != BMD_OK ) {
		PRINT_ERROR("Błąd odczytu wartości atrybutu. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
    }

    if (strcmp(attrValue, "bmd") == 0) {
    /*    Pobranie dokumentu do transformacji z archiwum     */
    	longRet = getXpathValue( message, (xmlChar*)"//SOURCE", &source);

		if ( longRet != BMD_OK ) {
			PRINT_ERROR("Nie określono nazwy serwera bmd. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		longRet = getXpathValue( message, (xmlChar*)"//GROUP_ID", &group);

		if ( longRet != BMD_OK ) {
			PRINT_ERROR("Nie określono grupy uzytkownika. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		longRet = getXpathValue( message, (xmlChar*)"//FILE_ID", &file);

		if ( longRet != BMD_OK ) {
			PRINT_ERROR("Nie określono identyfikatora pliku. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		/*    Pobranie parametrów serwera     */
		longRet = FindInQueue( &(_GLOBAL_awizoConfig.bmdservers), checkServerName,\
				source, (void**)(&queueNode));

		/*   Funkcja pobrania dokumentu do transformacji z archiwum       */
		PRINT_INFO("Import file with given id = %ld\n", atol(file));
		longRet = getFileByIdFromBAE((bmdserver_t*)(queueNode->node), atol(file), NULL, group, &bmdFile);
		if ( longRet != BMD_OK || bmdFile == NULL ) {
			PRINT_ERROR("Błąd pobierania pliku z archiwum. Error = %d\n", BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		longRet = MessageParse( bmdFile, &fileFromBmd );
		if ( longRet != BMD_OK ) {
			PRINT_ERROR("Błąd parsowania otrzymanego komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
			free_gen_buf(&bmdFile);
			return BMD_ERR_OP_FAILED;
		}
		free_gen_buf(&bmdFile);
		res = xsltApplyStylesheet( templateFile, fileFromBmd, &params);
		if (res == NULL) {
			PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
			xsltFreeStylesheet(templateFile);
			xmlFreeDoc(fileFromBmd);
			return BMD_ERR_OP_FAILED;
		}
		xmlFreeDoc(fileFromBmd);
    }
    else if (strcmp(attrValue, "inline") == 0) {
		/* metadane w tresci komunikatu*/
		res = xsltApplyStylesheet( templateFile, message, &params);
		if (res == NULL) {
			PRINT_ERROR("Błąd parsowania szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
			xsltFreeStylesheet(templateFile);
			return BMD_ERR_OP_FAILED;
		}
	}
    else {
		PRINT_ERROR("Nie rozpoznany rodzaj zrodla metadanych. Error = %d\n", BMD_ERR_OP_FAILED);
		xsltFreeStylesheet(templateFile);
		return BMD_ERR_OP_FAILED;
    }

    /*zapisujemy uzyskana wiadomosc (xml + xslt) do stringa*/
    if ( xsltSaveResultToString ( &doc_txt_ptr, &doc_txt_len, res, templateFile ) < 0 ) {
		PRINT_ERROR("Błąd zapisu wiadomości do bufora. Error = %d\n", BMD_ERR_OP_FAILED);
		xsltFreeStylesheet(templateFile);
		xmlFreeDoc(res);
		return BMD_ERR_OP_FAILED;
    }
	xmlFreeDoc(res);
	xsltFreeStylesheet(templateFile);

	/* przypisujemy tresc gotowego smsa do stringa */
	smsBody = (char*) calloc(doc_txt_len+2, sizeof(char));
	if (smsBody == NULL){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		free0( doc_txt_ptr );
		return BMD_ERR_MEMORY;
	}
    memmove(smsBody,doc_txt_ptr+22,doc_txt_len-22);
    free0( doc_txt_ptr );

    /* pobranie numeru odbiorcy */
    longRet = getSMSSenderReceiver( message, &receiver);
	if (longRet != BMD_OK){
		PRINT_ERROR("Błąd odczytu numeru odbiorcy smsa. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	int status 		= 0;
	GenBuf_t *gb 	= NULL;
	char *api_url 	= NULL;
	char *answer	= NULL;

	/* link do api, albo wyjscie z funkcji, jesli uzyskano ciag pusty */
	asprintf(&api_url, "%s", (_GLOBAL_shptr->config).sms_api_url);
	PRINT_NOTICE("api_url: %s\n", api_url);
	if (api_url == NULL || strlen(api_url) < 1)
	{
		return -1;
	}

	//generowanie linka do API
	status = bmd_str_replace(&api_url, "__TO__", receiver);
	if (status != BMD_OK)
	{
		PRINT_ERROR("Nie mozna wstawic numeru %s do zadania SMS. Blad %i.\n", receiver, status);
		return status;
	}

	char *urlmessage = NULL; /*url-encoded message*/
	urlmessage = url_encode(smsBody);
	status = bmd_str_replace(&api_url, "__MESSAGE__", urlmessage);
	free(urlmessage);
	if (status != BMD_OK)
	{
		PRINT_ERROR("Nie mozna wstawic tresci %s do zadania SMS. Blad %i.\n", receiver, status);
		return status;
	}

	status = set_gen_buf2(" ", 1, &gb);
	if (status != BMD_OK)
	{
		PRINT_ERROR("Nie mozna ustawic bufora z trescia wiadomosci. Blad %i.\n", status);
		return status;
	}

	//status = SimpleCurlPost(gb, api_url);
	status = SimpleCurlPostAndGetAnswer(api_url, gb, &answer);
	free0(api_url);
	if (status != BMD_OK)
	{
		PRINT_ERROR("Blad wywolania uslugi SMS po curl. Blad %i.\n", status);
		return status;
	}

	/* Zobaczymy co na to bramka sms */
	/* jesli sprawdzamy czy error*/
	if ( (_GLOBAL_shptr->config).sms_api_error != NULL && strlen((_GLOBAL_shptr->config).sms_api_error) > 0) {
		/*znaleziono w komunikacie od serwera sms fragment o bledzie*/
		if (strstr(answer, (_GLOBAL_shptr->config).sms_api_error) != NULL) {
			PRINT_ERROR("SMS response: %s\n", answer);
			free0(answer);
			return BMD_ERR_OP_FAILED;
		}
	}
	
	PRINT_NOTICE("SMS was sent: %s\n", answer);
	free0(answer);
	
	return BMD_OK;
}




/**
 * Pobranie informacji odnośnie odbiorcy wiadomoci email
 *
 *
 * @param[in] message           - struktura opisująca wiadomość
 * @param[out] receiver		- odbiorca wiadomości
 *
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 *
*/
long getSMSSenderReceiver( xmlDocPtr message, char** const receiver)
{
	char* stringSender = NULL;

	if (message == NULL) {
		PRINT_ERROR("Niepoprawny pierwszy parametr - komunikat. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if (receiver == NULL) {
		PRINT_ERROR("Niepoprawny drugi parametr - odbiorca. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* Pobranie nazwy nadawcy */
	if (getXpathValue( message, (xmlChar*)"//SENDER", &stringSender) != BMD_OK) {
		PRINT_ERROR("Błąd odczytu nazwy nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/* Konfiguracja nadawania wiadomosci */
	if (getSenderConfiguration(stringSender) != BMD_OK) {
		PRINT_ERROR("Błąd odczytu ustawień konfiguracyjnych z bazy danych. Error = %d\n", BMD_ERR_OP_FAILED );
		free0(stringSender);
		return BMD_ERR_OP_FAILED;
	}
	free0(stringSender);


    /*   Pobranie odbiorcy wiadomości sms z komunikatu     */
	if (getXpathValue( message, (xmlChar*)"//RECIPIENT_SMS", receiver) != BMD_OK) {
		PRINT_ERROR("Błąd odczytu odbiorcy. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

    return BMD_OK;
}



/* KOD URLENCODINGU z http://www.geekhideout.com/urlcode.shtml */
/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}


long getTime( char** const stringTime ) {

  /* ----------------------- */
     struct timeval      tv;
     struct tm*      tm_ptr;
  /* ----------------------- */


     if ( stringTime == NULL || *stringTime != NULL ) {
        PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

     gettimeofday(&tv, 0);
     tm_ptr = (struct tm*)localtime(&(tv.tv_sec));

     asprintf( stringTime, "%04d-%02d-%02d %02d:%02d:%02d.%06li", 1900+tm_ptr->tm_year, \
                            tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, \
                            tm_ptr->tm_sec,(long)tv.tv_usec );

     return BMD_OK;

}


long setAdviceStatus(void* dbase_handler, long dispatchSettingId, long status) {


/* --------------------------- */

   char* stringSQL     = NULL;
   char* value 	       = NULL;
   int intRet	       =    0;

/* --------------------------- */


	PRINT_INFO("AWIZOSOAPSERVERINF Set dispatchSetting status in database.\n" );


	if (dispatchSettingId <= 0) {
		PRINT_ERROR("Niepoprawny pierwszy parametr (dispatchSettingId). Error = %d\n", BMD_ERR_PARAM1);
        	return BMD_ERR_PARAM1;
	}

	asprintf( &stringSQL, "UPDATE buffer SET status = %ld WHERE id = %ld;", status, dispatchSettingId );
	intRet = ExecuteSQLQueryWithAnswerKnownDBConnection(dbase_handler, stringSQL, &value);
	
	return BMD_OK;

}



long sendAwizoSetFromDataBase(void) {


/* ------------------------------------- */

    char* stringSQL              = NULL;
    void* dbase_handler          = NULL;
    db_row_strings_t*  rowstruct = NULL;
    int   intRet                 =    0;
    long  longRet		 =    0;
    long  fetched                =    0;
    long  rows                   =    0;
    long  cols                   =    0;
    long  emailSendStatus        =    0;
    GenBuf_t			   plik;
    void* result                 = NULL;
    char* response               = NULL;
    void* template               = NULL;
    int   i                      =    0;
    long  smsSendStatus          =    0;
    char* MsgID			 = NULL;
    awizo_mode_t       mode = modeEMAIL;	/* domyslnie wysylka emaili, nie smsow */
    char*  	 templateId      = NULL;
    mail_send_info_t* mail_info  = NULL;
    xmlDocPtr                  hMessage;

/* ------------------------------------- */


  PRINT_INFO("AWIZOSOAPSERVERINF Get awizo from database buffer.\n" );

  intRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &dbase_handler);
 
  if ( intRet < 0 ) {
	PRINT_ERROR("Database connection failed. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
  }


  if (asprintf( &stringSQL, "SELECT message,id FROM buffer WHERE date_time <= CURRENT DATE AND deleted is null AND status = 0;") < 0) {
	PRINT_ERROR("Memory allocation failed. Error = %d\n", BMD_ERR_MEMORY);
	bmd_db_disconnect(&dbase_handler);
	return BMD_ERR_OP_FAILED;
  }

  if ( bmd_db_execute_sql( dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	bmd_db_disconnect(&dbase_handler);
	return BMD_ERR_OP_FAILED;
  } 


  free0(stringSQL);

  i=0;

  do {

	intRet = bmd_db_result_get_row(dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );
	
	if (intRet < 0) { 
	    PRINT_INFO("Brak wiadomości do planowanej wysyłki zapisanych w bazie danych\n");
	}
	
	if (intRet == 0) {


  		plik.buf = (char*)malloc(strlen(rowstruct->colvals[0])+1);
  		memset(plik.buf,0, strlen(rowstruct->colvals[0]));
  		memcpy(plik.buf, rowstruct->colvals[0], strlen(rowstruct->colvals[0]));
  		plik.size = strlen(rowstruct->colvals[0]);

		/* ****************************************** */
		/*  Parsowanie otrzymanego komunikatu xml     */
		/* ****************************************** */

		longRet = MessageParse(&plik, &hMessage);

		if ( longRet != BMD_OK ) {
			PRINT_ERROR("Message parse error. Error = %d\n", BMD_ERR_OP_FAILED);
			setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
			free0(plik.buf);
			bmd_db_row_struct_free( &rowstruct );
			continue;
		}
		
		/* ***************************************** */
 		/*   sprawdzenie trybu pracy wysylki awiz    */
 		/* ***************************************** */

   		mode = getAwizoMode(hMessage);

   		if (mode == modeNONE){
			PRINT_ERROR("Nie określono trybu pracy eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
			PRINT_INFO("MsgID : %s\n", MsgID);
			//saveRaportToDb(hMessage, dbase_handler, MsgID, -1, -1, "Awizo mode not selected", modeNONE);
			saveRaportToDb(hMessage, dbase_handler, -1, -1, -1, "Awizo mode not selected", modeNONE);
			setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
			bmd_db_row_struct_free( &rowstruct );
			xmlFreeDoc(hMessage);
			free0(plik.buf);
			free0(MsgID);
			continue;
		}


		/* *********************************** */
 		/*  Tylko w przypadku wysylki smsow    */
 		/* *********************************** */

		if (mode == modeSMS || mode == modeSMSEMAIL) {
			saveRaportToDb(hMessage, dbase_handler, -1, atol(templateId), 1, "Prepared successfully", modeSMS);
			smsSendStatus = sendSMS(template, hMessage, dbase_handler, -1);

			if ( smsSendStatus != BMD_OK ) {
				PRINT_ERROR("Błąd wysyłki wiadomości SMS. Error = %d\n", BMD_ERR_OP_FAILED);
				PRINT_INFO("MsgID : %s\n", MsgID);
				updateRaportInDb(dbase_handler, MsgID, modeSMS, "SMS send error", '0');
				setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
  			}
			else {

				/* ********************************************* */
 				/*  Zapis statusu po prawidłowej wysyłce SMSów   */
 				/* ********************************************* */

    				updateRaportInDb(dbase_handler, MsgID, modeSMS, "SMS was successfully send", '1');
				setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), 1);
			}
   		}

 		/* ********************************** */
 		/*  Tylko w przypadku wysylki maili   */
 		/* ********************************** */

   		if (mode == modeEMAIL || mode == modeSMSEMAIL) {

 		/* ********************************** */
 		/*   Przygotowanie tresci smime'a     */
 		/* ********************************** */

			longRet = prepareAwizo(template, hMessage, dbase_handler, -1, &mail_info );

			if (longRet != BMD_OK) {
				PRINT_ERROR("eAwizo prepared failed. Error = %d\n", BMD_ERR_OP_FAILED);
				PRINT_INFO("MsgID : %s\n", MsgID);
				saveRaportToDb(hMessage, dbase_handler, MsgID, atol(templateId), longRet, "eAwizo prepared failed", modeEMAIL);
				setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
				bmd_db_row_struct_free( &rowstruct );
				xmlFreeDoc(hMessage);
				free0(plik.buf);
				free0(templateId);
				free0(MsgID);

				if (smsSendStatus != BMD_OK) {
					PRINT_ERROR("SMS send error and eAwizo prepared failed. Error = %d\n", BMD_ERR_OP_FAILED);
				}
				
				continue;
			}

			/* ******************************************* */
			/*  Zapis statusu o prawidłowym skomponowaniu  */
			/*   eAwiza do tabeli raportów bazy danych     */
			/* ******************************************* */

			saveRaportToDb(hMessage, dbase_handler, -1, atol(templateId), 1, "Prepared successfully", modeEMAIL);

			_GLOBAL_awizoConfig.awizoTable = (mail_send_info_t*)calloc(1, sizeof(mail_send_info_t));

			if (_GLOBAL_awizoConfig.awizoTable == NULL) {
				PRINT_ERROR("Memory allocation failed. Error = %d\n", BMD_ERR_MEMORY);
				PRINT_INFO("MsgID : %s\n", MsgID);
				saveRaportToDb(hMessage, dbase_handler, MsgID, atol(templateId), BMD_ERR_MEMORY, "eAwizo table memory error", modeEMAIL);
				setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
				bmd_db_row_struct_free( &rowstruct );
				xmlFreeDoc(hMessage);
				free0(plik.buf);
				free0(templateId);
				free0(MsgID);
				
				if (smsSendStatus != BMD_OK){
					PRINT_ERROR("SMS send error and eAwizo table memory error");
				}

				continue;
  			}

			/*wysylka*/
			memcpy(&(_GLOBAL_awizoConfig.awizoTable[0]), mail_info, sizeof(mail_send_info_t));
			emailSendStatus = bmd_send_envoice_avizo(_GLOBAL_awizoConfig.awizoTable, 1, &response);

			if (emailSendStatus != BMD_OK) {
				PRINT_ERROR("eAwizo send failed. Error = %d\n", BMD_ERR_OP_FAILED);
				PRINT_INFO("MsgID : %s\n", MsgID);
				updateRaportInDb(dbase_handler, MsgID, modeEMAIL, "eAwizo send failed", response[0]);
				setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), -1);
				free0(response);
				freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, 1);
				bmd_db_row_struct_free( &rowstruct );
				xmlFreeDoc(hMessage);
				free0(plik.buf);
				free0(templateId);
				free0(MsgID);

				if (smsSendStatus != BMD_OK){
					PRINT_ERROR("SMS send error and eAwizo send failed");
				}

				continue;
			}	

			updateRaportInDb(dbase_handler, -1, modeEMAIL, "eAwizo send successfully", response[0]);
			setAdviceStatus(dbase_handler, atol(rowstruct->colvals[1]), 1);
			free0(response);
			freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, 1);
			free0(_GLOBAL_awizoConfig.awizoTable);

			if (smsSendStatus != BMD_OK){
				PRINT_ERROR("SMS send failed and eAwizo send successfully");
			}
		}

		xmlFreeDoc(hMessage);
		free0(plik.buf);
		free0(templateId);
		free0(MsgID);
		
		i+=1;
		bmd_db_row_struct_free( &rowstruct );
		fetched = 0;

	}

 }while(intRet == 0);

/*
 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));
  _GLOBAL_shptr->awizoAction = queueNothing;
 sem_post(&(_GLOBAL_shptr->awizoActionMutex));
*/

 bmd_db_row_struct_free( &rowstruct );
 bmd_db_result_free( &result );
 fetched = 0;

 bmd_db_disconnect(&dbase_handler);

 return BMD_OK;

}



