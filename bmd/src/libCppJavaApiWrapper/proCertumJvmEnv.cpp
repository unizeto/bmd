#include "proCertumJvmEnv.h"

#include <string>
#include <cstring>


using namespace std;

namespace javacall {

const char* proCertumJvmEnv::WRAPPER_CLASS = "pl/unizeto/javacall/CppJavaApiWrapper";

proCertumJvmEnv::proCertumJvmEnv(void)
{
}

proCertumJvmEnv::~proCertumJvmEnv(void)
{
}

ByteArrayCollection proCertumJvmEnv::getCerts() {

	LocalRef<jclass> signatureClass(findClass(WRAPPER_CLASS), env);
	jmethodID getCertsMethod = findMethod(signatureClass.get(), "getCertificates", "()[[B");

	LocalRef<jobjectArray> jCertsArray(
		callStaticMethod<jobject, jobjectArray>(signatureClass.get(), getCertsMethod),
		env);
	

	std::vector<jbyteArray> certsArray = toNativeArrayOf<jbyteArray>(jCertsArray.get());

	size_t certsCount = certsArray.size();
	ByteArrayCollection result(certsCount);

	for(size_t i = 0; i < certsCount; ++i) {
		LocalRef<jbyteArray> cert(certsArray[i], env);
		result[i] = toNativeArray<jbyte>(cert.get());

	}

	return result;
}

ByteArray proCertumJvmEnv::sign(const ByteArray& message, const ByteArray& certBytes, bool qualified, bool enveloping) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName;

	if(qualified) {
		functionName = enveloping ? "signQualifiedEnveloping" : "signQualifiedDetached";
	} else {
		functionName = enveloping ? "signEnveloping" : "signDetached";
	}

	jmethodID signMethod = findMethod(signatureClass.get(), functionName, "([B[B)[B");

	LocalRef<jbyteArray> jmessageBytes(toJavaArray(message), env);
	LocalRef<jbyteArray> jcertBytes(toJavaArray(certBytes), env);
	
	ArgsArray signParams(2);
	signParams[0].l = jmessageBytes.get();
	signParams[1].l = jcertBytes.get();

	LocalRef<jbyteArray> jresultArray(
		callStaticMethod<jobject, jbyteArray>
			(signatureClass.get(), signMethod, signParams),
		env);

	ByteArray result(toNativeArray<jbyte>(jresultArray.get()));

	return result;
}

void proCertumJvmEnv::validate(const ByteArray& signature, const ByteArray& signedData, bool detached) {

	LocalRef<jclass> signatureClass(findClass(WRAPPER_CLASS), env);
	
	string functionName = "validate";

	char const *const methodSignature = detached ? "([B[B)V" : "([B)V";
	jmethodID validateMethod = findMethod(signatureClass.get(), functionName, methodSignature);

	LocalRef<jbyteArray> jsignatureBytes(toJavaArray(signature), env);
	
	LocalRef<jbyteArray> jSignedDataBytes(detached ? toJavaArray(signedData) :  NULL, env);

	ArgsArray validateParams(2);
	validateParams[0].l = jsignatureBytes.get();
	validateParams[1].l = jSignedDataBytes.get();

	callStaticMethod<void>(signatureClass.get(), validateMethod, validateParams) ;
}

void proCertumJvmEnv::verify(const ByteArray& signature, const ByteArray& signedData, bool qualified, bool detached) {

	LocalRef<jclass> signatureClass(findClass(WRAPPER_CLASS), env);
	
	string functionName = "verify";

	char const *const methodSignature = detached ? "([B[BZ)V" : "([BZ)V";
	jmethodID validateMethod = findMethod(signatureClass.get(), functionName, methodSignature);

	LocalRef<jbyteArray> jsignatureBytes(toJavaArray(signature), env);
	
	LocalRef<jbyteArray> jSignedDataBytes(detached ? toJavaArray(signedData) : NULL, env);

	ArgsArray validateParams(3);
	validateParams[0].l = jsignatureBytes.get();

	if(detached) {
		validateParams[1].l = jSignedDataBytes.get();
		validateParams[2].z = (qualified ? JNI_TRUE : JNI_FALSE);
	} else {
		validateParams[1].z = (qualified ? JNI_TRUE : JNI_FALSE);
	}

	callStaticMethod<void>(signatureClass.get(), validateMethod, validateParams) ;
}

void proCertumJvmEnv::showSignatureDialog(const ByteArray& signatureBytes, bool modal) {

	LocalRef<jbyteArray> jSigBytes(toJavaArray(signatureBytes), env);
	
	LocalRef<jclass> signatureClass(findClass(WRAPPER_CLASS), env);
	jmethodID methodId = findMethod(signatureClass.get(), "showSignatureDialog", "([BZ)V");
	ArgsArray args(2);
	args[0].l = jSigBytes.get();
	args[1].z = modal;
	callStaticMethod<void>(signatureClass.get(), methodId, args);
}

