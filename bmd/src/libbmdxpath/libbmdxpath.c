#include <libbmdxpath.h>
#include <bmd/libbmderr/libbmderr.h>

/**
 * Funkcja alokuje pamięć pod strukturę bmdxpath_info_t
 * oraz ustawia nowy kontekst XPath, zgodnie z dostarczonym
 * dokumentem XML.
 * 
 * Należy wywołać tę funkcję przed korzystaniem z innych
 * funkcji biblioteki /c libbmdxpath. Po zakończeniu
 * wszystkich operacji strukturę bmd_info_t należy zwolnić
 * za pomocą funkcji bmdxpath_destroy().
 * 
 * @param doc Dokument, dla którego ma zostać zainicjalizowana
 * struktura bmdxpath_info_t.
 * @param info Wskaźnik, pod którym zostanie zainicjalizowana
 * struktura bmdxpath_info_t.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Niepoprawne wywołanie funkcji.
 * @retval BMD_ERR_MEMORY Nie można zaalokować struktury.
 * @retval BMDXPATH_CTX_ERROR Nie można utworzyć kontekstu kryptograficznego.
 * */
long bmdxpath_init(xmlDocPtr doc, bmdxpath_info_t **info)
{
	if (doc == NULL)
	{
		PRINT_WARNING("Empty document.\n");
		return BMD_ERR_PARAM1;
	}
	if (info == NULL || (*info) != NULL)
	{
		PRINT_WARNING("Wrong info structure pointer.\n");
		return BMD_ERR_PARAM2;
	}
	
	//alokacja pamięci
	(*info) = (bmdxpath_info_t*) calloc(1, sizeof(bmdxpath_info_t));
	if ((*info) == NULL)
	{
		PRINT_FATAL("No memory!\n");
		return BMD_ERR_MEMORY;
	}
	
	//tworzenie nowego kontekstu
	(*info)->ctx = xmlXPathNewContext(doc);
	if ((*info)->ctx == NULL)
	{
		PRINT_ERROR("Cannot create XPath context.\n");
		return BMDXPATH_CTX_ERROR;
	}
	
	return 0;
}

/**
 * Zwalniane są składowe struktury za pomocą fukcji biblioteki
 * LibXML2, a następnie zwalniana jest sama struktura.
 * 
 * @param info Wskaźnik na strukturę do zwolnienia.
 * 
 * @returns Funkcja zawsze zwraca 0.
 * */
long bmdxpath_destroy(bmdxpath_info_t **info)
{
	if (info == NULL || (*info) == NULL)
	{
		return 0;
	}
	
	if ((*info)->result)
	{
		xmlXPathFreeObject((*info)->result);
		(*info)->result = NULL;
	}
	
	if ((*info)->ctx)
	{
		xmlXPathFreeContext((*info)->ctx);
		(*info)->ctx = NULL;
	}
	
	free(*info);
	(*info) = NULL;
	
	return 0;
}


/**
 * @param info Wskażnik na zainicjalizowaną strukturę bmdxpath_info_t.
 * @param prefix Prefiks danej przestrzeni nazw, przykładowo "ns".
 * @param uri URI danej przestrzeni nazw.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Niepoprawne wywołanie funkcji.
 * @retval BMDXPATH_NAMESPACE_REGISTER_ERROR Nie udało się
 * zwolnić struktury. Więcej informacji w logach.
 * */
long bmdxpath_register_ns(bmdxpath_info_t *info, char *prefix, char *uri)
{
	long status;
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (prefix == NULL)
	{
		PRINT_WARNING("No namespace prefix given.\n");
		return BMD_ERR_PARAM2;
	}
	if (uri == NULL)
	{
		PRINT_WARNING("No URI given.\n");
		return BMD_ERR_PARAM3;
	}
	
	status = xmlXPathRegisterNs(info->ctx, (xmlChar*) prefix, (xmlChar*)uri);
	
	if(status != 0)
	{
		PRINT_ERROR("Cannot register NAMESPACE with prefix '%s' and URI '%s'.\n", prefix, uri);
		return BMDXPATH_NAMESPACE_REGISTER_ERROR;
	}
	
	return 0;
}


