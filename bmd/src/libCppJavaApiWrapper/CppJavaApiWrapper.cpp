
#ifdef WIN32
#include "stdafx.h"
#include <tchar.h>
#endif

#include "CppJavaApiWrapper.h"
#include "proCertumJvmEnv.h"



#include <string>
#include <memory>
#include <map>

#define _SEPARATE_THREAD_FOR_DIALOGS 1

using namespace std;

typedef unsigned char Byte;
typedef Byte* PByte;
typedef Byte const * PCByte;
typedef unsigned int BuffSize;

string g_currentError;
string g_detailedError;

void setError(const string& msg, const string& stacktrace = "") {
	g_currentError = msg;
	g_detailedError = stacktrace;
}

JAVACALL_DLL_API const char* GetCurrentErrorString() {
	return g_currentError.c_str();
}

JAVACALL_DLL_API const char* GetDetailedErrorMsg() {
	return g_detailedError.c_str();
}

javacall::proCertumJvmEnv g_javaSignature;

JAVACALL_DLL_API int proCertumJavaVmInit(const char* libdir, const char* jrepath, const char* params[][2], unsigned paramCount) {

	try {

		map<string, string> paramMap;
		for(size_t i=0; i<paramCount; ++i) {

			paramMap[params[i][0]] = (params[i][1] ? params[i][1] : "");
		}		

		g_javaSignature.init(libdir ? libdir : ".\\lib", jrepath ? jrepath : "", paramMap);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_INIT_ERROR;
	} catch(const exception& e) {
		setError(e.what());
		return JAVACALL_INIT_ERROR;
	} catch(...) {
		setError("Nieznany b³¹d");
		return JAVACALL_INIT_ERROR;
	}

	g_currentError = "OK";
	return JAVACALL_OK;
}

javacall::ByteArrayCollection cachedCerts;

JAVACALL_DLL_API int GetCertificateCount(int* count) {

	try {
		cachedCerts = g_javaSignature.getCerts();
		*count = cachedCerts.size();
		return JAVACALL_OK;
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const std::exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}
}

JAVACALL_DLL_API int GetCertificate(int cert, PByte certData, BuffSize* datalen) {

	const javacall::ByteArray& cachedCert = cachedCerts[cert];

	if(!certData) {
		*datalen = cachedCert.size();
	} else {
		std::copy(cachedCert.begin(), cachedCert.end(), certData);
		*datalen = cachedCert.size();
	}

	setError("OK");
	return JAVACALL_OK;
}

javacall::ByteArray cachedSignResult;

