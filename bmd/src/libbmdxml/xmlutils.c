#include <bmd/libbmdxml/libbmdxml.h>
#include <bmd/libbmderr/libbmderr.h>

/** Find all nodes using #xpath . 
 * 
 * @param document document to use
 * @param xpath path to object
 * @param result will contain search result; must be freed with xmlXPathFreeObject(). 
 * 
 * @retval 0 everything is ok
 * @retval <0 error occured
 */
 
long bmdxml_find_nodes (xmlDocPtr document, const char * xpath, xmlNodeSetPtr * result)
{
	xmlXPathContextPtr xpathCtx; 
    	xmlXPathObjectPtr xpathObj;
	xmlChar *prefix_ds = (xmlChar*) "ds";
	xmlChar *href_ds = (xmlChar*) "http://www.w3.org/2000/09/xmldsig#";
	xmlChar *prefix_xades = (xmlChar*) "xades";
	xmlChar *href_xades = (xmlChar*) "http://uri.etsi.org/01903/v1.3.2#";
	xmlChar *prefix_f = (xmlChar*) "f";
	xmlChar *href_f = (xmlChar*) "http://www.unizeto.pl/szablon/formularz/1";
	xmlChar *prefix_ndap = (xmlChar*) "ndap";
	xmlChar *href_ndap = (xmlChar*) "http://www.mswia.gov.pl/standardy/ndap";
	xmlChar *prefix_xsi = (xmlChar*) "xsi";
	xmlChar *href_xsi = (xmlChar*) "http://www.w3.org/2001/XMLSchema-instance";
		
	if (!(document != NULL))
	{
		PRINT_ERROR("Pointer to document cannot be NULL!\n");
		abort();
	}
	if (!(xpath != NULL))
	{
		PRINT_ERROR("xpath string cannot be NULL!\n");
		abort();
	}
	if (!(result != NULL && *result == NULL))
	{
		PRINT_ERROR("Pointer to results cannot be null! Results variable MUST be null!\n");
		abort();
	}

	xpathCtx = xmlXPathNewContext(document);
	if(xpathCtx == NULL) {
        		PRINT_DEBUG("Error: unable to create new XPath context\n"); 
        		return(-1);
    	}

		/* register namespace ds*/
		if(xmlXPathRegisterNs(xpathCtx, prefix_ds, href_ds) != 0)
		{
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix_ds, href_ds);
			return(-1);
		}

		/* register namespace xades*/
		if(xmlXPathRegisterNs(xpathCtx, prefix_xades, href_xades) != 0)
		{
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix_xades, href_xades);
			return(-1);
		}
		
		/* register namespace f*/
		if(xmlXPathRegisterNs(xpathCtx, prefix_f, href_f) != 0)
		{
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix_f, href_f);
			return(-1);
		}
		
		/* register namespace ndap*/
		if(xmlXPathRegisterNs(xpathCtx, prefix_ndap, href_ndap) != 0)
		{
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix_ndap, href_ndap);
			return(-1);
		}
		
		/* register namespace xsi*/
		if(xmlXPathRegisterNs(xpathCtx, prefix_xsi, href_xsi) != 0)
		{
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix_xsi, href_xsi);
			return(-1);
		}
	
	xpathObj = xmlXPathEvalExpression((xmlChar*)xpath, xpathCtx);
	if(xpathObj == NULL) {
        		PRINT_DEBUG("Error: unable to evaluate xpath expression \"%s\"\n", xpath);
		xmlXPathFreeContext(xpathCtx); 
		return(-2);
    	}
    	if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)){
                PRINT_VDEBUG("Found no items\n");
	}
	
	*result = xpathObj->nodesetval;
	xpathObj->nodesetval = NULL;
	
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx); 
	return 0;
}