ByteArray proCertumJvmEnv::signPDF(const ByteArray& message, const ByteArray& certBytes, bool qualified) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName;

	if(qualified) {
		functionName = "signPDFQualified";
	} else {
		functionName = "signPDF";
	}

	jmethodID signMethod = findMethod(signatureClass.get(), functionName, "([B[B)[B");

	LocalRef<jbyteArray> jmessageBytes(toJavaArray(message), env);
	LocalRef<jbyteArray> jcertBytes(toJavaArray(certBytes), env);
	
	ArgsArray signParams(2);
	signParams[0].l = jmessageBytes.get();
	signParams[1].l = jcertBytes.get();

	LocalRef<jbyteArray> jresultArray(
		callStaticMethod<jobject, jbyteArray>
			(signatureClass.get(), signMethod, signParams),
		env);

	ByteArray result(toNativeArray<jbyte>(jresultArray.get()));

	return result;
}

ByteArray proCertumJvmEnv::addTimestampToPDF(const ByteArray& message) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "addTimestampToPDF";

	jmethodID signMethod = findMethod(signatureClass.get(), functionName, "([B)[B");

	LocalRef<jbyteArray> jmessageBytes(toJavaArray(message), env);
	
	ArgsArray signParams(1);
	signParams[0].l = jmessageBytes.get();

	LocalRef<jbyteArray> jresultArray(
		callStaticMethod<jobject, jbyteArray>
			(signatureClass.get(), signMethod, signParams),
		env);

	ByteArray result(toNativeArray<jbyte>(jresultArray.get()));

	return result;
}

ByteArray proCertumJvmEnv::getLastAddedTimestampToPDF() {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "getLastAddedTimestampToPDF";

	jmethodID signMethod = findMethod(signatureClass.get(), functionName, "()[B");
	
	ArgsArray signParams(0);

	LocalRef<jbyteArray> jresultArray(
		callStaticMethod<jobject, jbyteArray>
			(signatureClass.get(), signMethod, signParams),
		env);

	ByteArray result(toNativeArray<jbyte>(jresultArray.get()));

	return result;
}

void proCertumJvmEnv::getPKCS7Info(const ByteArray& signedPdf, PKCS7Info * result) {

	LocalRef<jclass> wrapperClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "getPKCS7InfoFromPDF";

	jmethodID method = findMethod(wrapperClass.get(), functionName, "([B)Lpl/unizeto/javacall/PdfSignatureInfo;");

	LocalRef<jbyteArray> jmessageBytes(toJavaArray(signedPdf), env);
	
	ArgsArray params(1);
	params[0].l = jmessageBytes.get();

	LocalRef<jobject> jresult(
		callStaticMethod<jobject, jbyteArray>
			(wrapperClass.get(), method, params),
		env);
 
     /* Get a reference to obj's class */
     jclass cls = env->GetObjectClass(jresult.get());
 
     /* Look for the instance field in cls */
     jfieldID digestAlgFid = env->GetFieldID(cls, "digestAlg", "Ljava/lang/String;");
     jfieldID digestFid = env->GetFieldID(cls, "digest", "[B");
	 jfieldID pkcs7SignatureFid = env->GetFieldID(cls, "pkcs7Signature", "[B");
 
     /* Read the instance fields */
     LocalRef<jstring> jdigestAlg((jstring)env->GetObjectField(jresult.get(), digestAlgFid), env);
	 LocalRef<jbyteArray> jDigest((jbyteArray)env->GetObjectField(jresult.get(), digestFid), env);
	 LocalRef<jbyteArray> jpkcs7Signature((jbyteArray)env->GetObjectField(jresult.get(), pkcs7SignatureFid), env);

	 /* Copy values to C */
	 const char *digestAlg = env->GetStringUTFChars(jdigestAlg.get(), NULL);
	 result->hashAlg = new char[strlen(digestAlg) + 1]; 
	 strcpy(result->hashAlg, digestAlg);
	 env->ReleaseStringUTFChars(jdigestAlg.get(), digestAlg);
    
	 result->signatureLen = env->GetArrayLength(jpkcs7Signature.get());
	 result->signature = new unsigned char[result->signatureLen];
	 env->GetByteArrayRegion(jpkcs7Signature.get(), 0, result->signatureLen, (jbyte*)result->signature);

	 result->hashLen = env->GetArrayLength(jDigest.get());
	 result->hash = new unsigned char[result->hashLen];
	 env->GetByteArrayRegion(jDigest.get(), 0, result->hashLen, (jbyte*)result->hash);
}	

ByteArray proCertumJvmEnv::getTimeStamp(const ByteArray& message) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "getTimeStamp";

	jmethodID signMethod = findMethod(signatureClass.get(), functionName, "([B)[B");

	LocalRef<jbyteArray> jmessageBytes(toJavaArray(message), env);
	
	ArgsArray params(1);
	params[0].l = jmessageBytes.get();

	LocalRef<jbyteArray> jresultArray(
		callStaticMethod<jobject, jbyteArray>
			(signatureClass.get(), signMethod, params),
		env);

	ByteArray result(toNativeArray<jbyte>(jresultArray.get()));

	return result;
}

