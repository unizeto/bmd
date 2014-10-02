#ifndef _LIBBMDXPATH_H_
#define _LIBBMDXPATH_H_

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <ctype.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

typedef struct
{
	xmlXPathContextPtr ctx;
	xmlXPathObjectPtr result;
} bmdxpath_info_t;

/** Inicjalizacja struktury bmdxpath_info_t. */
long bmdxpath_init(xmlDocPtr doc, bmdxpath_info_t **info);

/** Zwalnianie pamięci po strukturze bmdxpath_info_t. */
long bmdxpath_destroy(bmdxpath_info_t **info);

/** Rejestracja pojedynczej przestrzeni nazw. */
long bmdxpath_register_ns(bmdxpath_info_t *info, char *prefix, char *uri);

/** Rejestracja przestrzeni nazw z listy. */
long bmdxpath_register_ns_list(bmdxpath_info_t *info, char *nslist, char insep, char outsep);

/** Wyszukiwanie węzłów po XPath. */
long bmdxpath_find(bmdxpath_info_t *info, char *xpath);

/** Zwraca ilość węzłów przechowywanych 
 * aktualnie w strukturze bmdxpath_info_t.*/
long bmdxpath_count_result_nodes(bmdxpath_info_t *info);

/** Pobieranie tablicy węzłów zakonczonej wartością NULL z zadanego adresu XPath. */
long bmdxpath_get_nodes(bmdxpath_info_t *info, char *xpath, xmlNodePtr **nodes);

/** Zwalnianie pamięci po tablicy węzłów utworzonej funkcją bmdxpath_get_nodes(). */
long bmdxpath_free_nodes(xmlNodePtr **nodes);

/** Pobieranie tablicy napisów zakończonej wartością NULL z zadanego adresu XPath. */
long bmdxpath_get_strings(bmdxpath_info_t *info, char *xpath, char ***values);

/** Zwalnianie pamięci po tablicy napisów utworzonej funkcją bmdxpath_get_string(). */
long bmdxpath_free_strings(char *** values);

/** Przetwarzanie tablicy stringow w jeden string */
long bmdxpath_strings2string_freetable(char** dstStr, size_t * length, char ***stringsTable, size_t tableSize);

/** Pobieranie jednego napisu z zadanego adresu XPath. */
long bmdxpath_get_string(bmdxpath_info_t *info, char *xpath, char **value);

/** Przypisanie do tagu / tagów odpowiedniego napisu.
\returns Ilość zapisanych węzłów lub kod błędu.*/
long bmdxpath_sprintf(bmdxpath_info_t *info, char *xpath, char *fmt, ...);

/** Zapisanie zawartości dokumentu do bufora generycznego. */
long bmdxpath_doc2genbuf(xmlDocPtr doc, GenBuf_t **bufor);

/** Zapisanie zawartości dokumentu do bufora (char*) */
long bmdxpath_doc2buffer(xmlDocPtr doc, char **bufor, long *len);

#endif /*_LIBBMDXPATH_H_ */
