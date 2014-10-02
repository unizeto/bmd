#ifndef _LIBBMDCRL_INCLUDED_
#define _LIBBMDCRL_INCLUDED_

#include <time.h>
#include <bmd/libbmdcms/libbmdcms.h>

#define ERR_CRL_DECODE_CRL_DATA				-300
#define ERR_CRL_VERSION_NOT_PRESENT			-301
#define ERR_CRL_CONVERT_INTEGER_T_TO_LONG		-302
#define ERR_CRL_UNRECOGNIZED_ATTR_TYPE			-303
#define ERR_CRL_EXTENSIONS_NOT_PRESENT			-304
#define ERR_CRL_UNRECOGNIZED_EXT_TYPE			-305
#define ERR_CRL_CHECK_EXT_TYPE				-306
#define ERR_CRL_UNDEFINED_EXTENSION_TYPE		-307
#define ERR_CRL_SERIAL_NOT_FOUND			-308
#define ERR_CRL_DECODE_CRL_SERIAL			-309
#define ERR_CRL_DELTAINFO_NOT_FOUND			-310
#define ERR_CRL_NEXT_UPDATE_NOT_PRESENT			-311

typedef enum CRLAttrType{CRL_ATTR_UNDEFINED,		/* nieznany atrybut */
			 CRL_ATTR_VERSION,		/* wersja listy CRL */
			 CRL_ATTR_SN,			/* numer seryjny listy CRL */
			 CRL_ATTR_DELTA,		/* "0" - pelny CRL, "numer seryjny pelnego CRLa" - delta CRL */
			 CRL_ATTR_SIGALG_OID,		/* OID algorytmu uzytego do podpisu CRLa */
			 CRL_ATTR_ISSUER_DN,		/* nazwa DN wystawcy CRLa */
			 CRL_ATTR_THIS_UPDATE,		/* czas wygenerowania obecnego CRLa */
			 CRL_ATTR_NEXT_UPDATE,		/* czas wygenerowania nastepnego CRLa */
			 CRL_ATTR_NUMBER_OF_REVOKED_CERTS /* liczba uniewaznionych certyfikatow znajdujacych sie w CRLu */
			} CRLAttrType_t;

typedef enum CRLExtType{CRL_EXT_UNDEFINED, 		/* nieznane rozszerzenie */
			CRL_EXT_SN,			/* numer seryjny CRLa */
			CRL_EXT_DELTA			/* 0 - pelny CRL, 1 - delta CRL */
			} CRLExtType_t;

#define OID_CRL_ID_CE_CRLNUMBER		2,5,29,20
#define OID_CRL_ID_CE_DELTACRLINDICATOR	2,5,29,27