int sign(PCByte messageBytes, BuffSize messageLen,
		 PCByte certBytes, BuffSize certDataLen,
		 PByte sigArray, BuffSize* outputlen,
		 bool qualified, bool enveloping)
{
	try {

		if(!sigArray) {
			javacall::ByteArray msg(messageBytes, messageBytes + messageLen);
			javacall::ByteArray cert(certBytes, certBytes + certDataLen);
			cachedSignResult = g_javaSignature.sign(msg, cert,qualified, enveloping);
			*outputlen = cachedSignResult.size();
		} else {
			copy(cachedSignResult.begin(), cachedSignResult.end(), sigArray);
			*outputlen = cachedSignResult.size();
		}
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

javacall::ByteArray cachedAddedTimestampResult;

JAVACALL_DLL_API int uniPDFAddTimestamp(
	PCByte messageBytes, BuffSize messageLen,
	PByte sigArray, BuffSize* outputlen,
	PByte addedTimestamp, BuffSize* addedTimestampLen) {

	try {

		if(!sigArray) {
			javacall::ByteArray msg(messageBytes, messageBytes + messageLen);
			cachedSignResult = g_javaSignature.addTimestampToPDF(msg);
			cachedAddedTimestampResult = g_javaSignature.getLastAddedTimestampToPDF();
			*outputlen = cachedSignResult.size();
			*addedTimestampLen = cachedAddedTimestampResult.size();
		} else {
			copy(cachedSignResult.begin(), cachedSignResult.end(), sigArray);
			copy(cachedAddedTimestampResult.begin(), cachedAddedTimestampResult.end(), addedTimestamp);
			*outputlen = cachedSignResult.size();
			*addedTimestampLen = cachedAddedTimestampResult.size();
		}
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}
int signPDF(PCByte messageBytes, BuffSize messageLen,
		 PCByte certBytes, BuffSize certDataLen,
		 PByte sigArray, BuffSize* outputlen,
		 bool qualified)
{
	try {

		if(!sigArray) {
			javacall::ByteArray msg(messageBytes, messageBytes + messageLen);
			javacall::ByteArray cert(certBytes, certBytes + certDataLen);
			cachedSignResult = g_javaSignature.signPDF(msg, cert, qualified);
			*outputlen = cachedSignResult.size();
		} else {
			copy(cachedSignResult.begin(), cachedSignResult.end(), sigArray);
			*outputlen = cachedSignResult.size();
		}
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESValidateEnveloping
			(unsigned char const * signatureBytes, unsigned int signatureLen) {

	 try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		g_javaSignature.validate(signature, javacall::ByteArray(), false);

	 } catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESValidateDetached
			(unsigned char const * signatureBytes, unsigned int signatureLen,
			 unsigned char const * signedDataBytes, unsigned int signedDataLen) {

	 try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		javacall::ByteArray signedData(signedDataBytes, signedDataBytes + signedDataLen);

		g_javaSignature.validate(signature, signedData, true);

	 } catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedEnveloping
(unsigned char const * signatureBytes, unsigned int signatureLen)
{
	try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		
		g_javaSignature.verify(signature, javacall::ByteArray(), false, false);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESVerifyNonQualifiedDetached
(unsigned char const * signatureBytes, unsigned int signatureLen,
 unsigned char const * signedDataBytes, unsigned int signedDataLen)
{
	try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		javacall::ByteArray signedData(signedDataBytes, signedDataBytes +signedDataLen);

		g_javaSignature.verify(signature, signedData, false, true);

	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESVerifyQualifiedEnveloping
(unsigned char const * signatureBytes, unsigned int signatureLen)
{
	try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		
		g_javaSignature.verify(signature, javacall::ByteArray(), true, false);

	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniXAdESVerifyQualifiedDetached
(unsigned char const * signatureBytes, unsigned int signatureLen,
 unsigned char const * signedDataBytes, unsigned int signedDataLen)
{
	try {
		javacall::ByteArray signature(signatureBytes, signatureBytes + signatureLen);
		javacall::ByteArray signedData(signedDataBytes, signedDataBytes + signedDataLen);

		g_javaSignature.verify(signature, signedData, true, true);

	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}
JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryDetached(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return sign(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			true, false);
}

JAVACALL_DLL_API int uniXAdESSignQualifiedBinaryEnveloping(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return sign(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			true, true);
}

JAVACALL_DLL_API int uniXAdESSignBinaryDetached(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return sign(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			false, false);
}

JAVACALL_DLL_API int uniXAdESSignBinaryEnveloping(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return sign(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			false, true);
}

JAVACALL_DLL_API int uniPDFSignQualified(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return signPDF(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			true);
}

JAVACALL_DLL_API int uniPDFSign(
	PCByte messageBytes, BuffSize messageLen,
	PCByte certBytes, BuffSize certDataLen,
	PByte sigArray, BuffSize* outputlen) {

		return signPDF(messageBytes, messageLen,
			certBytes, certDataLen,
			sigArray, outputlen,
			false);
}


JAVACALL_DLL_API int uniPDFGetPKCS7Info
							(unsigned char const * signatureBytes, unsigned int signatureLen,
							 PKCS7Info * result) {

	try {
		javacall::ByteArray signedPdf(signatureBytes, signatureBytes + signatureLen);
		g_javaSignature.getPKCS7Info(signedPdf, result);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API PKCS7Info * uniPDFAllocPKCS7Info(void) {

	return new PKCS7Info();
}

JAVACALL_DLL_API void uniPDFFreePKCS7Info(PKCS7Info * pkcs7Info) {

	if(pkcs7Info == NULL) {
		return;
	}

	if(pkcs7Info->signature != NULL) {
		delete[] pkcs7Info->signature;
	}
	if(pkcs7Info->hash != NULL) {
		delete[] pkcs7Info->hash;
	}
	if(pkcs7Info->hashAlg != NULL) {
		delete[] pkcs7Info->hashAlg;
	}
	delete pkcs7Info;
}

JAVACALL_DLL_API int uniPKCS11SetDriver(char const * driver) {

	try {
		g_javaSignature.setPkcs11Driver(driver);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniPKCS11SetPin(char const * pin) {

	try {
		g_javaSignature.setPkcs11Pin(pin);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniPKCS11SetSlotId(long int slotId) {

	try {
		g_javaSignature.setPkcs11SlotId(slotId);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniTSTGetTimeStamp
							(PCByte documentBytes, BuffSize documentLen,
							 PByte timestampBytes, BuffSize* timestampLen) {

    try {

		if(!timestampBytes) {
			javacall::ByteArray msg(documentBytes, documentBytes + documentLen);
			cachedSignResult = g_javaSignature.getTimeStamp(msg);
			*timestampLen = cachedSignResult.size();
		} else {
			copy(cachedSignResult.begin(), cachedSignResult.end(), timestampBytes);
			*timestampLen = cachedSignResult.size();
		}
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}					 
    
    setError("OK");
	return JAVACALL_OK;
}

JAVACALL_DLL_API int uniTSTSetTSAAddress(char const * tsaAddress) {

   	 
   try {
		g_javaSignature.setTSAAddress(tsaAddress);
	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& ex) {
		setError(ex.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

    setError("OK");
	return JAVACALL_OK;
}

#ifdef WIN32

HWND hwndOwner;
HWND hwndDialog;

JAVACALL_DLL_API void CallbackRegisterWindow(HWND window) {
	OutputDebugString(_T("Registering dialog window...\n"));
	hwndDialog = window;
}



//SetForegroundWindow to jedyny sposób, aby okno w³asciciela przenieœæ
//na wierzch po zamkniêciu dialogu. Tylko czy nie za mocny? 
//Focus Stealing chyba nie jest zagro¿eniem, skoro okno w³aœciciela jest
//zawsze tu¿ pod oknem dialogowym.
JAVACALL_DLL_API void CallbackEnableParent() {
	OutputDebugString(_T("Enabling parent window...\n"));
//	EnableWindow(hwndOwner, TRUE);
//	SetForegroundWindow(hwndOwner);
}


WNDPROC oldOwnerWndProc;
WNDPROC oldDialogWndProc;
HANDLE waitForAppModal = NULL;

LRESULT FAR PASCAL DialogSubClassFunc(   HWND hWnd,
									  UINT Message,
									  WPARAM wParam,
									  LPARAM lParam)
{

	//Okna JavaAwtDialog nie otrzymuj¹ czasami komunikatu WM_CLOSE.
	//WM_DESTROY jest otrzymywany zawsze, wiêc na niego reagujemy
	if ( Message == WM_DESTROY ) {
		OutputDebugString(_T("Destroying window...\n"));

		if(!hwndOwner) {
			SetEvent(waitForAppModal);
		} else {

			oldOwnerWndProc = (WNDPROC)SetWindowLong(hwndOwner,
				GWL_WNDPROC, (DWORD) oldOwnerWndProc);

			if(oldOwnerWndProc) {
				OutputDebugString(_T("Owner window unsubclassed\n"));
			} else {
				OutputDebugString(_T("Failed to unsubclass owner window\n"));
			}

			EnableWindow(hwndOwner, TRUE);
			SetForegroundWindow(hwndOwner);
			hwndOwner = NULL;
		}

	} else if (Message == WM_ACTIVATE && hwndOwner) {
		WORD activateState = LOWORD(wParam);
		if(HIWORD(wParam) == 0 && (activateState == WA_ACTIVE || activateState == WA_CLICKACTIVE) ) {
			SetWindowPos(hwndOwner, hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}

	return CallWindowProc(oldDialogWndProc, hWnd, Message, wParam,
		lParam);
}


LRESULT FAR PASCAL OwnerSubClassFunc(   HWND hWnd,
									 UINT Message,
									 WPARAM wParam,
									 LPARAM lParam)
{

	if (Message == WM_ACTIVATE) {
		WORD activateState = LOWORD(wParam);
		if(HIWORD(wParam) == 0 && (activateState == WA_ACTIVE || activateState == WA_CLICKACTIVE) ) {
			SetWindowPos(hwndDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	return CallWindowProc(oldOwnerWndProc, hWnd, Message, wParam,
		lParam);
}

struct ShowSigDlgThreadData {
	PCByte sigData;
	BuffSize sigLen;
	HWND owner;
	bool isModal;
};

DWORD WINAPI ShowSigDlgThreadProc(LPVOID params) {

#if _SEPARATE_THREAD_FOR_DIALOGS
	auto_ptr<ShowSigDlgThreadData> sigParams((ShowSigDlgThreadData*)params);
#else
	ShowSigDlgThreadData* sigParams((ShowSigDlgThreadData*)params);
#endif

	javacall::proCertumJvmEnv threadLocal;
	threadLocal.init("", "");

	javacall::ByteArray sigBytes(sigParams->sigData, sigParams->sigData + sigParams->sigLen);
	
#if _SEPARATE_THREAD_FOR_DIALOGS
	delete[] sigParams->sigData;
#endif

	threadLocal.showSignatureDialog(sigBytes, sigParams->isModal);

	if(sigParams->isModal) {

		if(hwndDialog == NULL) {
			OutputDebugString(_T("Window not found\n"));
			return 0;
		}

		oldDialogWndProc = (WNDPROC)SetWindowLong(hwndDialog,
			GWL_WNDPROC, (DWORD) DialogSubClassFunc);

		if(oldDialogWndProc) {
			OutputDebugString(_T("Dialog window subclassed\n"));
		} else {
			OutputDebugString(_T("Failed to subclass dialog window\n"));
			if(waitForAppModal) {
				SetEvent(waitForAppModal);
			}
		}

		if(sigParams->owner) {

			EnableWindow(sigParams->owner, FALSE);
			hwndOwner = sigParams->owner;
			oldOwnerWndProc = (WNDPROC)SetWindowLong(hwndOwner,
				GWL_WNDPROC, (DWORD) OwnerSubClassFunc);

			if(oldOwnerWndProc) {
				OutputDebugString(_T("Owner window subclassed\n"));
			} else {
				OutputDebugString(_T("Failed to subclass owner window\n"));
			}
		}
	}

#if !_SEPARATE_THREAD_FOR_DIALOGS
	if(sigParams->isModal && !sigParams->owner) {
		waitForAppModal = CreateEvent(NULL, FALSE, FALSE, NULL);
		WaitForSingleObject(waitForAppModal, INFINITE);
		CloseHandle(waitForAppModal);
		waitForAppModal = NULL;
	}
#endif

	return 0;
}

#if _SEPARATE_THREAD_FOR_DIALOGS

int DoShowSignatureDialog(const ShowSigDlgThreadData& threadData) {

	DWORD threadId;

	//Dane do przekazania do drugiego w¹tku kopiujemy na stertê, aby
	//drugi w¹tek móg³ sobie bez problemu po nie siêgn¹æ, a po u¿yciu zwolniæ.
	ShowSigDlgThreadData * onHeap = new ShowSigDlgThreadData(threadData);
	PByte ptr = new Byte[threadData.sigLen];
	copy(threadData.sigData, threadData.sigData + threadData.sigLen, ptr);
	onHeap->sigData = ptr;

	HANDLE th = CreateThread(NULL, 0,ShowSigDlgThreadProc, (LPVOID)onHeap, 0, &threadId);

	if(threadData.isModal && !threadData.owner) {
		waitForAppModal = CreateEvent(NULL, FALSE, FALSE, NULL);
		WaitForSingleObject(waitForAppModal, INFINITE);
		CloseHandle(waitForAppModal);
		waitForAppModal = NULL;
	}

	return JAVACALL_OK;
}
#endif

JAVACALL_DLL_API int ShowSignatureDialog(PCByte signatureData, BuffSize signatureLen, HWND owner) {

	try {
		ShowSigDlgThreadData threadData;

		threadData.owner = owner;
		threadData.sigData = signatureData;
		threadData.sigLen = signatureLen;
		threadData.isModal = true;

		
#if _SEPARATE_THREAD_FOR_DIALOGS
		return DoShowSignatureDialog(threadData);
#else
		ShowSigDlgThreadProc(&threadData);
		return JAVACALL_OK;
#endif

	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;
	} catch (const exception& e) {
		setError(e.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	return JAVACALL_OK;
}




JAVACALL_DLL_API int ShowSignatureDialogNonModal(unsigned char const * signatureBytes, unsigned int signatureLen)
{
	try {
		ShowSigDlgThreadData threadData;

		threadData.owner = NULL;
		threadData.sigData = signatureBytes;
		threadData.sigLen = signatureLen;
		threadData.isModal = false;

#if _SEPARATE_THREAD_FOR_DIALOGS
		return DoShowSignatureDialog(threadData);
#else
		ShowSigDlgThreadProc(&threadData);
		return JAVACALL_OK;
#endif

	} catch (const javacall::JvmException& e) {
		setError(e.what(), e.getStackTrace());
		return JAVACALL_GENERAL_ERROR;	
	} catch (const exception& e) {
		setError(e.what());
		return JAVACALL_GENERAL_ERROR;
	} catch (...) {
		setError("Nieznany b³¹d");
		return JAVACALL_GENERAL_ERROR;
	}

	return JAVACALL_OK;
}

#endif //WIN32
