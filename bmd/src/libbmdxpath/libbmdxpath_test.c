#include "libbmdxpath_test.h"

int main()
{
	int status=777;
	_GLOBAL_debug_level = 7;
	
	/*Do wyboru jedna funkcja, reszte zakomentowac.*/
	
// 	status = bmdxpath_init_test();
// 	status = bmdxpath_destroy_test();
// 	status = bmdxpath_register_ns_test("xmlns", "http://mydoc.example.org/schemas/");
// 	status = bmdxpath_register_ns_list_test("a,http://www.abc.com;b,http://www.bcd.com", ',', ';');
// 	status = bmdxpath_find_test();
// 	status = bmdxpath_count_result_nodes_test();
// 	status = bmdxpath_get_nodes_test();
//  	status = bmdxpath_get_strings_test();
//	status = bmdxpath_get_string_test();
// 	status = bmdxpath_sprintf_test();
	status =  bmdxpath_doc2buffer_test();
	status =  bmdxpath_doc2genbuf_test();
	
	printf("Status: %i.\n", status);
	return 0;
}


int bmdxpath_init_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_init_test()\n");
	
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return status;
	}
	printf("Zainicjalizowano kontekst z dokumentem '%s'.\n", info[0].ctx[0].doc[0].URL);
	
	xmlFreeDoc(doc);
	
	return status;
}


int bmdxpath_destroy_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_destroy_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	xmlFreeDoc(doc);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return status;
	}
	
	return 0;
}


int bmdxpath_register_ns_test(char *prefix, char *uri)
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_register_ns_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	xmlFreeDoc(doc);
	
	status = bmdxpath_register_ns(info, prefix, uri);
	if (status != 0)
	{
		printf("Blad rejestracji przestrzeni nazw %s:%s.\n", prefix, uri);
		return status;
	}
	
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	return 0;
}


int bmdxpath_register_ns_list_test(char *nslist, char insep, char outsep)
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_register_ns_list_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	xmlFreeDoc(doc);
	
	status = bmdxpath_register_ns_list(info, nslist, insep, outsep);
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw '%s'.\n", nslist);
		return status;
	}
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	return 0;
}


int bmdxpath_find_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_find_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_find(info, "/f:pismo/f:dane/f:tresc/f:tekst");
	if (status != 0)
	{
		printf("Blad wyszukiwania '/f:pismo/f:dane/f:tresc/f:tekst'.\n");
		return status;
	}
	
	status = bmdxpath_find(info, "/f:pismo/f:dane");
	if (status != 0)
	{
		printf("Blad wyszukiwania '/f:pismo/f:dane'.\n");
		return status;
	}
	
	status = bmdxpath_find(info, "/f:pismo/f:dane/");
	if (status == 0)
	{
		printf("Tutaj byl blad, a nie zostal wykryty!\n");
		return status;
	}
	printf("Umyslnie popelniony blad zostal wykryty :D.\n");
	
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}


int bmdxpath_count_result_nodes_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_count_result_nodes_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_find(info, "/f:pismo/f:dane/f:tresc/f:tekst");
	if (status != 0)
	{
		printf("Blad wyszukiwania '/f:pismo/f:dane/f:tresc/f:tekst'.\n");
		return status;
	}
	
	status = bmdxpath_count_result_nodes(info);
	printf("Wynik dzialania funkcji: %i.\n", status);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	xmlFreeDoc(doc);
	return status>=0?0:status;
}


int bmdxpath_get_nodes_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	int i = 0;
	xmlNodePtr *nodes = NULL;
	
	printf("bmdxpath_get_nodes_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_get_nodes(info, "/f:pismo/f:dane/f:tresc/f:tekst", &nodes);
	while (nodes[i] != NULL)
	{
		printf("Wezel %i: %s.\n", i, nodes[i]->name);
		xmlFreeNode(nodes[i]);
		i++;
	}
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}


