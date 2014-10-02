#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>

/* FIXME: dodac mozliwosc uzywania dluzszych kluczy i ogolnie sprawdzic te dane */
long bmd_pkcs11_sign(pkcs11_crypto_info_t *info,GenBuf_t *input,GenBuf_t **output)
{
//CK_MECHANISM mechanism = { CKM_RSA_PKCS, NULL_PTR, 0 };
	
CK_RV rv=(CK_RV)NULL;
CK_FUNCTION_LIST_PTR pFunctionList=NULL;
CK_ULONG length=0;

	PRINT_GK("DBG \n");

	pFunctionList=(CK_FUNCTION_LIST_PTR)info->pFunctionList;


	/*
	jesli wlaczona jest kompatybilnosc z CryptoTech'em (Sigillum/Kir), czyli zapamietywany jest pin za pomoca
	funkcji bmd_pkcs11_remember_pin(), wowczas przed kazdym podpisywaniem nalezy wylogowac sie i
	ponownie zalogowac do tokenu (w przeciwnym razie TYLKO pierwsze wywolanie bmd_pkcs11_sign()
	zakonczy sie powodzeniem)
	Kompatybilnosc z CryptoTech'em pozwala rowniez dzialac na innych bibliotekach PKCS#11, a roznica bedzie
	widoczna w czasie wykonywania operacji (wielokrotne wylogowanie i ponowne logowanie do tokenu wydluzy czas)
	(by WSZ)
	*/
	if(info->pin != NULL)
	{
		if(info->loggedIn != 0)
		{
		//	rv = pFunctionList->C_Logout(info->hSession);
			info->loggedIn = 0;
		}
		//rv=pFunctionList->C_Login(info->hSession, CKU_USER, (CK_UTF8CHAR_PTR)(info->pin), info->pinLen);
		if(rv != CKR_OK)
		{
			PRINT_GK("DBG rv = %lu\n", rv);
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",-4);
			return -4;
		}
		info->loggedIn = 1;
	}

	//rv=pFunctionList->C_SignInit(info->hSession,&mechanism,info->hPrivateKey);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv = %lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",-1);
		return -1;
	}
	// tu sie wysekfalcil przy logowaniu (logowanie z karty play i linia polecen bez pinu)
	//rv=pFunctionList->C_Sign(info->hSession, (unsigned char*)input->buf,input->size,NULL,&length);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv = %lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",-2);
		return -2;
	}

	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	(*output)->buf=(char *)malloc(length+2);
	memset((*output)->buf,0,length+1);

	//rv=pFunctionList->C_Sign(info->hSession, (unsigned char*)input->buf, input->size, (unsigned char*)(*output)->buf, &length);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv = %lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",-3);
		return -3;
	}

	(*output)->size=length;

	return 0;
}

