#define _WINSOCK2API_
#include <bmd/libbmdutils/libbmdutils2.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#define CSV_ICONV_IN_BUF_SIZE		768
#define CSV_ICONV_OUT_BUF_SIZE		3072

typedef struct bmd_err_heap_t
{
	int * err_code_heap;
	char ** err_str_heap;
	long heap_size;
}	bmd_err_heap_t;

#define LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME		512001
#define OID_NOT_FOUND_IN_DATAGRAM							69

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4311) /* pointer truncation - na razie tak pozniej sie zastanowie - MSil */
#endif
/************************************************/
/*  deklaracje lokalne dla funkcji biblioteki   */
/************************************************/

long bmdInitMtdDtgPaths( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, char ***errors );
long _bmdWriteDatagramMetadataInXML2GenBuf( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t*bmdOidXpath, GenBuf_t *content, long with_xml_header, char ***errors );
long _bmdWriteDatagramMetadataInCSV2GenBuf( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t*bmdOidXpath, GenBuf_t *content, long with_csv_header, char ***errors );

long _InsertRootTagOnXMLBuf(char* NewRootTag, char* NewRootAtributes, GenBuf_t * XMLBuf, long with_xml_header, char ***error);
long __GetXMLHeaderFromXMLBuf(GenBuf_t * Header, GenBuf_t * XMLBuf, GenBuf_t * newline, char ***error);
long __GetXMLRootFromXMLBuf(GenBuf_t * Root, GenBuf_t * XMLBuf, char ***error);
long _MergeDtgXMLs2DtgSetXML(GenBuf_t * DtgSetXMLBuf, GenBuf_t * DtgsXMLBufs, long DtgsXMLCnt, bmd_oid_xpath_t *bmdOidXpath, char ***error);

long ValidateCellSep(char* znak);
long ValidateTextSep(char* znak);

long change_code_page(bmd_oid_xpath_t * ctx, char ** in_out_buf);

void add_error(char*** errors_table, char*error);

/** Funkcja tworzaca/reallokujaca tablice bledow
*/
int realoc_err_heap(bmd_err_heap_t *err_heap, size_t new_size)
{
	if (err_heap == NULL)
	{
		return -1;
	}
	/* jesli nie bylo zaalokowane wczesniej*/
	if (err_heap->err_str_heap == NULL)
	{
		err_heap->err_str_heap = (char**) calloc(new_size, sizeof(char**));
		err_heap->err_code_heap = (int*) calloc(new_size, sizeof(int));
		err_heap->heap_size = (long)new_size;
	}
	else
	{
		PRINT_ERROR("Nie zaimplementowane reallokowanie stosu bledu bmd\n");
		return -1;
	}
	return BMD_OK;
}


/** Funkcja parsujaca stringowa tablice OidXpathow na tablice na potrzeby funkcji bmdInitBmdOidXpathStruct4XML()
	UWAGA - jakoze jest to funkcja wewnetrzna biblioteki, nie weryfikuje ona otrzymanych parametrow (zakladam, ze sa sprawdzone wczesniej)
*/
int _bmdReadXpathsTable( bmd_oid_xpath_t *bmdOidXpath, char **OidXpathsTable, char *** xpaths, char ***errors_ptr)
{
	int i=0, oid_cnt=0;
	char * OidXpath = NULL;
	char * Xpath = NULL;
	int errs_cnt=0;
	char **errors =NULL;
	char *error = NULL;
	char ** xpaths_ptr= NULL;
#ifdef DEBUG
	size_t buf_size=0;
#endif

	while (OidXpathsTable[oid_cnt++])								// zliczenie xpathow + 1
	{
	};

	/* inicjalizacja stosu bledow */
	errors = (char**) calloc(oid_cnt, sizeof(char*));
	if (errors_ptr)
	{
		*errors_ptr = errors;
	}

	if (oid_cnt > 100)											// na razie zakladam ze wiecej nie bedzie potrzebne - mozna zmienic jak sie okaze za malo
	{
		error = errors[errs_cnt++];
		asprintf(&error, "Error in _bmdReadXpathsTable parameters. XpathsTable size is too large (%ui).\n",oid_cnt);
		PRINT_DEBUG("Error in _bmdReadXpathsTable parameters. XpathsTable size is too large (%ui).\n",oid_cnt);
		return BMD_ERR_PARAM2;
	}

	bmdOidXpath->oid_cnt = oid_cnt-1;							//(rzeczywista liczba xpathow)
	bmdOidXpath->OIDTableBufs = (char**) calloc(oid_cnt, sizeof(char *));
	xpaths_ptr = (char**) calloc(oid_cnt, sizeof(char*));			//tablica bedzie zakonczona NULL

	/* przetwarzanie OidXpathow - separacja na OID i string z Xpath*/
	for(i=0; i < bmdOidXpath->oid_cnt ; i++)
	{
		OidXpath = OidXpathsTable[i];
#ifdef DEBUG
	#ifdef WIN32
		buf_size = strnlen( OidXpath, 1024);
	#else
		buf_size = strlen(OidXpath);
	#endif
		if (buf_size >=1024)
		{
			error = errors[errs_cnt++];
			asprintf(&error, "Error in _bmdReadXpathsTable parameters. XpathsTable %ui-th element is too large (%ui bytes).\n",i,buf_size);
			PRINT_DEBUG("Error in _bmdReadXpathsTable parameters. XpathsTable %ui-th element is too large (%ui bytes).\n",i,buf_size);
			return BMD_ERR_PARAM2;
		}
#endif
		Xpath = strrchr( OidXpath, '=');
		if ( (Xpath != NULL) && (Xpath[1] != 0) )					// znalaz sie znak '=' i cos za nim sie znajduje
		{
			Xpath[0] = 0;				// stawiamy znak konca linii zamiast znaku '=' rozdzielajac strin z OIdem oraz z string z Xpath
			(bmdOidXpath->OIDTableBufs[i])=(char *)malloc(sizeof(char)*(strlen(OidXpath)+1));
			memset(bmdOidXpath->OIDTableBufs[i], 0, strlen(OidXpath)+1);
			memcpy(bmdOidXpath->OIDTableBufs[i], OidXpath, strlen(OidXpath));

			if (bmdOidXpath->OIDTableBufs[i]!=NULL)
			{
				xpaths_ptr[i] = STRDUPL( Xpath+sizeof(char));			// kopiujemy xpath, ale od miejsca po znaku '='
			}
			else													//blad skladni w OID
			{
				error = errors[errs_cnt++];
				asprintf(&error, "Error in OIDXpath no. %ui. Wrong OID format: %s \n", i, OidXpath);
				PRINT_DEBUG("Error in OIDXpath no. %ui. Wrong OID format: %s \n", i, OidXpath);
			}
			Xpath[0] = '=';											// przywracamy znak '='
		}
		else
		{
			error = errors[errs_cnt++];
			asprintf(&error, "Error in OIDXpath no. %ui. Wrong OIDXpath format: %s \n", i, OidXpath);
			PRINT_DEBUG("Error in OIDXpath no. %ui. Wrong OIDXpath format: %s \n", i, OidXpath);
		}
		OidXpath = NULL;
		Xpath = NULL;
	}

	if (errs_cnt)
	{
		bmdOidXpath->oid_cnt = 0;
		i=0;
		while (bmdOidXpath->OIDTableBufs[i] != NULL)
		{
			free(bmdOidXpath->OIDTableBufs[i]); bmdOidXpath->OIDTableBufs[i] = NULL;

			if (xpaths_ptr[i])
			{
				free(xpaths_ptr[i]); xpaths_ptr[i] = NULL;
			}
			i++;
		}
		free(bmdOidXpath->OIDTableBufs); bmdOidXpath->OIDTableBufs = NULL;
		free(xpaths_ptr);	xpaths_ptr = NULL;

		return -(int)errs_cnt;
	}
	else
	{
		free(errors); errors = NULL;
		*errors_ptr = NULL;
	}
	*xpaths = xpaths_ptr;
	xpaths_ptr = NULL;
	return BMD_OK;
}

/************************************************/
/*			funkcje LIBBMDUTILS2 API			*/
/************************************************/


