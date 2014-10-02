#include <bmd/libbmdpkcs11/libbmdpkcs11_internal.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/bmd_ks/bmd_ks_ds.h>

#ifdef WIN32
#pragma warning(disable:4221)
#pragma warning(disable:4204)
#endif

/** szuka klucza prywatnego o danym CKA_ID - umieszcza jego uchwyt w privKey 
Tak szybko, jak to mozliwe, funkcja czysci pin i zwalnia po nim pamiec 
(nalezy przekazywac &pin, gdzie pin jest char*, bo na wyjsciu bedzie wyNULLowany
*/
long bmd_pkcs11_get_privkey(pkcs11_cert_t *cert_info,char **pin,long pin_length,pkcs11_crypto_info_t *info)
{
CK_SLOT_ID_PTR pSlotList=NULL;
CK_RV rv=(CK_RV)NULL;
CK_ULONG slots=MAX_READERS;
CK_FLAGS flags=CKF_SERIAL_SESSION;
CK_ULONG privatekeysCount=0;
CK_BYTE myTrue=TRUE;
CK_OBJECT_CLASS PrivateKeyClass=CKO_PRIVATE_KEY;
/* do wyszukania klucza prywatnego o danym id */
CK_ATTRIBUTE privatekeyTemplate[]={
				{CKA_CLASS,&PrivateKeyClass,sizeof(PrivateKeyClass)},
				{CKA_TOKEN,&myTrue,sizeof(myTrue)},
				{CKA_ID,NULL_PTR,0}
			};
CK_FUNCTION_LIST_PTR pFunctionList=NULL;
long reader_number=0;
long status=0;
long sl_temp=0;
CK_SESSION_HANDLE handle_temp;
CK_OBJECT_HANDLE object_handle_temp;

	PRINT_GK("DBG \n");
	/*
	to sprawdzenie musi byc wykonane jako pierwsze, bo niezaleznie od przebiegu funkcji
	pin musi byc memestowany i zwalniany na wyjsciu z funkcji
	*/
	if(pin == NULL)
	{
		PRINT_GK("DBG \n");
		return BMD_ERR_PARAM2;
	}
	if(cert_info==NULL)
	{
		PRINT_GK("DBG \n");
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(cert_info->cka_id==NULL)
	{
		PRINT_GK("DBG \n");
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(info==NULL)
	{
		PRINT_GK("DBG \n");
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

	pFunctionList=(CK_FUNCTION_LIST_PTR)info->pFunctionList;

	sl_temp = slots;
//	status=_bmd_pkcs11_get_slots_list(pFunctionList,&pSlotList, &sl_temp);
	slots = sl_temp;
	if(status!=0)
	{
		PRINT_GK("DBG \n");
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Error in getting slot list. Error=%li\n",status);
		return status;
	}

	//status=_bmd_pkcs11_get_reader_number(pSlotList,slots,pFunctionList,cert_info->slotID,&reader_number);
	if(status!=BMD_OK)
	{
		PRINT_GK("DBG \n");
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/*printf("Otwieram sesje z numerem %i ...\n",reader_number);*/
	/* otworz sesje z czytnikiem i pobierz CKA_ID certyfikatu okreslonego przez object->label */
	handle_temp = (info->hSession);
	rv=pFunctionList->C_OpenSession(pSlotList[reader_number],flags,NULL_PTR,NULL_PTR, &handle_temp);
	(info->hSession) = handle_temp;
	
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
		free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	rv=pFunctionList->C_Login(info->hSession,CKU_USER,(CK_UTF8CHAR_PTR)(*pin),pin_length);
	memset(*pin, 0, pin_length); /*Wymemsetowanie pin.*/
	free(*pin); *pin = NULL; /*Usuniecie wskazania na PIN.*/
	if( (rv==CKR_OK) || (rv==CKR_USER_ALREADY_LOGGED_IN) )
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		info->loggedIn = 1;
		/* ustal szablon i wyszukaj klucz prywatny odpowiadajacy takiemu CKA_ID */
		privatekeyTemplate[2].ulValueLen=cert_info->cka_id->size;
		privatekeyTemplate[2].pValue=cert_info->cka_id->buf;
	
		rv=pFunctionList->C_FindObjectsInit(info->hSession,privatekeyTemplate,3);
		if(rv==CKR_OK)
		{
			PRINT_GK("DBG rv=%lu\n", rv);
			object_handle_temp = info->hPrivateKey;
			rv=pFunctionList->C_FindObjects(info->hSession,&object_handle_temp,1,&privatekeysCount);
			PRINT_GK("DBG rv=%lu\n", rv);
			info->hPrivateKey = object_handle_temp;
			if(privatekeysCount==0)
			{
				PRINT_GK("DBG \n");
				rv=pFunctionList->C_FindObjectsFinal(info->hSession);
				if(rv!=CKR_OK)
				{
					PRINT_GK("DBG rv=%lu\n", rv);
					PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					return BMD_ERR_OP_FAILED;
				}
				PRINT_DEBUG("LIBBMDPKCS11ERR No data. Error=%i\n",BMD_ERR_NODATA);
				return BMD_ERR_NODATA;
			}
			else
			{
				PRINT_GK("DBG \n");
				rv=pFunctionList->C_FindObjectsFinal(info->hSession);
				if(rv!=CKR_OK)
				{
					PRINT_GK("DBG rv=%lu\n", rv);
					PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					return BMD_ERR_OP_FAILED;
				}
			}
		}
		else
		{
			PRINT_GK("DBG rv=%lu\n", rv);
			rv=pFunctionList->C_FindObjectsFinal(info->hSession);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG \n");
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			PRINT_DEBUG("LIBBMDPKCS11ERR No data. Error=%i\n",BMD_ERR_NODATA);
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		if(rv==CKR_PIN_INCORRECT)
		{
			PRINT_GK("DBG \n");
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_CREDENTIALS);
			return BMD_ERR_CREDENTIALS;
		}
		else
		{
			PRINT_GK("DBG \n");
			if(rv == CKR_PIN_LOCKED)
			{
				PRINT_GK("DBG \n");
				return BMD_PKCS11_PIN_LOCKED;
			}
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}
	free(pSlotList);pSlotList=NULL;
	
	set_gen_buf2(cert_info->gb_cert->buf,cert_info->gb_cert->size,&(info->cert));

	return BMD_OK;
}

/**
Funckja bmd_pkcs11_remember_pin() zapamietuje pin i jego dlugosc w skladowej pkcs11 kontekstu podpisujacego
*/
long bmd_pkcs11_remember_pin(pkcs11_crypto_info_t *pkcs11Info, char *pin, long pinLen)
{
	PRINT_GK("DBG \n");

	if(pkcs11Info == NULL)
	{
		PRINT_GK("DBG \n");
		return -1;
	}
	
	bmd_pkcs11_clear_pin(pkcs11Info); /*gdy wczesniej byl ustawiony pin, to zostanie zwolniony przed kolejnym zapamietaniem*/

	pkcs11Info->pin=(char*)calloc(pinLen, sizeof(char));
	if(pkcs11Info->pin == NULL)
	{
		PRINT_GK("DBG \n");
		return -2;
	}
	memcpy(pkcs11Info->pin, pin, pinLen);
	pkcs11Info->pinLen=pinLen;

	return 0;
}

/**
Funckja bmd_pkcs11_clear_pin() czysci pin i jego dlugosc w skladowej pkcs11 kontekstu podpisujacego
*/
long bmd_pkcs11_clear_pin(pkcs11_crypto_info_t *pkcs11Info)
{
	PRINT_GK("DBG \n");

	if(pkcs11Info == NULL)
	{
		PRINT_GK("DBG \n");
		return -1;
	}

	if(pkcs11Info->pin != NULL)
	{
		PRINT_GK("DBG \n");
		memset(pkcs11Info->pin, 0, pkcs11Info->pinLen);
	}
	free(pkcs11Info->pin);
	pkcs11Info->pin=NULL;
	pkcs11Info->pinLen=0;

	return 0;
}