int bmdxpath_free_nodes_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	int i = 0;
	xmlNodePtr *nodes = NULL;
	
	printf("bmdxpath_free_nodes_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_get_nodes(info, "/f:pismo/f:dane/f:tresc/f:tekst", &nodes);
	while (nodes[i] != NULL)
	{
		printf("Wezel %i: %s.\n", i, nodes[i]->name);
		i++;
	}
	
	bmdxpath_free_nodes(&nodes);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}


int bmdxpath_get_strings_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	int i = 0;
	char **values = NULL;
	
	printf("bmdxpath_get_strings_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_get_strings(info, "/f:pismo/f:dane/f:tresc/f:tekst", &values);
	while (values[i] != NULL)
	{
		printf("Wezel %i: %s.\n", i, values[i]);
		i++;
	}
	
	bmdxpath_free_strings(&values);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}


int bmdxpath_get_string_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	char *value = NULL;
	
	printf("bmdxpath_get_string_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns_list(info, "ds,http://www.w3.org/2000/09/xmldsig#;meta,http://crd.gov.pl/xml/schematy/epuap/meta/2008/01/07/;str,http://crd.gov.pl/xml/schematy/epuap/struktura/2008/01/07/;adr,http://crd.gov.pl/xml/schematy/epuap/adres/2008/01/07/;inst,http://crd.gov.pl/xml/schematy/epuap/instytucja/2008/01/07/;oso,http://crd.gov.pl/xml/schematy/epuap/osoba/2008/01/07/;f,http://www.unizeto.pl/szablon/formularz/1;ndap,http://www.mswia.gov.pl/standardy/ndap;xsi,http://www.w3.org/2001/XMLSchema-instance", ',', ';');
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_get_string(info, "//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:imie", &value);
	printf("Wartosc wezla: %s.\n", value);
	
	free(value); value = NULL;
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}

int bmdxpath_sprintf_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_sprintf_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	status = bmdxpath_register_ns(info, "f", "http://www.unizeto.pl/szablon/formularz/1");
	if (status != 0)
	{
		printf("Blad rejestracji listy przestrzeni nazw.\n");
		return -1;
	}
	
	status = bmdxpath_sprintf(info, "/f:pismo/f:dane/f:tresc/f:insert", "%s %i", "dziesiec", 10);
	if (status != 0)
	{
		printf("Blad aktualizacji wezla XML.\n");
		return status;
	}
	int tmp=0;
	char *bufor=NULL;
	xmlDocDumpFormatMemory(doc, (xmlChar**) &bufor, &tmp, 1);
	printf("%s\n", bufor);
	free(bufor);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	xmlFreeDoc(doc);
	return 0;
}


int bmdxpath_doc2genbuf_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_doc2genbuf_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	GenBuf_t *bufor = NULL;
	status = bmdxpath_doc2genbuf(doc, &bufor);
	if (status != 0)
	{
		printf("Blad zrzucania dokumentu do genbufa.\n");
		return status;
	}
	printf("ZAWARTOSC (%i znakow):\n|%s|\n", bufor->size, bufor->buf);
	
	xmlFreeDoc(doc);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	return 0;
}

int bmdxpath_doc2buffer_test()
{
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;
	int status = 0;
	
	printf("bmdxpath_doc2buffer_test()\n");
	doc = xmlParseFile("test.xml");
	if (doc == NULL)
	{
		printf("Nie mozna przeparsowac dokumentu.\n");
		status = -1;
	}
	
	status = bmdxpath_init(doc, &info);
	if (status != 0)
	{
		printf("Blad inicjalizacji struktury bmdxpath_info_t.\n");
		return -1;
	}
	
	char *bufor = NULL;
	status = bmdxpath_doc2buffer(doc, &bufor, NULL);
	if (status != 0)
	{
		printf("Blad zrzucania dokumentu do genbufa.\n");
		return status;
	}
	printf("ZAWARTOSC:\n|%s|\n", bufor);
	
	xmlFreeDoc(doc);
	
	status = bmdxpath_destroy(&info);
	if (status != 0)
	{
		printf("Nie mozna zniszczyc struktury bmdxpath_info_t.\n");
		return status;
	}
	
	return 0;
}