/** Read value of a given node */
long bmdxml_get_node_value_by_xpath (xmlDocPtr document, const char * xpath, char ** outbuff)
{
	long status = 0;
	xmlNodeSetPtr nodes  = NULL;
	xmlChar * value = NULL;
	
	if (!(document != NULL))
	{
		PRINT_ERROR("document nie może być null!\n");
		abort();
	}
	if(!(xpath != NULL && *xpath != 0))
	{
		PRINT_ERROR("xpath nie może być null i *xpath nie może być 0!\n");
		abort();
	}
	if(!(outbuff != NULL && *outbuff == NULL))
	{
		PRINT_ERROR("outbuff nie może być null i *outbuff musi być null!\n");
		abort();
	}
	
	if ((status=bmdxml_find_nodes(document,xpath,&nodes))!=0) {
		PRINT_DEBUG("xades_find_nodes(%s) failed\n",xpath);
		return status;
	}
	
	if( nodes == NULL )
	{
		PRINT_DEBUG("nodes was NULL\n");
		*outbuff = NULL;
		return -1;
	}
	if( nodes->nodeNr != 1 )  {
		PRINT_DEBUG("Invalid number of matching nodes for xpath %s: %i, should be 1\n",
			xpath, 
			nodes->nodeNr);

		if(nodes!=NULL)
			xmlXPathFreeNodeSet(nodes);
		*outbuff = NULL;
		return -2;
	}
	bmd_assert(nodes != NULL &&  nodes->nodeTab != NULL && nodes->nodeTab[0] != NULL, "LK");
	value = xmlNodeListGetString(document, nodes->nodeTab[0]->xmlChildrenNode, 1);
	
	if (value == NULL) {
		PRINT_VDEBUG("Tag %s exists, but is empty!\n",xpath);
		value = (xmlChar*) strdup("");
	}
	
	*outbuff = (char*) malloc (xmlStrlen(value)+1);
	strcpy (*outbuff, (char*) value);
	PRINT_VDEBUG("%s = %s\n",xpath, *outbuff);

	if(nodes!=NULL)
		xmlXPathFreeNodeSet(nodes);
	xmlFree(value);
	
	return 0;
}

long bmdxml_get_node_by_xpath(xmlDocPtr doc, xmlChar *xpath, xmlNodePtr *new_node)
{
	long status = 0;
	xmlXPathObjectPtr result = NULL;
	xmlNodeSetPtr nodeset = NULL;/*to nas interesuje,bedzie do zwolnienia w przyszlosci*/

	if (doc == NULL)
	{
		PRINT_DEBUG("Please parse document before using it!\n");
		return ERR_arg+1;
	}
	if (xpath == NULL)
	{
		PRINT_DEBUG("No xpath is given!\n");
		return ERR_arg+2;
	}
	if (new_node == NULL || *new_node != NULL)
	{
		PRINT_DEBUG("Wrong argument 3!\n");
		return ERR_arg+3;
	}
	status = bmdxml_find_nodes(doc, (char*) xpath, &nodeset);
	if (status != 0)
	{
		PRINT_DEBUG("Error while finding node with xpath %s!\n", xpath);
		return ERR_parser+1;
	}
	
	if (nodeset == NULL)
	{
		PRINT_DEBUG("Zle zapytanie - brak pasujacych tagow dla '%s'...\n", xpath);
		xmlXPathFreeObject(result);
		return ERR_parser+4;
	}
	if (nodeset->nodeNr > 1)
	{
		PRINT_DEBUG("Niejednoznaczne zapytanie... Za duzo pasujacych tagow dla '%s'...\n", xpath);
		xmlXPathFreeObject(result);
		return ERR_parser+4;
	}
	if (nodeset->nodeNr == 0)
	{
		PRINT_DEBUG("Zle zapytanie - brak pasujacych tagow dla '%s'...\n", xpath);
		xmlXPathFreeObject(result);
		return ERR_parser+4;
	}
	*new_node = nodeset->nodeTab[0];
	nodeset->nodeTab[0] = NULL;
	if (nodeset != NULL)
	{
		xmlXPathFreeNodeSet(nodeset);
	}

	return 0;
}

