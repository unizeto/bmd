/***************************************
 *                                     *
 * ATD obsługujące algorytm balancingu *
 * w systemie generowania logów        *
 *                                     *
 * Data : 27-08-2007                   *
 * Autor : Tomasz Klimek               *
 *                                     *
 ***************************************/

#ifndef __ATD__H
#define __ATD__H


#include <bmd/libbmdlog/balance/list.h>
#include <bmd/libbmdlog/balance/conn_node.h>
#include <bmd/libbmdlog/balance/load_node.h>


#define ATD list_t
#define ATD_PTR listNode_t*
#define ATD_LOAD_NODE_PTR loadNode_t*

#define INIT( atdPtr )                      InitList( atdPtr )
#define INSERT( atdPtr, elem )              InsertToList( atdPtr, elem )
#define DELETE( atdPtr, delNode, delFunc )  DeleteFromLogList( atdPtr, delNode, delFunc)

#define FIND( atdPtr, searchFunc, searchElem, direction, foundElem ) \
  FindInLogList( atdPtr, searchFunc, searchElem, direction, foundElem )

#endif