/**
 * Do funkcji dostarczona musi zostać lista przestrzeni nazw
 * w postaci para;para;para;para, gdzie średnik może zostać
 * zastąpiony innym separatorem, a para przyjmuje postać
 * wyr1,wyr2, gdzie przecinek również może zostać zastąpiony
 * innym separatorem.
 * 
 * @param info Zainicjalizowana struktura bmd_info_t.
 * @param nslist Ciąg par przestrzeni nazw do zarejestrowania.
 * @param insep Separator wewnątrz par.
 * @param outsep Separator pomiędzy parami.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Niepoprawne wywołanie funkcji.
 * @retval BMDXPATH_NAMESPACE_EXTRACT_ERROR Nie można wyodrębnić
 * przestrzeni nazw z dostarczonego ciągu.
 * @retval BMDXPATH_NAMESPACE_REGISTER_ERROR Nie można zarejestrować
 * przestrzeni nazw.
 * */
long bmdxpath_register_ns_list(bmdxpath_info_t *info, char *nslist, char insep, char outsep)
{
	long status;
	char **tab_ns = NULL;
	long num_ns = 0;
	long i;
	
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (nslist == NULL)
	{
		PRINT_WARNING("No namespaces list given.\n");
		return BMD_ERR_PARAM2;
	}
	if (!isgraph(insep)) //separatory muszą być drukowalne
	{
		PRINT_WARNING("Given separator between prefix and uri is not printable!\n");
		return BMD_ERR_PARAM3;
	}
	if (!isgraph(outsep)) //separatory muszą być drukowalne
	{
		PRINT_WARNING("Given separator between namespaces is not printable!\n");
		return BMD_ERR_PARAM4;
	}
	
	//rozdzielenie ciagu namespace'ow na tablice namespace'ow
	status = bmd_strsep(nslist, outsep, &tab_ns, &num_ns);
	if (status != 0 || num_ns == 0)
	{
		PRINT_ERROR("Cannot extract namespaces from namespaces list.\n");
		return BMDXPATH_NAMESPACE_EXTRACT_ERROR;
	}
	
	//rozdzielenie namespace'ow na prefix i URI; rejestracja
	for (i = 0; i < num_ns; i++)
	{
		char **cpair = NULL;
		long ipair = 0;
		status = bmd_strsep(tab_ns[i], insep, &cpair, &ipair);
		if (status != 0 || ipair != 2)
		{
			PRINT_ERROR("Improper namespace in namespace list (%s).\n", tab_ns[i]);
			return BMDXPATH_NAMESPACE_EXTRACT_ERROR;
		}
		status = bmdxpath_register_ns(info, cpair[0], cpair[1]);
		if (status != 0)
		{
			PRINT_ERROR("Cannot register given namespace.\n");
			return BMDXPATH_NAMESPACE_REGISTER_ERROR;
		}
		//PRINT_DEBUG5("ns: %s\t->\t%s\n", cpair[0], cpair[1]);
		bmd_strsep_destroy(&cpair, ipair);
	}
	bmd_strsep_destroy(&tab_ns, num_ns);
	
	return 0;
}


/**
 * Jest to funkcja wykorzystywana przez inne funkcje biblioteczne
 * i nie powinna być wykorzystywana poza nimi.
 * 
 * @param info Zainicjalizowana struktura bmdxpath_info_t.
 * @param xpath Ścieżka XPath pod którą mają być wyszukiwane
 * węzły dokumentu XML.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Nieprawidłowe wywołanie funkcji.
 * @retval BMDXPATH_XPATH_EVALUATE Błąd wykonywania polecenia XPath.
 * */
