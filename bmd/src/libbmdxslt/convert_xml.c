#include <bmd/libbmdxslt/libbmdxslt.h>

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

long convertXmlToHtml(	char *xml,
				char *xsl,
				char *html)
{
xsltStylesheetPtr cur 	= NULL;
xmlDocPtr doc		= NULL;
xmlDocPtr res		= NULL;
FILE *ptr			= NULL;
char *params[3];

	PRINT_INFO("LIBBMDXSLTINF Converting xml %s to html %s\n",xml,html);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (xml==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Invalid xml file path.\n\tError: %s.\n\tError=%i\n",GetErrorMsg(BMD_ERR_PARAM1),BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if (xsl==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Invalid xsl file path.\n\tError: %s.\n\tError=%i\n",GetErrorMsg(BMD_ERR_PARAM2),BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if (html==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Invalid html file path.\n\tError: %s.\n\tError=%i\n",GetErrorMsg(BMD_ERR_PARAM3),BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	/******************************/
	/*	dobranie plikow parami	*/
	/******************************/
	asprintf(&params[0], "%s", xml);
	asprintf(&params[1], "%s", xsl);
	params[2] = NULL;

	/******************************************/
	/*	ustawienie zmiennych srodowiskowych	*/
	/******************************************/
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	/******************************/
	/*	parsowanie pliku xsl	*/
	/******************************/
	cur=xsltParseStylesheetFile((const xmlChar *)xsl);
	if (cur==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Error in parsing xsl %s file.\n\tError: %s.\n\tError=%i\n", xsl, GetErrorMsg(BMDSOAP_SERVER_PARSE_XSL_ERROR),BMDSOAP_SERVER_PARSE_XSL_ERROR);
		return BMDSOAP_SERVER_PARSE_XSL_ERROR;
	}

	/************************/
	/*	parsowanie xml	*/
	/************************/
	doc=xmlParseFile(xml);
	if (doc==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Error in parsing xml %s file.\n\tError: %s.\n\tError=%i\n", xml, GetErrorMsg(BMDSOAP_SERVER_PARSE_XML_ERROR),BMDSOAP_SERVER_PARSE_XML_ERROR);
		return BMDSOAP_SERVER_PARSE_XML_ERROR;
	}

	/************************************/
	/*	zastosowanie wzorca na zrodle	*/
	/************************************/
	res=xsltApplyStylesheet(cur, doc, (const char **)params);
	if (res==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Error in applying stylesheet.\n\tError: %s.\n\tError=%i\n", GetErrorMsg(BMDSOAP_SERVER_APPLY_STYLE_ERROR),BMDSOAP_SERVER_APPLY_STYLE_ERROR);
		return BMDSOAP_SERVER_APPLY_STYLE_ERROR;
	}

	/************************************/
	/*	zapisanie wyniku do pliku	*/
	/************************************/
	ptr=fopen(html,"w+");
	if (ptr==NULL)
	{
		PRINT_ERROR("LIBBMDXSLTERR Error in saving conversion result.\n\tError: %s.\n\tError=%i\n", GetErrorMsg(BMDSOAP_SERVER_SAVE_HTML_ERROR),BMDSOAP_SERVER_SAVE_HTML_ERROR);
		return BMDSOAP_SERVER_SAVE_HTML_ERROR;
	}
	else
	{
		xsltSaveResultToFile(ptr, res, cur);
		fclose(ptr);
	}

	/******************/
	/*	porzadki	*/
	/******************/
	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

	xsltCleanupGlobals();
	xmlCleanupParser();

	return BMD_OK;
}

/*by WSZ - Windows friendly*/
/**
funkcja convertXmlFileToHtmlBuf() wykonuje transformacje na XML z pliku, przy uzyciu wskazanego pliku XSLT

@param xmlPath to sciezka do pliku z wejsciowym XML
@param xsltPath to sciezka do pliku .XSLT
@param resultBuffer to buffor z wynikiem transforamacji
@param resultBufferSize to dlugosc bufora resultBuffer

@retval 0 jesli sukces
@retval -1 jesli xmlPath jest NULL'em
@retval -2 jesli xmlPath to lanuch pusty
@retval -3 jesli xsltPath jest NULL
@retval -4 jesli xsltPath jest lanuchem pustym
@retval -5 jesli jesli resultBuffer jest NULL'em
@retval -6 jesli wartosc wyluskana z resultBuffer nie jest wyNULLowana
@retval -7 jesli resultBufferSize jest NULL'em
@retval -8 jesli nie mozna sparsowac XSLT
@retval -9 jesli nie mozna sparsowac XML
@retval -10 jesli blad podczas transformacji
@retval -11 jesli zawiodl zapis wyniku do bufora
*/

long convertXmlFileToHtmlBuf(char *xmlPath, char *xsltPath, char **resultBuffer, long *resultBufferSize)
{
xsltStylesheetPtr usedXSLT=NULL;
xmlDocPtr XMLdocument=NULL;
xmlDocPtr XMLresult=NULL;
char* buffer=NULL;
long bufferSize=0;
int si_temp=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(xmlPath==NULL)
		{ return -1; }
	if(strlen(xmlPath) == 0)
		{ return -2; }
	if(xsltPath==NULL)
		{ return -3; }
	if(strlen(xsltPath) == 0)
		{ return -4; }
	if(resultBuffer == NULL)
		{ return -5; }
	if(*resultBuffer != NULL)
		{ return -6; }
	if(resultBufferSize == NULL)
		{ return -7; }
	

	/******************************************/
	/*	ustawienie zmiennych srodowiskowych	*/
	/******************************************/
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	/******************************/
	/*	parsowanie pliku .xsl	*/
	/******************************/
	usedXSLT=xsltParseStylesheetFile((const xmlChar *)xsltPath);
	if(usedXSLT == NULL)
	{
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -8;
	}

	/************************/
	/*	parsowanie dokumentu .xml	*/
	/************************/
	XMLdocument=xmlParseFile(xmlPath);
	if(XMLdocument == NULL)
	{
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -9;
	}

	/************************************/
	/*	transformacja XSLT	*/
	/************************************/
	XMLresult=xsltApplyStylesheet(usedXSLT, XMLdocument, NULL);
	if(XMLresult == NULL)
	{
		xmlFreeDoc(XMLdocument);
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -10;
	}

	/************************************/
	/*	zapisanie do bufora	*/
	/************************************/
	si_temp=bufferSize;	
	xmlDocDumpMemory(XMLresult, (unsigned char**)(&buffer), &si_temp);
	bufferSize=si_temp;

	if(buffer == NULL)
	{
		xmlFreeDoc(XMLdocument);
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -11;
	}

	xsltFreeStylesheet(usedXSLT);
	xmlFreeDoc(XMLresult);
	xmlFreeDoc(XMLdocument);

	xsltCleanupGlobals();
	xmlCleanupParser();


	*resultBuffer=buffer;
	*resultBufferSize=bufferSize;
	return 0;
}

/*by WSZ - Windows friendly*/
/**
funkcja convertXmlBufToHtmlBuf() wykonuje transformacje na XML w buforze przy uzyciu wskazanego pliku XSLT

@param xmlBuf to bufor z wejsciowym XML
@param xmlSize to rozmiar bufora xmlBuf
@param xsltPath to sciezka do pliku .XSLT
@param resultBuffer to buffor z wynikiem transforamacji
@param resultBufferSize to dlugosc bufora resultBuffer

@retval 0 jesli sukces
@retval -1 jesli xmlBuf jest NULL'em
@retval -2 jesli xmlSize <= 0
@retval -3 jesli xsltPath jest NULL
@retval -4 jesli xsltPath jest lanuchem pustym
@retval -5 jesli jesli resultBuffer jest NULL'em
@retval -6 jesli wartosc wyluskana z resultBuffer nie jest wyNULLowana
@retval -7 jesli resultBufferSize jest NULL'em
@retval -8 jesli nie mozna sparsowac XSLT
@retval -9 jesli nie mozna sparsowac XML
@retval -10 jesli blad podczas transformacji
@retval -11 jesli zawiodl zapis wyniku do bufora
*/
long convertXmlBufToHtmlBuf(char *xmlBuf, long xmlSize, char *xsltPath, char **resultBuffer, long *resultBufferSize)
{
xsltStylesheetPtr usedXSLT=NULL;
xmlDocPtr XMLdocument=NULL;
xmlDocPtr XMLresult=NULL;
char* buffer=NULL;
long bufferSize=0;
int si_temp=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(xmlBuf==NULL)
		{ return -1; }
	if(xmlSize <= 0)
		{ return -2; }
	if(xsltPath == NULL)
		{ return -3; }
	if(strlen(xsltPath) == 0)
		{ return -4; }
	if(resultBuffer == NULL)
		{ return -5; }
	if(*resultBuffer != NULL)
		{ return -6; }
	if(resultBufferSize == NULL)
		{ return -7; }

	/******************************************/
	/*	ustawienie zmiennych srodowiskowych	*/
	/******************************************/
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	/******************************/
	/*	parsowanie pliku .xsl	*/
	/******************************/
	usedXSLT=xsltParseStylesheetFile((const xmlChar *)xsltPath);
	if(usedXSLT == NULL)
	{
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -8;
	}

	/************************/
	/*	parsowanie dokumentu .xml	*/
	/************************/
	XMLdocument=xmlParseMemory((const char *)xmlBuf, xmlSize);
	if(XMLdocument == NULL)
	{
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -9;
	}

	/************************************/
	/*	transformacja XSLT	*/
	/************************************/
	XMLresult=xsltApplyStylesheet(usedXSLT, XMLdocument, NULL);
	if(XMLresult == NULL)
	{
		xmlFreeDoc(XMLdocument);
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -10;
	}

	/************************************/
	/*	zapisanie do bufora	*/
	/************************************/
	si_temp	= bufferSize;
	xmlDocDumpMemory(XMLresult, (unsigned char**)(&buffer), &si_temp);
	bufferSize = si_temp;

	if(buffer == NULL)
	{
		xmlFreeDoc(XMLdocument);
		xsltFreeStylesheet(usedXSLT);
		xsltCleanupGlobals();
		xmlCleanupParser();
		return -11;
	}

	xsltFreeStylesheet(usedXSLT);
	xmlFreeDoc(XMLresult);
	xmlFreeDoc(XMLdocument);

	xsltCleanupGlobals();
	xmlCleanupParser();


	*resultBuffer=buffer;
	*resultBufferSize=bufferSize;
	return 0;
}

