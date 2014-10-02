#ifndef _LIBBMDTS_INCLUDED_
#define _LIBBMDTS_INCLUDED_

#include <time.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdasn1_common/TimeStampResp.h>
#include <bmd/libbmdasn1_common/TSTInfo.h>

#define LIBBMDTS_SCOPE
#if 0 //biblioteka linkwana statycznie, wiec export niepotrzebny
#ifdef WIN32
	#ifdef LIBBMDTS_EXPORTS
		#define LIBBMDTS_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDTS_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDTS_SCOPE
		#endif
	#endif
#else
	#define LIBBMDTS_SCOPE
#endif // ifdef WIN32
#endif //if 0


#define TRANSPORT_TYPE_TCP	0
#define TRANSPORT_TYPE_HTTP	1


/* ponizszy define ma miec wartosc 1 na wzor funkcji memcmp!! */
#define ERR_NO_CERTIFICATES_MATCHED_BY_THEIR_DN			1

#define OID_CMS_ID_CT_TSTINFO			1,2,840,113549,1,9,16,1,4

typedef enum TSAttrType{TS_ATTR_UNDEFINED,		/* nieznana wersja atrybutu */
			TS_ATTR_VERSION,		/* wersja znacznika czasu */
			TS_ATTR_GENTIME,		/* czas generacji znacznika */
			TS_ATTR_POLICY_OID,		/* OID polityki generacji znacznika */
			TS_ATTR_SN,			/* numer seryjny znacznika */
			TS_ATTR_TSA_DN,			/* nazwa wyrozniona wystawcy znacznika */
			TS_ATTR_TSA_CERT_SN,		/* numer seryjny certyfikatu wystawcy znacznika */
			TS_ATTR_TSA_CERT_VALID_NA,	/* data wygasniecia waznosci certyfikatu wystawcy znacznika */
			} TSAttrType_t;


