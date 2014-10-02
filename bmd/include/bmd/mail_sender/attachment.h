/*****************************************
 *			                 *
 * Implementacja węzła załącznika        *
 *			                 *
 * Data : 12-08-2009                     *
 * Autor : Tomasz Klimek                 *
 *			                 *
 *****************************************/

#ifndef __ATTACHMENT_NODE__
#define __ATTACHMENT_NODE__

#define MAX_ATTACHMENTS 10

  typedef struct{

     /* --------------------- */
	
	char* filename;	      /* nazwa pliku załącznika */
	char* mimetype;	      /* typ mime */
	char* disposition;    /* inline OR external */
	char* content;        /* zawartość */
	long  size;           /* rozmiar załącznika */

     /* --------------------- */

} attchNode_t;

/* -------------------------------------------------------------------------------- */
/* API */

long CreateAttchNode( attchNode_t** const, char const * const, char const * const, \
			char const * const, char const * const, long const );
void DestroyAttchNode( void* );

/* --------------------------------------------------------------------------------- */

#endif /* __ATTACHMENT_NODE__  */

