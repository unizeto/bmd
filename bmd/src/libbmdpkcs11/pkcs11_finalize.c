#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>


/* koniec biblioteki */
long bmd_pkcs11_finalize(pkcs11_crypto_info_t *pkcs11Info)
{
	CK_RV rv=(CK_RV)NULL;
	long status=0;

	PRINT_GK("DBG \n");
	if(pkcs11Info == NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(pkcs11Info->pLibrary == NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM2;
	}
	if(pkcs11Info->pFunctionList == NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM3;
	}
	
	if(pkcs11Info->loggedIn != 0)
	{
		PRINT_GK("DBG \n");
//		rv = ((CK_FUNCTION_LIST_PTR)(pkcs11Info->pFunctionList))->C_Logout(pkcs11Info->hSession);
		PRINT_GK("DBG rv=%lu\n", rv);
		pkcs11Info->loggedIn = 0;
	}

	if(pkcs11Info->hSession != 0)
	{
		PRINT_GK("DBG \n");
//		rv = ((CK_FUNCTION_LIST_PTR)(pkcs11Info->pFunctionList))->C_CloseSession(pkcs11Info->hSession);
		PRINT_GK("DBG rv=%lu\n", rv);
		pkcs11Info->hSession = 0;
	}

//	rv=((CK_FUNCTION_LIST_PTR)(pkcs11Info->pFunctionList))->C_Finalize(NULL_PTR);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=dlclose(pkcs11Info->pLibrary);
	if(status!=0)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	pkcs11Info->pLibrary = NULL;

	return BMD_OK;
}

