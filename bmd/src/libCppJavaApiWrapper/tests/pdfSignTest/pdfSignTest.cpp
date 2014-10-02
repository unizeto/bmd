#include <CppJavaApiWrapper.h>
#include <bmd/libbmdutils/libbmdutils.h>


GenBuf_t *_lGLOBAL_signingCert = NULL;
GenBuf_t *_lGLOBAL_pdfForSigning = NULL;

long initJvm()
{
long retVal	= 0;

	// dodatkowe parametry pracy maszyny wirtualnej Java
	const char* opts[][2] = {
		{"-Xmx", "32m"},
	//	{"-verbose:jni", NULL}
		{"-Dlog4j.configuration=", "log4j.properties"},
	};
	const int paramCount = 2;

	const char* javaDir="C:\\Program Files\\Java\\jre6";
	
	printf("JVM init...\n");
		
	retVal = proCertumJavaVmInit("../../../CppJavaAPIWrapperLibs", javaDir, opts, paramCount);
	if(retVal != JAVACALL_OK)
	{
		printf("%s", GetDetailedErrorMsg());
		return -1;
	}
	else
	{
		printf("OK : JVM initialized\n");
	}
	
	return 0;
}

/*
wczytanie certyfikatu podpisujacego i pdf do podpisania; ustawienie sterownika pkcs#11, pinu i ew. slotId
*/
long prepareForSigning()
{
long retVal	= 0;

	printf("Preparing for signing... \n");

	retVal = bmd_load_binary_content("C:\\Documents and Settings\\wlodzimierzszczygiel\\Pulpit\\certyfikaty\\kwalifikowany_testowy.cer", &_lGLOBAL_signingCert); // CERT
	//retVal = bmd_load_binary_content("C:\\Documents and Settings\\wlodzimierzszczygiel\\Pulpit\\certyfikaty\\korporacyjny_level4.cer", &_lGLOBAL_signingCert); // CERT
	if(retVal < 0)
	{
		printf("ERROR: bmd_load_binary_content(cert) : %li\n", retVal);
		return -11;
	}

	//retVal = bmd_load_binary_content("../../resources/sample.pdf", &_lGLOBAL_pdfForSigning); // PDF
	retVal = bmd_load_binary_content("C:\\faktury_testowe\\F_00324276_06_09.pdf", &_lGLOBAL_pdfForSigning); // PDF
	if(retVal < 0)
	{
		free_gen_buf(&_lGLOBAL_signingCert);
		printf("ERROR: bmd_load_binary_content(pdf) : %li\n", retVal);
		return -12;
	}


	retVal=uniPKCS11SetDriver("C:\\PKCS11\\cryptocertumpkcs11.dll"); // PKCS#11
	if(retVal != JAVACALL_OK)
	{
		free_gen_buf(&_lGLOBAL_signingCert);
		free_gen_buf(&_lGLOBAL_pdfForSigning);
		printf("%s", GetDetailedErrorMsg());
		return -1;
	}

	retVal=uniPKCS11SetPin("12345678"); // PIN
	if(retVal != JAVACALL_OK)
	{
		free_gen_buf(&_lGLOBAL_signingCert);
		free_gen_buf(&_lGLOBAL_pdfForSigning);
		printf("%s", GetDetailedErrorMsg());
		return -2;
	}

	/*
	retVal = uniPKCS11SetSlotId(1); //!!!!!
	if(retVal != JAVACALL_OK)
	{
		free_gen_buf(&_lGLOBAL_signingCert);
		free_gen_buf(&_lGLOBAL_pdfForSigning);
		printf("%s", GetDetailedErrorMsg());
		return -3;
	}
	*/

	printf("OK, prepared\n");

	return 0;
}

