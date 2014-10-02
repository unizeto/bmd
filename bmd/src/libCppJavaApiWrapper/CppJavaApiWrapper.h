/** @file
Plik nag��wkowy zawieraj�cy deklaracje wszystkich funkcji wyeksportowanych
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

Je�li nie zaznaczono inaczej, ka�da z funkcji zwraca jedn� z nast�puj�cych warto�ci:
 - JAVACALL_OK - wywo�anie funkcji zako�czy�o si� powodzeniem.
 - JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem. W tym przypadku
	mo�na wywo�a� funckj� #GetCurrentErrorString, aby uzyska� opis b��du. W wi�kszo�ci
	przypadk�w mo�na te� wywo�a� #GetDetailedErrorMsg, aby uzyska� bardziej szczeg�owy
	opis b��du.

Funkcje zwracaj�ce bufory bajt�w, jak np. funkcje podpisuj�ce, dzia�aj� na zasadzie
podw�jnego wywo�ania: w pierwszym wywo�aniu jako wska�nik na bufor wyj�ciowy podaje si�
NULL, a biblioteka zwr�ci wielko�� obszaru, jaki nale�y zaalokowa� na wynik operacji.
Nast�pnie u�ytkownik powinien zaalokowa� obszar o tej wielko�ci, i wywo�a� funkcj� raz
jeszcze, tym razem przekazuj�c do niej wska�nik na zaalokowany obszar. Przyk�adowo:

\code
unsigned sigSize;

//w pierwszym wywo�aniu jako wska�nik przekazywany jest NULL
//przez parametr wyj�ciowy zostaje zwr�cona wielko�� bufora wynikowego
uniXAdESSignBinaryEnveloping(msg, msglen, certData, certLen, NULL, &sigSize);

//u�ytkownik alokuje bufor o odpowiedniej wielko�ci
unsigned char* signature = new unsigned char[sigSize];

//w drugim wywo�aniu rezultat dzia�ania funkcji zostaje skopiowany do bufora wyj�ciowego.
uniXAdESSignBinaryEnveloping(msg, msglen, certData, certLen, signature, &sigSize));

\endcode

@{ */

/** Kod zwracany przez funkcje, kt�rej wykonanie zako�czy�o si� powodzeniem. */
static const int JAVACALL_OK = 0;
/** Kod zwracany w przypadku b��du inicjalizacji JVM. */
static const int JAVACALL_INIT_ERROR = 1;
/** Kod zwracany przez funkcje, kt�rej wykonanie zako�czy�o si� b��dem. */
static const int JAVACALL_GENERAL_ERROR = 2;

/** Informacje o podpisie PKCS#7 */
typedef struct {

	unsigned char * signature;
	unsigned int signatureLen;

	unsigned char * hash;
	unsigned int hashLen;

	char * hashAlg;
} PKCS7Info;

/** Zwraca wska�nik na ci�g znak�w zawieraj�cy skr�cony opis ostatniego b��du,
jaki wyst�pi� w bibliotece. Je�li istnieje potrzeba pobrania komunikatu, to nale�y
zrobi� to zaraz po wywo�aniu funkcji z biblioteki, poniewa� wywo�anie ka�dej kolejnej
funkcji mo�e ustawi� jego now� warto��.
Zwr�conych wska�nik�w nie nale�y przechowywa�, a pami�ci przez nie wskazywanej
nie nale�y modyfikowa� ani zwalnia�. Po wywo�aniu ka�dej funkcji z biblioteki uzyskany
do tej pory wska�nik oraz komunikat staj� si� niewa�ne.
*/
JAVACALL_DLL_API const char* GetCurrentErrorString();

/** Zwraca wska�nik na ci�g znak�w zawieraj�cy dok�adniejszy opis ostatniego b��du,
jaki wyst�pi� w bibliotece. Komunikat moze zawiera� dodatkowe informacje, jak np.
stacktrace wyj�tku java, je�li takowy wyst�pi�.

Uwagi co do zwr�conego wska�nika: patrz #GetCurrentErrorString.
*/
JAVACALL_DLL_API const char* GetDetailedErrorMsg();

/**
Inicjalizuje JVM, kt�ra b�dzie obs�ugiwa�a wywo�ania funkcji proCertumJavaApi.
Powinna by� to pierwsza funkcja wywo�ana podczas pracy z bibliotek�.

@param libdir - �cie�ka do katalogu, w kt�rym zawarte s� wszystkie pliki JAR
stanowi�ce zale�no�ci proCertumJavaApi oraz CppJavaApiWrappera. Mo�e by� NULL -
w takim przypadku przeszukiwany jest katalog <code>.\\lib</code>.

@param jrepath - �cie�ka do katalogu JRE, kt�re ma zosta� u�yte.

@param params - dwukolumnowa tabela zawieraj�ca dodatkowe parametry uruchomieniowe maszyny
				wirtualnej. W kolumnie pierwszej powinny znajdowa� si� nazwy
				parametr�w (ze znakiem '=', je�li jest konieczny), a w drugiej -
				warto�ci parametr�w. Je�li dany parametr nie posiada
				warto�ci (np. jaka� flaga, jak <code>-ea</code>), w drugiej kolumnie mo�e
				znajdowa� si� pusta warto��, lub NULL. Przyk�adowo, tablica z parametrami
				uruchomieniowymi mo�e wygl�da� tak:
\code
{
	{"-Xmx", "256m"},
	{"-jni:verbatim", NULL}
}
\endcode
				Mo�e by� NULL, je�li <code>paramCount</code> jest r�wne zero.

@param paramCount - ilo�� parametr�w uruchomieniowych przekazanych do JVM podczas
					jej tworzenia. Dla powy�szego przyk�adu powinna by� r�wna 2.
*/
JAVACALL_DLL_API int proCertumJavaVmInit(const char* libdir, const char* jrepath, const char* params[][2], unsigned paramCount);

