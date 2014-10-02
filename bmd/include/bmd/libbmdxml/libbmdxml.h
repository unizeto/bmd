/***************************************************************************
 *            libbmdxml.h
 *
 *  Fri Mar 24 13:57:15 2006
 *  Copyright  2006  Lukasz Klimek
 *  lklimek@certum.pl
 ****************************************************************************/

#ifndef _LIBBMDXML_H
#define _LIBBMDXML_H

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <bmd/common/bmd-os_specific.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
	#ifdef LIBBMDXML_EXPORTS
		#define LIBBMDXML_SCOPE __declspec(dllexport)
	#else
		#define LIBBMDXML_SCOPE __declspec(dllimport)
	#endif
#else
	#define LIBBMDXML_SCOPE extern
#endif

typedef enum EnumWithBase64Coding 	{ WITHOUT_BASE64, WITH_BASE64 } EnumWithBase64Coding_t;

LIBBMDXML_SCOPE long bmdxml_init(void);
long bmdxml_load_oids(char *oid_file);
LIBBMDXML_SCOPE long bmdxml_destroy();

long bmdxml_transfer_node_content(xmlDocPtr docin, char *xpathin,
											xmlDocPtr docout, char *xpathout);
long bmdxml_get_node_by_xpath(xmlDocPtr doc, xmlChar *xpath, xmlNodePtr *new_node);
long bmdxml_set_content_at_xpath(xmlDocPtr doc, char *xpath, char *content);

long bmdxml_doc2genbuf(xmlDocPtr doc, GenBuf_t **bufor);
long bmdxml_doc2buffer(xmlDocPtr doc, char **bufor, long *len);


LIBBMDXML_SCOPE long bmdxml_find_nodes (xmlDocPtr document, const char * xpath, xmlNodeSetPtr * result);
LIBBMDXML_SCOPE long bmdxml_get_node_value_by_xpath (xmlDocPtr document, const char * xpath, char ** outbuff);

/* datagram -> xml conversion */
    xmlChar *bmd_oid2xmltag(	char *OID, char **metadataName);
    xmlNodePtr bmd_metadata2xml(const MetaDataBuf_t * const metadata, EnumWithBase64Coding_t base64Coding);
    LIBBMDXML_SCOPE xmlNodePtr bmd_datagramset2xml(bmdDatagramSet_t * set, EnumWithBase64Coding_t base64Coding, bmdStringArray_t *allowedOids);
    xmlNodePtr bmd_datagram2xml(const bmdDatagram_t * bmdDatagram, EnumWithBase64Coding_t base64Coding, bmdStringArray_t* allowedOids);

#ifdef __cplusplus
}
#endif
#endif				/* _LIBBMDXML_H */
