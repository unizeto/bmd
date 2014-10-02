/****************************************************
 *					            *
 * Implementacja węzła cache                        *
 *						    *
 * Data  : 12-08-2009                               *
 * Autor : Tomasz Klimek                            *
 *						    *
 ****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/mail_sender/attachment.h>

/**
 *  Funkcja tworzy nowy węzeł cache
 * 
 * @param[in,out] cacheNode_tNode - nowy węzeł cache

 * @param[in] id 		  - identyfikator szablonu
 * @param[in] name		  - nazwa szablonu
 * @param[in] lang 		  - język szablonu
 * @param[in] version		  - wersja szablonu
 * @param[in] xsltohtml		  - szablon htmlowy
 * @param[in] xsltotext		  - szablon tekstowy
 * @param[in] attachments 	  - uchwyt do listy załączników
 *
 * @retval BMD_ERR_PARAM1         - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2         - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3         - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY         - błąd alokacji pamięci
 * @retval BMD_OK                 - zakończenie pomyślne
 *
*/

long CreateCacheNode( cacheNode_t** const cacheNode_tNode, \
			long const id, \
			char* const name, \
		 	char* const lang, \
			char* const version, \
			char* const xsltohtml, \
			char* const xsltotext, \
			long const sign,
			queue_t* const attachments ) {


	if ( cacheNode_tNode == NULL ){
		PRINT_ERROR("Niepoprawny parametr wyjściowy. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if ( id < 0 ){
		PRINT_ERROR("Niepoprawny identyfikator. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if (sign != 0 && sign != 1) {
		PRINT_ERROR("Niepoprawny parametr podpisu. Error = %d\n", BMD_ERR_PARAM8);
		return BMD_ERR_PARAM8;
	}

	*cacheNode_tNode = (cacheNode_t*)calloc(1, sizeof( cacheNode_t ));

	if ( (*cacheNode_tNode) == NULL ){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	(*cacheNode_tNode)->id = id;
	(*cacheNode_tNode)->sign = sign;

	if ( name ) {
		asprintf(&((*cacheNode_tNode)->name),"%s", name);
	}
	if ( lang ) {
		asprintf(&((*cacheNode_tNode)->lang),"%s", lang);
	}
	if ( version ) {
		asprintf(&((*cacheNode_tNode)->version),"%s", version);
	}
	if ( xsltohtml ) {
		 asprintf(&((*cacheNode_tNode)->xsltohtml), "%s", xsltohtml);
	}
	if ( xsltotext ) {
		asprintf(&((*cacheNode_tNode)->xsltotext), "%s", xsltotext);
	} 
	if ( attachments ) {
		(*cacheNode_tNode)->_attachments = attachments;
	}


   return BMD_OK;
 }

/**
 *  Funkcja zwalnia dany węzeł pamięci cache
 *
 * @param[in] cacheNode_tNode - zwalniany węzeł cache
 *
*/

void DestroyCacheNode( void* cacheNode_tNode ) {

 free0(((cacheNode_t*)cacheNode_tNode)->name);
 free0(((cacheNode_t*)cacheNode_tNode)->lang);
 free0(((cacheNode_t*)cacheNode_tNode)->version);
 free0(((cacheNode_t*)cacheNode_tNode)->xsltohtml);
 free0(((cacheNode_t*)cacheNode_tNode)->xsltotext);

 DestroyQueue(((cacheNode_t*)cacheNode_tNode)->_attachments, DestroyAttchNode );

 free0(cacheNode_tNode);
}


