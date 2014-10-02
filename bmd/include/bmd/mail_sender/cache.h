/*****************************************
 *			                 *
 * Implementacja węzła pamięci cache     *
 *			                 *
 * Data : 12-08-2009                     *
 * Autor : Tomasz Klimek                 *
 *			                 *
 *****************************************/

#ifndef __CACHE_NODE__
#define __CACHE_NODE__

#include <bmd/mail_sender/queue.h>

  typedef struct{

     /* ----------------------------------- */

        long		id;			/* id szablonu z bazy */
	char*		name;			/* nazwa szablonu */
	char*		lang;			/* język szablonu */
	char*		version;		/* wersja szablonu */
	char*		xsltohtml;		/* szablon wariant I */
	char*		xsltotext;		/* szablon wariant II */
	long		sign;			/* czy szablon ma być podpisywany */
	queue_t*	_attachments;		/* lista załączników powiązanych z szablonem */

     /* ----------------------------------- */

} cacheNode_t;

/* --------------------------------------------------------------------------- */
/* API */


long CreateCacheNode( cacheNode_t** const, long const, char* const, \
		 	char* const, char* const, char* const, \
			char* const, long const, queue_t* const );

void   DestroyCacheNode( void* const );
void   ReleaseAttachment( void* );

/* ---------------------------------------------------------------------------- */

#endif

