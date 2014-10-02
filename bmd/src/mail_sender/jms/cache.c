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

 * @param[in] _template - uchwyt szablonu
 * @param[in] attachments - uchwyt do listy załączników
 *
 * @retval BMD_ERR_PARAM1         - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2         - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3         - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY         - błąd alokacji pamięci
 * @retval BMD_OK                 - zakończenie pomyślne
 *
*/

long CreateCacheNode( cacheNode_t** const cacheNode_tNode, long const id, char* const name, \
		 	char* const namexpath, 	char* const title, char* const lang, \
			char* const langxpath,	char* const senderorgname, \
			char* const recipienttype, char* const recipienttypexpath, \
			char* const xsltohtml, char* const xsltotext, queue_t* const attachments ){


	if ( cacheNode_tNode == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if ( id < 0 ){
		PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*cacheNode_tNode = (cacheNode_t*)calloc(1, sizeof( cacheNode_t ));

	if ( (*cacheNode_tNode) == NULL ){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	(*cacheNode_tNode)->id = id;

	if ( name ) {
		asprintf(&((*cacheNode_tNode)->name),"%s", name);
	}
	if ( namexpath ) {
		asprintf(&((*cacheNode_tNode)->namexpath),"%s", namexpath);
	}
	if ( title ) {
		asprintf(&((*cacheNode_tNode)->title),"%s", title);
	}
	if ( lang ) {
		asprintf(&((*cacheNode_tNode)->lang),"%s", lang);
	}
	if ( langxpath ) {
		asprintf(&((*cacheNode_tNode)->langxpath),"%s", langxpath);
	}
	if ( senderorgname ) { 
		asprintf(&((*cacheNode_tNode)->senderorgname),"%s", senderorgname);
	}
	if ( recipienttype ) { 
		asprintf(&((*cacheNode_tNode)->recipienttype),"%s", recipienttype);
	}
	if ( recipienttypexpath ){
		asprintf(&((*cacheNode_tNode)->recipienttypexpath),"%s", recipienttypexpath);
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
 free0(((cacheNode_t*)cacheNode_tNode)->namexpath);
 free0(((cacheNode_t*)cacheNode_tNode)->title );
 free0(((cacheNode_t*)cacheNode_tNode)->lang);
 free0(((cacheNode_t*)cacheNode_tNode)->langxpath);
 free0(((cacheNode_t*)cacheNode_tNode)->senderorgname);
 free0(((cacheNode_t*)cacheNode_tNode)->recipienttype);
 free0(((cacheNode_t*)cacheNode_tNode)->recipienttypexpath);
 free0(((cacheNode_t*)cacheNode_tNode)->xsltohtml);
 free0(((cacheNode_t*)cacheNode_tNode)->xsltotext);

 DestroyQueue(((cacheNode_t*)cacheNode_tNode)->_attachments, DestroyAttchNode );

 free0(cacheNode_tNode);
}


