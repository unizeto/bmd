#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include<bmd/libbmdpkcs11/libbmdpkcs11_internal.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>

#define MAX_CERTIFICATES 100

#ifdef WIN32
#pragma warning(disable:4221)
#pragma warning(disable:4204)
#pragma warning(disable:4127)
#endif

/**
*Funkcja pobiera listę certyfikatów zapisanych na karcie odczytanej za pomocą slotów
*@param *info struktura zawieająca informacje na temat funkcji operujących na danych z karty kryptograficznej
*@param ***cert_list struktura do zapisu pobranej listy certyfikatów zapisanych na karcie kryptograficznej
*@retval BMD_OK poprawne wykonanie funkcji
*@retval !=BMD_OK nie poprawne wykonanie funkcji
*@retval BMD_PKCS11_NO_CERTS na znalezionych kartach brak certyfikatow ktore moznaby bylo odczytac za pomoca uzytej biblioteki PKCS11
*@retval BMD_PKCS11_NO_CARDS nie znaleziono kart w czytnikach obslugiwanych przez uzywana biblioteke
*/

long bmd_pkcs11_get_all_certs(pkcs11_crypto_info_t *info,pkcs11_cert_t ***cert_list)
{
long i,j;
CK_RV rv=CKR_OK;
CK_ULONG slots=MAX_READERS;
CK_SLOT_ID_PTR pSlotList=NULL;
//CK_SESSION_HANDLE hSession;
//CK_FLAGS flags=CKF_SERIAL_SESSION;
//CK_OBJECT_HANDLE hCertificates[MAX_CERTIFICATES];

CK_ULONG certificatesCount=0;
//CK_OBJECT_CLASS CertificateClass=CKO_CERTIFICATE;
//CK_BYTE myTrue=TRUE;
	
long no_of_certs=0;
long status=0;
char *tmp_cert_value=NULL;
char *tmp_cka_id_value=NULL;
long tmp_cert_length=-1;
long tmp_cka_id_length=-1;

long current_cert_index=0;
long total_no_of_certs=0;
long sl_temp		= 0;

CK_FUNCTION_LIST_PTR pFunctionList;

/*CK_ATTRIBUTE certificateTemplate[]={ 
				{CKA_CLASS,&CertificateClass,sizeof(CertificateClass)},
				{CKA_TOKEN,&myTrue,sizeof(myTrue)},
			};*/

CK_ATTRIBUTE certValueCkaIdTemplate[]={
					{CKA_VALUE,NULL_PTR,0},
					{CKA_ID,NULL_PTR,0}
				};

	PRINT_GK("DBG \n");

pFunctionList=(CK_FUNCTION_LIST_PTR)info->pFunctionList;

	sl_temp = slots;	
	//status=_bmd_pkcs11_get_slots_list(pFunctionList,&pSlotList,&sl_temp);
	slots = sl_temp;
	switch (status)
	{
	case BMD_OK:
		PRINT_GK("DBG \n");
		break;
	case BMD_ERR_NOTAVAIL:
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Error in getting slot list. Error=%li\n",status);
//		return BMD_PKCS11_NO_CARDS;
	case BMD_ERR_NODATA:
		PRINT_GK("DBG \n");
		return BMD_PKCS11_NO_CERTS;
	default:
		PRINT_GK("DBG \n");
		return status;
	}

	if(rv==CKR_OK)
	{	
		PRINT_GK("DBG \n");
		for(i=0;i<(long)slots;i++) /* dla kazdego czytnika */
		{
			PRINT_GK("DBG \n");
//			rv=pFunctionList->C_OpenSession(pSlotList[i],flags,NULL_PTR,NULL_PTR,&hSession);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG rv=%lu\n", rv);
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				continue;
				//return BMD_ERR_OP_FAILED;
			}
	//		rv=pFunctionList->C_FindObjectsInit(hSession,certificateTemplate,2);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG rv=%lu\n", rv);
	//			rv=pFunctionList->C_CloseSession(hSession);
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			while(1)
			{
				PRINT_GK("DBG \n");
	//			rv=pFunctionList->C_FindObjects(hSession,&hCertificates[no_of_certs],1,&certificatesCount);
				if(rv!=CKR_OK)
				{
					PRINT_GK("DBG rv=%lu\n", rv);
	//				rv=pFunctionList->C_CloseSession(hSession);
					PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					return BMD_ERR_OP_FAILED;
				}
				if(certificatesCount==0)
				{
					PRINT_GK("DBG \n");
					break;
				}
				no_of_certs++;
				total_no_of_certs++;
			}
	//		rv=pFunctionList->C_FindObjectsFinal(hSession);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG rv=%lu\n", rv);
	//			rv=pFunctionList->C_CloseSession(hSession);
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			
			for(j=0; j < no_of_certs; j++)
			{
				PRINT_GK("DBG \n");
				certValueCkaIdTemplate[0].pValue=NULL;
				certValueCkaIdTemplate[1].pValue=NULL;
				certValueCkaIdTemplate[0].ulValueLen=0;
				certValueCkaIdTemplate[1].ulValueLen=0;
				/* pobierz wartosc certyfikatu oraz cka_id */
	//			rv=pFunctionList->C_GetAttributeValue(hSession,hCertificates[j],certValueCkaIdTemplate,2);
				
				if(rv==CKR_OK)
				{
					PRINT_GK("DBG \n");
					tmp_cert_length=certValueCkaIdTemplate[0].ulValueLen;
					tmp_cert_value=(char *)malloc( (tmp_cert_length + 1) * sizeof(char));
					memset(tmp_cert_value,0, (tmp_cert_length + 1) * sizeof(char));
					certValueCkaIdTemplate[0].ulValueLen=tmp_cert_length,
					certValueCkaIdTemplate[0].pValue=tmp_cert_value;
						
					tmp_cka_id_length=certValueCkaIdTemplate[1].ulValueLen;
					tmp_cka_id_value=(char *)malloc( (tmp_cka_id_length + 1) * sizeof(char));
					memset(tmp_cka_id_value,0, (tmp_cka_id_length + 1) * sizeof(char));
					certValueCkaIdTemplate[1].ulValueLen=tmp_cka_id_length;
					certValueCkaIdTemplate[1].pValue=tmp_cka_id_value;

		//			rv=pFunctionList->C_GetAttributeValue(hSession,hCertificates[j],certValueCkaIdTemplate,2);
					if(rv==CKR_OK)
					{
						PRINT_GK("DBG rv=%lu\n", rv);
						(*cert_list)=(pkcs11_cert_t **)realloc(*cert_list, 
						sizeof(pkcs11_cert_t *) * (current_cert_index + 2));
						(*cert_list)[current_cert_index + 1]=NULL;
						(*cert_list)[current_cert_index]=(pkcs11_cert_t *) 
						malloc(sizeof(pkcs11_cert_t));
						memset((*cert_list)[current_cert_index],0,sizeof(pkcs11_cert_t));

						/* ustaw w strukturze wynikowej certyfikat */
						set_gen_buf2(tmp_cert_value,tmp_cert_length, 
						&( ( (*cert_list)[current_cert_index])->gb_cert));

						/* ustaw w strukturze wynikowej cka_id */
						set_gen_buf2(tmp_cka_id_value,tmp_cka_id_length,
						&( ( (*cert_list)[current_cert_index])->cka_id));

						( (*cert_list)[current_cert_index])->slotID = pSlotList[i];						
					}
					free(tmp_cert_value);tmp_cert_value=NULL;
					free(tmp_cka_id_value);tmp_cka_id_value=NULL;
				}
				else
				{
					PRINT_GK("DBG \n");
		//			rv=pFunctionList->C_CloseSession(hSession);
					PRINT_GK("DBG rv=%lu\n", rv);
					PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					return BMD_ERR_OP_FAILED;
				}
				current_cert_index++;
			} /* FOR ALL certs */
	//		rv=pFunctionList->C_CloseSession(hSession);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG rv=%lu\n", rv);
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			no_of_certs=0;
		} /* FOR ALL slots */
	} /* GetSlotList OK */

	free(pSlotList); pSlotList=NULL;
	if (total_no_of_certs)
	{
		PRINT_GK("DBG \n");
		return BMD_OK;
	}
	return BMD_PKCS11_NO_CERTS;
}

