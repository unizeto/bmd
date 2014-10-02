/****************************************************
 *					            *
 * Implementacja węzła załącznika                   *
 *						    *
 * Data  : 12-08-2009                               *
 * Autor : Tomasz Klimek                            *
 *						    *
 ****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdutils/libbmdutils.h>


/**
 *  Funkcja tworzy nowy węzeł struktury załącznika
 * 
 * @param[in,out] attchNode_tNode - nowy węzeł załącznika
 * @param[in] filename - nazwa pliku zalacznika
 * @param[in] mimetype - typ mime
 * @param[in] disposition - dyspozycja
 * @param[in] content - zawartosc 
 *
 * @retval BMD_ERR_PARAM1         - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2         - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM5         - niepoprawny piąty parametr
 * @retval BMD_ERR_MEMORY         - błąd alokacji pamięci
 * @retval BMD_OK                 - zakończenie pomyślne
 *
*/

long CreateAttchNode( attchNode_t** const attchNode_tNode, char const * const filename, \
			char const * const mimetype, char const * const disposition, \
			char const * const content, long const contentSize ) {


 if ( attchNode_tNode == NULL ) {
	PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
 }

 if ( filename == NULL ){
	PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
 }
 
 if ( content == NULL ){
	PRINT_ERROR("Niepoprawny piaty parametr. Error = %d\n", BMD_ERR_PARAM5);
	return BMD_ERR_PARAM5;
 }

 if ( contentSize < 0 ) {
	PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM6);
	return BMD_ERR_PARAM6;
 }

 *attchNode_tNode = (attchNode_t*)calloc(1, sizeof( attchNode_t ));

 if ((*attchNode_tNode) == NULL ){
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 asprintf( &((*attchNode_tNode)->filename ),"%s", filename);

 if ( mimetype ) {
	asprintf(&((*attchNode_tNode)->mimetype), "%s", mimetype);
 }

 if ( disposition ) {
	asprintf(&((*attchNode_tNode)->disposition), "%s", disposition);
 }

 (*attchNode_tNode)->content = (char*)calloc(contentSize, sizeof(char));

 if ((*attchNode_tNode)->content == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 memcpy((void*)((*attchNode_tNode)->content), content, contentSize );

 (*attchNode_tNode)->size = contentSize;

 return BMD_OK;

}

/**
 *  Funkcja zwalnia dany węzeł struktury załącznika
 *
 * @param[in] attchNode_tNode - zwalniany węzeł struktury załącznika
 *
*/

void DestroyAttchNode( void* attchNode_tNode ) {

 free0(((attchNode_t*)attchNode_tNode)->filename);
 free0(((attchNode_t*)attchNode_tNode)->mimetype);
 free0(((attchNode_t*)attchNode_tNode)->disposition);
 free0(((attchNode_t*)attchNode_tNode)->content);

 free0(attchNode_tNode);

 }