#if defined(__cplusplus)
extern "C" {
#endif

/****************
 * ONE CALL API *
 ***************/
/* API, w ktorym za pomoca jednej funkcji realizowana sa wszystkie operacjie pobierania 
   parametrow znacznika czasu */

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej wybrane za pomoca CRLAttrType atrybuty CRLa.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param CRLAttrType - wybrany atrybut CRLa. Aktualnie obslugiwane sa:
 * 			CRL_ATTR_VERSION,		- wersja CRLa (long)
 *			CRL_ATTR_SN			- numer seryjny CRLa (string)
 *			CRL_ATTR_DELTA			- rodzaj CRLa "0"->pelny "numer seryjny pelnego CRLa"->delta (string)
 *			CRL_ATTR_SIGALG_OID		- OID algorytmu uzytego do podpisu CRLa (string)
 *			CRL_ATTR_ISSUER_DN		- nazwa DN wystawcy CRLa (string)
 *			CRL_ATTR_THIS_UPDATE		- czas wygenerowania obecnego CRLa (string)
 *			CRL_ATTR_NEXT_UPDATE		- czas wygenerowania nastepnego CRLa(string)
 *			CRL_ATTR_NUMBER_OF_REVOKED_CERTS- liczba odwolanych certyfikatow (long)
 * @param AttributeString - jesli pobierany atrybut jest stringiem to jest on alokowany tutaj
 * @param AttributeLong   - jesli pobierany atrybut jest longiem to jest on alokowany tutaj
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - funkcja alokuje pamiec pod string AttributeString.
 */
long CRLGetAttr(GenBuf_t *GenBuf, CRLAttrType_t CRLAttrType, char **AttributeString, long *AttributeLong);

/************
 * EASY API *
 ***********/
/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej wersje.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param version - zwracana wersja CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 */
long GetVersionFromCRL(GenBuf_t *GenBuf, long *version);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej numer seryjny.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param SerialNumber - zwracana numer seryjny CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string SerialNumber
 */
long GetSerialNumberFromCRL(GenBuf_t *GenBuf, char **SerialNumber);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej informacje czy jest to pelny CRL czy delta-CRL.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param isDelta - zwracany rodzaj CRLa ("0"-pelny, "serial number bazowego CRLa"-delta)
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string isDelta
 */
long GetDeltaStatusFromCRL(GenBuf_t *GenBuf, char **isDelta);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej OID algorytmu uzytego do podpisu.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param SignatureAlgorithmOid - OID algorytmu uzytego do podpisu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string SignatureAlgorithmOid
 */
long GetSignatureAlgorithmOIDFromCRL(GenBuf_t *GenBuf, char **SignatureAlgorithmOid);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej nazwe wyrozniona DN wystawcy CRLa.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param IssuerDN - nazwa wyrozniona wystawcy CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string IssuerDN
 */
long GetIssuerDNFromCRL(GenBuf_t *GenBuf, char **IssuerDN);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej czas wygenerowania obecnego CRLa.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param ThisUpdate - czas wygenerowania obecnego CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string ThisUpdate
 */
long GetThisUpdateFromCRL(GenBuf_t *GenBuf, char **ThisUpdate);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej czas wygenerowania kolejnego CRLa.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param NextUpdate - czas wygenerowania kolejego CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 * @note - UWAGA: funkcja alokuje pamiec na string NextUpdate
 */
long GetNextUpdateFromCRL(GenBuf_t *GenBuf, char **NextUpdate);

/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej liczbe odwolanych certyfikatow w CRLu.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param NoOfRevokedCerts - liczba odwolanych certyfikatow znajdujacych sie w CRLu
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 */
long GetNumberOfRevokedCertsFromCRL(GenBuf_t *GenBuf, long *NoOfRevokedCerts);

/************
 * FAST API *
 ***********/
/** Funkcja parsuje zakodowana w DER strukture listy CRL i wyciaga
 * z niej strukture CertificateList_t, ktora jest uzywana przez dalsze
 * funkcje parsujace.
 *
 * @param GenBuf bufor generyczny zawierajacy liste CRL zakodowana w DER
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h
 */
long GetCertificateListFromCRL(GenBuf_t *GenBuf, CertificateList_t **CertificateList);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.version wersje wygenerowanego CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param version - wersja wygenerowanego CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 */
long GetVersionFromCertificateList(CertificateList_t *CertificateList, long *version);

/** Funkcja wyciaga z pola rozszerzen CRLa jego numer seryjny.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param SerialNumber - numer seryjny wygenerowanego CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string SerialNumber
 */
long GetSerialNumberFromCertificateList(CertificateList_t *CertificateList, char **SerialNumber);

/** Funkcja wyciaga z pola rozszerzen CRLa jego typ: pelny lub delta CRL.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param isDelta - typ CRLa: "0"->pelny CRL,
			      "serial number bazowego CRLa"->delta CRL
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string isDelta
 */
long GetIsDeltaInfoFromCertificateList(CertificateList_t *CertificateList, char **isDelta);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.signature wersje wygenerowanego CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param SignatureAlgorithmOid - OID algorytmu uzytego do podpisu CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string SignatureAlgorithmOid
 */
long GetSignatureAlgorithmOIDFromCertificateList(CertificateList_t *CertificateList, char **SignatureAlgorithmOid);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.issuer nazwe DN wystawcy CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param IssuerDN - nazwa DN wystawcy CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string IssuerDN
 */
long GetIssuerDNFromCertificateList(CertificateList_t *CertificateList, char **IssuerDN);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.thisUpdate czas wygenerowania obecnego CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param ThisUpdate - czas wygenerowania obecnego CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string ThisUpdate
 */
long GetThisUpdateFromCertificateList(CertificateList_t *CertificateList, char **ThisUpdate);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.nextUpdate czas wygenerowania kolejnego CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param NextUpdate - czas wygenerowania kolejnego CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: funkcja alokuje pamiec na string NextUpdate
 */
long GetNextUpdateFromCertificateList(CertificateList_t *CertificateList, char **NextUpdate);

/** Funkcja wyciaga z pola CertificateList->tbsCertList.revokedCertificates ilosc uniewaznionych
 * certyfikatow  CRLa znajdujacych sie na CRLu.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param NoOfRevokedCerts - liczba uniewaznionych certyfikatow znajdujacych sie na CRLu
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 */
long GetNumberOfRevokedCertsFromCertificateList(CertificateList_t *CertificateList, long *NoOfRevokedCerts);

/*****************
 * LOW LEVEL API *
 ****************/
/** Funkcja wyciaga z pola CertificateList->tbsCertList.crlExtensions wzkaznik na rozszerzenia CRLa.
 *
 * @param CertificateList - struktura zawierajca dane dot listy CRL
 * @param Extensions - wskaznik na rozszerzenia CRLa
 *
 * @return 0 - prawidlowe wykonanie, 
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - UWAGA: kopiowany jest tylko adres do rozszerzen, nie zwalniac CertficateList bo
 *		wskaznik bedzie pokazywał na zwolnione miejsce w pamięci!!!
 */
long GetExtensionsFromCertificateList(CertificateList_t *CertificateList, Extensions_t **Extensions);

/** Funkcja wyciaga z pola rozszerzen rozszerzenie CRLa o wskazanym CRLExtType typie.
 *
 * @param Extensions - wskaznik na rozszerzenia CRLa
 * @param CRLExtType -  wybrany rozszerzenie CRLa. Aktualnie obslugiwane sa:
 * 			CRL_EXT_SN,		- numer seryjny CRLa (long)
 * 			CRL_EXT_DELTA,		- rodzaj CRLa (string)  "0"->pelny CRL,
									"serial CRLa bazowego"->delta CRL
 * @param ExtensionString - jesli pobierane rozszerzenie jest stringiem to jest on alokowany tutaj
 * @param ExtensionLong   - jesli pobierane rozszerzenie jest longiem to jest on alokowany tutaj
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 */
long GetCRLExtension(Extensions_t *Extensions, CRLExtType_t CRLExtType, char **ExtensionString, long *ExtensionLong);

/** Funkcja wyciaga z pola rozszerzen numer seryjny CRLa.
 *
 * @param Extensions - wskaznik na rozszerzenia CRLa
 * @param SerialNumber - numer seryjny CRLa
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - funkcja alokuje pamiec pod string SerialNumber
 */
long GetSerialNumberFromCRLExtension(Extensions_t *Extensions, char **SerialNumber);

/** Funkcja wyciaga z pola rozszerzen numer seryjny bazowego CRLa lub "0"
 * jesli jest to delta-CRL.
 *
 * @param Extensions - wskaznik na rozszerzenia CRLa
 * @param isDelta - numer seryjny bazowego CRLa lub "0" dla delta-CRL
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 * @note - funkcja alokuje pamiec pod string isDelta
 */
long GetIsDeltaInfoFromCRLExtension(Extensions_t *Extensions, char **isDelta);

/** Funkcja zamienia OID typu rozszerzenia na typ wyliczeniowy CRLExtType_t.
 *
 * @param Oid - wskaznik OID typu rozszerzenia CRLa
 * @param CRLExtType - typ rozszerzenia CRLa (patrz na CRLExtType_t w libbmdcrl.h)
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 */
long GetCrlExtType(OBJECT_IDENTIFIER_t *Oid, CRLExtType_t *CRLExtType);

/** Funkcja dekoduje serial CRLa enkapsuowany w octet_stringu i zamienia go na string
 *
 * @param octet_string - wskaznik na OCTET_STRING zawiarajacy zakodowany nr seryjny
 * @param string - serial zapisany w postaci stringu zawierajacego reprezentacje 
 * heksadecymalna
 *
 * @return 0 - prawidlowe wykonanie,
 * @return <0 blad - kody w libbmdcrl.h oraz libbmdcms.h
 */
long ExtensionSerial2string(OCTET_STRING_t *octet_string, char **string);

long GetSigValueFromCRL(GenBuf_t *crl_buf,GenBuf_t **sig_value,GenBuf_t **DER_output);

#if defined(__cplusplus)
}
#endif
#endif /*_LIBBMDCRL_INCLUDED_*/

