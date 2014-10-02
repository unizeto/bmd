/** @file
Modu³ zawieraj¹cy klasy wspomagaj¹ce pracê z JVM w wykorzystaniu z proCertumJavaApi.
*/

#ifndef UNIZETO_JAVACALL_JAVASIGNATURE_ENV
#define UNIZETO_JAVACALL_JAVASIGNATURE_ENV

#include "JvmEnvironment.h"


#include "CppJavaApiWrapper.h"

#include <jni.h>

#include <vector>
#include <map>

namespace javacall {

/** Pomocniczy typedef dla tablicy bajtów. */
typedef std::vector<jbyte> ByteArray;
/** Pomocniczy typedef dla tablicy tablic bajtów. */
typedef  std::vector<ByteArray> ByteArrayCollection;

/**
Klasa wposmagaj¹ca pracê z konkretn¹ bibliotek¹ java, jak¹ jest proCertumJavaApi.
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

	@param params - mapa zawieraj¹ca dodatkowe parametry uruchomieniowe maszyny
				wirtualnej. Jako klucze w mapie powinny znajdowaæ siê nazwy
				parametrów (ze znakiem '=', jeœli jest konieczny), a jako
				wartoœci - wartoœci parametrów. Jeœli dany parametr nie posiada
				wartoœci (np. jakaœ flaga, jak <code>-ea</code>), do mapy powinna
				byæ w³o¿ona pusta wartoœæ. Przyk³adowo, mapa z parametrami
				uruchomieniowymi mo¿e wygl¹daæ tak:
\code
"-Xmx"                =>  "256m"
"-jni:verbatim"       =>  ""
\endcode
	
	*/
	void init(const std::string& libdir, const std::string& jrepath, const ParamMap& params = ParamMap());
};

} //namespace javacall

#endif //UNIZETO_JAVACALL_JAVASIGNATURE_ENV
