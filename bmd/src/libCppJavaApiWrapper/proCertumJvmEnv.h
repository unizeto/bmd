/** @file
Modu� zawieraj�cy klasy wspomagaj�ce prac� z JVM w wykorzystaniu z proCertumJavaApi.
*/

#ifndef UNIZETO_JAVACALL_JAVASIGNATURE_ENV
#define UNIZETO_JAVACALL_JAVASIGNATURE_ENV

#include "JvmEnvironment.h"


#include "CppJavaApiWrapper.h"

#include <jni.h>

#include <vector>
#include <map>

namespace javacall {

/** Pomocniczy typedef dla tablicy bajt�w. */
typedef std::vector<jbyte> ByteArray;
/** Pomocniczy typedef dla tablicy tablic bajt�w. */
typedef  std::vector<ByteArray> ByteArrayCollection;

/**
Klasa wposmagaj�ca prac� z konkretn� bibliotek� java, jak� jest proCertumJavaApi.
*/
class proCertumJvmEnv : public JvmEnvironment
{
public:
	static const char * WRAPPER_CLASS;
	proCertumJvmEnv(void);
	~proCertumJvmEnv(void);

	ByteArrayCollection getCerts();
	ByteArray sign(const ByteArray& message, const ByteArray& certBytes, bool qualified, bool enveloping);
	void validate(const ByteArray& signature, const ByteArray& signedData, bool detached);
	void verify(const ByteArray& signature, const ByteArray& signedData, bool qualified, bool detached);

	ByteArray signPDF(const ByteArray& message, const ByteArray& certBytes, bool qualified);
	ByteArray addTimestampToPDF(const ByteArray& message);
	ByteArray getLastAddedTimestampToPDF();
	void getPKCS7Info(const ByteArray& signedPdf, PKCS7Info * result);
	
	void setPkcs11Driver(char const * driver);
	void setPkcs11Pin(char const * pin);
	void setPkcs11SlotId(long int slotId);

	ByteArray getTimeStamp(const ByteArray& message);
	void setTSAAddress(char const * url);

	void showSignatureDialog(const ByteArray& signatureData, bool modal);

	/**

	@param params - mapa zawieraj�ca dodatkowe parametry uruchomieniowe maszyny
				wirtualnej. Jako klucze w mapie powinny znajdowa� si� nazwy
				parametr�w (ze znakiem '=', je�li jest konieczny), a jako
				warto�ci - warto�ci parametr�w. Je�li dany parametr nie posiada
				warto�ci (np. jaka� flaga, jak <code>-ea</code>), do mapy powinna
				by� w�o�ona pusta warto��. Przyk�adowo, mapa z parametrami
				uruchomieniowymi mo�e wygl�da� tak:
\code
"-Xmx"                =>  "256m"
"-jni:verbatim"       =>  ""
\endcode
	
	*/
	void init(const std::string& libdir, const std::string& jrepath, const ParamMap& params = ParamMap());
};

} //namespace javacall

#endif //UNIZETO_JAVACALL_JAVASIGNATURE_ENV
