
#ifdef WIN32
#include "stdafx.h"
#endif

#include <CppJavaApiWrapper.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>

#include <cstdlib>
#include <assert.h>

using namespace std;
using std::ifstream;
using std::ofstream;
using std::ios;

// kilka pomocniczych typedefów
typedef unsigned char Byte;
typedef unsigned int BuffSize;
typedef Byte* PByte;
typedef Byte const * PCByte;

// tablica bajtów opakowana w wektor
// dla automatycznego zwalniania pamiêci
typedef std::vector<Byte> ByteArray;


/**
 * Wczytuje caly plik do stringa
 */
bool file_rd_all( const char *fn, std::string & data) {
    char rdbuf[512];
	std::ifstream in(fn,std::ios::binary);
	if(!in) return false;
    data = "";
	struct stat st;
	if(stat(fn, &st)) return false;
	data.reserve(st.st_size);
    do {
        in.read(rdbuf, sizeof(rdbuf));
        if(in.bad()) return false;
        data.append(rdbuf, (unsigned int)in.gcount());
    } while(in);
	return true;
}

void dieOnError(int status) {

	if(status == JAVACALL_OK) {
		return;
	}

	cout << "B³¹d: " << GetCurrentErrorString() << "\n"
		"Stacktrace: " << GetDetailedErrorMsg();

	ofstream log;
	log.open ("CppJavaApiWrapper.log", ios::app);
    log << "B³¹d: " << GetCurrentErrorString() << "\n"
		"Stacktrace: " << GetDetailedErrorMsg();
    log.close();


	exit(status);
}

void initJvm() {
	
	// dodatkowe parametry pracy maszyny wirtualnej Java
	const char* opts[][2] = {
		{"-Xmx", "16m"},
	//	{"-verbose:jni", NULL}
		{"-Dlog4j.configuration=", "log4j.properties"} // polozenie wzgledem pierwszego parametru proCertumJavaVmInit
	};
	int paramCount = 2;

	// wskazujemy katalog z JRE
	//WSZ const char* javaEnv = getenv("JAVA_HOME");
	const char* javaEnv = "/usr/lib/jvm/java/jre";
	if(javaEnv == NULL) {
	    cout << "Brak ustawionej zmiennej JAVA_HOME" << endl;
	    exit(-1);
	}
	string javahome(javaEnv);
	
	// jeœli u¿ywamy JDK to JRE jest w podkatalogu 'jre'
	
	//WSZ javahome.append("/jre"); 

	//javahome = "c:/Program Files/Java/jre6";

	cout << "JVM init...";
	// inicjujemy Javê
	dieOnError(proCertumJavaVmInit("../CppJavaAPIWrapperLibs/", javahome.c_str(), opts, paramCount));
	cout << "OK" << endl;
}

/**
* Generowanie podpisu PDF.
*/
void testSignPdfOk() {

	// certyfikat podpisuj¹cego
	string signerCert;
	file_rd_all("resources/signer.cer" , signerCert);
	PCByte certBuf = reinterpret_cast<PCByte>(signerCert.data());
	BuffSize certBufLen = signerCert.length();

	// plik PDF do podpisania
	string pdfIn;
	file_rd_all("resources/sample.pdf", pdfIn);
	PCByte pdfBuf = reinterpret_cast<PCByte>(pdfIn.data());
	BuffSize pdfBuffLen = pdfIn.length();

	// sterownik PKCS#11 i PIN
	uniPKCS11SetDriver("crypto3PKCS.dll");
	uniPKCS11SetPin("5432");

	//uniPKCS11SetDriver("c:\\Program Files\\nCipher\\nfast\\bin\\cknfast.dll");
	//uniPKCS11SetPin("12345678");

	// wskazujemy, o który slot nam chodzi (nie zawsze trzeba to wywo³ywaæ, patrz dokumentacja funkcji)
	//uniPKCS11SetSlotId(761406618);
		
	BuffSize sigSize;

	// pierwsze wywo³anie
	dieOnError(uniPDFSign(pdfBuf, pdfBuffLen, certBuf, certBufLen, NULL, &sigSize));

	ByteArray sigNormal(sigSize);

	// drugie wywo³anie
	dieOnError(uniPDFSign(pdfBuf, pdfBuffLen, certBuf, certBufLen, &sigNormal[0], &sigSize));
	
	ofstream sigfile("resources/sample-signed-by-api.pdf", ios_base::binary);
	sigfile.write((char*)&sigNormal[0], sigNormal.size());
	sigfile.close();
}

/**
* Pobieranie podpisu PKCS#7 z podpisanego dokumentu PDF.
*/
void testGetPKCS7InfoFromPdfOk() {

	string signedPdfIn;
	const char *pdfFilename = "resources/sample-signed.pdf";
	
	file_rd_all(pdfFilename , signedPdfIn);
	PCByte signedPdfBuf = reinterpret_cast<PCByte>(signedPdfIn.data());
	BuffSize pdfBuffLen = signedPdfIn.length();

	PKCS7Info * pkcs7Info = uniPDFAllocPKCS7Info();

	dieOnError(uniPDFGetPKCS7Info(signedPdfBuf, pdfBuffLen, pkcs7Info));

	ofstream pkcs7file("resources/sample-signed.pdf.sig", ios::out | ios_base::binary);
	pkcs7file.write((char*)pkcs7Info->signature, pkcs7Info->signatureLen);
	pkcs7file.close();

	ofstream hashFile("resources/sample-signed.pdf.sig.hash", ios::out | ios_base::binary);
	hashFile.write((char*)pkcs7Info->hash, pkcs7Info->hashLen);
	hashFile.close();

	cout << "HashAlg: " << pkcs7Info->hashAlg <<'\n';
	cout << "HashLen: " << pkcs7Info->hashLen <<'\n';
	cout << "SignatureLen: " << pkcs7Info->signatureLen <<'\n';
	assert(strcmp("1.3.14.3.2.26", pkcs7Info->hashAlg) == 0);
	
	uniPDFFreePKCS7Info(pkcs7Info);
}

/**
* Dodawanie do PDF znacznika czasu.
*/
void testAddTimestampToPDFOk() {

	dieOnError(uniTSTSetTSAAddress("http://time.certum.pl"));

	// plik PDF do podpisania
	string pdfIn;
	file_rd_all("./resources/sample-signed.pdf", pdfIn);
	PCByte pdfBuf = reinterpret_cast<PCByte>(pdfIn.data());
	BuffSize pdfBuffLen = pdfIn.length();
	
	// dodanie do PDF znacznika czasu
	BuffSize newPdfSize;
	BuffSize tstSize;

	// pierwsze wywo³anie
	dieOnError(uniPDFAddTimestamp(pdfBuf, pdfBuffLen, NULL, &newPdfSize, NULL, &tstSize));

	ByteArray newPdf(newPdfSize);
	ByteArray tst(tstSize);

	// drugie wywo³anie
	dieOnError(uniPDFAddTimestamp(pdfBuf, pdfBuffLen, &newPdf[0], &newPdfSize, &tst[0], &tstSize));
	
	ofstream sigfile("./result_timestamped.pdf", ios_base::binary);
	sigfile.write((char*)&newPdf[0], newPdf.size());
	sigfile.close();

	ofstream tstfile("./result_timestamp.tsr", ios_base::binary);
	tstfile.write((char*)&tst[0], tst.size());
	tstfile.close();

}

int main()
{
	initJvm();

	for(int i=0; i<1; i++) {

		//testSignPdfOk();
		//testGetPKCS7InfoFromPdfOk();
		testAddTimestampToPDFOk();
	}
	
	return 0;
}
