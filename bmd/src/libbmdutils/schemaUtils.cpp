#include <bmd/libbmdutils/libbmdutils2.h>


/* 
funkcja bmd_validate_xml_buf() parsuje XML oraz XML Schema przekazane w buforach
a nastepnie wykonuje walidacje XML za pomoca tej XML Schema

@param xmlBuf to bufor z xml,ktory ma zostac zwalidowany
@param xmlBufSize to dlugosc bufora xmlBuf
@param schemaBuf to bufor z XML Schema
@param schemaBufSize to dlugosc schemaBuf

@retval 0 jesli pozytywnie zwalidowany XML za pomoca XML Schema
@retval < 0 jesli blad/XML zwalidowany negatywnie
 */
long bmd_validate_xml_buf(char *xmlBuf, long xmlBufSize, char *schemaBuf, long schemaBufSize)
{
	long retVal=0;
	xmlDocPtr xmlDocument=NULL;
	xmlSchemaParserCtxtPtr schemaParser=NULL;
	xmlSchemaPtr schemaDocument=NULL;
	xmlSchemaValidCtxtPtr schemaValidation=NULL;

	if(xmlBuf == NULL)
	{
		return -1;
	}
	if(xmlBufSize <= 0)
	{
		return -2;
	}
	if(schemaBuf == NULL)
	{
		return -3;
	}
	if(schemaBufSize <= 0)
	{
		return -4;
	}

	xmlDocument=xmlParseMemory(xmlBuf, xmlBufSize);
	if(xmlDocument == NULL)
	{
		return -5;
	}

	schemaParser=xmlSchemaNewMemParserCtxt(schemaBuf, schemaBufSize);
	if(schemaParser == NULL)
	{
		xmlFreeDoc(xmlDocument); xmlDocument=NULL;
		return -6;
	}

	schemaDocument=xmlSchemaParse(schemaParser);
	if(schemaDocument == NULL)
	{
		xmlSchemaFreeParserCtxt(schemaParser); schemaParser=NULL;
		xmlFreeDoc(xmlDocument); xmlDocument=NULL;
		return -7;
	}

	schemaValidation=xmlSchemaNewValidCtxt(schemaDocument);
	if(schemaValidation == NULL)
	{
		xmlSchemaFree(schemaDocument); schemaDocument=NULL;
		xmlSchemaFreeParserCtxt(schemaParser); schemaParser=NULL;
		xmlFreeDoc(xmlDocument); xmlDocument=NULL;
		return -7;
	}

	retVal=(long)xmlSchemaValidateDoc(schemaValidation, xmlDocument);
	xmlSchemaFreeValidCtxt(schemaValidation); schemaValidation=NULL;
	xmlSchemaFree(schemaDocument); schemaDocument=NULL;
	xmlSchemaFreeParserCtxt(schemaParser); schemaParser=NULL;
	xmlFreeDoc(xmlDocument); xmlDocument=NULL;
	if(retVal != 0)
	{
		return -8;
	}

	return 0;
}

/*

funkcja bmd_validate_xml_file() parsuje plik XML oraz plik XML Schema
a nastepnie wykonuje walidacje XML za pomoca XML Schema

@param xmlPath to sciezka (UTF-8) do pliku podlegajacego walidacji
@param schemaPath to sciezka (UTF-8) do pliku XML Schema

@retval 0 jesli pozytywnie zwalidowany XML za pomoca XML Schema
@retval < 0 jesli blad/XML zwalidowany negatywnie

*/
long bmd_validate_xml_file(	char *xmlPath, char *schemaPath)
{
	long retVal=0;
	GenBuf_t *xmlBuf=NULL;
	GenBuf_t *schemaBuf=NULL;

#ifdef WIN32
	wchar_t* widePath=NULL;
#endif //ifdef WIN32

	if(xmlPath == NULL)
	{
		return -11;
	}
	if(strlen(xmlPath) < 0)
	{
		return -12;
	}
	if(schemaPath == NULL)
	{
		return -13;
	}
	if(strlen(schemaPath) < 0)
	{
		return -14;
	}

#ifdef WIN32
	UTF8StringToWindowsString(xmlPath, &widePath, NULL);
	retVal=bmd_load_binary_content_wide(widePath, &xmlBuf);
	free(widePath); widePath=NULL;
#else
	retVal=bmd_load_binary_content(xmlPath, &xmlBuf);
#endif //ifdef WIN32
	if(retVal != 0)
	{
		return -15;
	}
	
#ifdef WIN32
	UTF8StringToWindowsString(schemaPath, &widePath, NULL);
	retVal=bmd_load_binary_content_wide(widePath, &schemaBuf);
	free(widePath); widePath=NULL;
#else
	retVal=bmd_load_binary_content(schemaPath, &schemaBuf);
#endif //ifdef WIN32
	if(retVal != 0)
	{
		free_gen_buf(&xmlBuf);
		return -16;
	}

	retVal=bmd_validate_xml_buf(xmlBuf->buf, xmlBuf->size, schemaBuf->buf, schemaBuf->size);
	free_gen_buf(&xmlBuf);
	free_gen_buf(&schemaBuf);
	return retVal;
}
