/** @file
XAdES implementation, utility functions 

@bug może mieszać się z inicjalizacją xmla w libbmdxml 
*/

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#endif /*XMLSEC_NO_XSLT */

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/xmlenc.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include <xmlsec/io.h>

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdxades/libbmdxades.h>
#include <bmd/libbmdxades/xmlio.h>

/** Initialize XAdES library */
long xades_init(void)
{
	long status= 0;
	xmlInitParser();
	LIBXML_TEST_VERSION
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
	xmlSubstituteEntitiesDefault(1);
#ifndef XMLSEC_NO_XSLT
	xmlIndentTreeOutput = 1;
#endif				/* XMLSEC_NO_XSLT */

	/* Init xmlsec library */
	if (xmlSecInit() < 0) {
		fprintf(stderr, "Error: xmlsec initialization failed.\n");
		return (-1);
	}




	/* Check loaded library version */
	if (xmlSecCheckVersion() != 1) {
		fprintf(stderr,
			"Error: loaded xmlsec library version is not compatible.\n");
		return (-1);
	}

	/* Load default crypto engine if we are supporting dynamic
	 * loading for xmlsec-crypto libraries. Use the crypto library
	 * name ("openssl", "nss", etc.) to load corresponding 
	 * xmlsec-crypto library.
	 */
#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
	if (xmlSecCryptoDLLoadLibrary(BAD_CAST "nss") < 0) { /* było "nss" */
		fprintf(stderr,
			"Error: unable to load default xmlsec-crypto library. Make sure\n"
			"that you have it installed and check shared libraries path\n"
			"(LD_LIBRARY_PATH) envornment variable.\n");
		return (-1);
	}
#endif				/* XMLSEC_CRYPTO_DYNAMIC_LOADING */

	/* Init crypto library */
	if (xmlSecCryptoAppInit(NULL) < 0) {
		fprintf(stderr, "Error: crypto initialization failed.\n");
		return (-1);
	}

	/* Init xmlsec-crypto library */
	if (xmlSecCryptoInit() < 0) {
		fprintf(stderr,
			"Error: xmlsec-crypto initialization failed.\n");
		return (-1);
	}

	/* register my own input callbacks */
	xmlSecIOCleanupCallbacks();
	status = xmlSecIORegisterCallbacks(bmdxml_IO_curl_Match, bmdxml_IO_curl_Open,
                              bmdxml_IO_curl_Read, bmdxml_IO_curl_Close);
	if(status != 0) {
		PRINT_DEBUG("%s cannot register curl callbacks: error %li\n",__FUNCTION__,status);
        	return(-1);
	} 

/*	
	status = xmlRegisterInputCallbacks(bmdxml_IO_curl_Match, bmdxml_IO_curl_Open,
                              bmdxml_IO_curl_Read, bmdxml_IO_curl_Close);
	if(status < 0) {
		PRINT_DEBUG("%s cannot register curl callbacks: error %i\n",__FUNCTION__,status);
        	return(-1);
	} 
*/
	/* end of input callbacks */


	PRINT_VDEBUG("bmdxml_IO_curl_* registered. :) \n");


	return 0;
}

/** Destroy XAdES configuration */
long xades_destroy(void)
{
	/* Shutdown xmlsec-crypto library */
	xmlSecCryptoShutdown();

	/* Shutdown crypto library */
	xmlSecCryptoAppShutdown();

	/* Shutdown xmlsec library */
	xmlSecShutdown();

	/* Shutdown libxslt/libxml */
#ifndef XMLSEC_NO_XSLT
	xsltCleanupGlobals();
#endif				/* XMLSEC_NO_XSLT */
	xmlCleanupParser();

	return (0);
}