long bmdxpath_find(bmdxpath_info_t *info, char *xpath)
{
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (xpath == NULL)
	{
		PRINT_WARNING("Wrong XPath given.\n");
		return BMD_ERR_PARAM2;
	}
	
	if (info->result != NULL)
	{
		xmlXPathFreeObject(info->result);
		info->result = NULL;
	}
	
	info->result = xmlXPathEvalExpression((xmlChar*)xpath, info->ctx);
	if(info->result == NULL)
	{
		PRINT_ERROR("Cannot evaluate XPath expression '%s'.\n", xpath);
		return BMDXPATH_XPATH_EVALUATE;
	}
	/* uwaga, dopiero ponizsze sprawdzenie daje informacje, czy odnalazl po xpath
	jesli nie znalazl po xpath, to ponizszy warunek true, inaczej false (nawet jesli pusty tag)*/
	if(xmlXPathNodeSetIsEmpty(info->result->nodesetval))
	{
		PRINT_WARNING("Empty set of nodes was evaluated from '%s' expression.\n", xpath);
		return BMDXPATH_XPATH_EVALUATE;
	}
	
	return 0;
}


/**
 * Funkcję można wywołać dopiero po tym, jak wyszukane
 * zostaną węzły poprzez funkcję bmdxpath_find().
 * 
 * @param 0 Sukces.
 * @param BMD_ERR_PARAMx Błędne wywołanie funkcji.
 * @param -1 Wywołano funkcję bmdxpath_count_result_nodes
 * przed wykonaniem wyszukiwania. Należy najpierw skorzystać
 * z funkcji bmdxpath_find().
 * */
long bmdxpath_count_result_nodes(bmdxpath_info_t *info)
{
	if (info == NULL)
	{
		PRINT_WARNING("Structure bmdxpath_info_t must be provided.\n");
		return BMD_ERR_PARAM1;
	}
	if (info->result == NULL)
	{
		PRINT_ERROR("You must evaluate xpath expression before counting the results.\n");
		return -1;
	}
	
	if (info->result->nodesetval)
	{
		return info->result->nodesetval->nodeNr;
	}
	
	//tutaj jestesmy, jesli nie istnieje zbior wezlow (nodeset)
	return 0;
}


/**
 * @param info Zainicjalizowana struktura bmdxpath_info_t.
 * @param xpath Ścieżka XPath do wyszukania.
 * @param nodes Wskaźnik na tablicę węzłów xmlNodePtr.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Nieprawidłowe wywołanie funkcji.
 * @retval BMDXPATH_XPATH_NO_RESULT Nie można znaleźć gałęzi
 * pod danym adresem XPath.
 * @retval BMDXPATH_XPATH_EVALUATE Błąd wyszukiwania po XPath.
 * */
long bmdxpath_get_nodes(bmdxpath_info_t *info, char *xpath, xmlNodePtr **nodes)
{
	long status = 0;
	long count = 0;
	long i;
	
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (xpath == NULL)
	{
		PRINT_WARNING("Wrong XPath given.\n");
		return BMD_ERR_PARAM2;
	}
	if (nodes == NULL || *nodes != NULL)
	{
		PRINT_WARNING("Wrong xmlNodePtr table provided.\n");
		return BMD_ERR_PARAM3;
	}
	
	//wyszukanie galezi
	status = bmdxpath_find(info, xpath);
	if (status != 0)
	{
		PRINT_ERROR("Cannot find nodes at given XPath '%s'.\n", xpath);
		return BMDXPATH_XPATH_NO_RESULT;
	}
	
	//liczymy ilosc wezlow
	count = bmdxpath_count_result_nodes(info);
	if (count < 0)
	{
		PRINT_ERROR("XPath expression not evaluated.\n");
		return BMDXPATH_XPATH_EVALUATE;
	}
	else if (count == 0)
	{
		PRINT_WARNING("Zero nodes found, returning empty table.\n");
	}
	
	//zapisanie tablicy wynikow
	(*nodes) = (xmlNodePtr*) calloc(count + 1, sizeof(xmlNodePtr));
	for (i=0; i<count; i++)
	{
		(*nodes)[i] = info->result->nodesetval->nodeTab[i];
		//info->result->nodesetval->nodeTab[i] = NULL;
	}
	
	return 0;
}


