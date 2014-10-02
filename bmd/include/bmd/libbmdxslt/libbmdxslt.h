#ifndef _LIBBMDXSLT_INCLUDED_
#define _LIBBMDXSLT_INCLUDED_

#include <string.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/debugXML.h>
#include <libxml2/libxml/HTMLtree.h>
#include <libxml2/libxml/xmlIO.h>
#include <libxml2/libxml/xinclude.h>
#include <libxml2/libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WIN32
	#ifdef LIBBMDXSLT_EXPORTS
		#define LIBBMDXSLT_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDXSLT_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDXSLT_SCOPE
		#endif
	#endif
#else
	#define LIBBMDXSLT_SCOPE
#endif	// ifdef WIN32

#if defined(__cplusplus)
extern "C" {
#endif

LIBBMDXSLT_SCOPE long convertXmlToHtml(	char *xml,
				char *xsl,
				char *html);

LIBBMDXSLT_SCOPE long convertXmlFileToHtmlBuf(char *xmlPath, char *xsltPath, char **resultBuffer, long *resultBufferSize);
LIBBMDXSLT_SCOPE long convertXmlBufToHtmlBuf(char *xmlBuf, long xmlSize, char *xsltPath, char **resultBuffer, long *resultBufferSize);
#if defined(__cplusplus)
}
#endif



#endif //infndef _LIBBMDXSLT_INCLUDED_