/**Funkcja inicjalizujaca strukture bmd_oid_xpath_t na potrzeby pobierania do XML
@note po zakonczeniu operacji na strukturze zwolnic ja nalezy za pomoca bmdFreeBmdOidXpathStructElements()
@param Doc wczytany dokument XML (np. bmdOpenXmlDoc() lub doc=xmlReadMemory((const char *)content->buf,content->size,NULL,NULL,0);)
@param bmdOidXpath wskaznik na istniejaca strukture kontekstu dla dowolnej funkcji Dtg2XML do zainicjalizowania - struktura musi juz istniec, ale nie powinna byc zmieniana recznie
@param OidXpathsTable tablica zakonczona NULL zawierajaca Xpath-y w formacie: OID=xpath (np. "1.2.616.1.113527.4.123=\\tagPierwszy\tagDrugiXpatha") - element tablicy (OID+sciezka nie moga byc dluzsze niz 1024 znaki (bajty), tablica nie moze miec wiecej niz 100 elementow
@param MtdXmlDstPath opcjonalne (moze byc NULL) - sciezka do folderu na zapisany plik metadanych (nazwa pliku okreslana jest na podstawie nazwy w datagramie, w funkcji bmdMtdDWriteXML2Files())
@param errors_ptr opcjonalne (moze byc NULL) - wskaznik na tablice stringow z komunikatami bledow
*/
long bmdInitBmdOidXpathStruct4XML(xmlDocPtr doc, bmd_oid_xpath_t *bmdOidXpath, char **OidXpathsTable, char ***errors_ptr)
{
	int retval=BMD_OK;
	int i=0;/*, oid_cnt=0;*/
	char ** xpaths =NULL;
#if 1
	if (doc == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("xmlDocPtr pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM2;
	}
	if (OidXpathsTable == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("XpathsTable pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if (OidXpathsTable[0] == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("XpathsTable table is empty.\n");
		return BMD_ERR_PARAM3;
	}
#endif

	/****************************/
	/* inicjalizowanie struktur	*/
	/****************************/

	retval = bmdxpath_init( doc, &bmdOidXpath->libbmdxpath_ctx);
	if (retval != BMD_OK)
	{
		//trzeba zastosowac strukture taka jak bmd_err_heap_t, i wtedy jakas funkcja dopisywac do niej kolejny blad
		// bo korzystalismy z funkcji ktora nie zapisuje bledow do stosu bledow errors_ptr
		// a musimy poinformowac ze napotkalismy na blad w funkcji bmdxpath_init()
		//errors = *errors_ptr
		//error = errors[errs_cnt++];
		//asprintf(&error, "Error in OIDXpath no. %ui. Wrong OIDXpath format: %s \n", i, OidXpath);

		free(bmdOidXpath->out_path);
		bmdOidXpath->out_path = NULL;

		return BMD_ERR_OP_FAILED;
	}
	retval = _bmdReadXpathsTable(bmdOidXpath,OidXpathsTable, &xpaths, errors_ptr);
	if (retval != BMD_OK)
	{
		// do errors_ptr dopislabym chyba jeszcze info o wyjsciu z funkcji bmdInitBmdOidXpathStruct4XML()
		bmdxpath_free_strings(&xpaths);
		free(bmdOidXpath->out_path);
		bmdOidXpath->out_path = NULL;
		bmdxpath_destroy(&bmdOidXpath->libbmdxpath_ctx);
		return BMD_ERR_OP_FAILED;
	}
	// zakladamy tablice na XMLNodes
	bmdOidXpath->XMLNodes = calloc((size_t)bmdOidXpath->oid_cnt,sizeof(xmlNodeSetPtr));
	if (bmdOidXpath->XMLNodes == NULL)
	{
		// do errors_ptr dopislabym chyba jeszcze info o wyjsciu z funkcji bmdInitBmdOidXpathStruct4XML()
		free(bmdOidXpath->out_path);
		bmdOidXpath->out_path = NULL;
		bmdxpath_destroy(&bmdOidXpath->libbmdxpath_ctx);
		return BMD_ERR_OP_FAILED;
	}

	//wyszukujemy NodeSets dla XPath'ow
	for (i=0; i < bmdOidXpath->oid_cnt ; i++)
	{
		if (xpaths[i])
		{
			retval = bmdxpath_find(bmdOidXpath->libbmdxpath_ctx, xpaths[i]);
			if (retval!=BMD_OK || bmdOidXpath->libbmdxpath_ctx->result->nodesetval==NULL)
			{
				PRINT_INFO("No XmlNode found for xpath:%s.\n",xpaths[i]);
				// zapisujemy blad do stosu, ale nie przerywamy petli, bo moze byl blad do pominiecia
			}
			else
			{
				if (bmdOidXpath->libbmdxpath_ctx && bmdOidXpath->libbmdxpath_ctx->result
					&& bmdOidXpath->XMLNodes && bmdOidXpath->XMLNodes[i]==NULL)
				{
					// zapisanie z info->result->nodesetval do Nodow
					bmdOidXpath->XMLNodes[i] = bmdOidXpath->libbmdxpath_ctx->result->nodesetval;
					bmdOidXpath->libbmdxpath_ctx->result->type = 0;
					bmdOidXpath->libbmdxpath_ctx->result->nodesetval = NULL;
				}
				else
				{
					//blad wewnetrzny - nie spodziewany, bo wczesniejsze funkcje nie zglaszaly bledu, ktory moglby miec na to wplyw
				}
			}
		}
		else
		{
			// zapisujemy blad do stosu, ale nie przerywamy petli, bo moze byl blad do pominiecia
		}
	}
	//czy wyczyscic stos bledow? bo skoro funkcja dotarla do konca to raczej poprawnie zakonczyla dzialanie
	bmdxpath_free_strings(&xpaths);

	return retval;
}

/**Funkcja inicjalizujaca strukture bmd_oid_xpath_t na potrzeby pobierania do CSV
@note po zakonczeniu operacji na strukturze zwolnic ja nalezy za pomoca bmdFreeBmdOidXpathStructElements()
@param Doc wczytany dokument XML (np. bmdOpenXmlDoc() lub doc=xmlReadMemory((const char *)content->buf,content->size,NULL,NULL,0);)
@param bmdOidXpath wskaznik na istniejaca strukture kontekstu dla dowolnej funkcji Dtg2XML do zainicjalizowania - struktura musi juz istniec, ale nie powinna byc zmieniana recznie
@param OidXpathsTable tablica zakonczona NULL zawierajaca Xpath-y w formacie: OID=xpath (np. "1.2.616.1.113527.4.123=\\tagPierwszy\tagDrugiXpatha") - element tablicy (OID+sciezka nie moga byc dluzsze niz 1024 znaki (bajty), tablica nie moze miec wiecej niz 100 elementow
@param MtdXmlDstPath opcjonalne (moze byc NULL) - sciezka do folderu na zapisany plik metadanych (nazwa pliku okreslana jest na podstawie nazwy w datagramie, w funkcji bmdMtdDWriteXML2Files())
@param errors_ptr opcjonalne (moze byc NULL) - wskaznik na tablice stringow z komunikatami bledow
*/
long bmdInitBmdOidXpathStruct4CSV(bmd_oid_xpath_t *bmdOidXpath, char **OidXpathsTable, char ***errors_ptr)
{
	int retval=BMD_OK;
	char ** xpaths =NULL;
#if 1
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (OidXpathsTable == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("XpathsTable pointer is NULL.\n");
		return BMD_ERR_PARAM2;
	}
	if (OidXpathsTable[0] == NULL)
	{
		PRINT_ERROR("Error in bmdInitBmdOidXpathStruct4XML parameters.\n");
		PRINT_DEBUG("XpathsTable table is empty.\n");
		return BMD_ERR_PARAM2;
	}
#endif

	/****************************/
	/* inicjalizowanie struktur	*/
	/****************************/

	retval = _bmdReadXpathsTable(bmdOidXpath,OidXpathsTable, &xpaths, errors_ptr);
	if (retval != BMD_OK)
	{
		// do errors_ptr dopislabym chyba jeszcze info o wyjsciu z funkcji bmdInitBmdOidXpathStruct4XML()
		bmdxpath_free_strings(&xpaths);
		free(bmdOidXpath->out_path);
		bmdOidXpath->out_path = NULL;
		bmdxpath_destroy(&bmdOidXpath->libbmdxpath_ctx);
		return BMD_ERR_OP_FAILED;
	}
	//czy wyczyscic stos bledow? bo skoro funkcja dotarla do konca to raczej poprawnie zakonczyla dzialanie
	bmdOidXpath->CSVNodes = xpaths;
	xpaths = NULL;
	return retval;
}

/* Funkcja zwalniajaca po bmdMtdD2XMLGetDownloadSettingsFromMDTML lub bmdMtdD2CSVGetDownloadSettingsFromMDTML
*/
long bmdFreeBmdOidXpathStructElements(	bmd_oid_xpath_t **bmdOidXpath,
							char ***errors_ptr)
{
long i					= 0;
char *TableBufPtr				= NULL;
xmlXPathObjectPtr XMLObjTmp		= NULL;
xmlDocPtr doc				= NULL;
bmd_oid_xpath_t *bmdOidXpathPtr	= NULL;
char **CSVNodesPtr			= NULL;

	if (bmdOidXpath==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*bmdOidXpath==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	bmdOidXpathPtr = *bmdOidXpath;
	if (bmdOidXpathPtr->libbmdxpath_ctx)
	{
		doc = bmdOidXpathPtr->libbmdxpath_ctx->ctx->doc;
	}

	if (bmdOidXpathPtr->cd != NULL)
	{
		if (iconv_close(bmdOidXpathPtr->cd) != 0)
		{
			//"Error Closing iconv";
			//error(FATAL, BAD_CLOSE);
		}
	}
	bmdOidXpathPtr->datagram_cnt = 0;
	bmdOidXpathPtr->datagramset_cnt = 0;
	bmdOidXpathPtr->DtgPrgrsCallback = NULL;
	bmdOidXpathPtr->DtgSetPrgrsCallback = NULL;
	if (bmdOidXpathPtr)
	{
		CSVNodesPtr = bmdOidXpathPtr->CSVNodes;
	}

	for (i=0; i < bmdOidXpathPtr->oid_cnt; i++)
	{
		if (bmdOidXpathPtr->XMLNodes)
		{
			// zwolnienie node setow
			XMLObjTmp = calloc(1,sizeof(xmlXPathObject));
			XMLObjTmp->type=XPATH_NODESET;
			XMLObjTmp->nodesetval = bmdOidXpathPtr->XMLNodes[i];
			xmlXPathFreeObject(XMLObjTmp);
			XMLObjTmp = NULL;
		}
		//zwolnienie tablicy pojedynczego OIDa
		if (bmdOidXpathPtr->OIDTableBufs)
		{
			TableBufPtr = bmdOidXpathPtr->OIDTableBufs[i];
			free(TableBufPtr); TableBufPtr = NULL;
		}
		if (CSVNodesPtr)
		{
			free(CSVNodesPtr[i]);
		}
	}
	bmdxpath_destroy(&bmdOidXpathPtr->libbmdxpath_ctx);
	free(bmdOidXpathPtr->DtgNodes);
	free(bmdOidXpathPtr->XMLNodes);
	free(bmdOidXpathPtr->OIDTableBufs);
	free(bmdOidXpathPtr->out_path);
	free(bmdOidXpathPtr->CSVNodes);
	free(bmdOidXpathPtr->CSV_header.buf);
	free(bmdOidXpathPtr->ColValType);
	free(bmdOidXpathPtr->ColCcvwqm);
	bmdOidXpathPtr->oid_cnt =0;

	bmdOidXpathPtr->DtgNodes = NULL;
	bmdOidXpathPtr->XMLNodes = NULL;
	bmdOidXpathPtr->OIDTableBufs = NULL;
	bmdOidXpathPtr->out_path = NULL;
	bmdOidXpathPtr->CSVNodes = NULL;
	bmdOidXpathPtr->CSV_header.buf = NULL;
	bmdOidXpathPtr->CSV_header.size = 0;
	bmdOidXpathPtr->cell_separator_char = 0;
	bmdOidXpathPtr->text_separator_char = 0;
	bmdOidXpathPtr->ColValType=NULL;
	bmdOidXpathPtr->ColCcvwqm=NULL;
	bmdOidXpathPtr->src_dec_separator = 0;
	bmdOidXpathPtr->dst_dec_separator = 0;
	CSVNodesPtr = NULL;
	bmdOidXpathPtr = NULL;
	xmlFreeDoc(doc);
	free(*bmdOidXpath);
	*bmdOidXpath = NULL;
	return BMD_OK;
}

/** Funkcja zapisujaca metadane odczytane z datagramSetu do plikow XML na podstawie szablonu i Xpath
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()

@param bmdDatagramSet wskaznik na datagramSet odpowiedzi na rzadanie wyszukiwania
@param process_num wskaznik na licznik datagramow do przetworzenia (opcjonalny)
@param process_num jezeli wskaznik nie jest NULL i wskazuje na 0 to zwroci liczbe datagramow jaka przetworzono
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param forms to zestaw formularzy, z ktorych jeden zostanie wykorzystany przy tworzeniu mapy indeksow metadanych
		(mozna podac NULL, ale wowczas moze nastapic sytuacja, ze w wynikach bedzie pusta wartosc dla jakiejs metadanej mimo, ze wiele dokumentow ma ta wartosc ustawiona)
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdMtdDWriteXML2Files(	bmdDatagramSet_t *bmdDatagramSet,
					long * process_num,
					bmd_oid_xpath_t *bmdOidXpath,
					const bmd_form_info_t *forms,
					char ***errors )
{
	bmd_err_heap_t tmp_err_heap;
	long iteracja=0;
	/*long err_heap_size = 0;*/
	long do_przetworzenia=0;
	int out_path_len = 0;
	int retval = BMD_OK;
	bmdDatagram_t *bmdDatagramPtr = NULL;
	char * tmpFilePath = NULL;
	GenBuf_t DtgContent;	//zawartosc datagramu w postaci XML
	char *dot = NULL;
	long dot_pos = 0;

#ifdef WIN32
	wchar_t *widePath=NULL;
#endif


	DtgContent.buf=NULL;
	DtgContent.size=0;

#if 1
	/** Walidacja parametrow */
	if (bmdDatagramSet==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->libbmdxpath_ctx == NULL || bmdOidXpath->XMLNodes == NULL
		|| bmdOidXpath->XMLNodes[0] == NULL || bmdOidXpath->OIDTableBufs ==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if ((process_num == NULL) || (*process_num == 0))
	{
		do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize; //- (int)(nie_koncowka)
	}
	else
	{
		if (*process_num > bmdDatagramSet->bmdDatagramSetSize)
		{
			do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize;
			PRINT_DEBUG("process_num set to %lu, but only %lu datagrams in DatagramSet.\n",
				*process_num, do_przetworzenia);
		}
		else
		{
			do_przetworzenia = *process_num;
		}
	}

	// sprawdzic jeszcze typ datagramu

	/** utworzenie tablicy bledow - dla kazdego datagramu po jednym rekordzie*/
	tmp_err_heap.err_str_heap = NULL;
	tmp_err_heap.err_code_heap = NULL;
	tmp_err_heap.heap_size = 0;
	realoc_err_heap( &tmp_err_heap, do_przetworzenia);

	if (do_przetworzenia == 0)
	{
		PRINT_DEBUG("No data to write.\n");
		// na stos z bledami!
		return BMD_OK;
	}

	if ( bmdOidXpath->DtgNodes == NULL)
	{
		PRINT_DEBUG("Initializing bmdOidXpath struct for metadata datagram paths.\n");
		//sprawdzic czy jest datagram - przeszed weryfikacje parametrow wiec powinien byc
		retval = bmdInitMtdDtgPaths( bmdDatagramSet->bmdDatagramSetTable[0], bmdOidXpath, forms, errors );
		if (retval != BMD_OK)
		{
			//powinnismy zwrocic blad z bmdInitMtdDtgPaths - jest w errors
			//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
			return retval;
		}
	}
	if (bmdOidXpath->DtgNodes == NULL || bmdOidXpath->DtgNodes[0].MtdType == 0)
	{
		//powinnismy zwrocic warrningi z bmdInitMtdDtgPaths jako errory - jest w errors
		//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
		return BMD_ERR_PARAM3;
	}
	if (bmdOidXpath->out_path == NULL || strlen(bmdOidXpath->out_path) < 1)
	{
		//zapisac w stosie bledow ze struktura nie zostala zainicjalizowana do zapisywania plikow XML
		return BMD_ERR_NOTINITED;
	}
#endif

	out_path_len = (int)strlen(bmdOidXpath->out_path);

	for(iteracja=0 ; iteracja < do_przetworzenia ; iteracja++) //jezeli nie koncowka to pobrano o jeden wiecej niz chcemy na kontrolke - wiec odejmujemy jeden
	{
		bmdDatagramPtr = bmdDatagramSet->bmdDatagramSetTable[iteracja];
		if (bmdDatagramPtr == NULL)
		{
			//Blad protokolu
			continue;
		}
		if (bmdDatagramPtr->datagramStatus>=0)
		{
			retval = _bmdWriteDatagramMetadataInXML2GenBuf(bmdDatagramPtr,bmdOidXpath,&DtgContent,-1,errors);
			if (retval != BMD_OK)
			{
				//powinnismy zwrocic warrningi z _bmdWriteDatagramMetadataInXML2GenBuf jako errory - jest w errors
				//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
				return retval;
			}
			//pobranie nazwy pliku z datagramu i dopisanie do sciezki pobran z nowym rozszerzeniem (CSV)

			dot = strrchr((char*)bmdDatagramPtr->protocolDataFilename->buf, '.');
			dot_pos = (long)dot - (long)bmdDatagramPtr->protocolDataFilename->buf;
			if (dot_pos==0)
			{
				dot_pos = (long)strlen((char *)bmdDatagramPtr->protocolDataFilename->buf);
			}
			tmpFilePath = calloc( dot_pos + 4 + out_path_len + 2, sizeof(char)); // +1 na "\\" + 1 na NULL na koncu +4 na .CSV
			if (tmpFilePath==NULL)
			{
				//dopisac komunikat warninga do stosu bledow
				continue;
			}
			tmpFilePath = memcpy(tmpFilePath, bmdOidXpath->out_path,out_path_len);	//do tmpFilePath zapisuje folder na zapiywanie
#ifdef WIN32
			tmpFilePath[out_path_len] = '\\'; //zakancza znakiem rozdzielajacym foldery
			memcpy(tmpFilePath+out_path_len+1, bmdDatagramPtr->protocolDataFilename->buf, dot_pos);
			memcpy(tmpFilePath+out_path_len+1+dot_pos, ".xml", 4);
			retval=UTF8StringToWindowsString(tmpFilePath, &widePath, NULL);
			retval = bmd_save_buf_wide(&DtgContent, widePath);
			free(widePath); widePath=NULL;
#else
			tmpFilePath[out_path_len] = '/'; //
			memcpy(tmpFilePath+out_path_len+1, bmdDatagramPtr->protocolDataFilename->buf, dot_pos);
			memcpy(tmpFilePath+out_path_len+1+dot_pos, ".xml", 4);
			retval = bmd_save_buf(&DtgContent,tmpFilePath);
#endif
			free(tmpFilePath); tmpFilePath=NULL;
			free(DtgContent.buf);
			DtgContent.buf = NULL;
			DtgContent.size = 0;
			//sprawdzenie statusu zapisania buforu do pliku
		}
		else
		{
			//jakis blad czy warning zglaszamy?
		}
	} /* for po kolejnych dokumentach w odpowiedzi */

	//skladamy datagramy z DtgContentTable do jednego XMLa w
	if (errors == NULL)
	{
		bmdOidXpath->datagramset_cnt++;

		if (process_num && *process_num==0)
		{
			*process_num = do_przetworzenia;
		}
	}
	return BMD_OK;
}

/** Funkcja zapisujaca metadane odczytane z datagramSetu do plikow XML na podstawie szablonu i Xpath
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()

@param bmdDatagramSet wskaznik na datagramSet odpowiedzi na rzadanie wyszukiwania
@param process_num wskaznik na licznik datagramow do przetworzenia (opcjonalny)
@param process_num jezeli wskaznik nie jest NULL i wskazuje na 0 to zwroci liczbe datagramow jaka przetworzono
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param forms to zestaw formularzy, z ktorych jeden zostanie wykorzystany przy tworzeniu mapy indeksow metadanych
		(mozna podac NULL, ale wowczas moze nastapic sytuacja, ze w wynikach bedzie pusta wartosc dla jakiejs metadanej mimo, ze wiele dokumentow ma ta wartosc ustawiona)
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdMtdDWriteCSV2Files(	bmdDatagramSet_t *bmdDatagramSet,
					long * process_num,
					bmd_oid_xpath_t *bmdOidXpath,
					const bmd_form_info_t *forms,
					char ***errors )
{
	bmd_err_heap_t tmp_err_heap;
	long iteracja=0;
	/*long err_heap_size = 0;*/
	long do_przetworzenia=0;
	int out_path_len = 0;
	int retval = BMD_OK;
	bmdDatagram_t *bmdDatagramPtr = NULL;
	char * tmpFilePath = NULL;
	long dot_pos = 0;
	char *dot = NULL;
	GenBuf_t DtgContent;	//zawartosc datagramu w postaci XML

	DtgContent.buf=NULL;
	DtgContent.size=0;

#if 1
	/** Walidacja parametrow */
	if (bmdDatagramSet==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->CSV_header.buf == NULL || bmdOidXpath->CSVNodes == NULL
		|| bmdOidXpath->CSVNodes[0] == NULL || bmdOidXpath->OIDTableBufs ==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if ((process_num == NULL) || (*process_num == 0))
	{
		do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize; //- (int)(nie_koncowka)
	}
	else
	{
		if (*process_num > bmdDatagramSet->bmdDatagramSetSize)
		{
			do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize;
			PRINT_DEBUG("process_num set to %lu, but only %lu datagrams in DatagramSet.\n",
				*process_num, do_przetworzenia);
		}
		else
		{
			do_przetworzenia = *process_num;
		}
	}

	// sprawdzic jeszcze typ datagramu

	/** utworzenie tablicy bledow - dla kazdego datagramu po jednym rekordzie*/
	tmp_err_heap.err_str_heap = NULL;
	tmp_err_heap.err_code_heap = NULL;
	tmp_err_heap.heap_size = 0;
	realoc_err_heap( &tmp_err_heap, do_przetworzenia);

	if (do_przetworzenia == 0)
	{
		PRINT_DEBUG("No data to write.\n");
		// na stos z bledami!
		return BMD_OK;
	}

	if ( bmdOidXpath->DtgNodes == NULL)
	{
		PRINT_DEBUG("Initializing bmdOidXpath struct for metadata datagram paths.\n");
		//sprawdzic czy jest datagram - przeszed weryfikacje parametrow wiec powinien byc
		retval = bmdInitMtdDtgPaths( bmdDatagramSet->bmdDatagramSetTable[0], bmdOidXpath, forms, errors );
		if (retval != BMD_OK)
		{
			//powinnismy zwrocic blad z bmdInitMtdDtgPaths - jest w errors
			//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
			return retval;
		}
	}
	if (bmdOidXpath->DtgNodes == NULL || bmdOidXpath->DtgNodes[0].MtdType == 0)
	{
		//powinnismy zwrocic warrningi z bmdInitMtdDtgPaths jako errory - jest w errors
		//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
		return BMD_ERR_PARAM3;
	}
	if (bmdOidXpath->out_path == NULL || strlen(bmdOidXpath->out_path) < 1)
	{
		//zapisac w stosie bledow ze struktura nie zostala zainicjalizowana do zapisywania plikow XML
		return BMD_ERR_NOTINITED;
	}
#endif

	out_path_len = (int)strlen(bmdOidXpath->out_path);

	for(iteracja=0 ; iteracja < do_przetworzenia ; iteracja++) //jezeli nie koncowka to pobrano o jeden wiecej niz chcemy na kontrolke - wiec odejmujemy jeden
	{
		bmdDatagramPtr = bmdDatagramSet->bmdDatagramSetTable[iteracja];
		if (bmdDatagramPtr == NULL)
		{
			//Blad protokolu
			continue;
		}
		if (bmdDatagramPtr->datagramStatus>=0)
		{
			retval = _bmdWriteDatagramMetadataInCSV2GenBuf(bmdDatagramPtr,bmdOidXpath,&DtgContent,0,errors);
			if (retval != BMD_OK)
			{
				//powinnismy zwrocic warrningi z _bmdWriteDatagramMetadataInXML2GenBuf jako errory - jest w errors
				//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
				return retval;
			}
			//pobranie nazwy pliku z datagramu i dopisanie do sciezki pobran z nowym rozszerzeniem (CSV)

			dot = strrchr((char*)bmdDatagramPtr->protocolDataFilename->buf, '.');
			dot_pos = (long)dot - (long)bmdDatagramPtr->protocolDataFilename->buf;
			if (dot_pos==0)
			{
				dot_pos = (int)strlen((char *)bmdDatagramPtr->protocolDataFilename->buf);
			}
			tmpFilePath = calloc( dot_pos + 4 + out_path_len + 2, sizeof(char)); // +1 na "\\" + 1 na NULL na koncu +4 na .CSV
			if (tmpFilePath==NULL)
			{
				//dopisac komunikat warninga do stosu bledow
				continue;
			}
			tmpFilePath = memcpy(tmpFilePath, bmdOidXpath->out_path,out_path_len);	//do tmpFilePath zapisuje folder na zapiywanie
#ifdef WIN32
			tmpFilePath[out_path_len] = '\\'; //									// zakancza znakiem rozdzielajacym foldery
#else
			tmpFilePath[out_path_len] = '/'; //
#endif

			memcpy(tmpFilePath+out_path_len+1, bmdDatagramPtr->protocolDataFilename->buf, dot_pos);
			memcpy(tmpFilePath+out_path_len+1+dot_pos, ".csv", 4);
			//zapisac bufor do pliku
			//zwolnic nazwe
			retval = bmd_save_buf(&DtgContent,tmpFilePath);
			free(tmpFilePath); tmpFilePath=NULL;
			free(DtgContent.buf);
			DtgContent.buf = NULL;
			DtgContent.size = 0;
			//sprawdzenie statusu zapisania buforu do pliku
		}
		else
		{
			//jakis blad czy warning zglaszamy?
		}
	} /* for po kolejnych dokumentach w odpowiedzi */

	//skladamy datagramy z DtgContentTable do jednego XMLa w
	if (errors == NULL)
	{
		bmdOidXpath->datagramset_cnt++;

		if (process_num && *process_num==0)
		{
			*process_num = do_przetworzenia;
		}
	}
	return BMD_OK;
}








/** Funkcja otwierajaca plik XML z podanej sciezki, do struktury XmlDoc
@param xmlDocPtr wskaznik do struktury xmlDoc (wskaznik powinien wskazywac na NULL - bo nie zwalnia poprzedniej struktury)
@param XmlFilePath sciezka (koniecznie zakodowana w UTF-8) do pliku zawierajacego xml
@retval BMD_OK
*/
long bmdOpenXmlDoc(xmlDocPtr *doc, char *XmlFilePath)
{
	GenBuf_t *content=NULL;
	int retval=BMD_OK;

#ifdef WIN32
	wchar_t* wideXmlFilePath=NULL;
#endif

	if (doc == NULL)
	{
		PRINT_ERROR("Invalid first parameter in bmdOpenXmlDoc().\n");
		return BMD_ERR_PARAM1;
	}
	if (XmlFilePath == NULL)
	{
		PRINT_ERROR("Invalid second parameter in bmdOpenXmlDoc().\n");
		return BMD_ERR_PARAM2;
	}

#ifdef WIN32
	retval= UTF8StringToWindowsString(XmlFilePath, &wideXmlFilePath, NULL);
	if(retval != 0)
	{
		return retval;
	}
	retval=bmd_load_binary_content_wide(wideXmlFilePath,&content);
	free(wideXmlFilePath); wideXmlFilePath=NULL;
#else
	retval=bmd_load_binary_content(XmlFilePath,&content);
#endif
	if(retval!=BMD_OK)
	{
		//bmd_load_binary_content(XmlFilePath,&content); jest nasza funkcja - wiec pewnie dala jakies Print_error juz
		*doc= NULL;
		return retval;
	}

	*doc=xmlParseMemory((const char *)content->buf,content->size);
	free_gen_buf(&content);

	if (*doc == NULL)
	{
		PRINT_ERROR("Error in parsing XML in bmdOpenXmlDoc().\n");
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}


/** Funkcja zapisujaca metadane odczytane z datagramSetu do plikow XML na podstawie szablonu i Xpath
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()

@param bmdDatagramSet wskaznik na datagramSet odpowiedzi na rzadanie wyszukiwania
@param process_num wskaznik na licznik datagramow do przetworzenia (opcjonalny)
@param process_num jezeli wskaznik nie jest NULL i wskazuje na 0 to zwroci liczbe datagramow jaka przetworzono
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param forms to zestaw formularzy, z ktorych jeden zostanie wykorzystany przy tworzeniu mapy indeksow metadanych
		(mozna podac NULL, ale wowczas moze nastapic sytuacja, ze w wynikach bedzie pusta wartosc dla jakiejs metadanej mimo, ze wiele dokumentow ma ta wartosc ustawiona)
@param content wskazik na docelowy bufor na dokument XML z datagramsetem
@param errors opcjonalny (moze byc NULL) wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdMtdDWriteXML2GenBuf(	bmdDatagramSet_t *bmdDatagramSet,
					long * process_num,
					bmd_oid_xpath_t *bmdOidXpath,
					const bmd_form_info_t *forms,
					GenBuf_t *content,
					char ***errors )
{
	bmd_err_heap_t tmp_err_heap;
	long iteracja=0;
	/*long err_heap_size = 0;*/
	long do_przetworzenia=0;
	int retval = BMD_OK;
	bmdDatagram_t *bmdDatagramPtr = NULL;
	GenBuf_t * DtgContentTable = NULL;	//tablica wynikow na datagramy
	GenBuf_t tmpResutContent;
	tmpResutContent.buf=NULL;
	tmpResutContent.size=0;
#if 1
	/** Walidacja parametrow */
	if (bmdDatagramSet==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->libbmdxpath_ctx == NULL || bmdOidXpath->XMLNodes == NULL
		|| bmdOidXpath->XMLNodes[0] == NULL || bmdOidXpath->OIDTableBufs ==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteXML2GenBuf parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if ((process_num == NULL) || (*process_num == 0))
	{
		do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize; //- (int)(nie_koncowka)
	}
	else
	{
		if (*process_num > bmdDatagramSet->bmdDatagramSetSize)
		{
			do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize;
			PRINT_DEBUG("process_num set to %lu, but only %lu datagrams in DatagramSet.\n",
				*process_num, do_przetworzenia);
		}
		else
		{
			do_przetworzenia = *process_num;
		}
	}

	// sprawdzic jeszcze typ datagramu

	/** utworzenie tablicy bledow - dla kazdego datagramu po jednym rekordzie*/
	tmp_err_heap.err_str_heap = NULL;
	tmp_err_heap.err_code_heap = NULL;
	tmp_err_heap.heap_size = 0;
	realoc_err_heap( &tmp_err_heap, do_przetworzenia);

	if (do_przetworzenia == 0)
	{
		PRINT_DEBUG("No data to write.\n");
		// na stos z bledami!
		return BMD_OK;
	}

	if ( bmdOidXpath->DtgNodes == NULL)
	{
		PRINT_DEBUG("Initializing bmdOidXpath struct for metadata datagram paths.\n");
		//sprawdzic czy jest datagram - przeszed weryfikacje parametrow wiec powinien byc
		retval = bmdInitMtdDtgPaths( bmdDatagramSet->bmdDatagramSetTable[0], bmdOidXpath, forms, errors );
		if (retval != BMD_OK)
		{
			//powinnismy zwrocic blad z bmdInitMtdDtgPaths - jest w errors
			//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
			return retval;
		}
	}
	if (bmdOidXpath->DtgNodes == NULL || bmdOidXpath->DtgNodes[0].MtdType == 0)
	{
		//powinnismy zwrocic warrningi z bmdInitMtdDtgPaths jako errory - jest w errors
		//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
		return BMD_ERR_PARAM3;
	}
#endif

	//utworzyc tablice buforow na kazdy datagram w XMLu
	DtgContentTable = calloc(do_przetworzenia, sizeof(GenBuf_t));
	if (DtgContentTable == NULL)
	{
		// zwrocic komunikat bledu
		return BMD_ERR_MEMORY;
	}

	for(iteracja=0 ; iteracja < do_przetworzenia ; iteracja++) //jezeli nie koncowka to pobrano o jeden wiecej niz chcemy na kontrolke - wiec odejmujemy jeden
	{
		bmdDatagramPtr = bmdDatagramSet->bmdDatagramSetTable[iteracja];
		if (bmdDatagramPtr == NULL)
		{
			//Blad protokolu
			continue;
		}
		if (bmdDatagramPtr->datagramStatus>=0)
		{
			retval = _bmdWriteDatagramMetadataInXML2GenBuf(bmdDatagramPtr,bmdOidXpath,&(DtgContentTable[iteracja]),0,errors);
			if (retval != BMD_OK)
			{
				//powinnismy zwrocic warrningi z _bmdWriteDatagramMetadataInXML2GenBuf jako errory - jest w errors
				//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
				return retval;
			}
		}
		else
		{
			//jakis blad czy warning zglaszamy?
		}
	} /* for po kolejnych dokumentach w odpowiedzi */

	retval = BMD_ERR_OP_FAILED;
	//skladamy datagramy z DtgContentTable do jednego XMLa w
	if (errors == NULL)
	{
		//sklada datagramset_metadata XML
		retval = _MergeDtgXMLs2DtgSetXML(&tmpResutContent, DtgContentTable, do_przetworzenia, bmdOidXpath, errors);
		if (retval != BMD_OK)
		{
			PRINT_DEBUG("Blad skladania datagramSetu w XMLu.\n");
		}
		else
		{
			bmdOidXpath->datagramset_cnt++;
			content->buf = tmpResutContent.buf;
			content->size = tmpResutContent.size;
			tmpResutContent.buf = NULL;
			tmpResutContent.size = 0;

			if (process_num )
			{
				*process_num = do_przetworzenia;
			}
		}
	}
	//zwalnia tymczasowe bufory zawartosci datagramow
	for(iteracja=0 ; iteracja < do_przetworzenia ; iteracja++) //jezeli nie koncowka to pobrano o jeden wiecej niz chcemy na kontrolke - wiec odejmujemy jeden
	{
		free(DtgContentTable[iteracja].buf);
		DtgContentTable[iteracja].buf= NULL;
	}
	free(DtgContentTable);
	return retval;
}

/** Funkcja zapisujaca metadane odczytane z datagramSetu do bufora na podstawie szablonu i Xpath
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4CSV()

@param bmdDatagramSet wskaznik na datagramSet odpowiedzi na rzadanie wyszukiwania
@param process_num wskaznik na licznik datagramow do przetworzenia (opcjonalny)
@param process_num jezeli wskaznik nie jest NULL i wskazuje na 0 to zwroci liczbe datagramow jaka przetworzono
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param forms to zestaw formularzy, z ktorych jeden zostanie wykorzystany przy tworzeniu mapy indeksow metadanych
		(mozna podac NULL, ale wowczas moze nastapic sytuacja, ze w wynikach bedzie pusta wartosc dla jakiejs metadanej mimo, ze wiele dokumentow ma ta wartosc ustawiona)
@param with_csv_header flaga oznaczajaca czy otrzymany bufor ma zawierac naglowek CSV(wartosc 1) czy nie (wartosc 0)
@param content wskazik na docelowy bufor na dokument XML z datagramsetem
@param errors opcjonalny (moze byc NULL) wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdMtdDWriteCSV2GenBuf(	bmdDatagramSet_t *bmdDatagramSet,
					long * process_num,
					bmd_oid_xpath_t *bmdOidXpath,
					const bmd_form_info_t *forms,
					long with_csv_header,
					GenBuf_t *content,
					char ***errors )
{
	bmd_err_heap_t tmp_err_heap;
	long iteracja=0;
	/*long err_heap_size = 0*/

	long do_przetworzenia=0;
	int retval = BMD_OK;
	bmdDatagram_t *bmdDatagramPtr = NULL;
	GenBuf_t tmpResutContent;
	SubstringsGenBuf_t wiersz;
	int wcsvh = with_csv_header;
	size_t tmp_size;

	tmpResutContent.buf=NULL;
	tmpResutContent.size=0;

#if 1
	/** Walidacja parametrow */
	if (bmdDatagramSet==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->CSV_header.buf == NULL || bmdOidXpath->CSVNodes == NULL
		|| bmdOidXpath->CSVNodes[0] == NULL || bmdOidXpath->OIDTableBufs ==NULL)
	{
		PRINT_ERROR("Error in bmdMtdDWriteCSV2Files parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if ((process_num == NULL) || (*process_num == 0))
	{
		do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize; //- (int)(nie_koncowka)
	}
	else
	{
		if (*process_num > bmdDatagramSet->bmdDatagramSetSize)
		{
			do_przetworzenia = bmdDatagramSet->bmdDatagramSetSize;
			PRINT_DEBUG("process_num set to %lu, but only %lu datagrams in DatagramSet.\n",
				*process_num, do_przetworzenia);
		}
		else
		{
			do_przetworzenia = *process_num;
		}
	}

	// sprawdzic jeszcze typ datagramu

	/** utworzenie tablicy bledow - dla kazdego datagramu po jednym rekordzie*/
	tmp_err_heap.err_str_heap = NULL;
	tmp_err_heap.err_code_heap = NULL;
	tmp_err_heap.heap_size = 0;
	realoc_err_heap( &tmp_err_heap, do_przetworzenia);

	if (do_przetworzenia == 0)
	{
		PRINT_DEBUG("No data to write.\n");
		// na stos z bledami!
		return BMD_OK;
	}

	if ( bmdOidXpath->DtgNodes == NULL)
	{
		PRINT_DEBUG("Initializing bmdOidXpath struct for metadata datagram paths.\n");
		//sprawdzic czy jest datagram - przeszed weryfikacje parametrow wiec powinien byc
		retval = bmdInitMtdDtgPaths( bmdDatagramSet->bmdDatagramSetTable[0], bmdOidXpath, forms, errors );
		if (retval != BMD_OK)
		{
			//powinnismy zwrocic blad z bmdInitMtdDtgPaths - jest w errors
			//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
			return retval;
		}
	}
	if (bmdOidXpath->DtgNodes == NULL || bmdOidXpath->DtgNodes[0].MtdType == 0)
	{
		//powinnismy zwrocic warrningi z bmdInitMtdDtgPaths jako errory - jest w errors
		//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
		return BMD_ERR_PARAM3;
	}
	//if (bmdOidXpath->out_path == NULL || strlen(bmdOidXpath->out_path) < 1)
	//{
	//	//zapisac w stosie bledow ze struktura nie zostala zainicjalizowana do zapisywania plikow XML
	//	return BMD_ERR_NOTINITED;
	//}
#endif

	//utworzyc tablice buforow na kazdy datagram w CSVu
	wiersz.no_of_substrings = do_przetworzenia;
	wiersz.substrings_table = calloc(do_przetworzenia, sizeof(char*));
	if (wiersz.substrings_table == NULL)
	{
		// zwrocic komunikat bledu
		return BMD_ERR_MEMORY;
	}

	for(iteracja=0 ; iteracja < do_przetworzenia ; iteracja++) //jezeli nie koncowka to pobrano o jeden wiecej niz chcemy na kontrolke - wiec odejmujemy jeden
	{
		bmdDatagramPtr = bmdDatagramSet->bmdDatagramSetTable[iteracja];
		if (bmdDatagramPtr == NULL)
		{
			//Blad protokolu
			continue;
		}
		if (bmdDatagramPtr->datagramStatus>=0)
		{
			retval = _bmdWriteDatagramMetadataInCSV2GenBuf(bmdDatagramPtr,bmdOidXpath,&(tmpResutContent),wcsvh,errors);
			if (retval != BMD_OK)
			{
				//powinnismy zwrocic warrningi z _bmdWriteDatagramMetadataInXML2GenBuf jako errory - jest w errors
				//ewentualnie dopisac, ze nie udalo sie wykonac bmdMtdDWriteXML2GenBuf, bo errors
				return retval;
			}
			wcsvh = -1;
			wiersz.substrings_table[iteracja] = (char*)tmpResutContent.buf;
			tmpResutContent.buf = NULL;
			tmpResutContent.size = 0;
		}
		else
		{
			//jakis blad czy warning zglaszamy?
		}
	} /* for po kolejnych dokumentach w odpowiedzi */

	//skladamy datagramy z wiersz.substrings_table do jednego CSVa w
	retval = bmdxpath_strings2string_freetable((char **)&(tmpResutContent.buf), &tmp_size, &wiersz.substrings_table, do_przetworzenia);

	if (process_num != NULL)
	{
		*process_num = do_przetworzenia; //- (int)(nie_koncowka)
	}

	tmpResutContent.size=(int)tmp_size;
	if (retval == BMD_OK)
	{
		content->buf = tmpResutContent.buf;
		content->size= tmpResutContent.size;
		tmpResutContent.buf = NULL;
		tmpResutContent.size = 0;
	}

	return retval;
}


/** Funkcja zapisujaca metadane odczytane z datagramSetu do plikow XML na podstawie szablonu i Xpath
@note funkcja wewnetrzna - bez sprawdzania danych wejsciowych
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()
@note przed pierwszym uzyciem nalezy takze zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitMtdDtgPaths()

@param bmdDatagram wskaznik na datagram z odpowiedzia na rzadanie wyszukiwania
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param content wskaznik na bufor docelowy dla
@param with_xml_header wskazuje czy stosowac naglowek <?xml ...> i tag roota <datagram_metadata>: wartosc -1: zostaw <?xml...>, ale nie dodawaj roota <datagram_metadata> ; 0 - usun naglowek <?xml...> ale dodaj <datagram_metadata>; 1 - pozostaw naglowek <?xml..> i dodaj root <datagram_metadata>
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long _bmdWriteDatagramMetadataInXML2GenBuf(	bmdDatagram_t *bmdDatagram,
								bmd_oid_xpath_t *bmdOidXpath,
								GenBuf_t *content,
								long with_xml_header,
								char ***errors )
{
long i					= 0;
long j					= 0;
GenBuf_t * tmpbuf				= NULL;
long retval					= 0;
long cancel					= 0;
mtd_in_datagram_path *dtgPathCpyPtr = NULL;
MetaDataBuf_t **DtgMetaDataCpyPtr	= NULL;
long DtgNoOfMetaDataCpy			= 0;
char *OIDTableBufCpyPtr			= NULL;
char *dtgOIDTableCpyPtr			= NULL;
char *mtd_content				= NULL;
xmlNodeSetPtr NodeSetCpyPtr		= NULL;
char* TagAttribute			= NULL;
char TmpType				= 0;

	for (i=0; (cancel == 0) && (i < bmdOidXpath->oid_cnt); i++)
	{
		if (bmdOidXpath->DtgPrgrsCallback != NULL)
		{
			retval = bmdOidXpath->DtgPrgrsCallback(i,bmdOidXpath->oid_cnt);
			if (retval != BMD_OK)
			{
				cancel = 1;
				continue;
			}
		}
		dtgPathCpyPtr = &bmdOidXpath->DtgNodes[i];			//odczytanie dtgpath dla metadanej
		switch (dtgPathCpyPtr->MtdType)
		{
		case ADDITIONAL_METADATA:
			DtgMetaDataCpyPtr = bmdDatagram->additionalMetaData;
			DtgNoOfMetaDataCpy = bmdDatagram->no_of_additionalMetaData;
			break;
		case SYS_METADATA:
			DtgMetaDataCpyPtr = bmdDatagram->sysMetaData;
			DtgNoOfMetaDataCpy = bmdDatagram->no_of_sysMetaData;
			break;
		//case PKI_METADATA:
		//	DtgMetaDataCpyPtr = bmdDatagram->pkiMetaData;
		//	DtgNoOfMetaDataCpy = bmdDatagram->no_of_pkiMetaData;
		//	break;
		//case USER_METADATA:
		//	DtgMetaDataCpyPtr = bmdDatagram->actionMetaData;
		//	DtgNoOfMetaDataCpy = bmdDatagram->no_of_actionMetaData;
		//	break;
		default:
			//internal error
			PRINT_DEBUG("_bmdWriteDatagramMetadataInXML2GenBuf nieobslugiwany typ metadanej do zapisania w XMLu.\n");
			break;
		}
		//sprawdz czy dtgpath nie klamal
		retval = -1;
		if ( (long)dtgPathCpyPtr->MtdPos < DtgNoOfMetaDataCpy)
		{
			dtgOIDTableCpyPtr = DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->OIDTableBuf;
			OIDTableBufCpyPtr = bmdOidXpath->OIDTableBufs[i];
			retval = strcmp(dtgOIDTableCpyPtr, OIDTableBufCpyPtr);
			if (retval != 0)
			{
				// jakies dodatkowe informacje debugowe?
			}
		}
		else
		{
			if (dtgPathCpyPtr->MtdType == SYS_METADATA &&
				dtgPathCpyPtr->MtdPos == LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)
			{
				retval = LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME;
			}
		}
		if (retval != 0 && retval != LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)
		{
			PRINT_ERROR("Internal Error in _bmdWriteDatagramMetadataInXML2GenBuf.\nDtgPath error or protcol error.\n");
			continue;
		}

		//odczytaj wartosc metadanej z datagramu
		if (retval == 0)
		{
			/*tmpAny.buf = (unsigned char*) DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnyBuf;
			tmpAny.size = DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnySize;
			retval = Any2Octetstring2String(&tmpAny,&mtd_content);*/
			mtd_content = malloc(DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnySize + 1);
			if(mtd_content == NULL)		{	BMD_FOK(NO_MEMORY);	}
			memset(mtd_content, 0, DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnySize + 1);
			memcpy(mtd_content, DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnyBuf,
			DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnySize);
		}
		else
		{
			mtd_content = STRDUPL((char*)bmdDatagram->protocolDataFilename->buf);
			if (mtd_content)
			{
				retval = BMD_OK;
			}
		}
		if (retval != BMD_OK)
		{
			PRINT_ERROR("Error evaluating metadata value. (MtdType:%i, MtdPos: %i).\n",dtgPathCpyPtr->MtdType ,dtgPathCpyPtr->MtdPos);
			continue;
		}

		//odczytaj xmlNodeSet
		NodeSetCpyPtr = bmdOidXpath->XMLNodes[i];
		if (bmdOidXpath->ColValType)
		{
			TmpType = bmdOidXpath->ColValType[i];
			if (TmpType)
			{
				mtd_content = NumConvert(bmdOidXpath, mtd_content, TmpType);
			}
			else
			{
				mtd_content = XMLEscape(bmdOidXpath, mtd_content, TmpType);
			}
		}
		else
		{
			mtd_content = XMLEscape(bmdOidXpath, mtd_content, TmpType);
		}
		//ustaw wartosci we wszystkich XmlNodePtr
		//zapisuje warotsc metadanej o OID we wszystich Nodach speniajcych Xpatha
		if (NodeSetCpyPtr)
		{
			for (j=0; (long)j < (long)NodeSetCpyPtr->nodeNr; j++)
			{
				xmlNodeSetContent(NodeSetCpyPtr->nodeTab[j], (xmlChar*) mtd_content);
				//sprawdzic przez pobranie?? - chyba bez sensu
			}
		}
		free(mtd_content);
	}

	if (cancel == 1)
	{
		//jakies info do error??
		return BMD_CANCEL;
	}
	//zapisz w buforze tymczasowym XMLa z szablonu
	retval = bmdxpath_doc2genbuf(bmdOidXpath->libbmdxpath_ctx->ctx->doc, &tmpbuf);
	if ( retval != BMD_OK)
	{
		//wywalamy blad
		free_gen_buf(&tmpbuf);
		return BMD_ERR_OP_FAILED;
	}

	if (with_xml_header != -1)
	{
		//obcinamy (0) lub zostawiamy naglowek(1), wstawiamy root_tag <bmddatagram>tmpbuf</bmddatagram>
		mtd_content = STRDUPL("datagram_metadata");
		bmdOidXpath->datagram_cnt++;
		asprintf(&TagAttribute, " number=\"%lu\"",bmdOidXpath->datagram_cnt);
		retval = _InsertRootTagOnXMLBuf( mtd_content, TagAttribute, tmpbuf, with_xml_header, errors);
		free(mtd_content);
		free(TagAttribute);
		if (retval != BMD_OK)
		{
			bmdOidXpath->datagram_cnt--;
			free_GenBuf(&tmpbuf);
			//error na stos bledow
			PRINT_ERROR("Blad _InsertRootTagOnXMLBuf w _bmdWriteDatagramMetadataInXML2GenBuf.\n");
			return retval;
		}
	}

	//otrzymany bufor przedstawiamy w content
	content->buf = tmpbuf->buf;
	content->size = tmpbuf->size;
	tmpbuf->buf = NULL;
	tmpbuf->size = 0;

	return BMD_OK;
}


/** Funkcja zapisujaca metadane odczytane z datagramu do XML z datagramSetu w buforze na podstawie szablonu i Xpath
@note funkcja raczej wewnetrzna
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()
@note przed pierwszym uzyciem nalezy takze zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitMtdDtgPaths()

@param bmdDatagram wskaznik na datagram z odpowiedzia na rzadanie wyszukiwania
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2XML
@param content wskaznik na bufor docelowy dla
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdWriteDatagramMetadataInXML2GenBuf(	bmdDatagram_t *bmdDatagram,
								bmd_oid_xpath_t *bmdOidXpath,
								GenBuf_t *content,
								char ***errors )
{
#if 1
	/** Walidacja parametrow */
	if (bmdDatagram==NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->libbmdxpath_ctx == NULL || bmdOidXpath->XMLNodes == NULL
		|| bmdOidXpath->XMLNodes[0] == NULL || bmdOidXpath->OIDTableBufs ==NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->DtgNodes == NULL || bmdOidXpath->DtgNodes[0].MtdType == 0)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
#endif

	return _bmdWriteDatagramMetadataInXML2GenBuf(bmdDatagram, bmdOidXpath, content, 1, errors );
}

/** Funkcja zapisujaca metadane odczytane z datagramSetu do plikow XML na podstawie szablonu i Xpath
@note funkcja wewnetrzna - bez sprawdzania danych wejsciowych
@note przed pierwszym uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()
@note przed pierwszym uzyciem nalezy takze zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitMtdDtgPaths()

@param bmdDatagram wskaznik na datagram z odpowiedzia na rzadanie wyszukiwania
@param bmdOidXpath struktura kontekstu dla funkcji Dtg2CSV
@param content wskaznik na bufor docelowy dla
@param with_xml_header wskazuje czy stosowac naglowek <?xml ...> i tag roota <datagram_metadata>: wartosc -1: zostaw <?xml...>, ale nie dodawaj roota <datagram_metadata> ; 0 - usun naglowek <?xml...> ale dodaj <datagram_metadata>; 1 - pozostaw naglowek <?xml..> i dodaj root <datagram_metadata>
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long _bmdWriteDatagramMetadataInCSV2GenBuf(	bmdDatagram_t *bmdDatagram,
								bmd_oid_xpath_t*bmdOidXpath,
								GenBuf_t *content,
								long with_csv_header,
								char ***errors )
{
long i					= 0;/*, j=0;*/
GenBuf_t * tmpbuf				= NULL;
long retval 				= 0;
long cancel					= 0;
mtd_in_datagram_path *dtgPathCpyPtr =NULL;
MetaDataBuf_t **DtgMetaDataCpyPtr	= NULL;
long DtgNoOfMetaDataCpy			= 0;
char *OIDTableBufCpyPtr			= NULL;
char *dtgOIDTableCpyPtr			= NULL;
char *mtd_content				= NULL;
char *temp_mtd_content			= NULL;
char* tmp_value				= NULL;
char *cell_separator			= NULL;
char *text_separator			= NULL;
SubstringsGenBuf_t wiersz;
size_t len					= 0;
char znak					= 0;
char TmpType				= 0;

long iter					= 0;
long metadataIndexInDatagram	= -1;


	cell_separator = STRDUPL(";");
	if(cell_separator == NULL)
		{ BMD_FOK(NO_MEMORY); }
	cell_separator[0] = bmdOidXpath->cell_separator_char;
	text_separator = STRDUPL("\"");
	if(text_separator == NULL)
		{ BMD_FOK(NO_MEMORY); }
	text_separator[0] = bmdOidXpath->text_separator_char;

	wiersz.substrings_table = calloc(bmdOidXpath->oid_cnt,sizeof(char*));
	wiersz.no_of_substrings = bmdOidXpath->oid_cnt;
	if(wiersz.no_of_substrings > 0 && wiersz.substrings_table == NULL)
		{ BMD_FOK(NO_MEMORY); }

	for (i=0; (cancel == 0) && (i < bmdOidXpath->oid_cnt); i++)
	{
		if (bmdOidXpath->DtgPrgrsCallback != NULL)
		{
			retval = bmdOidXpath->DtgPrgrsCallback(i,bmdOidXpath->oid_cnt);
			if (retval != BMD_OK)
			{
				cancel = 1;
				continue;
			}
		}
		dtgPathCpyPtr = &bmdOidXpath->DtgNodes[i];			//odczytanie dtgpath dla metadanej
		switch (dtgPathCpyPtr->MtdType)
		{
		case ADDITIONAL_METADATA:
			DtgMetaDataCpyPtr = bmdDatagram->additionalMetaData;
			DtgNoOfMetaDataCpy = bmdDatagram->no_of_additionalMetaData;
			break;
		case SYS_METADATA:
			DtgMetaDataCpyPtr = bmdDatagram->sysMetaData;
			DtgNoOfMetaDataCpy = bmdDatagram->no_of_sysMetaData;
			break;
		//case PKI_METADATA:
		//	DtgMetaDataCpyPtr = bmdDatagram->pkiMetaData;
		//	DtgNoOfMetaDataCpy = bmdDatagram->no_of_pkiMetaData;
		//	break;
		//case USER_METADATA:
		//	DtgMetaDataCpyPtr = bmdDatagram->actionMetaData;
		//	DtgNoOfMetaDataCpy = bmdDatagram->no_of_actionMetaData;
		//	break;
		default:
			//internal error
			PRINT_DEBUG("_bmdWriteDatagramMetadataInXML2GenBuf nieobslugiwany typ metadanej do zapisania w XMLu.\n");
			//continue;
			break;
		}
		//sprawdz czy dtgpath nie klamal
		retval = -1;
		metadataIndexInDatagram = -1;
		if ( (long)dtgPathCpyPtr->MtdPos < DtgNoOfMetaDataCpy)
		{
			dtgOIDTableCpyPtr = DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->OIDTableBuf;
			OIDTableBufCpyPtr = bmdOidXpath->OIDTableBufs[i];
			retval = strcmp(dtgOIDTableCpyPtr, OIDTableBufCpyPtr);
			if (retval != 0)
			{
				retval = 0; // nawet jesli nie znajdzie metadanej w datagramie odpowiedzi Search (wtedy swiadczy o tym metadataIndexInDatagram == -1)
				// Metadana o pustej wartosci (NULL lub lancuch pusty) jest pomijana w odpowiedzi Search i to powoduje przesuniecia w tablicy metadanych.
				// W zwiazku z tym nie zewsze mozna opierac sie na dtgPathCpyPtr->MtdPos. Jesli wystepuje niezgodnosc w kolejnosci metadanych,
				// to nalezy przeszukac cala tablice metadanych w celu ustalenia wlasciwego indeksu.
				if(dtgPathCpyPtr->MtdType == ADDITIONAL_METADATA)
				{
					for(iter=0; iter<bmdDatagram->no_of_additionalMetaData; iter++)
					{
						if(bmdDatagram->additionalMetaData[iter]->OIDTableBuf == NULL)
							{ continue; }
						if(strcmp(bmdDatagram->additionalMetaData[iter]->OIDTableBuf, OIDTableBufCpyPtr) == 0)
						{
							metadataIndexInDatagram = iter; // ustalony wlasciwy indeks metadanej (w zwiazku z przesunieciem)
							break;
						}
					}
				}
				else if(dtgPathCpyPtr->MtdType == SYS_METADATA)
				{
					for(iter=0; iter<bmdDatagram->no_of_sysMetaData; iter++)
					{
						if(bmdDatagram->sysMetaData[iter]->OIDTableBuf == NULL)
							{ continue; }
						if(strcmp(bmdDatagram->sysMetaData[iter]->OIDTableBuf, OIDTableBufCpyPtr) == 0)
						{
							metadataIndexInDatagram = iter; // ustalony wlasciwy indeks metadanej (w zwiazku z przesunieciem)
							break;
						}
					}
				}
			}
			else
			{
				metadataIndexInDatagram = dtgPathCpyPtr->MtdPos; // indeks metadanej jest zgodny z tym wstepnie ustalonym
			}
		}
		else
		{
			if (dtgPathCpyPtr->MtdType == SYS_METADATA &&
				dtgPathCpyPtr->MtdPos == LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)
			{
				retval = LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME;
			}
			else
			{
				if (dtgPathCpyPtr->MtdType == OID_NOT_FOUND_IN_DATAGRAM &&
				dtgPathCpyPtr->MtdPos == OID_NOT_FOUND_IN_DATAGRAM)
				{
					retval = OID_NOT_FOUND_IN_DATAGRAM;
				}
			}
		}

		if (retval != 0 && retval != LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME && retval != OID_NOT_FOUND_IN_DATAGRAM)
		{
			PRINT_ERROR("Internal Error in _bmdWriteDatagramMetadataInXML2GenBuf.\nDtgPath error or protcol error.\n");
			continue;
		}

		if (bmdOidXpath->ColValType)
		{
			TmpType = bmdOidXpath->ColValType[i];
		}

		//odczytaj wartosc metadanej z datagramu
		if (retval == 0)
		{
			/*tmpAny.buf = (unsigned char*) DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnyBuf;
			tmpAny.size = DtgMetaDataCpyPtr[dtgPathCpyPtr->MtdPos]->AnySize;
			retval = Any2Octetstring2String(&tmpAny,&tmp_value);
			tmpAny.buf = NULL;
			tmpAny.size = 0;*/
			if(metadataIndexInDatagram > -1) // znalazl metadana w datagramie odpowiedzi Search
			{
				tmp_value = malloc(DtgMetaDataCpyPtr[metadataIndexInDatagram]->AnySize + 1);
				if(tmp_value == NULL)		{	BMD_FOK(NO_MEMORY);	}
				memset(tmp_value, 0, DtgMetaDataCpyPtr[metadataIndexInDatagram]->AnySize + 1);
				memcpy(tmp_value, DtgMetaDataCpyPtr[metadataIndexInDatagram]->AnyBuf,
				DtgMetaDataCpyPtr[metadataIndexInDatagram]->AnySize);
			}
			else // jesli nie znalazl metadanej w datagramie odpowiedzi Search tzn. ze serwer pominal bo wartosc pusta
			{
				tmp_value = STRDUPL("");
				if(tmp_value == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}

			//najpierw escapowanie (stringu w UTF-8)
			mtd_content = CSVEscape(bmdOidXpath, tmp_value, TmpType);
			free(tmp_value); tmp_value=NULL;

			//potem konwersja do strony kodowej
			if (bmdOidXpath->cd)
			{
				if (change_code_page(bmdOidXpath, &mtd_content) != BMD_OK)
				{
					free(mtd_content);
					mtd_content = STRDUPL("#code page conversion error#");
					if(mtd_content == NULL)
						{ BMD_FOK(NO_MEMORY); }
				}
			}
		}
		else
		{
			switch (retval)
			{
			case LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME:
				if (bmdOidXpath->cd)
				{
					temp_mtd_content= STRDUPL((char *)bmdDatagram->protocolDataFilename->buf);

					//najpierw escapowanie (stringu w UTF-8)
					mtd_content = CSVEscape(bmdOidXpath, temp_mtd_content, TmpType);
					free(temp_mtd_content); temp_mtd_content=NULL;

					//potem konwersja do strony kodowej
					if (change_code_page(bmdOidXpath, &mtd_content) != BMD_OK)
					{
						free(mtd_content);
						mtd_content = STRDUPL("#code page conversion error#");
						if(mtd_content == NULL)
							{ BMD_FOK(NO_MEMORY); }
					}
				}
				else
				{
					mtd_content = CSVEscape(bmdOidXpath, (char *)bmdDatagram->protocolDataFilename->buf, TmpType);
				}
				if (mtd_content)
				{
					retval = BMD_OK;
				}
				break;
			case OID_NOT_FOUND_IN_DATAGRAM:
				mtd_content = STRDUPL("");
				if (mtd_content)
				{
					retval = BMD_OK;
				}
				else
					{ BMD_FOK(NO_MEMORY); }
				break;
			default:
				retval = -1;
			}
		}
		TmpType = 0;
		if (retval != BMD_OK)
		{
			PRINT_ERROR("Error evaluating metadata value. (MtdType:%i, MtdPos: %i).\n",dtgPathCpyPtr->MtdType ,dtgPathCpyPtr->MtdPos);
			return BMD_ERR_NODATA;
		}
		dtgPathCpyPtr = NULL;
		DtgMetaDataCpyPtr = NULL;
		DtgNoOfMetaDataCpy =0;
		OIDTableBufCpyPtr = NULL;
		dtgOIDTableCpyPtr=NULL;

#if 1	// dodaje wartosci metadanej do elementu tablicy "wiersz"
		retval = 0;
		if (bmdOidXpath->accvwqm)								// sprawdzam czy zawsze uszy
		{
			retval = 1;
			len = strlen(mtd_content);
		}
		else													// jezeli nie zawsze
		{
			if (bmdOidXpath->ColCcvwqm)
			{
				len = strlen(mtd_content);
				if (bmdOidXpath->ColCcvwqm[i] != '0')			// jezeli ustawiono w oid_csv_coll_name atrybut ccvwqm (na wartosc inna niz 0)
				{
					retval = 1;
				}
			}
			else												// analizuje zawartosc, by sprawdzic czy trzeba otaczac uszami
			{
				len = 0;
				retval = 0;
				if (mtd_content[0] == '0')							//jezeli metadana zaczyna sie od zera to w uszy
				{
					retval = 1;
				}
				while (mtd_content[len])							//to sprawdzam czy numdata
				{
					znak = mtd_content[len];
					if ( (znak < '0' || znak > '9') && znak != '-' && znak != '.' && znak != ',' && znak != ':')
					{
						retval = 1;
					}
					len++;											// i jednoczesnie obliczam dlugosc stringa
					znak = 0;
				}
			}
		}
		if (retval == 1)										// otaczamy zawartosc komorki uszami
		{
			wiersz.substrings_table[i] = calloc(len + 5, sizeof(char));	// 3 + 2 na koniec
			if(wiersz.substrings_table[i] == NULL)
				{ BMD_FOK(NO_MEMORY); }
			strncat(wiersz.substrings_table[i], text_separator , 1);
			strncat(wiersz.substrings_table[i], mtd_content , len);
			strncat(wiersz.substrings_table[i], text_separator , 1);
			strncat(wiersz.substrings_table[i], cell_separator , 1);
		}
		else
		{
			wiersz.substrings_table[i] = calloc(len + 3, sizeof(char));	// 1  + 2 na koniec
			if(wiersz.substrings_table[i] == NULL)
				{ BMD_FOK(NO_MEMORY); }
			strncat(wiersz.substrings_table[i], mtd_content , len);
			strncat(wiersz.substrings_table[i], cell_separator , 1);
		}
#endif
		free(mtd_content);
		mtd_content = NULL;
	}	//koniec petli for

	// skasowanie ostatniego ; w wierszu i zastapienie go znakiem konca linii (13,10) lub 10
	if (bmdOidXpath->oid_cnt)
	{
		while(wiersz.substrings_table[i-1] == NULL && i >= 0)
		{
			i--;
		}
		if (wiersz.substrings_table[i-1])
		{
			len = strlen(wiersz.substrings_table[i-1]);
#ifdef WIN32
			(wiersz.substrings_table[i-1])[len-1] = 13;
			(wiersz.substrings_table[i-1])[len] = 10;
#else
			(wiersz.substrings_table[i-1])[len-1] = 10;
			(wiersz.substrings_table[i-1])[len] = 0;
#endif
		}
		tmpbuf = calloc(1, sizeof(GenBuf_t));
		if(tmpbuf == NULL)
			{ BMD_FOK(NO_MEMORY); }
		retval = bmdxpath_strings2string_freetable((char **)&(tmpbuf->buf), &len, &wiersz.substrings_table, bmdOidXpath->oid_cnt);
		if (retval == BMD_OK)
		{
			tmpbuf->size= (int)len;
		}
		len = 0;
	}

	if (with_csv_header == 1)
	{
		tmpbuf->size = bmdOidXpath->CSV_header.size + tmpbuf->size;
		tmp_value = calloc( tmpbuf->size + 1, sizeof (char));
		if(tmp_value == NULL)
			{ BMD_FOK(NO_MEMORY); }
		strncat( (char*)tmp_value, (char *)bmdOidXpath->CSV_header.buf , bmdOidXpath->CSV_header.size);
		strncat( (char*)tmp_value, (char *)tmpbuf->buf , tmpbuf->size);
		free (tmpbuf->buf);
		tmpbuf->buf = (char*) tmp_value;
		tmp_value = NULL;
	}

	free(cell_separator);
	cell_separator = NULL;
	free(text_separator);
	text_separator = NULL;

	if (cancel == 1)
	{
		//jakies info do error??
		return BMD_CANCEL;
	}

	//otrzymany bufor przedstawiamy w content
	content->buf = tmpbuf->buf;
	content->size = tmpbuf->size;
	tmpbuf->buf = NULL;
	tmpbuf->size = 0;

	return BMD_OK;
}









/** Funkcja inicjalizujaca
@note funkcja raczej wewnetrzna
@note przed uzyciem nalezy zainicjalizowac strukture bmd_oid_xpath_t za pomoca bmdInitBmdOidXpathStruct4XML()
@note przed wywolaniem sprawdz czy (bmdOidXpath->DtgNodes == NULL) - jezeli nie, to szkoda czasu na ladowanie funkcji

@param bmdDatagram wskaznik na datagram odpowiedzi na zadanie wyszukiwania - szablon do okreslenia sciezek do metadanych o podanych OID
@param content wskaznik na bufor docelowy dla
@param forms to zestaw formularzy, z ktorych jeden zostanie wykorzystany przy tworzeniu mapy indeksow metadanych
		(mozna podac NULL, ale wowczas moze nastapic sytuacja, ze w wynikach bedzie pusta wartosc dla jakiejs metadanej mimo, ze wiele dokumentow ma ta wartosc ustawiona)
@param errors wskaznik na tablice stringow z komunikatami bledow (opcjonalny, moze byc NULL)
@retval
*/
long bmdInitMtdDtgPaths(	bmdDatagram_t *bmdDatagram,
					bmd_oid_xpath_t *bmdOidXpath,
					const bmd_form_info_t *forms,
					char ***errors )
{
long i				= 0;
long j				= 0;
char *OIDTableBufPtr		= NULL;
char *dtgOIDTableBufPtr		= NULL;
long oid_found			= 0;
long retval				= 0;
char file_name_oid[]		= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME};
char ** errors_tmp=NULL;
int errors_cnt=0;

long iter			= 0;

#if 1
	/** Walidacja parametrow */
	if (bmdDatagram==NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdDatagramSet pointer is NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (bmdOidXpath == NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath pointer is NULL.\n");
		return BMD_ERR_PARAM3;
	}
	if ( bmdOidXpath->OIDTableBufs == NULL)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly.\n");
		return BMD_ERR_PARAM3;
	}
	if (bmdOidXpath->oid_cnt == 0)
	{
		PRINT_ERROR("Error in bmdWriteMetadataInXMLsFromDatagramSet parameters.\n");
		PRINT_DEBUG("bmdOidXpath struct is not inited properly. No Xpaths defined.\n");
		return BMD_ERR_PARAM3;
	}
#endif

	if (bmdOidXpath->DtgNodes != NULL)
	{
		return BMD_ERR_UNIMPLEMENTED;
	}
	//inicjalizacja tablicy sciezek do metadanych w datagramach
	bmdOidXpath->DtgNodes = calloc(bmdOidXpath->oid_cnt, sizeof(mtd_in_datagram_path));
	if (bmdOidXpath->DtgNodes == NULL)
	{
		return BMD_ERR_MEMORY;
	}

	// dla kazdego OidXpatha - wyszukiwanie polozenia OIDu w datagramie
	// kolejnosc przeszukiwania: ADDITIONAL_METADATA, SYS_METADATA, PKI_METADATA, USER_METADATA

	errors_tmp = calloc(bmdOidXpath->oid_cnt+1, sizeof(char*));
	for ( i=0; i < bmdOidXpath->oid_cnt; i++)
	{
		OIDTableBufPtr = bmdOidXpath->OIDTableBufs[i];
		oid_found = 0;

		// najpierw wyszukiwanie w metadanych ADDITIONAL_METADATA
		for (j=0; (oid_found==0) && (j < bmdDatagram->no_of_additionalMetaData) ; j++)
		{
			dtgOIDTableBufPtr = bmdDatagram->additionalMetaData[j]->OIDTableBuf;
			retval = strcmp(dtgOIDTableBufPtr, OIDTableBufPtr);
			if (retval == 0)
			{
				oid_found = 1;
				bmdOidXpath->DtgNodes[i].MtdType = ADDITIONAL_METADATA;
				bmdOidXpath->DtgNodes[i].MtdPos = j;
			}
			else
			{
				if (retval == BMD_ERR_PARAM1 || retval == BMD_ERR_PARAM2)
				{
					//zglaszamy blad - takiej sytuacji nie powinno byc, cos jest spaprane
				}
			}
		}
		// kolejno jesli nie znaleziono wczesniej to wyszukiwanie w metadanych SYS_METADATA
		for (j=0; (oid_found==0) && (j < bmdDatagram->no_of_sysMetaData) ; j++)
		{
			dtgOIDTableBufPtr = bmdDatagram->sysMetaData[j]->OIDTableBuf;
			retval = strcmp(dtgOIDTableBufPtr, OIDTableBufPtr);
			if (retval == 0)
			{
				oid_found = 1;
				bmdOidXpath->DtgNodes[i].MtdType = SYS_METADATA;
				bmdOidXpath->DtgNodes[i].MtdPos = j;
			}
			else
			{
				if (retval == BMD_ERR_PARAM1 || retval == BMD_ERR_PARAM2)
				{
					//zglaszamy blad - takiej sytuacji nie powinno byc, cos jest spaprane
				}
			}
		}
		// kolejno jesli nie znaleziono wczesniej to wyszukiwanie w metadanych PKI_METADATA
		for (j=0; (oid_found==0) && (j < bmdDatagram->no_of_pkiMetaData) ; j++)
		{
			dtgOIDTableBufPtr = bmdDatagram->pkiMetaData[j]->OIDTableBuf;
			retval = strcmp(dtgOIDTableBufPtr, OIDTableBufPtr);
			if (retval == 0)
			{
				oid_found = 1;
				bmdOidXpath->DtgNodes[i].MtdType = PKI_METADATA;
				bmdOidXpath->DtgNodes[i].MtdPos = j;
			}
			else
			{
				if (retval == BMD_ERR_PARAM1 || retval == BMD_ERR_PARAM2)
				{
					//zglaszamy blad - takiej sytuacji nie powinno byc, cos jest spaprane
				}
			}
		}
		// kolejno jesli nie znaleziono wczesniej to wyszukiwanie w metadanych USER_METADATA
		for (j=0; (oid_found==0) && (j < bmdDatagram->no_of_actionMetaData) ; j++)
		{
			dtgOIDTableBufPtr = bmdDatagram->actionMetaData[j]->OIDTableBuf;
			retval = strcmp(dtgOIDTableBufPtr, OIDTableBufPtr);
			if (retval == 0)
			{
				oid_found = 1;
				bmdOidXpath->DtgNodes[i].MtdType = ACTION_METADATA;
				bmdOidXpath->DtgNodes[i].MtdPos = j;
			}
			else
			{
				if (retval == BMD_ERR_PARAM1 || retval == BMD_ERR_PARAM2)
				{
					//zglaszamy blad - takiej sytuacji nie powinno byc, cos jest spaprane
				}
			}
		}
		if (oid_found==0)	// probujemy czy to bedzie
		{
			dtgOIDTableBufPtr = file_name_oid;
			retval = strcmp(dtgOIDTableBufPtr, OIDTableBufPtr);
			if (retval == 0)
			{
				oid_found = 1;
				bmdOidXpath->DtgNodes[i].MtdType = SYS_METADATA;
				bmdOidXpath->DtgNodes[i].MtdPos = LOCAL_LIB_SYS_METADATA_CRYPTO_OBJECTS_FILENAME;	// specjany znacznik dla SYS_METADATA 1.2.616.1.113527.4.3.2.5.2 - nazwa pliku
			}
			else
			{
				if (retval == BMD_ERR_PARAM1 || retval == BMD_ERR_PARAM2)
				{
					//zglaszamy blad - takiej sytuacji nie powinno byc, cos jest spaprane
				}
			}
		}
		// Jesli nie znaleziono w datagramie, to trzeba jeszcze poszukac w formularzu, czy taka metadana jest w systemie.
		// Jesli metadana zdefiniowana jest w systemie, to wystarczy wstawic falszywy indeks.
		// Na etapie generowania csv (przy wykorzystaniu mapy) niespojnosc w kolejnosci metadanych bedzie wykryta
		// i odpowiednio przeszukana zostanie cala tablica metadanych (dodatkowych) odrebnie dla kazdego datagramu.
		if(oid_found==0)
		{
			// jesli metadana znaleziona w formularzu
			if(forms != NULL)
			{
				if(forms->user_view_form != NULL)
				{
					iter = 0;
					while(forms->user_view_form[iter] != NULL)
					{
						if(forms->user_view_form[iter]->oid != NULL && forms->user_view_form[iter]->oid->buf != NULL && forms->user_view_form[iter]->oid->size > 0)
						{
							char *tmpOid = calloc(forms->user_view_form[iter]->oid->size + 1, sizeof(char));
							if(tmpOid == NULL)
								{ continue; }
							memcpy(tmpOid, forms->user_view_form[iter]->oid->buf, forms->user_view_form[iter]->oid->size * sizeof(char));
							if(strcmp(tmpOid, OIDTableBufPtr) == 0)
							{
								bmdOidXpath->DtgNodes[i].MtdType = determine_oid_type_str(tmpOid);
								bmdOidXpath->DtgNodes[i].MtdPos = 0; // fake index
								oid_found = 1;
								free(tmpOid); tmpOid = NULL;
								break;
							}
							free(tmpOid); tmpOid = NULL;
						}
						iter++;
					}
				}
			}
		}
		if (oid_found==0)
		{
			//zglaszamy blad, albo warning, nie znaleziono w datagramie metadanej o OID podanym w OIDxPath
			PRINT_INFO("No metadata in datagram found for OID in %li OIDXpath.\n",i+1);
			bmdOidXpath->DtgNodes[i].MtdType = OID_NOT_FOUND_IN_DATAGRAM;
			bmdOidXpath->DtgNodes[i].MtdPos = OID_NOT_FOUND_IN_DATAGRAM;
			if (errors)
			{
				//bmd_oid_to_string(,&tmp)
				asprintf(&(errors_tmp[errors_cnt]),"No metadata in datagram found for OID in %li OIDXpath.\n",i+1);
				errors_cnt++;
			}
		}
		// szukamy dla kolejnego OIDA
	}
	if (errors_cnt && errors)
	{
		*errors = errors_tmp;
	}

	return BMD_OK;
}



