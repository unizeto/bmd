/** @file
Plik nag³ówkowy zawieraj¹cy deklaracje wszystkich funkcji wyeksportowanych
z DLLki.
*/

#ifndef PROCERTUMJAVAAPI_H
#define PROCERTUMJAVAAPI_H


#ifdef WIN32
#include <windows.h>
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the JAVACALLDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// JAVACALLDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef JAVACALLDLL_EXPORTS
#define JAVACALL_DLL_API __declspec(dllexport)
#else
#define JAVACALL_DLL_API __declspec(dllimport)
#endif
#else
#define JAVACALL_DLL_API 
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CppJavaApiWrapper Interfejs biblioteki CppJavaApiWrapper

Interfejs biblioteki CppJavaApiWrapper.

Jeœli nie zaznaczono inaczej, ka¿da z funkcji zwraca jedn¹ z nastêpuj¹cych wartoœci:
 - JAVACALL_OK - wywo³anie funkcji zakoñczy³o siê powodzeniem.
 - JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem. W tym przypadku
	mo¿na wywo³aæ funckjê #GetCurrentErrorString, aby uzyskaæ opis b³êdu. W wiêkszoœci
	przypadków mo¿na te¿ wywo³aæ #GetDetailedErrorMsg, aby uzyskaæ bardziej szczegó³owy
	opis b³êdu.

Funkcje zwracaj¹ce bufory bajtów, jak np. funkcje podpisuj¹ce, dzia³aj¹ na zasadzie
podwójnego wywo³ania: w pierwszym wywo³aniu jako wskaŸnik na bufor wyjœciowy podaje siê
NULL, a biblioteka zwróci wielkoœæ obszaru, jaki nale¿y zaalokowaæ na wynik operacji.
Nastêpnie u¿ytkownik powinien zaalokowaæ obszar o tej wielkoœci, i wywo³aæ funkcjê raz
jeszcze, tym razem przekazuj¹c do niej wskaŸnik na zaalokowany obszar. Przyk³adowo:

\code
unsigned sigSize;

//w pierwszym wywo³aniu jako wskaŸnik przekazywany jest NULL
//przez parametr wyjœciowy zostaje zwrócona wielkoœæ bufora wynikowego
uniXAdESSignBinaryEnveloping(msg, msglen, certData, certLen, NULL, &sigSize);

//u¿ytkownik alokuje bufor o odpowiedniej wielkoœci
unsigned char* signature = new unsigned char[sigSize];

//w drugim wywo³aniu rezultat dzia³ania funkcji zostaje skopiowany do bufora wyjœciowego.
uniXAdESSignBinaryEnveloping(msg, msglen, certData, certLen, signature, &sigSize));

\endcode