/**
 * @param nodes Wskaźnik na tablicę węzłów do zwolnienia.
 * 
 * @return Funkcja zawsze zwraca 0.
 * */
long bmdxpath_free_nodes(xmlNodePtr **nodes)
{
	long i = 0;
	
	if (nodes == NULL || *nodes == NULL)
	{
		return 0;
	}
	
	while ((*nodes)[i])
	{
		xmlFreeNode((*nodes)[i]);
		(*nodes)[i] = NULL;
		i++;
	}
	free(*nodes);
	*nodes = NULL;
	return 0;
}


/**
 * Funkcja dokonuje wyszukiwania po podanej ścieżce XPath.
 * Ze znalezionych węzłów sa wydobywane napisy i zapisywane
 * w tablicy values. Po ostatnim napisie umieszczony zostaje
 * NULL.
 * 
 * Funkcja alokuje miejsce na tablicę napisów. Następnie
 * trzeba tę tablicę zwolnić - albo ręcznie, albo funkcją
 * bmdxpath_free_strings().
 * 
 * @param info Zainicjalizowana struktura bmdxpath_info_t.
 * @param xpath Ścieżka XPath do wyszukania.
 * @param values Wskaźnik na tablicę napisów.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Błędne wywołanie funkcji.
 * @retval BMDXPATH_XPATH_NO_RESULT Nie można znaleźć nic 
 * pod podanym adresem.
 * @retval BMDXPATH_XPATH_EVALUATE Błąd wyszukiwania.
 * */
long bmdxpath_get_strings(bmdxpath_info_t *info, char *xpath, char ***values)
{
	long status = 0;
	long count = 0;
	long i;
	
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (xpath == NULL)
	{
		PRINT_WARNING("Wrong XPath given.\n");
		return BMD_ERR_PARAM2;
	}
	if (values == NULL || *values != NULL)
	{
		PRINT_WARNING("Wrong **char table provided.\n");
		return BMD_ERR_PARAM3;
	}
	
	//wyszukanie galezi
	status = bmdxpath_find(info, xpath);
	if (status != 0)
	{
		PRINT_ERROR("Cannot find nodes at given XPath '%s'.\n", xpath);
		return BMDXPATH_XPATH_NO_RESULT;
	}
	
	//liczymy ilosc wezlow
	count = bmdxpath_count_result_nodes(info);
	if (count < 0)
	{
		PRINT_ERROR("XPath expression not evaluated.\n");
		return BMDXPATH_XPATH_EVALUATE;
	}
	else if (count == 0)
	{
		PRINT_WARNING("Zero nodes found, returning empty table.\n");
	}
	
	//zapisanie tablicy wynikow
	(*values) = (char**) calloc(count + 1, sizeof(char*));
	for (i=0; i<count; i++)
	{
		if (info->result->nodesetval->nodeTab[i]->children)
		{
			(*values)[i] = strdup((char*)info->result->nodesetval->nodeTab[i]->children->content);
		}
		else
		{
			PRINT_WARNING("Empty tag.");
			(*values)[i] = strdup("");
		}
	}
	
	return 0;
}


/**
 * @param values Tablica napisów zakończona wartością NULL
 * do zwolnienia.
 * 
 * @return Funkcja zawsze zwraca 0.
 * */
long bmdxpath_free_strings(char ***values)
{
	long i = 0;
	
	if (values == NULL || *values == NULL)
	{
		return 0;
	}
	
	while ((*values)[i])
	{
		free((*values)[i]);
		(*values)[i] = NULL;
		i++;
	}
	free(*values);
	*values = NULL;
	return 0;
}