long _InsertRootTagOnXMLBuf(	char* NewRootTag,
					char* NewRootAtributes,
					GenBuf_t * XMLBuf,
					long with_xml_header,
					char ***error)
{
	int retval = BMD_OK;
	GenBuf_t NewLine;
	GenBuf_t XMLHeader;
	GenBuf_t XMLRoot;
	GenBuf_t tmpXMLBuf;
	int offset=0;

	if (NewRootTag == NULL || strlen(NewRootTag) <=0
		|| XMLBuf==NULL || XMLBuf->buf == NULL || XMLBuf->size <=0)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad wywolania _InsertRootTagOnXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}

	NewLine.buf=NULL;
	NewLine.size=0;
	XMLHeader.buf=NULL;
	XMLHeader.size=0;
	XMLRoot.buf=NULL;
	XMLRoot.size=0;
	tmpXMLBuf.buf=NULL;
	tmpXMLBuf.size=0;

	retval = __GetXMLHeaderFromXMLBuf(&XMLHeader, XMLBuf, &NewLine, error);
	if (retval != BMD_OK)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad wywolania __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return retval;
	}
	if (with_xml_header == 0)
	{
		XMLHeader.size = 0;
	}
	retval = __GetXMLRootFromXMLBuf(&XMLRoot, XMLBuf, error);
	if (retval != BMD_OK)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad wywolania __GetXMLRootFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return retval;
	}

	tmpXMLBuf.size = XMLHeader.size + XMLRoot.size + 2*(int)strlen(NewRootTag)
		+ 4*NewLine.size + (int)strlen("<></>") + (int)strlen(NewRootAtributes);
	tmpXMLBuf.buf = calloc(tmpXMLBuf.size,sizeof(char));

	// przeniesienie naglowka do bufora tymczasowego
	memmove(tmpXMLBuf.buf,XMLHeader.buf,XMLHeader.size);
	if (tmpXMLBuf.buf==NULL)
	{
		PRINT_ERROR("blad wywolania memmove w _InsertRootTagOnXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_OP_FAILED;
	}
	offset += XMLHeader.size;
	//XMLHeader.buf = NULL;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf.buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;
	//otwarcie tagu
	strcpy( (char*)tmpXMLBuf.buf+offset, "<");
	offset += 1;
	//wpisanie tagu nowego roota
	strcpy( (char*)tmpXMLBuf.buf+offset, NewRootTag);
	offset += (int)strlen(NewRootTag);
	//wpisanie atrybutu tagu nowego roota
	retval = (int)strlen(NewRootAtributes);
	if (retval > 0)
	{
		strcpy( (char*)tmpXMLBuf.buf+offset, NewRootAtributes);
		offset += retval;
	}
	//zamkniecie tagu nowego roota
	strcpy( (char*)tmpXMLBuf.buf+offset, ">");
	offset += 1;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf.buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;
	// doklejenie pobranego root'a do bufora tymczasowego
	memmove(tmpXMLBuf.buf+offset, XMLRoot.buf,XMLRoot.size);
	offset += XMLRoot.size;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf.buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;
	//otwarcie tagu zamykajacego
	strcpy( (char*)tmpXMLBuf.buf+offset, "</");
	offset += 2;
	//wpisanie tagu nowego roota
	strcpy( (char*)tmpXMLBuf.buf+offset, NewRootTag);
	offset += (int)strlen(NewRootTag);
	//zamkniecie tagu nowego roota
	strcpy( (char*)tmpXMLBuf.buf+offset, ">");
	offset += 1;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf.buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;

	//sprawdzamy czy nie przekroczylismy bufora... (czy na koncu jest NULL)
	retval = BMD_OK;
	//if (tmpXMLBuf.buf[tmpXMLBuf.size-1] != 0)
	//{
	//	PRINT_ERROR("Internal Error in _InsertRootTagOnXMLBuf. Buffer not ended properly.\n");
	//	retval -= 1;
	//}
	if (offset > tmpXMLBuf.size)
	{
		PRINT_ERROR("Internal Error in _InsertRootTagOnXMLBuf. Buffer overflow detected.\n");
		retval -= 1;
	}
	if (retval == BMD_OK)
	{
		free(XMLBuf->buf);
		XMLBuf->buf = tmpXMLBuf.buf;
		XMLBuf->size = tmpXMLBuf.size;
		tmpXMLBuf.buf = NULL;
	}
	else
	{
		free(tmpXMLBuf.buf);
	}
	free(NewLine.buf);
	free(XMLHeader.buf);
	free(XMLRoot.buf);

	NewLine.buf=NULL;
	NewLine.size=0;
	XMLHeader.buf=NULL;
	XMLHeader.size=0;
	XMLRoot.buf=NULL;
	XMLRoot.size=0;
	tmpXMLBuf.buf=NULL;
	tmpXMLBuf.size=0;

	return retval;
}