@{ */

/** Kod zwracany przez funkcje, której wykonanie zakoñczy³o siê powodzeniem. */
static const int JAVACALL_OK = 0;
/** Kod zwracany w przypadku b³êdu inicjalizacji JVM. */
static const int JAVACALL_INIT_ERROR = 1;
/** Kod zwracany przez funkcje, której wykonanie zakoñczy³o siê b³êdem. */
static const int JAVACALL_GENERAL_ERROR = 2;

/** Informacje o podpisie PKCS#7 */
typedef struct {

	unsigned char * signature;
	unsigned int signatureLen;

	unsigned char * hash;
	unsigned int hashLen;

	char * hashAlg;
} PKCS7Info;

/** Zwraca wskaŸnik na ci¹g znaków zawieraj¹cy skrócony opis ostatniego b³êdu,
jaki wyst¹pi³ w bibliotece. Jeœli istnieje potrzeba pobrania komunikatu, to nale¿y
zrobiæ to zaraz po wywo³aniu funkcji z biblioteki, poniewa¿ wywo³anie ka¿dej kolejnej
funkcji mo¿e ustawiæ jego now¹ wartoœæ.
Zwróconych wskaŸników nie nale¿y przechowywaæ, a pamiêci przez nie wskazywanej
nie nale¿y modyfikowaæ ani zwalniaæ. Po wywo³aniu ka¿dej funkcji z biblioteki uzyskany
do tej pory wskaŸnik oraz komunikat staj¹ siê niewa¿ne.
*/
JAVACALL_DLL_API const char* GetCurrentErrorString();

/** Zwraca wskaŸnik na ci¹g znaków zawieraj¹cy dok³adniejszy opis ostatniego b³êdu,
jaki wyst¹pi³ w bibliotece. Komunikat moze zawieraæ dodatkowe informacje, jak np.
stacktrace wyj¹tku java, jeœli takowy wyst¹pi³.

Uwagi co do zwróconego wskaŸnika: patrz #GetCurrentErrorString.
*/
JAVACALL_DLL_API const char* GetDetailedErrorMsg();

/**
Inicjalizuje JVM, która bêdzie obs³ugiwa³a wywo³ania funkcji proCertumJavaApi.
Powinna byæ to pierwsza funkcja wywo³ana podczas pracy z bibliotek¹.

@param libdir - œcie¿ka do katalogu, w którym zawarte s¹ wszystkie pliki JAR
stanowi¹ce zale¿noœci proCertumJavaApi oraz CppJavaApiWrappera. Mo¿e byæ NULL -
w takim przypadku przeszukiwany jest katalog <code>.\\lib</code>.

@param jrepath - œcie¿ka do katalogu JRE, które ma zostaæ u¿yte.

@param params - dwukolumnowa tabela zawieraj¹ca dodatkowe parametry uruchomieniowe maszyny
				wirtualnej. W kolumnie pierwszej powinny znajdowaæ siê nazwy
				parametrów (ze znakiem '=', jeœli jest konieczny), a w drugiej -
				wartoœci parametrów. Jeœli dany parametr nie posiada
				wartoœci (np. jakaœ flaga, jak <code>-ea</code>), w drugiej kolumnie mo¿e
				znajdowaæ siê pusta wartoœæ, lub NULL. Przyk³adowo, tablica z parametrami
				uruchomieniowymi mo¿e wygl¹daæ tak:
\code
{
	{"-Xmx", "256m"},
	{"-jni:verbatim", NULL}
}
\endcode
				Mo¿e byæ NULL, jeœli <code>paramCount</code> jest równe zero.

@param paramCount - iloœæ parametrów uruchomieniowych przekazanych do JVM podczas
					jej tworzenia. Dla powy¿szego przyk³adu powinna byæ równa 2.
*/
JAVACALL_DLL_API int proCertumJavaVmInit(const char* libdir, const char* jrepath, const char* params[][2], unsigned paramCount);

/** Pobiera iloœæ certyfikatów w magazynie Windows.
@param[out] count - wskaŸnik na zmienn¹, w której ma zostaæ zapisana iloœæ
	certyfikatów.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int GetCertificateCount(int* count);

/** Pobiera certyfikat z magazynu Windows.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param cert - indeks certyfikatu w magazynie Windows, który ma byæ pobrany.
@param[out] certData - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu
	musi byæ to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie.
	W buforze tym zostan¹ zapisane bajty certyfikatu pobranego z magazynu Windows.
@param[out] datalen - wskaŸnik na zmienn¹, do której zapisana zostanie wielkoœæ
	bufora wystarczaj¹ca do przechowania pobranego certyfikatu.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int GetCertificate(int cert, 
									unsigned char* certData, 
									unsigned int* datalen);

/** Wykonuje kwalifikowany, zewnêtrzny podpis XAdES.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy wiadomoœæ do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryDetached
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje kwalifikowany, wewnêtrzny podpis XAdES.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy wiadomoœæ do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryEnveloping
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany, zewnêtrzny podpis XAdES.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy wiadomoœæ do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniXAdESSignBinaryDetached
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany, wewnêtrzny podpis XAdES.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy wiadomoœæ do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniXAdESSignBinaryEnveloping
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje kwalifikowany podpis PDF.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy plik PDF do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniPDFSignQualified
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany podpis PDF.
Funkcja dzia³a na zasadzie podwójnego wywo³ania.

@param messageBytes - wskaŸnik na bufor zawieraj¹cy plik PDF do podpisania.
@param messagelen - d³ugoœæ wiadomoœci do podpisania.
@param certBytes - bufor zawieraj¹cy certyfikat podpisuj¹cy
@param certDataLen - wielkoœæ certyfikatu podpisuj¹cego
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo³ania funkcji.

@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniPDFSign
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Pobiera z dokumentu PDF informacje o podpisie PKCS#7. Zwraca podpis PKCS#7 i informacje o skrócie.
Wykonuje weryfikacjê integralnoœci podpisu.

@param signatureBytes - wskaŸnik na bufor zawieraj¹cy plik PDF z podpisem.
@param signatureLen - d³ugoœæ podpisu.
@param[out] result - struktura na informacje o podpisie.
@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniPDFGetPKCS7Info
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 PKCS7Info * result);

JAVACALL_DLL_API PKCS7Info * uniPDFAllocPKCS7Info(void);

JAVACALL_DLL_API void uniPDFFreePKCS7Info(PKCS7Info * pkcs7Info);

/** Dodaje do pliku PDF znacznik czasu.

@param signatureBytes - wskaŸnik na bufor zawieraj¹cy plik PDF z podpisem.
@param signatureLen - d³ugoœæ podpisu.
@param[out] result - w pierwszym wywo³aniu musi byæ NULL. W drugim wywo³aniu musi byæ
	to wskaŸnik na bufor o wielkoœci zwróconej przez pierwsze wywo³anie. W tym
	buforze zostan¹ zapisane bajty pliku PDF z dodanym znacznikiem czasu.
@param[out] outputlen - wskaŸnik na zmienn¹, w której zostanie zapisana wielkoœc bufora
	wymagana do pomieszczenia pliku PDF z dodanym znacznikiem czasu.
@retval JAVACALL_OK - wywo³anie funkcji powiod³o siê.
@retval JAVACALL_GENERAL_ERROR - wywo³anie funkcji zakoñczy³o siê b³êdem.
*/
JAVACALL_DLL_API int uniPDFAddTimestamp
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char * result, unsigned int* outputlen,
							 unsigned char * addedTimestamp, unsigned int* addedTimestampLen);


