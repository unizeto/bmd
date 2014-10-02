#include <bmd/libbmdxml/libbmdxml.h>

int main(int argc, char **argv)
{
	xmlDocPtr src_xml = NULL, dest_xml = NULL;
	char *bufor = NULL;
	_GLOBAL_debug_level = 2;
	if (argc != 3)
	{
		printf("usage:\t./doc2upo src-xml dest-xml\n");
		exit(1);
	}

	/*parsowanie wejsciowych xmli*/
	src_xml = xmlParseFile(argv[1]);
	dest_xml = xmlParseFile(argv[2]);

	/*transfer tagów*/
	bmdxml_transfer_node_content(src_xml, "//dokument//nagłówek//twórca//osoba//nazwisko",dest_xml, "//upo//interesant//nazwisko");
	
	bmdxml_transfer_node_content(src_xml, "//dokument//nagłówek//twórca//osoba//imię",dest_xml, "//upo//interesant//imie");

	/*zapis do genbufa*/
	bmdxml_doc2buffer(dest_xml, &bufor, NULL);

	/*wydruk genbufa*/
	printf("%s\n", bufor);

	free(bufor);
	xmlFreeDoc(src_xml);
	xmlFreeDoc(dest_xml);
	return 0;
}