void proCertumJvmEnv::setPkcs11Driver(char const * driver) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "setPkcs11Driver";

	jmethodID method = findMethod(signatureClass.get(), functionName, "(Ljava/lang/String;)V");

	// UWAGA, nazwa sterownika nie jest w UTF, ale zak³adamy ¿e nie bêdzie mia³a polskich literek w nazwie wiêc jest w UTF.
	LocalRef<jstring> str (env->NewStringUTF(driver), env);

	ArgsArray params(1);
	params[0].l = str.get();

	callStaticMethod<void>(signatureClass.get(), method, params) ;
}

void proCertumJvmEnv::setPkcs11Pin(char const * pin) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "setPkcs11Pin";

	jmethodID method = findMethod(signatureClass.get(), functionName, "([C)V");

	// UWAGA, wartoœæ nie jest w UTF, ale zak³adamy ¿e nie bêdzie mia³a polskich literek w nazwie wiêc jest w UTF.
	LocalRef<jstring> str (env->NewStringUTF(pin), env);

	const jchar* buf = env->GetStringChars(str.get(), 0);
	LocalRef<jcharArray> charArray (env->NewCharArray(strlen(pin)), env);
	env->SetCharArrayRegion(charArray.get(), 0, strlen(pin), buf);

	ArgsArray params(1);
	params[0].l = charArray.get();

	callStaticMethod<void>(signatureClass.get(), method, params) ;

	env->ReleaseStringChars(str.get(), buf);
}

void proCertumJvmEnv::setPkcs11SlotId(long int slotId) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "setPkcs11SlotId";

	jmethodID method = findMethod(signatureClass.get(), functionName, "(J)V");

	ArgsArray params(1);
	params[0].j = slotId;

	callStaticMethod<void>(signatureClass.get(), method, params) ;
}

void proCertumJvmEnv::setTSAAddress(char const * url) {

	LocalRef<jclass> signatureClass (findClass(WRAPPER_CLASS), env);
	
	string functionName = "setTSAAddress";

	jmethodID method = findMethod(signatureClass.get(), functionName, "([C)V");

	// UWAGA, wartoœæ nie jest w UTF, ale zak³adamy ¿e nie bêdzie mia³a polskich literek w nazwie wiêc jest w UTF.
	LocalRef<jstring> str (env->NewStringUTF(url), env);

	const jchar* buf = env->GetStringChars(str.get(), 0);
	LocalRef<jcharArray> charArray (env->NewCharArray(strlen(url)), env);
	env->SetCharArrayRegion(charArray.get(), 0, strlen(url), buf);

	ArgsArray params(1);
	params[0].l = charArray.get();

	callStaticMethod<void>(signatureClass.get(), method, params) ;

	env->ReleaseStringChars(str.get(), buf);
}

template <typename T, int N>
int tableSize(T(&)[N]) {
	return N;
}

void proCertumJvmEnv::init(const string& libdir, const string& jrepath, const ParamMap& params) {

	string jars[] = {
		"CppJavaApiWrapper.jar",
		"activation.jar",
		"bcmail-jdk15-145.jar",
		"bcprov-jdk15-145.jar",
		"bctsp-jdk15-145.jar",
		"CAPIProv.jar",
		"CMSUtil.jar",
		"commons-codec-1.3.jar",
		"commons-collections-3.2.jar",
		"commons-discovery.jar",
		"commons-io-1.4.jar",
		"commons-lang-2.4.jar",
		"CryptoCMP.jar",
		"ElectronicSignaturePolicies.jar",
		"iaikPkcs11Provider.jar",
		"iaikPkcs11Wrapper.jar",
		"iaik_cms.jar",
		"iaik_jce_full.jar",
		"iaik_tsp.jar",
		"iaik_xades.jar",
		"iaik_xsect.jar",
		"iText-2.1.7.jar",
		"jaxb-api.jar",
		"jaxb-impl.jar",
		"jaxb1-impl.jar",
		"jna.jar",
		"jsr173_1.0_api.jar",
		"log4j.jar",
		"log4j.properties",
		"mail.jar",
		"proCertumJavaAPI.jar",
		"slf4j-api-1.4.0.jar",
		"slf4j-log4j12-1.4.0.jar",
		"swing-layout-1.0.jar",
		"swing-worker-1.1.jar",
		"xmlElectronicSignaturePolicies.jar"
	};

	string classpath;
	for(int i=0; i < tableSize(jars); ++i) {
		classpath.
			append(libdir).

			append("/").
			append(jars[i]).
#ifdef WIN32						
			append(";");
#else 
			append(":");
#endif			
	}

	// dodajemy te¿ katalog
	classpath.append(libdir);

#ifdef WIN32
	string jvmDllPath = "jvm.dll";
#else
	string jvmDllPath = "libjvm.so";
#endif	
	if(jrepath.length()) {
#ifdef WIN32	
		jvmDllPath = jrepath + "\\bin\\client\\jvm.dll";
#else
		jvmDllPath = jrepath + "/lib/i386/client/libjvm.so";
#endif		
	}

	map<string, string> newParams(params);
	newParams["-Djava.class.path="] = classpath;


	JvmEnvironment::init(jvmDllPath.c_str(), newParams);
}

} //namespace javacall