/** Funkcja pobierajaca naglowek XMLa, oraz okreslajaca znak konca linii zastosowany w dokumencie XML
@param Header bufor na odnaleziony naglowek
@param XMLBuf bufor zrodlowego dokumentu XML
@param newline bufor na znak konca linii znaleziony za naglowkiem
@param error stos bledow??
*/
long __GetXMLHeaderFromXMLBuf(	GenBuf_t * Header,
						GenBuf_t * XMLBuf,
						GenBuf_t * newline,
						char ***error)
{
	/*int i=0;*/
	long offset=0;
	char * bptr = NULL;
	if (Header == NULL || newline==NULL
		 || XMLBuf == NULL || XMLBuf->buf == NULL || XMLBuf->size <=6)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad parametrow __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}

	//sprawdzamy czy naglowek poprawnie sie zaczyna od "<?xml"
	if ( 0 != memcmp(XMLBuf->buf, "<?xml", 5) )
	{
		PRINT_ERROR("Bledny naglowek w __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}
	//szukamy znaku ">" i zapamietujemy pozycje
	bptr = strchr((char*)XMLBuf->buf,'>');
	if (bptr == NULL)
	{
		PRINT_ERROR("Bledny naglowek w __GetXMLHeaderFromXMLBuf. Nie zakonczony znakiem '>'.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}
	offset = ((long)bptr - (long)XMLBuf->buf) + 1;
	Header->buf = calloc(offset,sizeof(char));
	memcpy(Header->buf, XMLBuf->buf, offset);
	Header->size = offset;

	//okreslamy znak konca linii po ">" czy jest to 13 10, czy 10, czy 13
	if (bptr[1]==13 && bptr[2]==10)
	{
		newline->buf = calloc(2,sizeof(char));
		strncpy((char *)newline->buf, bptr+1, 2);
		newline->size = 2;
	}
	else
	//if (bptr[1]==10)	// taki znak dajemy domyslnie jesli nie jest 13 10
	{
		newline->buf = calloc(1,sizeof(char));
		newline->buf[0] = (char) 10;
		newline->size = 1;
	}
	//sprawdzamy czy nastepny tag jest juz tagiem dokumentu czy tagiem <costam xmlns:jakis name space
	// nie zaimlementowane
	// nalezalo by utworzyc nagowek dopiero tutaj a nie wyzej po sprawdzeniu ze juz caly
	return BMD_OK;
}

long __GetXMLRootFromXMLBuf(	GenBuf_t * Root,
					GenBuf_t * XMLBuf,
					char ***error)
{
	//tak jak w __GetXMLHeaderFromXMLBuf szukamy pierwszego tagu po naglowku
	//tu ograniczamy sie do odnalezienia pierwszego otwarcia tagu
	//! uwaga - stosowanie name spaceow spowoduje wlaczenie namespace do roota
	//return BMD_ERR_UNIMPLEMENTED;

	/*int i=0;*/
	long offset=0;
	char * bptr = NULL;
	if (Root == NULL
		 || XMLBuf == NULL || XMLBuf->buf == NULL || XMLBuf->size <=6)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad parametrow __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}

	//sprawdzamy czy naglowek poprawnie sie zaczyna od "<?xml"
	if ( 0 != memcmp(XMLBuf->buf, "<?xml", 5) )
	{
		PRINT_ERROR("Bledny naglowek w __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}
	//szukamy pierwszego po "<?xml" znaku "<" i zapamietujemy pozycje
	bptr = strchr((char *)XMLBuf->buf+5,'<');
	if (bptr == NULL)
	{
		PRINT_ERROR("Bledny naglowek w __GetXMLHeaderFromXMLBuf. Nie zakonczony znakiem '>'.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_PARAM1;
	}
	offset = XMLBuf->size - ((long)bptr - (long)XMLBuf->buf);
	Root->buf = calloc(offset,sizeof(char));
	memcpy(Root->buf, bptr, offset);
	Root->size = offset;
	return BMD_OK;
}

long _MergeDtgXMLs2DtgSetXML(	GenBuf_t * DtgSetXMLBuf,
					GenBuf_t * DtgsXMLBufs,
					long DtgsXMLCnt,
					bmd_oid_xpath_t *bmdOidXpath,
					char ***error)
{
	int retval = 0;
	int offset=0;
	long i = 0;
	GenBuf_t *tmpXMLBuf = NULL;
	GenBuf_t NewLine;
	GenBuf_t XMLHeader;
	GenBuf_t *XMLDtgCpyPtr=NULL;
	char * NewRootAtributes = NULL;
	char * NewRootTag = NULL;

	NewLine.buf=NULL;
	NewLine.size=0;
	XMLHeader.buf=NULL;
	XMLHeader.size=0;

	//inicjalizacja
	NewRootTag = STRDUPL("datagramset_metadata");
	asprintf(&NewRootAtributes, " number=\"%lu\"",bmdOidXpath->datagramset_cnt);

	// zapisuje do genbufa xml z bmdOidXpath->...->doc - dla wyciagniecia przykladowego naglowka XML
	retval = bmdxpath_doc2genbuf(bmdOidXpath->libbmdxpath_ctx->ctx->doc, &tmpXMLBuf );
	if (retval != BMD_OK)
	{
		return -1;
	}
	// wyciaga header i znak konca linii z tego dokumentu
	retval = __GetXMLHeaderFromXMLBuf(&XMLHeader, tmpXMLBuf, &NewLine, error);
	if (retval != BMD_OK)
	{
		//dopisac do error, ze jest blad
		PRINT_ERROR("blad wywolania __GetXMLHeaderFromXMLBuf.\n");
		//PRINT_DEBUG("");
		return retval;
	}
	free(tmpXMLBuf->buf);
	tmpXMLBuf->size = 0;

	// zlicza wszystkie size
	tmpXMLBuf->size = 0;
	for (i=0; i < DtgsXMLCnt; i++)
	{
		tmpXMLBuf->size += DtgsXMLBufs[i].size;
	}
	tmpXMLBuf->size += XMLHeader.size + 2*(int)strlen(NewRootTag)
		+ (4+DtgsXMLCnt)*NewLine.size + (int)strlen("<></>") + (int)strlen(NewRootAtributes);
	tmpXMLBuf->buf = calloc(tmpXMLBuf->size, sizeof(char));

	// memmove header i w petli
		//memmove contenty
	offset = 0;
	// przeniesienie naglowka do bufora tymczasowego
	memmove(tmpXMLBuf->buf,XMLHeader.buf,XMLHeader.size);
	if (tmpXMLBuf->buf==NULL)
	{
		PRINT_ERROR("blad wywolania memmove w _InsertRootTagOnXMLBuf.\n");
		//PRINT_DEBUG("");
		return BMD_ERR_OP_FAILED;
	}
	offset += XMLHeader.size;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf->buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;
	//otwarcie tagu
	strcpy( (char*)tmpXMLBuf->buf+offset, "<");
	offset += 1;
	//wpisanie tagu nowego roota
	strcpy( (char*)tmpXMLBuf->buf+offset, NewRootTag);
	offset += (int)strlen(NewRootTag);
	//wpisanie atrybutu tagu nowego roota
	retval = (int)strlen(NewRootAtributes);
	if (retval > 0)
	{
		strcpy( (char*)tmpXMLBuf->buf+offset, NewRootAtributes);
		offset += retval;
	}
	//zamkniecie tagu nowego roota
	strcpy( (char*)tmpXMLBuf->buf+offset, ">");
	offset += 1;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf->buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;

	for (i=0; i < DtgsXMLCnt; i++)
	{
		XMLDtgCpyPtr = DtgsXMLBufs+i;
		// doklejenie pobranego datagramu do bufora tymczasowego
		memmove(tmpXMLBuf->buf+offset, XMLDtgCpyPtr->buf,XMLDtgCpyPtr->size);
		offset += XMLDtgCpyPtr->size;
		//dopisanie konca linii po datagramie
		memcpy( tmpXMLBuf->buf+offset , NewLine.buf, NewLine.size);
		offset += NewLine.size;
	}

	//otwarcie tagu zamykajacego
	strcpy( (char*)tmpXMLBuf->buf+offset, "</");
	offset += 2;
	//wpisanie tagu nowego roota
	strcpy( (char*)tmpXMLBuf->buf+offset, NewRootTag);
	offset += (int)strlen(NewRootTag);
	//zamkniecie tagu nowego roota
	strcpy( (char*)tmpXMLBuf->buf+offset, ">");
	offset += 1;
	//dopisanie konca linii po naglowku
	memcpy( tmpXMLBuf->buf+offset , NewLine.buf, NewLine.size);
	offset += NewLine.size;

	//sprawdzamy czy nie przekroczylismy bufora... (czy na koncu jest NULL)
	retval = BMD_OK;
	if (tmpXMLBuf->buf[tmpXMLBuf->size-1] != 0)
	{
		PRINT_ERROR("Internal Error in _InsertRootTagOnXMLBuf. Buffer not ended properly.\n");
		retval -= 1;
	}
	if (offset > tmpXMLBuf->size)
	{
		PRINT_ERROR("Internal Error in _InsertRootTagOnXMLBuf. Buffer overflow detected.\n");
		retval -= 1;
	}
	if (retval == BMD_OK)
	{
		free(DtgSetXMLBuf->buf);
		DtgSetXMLBuf->buf = tmpXMLBuf->buf;
		DtgSetXMLBuf->size = tmpXMLBuf->size;
		tmpXMLBuf->buf = NULL;
	}
	else
	{
		free(tmpXMLBuf->buf);
	}
	free(NewLine.buf);
	free(XMLHeader.buf);

	NewLine.buf=NULL;
	NewLine.size=0;
	XMLHeader.buf=NULL;
	XMLHeader.size=0;
	tmpXMLBuf->buf=NULL;
	tmpXMLBuf->size=0;

	return retval;
}

/****************************************/
/*	Funkcje do obslugi konfiguracji XML	*/
/****************************************/


//wczytuje xpath bmd_metadata_template/csv_download_metadata_settings/oid_csv_coll_name
	//sprawdza czy sa jakies XMLNodes



long bmdVerifyXmlDownloadSettings(	xmlDocPtr doc,
						char ***errors_ptr)
{
	int retval;
	char ** errors = NULL;

	bmd_oid_xpath_t * XMLbmdOidXpath = NULL;
	bmd_oid_xpath_t * CSVbmdOidXpath = NULL;
#if 1
	if (doc == NULL)
	{
		return BMD_ERR_PARAM1;
	}
#endif

	retval = bmdGetMtdDownloadSettings(doc, &XMLbmdOidXpath, &CSVbmdOidXpath, &errors);

	if (retval != BMD_OK)
	{
		// dopisz errors do errors_ptr
	}

	bmdFreeBmdOidXpathStructElements(&XMLbmdOidXpath,&errors);
	bmdFreeBmdOidXpathStructElements(&CSVbmdOidXpath,&errors);

	return retval;
}

long bmdGetMtdDownloadSettings(xmlDocPtr doc, bmd_oid_xpath_t ** XMLbmdOidXpath, bmd_oid_xpath_t ** CSVbmdOidXpath, char ***errors_ptr)
{
	int retval,i;
	int j;
	xmlDocPtr tmpdoc = NULL, xmltemplate =NULL;
	xmlNodeSetPtr NodeSetPtr = NULL;
	xmlNodePtr NodePtr = NULL;
	GenBuf_t *bufor_tmp = NULL;
	char * value = NULL;
	char * tmp_value = NULL;
	int XMLReady = 0, CSVReady = 0;
	char **xpaths=NULL, **errors = NULL;
	/*bmd_oid_xpath_t * bmdOidXpath = NULL;*/
	char ** oidxpathsTmpTablePtr = NULL;
	/*char * oidxpathsTmpElemPtr = NULL;*/
	bmdxpath_info_t *xpathctx = NULL;
	bmd_oid_xpath_t *XMLbmdOidXpathPtr=NULL, *CSVbmdOidXpathPtr=NULL;
	size_t len;
	char *cell_separator= NULL;
	char *text_separator= NULL;
	char **cols = NULL;
	struct _xmlAttr * propertPtr;
	char tmpsrc_dec_separator=0;						// zrodlowy separator ulamkow dziesietnych (jezeli ma byc zmieniany)
	char tmpdst_dec_separator=0;						// docelowy separator ulamkow dziesietnych (jezeli ma byc zmieniany)
	int tmp_code_page=0;
	char* tmp_code_page_char=NULL;
	iconv_t tmp_convdesc = 0;
	size_t offset = 0;


	if (doc == NULL || ((XMLbmdOidXpath == NULL || *XMLbmdOidXpath != NULL)
					&&  (CSVbmdOidXpath == NULL || *CSVbmdOidXpath != NULL)) )
	{
		PRINT_ERROR("bmdGetMtdDownloadSettings() parameter error.\n");
		return BMD_ERR_PARAM1;
	}

	retval = bmdxpath_init(doc,&xpathctx);
	if (retval != BMD_OK)
	{
		// jakis komunikat na stos
		return BMD_ERR_OP_FAILED;
	}

	retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings");		//wyszukuje xpath bmd_metadata_settings
	if (retval != BMD_OK)
	{
		add_error(errors_ptr, "Error getting metadata download settings.\n");
		add_error(errors_ptr, "tag <bmd_metadata_settings> not found\n");
		return BMD_ERR_FORMAT;
	}
	//sprawdza atrybut version czy jest rowny...
	retval = bmdxpath_get_string(xpathctx,"//bmd_metadata_settings/@version",&value);
	if (retval != BMD_OK)
	{
		// jakis komunikat na stos
		return BMD_ERR_FORMAT;
	}
	
	if(value == NULL || strcmp(value,DTG_2_XML_SETTINGS_FILE_VERSION) != 0)
	{
		PRINT_ERROR("Unsuported version of XML settings and template file.\n");
		PRINT_DEBUG("Selected file is in version %s.\n",value);
		add_error(errors_ptr, "Error getting metadata download settings.\n");
		add_error(errors_ptr, "Unsuported version of XML settings and template file.\n");
//TODO trunk uzupelnic		asprintf(&c_tmp,"Selected file is in version %s.\n",value);
		//add_error(errors_ptr, c_tmp);
		//free(c_tmp);	c_tmp = NULL;
		free(value);	value = NULL;
		return BMD_ERR_FORMAT;
	}
	free(value); value = NULL;

	//sprawdza atrybut subversion czy jest rozny...
	retval = bmdxpath_get_string(xpathctx,"//bmd_metadata_settings/@subversion",&value);
	if (retval != BMD_OK)
	{
		// jakis komunikat na stos
		return BMD_ERR_FORMAT;
	}
	
	if(value == NULL || strcmp(value,DTG_2_XML_SETTINGS_FILE_SUBVERSION) != 0)
	{
		PRINT_INFO("Diffrent version of XML settings and template file.\n");
		PRINT_DEBUG("Selected file is in version: %s.%s.\n",DTG_2_XML_SETTINGS_FILE_VERSION, value);
		PRINT_DEBUG("This software uses version:  %s.%s.\n",DTG_2_XML_SETTINGS_FILE_VERSION, DTG_2_XML_SETTINGS_FILE_SUBVERSION);
	}
	else
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/options/number_conversion/src_dec_separator");		//wyszukuje
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/options/text_separator_char in XmlDownloadSettings File.\n");
			// jakis komunikat na stos
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			tmpsrc_dec_separator = ((char*)NodePtr->children->content)[0];
			NodePtr = NULL;
		}

		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/options/number_conversion/dst_dec_separator");		//wyszukuje
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			tmpdst_dec_separator = tmpsrc_dec_separator;
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/options/text_separator_char in XmlDownloadSettings File.\n");
			// jakis komunikat na stos
			tmpdst_dec_separator = tmpsrc_dec_separator;
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			tmpdst_dec_separator = ((char*)NodePtr->children->content)[0];
			NodePtr = NULL;
		}

		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/options/code_page");		//wyszukuje
		if (retval == BMD_OK)
		{
			//sprawdza czy sa jakies XMLNodes
			if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
			{
				// jakis komunikat na stos
			}
			else
			{
				NodePtr = xpathctx->result->nodesetval->nodeTab[0];
				tmp_code_page_char = (char*)NodePtr->children->content;
				if ((STRCASECMP(tmp_code_page_char, "UTF8") == 0) ||
					(STRCASECMP(tmp_code_page_char, "UTF") == 0) )
				{
					tmp_code_page=0;
				}
				else
				{
					if ((STRCASECMP(tmp_code_page_char, "CP1250") == 0) ||
						(STRCASECMP(tmp_code_page_char, "CP-1250") == 0) ||
						(STRCASECMP(tmp_code_page_char, "Windows1250") == 0) ||
						(STRCASECMP(tmp_code_page_char, "Windows-1250") == 0))
					{
						tmp_code_page = BMD_CODE_PAGE_CP1250;
					}
					else
					{
						if ((STRCASECMP(tmp_code_page_char, "ISO 8859-2") == 0) ||
							(STRCASECMP(tmp_code_page_char, "ISO8859-2") == 0) ||
							(STRCASECMP(tmp_code_page_char, "ISO/IEC 8859-2") == 0) ||
							(STRCASECMP(tmp_code_page_char, "ISO/IEC8859-2") == 0) ||
							(STRCASECMP(tmp_code_page_char, "Latin-2") == 0) ||
							(STRCASECMP(tmp_code_page_char, "Latin2") == 0) )
						{
							tmp_code_page = BMD_CODE_PAGE_ISO88592;
						}
						else
						{
							PRINT_ERROR("Code Page idetifier not recognized. Using UTF8.\n");
							tmp_code_page=0;
						}
					}
				}
			}
		}
		/****************************/
		/*	Opcjonalne dla CSV		*/
		/****************************/
		switch( tmp_code_page)
		{
		case 0:
			break;
		case BMD_CODE_PAGE_CP1250:
			tmp_convdesc = iconv_open("CP1250","UTF-8");
			break;
		case BMD_CODE_PAGE_ISO88592:
			tmp_convdesc = iconv_open("ISO-8859-2","UTF-8");
			break;
		default:
			//"Conversion unrecognized"
			break;
		}
		/* sprawdzenie inicjalizacji */
		if (tmp_convdesc == (iconv_t)-1)
		{
		//	error(FATAL, BAD_OPEN);
			retval = -4;
		}

	}
	free(value); value = NULL;


#if 1		// ustawienia XML-owe
	//wczytuje xpath bmd_metadata_settings/xml_download_metadata_settings
	if (XMLbmdOidXpath == NULL)
	{
		XMLReady = -1;
	}

	retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/xml_download_metadata_settings");		//wyszukuje xml_download_metadata_settings
	if (retval != BMD_OK)
	{
		// jakis komunikat na stos
		XMLReady = -1;
	}

	if (XMLReady == 0)
	{
		//wczytuje xpath bmd_metadata_settings/xml_download_metadata_settings/template
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/xml_download_metadata_settings/template");
		//sprawdza czy sa jakies XMLNodes (minimum 1)
		if (retval!=BMD_OK || xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeTab == NULL)
		{
			PRINT_INFO("could not find bmd_metadata_settings/xml_download_metadata_settings/template in XmlDownloadSettings File.\n");
			// zapisujemy blad do stosu
			XMLReady = -1;
		}
		else
		{
			NodeSetPtr = xpathctx->result->nodesetval;
			NodePtr = NodeSetPtr->nodeTab[0];
			//sprawdza czy da sie z tempate utworzyc nowy dokument XML
			if (NodePtr->children->next == NULL)
			{
				PRINT_ERROR("bmd_metadata_settings/xml_download_metadata_settings/template in XmlDownloadSettings File is empty.\n");
				XMLReady = -1;
			}
			if (XMLReady == 0)
			{
				tmpdoc  = xmlNewDoc(BAD_CAST "1.0");
				xmlDocSetRootElement(tmpdoc , NodePtr->children->next);
				//bufor_tmp = calloc(1,sizeof(GenBuf_t));
				bmdxpath_doc2genbuf(tmpdoc, &bufor_tmp);
				xmltemplate=xmlParseMemory((const char *)bufor_tmp->buf,bufor_tmp->size);
				free_gen_buf(&bufor_tmp);

				if(xmltemplate == NULL)
				{
					PRINT_ERROR("Attemp to parse XML template failed in bmdGetMtdDownloadSettings().\n");
					XMLReady = -1;
				}
			}
			NodePtr = NULL;
			NodeSetPtr = NULL;
		}
	}
	if (XMLReady == 0)
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/xml_download_metadata_settings/oid_xpath");		//wyszukuje oid_xpath
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			XMLReady = -1;
		}
	}
	//sprawdza czy sa jakies XMLNodes
	if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
	{
		PRINT_INFO("could not find any bmd_metadata_settings/xml_download_metadata_settings/oid_xpath in XmlDownloadSettings File.\n");
		// jakis komunikat na stos
		XMLReady = -1;
	}

	if (XMLReady == 0)
	{
		if (XMLbmdOidXpathPtr == NULL)
		{
			XMLbmdOidXpathPtr = calloc(1, sizeof(bmd_oid_xpath_t));
		}
		NodeSetPtr = xpathctx->result->nodesetval;
		oidxpathsTmpTablePtr = calloc(NodeSetPtr->nodeNr+1, sizeof(char*));
		XMLbmdOidXpathPtr->ColValType = calloc(NodeSetPtr->nodeNr+1, sizeof(char));
		xpaths = calloc(NodeSetPtr->nodeNr+1, sizeof(char*));

		for (i=0; i < NodeSetPtr->nodeNr; i++)
		{
			NodePtr = NodeSetPtr->nodeTab[i];
			oidxpathsTmpTablePtr[i] = STRDUPL((char*)NodePtr->children->content);

			if (XMLbmdOidXpathPtr->ColValType)
			{
				propertPtr = NodePtr->properties;
				while (propertPtr && propertPtr->name && propertPtr->children && propertPtr->children->content)
				{
					if (strcmp((const char *)propertPtr->name, "type") == 0)
					{
						XMLbmdOidXpathPtr->ColValType[i] = propertPtr->children->content[0];
					}
					propertPtr = propertPtr->next;
				}
				propertPtr = NULL;
			}
			NodePtr = NULL;
		}
		NodeSetPtr = NULL;

		retval = bmdInitBmdOidXpathStruct4XML(xmltemplate, XMLbmdOidXpathPtr, oidxpathsTmpTablePtr, &errors );
		xmltemplate = NULL;	//zapominamy o tym obiekcie, bo pamieta go juz struktura bmdOidXpath
		bmdxpath_free_strings(&oidxpathsTmpTablePtr);
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			XMLReady = -1;
		}
	}
	else
	{
		if (xmltemplate)
		{
			xmlFreeDoc(xmltemplate);
		}
	}