#if defined(__cplusplus)
extern "C" {
#endif

/****************
 * ONE CALL API *
 ***************/
/* API, w ktorym za pomoca jednej funkcji realizowana sa wszystkie operacjie pobierania
   parametrow znacznika czasu */

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej wybrane za pomoca TSAttrType atrybuty znacznika.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TSAttrType - wybrany atrybut znacznika. Aktualnie obslugiwane sa:
 * 			TS_ATTR_VERSION,		- wersja znacznika czasu (long)
 *			TS_ATTR_GENTIME,		- czas generacji znacznika (string)
 *			TS_ATTR_POLICY_OID,		- OID polityki generacji znacznika (string)
 *			TS_ATTR_SN,			- numer seryjny znacznika (string)
 *			TS_ATTR_TSA_DN,			- nazwa wyrozniona wystawcy znacznika (string)
 *			TS_ATTR_TSA_CERT_SN,		- numer seryjny certyfikatu wystawcy znacznika (string)
 *			TS_ATTR_TSA_CERT_VALID_NA,	- data wygasniecia waznosci certyfikatu wystawcy znacznika (string)
 * @param AttributeString - jesli pobierany atrybut jest stringiem to jest on alokowany tutaj
 * @param AttributeLong   - jesli pobierany atrybut jest longiem to jest on alokowany tutaj
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 * @note - funkcja alokuje pamiec pod string AttributeString.
 */
LIBBMDTS_SCOPE long TSGetAttr(GenBuf_t *GenBuf, TSAttrType_t TSAttrType, char **AttributeString, long *AttributeLong, long twl_no_of_certificate);

/************
 * EASY API *
 ***********/
/** Funkcja dokonuje timestampowania danych podanych w buforze generycznym PlainData.
 * Odpowiedz z serwera umieszczana jest w TimestampedData.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TimestampGenTime - zwracany czas generacji znacznika
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 * @note - funkcja alokuje pamiec na TimestampedData
 */
LIBBMDTS_SCOPE long TimestampGenBuf(	GenBuf_t *PlainData,
					GenBuf_t **TimestampedData,
					TimestampSource_t TimestampSource,
					char *TsSrvAddr,
					long TsSrvPort,
					long TsSrvTimeout);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej czas generacji znacznika do postaci stringu.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TimestampGenTime - zwracany czas generacji znacznika
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetGenerationTimeFromTimestamp(GenBuf_t *GenBuf, char **TimestampGenTime);
LIBBMDTS_SCOPE long GetGenerationTimeFromTimestamp_time(	GenBuf_t *GenBuf,time_t *TimestampGenTime);
/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej wersje znacznika czasu.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param version - zwracany wersja znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetVersionFromTimestamp(GenBuf_t *GenBuf, long *version);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej polike wystawiania znacznika czasu.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param PolicyOid - zwracany OID polityki znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetPolicyOIDFromTimestamp(GenBuf_t *GenBuf, char **PolicyOid);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej numer seryjny znacznika czasu.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param SerialNumber - zwracany numer seryjny znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetSerialNumberFromTimestamp(GenBuf_t *GenBuf, char **SerialNumber);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej nazwe wyrozniona DN oraganu wystawiajacego znacznik czasu (TSA).
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TsaDn - zwracany nazwa wyrozniona wystawcy znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetTsaDnFromTimestamp(GenBuf_t *GenBuf, char **TsaDn);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z certyfikat wystawcy znacznika a z niego numer seryjny.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TsaCertSn - zwracany numer seryjny certyfikatu wystawcy znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetTsaCertificateSnFromTimestamp(GenBuf_t *GenBuf, char **TsaCertSn, long twl_no_of_certificate);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z certyfikat wystawcy znacznika a z niego czas waznosci NotAfter.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TsaCertValidityNA - zwracany data waznosci (NotAfter) certyfikatu wystawcy znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetTsaCertificateValidityNAFromTimestamp(GenBuf_t *GenBuf, char **TsaCertValidityNA, long twl_no_of_certificate);


/************
 * FAST API *
 ***********/
/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej strukture posrednia TSTInfo_t, ktora jest uzywana przez dalsze
 * funkcje parsujace.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetTSTInfoFromTimestamp(GenBuf_t *GenBuf, TSTInfo_t **TSTInfo);

/** Funkcja parsuje zakodowana w DER strukture znacznika czasu i wyciaga
 * z niej z pola SignedData z zbior certyfikatow.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param Certificate - tablica struktur zawierajca certyfikaty skojarzone ze znacznikiem czasu
 * @param NoOfCertificates - liczba pobanych certyfikatow skojarzonych ze znacznikiem czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetCertificatesFromTimestamp(GenBuf_t *GenBuf, Certificate_t ***Certificate, long *NoOfCertificates);

/** Funkcja zwalnia dynamicznie zaalokowana tablice certyfikatow CertificatesTable i NULLuje wskaznik
 * pokazujacy na nia.
 *
 * @param CertificatesTable tablica ccertyfikatow
 * @param NoOfCertificates - liczba elementow w tablicy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long FreeSetOfCertificatesFromTimestamp(Certificate_t ***CertificatesTable, long NoOfCertificates);

/** Funkcja wyciaga z pola TSTInfo->genTime czas wygenerowania znacznika czasu.
 *
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 * @param GenerationTime - czas wygenerowania znacznika
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 */
LIBBMDTS_SCOPE long GetTimeFromTSTInfo(TSTInfo_t *TSTInfo, char **GenerationTime);

/** Funkcja wyciaga z pola TSTInfo->version wersje wygenerowanego znacznika czasu.
 *
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 * @param version - wersja wygenerowanego znacznika
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 */
LIBBMDTS_SCOPE long GetVersionFromTSTInfo(TSTInfo_t *TSTInfo, long *version);

/** Funkcja wyciaga z pola TSTInfo->policy OID polityki wygenerowanego znacznika czasu.
 *
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 * @param policyOID - OID polityki znacznika w postaci stringu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note jesli ktos chcialby policyOID w postaci OBJECT_IDENTIFIER_t lub long[] to dajcie
 * mi znak
 * @note funkcja automatycznie alokuje pamiec na string policyOID
 */
LIBBMDTS_SCOPE long GetPolicyOIDFromTSTInfo(TSTInfo_t *TSTInfo, char **policyOID);

/** Funkcja wyciaga z pola TSTInfo->serialNumber numer seryjny wygenerowanego znacznika czasu.
 *
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 * @param serialNumber - numer seryjny znacznika w postaci stringu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note jesli ktos chcialby serialNumber w postaci long lub lub jakiejkolwiek innej
 * to dajcie mi znak
 * @note funkcja automatycznie alokuje pamiec na string serialNumber
 */
LIBBMDTS_SCOPE long GetSerialNumberFromTSTInfo(TSTInfo_t *TSTInfo, char **serialNumber);

/*****************
 * LOW LEVEL API *
 ****************/
/** Funkcja dekoduje zakodowana w DER strukture znacznika czasu do postaci
 * struktury w C typu TimeStampResp_t bedacej odpowiedzia serwera znacznika
 * czasu.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TimeStampResp - struktura zawierajca odpowiedz serwera znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 * @note funkcja automatycznie alokuje pamiec na TimeStampResp_t
 */
LIBBMDTS_SCOPE long GetTimeStampResp_from_GenBuf(GenBuf_t *GenBuf, TimeStampResp_t **TimeStampResp);

/** Funkcja zwraca status odpowiedzi na wystawienie znacznika czasu.
 *
 * @param TimeStampResp - struktura zawierajca odpowiedz serwera znacznika czasu
 * @param status status odpowiedzi serwera znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h
 */
LIBBMDTS_SCOPE long GetTimeStampRespStatus(TimeStampResp_t *TimeStampResp, long *status);


/** Funkcja dekoduje zakodowana w DER strukture znacznika czasu do postaci
 * struktury w C bedacej koperta CMSowa typu ContentInfo_t.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param ContentInfo - struktura zawierajca koperte CMSowa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note funkcja automatycznie alokuje pamiec na ContentInfo_t
 */
LIBBMDTS_SCOPE long GetTimeStampToken_from_GenBuf(GenBuf_t *GenBuf, ContentInfo_t **ContentInfo);

/** Funkcja sprawdza czy zawartosc koperty CMSowej jest rzeczywiscie podpisem.
 * Wykonuje to poprze porownanie OIDu pola ContentInfo->contentType z OIDem
 * typu SignedData.
 *
 * @param ContentInfo - struktura zawierajca koperte CMSowa
 *
 * @return 0 - prawidlowe wykonanie = zawartosc koperty jest podpisem,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 */
LIBBMDTS_SCOPE long CheckIfCmsIsSignature(ContentInfo_t *ContentInfo);

/** Funkcja sprawdza czy zawartosc typu SignedData jest rzeczywiscie znacznikiem
 * czasu. Wykonuje to poprze porownanie OIDu pola
 * SignedData->encapContentInfo.eContentType z OIDem typu SignedData.
 *
 * @param SignedData - struktura zawierajca podpis
 *
 * @return 0 - prawidlowe wykonanie = zawartosc koperty jest podpisem,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 */
LIBBMDTS_SCOPE long CheckIfSignatureIsTimestamp(SignedData_t *SignedData);

/** Funkcja dekoduje zakodowana w DER strukture znacznika czasu umieszczona
 * w kopercie CMS w polu eContent do postaci
 * struktury w C bedacej struktura znacznika czasu TSTInfo_t.
 *
 * @param eContent bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TSTInfo - struktura zawierajca dane dot znacznika czasu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note funkcja automatycznie alokuje pamiec na TSTInfo_t
 */
LIBBMDTS_SCOPE long GetTSTInfo_from_SignedData(OCTET_STRING_t *eContent, TSTInfo_t **TSTInfo);

/** Funkcja pobiera ze struktury SignedData_t certyfikaty umieszczajac je
 * w dynamicznie alokowanej tablicy certyfikatow CertificatesTable.
 *
 * @param SignedData struktura SignedData CMSa
 * @param CertificatesTable - tablica struktur Certyfikatow X509
 * @param NoOfCertificates - liczba certyfikatow w tablicy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note funkcja automatycznie alokuje pamiec na CertificatesTable
 */
LIBBMDTS_SCOPE long GetCertificatesFromSignedData(SignedData_t *SignedData, Certificate_t ***CertificatesTable, long *NoOfCertificates);

/** Funkcja pobiera z koperty znacznika czasu certyfikaty umieszczajac je
 * w dynamicznie alokowanej tablicy certyfikatow CertificatesTable.
 *
 * @param GenBuf bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param CertificatesTable - tablica struktur Certyfikatow X509
 * @param NoOfCertificates - liczba certyfikatow w tablicy
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note funkcja automatycznie alokuje pamiec na CertificatesTable
 */
LIBBMDTS_SCOPE long GetSetOfCertificatesFromTimestamp(GenBuf_t *GenBuf, Certificate_t ***CertificatesTable, long *NoOfCertificates);

/** Funkcja przeszukuje tablice certufikatow w poszukiwaniu tego, ktory ma DN
 * posiadacza rowny podanemu w stringu TsaDn. Jesli znaleziono taki certyfikat
 * to jego numer w tablicy jest zwracany w zmiennej NoOfMatchedCert zas sam certyfikat
 * umieszczany jest pod MatchedCertificate.
 *
 * @param TsaDn nazwa wyrozniona DN wystawcy znacznika w postaci stringu
 * w formacie zwracanym przez funkcje GeneralName2string
 * @param CertificatesTable - tablica struktur Certyfikatow X509
 * @param NoOfCertificates - liczba certyfikatow w tablicy
 * @param NoOfMatchedCert - zwracany numer certyfikatu o DN identycznym jak w TsaDn
 * @param MatchedCertificate - zwracany certyfikat o DN identycznym jak w TsaDn
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @return >0 - prawidlowe wykonanie, ale nie znaleziono certyfikatu o podanym DN.
 * dodatkowowo zmienna NoOfMatchedCert przyjmuje wartosc -1.
 * @note - funkcja alokuje pamiec pod zmienna MatchedCertificate.
 */
LIBBMDTS_SCOPE long CompareTimestampTSADNWithTSACertificateDN(	char *TsaDn, Certificate_t **CertificatesTable,
						long NoOfCertificates, long *NoOfMatchedCert,
						Certificate_t **MatchedCertificate);

/** Funkcja przeszukuje tablice certufikatow w poszukiwaniu tego, ktory ma DN
 * posiadacza rowny podanemu w stringu TsaDn. Jesli znaleziono taki certyfikat
 * to jest on umieszczany jest pod TsaCertificate.
 *
 * @param GenBuf - bufor generyczny zawierajacy znacznik czasu zakodowany w DER
 * @param TsaCertificate - zwracany certyfikat Tsa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdts.h oraz libbmdcms.h
 * @note - funkcja alokuje pamiec pod zmienna TsaCertificate.
 */
LIBBMDTS_SCOPE long GetTsaCertificateFromTimestamp(GenBuf_t *GenBuf, Certificate_t **TsaCertificate,  long twl_no_of_certificate);

LIBBMDTS_SCOPE long CreateTsRequest(GenBuf_t *hash,char *hash_algo,GenBuf_t **output);

/* Funkcje wewnetrzne - nieeksportowalne */
long _bmd_create_ts_request(GenBuf_t *hash,long hash_algo,GenBuf_t **output);


/**-------------------------------*/
/**by WSZ*/
/*uzupelnienie do funkcji Szu*/
long Get_TSResp_failInfo(TimeStampResp_t *ts_resp, long *failure_info);
long Get_GenBuf_failInfo(GenBuf_t *resp_buf, long *failure_info);
long Get_TSResp_status(TimeStampResp_t *ts_resp, long *ts_resp_status);
long Get_GenBuf_status(GenBuf_t *resp_buf, long *ts_resp_status);

/**---------*/
/** ONE CALL API (by WSZ)
 - troszke wydajniejsze, niz zaimplementowane przez Szu*/
/**---------*/
struct ts_resp_features
{
	long version;
	char *TSA_policyId;
	char *hash_alg_oid;
	OCTET_STRING_t *hash_val;
	long serial;
	time_t genTime;
	char *tsa;
	//long nonce;
};
typedef struct ts_resp_features TSRespFeatures_t;

/** funkcja Parse_TimeStampResponse() dekoduje i parsuje odpowiedz serwera znacznika czasu
jesli status odpowiedzi == odrzucony, to funkcja zwraca BMD_OK a *features = NULL
jesli status odpowiedzi == oznakowano czasem, to funkcja zwraca BMD_OK i *features != NULL
jesli jakas operacja zawiodla, to wartosc zwracana != BMD_OK i *features = NULL
*/
LIBBMDTS_SCOPE long Parse_TimeStampResponse(GenBuf_t* ts_resp, TSRespFeatures_t** features);
/** funkcja Destroy_TSRespFeatures przeznaczona jest do czyszczenia zawartosci struktury otrzymanej w wyniku parsowania odpowiedzi serwera znacznika czasu*/
LIBBMDTS_SCOPE long Destroy_TSRespFeatures(TSRespFeatures_t **tsresp_features);
/**by WSZ*/
/**-----------------------------*/



#if defined(__cplusplus)
}
#endif
#endif /*_LIBBMDTS_INCLUDED_*/

