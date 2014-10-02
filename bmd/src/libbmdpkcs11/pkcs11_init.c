#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>
#include <bmd/bmd_ks/bmd_ks_ds.h>


/**
*Funkcja pobiera listę funkcji zawartych w bibliotece i przypisuje ją do struktury
*@param *pkcs11_library lokalizacja bilioteki zawierającej funkcje
*@param *info struktura w której zostanie zapisana lista funkcji odtsępnych w bibliotece
*@retval BMD_OK poprawne wykonanie funkcji
*@retval !=BMD_OK niepoprawne wykonanie funkcji
*/

long bmd_pkcs11_init(const char *pkcs11_library,pkcs11_crypto_info_t *info)
{
	CK_C_GetFunctionList pC_GetFunctionList = NULL_PTR;
	CK_RV rv;
#ifdef WIN32
	wchar_t *wideFilename=NULL;
#endif
	
	PRINT_GK("DBG \n");
	if(pkcs11_library==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
#ifdef WIN32
	PRINT_GK("DBG \n");
	UTF8StringToWindowsString((char*)pkcs11_library, &wideFilename, NULL);
	info->pLibrary=dlopen(wideFilename, RTLD_LAZY);
	free(wideFilename); wideFilename=NULL;
#else
	PRINT_GK("DBG \n");
	info->pLibrary=dlopen(pkcs11_library, RTLD_LAZY);
#endif
	if(info->pLibrary==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}
	
	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(info->pLibrary,"C_GetFunctionList");
	if(pC_GetFunctionList==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	
	rv=(pC_GetFunctionList)((CK_FUNCTION_LIST_PTR *)(&(info->pFunctionList)));
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	rv=((CK_FUNCTION_LIST_PTR)info->pFunctionList)->C_Initialize(NULL_PTR);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	return 0;
}