#endif
#if 1		// ustawienia CSV-owe
	if (CSVbmdOidXpath == NULL)
	{
		CSVReady = -1;
	}
	//wczytuje xpath bmd_metadata_settings/xml_download_metadata_settings
	retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings");		//wyszukuje xml_download_metadata_settings
	if (retval != BMD_OK)
	{
		// jakis komunikat na stos
		CSVReady = -1;
	}
	else
	{
		if (CSVbmdOidXpathPtr == NULL)
		{
			CSVbmdOidXpathPtr = calloc(1, sizeof(bmd_oid_xpath_t));
		}
	}

	if (CSVReady == 0)
	{
		CSVbmdOidXpathPtr->accvwqm = 0;
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings/options/always_close_cell_value_with_quoatation_mark");		//wyszukuje oid_xpath
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/csv_download_metadata_settings/options/always_close_cell_value_with_quoatation_mark in XmlDownloadSettings File.\n");
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			if (((char*)NodePtr->children->content)[0]!='0')
			{
				CSVbmdOidXpathPtr->accvwqm = 1;
			}
			NodePtr = NULL;
		}
	}

	if (CSVReady == 0)
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings/oid_csv_coll_name");		//wyszukuje oid_xpath
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			CSVReady = -1;
		}
	}
	//sprawdza czy sa jakies XMLNodes
	if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
	{
		PRINT_INFO("could not find any bmd_metadata_settings/csv_download_metadata_settings/options/oid_csv_coll_name in XmlDownloadSettings File.\n");
		// jakis komunikat na stos
		CSVReady = -1;
	}

	if (CSVReady == 0)
	{
		NodeSetPtr = xpathctx->result->nodesetval;
		oidxpathsTmpTablePtr = calloc(NodeSetPtr->nodeNr+1, sizeof(char*));

		CSVbmdOidXpathPtr->ColCcvwqm = calloc(NodeSetPtr->nodeNr+1, sizeof(char));
		CSVbmdOidXpathPtr->ColValType = calloc(NodeSetPtr->nodeNr+1, sizeof(char));

		for (i=0; i < NodeSetPtr->nodeNr; i++)
		{
			NodePtr = NodeSetPtr->nodeTab[i];
			offset = 0;
			while ((int)(NodePtr->children->content+offset)[0] < 48)
			{
				offset++;
			}
			oidxpathsTmpTablePtr[i] = STRDUPL((char*)NodePtr->children->content+offset);


			if (CSVbmdOidXpathPtr->ColCcvwqm && CSVbmdOidXpathPtr->ColValType)
			{
				propertPtr = NodePtr->properties;
				while (propertPtr && propertPtr->name && propertPtr->children && propertPtr->children->content)
				{
					if (strcmp((const char *)propertPtr->name, "type") == 0)
					{
						CSVbmdOidXpathPtr->ColValType[i] = propertPtr->children->content[0];
					}
					else
					if (strcmp((const char *)propertPtr->name, "ccvwqm") == 0)
					{
						CSVbmdOidXpathPtr->ColCcvwqm[i] = propertPtr->children->content[0];
					}
					propertPtr = propertPtr->next;
				}
				propertPtr = NULL;
			}
			NodePtr = NULL;
		}
		NodeSetPtr = NULL;

		retval = bmdInitBmdOidXpathStruct4CSV(CSVbmdOidXpathPtr, oidxpathsTmpTablePtr, &errors );
		bmdxpath_free_strings(&oidxpathsTmpTablePtr);
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			CSVReady = -1;
		}
	}

	if (CSVReady == 0)
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings/options/cell_separator_char");		//wyszukuje oid_xpath
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/csv_download_metadata_settings/options/cell_separator_char in XmlDownloadSettings File.\n");
			// jakis komunikat na stos
			CSVbmdOidXpathPtr->cell_separator_char = ';';
			//CSVReady = -1;
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			CSVbmdOidXpathPtr->cell_separator_char = ((char*)NodePtr->children->content)[0];
			ValidateCellSep(&CSVbmdOidXpathPtr->cell_separator_char);
			NodePtr = NULL;
		}
	}

	if (CSVReady == 0)
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings/options/text_separator_char");		//wyszukuje
		if (retval != BMD_OK)
		{
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/csv_download_metadata_settings/options/text_separator_char in XmlDownloadSettings File.\n");
			// jakis komunikat na stos
			//CSVReady = -1;
			CSVbmdOidXpathPtr->text_separator_char = '\"';
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			CSVbmdOidXpathPtr->text_separator_char = ((char*)NodePtr->children->content)[0];
			ValidateTextSep(&CSVbmdOidXpathPtr->text_separator_char);
			NodePtr = NULL;
		}
	}

	if (CSVReady == 0)
	{
		retval = bmdxpath_find(xpathctx,"//bmd_metadata_settings/csv_download_metadata_settings/options/escape_char");		//wyszukuje
		if (retval != BMD_OK)
		{
			CSVbmdOidXpathPtr->escape_char = CSVbmdOidXpathPtr->text_separator_char;
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		//sprawdza czy sa jakies XMLNodes
		if (xpathctx->result->nodesetval == NULL || xpathctx->result->nodesetval->nodeNr < 1)
		{
			PRINT_INFO("could not find any bmd_metadata_settings/csv_download_metadata_settings/options/escape_char in XmlDownloadSettings File.\n");
			CSVbmdOidXpathPtr->escape_char = CSVbmdOidXpathPtr->text_separator_char;
			// jakis komunikat na stos
			//CSVReady = -1;
		}
		else
		{
			NodePtr = xpathctx->result->nodesetval->nodeTab[0];
			CSVbmdOidXpathPtr->escape_char = ((char*)NodePtr->children->content)[0];
			//ValidateTextSep(&CSVbmdOidXpathPtr->escape_char);
			NodePtr = NULL;
		}
	}

	if (CSVReady == 0 && CSVbmdOidXpathPtr->CSVNodes == NULL)
	{
		// jakis komunikat na stos
		CSVReady = -1;
	}

	if (CSVReady == 0)
	{
		/****************************/
		/*	tworzenie naglowka CSV	*/
		/****************************/

		cell_separator = STRDUPL(";");
		cell_separator[0] = CSVbmdOidXpathPtr->cell_separator_char;
		text_separator = STRDUPL("\"");
		text_separator[0] = CSVbmdOidXpathPtr->text_separator_char;

		CSVbmdOidXpathPtr->CSV_header.size = 0;
		if (CSVbmdOidXpathPtr->CSV_header.buf)
		{
			free(CSVbmdOidXpathPtr->CSV_header.buf);
		}
		CSVbmdOidXpathPtr->CSV_header.buf = NULL;

		cols = calloc(CSVbmdOidXpathPtr->oid_cnt, sizeof(char*));

		if (tmp_code_page)
		{
			CSVbmdOidXpathPtr->cd = tmp_convdesc;
			for (j=0; j < CSVbmdOidXpathPtr->oid_cnt; j++)
			{
				tmp_value = STRDUPL(CSVbmdOidXpathPtr->CSVNodes[j]);
				if (change_code_page(CSVbmdOidXpathPtr, &tmp_value) != BMD_OK)
				{
					free(tmp_value);
					tmp_value = STRDUPL("#code page conversion error#");
				}
				value = CSVEscape(CSVbmdOidXpathPtr, tmp_value,1);
				free(tmp_value);
				len = strlen(value);
				cols[j] = calloc(len + 5, sizeof(char));	//3 na znaki separatorow i 2 na znak konca stringa
				strncat(cols[j], text_separator , 1);
				strncat(cols[j], value , len);
				strncat(cols[j], text_separator , 1);
				strncat(cols[j], cell_separator , 1);
				free(value);
			}
		}
		else
		{
			for (j=0; j < CSVbmdOidXpathPtr->oid_cnt; j++)
			{
				value = CSVEscape(CSVbmdOidXpathPtr, CSVbmdOidXpathPtr->CSVNodes[j],1);
				len = strlen(value);
				cols[j] = calloc(len + 5, sizeof(char));	//3 na znaki separatorow i 2 na znak konca stringa
				strncat(cols[j], text_separator , 1);
				strncat(cols[j], value , len);
				strncat(cols[j], text_separator , 1);
				strncat(cols[j], cell_separator , 1);
				free(value);
			}
		}
		// skasowanie ostatniego ; w wierszu i zastapienie go znakiem konca linii (13,10) lub 10
		if (CSVbmdOidXpathPtr->oid_cnt)
		{
			len = strlen(cols[j-1]);
#ifdef WIN32
			(cols[j-1])[len-1] = 13;
			(cols[j-1])[len] = 10;
#else
			(cols[j-1])[len-1] = 10;
			(cols[j-1])[len] = 0;
#endif
			retval = bmdxpath_strings2string_freetable((char **)&(CSVbmdOidXpathPtr->CSV_header.buf), &len, &cols, CSVbmdOidXpathPtr->oid_cnt);
			CSVbmdOidXpathPtr->CSV_header.size = (int)len;
			if (retval != BMD_OK)
			{
				CSVbmdOidXpathPtr->CSV_header.size = 0;
			}
		}
		free(cell_separator);cell_separator= NULL;
		free(text_separator);text_separator= NULL;
	}

#endif

	if (XMLReady == 0 && XMLbmdOidXpath != NULL)
	{
		XMLbmdOidXpathPtr->src_dec_separator = tmpsrc_dec_separator;
		XMLbmdOidXpathPtr->dst_dec_separator = tmpdst_dec_separator;
		XMLbmdOidXpathPtr->for_XML_CSV = 1;
		*XMLbmdOidXpath = XMLbmdOidXpathPtr;
		XMLbmdOidXpathPtr = NULL;
	}
	if (CSVReady == 0 && CSVbmdOidXpath != NULL)
	{
		CSVbmdOidXpathPtr->src_dec_separator = tmpsrc_dec_separator;
		CSVbmdOidXpathPtr->dst_dec_separator = tmpdst_dec_separator;
		CSVbmdOidXpathPtr->for_XML_CSV = 2;
		CSVbmdOidXpathPtr->cd = tmp_convdesc;
		*CSVbmdOidXpath = CSVbmdOidXpathPtr;
		CSVbmdOidXpathPtr = NULL;
		tmp_convdesc = NULL;
	}

	return BMD_OK;
}