/**
Waliduje integralnoœæ wewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zwalidowana.
@param signatureLen - iloœæ bajtów sygnatury.

@retval JAVACALL_OK - Walidacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Sygnatura jest niepoprawna, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESValidateEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Waliduje integralnoœæ zewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zwalidowana.
@param signatureLen - iloœæ bajtów sygnatury.
@param signedDataBytes - Bufor zawieraj¹cy podpisan¹ wiadomoœæ.
@param signedDataLen - Wielkoœæ podpisanej wiadomoœci.

@retval JAVACALL_OK - Walidacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Sygnatura jest niepoprawna, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESValidateDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);

/**
Weryfikuje poprawnoœæ niekwalifikowanego, wewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zweryfikowana.
@param signatureLen - iloœæ bajtów sygnatury.

@retval JAVACALL_OK - Weryfikacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Weryfikuje poprawnoœæ niekwalifikowanego, zewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zweryfikowana.
@param signatureLen - iloœæ bajtów sygnatury.
@param signedDataBytes - Bufor zawieraj¹cy podpisan¹ wiadomoœæ.
@param signedDataLen - Wielkoœæ podpisanej wiadomoœci.

@retval JAVACALL_OK - Weryfikacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);

/**
Weryfikuje poprawnoœæ kwalifikowanego, wewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zweryfikowana.
@param signatureLen - iloœæ bajtów sygnatury.

@retval JAVACALL_OK - Weryfikacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESVerifyQualifiedEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Weryfikuje poprawnoœæ kwalifikowanego, zewnêtrznego podpisu XAdES.
@param signatureBytes - bufor zawieraj¹cy bajty sygnatury, która ma zostaæ zweryfikowana.
@param signatureLen - iloœæ bajtów sygnatury.
@param signedDataBytes - Bufor zawieraj¹cy podpisan¹ wiadomoœæ.
@param signedDataLen - Wielkoœæ podpisanej wiadomoœci.

@retval JAVACALL_OK - Weryfikacja zakoñczona pomyœlnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast¹pi³ inny b³¹d.
*/
JAVACALL_DLL_API int uniXAdESVerifyQualifiedDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);


