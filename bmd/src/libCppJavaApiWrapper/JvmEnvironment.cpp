#include "JvmEnvironment.h"


#include <string>
#include <iostream>
#include <exception>
#include <cstdlib>
#include <map>

#ifndef WIN32
#include <dlfcn.h>
#endif

using namespace std;




namespace javacall {


	namespace _Traits {
		
		template<> FunPtrs<jbyte>::NewArrayPtrType FunPtrs<jbyte>::newArrayPtr =  &JNIEnv::NewByteArray;
		template<> FunPtrs<jbyte>::CallStaticMethodAPtrType FunPtrs<jbyte>::callStaticMethodA = &JNIEnv::CallStaticByteMethodA;
		template<> FunPtrs<jbyte>::SetArrayRegionPtrType FunPtrs<jbyte>::setArrayRegion = &JNIEnv::SetByteArrayRegion;
		template<> FunPtrs<jbyte>::GetArrayRegionPtrType FunPtrs<jbyte>::getArrayRegion = &JNIEnv::GetByteArrayRegion;

		FunPtrs<void>::CallStaticMethodAPtrType FunPtrs<void>::callStaticMethodA = &JNIEnv::CallStaticVoidMethodA;

		FunPtrs<jobject>::NewArrayPtrType FunPtrs<jobject>::newArrayPtr =  &JNIEnv::NewObjectArray;
		FunPtrs<jobject>::CallStaticMethodAPtrType FunPtrs<jobject>::callStaticMethodA = &JNIEnv::CallStaticObjectMethodA;

	}

JvmEnvironment::JvmEnvironment() : attached(false), canDestroy(false),
								jvmLib(NULL),
								fnCreateJavaVM(NULL), 
								fnGetCreatedJavaVMs(NULL), 
								fnGetDefaultJavaVMInitArgs(NULL)
{
}

JvmEnvironment::~JvmEnvironment() {

	if(vm) {
		if(attached) {
			vm->DetachCurrentThread();			
		} else if (canDestroy) {			
			// w tej wersji JNI nie dzia³a.
			//jint res = vm->DestroyJavaVM();
		}
	}

	if(jvmLib) {
#ifdef WIN32		
		FreeLibrary(jvmLib);
#else
		
		dlclose(jvmLib);	    
#endif		
	}
}

class ClearExceptionOnExit {
private:
	JNIEnv* env_;
public:
	ClearExceptionOnExit(JNIEnv* env) : env_(env) {}
	~ClearExceptionOnExit() { env_->ExceptionClear();}
};

std::string getExceptionMessage(JNIEnv* env, jthrowable ex) {

	ClearExceptionOnExit clear(env);
	string message = "";

	LocalRef<jclass> clazz(env->GetObjectClass(ex), env);

	jmethodID method = env->GetMethodID(clazz.get(), "getMessage", "()Ljava/lang/String;");
	if(!method) {
		return message;
	}

	LocalRef<jstring>jmessage(static_cast<jstring>(env->CallObjectMethod(ex, method)), env);
	if(!jmessage.get()) {
		return message;
	}
	
	char const* chars = env->GetStringUTFChars(jmessage.get(), NULL);
	if(!chars) {
		return message;
	}

	message.append(chars);
	env->ReleaseStringUTFChars(jmessage.get(), chars);

	return message;
}

std::string getExceptionClassName(JNIEnv* env, jthrowable ex) {
	
	ClearExceptionOnExit clear(env);
	string classname = "";

	LocalRef<jclass> clazz(env->GetObjectClass(ex), env);

	jmethodID getNameMethod = env->GetMethodID(clazz.get(), "getName", "()V");

	if(!getNameMethod) {
		return classname;
	}

	LocalRef<jstring> jclassName(static_cast<jstring>(env->CallObjectMethod(ex, getNameMethod)), env);
	if(env->ExceptionCheck()) {
		return classname;
	}

	char const* chars = env->GetStringUTFChars(jclassName.get(), NULL);
	if(!chars) {
		return classname;
	}

	classname.append(chars);
	env->ReleaseStringUTFChars(jclassName.get(), chars);

	return classname;

}

std::string getStackTrace(JNIEnv* env, jthrowable ex) {

	ClearExceptionOnExit clear(env);
	string stacktrace = "";
	
	LocalRef<jclass> swClass(env->FindClass("java/io/StringWriter"), env);
	if(!swClass.get()) {
		return stacktrace;
	}

	jmethodID swCtor = env->GetMethodID(swClass.get(), "<init>", "()V");
	if(!swCtor) {
		return stacktrace;
	}

	LocalRef<> stringWriter(env->NewObject(swClass.get(), swCtor), env);
	if(!stringWriter.get()) {
		return stacktrace;
	}


	LocalRef<jclass> pwClass(env->FindClass("java/io/PrintWriter"), env);
	if(!pwClass.get()) {
		return stacktrace;
	}

	jmethodID pwCtor = env->GetMethodID(pwClass.get(), "<init>", "(Ljava/io/Writer;)V");
	if(!pwCtor) {
		return stacktrace;
	}

	LocalRef<> printWriter(env->NewObject(pwClass.get(), pwCtor, stringWriter.get()), env);
	if(!printWriter.get()) {
		return stacktrace;
	}

	LocalRef<jclass> exClass(env->GetObjectClass(ex), env);

	jmethodID pstMethod  = env->GetMethodID(exClass.get(), "printStackTrace", "(Ljava/io/PrintWriter;)V");
	if(!pstMethod) {
		return stacktrace;
	}

	env->CallVoidMethod(ex, pstMethod, printWriter.get());
	if(env->ExceptionCheck()) {
		return stacktrace;
	}

	jmethodID toStringMethod = env->GetMethodID(swClass.get(), "toString", "()Ljava/lang/String;");
	if(!toStringMethod) {
		return stacktrace;
	}

	LocalRef<jstring> jresult(static_cast<jstring>(env->CallObjectMethod(stringWriter.get(), toStringMethod)), env);
	if(env->ExceptionCheck()) {
		return stacktrace;
	}

	char const* chars = env->GetStringUTFChars(jresult.get(), NULL);
	if(!chars) {
		return stacktrace;
	}

	stacktrace.append(chars);
	env->ReleaseStringUTFChars(jresult.get(), chars);

	return stacktrace;
}

/**/
void JvmEnvironment::checkException() {


	LocalRef<jthrowable> ex(env->ExceptionOccurred(), env);
	if(ex.get()) {
		env->ExceptionClear();

		string message = getExceptionMessage(env, ex.get());
		string stacktrace = getStackTrace(env, ex.get());
		string className = getExceptionClassName(env, ex.get());
		
		throw JvmException(message, className, stacktrace);
	}
	
}

jclass JvmEnvironment::findClass(const string& name) {

	jclass clazz = env->FindClass(name.c_str());	
	if (clazz == NULL) {  
		checkException();
	} 

	return clazz;
}

jmethodID JvmEnvironment::findMethod(jclass clazz, const string& name, const string& signature) {

	jmethodID methodId = env->GetStaticMethodID(clazz, name.c_str(), signature.c_str()); 
	if (methodId == NULL) { 
		checkException();
	} 

	return methodId;
}

void JvmEnvironment::init(const char* libPath, const map<string, string>& params) {

	LoadJvmDll(libPath);

	JavaVMInitArgs vm_args;

	vm_args.version = JNI_VERSION_1_6;
	jint result = fnGetDefaultJavaVMInitArgs(&vm_args);

	if(result < 0) {
		throw std::runtime_error("JNI_GetDefaultJavaVMInitArgs nie powiod³o sie.");
	}

	vm_args.ignoreUnrecognized = JNI_TRUE;

	jint existingJvmsCount;
	result = fnGetCreatedJavaVMs(&vm, 1, &existingJvmsCount);
	if (vm != NULL && result == 0 && existingJvmsCount > 0 )
	{
		result = vm->GetEnv((void**)&env, vm_args.version);
 
		if ( result == JNI_EDETACHED )
		{
			result = vm->AttachCurrentThread((void**)&env, NULL);
			attached = true;
		}
 
		if ( result < 0 ) {
			throw std::runtime_error("AttachCurrentThread nie powiod³o sie.");
		}		

	} else {

		vector<JavaVMOption> options(params.size());
		vector<string> strOptions(params.size());

		map<string, string>::const_iterator it = params.begin(),
			end = params.end();

		for(int i=0; it != end; ++it, ++i) {
			string option = it->first;
			if(it->second.length()) {
				option.append(it->second);
			}
			strOptions[i] = option;
		}

		for(size_t i=0; i<strOptions.size(); ++i) {
			//non const-correct api :(
			options[i].optionString = const_cast<char*>(strOptions[i].c_str());
		}

		vm_args.options = (options.size() ? &options[0] : NULL);
		vm_args.nOptions = options.size();

		result = fnCreateJavaVM(&vm, (void**)&env, &vm_args);
		if(result < 0) {
			throw std::runtime_error("JNI_CreateJavaVM nie powiod³o sie.");
		}
		canDestroy = true;
	}
}


void JvmEnvironment::LoadJvmDll(const char* dllPath) {

	if(dllPath) {
#ifdef WIN32	
		jvmLib = LoadLibrary(dllPath);
#else
		jvmLib = dlopen(dllPath, RTLD_LAZY);
#endif		
	}

	if(!jvmLib) {
		throw runtime_error("Nie uda³o siê za³adowaæ biblioteki jvm.");
	}

#ifdef WIN32
	fnCreateJavaVM = (PROC_CREATE_VM)GetProcAddress(jvmLib, "JNI_CreateJavaVM");
	fnGetCreatedJavaVMs = (PROC_GET_CREATED_VMS)GetProcAddress(jvmLib, "JNI_GetCreatedJavaVMs");
	fnGetDefaultJavaVMInitArgs = (PROC_GET_DEFAULT_JVM_ARGS)GetProcAddress(jvmLib, "JNI_GetDefaultJavaVMInitArgs");
#else
	fnCreateJavaVM = (PROC_CREATE_VM)dlsym(jvmLib, "JNI_CreateJavaVM");
	fnGetCreatedJavaVMs = (PROC_GET_CREATED_VMS)dlsym(jvmLib, "JNI_GetCreatedJavaVMs");
	fnGetDefaultJavaVMInitArgs = (PROC_GET_DEFAULT_JVM_ARGS)dlsym(jvmLib, "JNI_GetDefaultJavaVMInitArgs");
#endif	

	if(!(fnCreateJavaVM && fnGetCreatedJavaVMs && fnGetDefaultJavaVMInitArgs)){
		throw runtime_error("£adowanie funkcji z biblioteki jvm nie powiod³o siê");
	}
}

template<>
void JvmEnvironment::callStaticMethod<void>(jclass classId, jmethodID methodID, const jvalue* args) {

	_Traits::FunPtrs<void>::CallStaticMethodA(env, classId, methodID, args);

	checkException();
}


/** Specjalizacja dla tablic java przechowuj¹cych obiekty typu
<code>jobject</code>.

@see toNativeArray(typename _Traits::Types<JniType>::ArrayType)
*/
template <>
std::vector<jobject> JvmEnvironment::toNativeArray<jobject>(jobjectArray javaArray) {

	std::vector<jobject> nativeArray;

	jsize size = env->GetArrayLength(javaArray);

	if(size) {
		nativeArray.resize(size);
		
		for(int i=0; i<size; ++i) {
			nativeArray[i] = env->GetObjectArrayElement(javaArray, i);
			checkException();
		}
	}

	return nativeArray;
}



} //namespace javacall
