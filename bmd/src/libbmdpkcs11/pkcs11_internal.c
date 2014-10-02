#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/common/bmd-os_specific.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <pkcs11/pkcs11t.h>


/**
*Funkcja za pomocą funkcji zawartych w strukturze wyznacza dostępne sloty kart oraz ich ilość
*@param pFunctionList lista funkcji umożliwiających wykonanie zadania
*@param *slots struktura, w której zostaną zapisane wskaźniki do znalezionych slotów 
*@param *count wskazanie na miejsce, gdzie ma zostać zapisana liczba dostępnych slotów
*@retval BMD_OK poprawne wykonanie funkcji 
*@retval !=BMD_OK niepoprawne wykonanie funkcji
*/
int _bmd_pkcs11_get_slots_list(CK_FUNCTION_LIST_PTR pFunctionList,CK_SLOT_ID_PTR *slots,unsigned long *count)
{
	int max_count=MAX_READERS;
	CK_RV rv=(CK_RV)NULL;;

	PRINT_GK("DBG \n");

	*count=MAX_READERS;

	if(pFunctionList==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(slots==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*slots)!=NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	(*slots)=(CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID)*max_count);
	if((*slots)==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	
	memset(*slots,0,max_count*sizeof(CK_SLOT_ID));
	//rv=pFunctionList->C_GetSlotList(TRUE,*slots,count);
	if(rv!=CKR_OK)
	{
		PRINT_GK("DBG rv=%lu\n", rv);
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	
	if((*count)==0)
	{
		PRINT_GK("DBG \n");
		free(*slots);
		*slots=NULL;
		
		/* sprawdzenie czy sa czytnik w systemie */
		//rv=pFunctionList->C_GetSlotList(FALSE,*slots,count);
		if(rv!=CKR_OK)
		{
			PRINT_GK("DBG rv=%lu\n", rv);
			PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		if(*count==0)
		{
			PRINT_GK("DBG \n");
			free(*slots);
			*slots=NULL;
			PRINT_DEBUG("LIBBMDPKCS11ERR No card device. Error=%i\n",BMD_ERR_NOTAVAIL);
			return BMD_ERR_NOTAVAIL; /* nie ma czytnikow */
		}
		PRINT_DEBUG("LIBBMDPKCS11ERR No data. Error=%i\n",BMD_ERR_NODATA);
		return BMD_ERR_NODATA;
	}

	return BMD_OK;
}

/** zwraca numer czytnika z listy czytnikow dla wybranej nazwy */
/**
*Funkcja wyznacza numer slota z losty slotów o podanej nazwie 
*@param pSlotList lista dostępnych slotów
*@param slots ilość dostępnych slotów 
*@param pFunctionList lista funkcji umożliwiających wykonanie zadania
*@param *readerName wskaźnik na nazwę poszukiwanego slota
*@param *number wskaźnik na miejsce w pamięci, do którego zostanie przypisany numer znalezionego slota
*@retval BMD_OK poprawne wykonanie funkcji 
*@retval !=BMD_OK niepoprawne wykonanie funkcji
*/
int _bmd_pkcs11_get_reader_number(CK_SLOT_ID_PTR pSlotList,CK_ULONG slots,
								  CK_FUNCTION_LIST_PTR pFunctionList,CK_ULONG slotId,int *number)
{
	unsigned int i;
	int flag=0;

	PRINT_GK("DBG \n");

	if(pSlotList==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if(pFunctionList==NULL)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	
 	for(i=0;i<slots;i++)
	{
		PRINT_GK("DBG \n");
		if(pSlotList[i] == slotId)
		{
			PRINT_GK("DBG \n");
			flag=1;
			break;
		}
	}
	if(flag!=1)
	{
		PRINT_GK("DBG \n");
		PRINT_DEBUG("LIBBMDPKCS11ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	*number=i;

	return BMD_OK;
}