long signFile()
{
long retVal	= 0;
long signedPdfSize	= 0;
unsigned char*signedPdfBuf	= NULL;

	retVal = uniPDFSignQualified((const unsigned char*)_lGLOBAL_pdfForSigning->buf, _lGLOBAL_pdfForSigning->size, (const unsigned char*)_lGLOBAL_signingCert->buf, _lGLOBAL_signingCert->size, NULL, (unsigned int*)&signedPdfSize);
	if(retVal != JAVACALL_OK)
	{
		printf("%s", GetDetailedErrorMsg());
		return -1;
	}

	signedPdfBuf=(unsigned char*)calloc(signedPdfSize, sizeof(unsigned char));
	if(signedPdfBuf == NULL)
	{
		printf("ERROR calloc\n");
		return -2;
	}

	retVal = uniPDFSignQualified((const unsigned char*)_lGLOBAL_pdfForSigning->buf, _lGLOBAL_pdfForSigning->size, (const unsigned char*)_lGLOBAL_signingCert->buf, _lGLOBAL_signingCert->size, signedPdfBuf, (unsigned int*)&signedPdfSize);
	free(signedPdfBuf); signedPdfBuf=NULL;
	signedPdfSize = 0;
	if(retVal != JAVACALL_OK)
	{
		printf("%s", GetDetailedErrorMsg());
		return -3;
	}

	return 0;
}

long signfileAndGetPKCS7()
{
long retVal	= 0;
long signedPdfSize	= 0;
unsigned char*signedPdfBuf	= NULL;

	retVal = uniPDFSignQualified((const unsigned char*)_lGLOBAL_pdfForSigning->buf, _lGLOBAL_pdfForSigning->size, (const unsigned char*)_lGLOBAL_signingCert->buf, _lGLOBAL_signingCert->size, NULL, (unsigned int*)&signedPdfSize);
	if(retVal != JAVACALL_OK)
	{
		printf("%s", GetDetailedErrorMsg());
		return -1;
	}

	signedPdfBuf=(unsigned char*)calloc(signedPdfSize, sizeof(unsigned char));
	if(signedPdfBuf == NULL)
	{
		printf("ERROR calloc\n");
		return -2;
	}

	retVal = uniPDFSignQualified((const unsigned char*)_lGLOBAL_pdfForSigning->buf, _lGLOBAL_pdfForSigning->size, (const unsigned char*)_lGLOBAL_signingCert->buf, _lGLOBAL_signingCert->size, signedPdfBuf, (unsigned int*)&signedPdfSize);
	if(retVal != JAVACALL_OK)
	{
		free(signedPdfBuf); signedPdfBuf=NULL;
		signedPdfSize = 0;
		printf("%s", GetDetailedErrorMsg());
		return -3;
	}

	// wyciaganie PKCS#7
	PKCS7Info* pkcs7 = uniPDFAllocPKCS7Info();
	if(pkcs7 == NULL)
	{
		printf("%s", GetDetailedErrorMsg());
		free(signedPdfBuf); signedPdfBuf=NULL;
		signedPdfSize = 0;
		return -11;
	}

	retVal = uniPDFGetPKCS7Info(signedPdfBuf, signedPdfSize, pkcs7);
	free(signedPdfBuf); signedPdfBuf=NULL;
	signedPdfSize = 0;
	uniPDFFreePKCS7Info(pkcs7); pkcs7=NULL;
	if(retVal != JAVACALL_OK)
	{
		printf("%s", GetDetailedErrorMsg());
		return -12;
	}

	return 0;
}


long cleanUp()
{
	free_gen_buf(&_lGLOBAL_pdfForSigning);
	free_gen_buf(&_lGLOBAL_signingCert);

	return 0;
}


long main()
{
long retVal	= 0;
long iter = 0;

	retVal = initJvm();
	if(retVal < 0)
	{
		printf("ERROR: initJvm() : %li\n", retVal);
		return -1;
	}

	retVal = prepareForSigning();
	if(retVal < 0)
	{
		printf("ERROR: prepareForSigning() : %li\n", retVal);
		return -2;
	}

	for(iter = 0; iter< 100; iter++)
	{
		//retVal = signFile();
		retVal = signfileAndGetPKCS7();
		if(retVal < 0)
		{
			printf("ERROR: signFile() : %li\nBREAK\n", retVal);
			break;
		}
		else
		{
			printf("OK, signed nr %li\n", iter);
		}
	}

	retVal = cleanUp();

	return 0;
}