/** Funkcja przetwarzajaca tablice tringow w jeden string
@param dstStr docelowy wskaznik na string
@param len - dlugosc uzyskanego stringa
@param stringsTable Tablica napisów
@param tableSize - ilosc wierszy w tablicy
@return Funkcja zawsze zwraca 0 w przypadku powodzenia
*/
long bmdxpath_strings2string_freetable(char** dstStr, size_t * length, char ***stringsTable, size_t tableSize)
{
	size_t i = 0;
	size_t len = 0;	
	size_t *len_table = NULL;
	if (dstStr == NULL || *dstStr != NULL)
	{
		return BMD_ERR_PARAM1;
	}

	if (stringsTable == NULL || *stringsTable == NULL) // || (*stringsTable)[0] == 0)
	{
		return BMD_ERR_PARAM3;
	}
	if (tableSize == 0)
	{
		return BMD_OK;
	}
	len_table = calloc(tableSize, sizeof(size_t));

	for (i=0; i < tableSize; i++)
	{
		if ((*stringsTable)[i])
		{
			len_table[i] = strlen((*stringsTable)[i]);
			len += len_table[i];
		}
	}
	if (length)
	{
		*length = len;
	}
	len ++;
	*dstStr = calloc(len, sizeof(char));
	
	len=0;
	for (i=0; i < tableSize; i++)
	{
		strncat(*dstStr + len, (*stringsTable)[i], len_table[i]);
		len += len_table[i];
		free((*stringsTable)[i]);
		(*stringsTable)[i] = NULL;
	}

	free(*stringsTable);
	free(len_table);
	len_table = NULL;
	*stringsTable = NULL;
	
	return 0;
}



/**
 * Funkcja zwraca tylko jeden napis - wynik wyszukiwania
 * węzłów pod daną ścieżką XPath. W przypadku gdy wartości
 * znalezione zostanie więcej, zwraca pierwszą wartość.
 *
 * @param info Zainicjalizowana struktura bmdxpath_info_t.
 * @param xpath Ścieżka XPath do wyszukania.
 * @param value Wskaźnik na napis.
 * 
 * @retval 0 Sukces.
 * @retval BMD_ERR_PARAMx Nieprawidłowe wywołanie funkcji.
 * @retval BMDXPATH_XPATH_NO_RESULT Brak wyników wyszukiwania.
 * @retval BMDXPATH_XPATH_EVALUATE Bład wyszukiwania.
 * */
long bmdxpath_get_string(bmdxpath_info_t *info, char *xpath, char **value)
{
	long status = 0;
	long count = 0;
	
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (xpath == NULL)
	{
		PRINT_WARNING("Wrong XPath given.\n");
		return BMD_ERR_PARAM2;
	}
	if (value == NULL)
	{
		PRINT_WARNING("Wrong *char pointer provided.\n");
		return BMD_ERR_PARAM3;
	}
	
	//wyszukanie galezi
	status = bmdxpath_find(info, xpath);
	if (status != 0)
	{
		PRINT_ERROR("Cannot find nodes at given XPath '%s'.\n", xpath);
		return BMDXPATH_XPATH_NO_RESULT;
	}
	
	//liczymy ilosc wezlow
	count = bmdxpath_count_result_nodes(info);
	if (count < 0)
	{
		PRINT_ERROR("XPath expression not evaluated.\n");
		return BMDXPATH_XPATH_EVALUATE;
	}
	else if (count == 0)
	{
		PRINT_WARNING("Zero nodes found, NULL will remain as value.\n");
		return 0;
	}
	
	//zapisanie wyniku
	if (info->result->nodesetval->nodeTab[0]->children)
	{
		(*value) = strdup((char*)info->result->nodesetval->nodeTab[0]->children->content);
	}
	else
	{
		PRINT_WARNING("Empty tag.");
		(*value) = strdup("");
	}
	
	return 0;
}