/** Pobiera ilo�� certyfikat�w w magazynie Windows.
@param[out] count - wska�nik na zmienn�, w kt�rej ma zosta� zapisana ilo��
	certyfikat�w.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int GetCertificateCount(int* count);

/** Pobiera certyfikat z magazynu Windows.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param cert - indeks certyfikatu w magazynie Windows, kt�ry ma by� pobrany.
@param[out] certData - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu
	musi by� to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie.
	W buforze tym zostan� zapisane bajty certyfikatu pobranego z magazynu Windows.
@param[out] datalen - wska�nik na zmienn�, do kt�rej zapisana zostanie wielko��
	bufora wystarczaj�ca do przechowania pobranego certyfikatu.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int GetCertificate(int cert, 
									unsigned char* certData, 
									unsigned int* datalen);

/** Wykonuje kwalifikowany, zewn�trzny podpis XAdES.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy wiadomo�� do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryDetached
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje kwalifikowany, wewn�trzny podpis XAdES.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy wiadomo�� do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryEnveloping
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany, zewn�trzny podpis XAdES.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy wiadomo�� do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniXAdESSignBinaryDetached
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany, wewn�trzny podpis XAdES.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy wiadomo�� do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniXAdESSignBinaryEnveloping
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje kwalifikowany podpis PDF.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy plik PDF do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniPDFSignQualified
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Wykonuje niekwalifikowany podpis PDF.
Funkcja dzia�a na zasadzie podw�jnego wywo�ania.

@param messageBytes - wska�nik na bufor zawieraj�cy plik PDF do podpisania.
@param messagelen - d�ugo�� wiadomo�ci do podpisania.
@param certBytes - bufor zawieraj�cy certyfikat podpisuj�cy
@param certDataLen - wielko�� certyfikatu podpisuj�cego
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty sygnatury uzyskanej po podpisaniu danych.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia sygnatury uzyskanej w wyniku wywo�ania funkcji.

@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniPDFSign
							(unsigned char const * messageBytes, unsigned int messagelen,
							 unsigned char const * certBytes, unsigned int certDataLen,
							 unsigned char * result, unsigned int* outputlen);

/** Pobiera z dokumentu PDF informacje o podpisie PKCS#7. Zwraca podpis PKCS#7 i informacje o skr�cie.
Wykonuje weryfikacj� integralno�ci podpisu.

@param signatureBytes - wska�nik na bufor zawieraj�cy plik PDF z podpisem.
@param signatureLen - d�ugo�� podpisu.
@param[out] result - struktura na informacje o podpisie.
@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniPDFGetPKCS7Info
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 PKCS7Info * result);

JAVACALL_DLL_API PKCS7Info * uniPDFAllocPKCS7Info(void);

JAVACALL_DLL_API void uniPDFFreePKCS7Info(PKCS7Info * pkcs7Info);

/** Dodaje do pliku PDF znacznik czasu.

@param signatureBytes - wska�nik na bufor zawieraj�cy plik PDF z podpisem.
@param signatureLen - d�ugo�� podpisu.
@param[out] result - w pierwszym wywo�aniu musi by� NULL. W drugim wywo�aniu musi by�
	to wska�nik na bufor o wielko�ci zwr�conej przez pierwsze wywo�anie. W tym
	buforze zostan� zapisane bajty pliku PDF z dodanym znacznikiem czasu.
@param[out] outputlen - wska�nik na zmienn�, w kt�rej zostanie zapisana wielko�c bufora
	wymagana do pomieszczenia pliku PDF z dodanym znacznikiem czasu.
@retval JAVACALL_OK - wywo�anie funkcji powiod�o si�.
@retval JAVACALL_GENERAL_ERROR - wywo�anie funkcji zako�czy�o si� b��dem.
*/
JAVACALL_DLL_API int uniPDFAddTimestamp
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char * result, unsigned int* outputlen,
							 unsigned char * addedTimestamp, unsigned int* addedTimestampLen);


