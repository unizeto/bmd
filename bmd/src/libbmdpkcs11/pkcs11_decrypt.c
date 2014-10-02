#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>


/**
*Funkcja deszyfrująca dane zapisane w jednej strukturze i zapisuje je w drugiej
*@param *info struktura zawierająca wskaźniki na funkcje deszyfrujące oraz informacje o sesji połączenia
*@param *input wskaźnik na strukturę zawierającą zaszyfrowane dane
*@param **output wskaźnik na strukturę w której zostanie zapisany wynik operacji deszyfrowania
*@retval BMD_OK poprawne wykonanie funkcji
*@retval !=BMD_OK niepoprawne wykonanie funkcji
*/

long bmd_pkcs11_decrypt(pkcs11_crypto_info_t *info,GenBuf_t *input,GenBuf_t **output)
{
	//CK_MECHANISM mechanism = { CKM_RSA_PKCS, NULL_PTR, 0 };
	CK_RV rv=(CK_RV)NULL;;
	CK_FUNCTION_LIST_PTR pFunctionList=NULL;
	CK_ULONG length=0;
	long iter=0;
	char* tmp_buf = NULL;
	
	PRINT_GK("DBG \n");

	if(info==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info->pLibrary==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info->pFunctionList==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info->hSession==0)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info->hPrivateKey==0)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if(input==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(input->buf==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	if(output==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if((*output)!=NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	pFunctionList=(CK_FUNCTION_LIST_PTR)info->pFunctionList;

	//rv=pFunctionList->C_DecryptInit(info->hSession,&mechanism,info->hPrivateKey);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv = %lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	if(info->dontEstimateDecryptBufferSize != 0)
	{
		iter = 1;

		do
		{
			length = iter * (input->size) ;

			tmp_buf=(char *)calloc(length+2, sizeof(char));
			if(tmp_buf == NULL)
			{
				BMD_FOK(BMD_ERR_MEMORY);
			}
			
			//rv=pFunctionList->C_Decrypt(info->hSession, (unsigned char*)(input->buf), input->size, (unsigned char*)tmp_buf, &length);
			PRINT_GK("DBG input=%p, input_size = %li, output=%p, output_size=%lu\n", input->buf, input->size, tmp_buf, length);
			if(rv!=CKR_OK)
			{
				PRINT_GK("DBG rv = %lu\n", rv);
				PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				free(tmp_buf); tmp_buf = NULL;
				if(rv != CKR_BUFFER_TOO_SMALL) // jesli inny blad niz za maly bufor, to blad calej operacji
				{
					BMD_FOK(BMD_ERR_OP_FAILED);
				}
				//else {} // w przeciwnym razie zwiekszenie rozmiaru bufora wyjsciowego i ponowna proba zdeszyfrowania
			}
			else
			{
				PRINT_GK("DBG rv = %lu\n", rv);
				break;
			}
			iter++;
		}
		while(iter*input->size < 10000000) // bufor maksymalnie 10MB
		;

		if(rv!=CKR_OK)
		{
			PRINT_GK("DBG rv = %lu\n", rv);
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
		//rv=pFunctionList->C_Decrypt(info->hSession,(unsigned char*)input->buf,input->size,NULL,&length);
		PRINT_GK("DBG input=%p, input_size = %li, output=%p, output_size=%lu\n", input->buf, input->size, tmp_buf, length);
		if(rv!=CKR_OK)
		{
			PRINT_GK("DBG rv = %lu\n", rv);
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		
		tmp_buf=(char *)calloc(length+2, sizeof(char));
		if(tmp_buf == NULL)
		{
			BMD_FOK(BMD_ERR_MEMORY);
		}
		
		//rv=pFunctionList->C_Decrypt(info->hSession, (unsigned char*)(input->buf), input->size, (unsigned char*)tmp_buf, &length);
		PRINT_GK("DBG input=%p, input_size = %li, output=%p, output_size=%lu\n", input->buf, input->size, tmp_buf, length);
		if(rv!=CKR_OK)
		{
			free(tmp_buf); tmp_buf = NULL;
			PRINT_GK("DBG rv = %lu\n", rv);
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}

	(*output)=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
	if((*output) == NULL)
	{
		free(tmp_buf); tmp_buf = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}
	(*output)->buf = tmp_buf; tmp_buf = NULL;
	(*output)->size = length;

	return 0;
}