/** Funkcja pobierajaca ustawienia do pobierania metadanych do XML
	Funkcja pobiera ustawienia z pliku mdtml (odpowiednia struktura XML - \src\klient\configs\metadata settings and template.mdtml)
@note po uzyciu nalezy zwolnic MDTMLFilePath oraz ewentualnie errors_ptr

@param bmdOidXpath - wskaznik na strukture docelowa
@param MDTMLFilePath - sciezka do pliku zawierajacego ustawienia (plik w formacie mdtml - odpowiednia struktura XML - \src\klient\configs\metadata settings and template.mdtml)
@param errors_ptr - nie uzywany jeszcze stos bledow
*/
long bmdMtdD2XMLGetDownloadSettingsFromMDTML(	bmd_oid_xpath_t ** bmdOidXpath,
								char *MDTMLFilePath,
								char ***errors_ptr)
{
	int retval;
	char ** errors = NULL;
	xmlDocPtr doc = NULL;

#if 1
	if (bmdOidXpath == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if (MDTMLFilePath == NULL || strlen(MDTMLFilePath) < 1)
	{
		return BMD_ERR_PARAM2;
	}
#endif
	retval = bmdOpenXmlDoc(&doc, MDTMLFilePath);
	if (retval != BMD_OK)
	{
		add_error(errors_ptr, "Error getting metadata download settings.\n");
		add_error(errors_ptr, "Error opening file: \n");
		add_error(errors_ptr, MDTMLFilePath);
		PRINT_ERROR("bmdOpenXmlDoc w bmdGetXmlDownloadSettings.\n");
		return -3;
	}
	if (errors_ptr)
	{
		errors = *errors_ptr;
	}
	retval = bmdGetMtdDownloadSettings(doc, bmdOidXpath, NULL, &errors);
	xmlFreeDoc(doc);
	// wrzucic obsluge stosu bledow
	return retval;
}

/** Funkcja pobierajaca ustawienia do pobierania metadanych do CSV
	Funkcja pobiera ustawienia z pliku mdtml (odpowiednia struktura XML - \src\klient\configs\metadata settings and template.mdtml)
@note po uzyciu nalezy zwolnic MDTMLFilePath oraz ewentualnie errors_ptr

@param bmdOidXpath - wskaznik na strukture docelowa
@param MDTMLFilePath - sciezka do pliku zawierajacego ustawienia (plik w formacie mdtml - odpowiednia struktura XML - \src\klient\configs\metadata settings and template.mdtml)
@param errors_ptr - nie uzywany jeszcze stos bledow
*/
long bmdMtdD2CSVGetDownloadSettingsFromMDTML(	bmd_oid_xpath_t ** bmdOidXpath,
								char *MDTMLFilePath,
								char ***errors_ptr)
{
	int retval;
	char ** errors = NULL;
	xmlDocPtr doc = NULL;

#if 1
	if (bmdOidXpath == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if (MDTMLFilePath == NULL || strlen(MDTMLFilePath) < 1)
	{
		return BMD_ERR_PARAM2;
	}
#endif
	retval = bmdOpenXmlDoc(&doc, MDTMLFilePath);
	if (retval != BMD_OK)
	{
		//wypisz blad
		PRINT_ERROR("bmdOpenXmlDoc w bmdGetXmlDownloadSettings.\n");
		return -3;
	}

	retval = bmdGetMtdDownloadSettings(doc, NULL, bmdOidXpath, &errors);
	xmlFreeDoc(doc);
	doc = NULL;
	// wrzucic obsluge stosu bledow
	return retval;
}

/* Funkcja ustawiajaca docelowy folder na pobrane pliki metadanych w kontekscie bmdOidXpath
@note - funkcja musi zostac uzyta na strukturze utworzonej za pomoca bmdMtdD2XMLGetDownloadSettingsFromMDTML() lub bmdInitBmdOidXpathStruct4XML()
@param bmdOidXpath - wskaznik na zainicjalizowana wczesniej strukture
@param DowloadDir - sciezka folderu docelowego dla pobranych plikow metadanych
@param errors_ptr - nie uzywany jeszcze stos bledow
*/
long bmdMtdDSetDowloadDir(	bmd_oid_xpath_t * bmdOidXpath,
					char *DowloadDir,
					char ***errors_ptr)
{
	/*char ** errors = NULL;*/
#if 1
	if (bmdOidXpath == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if (DowloadDir == NULL || strlen(DowloadDir) < 1)
	{
		return BMD_ERR_PARAM2;
	}
#endif

	if(bmdOidXpath->out_path)
	{
		free(bmdOidXpath->out_path);
	}
	bmdOidXpath->out_path = STRDUPL(DowloadDir);

	return BMD_OK;
}

long ValidateCellSep(	char* znak)
{
	if (znak == NULL)
	{
		return BMD_ERR_PARAM1;
	}

	if (*znak != ';' && *znak != ',' && *znak != 9 && *znak != ' ' )
	{
		* znak = ';';
		return BMD_ERR_FORMAT;
	}
	return BMD_OK;
}

long ValidateTextSep(char* znak)
{
	if (znak == NULL)
	{
		return BMD_ERR_PARAM1;
	}

	if ( *znak != '\'' && *znak != '\"' )
	{
		* znak = '\"';
		return BMD_ERR_FORMAT;
	}
	return BMD_OK;
}

char *CSVEscape(bmd_oid_xpath_t * ctx, char * value, char ValType)
{
	long len = 0;
	long charLen = 0;
	char *znak_we = NULL;
	char *znak_wy = NULL;
	char * retval = NULL;

	if (ctx == NULL || value == NULL)
	{
		return NULL;
	}

	znak_we = (char *)value;	//zliczenie znakow w zrodlowym (iter) i docelowym (len) stringu
	if(ctx->cd!=NULL)
	{
		charLen = 1;
	}	

	while (*znak_we != '\0')
	{
		if(ctx->cd==NULL && ValidateFirstUtf8Character(znak_we, &charLen) != 0)
		{
			return NULL;
		}

		if(charLen == 1)
		{
			if (*znak_we == ctx->text_separator_char)
			{
				len+=2;
			}
			else
			{
				if (*znak_we < 32 || *znak_we == 127)
				{
					if (*znak_we == 10 || *znak_we == 13 || *znak_we == 9 ) //jezeli koniec linii lub TAB
					{
						len++;					// spacja zamiast
					}
				}
				else
				{
					len++;						//kopia oryginalnego znaku
				}
			}

		}
		else //charLen > 1
		{
			len+=charLen;
		}

		znak_we+=charLen;
	}

	retval = calloc(len+1, sizeof(char));		//allokacja pamieci na nowy string
	if(retval == NULL)
	{
		return NULL;
	}

	znak_wy = (char *)retval;
	znak_we = (char *)value;

	while (*znak_we != '\0')
	{
		if(ctx->cd==NULL && ValidateFirstUtf8Character(znak_we, &charLen) != 0)
		{
			free(retval);
			return NULL;
		}

		if(charLen == 1)
		{
			if (*znak_we == ctx->text_separator_char)
			{
				*(znak_wy++) = ctx->escape_char;
				*(znak_wy++) = ctx->text_separator_char;
			}
			else
			{
				if (*znak_we < 32 || *znak_we == 127)
				{
					if (*znak_we == 10 || *znak_we == 13 || *znak_we == 9 ) //jezeli koniec linii lub TAB
					{
						*(znak_wy++) = 32;						// spacja zamiast
					}
				}
				else
				{
					if (ValType == 'n' && *znak_we == ctx->src_dec_separator)
					{
						*(znak_wy++) = ctx->dst_dec_separator;
					}
					else
					{
						*(znak_wy++) = *znak_we;					//kopia oryginalnego znaku
					}
				}
			}
		}
		else //charLen > 1
		{
			memcpy(znak_wy, znak_we, charLen);
			znak_wy+=charLen;
		}

		znak_we+=charLen;
	}

	znak_we = NULL;
	znak_wy = NULL;
	return retval;
}

char *XMLEscape(bmd_oid_xpath_t * ctx, char * value, char ValType)
{
	long len = 0;
	long charLen = 0;
	char *znak_we = NULL;
	char *znak_wy = NULL;
	char * retval = NULL;

	if (ctx == NULL || value == NULL)
	{
		return NULL;
	}

	znak_we = (char *) value;	//zliczenie znakow w zrodlowym (iter) i docelowym (len) stringu
	while (*znak_we != '\0')
	{
		//jesli bledny znak
		if(ValidateFirstUtf8Character(znak_we, &charLen) != 0)
		{
			return NULL;
		}

		if(charLen == 1)
		{
			switch(*znak_we)
			{
				case 38: //& (&amp;)
				{
					len+=5;
					break;
				}
				case 34: //" (&quot;)
				{
					len+=6;
					break;
				}
				case 39: //' (&apos;)
				{
					len+=6;
					break;
				}
				case 60: //< (&lt;)
				{
					len+=4;
					break;
				}
				case 62: //> (&gt;)
				{
					len+=4;
					break;
				}
				default:
				{
					if (*znak_we < 32 || *znak_we == 127)
					{
						if (*znak_we == 10 || *znak_we == 13 || *znak_we == 9 ) //jezeli koniec linii lub TAB
						{
							len++;					// spacja zamiast
						}
					}
					else
					{
						len++;						//kopia oryginalnego znaku
					}
				}
			} //end switch
		}
		else //charLen > 1
		{
			len+=charLen;
		}

		znak_we+=charLen;
	}

	retval = calloc(len+1, sizeof(char));		//allokacja pamieci na nowy string
	if(retval == NULL)
	{
		return NULL;
	}

	znak_wy = (char*) retval;
	znak_we = (char*) value;

	while (*znak_we != '\0')
	{
		if(ValidateFirstUtf8Character(znak_we, &charLen) != 0)
		{
			free(retval);
			return NULL;
		}

		if(charLen == 1)
		{
			switch(*znak_we)
			{
				case 38: //& (&amp;)
				{
					memcpy(znak_wy, "&amp;", 5);
					znak_wy+=5;
					break;
				}
				case 34: //" (&quot;)
				{
					memcpy(znak_wy, "&quot;", 6);
					znak_wy+=6;
					break;
				}
				case 39: //' (&apos;)
				{
					memcpy(znak_wy, "&apos;", 6);
					znak_wy+=6;
					break;
				}
				case 60: //< (&lt;)
				{
					memcpy(znak_wy, "&lt;", 4);
					znak_wy+=4;
					break;
				}
				case 62: //> (&gt;)
				{
					memcpy(znak_wy, "&gt;", 4);
					znak_wy+=4;
					break;
				}
				default:
				{
					if (*znak_we < 32 || *znak_we == 127)
					{
						if (*znak_we == 10 || *znak_we == 13 || *znak_we == 9 ) //jezeli koniec linii lub TAB
						{
							*znak_wy = 32;						// spacja zamiast
							znak_wy++;
						}
					}
					else
					{
						//if (ValType == 'n' && *znak_we == ctx->src_dec_separator)
						//{
						//	*(znak_wy++) = ctx->dst_dec_separator;
						//}
						//else
						{
							*znak_wy = *znak_we;					//kopia oryginalnego znaku
							znak_wy++;
						}
					}
				}
			} //end switch
		}
		else //charLen > 1
		{
			memcpy(znak_wy, znak_we, charLen);
			znak_wy+=charLen;
		}

		znak_we+=charLen;
	}

	znak_we = NULL;
	znak_wy = NULL;
	return retval;
}

char *NumConvert(bmd_oid_xpath_t * ctx, char * value, char ValType)
{
	char *znak_we;

	if (ctx == NULL || value == NULL)
	{
		return NULL;
	}

	if (ValType != 'n')
	{
		return value;
	}
	znak_we = (char *)value;	//zliczenie znakow w zrodlowym (iter) i docelowym (len) stringu
	while (*znak_we != 0)
	{
		if (*znak_we == ctx->src_dec_separator)
		{
			*(znak_we++) = ctx->dst_dec_separator;
		}
		znak_we++;
	}
	znak_we = NULL;
	return value;
}

long change_code_page(bmd_oid_xpath_t * ctx, char ** in_out_buf)
{
char * tmp_out_ptr = NULL;
char * tmp_in_ptr = NULL;
static char tmp_in[CSV_ICONV_IN_BUF_SIZE];
static char tmp_out[CSV_ICONV_OUT_BUF_SIZE];
size_t in_len, out_len, ret_val;

	in_len = strlen(*in_out_buf);
	assert (in_len < CSV_ICONV_IN_BUF_SIZE);
	if (in_len >= CSV_ICONV_IN_BUF_SIZE)	{
		return -1;
	}
	out_len = in_len * 3;

	tmp_in_ptr = tmp_in;
	tmp_out_ptr = tmp_out;

	memset(tmp_in_ptr,0, CSV_ICONV_IN_BUF_SIZE);
	memset(tmp_out_ptr,0, CSV_ICONV_OUT_BUF_SIZE);

	strcpy(tmp_in, *in_out_buf);

	ret_val = iconv(ctx->cd, &tmp_in_ptr, &in_len, &tmp_out_ptr, &out_len);
	switch (ret_val)
	{
	case EINVAL:		PRINT_DEBUG("Bledna wartosc do konwersji.\n");		return -2;
		break;
	case EILSEQ:		PRINT_DEBUG("Wieloznakowa wartosc w wejsciu.\n");		return -3;		break;	case E2BIG:		PRINT_DEBUG("Za malo miejsca na wyjscie.\n");		return -4;		break;	};
	assert (out_len <= CSV_ICONV_OUT_BUF_SIZE);
	free(*in_out_buf);
	*in_out_buf = STRDUPL(tmp_out);

	return BMD_OK;
}

void add_error(char*** errors_table, char*error)
{
	size_t ST_i=0;
	char ** err_tbl_ptr = NULL;
	char ** tmp_tbl_ptr = NULL;
	if (errors_table == NULL)
	{
		return;
	}
	err_tbl_ptr = *errors_table;
	if (err_tbl_ptr==NULL)
	{
		err_tbl_ptr = (char**) calloc(2, sizeof(char*));
		*errors_table = err_tbl_ptr;
		err_tbl_ptr[0] = STRDUPL(error);
	}
	else
	{
		while (err_tbl_ptr[ST_i++]);
		tmp_tbl_ptr = (char**) calloc(ST_i+1, sizeof(char*));
		ST_i--;
		memmove(tmp_tbl_ptr, err_tbl_ptr, sizeof(char*)*ST_i);
		free(err_tbl_ptr);
		err_tbl_ptr = tmp_tbl_ptr;
		err_tbl_ptr[ST_i] = STRDUPL(error);
		err_tbl_ptr[ST_i+1] = NULL;
		*errors_table = err_tbl_ptr;
	}
	return;
}

