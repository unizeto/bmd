/* Plik naglowkowy zawierajacy wspolne definicje struktur danych, zmienne wykorzystywane we wszystkich skladnikach BMD */

#ifndef _BMD_COMMON_INCLUDED_
#define _BMD_COMMON_INCLUDED_
#include <bmd/common/bmd-os_specific.h>	/* ustawienia specyficzne dla roznych systemow */
#include <bmd/common/bmd-include.h>		/* podstawowe plikow naglowkowych + curl + pcre */
#include <bmd/common/bmd-const.h>		/* stale uzywane w kodzie BMD */
#include <bmd/common/bmd-errors.h>		/* kody bledow */
#include <bmd/common/bmd-oids.h>		/* identyfikatory obiektow stosowane w bmd */
#include <bmd/common/bmd-sds.h>			/* wlaczenie struktur z typami podstawowymi dla kompilatora (SDS - Simple Data Structures) */

#ifdef _DEBUG_UNIX_
#include <time.h>
#include <sys/time.h>
#endif				/* ifdef _DEBUG_UNIX_ */
typedef enum pcre_multiline { SINGLELINE, MULTILINE } pcre_multiline_t;

typedef enum EnumWithProtocolData 	{ WITHOUT_PROTOCOL_DATA, WITH_PROTOCOL_DATA } EnumWithProtocolData_t;
typedef enum EnumWithPkiMetadata 	{ WITHOUT_PKI_METADATA, WITH_PKI_METADATA } EnumWithPkiMetadata_t;
typedef enum EnumWithAddMetadata 	{ WITHOUT_ADD_METADATA, WITH_ADD_METADATA } EnumWithAddMetadata_t;
typedef enum EnumWithSysMetadata 	{ WITHOUT_SYS_METADATA, WITH_SYS_METADATA } EnumWithSysMetadata_t;
typedef enum EnumWithActoinMetadata 	{ WITHOUT_ACTION_METADATA, WITH_ACTION_METADATA } EnumWithActionMetadata_t;
typedef enum EnumGetConfigTime		{ TIME_FIRST, TIME_AGAIN } EnumGetConfigTime_t;

typedef enum EnumUpdateGetInvoice 	{ UPDATE_GET_INOICE_ALWAYS, UPDATE_GET_INVOICE_ONCE } EnumUpdateGetInvoice_t;

#ifndef SWIG
typedef enum { IS_VOID, IS_NUMERIC, IS_STRING, IS_WCSTRING,	IS_TIME } COMPONENT_TYPE;
#endif

#endif	/* */
