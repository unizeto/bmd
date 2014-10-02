/*****************************************************
 *                                                   *
 * Implementacja kolejki do przechowywania dowolnego *
 * rodzaju węzła.                                    *
 *                                                   *
 * Data : 11-08-2009                                 *
 * Autor : Tomasz Klimek                             *
 *                                                   *
 *****************************************************/

#include <bmd/mail_sender/queue.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

/**
* Funkcja inicjuje strukturę kolejki.
*
* @param[in,out] queue_tqueue - uchwyt do kolejki
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_OK - zakończenie pomyślne
*/


long InitQueue( queue_t* const queue_tqueue ){

	if ( queue_tqueue == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	queue_tqueue->head = NULL;
	queue_tqueue->tail = NULL;
	queue_tqueue->ulCount = 0;

	return BMD_OK;
}



/**
* Funkcja zwraca liczbę elementów w kolejce
*
* @param[in] queue_tqueue - uchwyt do kolejki
*
* @return liczba elementów w kolejce
*/


long ElementCount( queue_t* const queue_tqueue  ) {

	if ( queue_tqueue == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	return(queue_tqueue->ulCount);

}


/**
* Funkcja wstawia nowy element do kolejki.
*
* @param[in,out] queue_tqueue - uchwyt do kolejki
* @param[in] _node - wstawiany węzeł
*
* @return status zakończenia 
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_OK - zakończenie pomyślne
*/


long InsertToQueue( queue_t* const queue_tqueue, void* const _node ){

     /* ------------------------------------ */

	queueNode_t* queueNode_tNode = NULL;

     /* ------------------------------------ */

	if ( queue_tqueue == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
    	}

	queueNode_tNode = (queueNode_t*)calloc(1, sizeof(queueNode_t));

	if ( queueNode_tNode == NULL ){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	queueNode_tNode->prev = NULL;
	queueNode_tNode->node = _node;

	if ( queue_tqueue->tail != NULL ){
		queue_tqueue->tail->prev = queueNode_tNode;
		queueNode_tNode->next = queue_tqueue->tail;
	}
	else{
		queueNode_tNode->next = NULL;
		queue_tqueue->head = queueNode_tNode;
	}

	queue_tqueue->tail = queueNode_tNode;
	queue_tqueue->ulCount++;

	return BMD_OK;

 }
 
/**
* Funkcja wyszukuje zadany węzeł w kolejce.
*
* @param[in] queue_tqueue - uchwyt do kolejki
* @param[in] select - wskaźnik do funkcji porównującej
* @param[in] elem - poszukiwany element
* @param[out] out - wskaźnik na odnaleziony węzeł
*
* @return status zakończenia operacji
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval EMPTY_QUEUE - kolejka jest pusta
* @retval FOUND_IN_QUEUE - odnaleziono poszukiwany węzeł
* @retval NFOUND_IN_QUEUE - nie odnaleziono poszukiwanego węzła
*
*/


long FindInQueue( queue_t* queue_tqueue, long (*select)(const void* const, const void* const),\
                const void* const elem, void** const out) {

     /* ------------------------------------ */

	long          intRet          =    0;
	queueNode_t*  queueNode_tNode = NULL;

    /* ----------------------------------- */


	if ( queue_tqueue == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if ( select == NULL ){
		PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if ( elem == NULL ){
		PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if ( queue_tqueue->head == NULL || queue_tqueue->tail == NULL ){
		return EMPTY_QUEUE;
	}

	queueNode_tNode = ( out && *out) ? *out : queue_tqueue->head;

	while( 1 ){
		intRet = select( queueNode_tNode->node, elem );

		if ( intRet == FOUND_IN_QUEUE ){
			PRINT_INFO("Odnaleziono poszukiwany element\n");

			if ( out ) {
				(*out) = queueNode_tNode;
			}
		break;
		}

		queueNode_tNode = queueNode_tNode->prev;

		if ( queueNode_tNode == NULL ){
			PRINT_INFO("Nie odnaleziono poszukiwanego elementu\n");
			break;
		}
	}

   return intRet;

 }
 
/**
* Funkcja usuwa zadany węzeł z kolejki.
*
* @param[in] queue_tqueue - uchwyt do kolejki
* @param[in] elem - usuwany węzeł
* @param[out] del - wskaźnik do funkcji usuwającej zawartość węzła
*
* @return status zakończenia operacji
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval EMPTY_QUEUE - kolejka jest pusta
* @retval BMD_OK  - zakończenie pomyślne
*
*/

long DeleteFromQueue( queue_t* const queue_tqueue, void** const elem, void (*del)(void*) ){


	void* delNode = NULL;


	if ( queue_tqueue == NULL ) {
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if ( elem == NULL || *elem == NULL ) {
		PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if ( queue_tqueue->head == NULL || queue_tqueue->tail == NULL ) {
		PRINT_INFO("Kolejka jest pusta. Error = %d\n", EMPTY_QUEUE);
		return EMPTY_QUEUE;
	}

	if ( del != NULL ) {
		del( ((queueNode_t*)(*elem))->node );
	}

	delNode = (*elem);

	if ( ((queueNode_t*)(*elem))->next != NULL ) { 
		((queueNode_t*)(*elem))->next->prev = ((queueNode_t*)(*elem))->prev;
	}

	if ( ((queueNode_t*)(*elem))->prev != NULL ) {
		((queueNode_t*)(*elem))->prev->next = ((queueNode_t*)(*elem))->next;
	} 

	if ( (queueNode_t*)delNode == queue_tqueue->tail ) {
		queue_tqueue->tail = ((queueNode_t*)delNode)->next;
	}
 
	if ( (queueNode_t*)delNode == queue_tqueue->head ) {
		queue_tqueue->head = ((queueNode_t*)delNode)->prev; 
	}
 
	if ( del == NULL ) {
         (*elem) = ((queueNode_t*)(*elem))->node;
	}

	queue_tqueue->ulCount--;
	free( delNode );

 return BMD_OK;

 }

/**
* Funkcja usuwa węzły z kolejki.
*
* @param[in] queue_tqueue - uchwyt do kolejki
*
* @return status zakończenia operacji
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_OK  - zakończenie pomyślne
*
*/

long DestroyQueue(queue_t* const queue_tqueue, void (*del)(void*))
{

     /* ------------------------------------- */

	 queueNode_t*  queueNode_tNode = NULL;  

     /* ------------------------------------- */


	if ( queue_tqueue == NULL ){
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	for( queueNode_tNode = queue_tqueue->head; \
	     queueNode_tNode; queueNode_tNode = queue_tqueue->head){

		DeleteFromQueue( queue_tqueue, (void**)&queueNode_tNode, del );
	}

   return BMD_OK;
}

