#ifndef _LIBBMDUTILS2_INCLUDED_
#define _LIBBMDUTILS2_INCLUDED_



#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdxpath/libbmdxpath.h>
#include <iconv.h>

#include <bmd/libbmdutils/libbmdutils.h>
#include <libxml/xmlschemas.h>

#define LIBBMDUTILS2_SCOPE
#if 0 //biblioteka linkowana statycznie, wiec export niepotrzebny
#ifdef WIN32
	#ifdef LIBBMDUTILS_EXPORTS
		#define LIBBMDUTILS2_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDUTILS2_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDUTILS2_SCOPE
		#endif
	#endif
#else
	#define LIBBMDUTILS2_SCOPE
#endif // ifdef WIN32
#endif // if 0


#define DTG_2_XML_SETTINGS_FILE_VERSION			"2"
#define DTG_2_XML_SETTINGS_FILE_SUBVERSION		"1"

#if defined(__cplusplus)
	extern "C" {
#endif


/* struktura wewnetrzna*/
typedef struct
{
	enum EnumMetaData MtdType;
	//TODO sprawdzic czy usuniecie unsigned nie popsulo
	int MtdPos;
}	mtd_in_datagram_path;


/************************************************/
/*************** LIBBMDUTILS2 API ***************/
/************************************************/

typedef int (*ProgressCallback_function_t)(int current, int overal);

typedef struct
{
	int for_XML_CSV;							// flaga oznaczajaca dla jakiego typu eksportu przeznaczona jest struktura - 1 - XML - 2 CSV

	char **OIDTableBufs;						// tablica OIDow - wielkosc tablicy taka jak oid_cnt
	xmlNodeSetPtr *XMLNodes;					// tablica galezi XML odpowiadajacych OIDom z tablicy OIDTableBufs (poniewaz xpath dla jednego OID moze byc znaleziony w kilku mijescach, to zapamietujemy wszystkie w xmlNodeSetPtr - wielkosc tablicy taka jak oid_cnt
	mtd_in_datagram_path *DtgNodes;				// tablica sciezki polozenia metadanej w datagramie odpowiadajaca OIDom z tablicy OIDTableBufs - tablica inicjalizowana podczas pierwszego przetwarzania datagramsetu - wielkosc tablicy taka jak oid_cnt
	char ** CSVNodes;							// tablica nazw kolumn w dokumencie CSV

	bmdxpath_info_t *libbmdxpath_ctx;			// wskaznik na kontekst z biblioteki libbmdxpath
	char cell_separator_char;					// znak rozdzielajacy komorki w pliku CSV
	char text_separator_char;					// znak otaczajacy zawartosci komorek CSV
	char escape_char;							// inny znak escapujacy
	GenBuf_t CSV_header;						// naglowek CSV
	int accvwqm;								// Always Close Cell Value With Quoatation Mark (flaga okreslajaca czy takze liczby i daty maja byc w CSV otaczane uszami)
	char * ColValType;							// tablica typow wartosci kolumn (text, num, date)
	char * ColCcvwqm;							// tablica znacznikow dla kolumn Close Cell Value With Quoatation Mark 
	char src_dec_separator;						// zrodlowy separator ulamkow dziesietnych (jezeli ma byc zmieniany)
	char dst_dec_separator;						// docelowy separator ulamkow dziesietnych (jezeli ma byc zmieniany)
	iconv_t cd;									/* conversion descriptor */ 

	int oid_cnt;								// licznik xpathow
	long datagram_cnt;							// licznik przetworzonych datagramow - numer datagramu umieszczany w XMLu z datagramem atrybut Number tagu <datagram_metadata>
	long datagramset_cnt;						// licznik przetworzonych datagramsetow - numer datagramu umieszczany w XMLu z datagramsetem atrybut Number tagu <datagramset_metadata>

	char * out_path;							// sciezka na pliki wynikowe w przypadku zapisu do pliku/plikow
	ProgressCallback_function_t DtgSetPrgrsCallback;	//opcjonalny wskaznik na funkcje Callback wywolywana podczas obrobki datagramSetu (przed obrobka kazdego datagramu)
	ProgressCallback_function_t DtgPrgrsCallback;	//opcjonalny wskaznik na funkcje Callback wywolywana podczas obrobki datagramu (przed przetwarzaniem kazdej metadanej)
}	bmd_oid_xpath_t;							// struktura do przechowania kontekstu dla funkcji


/************************************************/
/********	LIBBMDUTILS2 easy API	 ************/
/************************************************/
	// weryfikacja  - przydatna przy wprowadzaniu ustawien (np. settings_dialog.cpp)
	LIBBMDUTILS2_SCOPE long bmdVerifyXmlDownloadSettings(xmlDocPtr doc, char ***errors_ptr);

	//inicjalizacja na podstawie pliku konfiguracyjnego
	LIBBMDUTILS2_SCOPE long bmdMtdD2XMLGetDownloadSettingsFromMDTML(bmd_oid_xpath_t ** bmdOidXpath , char *MDTMLFilePath, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdMtdD2CSVGetDownloadSettingsFromMDTML(bmd_oid_xpath_t ** bmdOidXpath , char *MDTMLFilePath, char ***errors_ptr);

	LIBBMDUTILS2_SCOPE long bmdMtdDSetDowloadDir(bmd_oid_xpath_t * bmdOidXpath , char *DowloadDir, char ***errors_ptr);

	//funkcje wlasciwe - zapisywania metadanych
	LIBBMDUTILS2_SCOPE long bmdMtdDWriteXML2Files( bmdDatagramSet_t *bmdDatagramSet, long * process_num, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdMtdDWriteXML2GenBuf( bmdDatagramSet_t *bmdDatagramSet, long * process_num, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, GenBuf_t *content, char ***errors );
	LIBBMDUTILS2_SCOPE long bmdMtdDWriteCSV2Files( bmdDatagramSet_t *bmdDatagramSet, long * process_num, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdMtdDWriteCSV2GenBuf( bmdDatagramSet_t *bmdDatagramSet, long * process_num, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, long with_csv_header, GenBuf_t *content, char ***errors );

	// funkcja porzadkowa - czyszczenie struktur
	LIBBMDUTILS2_SCOPE long bmdFreeBmdOidXpathStructElements(bmd_oid_xpath_t **bmdOidXpath, char ***errors_ptr);


/************************************************/
/******** LIBBMDUTILS2 low level API ************/
/************************************************/

	/* funkcje niskopoziomowe - zalecam uzycie funcji wyzszego API*/
	LIBBMDUTILS2_SCOPE long bmdOpenXmlDoc(xmlDocPtr *doc, char *XmlFilePath);
	LIBBMDUTILS2_SCOPE long bmdGetMtdDownloadSettings(xmlDocPtr doc, bmd_oid_xpath_t ** XMLbmdOidXpath, bmd_oid_xpath_t ** CSVbmdOidXpath, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdInitBmdOidXpathStruct4XML(xmlDocPtr doc, bmd_oid_xpath_t *bmdOidXpath, char **OidXpathsTable, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdInitBmdOidXpathStruct4CSV(bmd_oid_xpath_t *bmdOidXpath, char **OidXpathsTable, char ***errors_ptr);
	LIBBMDUTILS2_SCOPE long bmdInitMtdDtgPaths( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t *bmdOidXpath, const bmd_form_info_t *forms, char ***errors );
	LIBBMDUTILS2_SCOPE long bmdWriteDatagramMetadataInXML2GenBuf( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t *bmdOidXpath, GenBuf_t *content, char ***errors );
	LIBBMDUTILS2_SCOPE long _bmdWriteDatagramMetadataInXML2GenBuf( bmdDatagram_t *bmdDatagram, bmd_oid_xpath_t*bmdOidXpath, GenBuf_t *content, long with_xml_header, char ***errors );

	//bmdMtdDWriteXML2GenBuf
	char *CSVEscape(bmd_oid_xpath_t * ctx, char * value, char ValType);
	char *NumConvert(bmd_oid_xpath_t * ctx, char * value, char ValType);
	char *XMLEscape(bmd_oid_xpath_t * ctx, char * value, char ValType);

	/*<schemaUtils.cpp>*/
	LIBBMDUTILS2_SCOPE long bmd_validate_xml_buf(char *xmlBuf, long xmlBufSize, char *schemaBuf, long schemaBufSize);
	LIBBMDUTILS2_SCOPE long bmd_validate_xml_file(	char *xmlPath, char *schemaPath);
	/*<schemaUtils.cpp>*/

#if defined(__cplusplus)
}
#endif

#endif	/* _LIBBMDUTILS2_INCLUDED_ */
