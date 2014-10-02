/*****************************************************
 *                                                   *
 * Implementacja kolejki do przechowywania dowolnego *
 * rodzaju węzła.                                    *
 *                                                   *
 * Data : 11-08-2009                                 *
 * Autor : Tomasz Klimek                             *
 *                                                   *
 ****************************************************/

#ifndef __QUEUE__
#define __QUEUE__

#include <stdio.h>


#define FOUND_IN_QUEUE 1
#define NFOUND_IN_QUEUE 0
#define EMPTY_QUEUE -1


typedef struct queueNode{

     /* ------------------------ */

	void*              node;  /* przechowywany węzeł */
	struct queueNode*  prev;  /* poprzedni węzeł */
	struct queueNode*  next;  /* następny węzeł */

     /* ------------------------ */

} queueNode_t;

typedef struct{

     /* -------------------- */

	queueNode_t*   head;  /* początek listy */
	queueNode_t*   tail;  /* koniec listy */
	long        ulCount;  /* liczba elementów */

     /* -------------------- */

} queue_t;

 /* --------------------------------------------------------------------------------------------------------- */

 /* API */

long InitQueue( queue_t* const );
long InsertToQueue( queue_t* const, void* const );
long FindInQueue( queue_t*, long (*)(const void* const, const void* const),const void* const, void** const );
long DeleteFromQueue( queue_t* const, void** const, void (*)(void*) );
long DestroyQueue( queue_t* const, void (*)(void*) );
long ElementCount( queue_t* const );

 /* --------------------------------------------------------------------------------------------------------- */

#endif