long vasprintf_port (char **result, const char *format, va_list args);
long bmdxpath_sprintf(bmdxpath_info_t *info, char *xpath, char *fmt, ...)
{
	long status = 0;
	xmlNodePtr *nodes = NULL;
	long i = 0;
	char *content = NULL;
	va_list list;
	
	if (info == NULL)
	{
		PRINT_WARNING("No XPath context given.\n");
		return BMD_ERR_PARAM1;
	}
	if (xpath == NULL)
	{
		PRINT_WARNING("Wrong XPath given.\n");
		return BMD_ERR_PARAM2;
	}
	if (fmt == NULL)
	{
		PRINT_WARNING("Wrong format provided.\n");
		return BMD_ERR_PARAM3;
	}
	
	status = bmdxpath_get_nodes(info, xpath, &nodes);
	if (status != 0)
	{
		PRINT_ERROR("Cannot find given nodes to insert data.\n");
		return BMDXPATH_XPATH_GET_NODES;
	}
	
	va_start(list, fmt);
#ifdef WIN32
	vasprintf_port(&content, fmt, list);
#else
	vasprintf(&content, fmt, list);
#endif
	va_end(list);
	while (nodes[i] != NULL)
	{
		xmlNodeSetContent(nodes[i], (xmlChar*) content);
		i++;
	}
	
	//bmdxpath_free_nodes(&nodes);
	free(content); content=NULL;
	return i;
}

/**
 * Funkcja wykorzystuje xmlDocDumpFormatMemoryEnc do zapisania
 * dokumentu w buforze generycznym. Jest wygodniejsza do 
 * zastosowań BMD, ale nie jest niezbędna.
 * @param doc Dokument do zapisania.
 * @param bufor Bufor generyczny w którym dokonany zostanie
 * zapis. Bufor zostanie zaalokowany.
 * */
long bmdxpath_doc2genbuf(xmlDocPtr doc, GenBuf_t **bufor)
{
int temp		= 0;

	if (doc == NULL)
	{
		PRINT_DEBUG("Wrong argument 1: xmlDocPtr!\n");
		return BMD_ERR_PARAM1;
	}
	if (bufor == NULL || *bufor != NULL)
	{
		PRINT_DEBUG("Wrong argument 2: GenBuf_t!\n");
		return BMD_ERR_PARAM2;
	}
	
	*bufor = (GenBuf_t*) malloc (sizeof(GenBuf_t));
	if (*bufor == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}

	/*zapisujemy do genbufa zawartosc dokumentu*/
	temp = (*bufor)->size;
	xmlDocDumpFormatMemoryEnc(doc, (unsigned char **)&((*bufor)->buf), &temp, "UTF-8", 1);
	(*bufor)->size = temp;

	if ((*bufor)->buf == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}
	
	return 0;
}

/**
 * Funkcja wykorzystuje xmlDocDumpFormatMemory do zapisania
 * dokumentu w buforze. Jest wygodniejsza do zastosowań
 * BMD, ale nie jest niezbędna.
 * @param doc Dokument który chcemy zapisać.
 * @param bufor Tutaj zapisany zostanie dokument.
 * @param len Tutaj zapisana zostanie długośc zapisanego
 * dokumentu. Jeśli NULL, długość nie zostanie zapisana.
 * */
long bmdxpath_doc2buffer(xmlDocPtr doc, char **bufor, long *len)
{
	long tmp		= 0;
	int temp		= 0;
	
	if (doc == NULL)
	{
		PRINT_DEBUG("Wrong argument 1: xmlDocPtr!\n");
		return BMD_ERR_PARAM1;
	}
	if (bufor == NULL || *bufor != NULL)
	{
		PRINT_DEBUG("Wrong argument 2: char should not be allocated!\n");
		return BMD_ERR_PARAM2;
	}
	
	/*zapisujemy zawartosc dokumentu*/
	if (len == NULL)/*bez zapisu dlugosci*/
	{
		temp = tmp;
		xmlDocDumpFormatMemoryEnc(doc, (xmlChar**) bufor, &temp, "UTF-8", 1);
		tmp = temp;
	}
	else
	{
		temp = *len;
		xmlDocDumpFormatMemoryEnc(doc, (xmlChar**) bufor, &temp, "UTF-8", 1);
		*len = temp;
	}
	if (*bufor == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}
	
	return 0;
}