long bmdxml_transfer_node_content(xmlDocPtr docin, char *xpathin,
											xmlDocPtr docout, char *xpathout)
{
	xmlNodePtr nodeIN		= NULL;/*galaz w drzewie we*/
	xmlNodePtr nodeOUT	= NULL;/*galaz w drzewie wy*/
	xmlChar *content		= NULL;/*transferowana tresc*/
	long status = 0;

	/*weryfikacja poprawnosci wywolania*/
	if (docin == NULL)
	{
		PRINT_DEBUG("Dokument wejsciowy nie jest przeparsowany!\n");
		return ERR_arg+1;
	}
	if (docout == NULL)
	{
		PRINT_DEBUG("Dokument wyjsciowy nie jest przeparsowany!\n");
		return ERR_arg+3;
	}
	if (xpathin == NULL)
	{
		PRINT_DEBUG("Prosze podac xpath do danych w dok. wejsciowym!\n");
		return ERR_arg+2;
	}
	if (xpathout == NULL)
	{
		PRINT_DEBUG("Prosze podac xpath do danych w dok. wyjsciowym!\n");
		return ERR_arg+4;
	}

	/*pobieramy uchwyty do interesujacych nas galezi*/
	status = bmdxml_get_node_by_xpath(docin, (xmlChar*) xpathin, &nodeIN);
	if (status != 0)
	{
		PRINT_DEBUG("Blad przy pobieraniu taga %s!\n", xpathin);
		return ERR_parser+1;
	}
	status = bmdxml_get_node_by_xpath(docout, (xmlChar*) xpathout, &nodeOUT);
	if (status != 0)
	{
		PRINT_DEBUG("Blad przy pobieraniu taga %s!\n", xpathout);
		return ERR_parser+2;
	}

	content = xmlXPathCastNodeToString(nodeIN);
	if (content == NULL)
	{
		PRINT_ERROR("Brak pamieci!\n");
		return ERR_sys+1;
	}
	
	xmlNodeSetContent(nodeOUT, content);
	free(content);
	return 0;
}

long bmdxml_set_content_at_xpath(xmlDocPtr doc, char *xpath, char *content)
{
	xmlNodePtr nodeOUT = NULL;
	long status = 0;
	if (doc == NULL)
	{
		PRINT_DEBUG("Wrong argument 1 - document not parsed!\n");
		return ERR_arg+1;
	}
	if (xpath == NULL)
	{
		PRINT_DEBUG("Wrong argument 2 - no xpath given!\n");
		return ERR_arg+2;
	}
	if (content == NULL)
	{
		PRINT_DEBUG("Wrong argument 3 - no content given!\n");
		return ERR_arg+2;
	}

	/*wyszukujemy tag w xmlu, do ktorego wstawimy zawartosc*/
	status = bmdxml_get_node_by_xpath(doc, (xmlChar*) xpath, &nodeOUT);
	if (status != 0)
	{
		PRINT_DEBUG("Blad przy wyszukiwaniu taga %s!\n", xpath);
		return ERR_parser+1;
	}

	/*ustawiamy zawartosc w znalezionym tagu*/
	xmlNodeSetContent(nodeOUT, (xmlChar*) content);

	return 0;
}


long bmdxml_doc2genbuf(xmlDocPtr doc, GenBuf_t **bufor)
{
	int temp	= 0;

	if (doc == NULL)
	{
		PRINT_DEBUG("Wrong argument 1: xmlDocPtr!\n");
		return ERR_arg+1;
	}
	if (bufor == NULL || *bufor != NULL)
	{
		PRINT_DEBUG("Wrong argument 2: GenBuf_t!\n");
		return ERR_arg+2;
	}
	
	*bufor = (GenBuf_t*) malloc (sizeof(GenBuf_t));
	if (*bufor == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return ERR_sys+1;
	}

	/*zapisujemy do genbufa zawartosc dokumentu*/
	temp = (*bufor)->size;
	xmlDocDumpFormatMemoryEnc(doc, (unsigned char**)(&((*bufor)->buf)), &temp, "UTF-8", 1);
	(*bufor)->size = temp;
	if ((*bufor)->buf == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return ERR_sys+2;
	}
	
	return 0;
}

long bmdxml_doc2buffer(xmlDocPtr doc, char **bufor, long *len)
{
	int temp	= 0;

	if (doc == NULL)
	{
		PRINT_DEBUG("Wrong argument 1: xmlDocPtr!\n");
		return ERR_arg+1;
	}
	if (bufor == NULL || *bufor != NULL)
	{
		PRINT_DEBUG("Wrong argument 2: char should not be allocated!\n");
		return ERR_arg+2;
	}
	
	/*zapisujemy zawartosc dokumentu*/
	if (len == NULL)/*bez zapisu dlugosci*/
	{
		xmlDocDumpFormatMemory(doc, (xmlChar**) bufor, &temp, 1);
	}
	else
	{
		temp = *len;
		xmlDocDumpFormatMemory(doc, (xmlChar**) bufor, &temp, 1);
		*len = temp;
	}
	if (*bufor == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return ERR_sys+1;
	}
	
	return 0;
}