/**
Waliduje integralno�� wewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zwalidowana.
@param signatureLen - ilo�� bajt�w sygnatury.

@retval JAVACALL_OK - Walidacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Sygnatura jest niepoprawna, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESValidateEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Waliduje integralno�� zewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zwalidowana.
@param signatureLen - ilo�� bajt�w sygnatury.
@param signedDataBytes - Bufor zawieraj�cy podpisan� wiadomo��.
@param signedDataLen - Wielko�� podpisanej wiadomo�ci.

@retval JAVACALL_OK - Walidacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Sygnatura jest niepoprawna, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESValidateDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);

/**
Weryfikuje poprawno�� niekwalifikowanego, wewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zweryfikowana.
@param signatureLen - ilo�� bajt�w sygnatury.

@retval JAVACALL_OK - Weryfikacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Weryfikuje poprawno�� niekwalifikowanego, zewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zweryfikowana.
@param signatureLen - ilo�� bajt�w sygnatury.
@param signedDataBytes - Bufor zawieraj�cy podpisan� wiadomo��.
@param signedDataLen - Wielko�� podpisanej wiadomo�ci.

@retval JAVACALL_OK - Weryfikacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);

/**
Weryfikuje poprawno�� kwalifikowanego, wewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zweryfikowana.
@param signatureLen - ilo�� bajt�w sygnatury.

@retval JAVACALL_OK - Weryfikacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESVerifyQualifiedEnveloping
							(unsigned char const * signatureBytes, unsigned int signatureLen);

/**
Weryfikuje poprawno�� kwalifikowanego, zewn�trznego podpisu XAdES.
@param signatureBytes - bufor zawieraj�cy bajty sygnatury, kt�ra ma zosta� zweryfikowana.
@param signatureLen - ilo�� bajt�w sygnatury.
@param signedDataBytes - Bufor zawieraj�cy podpisan� wiadomo��.
@param signedDataLen - Wielko�� podpisanej wiadomo�ci.

@retval JAVACALL_OK - Weryfikacja zako�czona pomy�lnie.
@retval JAVACALL_GENERAL_ERROR - Podpis jest niepoprawny, lub nast�pi� inny b��d.
*/
JAVACALL_DLL_API int uniXAdESVerifyQualifiedDetached
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 unsigned char const * signedDataBytes, unsigned int signedDataLen);


/** Wy�wietla modalne okno prezentuj�ce informacje o podpisie.
@param signatureBytes - bufor zawieraj�cy podpis.
@param signatureLen - wielko�� bufora z podpisem
@param owner - uchwyt okna, kt�re ma by� w�a�cicielem tego dialogu. W aplikacji
	okienkowej, je�li posiada ona jakie� widoczne okno, nie powinien mie� on
	warto�ci NULL. Mo�e by� NULL, je�li aplikacja nie posiada �adnego widocznego
	okna, lub jest to aplikacja konsolowa.
*/
#ifdef WIN32
JAVACALL_DLL_API int ShowSignatureDialog(unsigned char const * signatureBytes, unsigned int signatureLen,
										 HWND owner);
#endif										 

/** Wy�wietla niemodalne okno prezentuj�ce informacje o podpisie.
@param signatureBytes - bufor zawieraj�cy podpis.
@param signatureLen - wielko�� bufora z podpisem
*/
JAVACALL_DLL_API int ShowSignatureDialogNonModal(unsigned char const * signatureBytes, unsigned int signatureLen);

/** Ustawia nazw� sterownika PKCS#11 u�ywanego jako magazyn kluczy. Sterownik musi by� dost�pny w PATH. Mo�na te� poda� pe�n� �cie�k� do pliku sterownika.
@param driver - nazwa sterownika bez rozszerzenia np. crypto3PKCS lub cknfast. 
*/
JAVACALL_DLL_API int uniPKCS11SetDriver(char const * driver);

/** Ustawia kod PIN u�ywany podczas dost�pu do kluczy prywatnych na karcie dost�pnej przez PKCS#11
@param pin - kod PIN
*/
JAVACALL_DLL_API int uniPKCS11SetPin(char const * pin);

/** Ustawia numer slotu u�ywany przez PKCS#11. Je�li nie ustawimy numeru slotu b�d� u�ywane wszystkie. 
UWAGA: Mog� wyst�pi� problemy je�li r�ne sloty maj� r�ne kody PIN i posiadaj� takie same klucze prywatne. 
W takim przypadku wskazanie slotu jest konieczne, �eby biblioteka nie pr�bowa�a u�y� przekazanego kodu PIN do nie do tego slotu co trzeba.
@param slotId - numer slotu lub -1 je�li wszystkie sloty
*/
JAVACALL_DLL_API int uniPKCS11SetSlotId(long int slotId);

JAVACALL_DLL_API int uniTSTGetTimeStamp
							(unsigned char const * documentBytes, unsigned int documentLen,
							 unsigned char* timestampBytes, unsigned int* timestampLen);
							 
JAVACALL_DLL_API int uniTSTSetTSAAddress(char const * tsaAddress);							 
							 
							 
/** @} */ //koniec grupy CppJavaApiWrapper

//Funkcje poni�ej s� wyeksportowane na wewn�trzne potrzeby biblioteki.
//Nie powinny by� wywo�ywane przez programist� bezpo�rednio.

#ifdef WIN32
JAVACALL_DLL_API void CallbackRegisterWindow(HWND window);
JAVACALL_DLL_API void CallbackEnableParent();
#endif

#ifdef __cplusplus
} //extern "C"
#endif

#endif