/** Wyœwietla modalne okno prezentuj¹ce informacje o podpisie.
@param signatureBytes - bufor zawieraj¹cy podpis.
@param signatureLen - wielkoœæ bufora z podpisem
@param owner - uchwyt okna, które ma byæ w³aœcicielem tego dialogu. W aplikacji
	okienkowej, jeœli posiada ona jakieœ widoczne okno, nie powinien mieæ on
	wartoœci NULL. Mo¿e byæ NULL, jeœli aplikacja nie posiada ¿adnego widocznego
	okna, lub jest to aplikacja konsolowa.
*/
#ifdef WIN32
JAVACALL_DLL_API int ShowSignatureDialog(unsigned char const * signatureBytes, unsigned int signatureLen,
										 HWND owner);
#endif										 

/** Wyœwietla niemodalne okno prezentuj¹ce informacje o podpisie.
@param signatureBytes - bufor zawieraj¹cy podpis.
@param signatureLen - wielkoœæ bufora z podpisem
*/
JAVACALL_DLL_API int ShowSignatureDialogNonModal(unsigned char const * signatureBytes, unsigned int signatureLen);

/** Ustawia nazwê sterownika PKCS#11 u¿ywanego jako magazyn kluczy. Sterownik musi byæ dostêpny w PATH. Mo¿na te¿ podaæ pe³n¹ œcie¿kê do pliku sterownika.
@param driver - nazwa sterownika bez rozszerzenia np. crypto3PKCS lub cknfast. 
*/
JAVACALL_DLL_API int uniPKCS11SetDriver(char const * driver);

/** Ustawia kod PIN u¿ywany podczas dostêpu do kluczy prywatnych na karcie dostêpnej przez PKCS#11
@param pin - kod PIN
*/
JAVACALL_DLL_API int uniPKCS11SetPin(char const * pin);

/** Ustawia numer slotu u¿ywany przez PKCS#11. Jeœli nie ustawimy numeru slotu bêd¹ u¿ywane wszystkie. 
UWAGA: Mog¹ wyst¹piæ problemy jeœli ró¿ne sloty maj¹ ró¿ne kody PIN i posiadaj¹ takie same klucze prywatne. 
W takim przypadku wskazanie slotu jest konieczne, ¿eby biblioteka nie próbowa³a u¿yæ przekazanego kodu PIN do nie do tego slotu co trzeba.
@param slotId - numer slotu lub -1 jeœli wszystkie sloty
*/
JAVACALL_DLL_API int uniPKCS11SetSlotId(long int slotId);

JAVACALL_DLL_API int uniTSTGetTimeStamp
							(unsigned char const * documentBytes, unsigned int documentLen,
							 unsigned char* timestampBytes, unsigned int* timestampLen);
							 
JAVACALL_DLL_API int uniTSTSetTSAAddress(char const * tsaAddress);							 
							 
							 
/** @} */ //koniec grupy CppJavaApiWrapper

//Funkcje poni¿ej s¹ wyeksportowane na wewnêtrzne potrzeby biblioteki.
//Nie powinny byæ wywo³ywane przez programistê bezpoœrednio.

#ifdef WIN32
JAVACALL_DLL_API void CallbackRegisterWindow(HWND window);
JAVACALL_DLL_API void CallbackEnableParent();
#endif

#ifdef __cplusplus
} //extern "C"
#endif

#endif
