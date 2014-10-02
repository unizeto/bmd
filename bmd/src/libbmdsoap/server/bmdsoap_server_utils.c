#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdutils/libbmdutils2.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

char *CSVEscape(bmd_oid_xpath_t * ctx, char * value, char ValType);

/******************************************/
/*	wyszukiwanie plikow w archiwum	*/
/******************************************/
long sendRequestToBMD(	bmd_info_t *bi,
			bmdDatagramSet_t *req_dtg_set,
			long twf_max_datagram_in_set_transmission,
			bmdDatagramSet_t **resp_dtg_set,
			long twf_free_datagrams_flag,
			long deserialisation_max_memory)
{
	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	BMD_FOK(bmd_send_request(	bi,
					req_dtg_set,
					twf_max_datagram_in_set_transmission,
					resp_dtg_set,
					twf_free_datagrams_flag,
					deserialisation_max_memory));
	return (*resp_dtg_set)->bmdDatagramSetTable[0]->datagramStatus;

}

/******************************/
/*	stworzenie datagramu	*/
/******************************/
long createDatagram(	char *userClassId,
			char **oids_table,
			char **values_table,
			long oids_count,
			long datagramType,
			long owner_status,
			long sql_operator,
			bmdDatagramSet_t **dtg_set)
{
long i			= 0;
bmdDatagram_t *dtg	= NULL;

	BMD_FOK_NP(bmd_datagram_create(	datagramType,&dtg));
	if (userClassId!=NULL)
	{
		BMD_FOK_NP(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	BMD_FOK_NP(bmd_datagram_set_status(owner_status, sql_operator,&dtg));

	if (datagramType==BMD_DATAGRAM_TYPE_SEARCH)
	{
		BMD_FOK(bmd_datagram_add_limits(0,10000,&dtg));
	}

	for (i=0; i<oids_count; i++)
	{
		if ((oids_table[i]!=NULL) && (values_table[i]!=NULL))
		{
			BMD_FOK(bmd_datagram_add_metadata_char(	oids_table[i], values_table[i], &dtg));
		}
	}

	BMD_FOK(bmd_datagram_add_to_set(dtg,dtg_set));

	return BMD_OK;
}

/************************************************/
/*	sprawdzenie, czy string zawiera podstring	*/
/************************************************/
long isSubstring(	char *string,
			char *substring)
{
long re			= -1;
int erroroffset		= 0;
const char *error		= NULL;
char *regexp		= NULL;
pcre *rc			= NULL;
int ovector[30];

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (string==NULL)
	{
		return -1;
	}
	if (substring==NULL)
	{
		return -2;
	}

	/************************************************/
	/*	walidacja przy pomocy wyrazen regularnych	*/
	/************************************************/
	asprintf(&regexp,"(.)*%s(.)*",substring);
	rc=pcre_compile(regexp, PCRE_MULTILINE|PCRE_UTF8, &error,&erroroffset,NULL);
	if(rc==NULL)
	{
		return BMD_ERR_OP_FAILED;
	}

	re = pcre_exec(rc,NULL,(const char *)string,strlen(string),0,0,ovector,30);
	if(re<0)
	{
		return -1;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	free(regexp);regexp=NULL;
	pcre_free(rc);rc=NULL;

	return BMD_OK;
}

/************************/
/*	skopiowanie pliku	*/
/************************/
long fileCopy(	char *source,
			char *destination)
{
GenBuf_t *gb	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (source==NULL)
	{
		return -1;
	}
	if (destination==NULL)
	{
		return -2;
	}

	bmd_load_binary_content(source,&gb);
	bmd_save_buf(gb, destination);

	/******************/
	/*	porzadki	*/
	/******************/
	free_gen_buf(&gb);

	return BMD_OK;
}

/******************************/
/*	przesuniecie pliku	*/
/******************************/
long fileMove(	char *source,
			char *destination)
{
GenBuf_t *gb	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (source==NULL)
	{
		return -1;
	}
	if (destination==NULL)
	{
		return -2;
	}

	bmd_load_binary_content(source,&gb);
	bmd_save_buf(gb, destination);

	/******************/
	/*	porzadki	*/
	/******************/
	free_gen_buf(&gb);
	unlink(source);

	return BMD_OK;
}

/******************************/
/*	delete temporary files	*/
/******************************/
long deleteTempFiles(	char **zip_list,
				long zip_list_size)
{
long i		= 0;
long status	= 0;
	for (i=0; i<zip_list_size; i++)
	{
		status=remove(zip_list[i]);
		if (status<BMD_OK)
		{
			PRINT_ERROR("SOAPSERVERERR Error in deleting file %s\n",zip_list[i]);
		}
	}

	return BMD_OK;
}

/******************************************************************/
/*	konwersja listy metadanych z formatu uzywanego w xml'u	*/
/******************************************************************/
long ConvertMtdsList(	struct bmd230__mtdsValues *mtds,
				char ***oids_table,
				char ***values_table,
				long *oids_count)
{
long max_metadata_value_length	= 0;
int i						= 0;

	if(mtds == NULL)
		{ return BMD_ERR_PARAM1; }
	if(oids_table == NULL)
		{ return BMD_ERR_PARAM2; }
	if(values_table == NULL)
		{ return BMD_ERR_PARAM3; }
	if(oids_count == NULL)
		{ return BMD_ERR_PARAM4; }

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	/************************************************/
	/*	alokacja pamieci na listy metadanych	*/
	/************************************************/
	(*oids_table)=(char **)calloc(mtds->__size, sizeof(char*));
	(*values_table)=(char **)calloc(mtds->__size, sizeof(char*));
	(*oids_count)=mtds->__size;

	if(*oids_table == NULL || *values_table == NULL)
		{ return BMD_ERR_MEMORY; }
	
	
	for (i=0; i<mtds->__size; i++)
	{
		if(mtds->__ptr[i].mtdOid==NULL)
		{
			PRINT_ERROR("BMDSOAPERR Unspecified OID for additional metadata\n");
			return INVALID_ADDITIONAL_METADATA_OID_FORMAT;
		}
		
		if(mtds->__ptr[i].mtdValue==NULL)
		{
			PRINT_ERROR("BMDSOAPERR Unspecified value for additional metadata with OID <%s>\n", mtds->__ptr[i].mtdOid);
			return BMD_VERIFY_REQUEST_INVALID_ADD_METADATA;
		}
		
		if (strlen(mtds->__ptr[i].mtdValue)>max_metadata_value_length)
		{
			return BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG;
		}
		
		asprintf(&((*oids_table)[i]), "%s", mtds->__ptr[i].mtdOid);
		asprintf(&((*values_table)[i]), "%s", mtds->__ptr[i].mtdValue);
		if((*oids_table)[i] == NULL || (*values_table)[i] == NULL)
		{
			return BMD_ERR_MEMORY;
		}
	}

	return BMD_OK;
}


/**
funkcja FillResponseWithMetadata na podstawie datagramu odpowiedzi wypelnia metadane
w odpowiedzi SOAP (by WSZ)

@param soapStruct to struktura sopa, na ktorej podstawie alokowane sa metadane
@param responseDatagram to datagram odpowiedzi, na ktorego podstawie wypelniane sa metadane
@param formMtds to struktura, ktora informuje, jakie metadane (oidy) maja znalezc sie w odpowiedzi (moze byc NULL)
@param includeSYSMetadataFlag (0|1) okresla, czy maja byc ustawiane metadane systemowe (priorytet wyzszy od formularza)
@param includeADDMetadataFlag (0|1) okresla, czy maja byc ustawiane metadane dodatkowe (priorytet wyzszy od formularza)
@param includeACTIONMetadataFlag (0|1) okresla, czy maja byc ustawiane metadane akcji (priorytet wyzszy od formularza)
@param responseMetadata alokowana struktura z ustawionymi metadanymi

@retval 0 jesli sukces
@retval <0 w przypadku bledu

*/
long FillResponseWithMetadata( struct soap *soapStruct,
				bmdDatagram_t *responseDatagram,
				struct bmd230__mtdsValues *formMtds,
				long includeSYSMetadataFlag, long includeADDMetadataFlag, long includeACTIONMetadataFlag,
				struct bmd230__mtdsValues **responseMetadata )
{

long iter=				0;
long jupiter=				0;

struct bmd230__mtdsValues *tempMetadata=	NULL;

long allocatedMetadataCount=		0;
long practicallySetMetadataCount=	0;
long addThisMetadata=1;

char *metadataOid=			NULL;
char *metadataValue=			NULL;
long metadataValueLength=		0;


	if(soapStruct == NULL)
		{ return -1; }
	if(responseDatagram == NULL)
		{ return -2; }
	if(responseMetadata == NULL)
		{ return -3; }
	if(*responseMetadata != NULL)
		{ return -4; }


	tempMetadata=(struct bmd230__mtdsValues *)soap_malloc(soapStruct, sizeof(struct bmd230__mtdsValues));
	if(tempMetadata == NULL)
	{
		return -11;
	}
	memset(tempMetadata, 0, sizeof(struct bmd230__mtdsValues));

	/*jesli brak formularza, to ustawienie wszystkich metadanych*/
	if(formMtds == NULL)
	{
		if(includeSYSMetadataFlag > 0)
			{ allocatedMetadataCount += responseDatagram->no_of_sysMetaData; }
		if(includeADDMetadataFlag > 0)
			{ allocatedMetadataCount += responseDatagram->no_of_additionalMetaData; }
		if(includeACTIONMetadataFlag > 0)
			{ allocatedMetadataCount += responseDatagram->no_of_actionMetaData; }

	}
	/*jesli pusty formularz, to nie ustawiam zadnych metadanych*/
	else if(formMtds != NULL && formMtds->__size <= 0)
	{
		return 0;
		//if(includeSYSMetadataFlag > 0)
		//	{ allocatedMetadataCount += responseDatagram->no_of_sysMetaData; }
		//if(includeADDMetadataFlag > 0)
		//	{ allocatedMetadataCount += responseDatagram->no_of_additionalMetaData; }
		//if(includeACTIONMetadataFlag > 0)
		//	{ allocatedMetadataCount += responseDatagram->no_of_actionMetaData; }
	}
	else
	{
		allocatedMetadataCount=formMtds->__size;
	}


	tempMetadata->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soapStruct, sizeof(struct bmd230__mtdSingleValue) * allocatedMetadataCount);
	if(tempMetadata->__ptr == NULL)
	{
		return -12;
	}
	memset(tempMetadata->__ptr, 0, sizeof(struct bmd230__mtdSingleValue) * allocatedMetadataCount);
	tempMetadata->__size = allocatedMetadataCount;



	if(includeSYSMetadataFlag > 0)
	{
		for(iter=0; iter<responseDatagram->no_of_sysMetaData; iter++)
		{
			addThisMetadata=0;
			metadataOid=responseDatagram->sysMetaData[iter]->OIDTableBuf;
			metadataValue=responseDatagram->sysMetaData[iter]->AnyBuf;
			metadataValueLength=responseDatagram->sysMetaData[iter]->AnySize;
			if(metadataOid == NULL || metadataValue == NULL || metadataValueLength < 0)
			{
				continue;
			}

			/*sprawdzenie, czy nalezy dodac */
			if(formMtds != NULL && formMtds->__size > 0)
			{
				for(jupiter=0; jupiter<formMtds->__size; jupiter++)
				{
					if(formMtds->__ptr[jupiter].mtdOid == NULL)
						{ return -21; }
					if(strcmp(metadataOid, formMtds->__ptr[jupiter].mtdOid) == 0)
					{
						addThisMetadata=1;
						break;
					}
				}
			}
			else
			{
				addThisMetadata=1;
			}

			if(addThisMetadata > 0)
			{
				tempMetadata->__ptr[practicallySetMetadataCount].mtdOid=soap_malloc(soapStruct, strlen(metadataOid)+1 );
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid == NULL)
					{ return -22; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, 0, strlen(metadataOid)+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, metadataOid, strlen(metadataOid));

				tempMetadata->__ptr[practicallySetMetadataCount].mtdValue=soap_malloc(soapStruct, metadataValueLength+1);
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue == NULL)
					{ return -23; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, 0, metadataValueLength+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, metadataValue, metadataValueLength);

				tempMetadata->__ptr[practicallySetMetadataCount].mtdDesc=NULL;

				practicallySetMetadataCount++;
			}
		}
	}


	if(includeADDMetadataFlag > 0)
	{
		for(iter=0; iter<responseDatagram->no_of_additionalMetaData; iter++)
		{
			addThisMetadata=0;
			metadataOid=responseDatagram->additionalMetaData[iter]->OIDTableBuf;
			metadataValue=responseDatagram->additionalMetaData[iter]->AnyBuf;
			metadataValueLength=responseDatagram->additionalMetaData[iter]->AnySize;
			if(metadataOid == NULL || metadataValue == NULL || metadataValueLength < 0)
			{
				continue;
			}

			/*sprawdzenie, czy nalezy dodac */
			if(formMtds != NULL && formMtds->__size > 0)
			{
				for(jupiter=0; jupiter<formMtds->__size; jupiter++)
				{
					if(formMtds->__ptr[jupiter].mtdOid == NULL)
						{ return -31; }
					if(strcmp(metadataOid, formMtds->__ptr[jupiter].mtdOid) == 0)
					{
						addThisMetadata=1;
						break;
					}
				}
			}
			else
			{
				addThisMetadata=1;
			}

			if(addThisMetadata > 0)
			{
				tempMetadata->__ptr[practicallySetMetadataCount].mtdOid=soap_malloc(soapStruct, strlen(metadataOid)+1 );
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid == NULL)
					{ return -32; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, 0, strlen(metadataOid)+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, metadataOid, strlen(metadataOid));

				tempMetadata->__ptr[practicallySetMetadataCount].mtdValue=soap_malloc(soapStruct, metadataValueLength+1);
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue == NULL)
					{ return -33; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, 0, metadataValueLength+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, metadataValue, metadataValueLength);

				tempMetadata->__ptr[practicallySetMetadataCount].mtdDesc=NULL;

				practicallySetMetadataCount++;
			}
		}
	}


	if(includeACTIONMetadataFlag > 0)
	{
		for(iter=0; iter<responseDatagram->no_of_actionMetaData; iter++)
		{
			addThisMetadata=0;
			metadataOid=responseDatagram->actionMetaData[iter]->OIDTableBuf;
			metadataValue=responseDatagram->actionMetaData[iter]->AnyBuf;
			metadataValueLength=responseDatagram->actionMetaData[iter]->AnySize;
			if(metadataOid == NULL || metadataValue == NULL || metadataValueLength < 0)
			{
				continue;
			}

			/*sprawdzenie, czy nalezy dodac */
			if(formMtds != NULL && formMtds->__size > 0)
			{
				for(jupiter=0; jupiter<formMtds->__size; jupiter++)
				{
					if(formMtds->__ptr[jupiter].mtdOid == NULL)
						{ return -41; }
					if(strcmp(metadataOid, formMtds->__ptr[jupiter].mtdOid) == 0)
					{
						addThisMetadata=1;
						break;
					}
				}
			}
			else
			{
				addThisMetadata=1;
			}

			if(addThisMetadata > 0)
			{
				tempMetadata->__ptr[practicallySetMetadataCount].mtdOid=soap_malloc(soapStruct, strlen(metadataOid)+1 );
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid == NULL)
					{ return -42; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, 0, strlen(metadataOid)+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdOid, metadataOid, strlen(metadataOid));

				tempMetadata->__ptr[practicallySetMetadataCount].mtdValue=soap_malloc(soapStruct, metadataValueLength+1);
				if(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue == NULL)
					{ return -43; }
				memset(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, 0, metadataValueLength+1);
				memcpy(tempMetadata->__ptr[practicallySetMetadataCount].mtdValue, metadataValue, metadataValueLength);

				tempMetadata->__ptr[practicallySetMetadataCount].mtdDesc=NULL;

				practicallySetMetadataCount++;
			}
		}
	}


	tempMetadata->__size=practicallySetMetadataCount;


	*responseMetadata=tempMetadata;
	tempMetadata=NULL;
	return 0;

}

/****************************************************************/
/*	Funkcja	przepisujaca metadane do datagramu odpowiedzi	*/
/****************************************************************/
/****************************************************************************************/
/*Zamienia liste wynikow otrzymana z serwera na liste wynikow odsylana z soap. Mozna podac rowniez oid metadanej
ktora tylko bedzie wstawiona do wyniku.
twf_soap - struktura przechowujaca zaalokowana w soap pamiec.
twf_metadata_array_struct - tablica metadanych
twf_metadata_count - wielkosc tablicy metadanych
twf_result_mtds - tablica z wynikami wyszukiwania
twf_formMtds - tablica oid metadanych, ktorych tylko mamy uzyc w odpowiedzi
twf_formMtds_number - numer oid z tablicy twf_formMtds
twf_mtd_result_count - kolejny numer metadanej wrzucanej do tablicy wynikow twf_metadata_array_struct. Po zakonczeniu
	dzialania funkcji w zmiennej tej znajdzie sie sumaryczna ilosc metadanych wrzuconych do twf_metadata_array_struct.*/
long MtdsListToResponse (	struct soap *twf_soap,
				MetaDataBuf_t **twf_metadata_array_struct,
				long twf_metadata_count,

				struct bmd230__mtdsValues *twf_result_mtds,

				struct bmd230__mtdsValues *twf_formMtds,
				long twf_formMtds_number,
				long *twf_mtd_result_count)
{
long j				= 0;
long mtd_result_count		= 0;
char *metadata_oid_str		= NULL;
char *metadata_value		= NULL;
long metadata_size		= 0;

	if(twf_soap == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_metadata_array_struct == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_result_mtds == NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(twf_mtd_result_count == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}

	mtd_result_count = *twf_mtd_result_count;

	/****************************************************************/
	/*      pobranie oid'a metadanej z odpowiedzi od serwera BMD    */
	/****************************************************************/

	for (j=0; j < twf_metadata_count; j++)
	{/*for (j=0; j < twf_metadata_count; j++)*/

		if(twf_metadata_array_struct[j]->OIDTableBuf != NULL)
		{
			metadata_oid_str=strdup(twf_metadata_array_struct[j]->OIDTableBuf);
		}
		
		if (metadata_oid_str==NULL)
		{
			continue;
		}

	/****************************************************************/
	/*      pobranie wartosci metadanej z odpowiedzi od serwera     */
	/****************************************************************/
		metadata_value = twf_metadata_array_struct[j]->AnyBuf;
		metadata_size = twf_metadata_array_struct[j]->AnySize;

	/****************************************************************/
	/*      wszystkie metadane znalezionego pojedynczego pliku      */
	/****************************************************************/

		if(twf_formMtds != NULL)
		{/*if(twf_formMtds != NULL)*/

			if(twf_formMtds->__ptr[twf_formMtds_number].mtdOid == NULL)
			{
				PRINT_ERROR("Undefined metadata OID in table at index %li\n", twf_formMtds_number);
				BMD_FOK(BMDSOAP_SERVER_UNDEFINED_METADATA_OID_FORM);
			}
		
			if (strcmp(metadata_oid_str, twf_formMtds->__ptr[twf_formMtds_number].mtdOid)==0)
			{
				twf_result_mtds->__ptr[mtd_result_count].mtdOid = \
				(char *)soap_malloc(twf_soap, (strlen(metadata_oid_str) + 1) * sizeof(char));
				if(twf_result_mtds->__ptr[mtd_result_count].mtdOid == NULL)
				{
					BMD_FOK(NO_MEMORY);
				}
				memset(twf_result_mtds->__ptr[mtd_result_count].mtdOid, \
				0, (strlen(metadata_oid_str) + 1) * sizeof(char));
				memcpy(twf_result_mtds->__ptr[mtd_result_count].mtdOid, \
				metadata_oid_str,strlen(metadata_oid_str)*sizeof(char));

				twf_result_mtds->__ptr[mtd_result_count].mtdValue= \
				(char *)soap_malloc(twf_soap, (metadata_size + 1)*sizeof(char));
				if(twf_result_mtds->__ptr[mtd_result_count].mtdValue == NULL)
				{
					BMD_FOK(NO_MEMORY);
				}
				memset( twf_result_mtds->__ptr[mtd_result_count].mtdValue,0, \
				(metadata_size + 1) * sizeof(char));
				memcpy( twf_result_mtds->__ptr[mtd_result_count].mtdValue, \
				metadata_value, metadata_size*sizeof(char));

				twf_result_mtds->__ptr[mtd_result_count].mtdDesc= \
				(char *)soap_malloc(twf_soap,2*sizeof(char));
				if( twf_result_mtds->__ptr[mtd_result_count].mtdDesc == NULL)
				{
					BMD_FOK(NO_MEMORY);
				}
				memset(twf_result_mtds->__ptr[mtd_result_count].mtdDesc,0, 2*sizeof(char));

				mtd_result_count++;
				break;
			}
		}/*\if(twf_formMtds != NULL)*/
		else
		{/*else(twf_formMtds != NULL)*/
			twf_result_mtds->__ptr[mtd_result_count].mtdOid = \
			(char *)soap_malloc(twf_soap, (strlen(metadata_oid_str) + 1) * sizeof(char));
			if(twf_result_mtds->__ptr[mtd_result_count].mtdOid == NULL)
			{
				BMD_FOK(NO_MEMORY);
			}
			memset(twf_result_mtds->__ptr[mtd_result_count].mtdOid, \
			0, (strlen(metadata_oid_str) + 1) * sizeof(char));
			memcpy(twf_result_mtds->__ptr[mtd_result_count].mtdOid, \
			metadata_oid_str,strlen(metadata_oid_str)*sizeof(char));

			twf_result_mtds->__ptr[mtd_result_count].mtdValue= \
			(char *)soap_malloc(twf_soap, (metadata_size + 1)*sizeof(char));
			if(twf_result_mtds->__ptr[mtd_result_count].mtdValue == NULL)
			{
				BMD_FOK(NO_MEMORY);
			}
			memset(twf_result_mtds->__ptr[mtd_result_count].mtdValue,0, \
			(metadata_size + 1)*sizeof(char));
			memcpy(twf_result_mtds->__ptr[mtd_result_count].mtdValue, \
			metadata_value, metadata_size*sizeof(char));

			twf_result_mtds->__ptr[mtd_result_count].mtdDesc= \
			(char *)soap_malloc(twf_soap,2*sizeof(char));
			if(twf_result_mtds->__ptr[mtd_result_count].mtdDesc == NULL)
			{
				BMD_FOK(NO_MEMORY);
			}
			memset(twf_result_mtds->__ptr[mtd_result_count].mtdDesc,0,2*sizeof(char));

			mtd_result_count++;
		}/*\else(twf_formMtds != NULL)*/
		/*}*//*\for(k=0; k < form_mtds_count; k++)*/

		free(metadata_oid_str);
		metadata_oid_str=NULL;

	}/*\for (j=0; j < twf_metadata_count; j++)*/

	*twf_mtd_result_count = mtd_result_count;

	return BMD_OK;
}


/****************************************************************/
/*      Funkcja przepisujaca metadane do pliku CSV		*/
/****************************************************************/
/*Zamienia liste wynikow otrzymana z serwera na plik DVCS. Mozna podac rowniez oid metadanej
ktora tylko bedzie wstawiona do wyniku.
twf_soap - struktura przechowujaca zaalokowana w soap pamiec.
twf_metadata_array_struct - tablica metadanych
twf_metadata_count - wielkosc tablicy metadanych
twf_tmp_csv_file - bufor w pamiecie przechowujacy zawartosc generowanego pliku
twf_csv_delimiter - znak jakim nalezy rozdzielac poszczegolne wartosci.
twf_first_run_flag - flaga okreslajaca czy jest to pierwsze wywolanie funkcji dla danego datagramu 1 - tak, 0 nie
twf_formMtds - tablica oid metadanych, ktorych tylko mamy uzyc do generowania pliku CSV
twf_formMtds_number - numer oid z tablicy twf_formMtds*/
long MtdsListToCSV (	struct soap *twf_soap,
			MetaDataBuf_t **twf_metadata_array_struct,
			long twf_metadata_count,
			BMD_attr_t **twf_form,
			long twf_from_count,

			char **twf_tmp_csv_file,

			long *twf_first_run_flag,

			struct bmd230__mtdsValues *twf_formMtds,
			long twf_formMtds_number,

			CSV_escape_conf_t *twf_CSV_escape_conf)
{
long iteration_0		= 0;
long j			= 0;
long value_type		= 0;
long status			= 0;
long escape_type_flag	= 0;
long metadata_size	= 0;
char *metadata_oid_str	= NULL;
char *tmp_string		= NULL;
char *csv_delimiter	= NULL;
char *escaped_value	= NULL;
char *form_oid_str	= NULL;
char *metadata_value	= NULL;


	if(twf_soap == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_metadata_array_struct == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_tmp_csv_file == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(twf_CSV_escape_conf == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}

	if(twf_CSV_escape_conf->tws_csv_delimiter == NULL)
	{
		asprintf(&csv_delimiter,",");
		if(csv_delimiter == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	else
	{
		asprintf(&csv_delimiter,"%s",twf_CSV_escape_conf->tws_csv_delimiter);
		if(csv_delimiter == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}


	if(     ( ( twf_CSV_escape_conf->tws_enable_string_escaping == 1 &&
		twf_CSV_escape_conf->tws_enable_number_escaping == 1 &&
		twf_CSV_escape_conf->tws_enable_date_escaping == 1) ||

		(twf_CSV_escape_conf->tws_enable_string_escaping == 0 &&
		twf_CSV_escape_conf->tws_enable_number_escaping == 0 &&
		twf_CSV_escape_conf->tws_enable_date_escaping == 0) ) && ( twf_CSV_escape_conf->src_delimeter == NULL ||
		twf_CSV_escape_conf->dst_delimeter == NULL ) )
	{
		escape_type_flag = 1;
	}
	/****************************************************************/
	/*      pobranie oid'a metadanej z odpowiedzi od serwera BMD    */
	/****************************************************************/
	for (j=0; j < twf_metadata_count; j++)
	{/*for (j=0; j < twf_metadata_count; j++)*/

		/****************************************************************/
		/*      pobranie oid'a metadanej z odpowiedzi od serwera BMD	*/
		/****************************************************************/
		asprintf(&metadata_oid_str, "%s", twf_metadata_array_struct[j]->OIDTableBuf);

		if (metadata_oid_str==NULL)
		{
			continue;
		}

		/****************************************************************/
		/*      pobranie wartosci metadanej z odpowiedzi od serwera     */
		/****************************************************************/
		metadata_value = twf_metadata_array_struct[j]->AnyBuf;
		metadata_size = twf_metadata_array_struct[j]->AnySize;

		value_type = 0;

		/*Ustalam typ metadanej*/
		if(escape_type_flag == 0)
		{
			for(iteration_0 = 0; iteration_0 < twf_from_count; iteration_0++)
			{
				status = bmd_attr_get_oid(iteration_0, twf_form, &form_oid_str);
				if(status < BMD_OK)
				{
					BMD_FOK(BMDSOAP_SERVER_CANNOT_OBTAIN_METADATA_TYPE);
				}
				if( strcmp(metadata_oid_str, form_oid_str) == 0 && \
				strlen(metadata_oid_str) == strlen(form_oid_str) )
				{
					value_type = twf_form[iteration_0]->contentType;
					PRINT_VDEBUG("Metadata oid: %s metadata type: %li\n",
					metadata_oid_str, value_type);
					free(form_oid_str);
					form_oid_str = NULL;
					break;
				}
				free(form_oid_str);
				form_oid_str = NULL;
			}
		}

		if(twf_formMtds != NULL)
		{/*if(twf_formMtds != NULL)*/
		
			if(twf_formMtds->__ptr[twf_formMtds_number].mtdOid == NULL)
			{
				PRINT_ERROR("Undefined metadata OID in table at index %li\n", twf_formMtds_number);
				BMD_FOK(BMDSOAP_SERVER_UNDEFINED_METADATA_OID_FORM);
			}
		
			if (strcmp(metadata_oid_str, twf_formMtds->__ptr[twf_formMtds_number].mtdOid)==0)
			{/*if (strcmp(metadata_oid_str, twf_formMtds->__ptr[twf_formMtds_number].mtdOid)==0)*/
				if (*twf_tmp_csv_file!=NULL)
				{
					if (metadata_value != NULL)
					{
						if(twf_formMtds_number == 0 && *twf_first_run_flag == 1)
						{
							status = CharEscape( (char *)metadata_value,
							metadata_size,
							&escaped_value,
							value_type,
							twf_CSV_escape_conf);
							if(status < BMD_OK)
							{
								BMD_FOK(status);
							}
							asprintf(&tmp_string, "%s%s", *twf_tmp_csv_file, \
							escaped_value);
						}
						else
						{
							status = CharEscape( (char *)metadata_value,
							metadata_size,
							&escaped_value,
							value_type,
							twf_CSV_escape_conf);
							if(status < BMD_OK)
							{
								BMD_FOK(status);
							}
							asprintf(&tmp_string, "%s%s%s", *twf_tmp_csv_file, \
							csv_delimiter, escaped_value);
						}
					}
					else
					{
						if(twf_formMtds_number == 0 && *twf_first_run_flag == 1)
						{
							asprintf(&tmp_string, "%s%s", *twf_tmp_csv_file, "");
						}
						else
						{
							asprintf(&tmp_string, "%s%s%s", *twf_tmp_csv_file, csv_delimiter, \
							"");
						}
					}
					free(*twf_tmp_csv_file);
					*twf_tmp_csv_file = tmp_string;
					tmp_string = NULL;
					if(*twf_tmp_csv_file == NULL)
					{
						BMD_FOK(NO_MEMORY);
					}
				}
				else
				{
					if (metadata_value != NULL)
					{
						status = CharEscape( (char *)metadata_value,
						metadata_size,
						&escaped_value,
						value_type,
						twf_CSV_escape_conf);
						if(status < BMD_OK)
						{
							return status;
						}
						asprintf(twf_tmp_csv_file, "%s", escaped_value);

					}
					else
					{
						asprintf(twf_tmp_csv_file, "%s", "");
					}
					if(*twf_tmp_csv_file == NULL)
					{
						BMD_FOK(NO_MEMORY);
					}
				}
				*twf_first_run_flag = 0;
				break;
			}/*\if (strcmp(metadata_oid_str, twf_formMtds->__ptr[twf_formMtds_number].mtdOid)==0)*/
		}/*if(twf_formMtds != NULL)*/
		else
		{/*else(twf_formMtds != NULL)*/
			if (*twf_tmp_csv_file!=NULL)
			{
				if (metadata_value != NULL)
				{
					if(j == 0 && *twf_first_run_flag == 1)
					{
						status = CharEscape( (char *)metadata_value,
						metadata_size,
						&escaped_value,
						value_type,
						twf_CSV_escape_conf);
						if(status < BMD_OK)
						{
							BMD_FOK(status);
						}
						asprintf(&tmp_string, "%s%s", *twf_tmp_csv_file, \
						escaped_value);
					}
					else
					{
						status = CharEscape( (char *)metadata_value,
						metadata_size,
						&escaped_value,
						value_type,
						twf_CSV_escape_conf);
						if(status < BMD_OK)
						{
							BMD_FOK(status);
						}
						asprintf(&tmp_string, "%s%s%s", *twf_tmp_csv_file, csv_delimiter, \
						escaped_value);
					}
				}
				else
				{
					if(j == 0 && *twf_first_run_flag == 1)
					{
						asprintf(&tmp_string, "%s%s", *twf_tmp_csv_file, "");
					}
					else
					{
						asprintf(&tmp_string, "%s%s%s", *twf_tmp_csv_file, csv_delimiter, "");
					}
				}
				*twf_first_run_flag = 0;

				free(*twf_tmp_csv_file);
				*twf_tmp_csv_file = tmp_string;
				tmp_string = NULL;
				if(*twf_tmp_csv_file == NULL)
				{
					BMD_FOK(NO_MEMORY);
				}

			}
			else
			{
				if (metadata_value != NULL)
				{
					status = CharEscape( (char *)metadata_value,
					metadata_size,
					&escaped_value,
					value_type,
					twf_CSV_escape_conf);
					if(status < BMD_OK)
					{
						BMD_FOK(status);
					}
					asprintf(twf_tmp_csv_file, "%s", escaped_value);
				}
				else
				{
					asprintf(twf_tmp_csv_file, "%s", "");
				}
				if(*twf_tmp_csv_file == NULL)
				{
					BMD_FOK(NO_MEMORY);
				}
			}
		}/*\else(twf_formMtds != NULL)*/
		free(metadata_oid_str);
		metadata_oid_str=NULL;

		/*free_gen_buf(&metadata_value_buf);*/

		free(escaped_value);
		escaped_value = NULL;
	}/*\for (j=0; j < twf_metadata_count; j++)*/

	free(csv_delimiter);
	csv_delimiter = NULL;

	return BMD_OK;
}

/*
Funkcja sluzaca do escapowania w buforze wystapienia znaku.
twf_value - escapowany bufor
twf_value_count - ilosc znakow w buforze
twf_escaped_value - bufor wynikowy. Przy wywolaniu funkcji wskaznik musi wskazywac na NULL.
twf_char_to_escape - znak escapowany
twf_escape_char - ciag escapujacy dany znak
twf_enable_escaping - czy wlaczyc escapowanie - 1 tak, 0 - nie. Przydatne gdy chcemy jedynie przegrac genbuf do stringa.
*/
long CharEscape(	char *twf_value,
			long twf_value_count,
			char **twf_escaped_value,
			long twf_value_type,

			CSV_escape_conf_t *twf_CSV_escape_conf)
{
char *char_to_escape		= NULL;
char *escape_char		= NULL;
char *value_with_null		= NULL;
char *m_value_escape		= NULL;
long iteration_0		= 0;
long bytes_count		= 0;
long enable_escaping		= 0;
long enable_CSVEscape		= 1; /*Czy korzystac z funkcji Radka na razie na sztywno tak.*/
bmd_oid_xpath_t escape_conf;	/*Do funkcji escape napisanej przez Radka*/

	PRINT_VDEBUG("CharEscape - escaping values\n");

	if(twf_value == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM1);
	}

	if(twf_value_count < 1)
	{
		BMD_FOK(BMD_ERR_PARAM2);
	}

	if(twf_escaped_value == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM3);
	}

	if(*twf_escaped_value != NULL)
	{
		BMD_FOK(BMD_ERR_PARAM3);
	}

	if(twf_value_type < 0 || twf_value_type > 3)
	{
		/*Wykomentowane bo jest potrzeba okreslenia nowych wartosci*/
		/*BMD_FOK(BMD_ERR_PARAM3);*/
	}

	if(twf_CSV_escape_conf == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM5);
	}

	if(twf_CSV_escape_conf->tws_char_to_escape == NULL)
	{
		PRINT_VDEBUG("No character to escape. Assuming default.\n");
		asprintf(&char_to_escape,"\"");
		if(char_to_escape == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	else
	{
		if( strlen(twf_CSV_escape_conf->tws_char_to_escape) > 0)
		{
			asprintf(&char_to_escape, "%s", twf_CSV_escape_conf->tws_char_to_escape);
		}
		else
		{
			asprintf(&char_to_escape,"\"");
		}
		if(char_to_escape == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	PRINT_VDEBUG("Character to escape: %s.\n", char_to_escape);

	if(twf_CSV_escape_conf->tws_escape_char == NULL)
	{
		PRINT_VDEBUG("No escape character. Assuming default.\n");
		asprintf(&escape_char,"\"");
		if(escape_char == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	else
	{
		if(strlen(twf_CSV_escape_conf->tws_escape_char) > 0)
		{
			asprintf(&escape_char, "%s", twf_CSV_escape_conf->tws_escape_char);
		}
		else
		{
			asprintf(&escape_char,"\"");
		}
                if(escape_char == NULL)
                {
			BMD_FOK(NO_MEMORY);
                }
	}
	PRINT_VDEBUG("Escape character: %s.\n", escape_char);

	if(twf_CSV_escape_conf->tws_m_value_escape == NULL)
	{
		PRINT_VDEBUG("No surrounding value escape character. Assuming default.\n");
		asprintf(&m_value_escape,"\"");
		if(m_value_escape == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	else
	{
		if(strlen(twf_CSV_escape_conf->tws_m_value_escape) > 0)
		{
			asprintf(&m_value_escape, "%s", twf_CSV_escape_conf->tws_m_value_escape);
		}
		else
		{
			asprintf(&m_value_escape,"\"");
		}
		if(m_value_escape == NULL)
		{

			BMD_FOK(NO_MEMORY);
		}
	}
	PRINT_VDEBUG("Surrounding escape character: %s.\n", m_value_escape);

	if(twf_CSV_escape_conf->tws_enable_escaping != 0)
	{
		enable_escaping = 1;
	}
	else
	{
		enable_escaping = twf_CSV_escape_conf->tws_enable_escaping;
	}

	PRINT_VDEBUG("Escaping enabled: %li.\n", enable_escaping);

	if(enable_CSVEscape == 0 || enable_escaping == 0)
	{/*if(enable_CSVEscape == 0)*/
		/*Zliczanie bajtow potrzebnych na wynik*/
		if(enable_escaping == 1)
		{
			bytes_count = 0;
			for(iteration_0 = 0; iteration_0 < twf_value_count; iteration_0++)
			{
				bytes_count++;
				if(twf_value[iteration_0] == char_to_escape[0])
				{
					bytes_count = bytes_count + strlen(escape_char);
				}
			}
		}
		else
		{
			bytes_count = twf_value_count;
		}
		PRINT_VDEBUG("Input buffer length: %li Output buffor length: %li.\n", twf_value_count, bytes_count);

		/*Alokacja bufora wynikowego*/
		*twf_escaped_value = malloc( (bytes_count + 1)*sizeof(char) );
		if(*twf_escaped_value == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		memset(*twf_escaped_value, 0, (bytes_count + 1)*sizeof(char) );

		if(twf_CSV_escape_conf->src_delimeter != NULL && twf_CSV_escape_conf->dst_delimeter != NULL && \
		twf_value_type == 2)
		{
			if(strlen(twf_CSV_escape_conf->src_delimeter) > 0 && \
			strlen(twf_CSV_escape_conf->dst_delimeter) > 0)
			{
				for(iteration_0 = 0; iteration_0 < twf_value_count; iteration_0++)
				{
					if( twf_value[iteration_0] == twf_CSV_escape_conf->src_delimeter[0] )
					{
						(*twf_escaped_value)[iteration_0] = twf_CSV_escape_conf->dst_delimeter[0];
					}
					else
					{
						(*twf_escaped_value)[iteration_0] =  twf_value[iteration_0];
					}
				}
			}
		}

		if(bytes_count == twf_value_count)
		{/*if(bytes_counts == twf_value_count)*/
			/*Nie dokonano zadnego escapowania*/
			memcpy(*twf_escaped_value, twf_value, bytes_count * sizeof(char));
			PRINT_VDEBUG("No escaping needed. Output value %s. Output value length %li.\n", *twf_escaped_value,
			bytes_count);
		}/*\if(bytes_counts == twf_value_count)*/
		else
		{/*\else(bytes_counts == twf_value_count)*/
			bytes_count = 0;
			for(iteration_0 = 0; iteration_0 < twf_value_count; iteration_0++)
			{
				if(twf_value[iteration_0] == char_to_escape[0])
				{
					memcpy( *twf_escaped_value + bytes_count, escape_char, strlen(escape_char));
					bytes_count =  bytes_count + strlen(escape_char);
					(*twf_escaped_value)[bytes_count] = twf_value[iteration_0];
				}
				else
				{
					(*twf_escaped_value)[bytes_count] = twf_value[iteration_0];
				}
				bytes_count++;
			}
			PRINT_VDEBUG("Escaping needed. Output value %s. Output value length %li.\n", *twf_escaped_value, \
			bytes_count);
		}/*\else(bytes_counts == twf_value_count)*/
	}/*\if(enable_CSVEscape == 0)*/
	else
	{/*else(enable_CSVEscape == 0)*/
		/*Wypelnienie opcji konfiguracyjnej*/
		memset(&escape_conf, 0, sizeof(bmd_oid_xpath_t));
		escape_conf.text_separator_char = *char_to_escape;
		escape_conf.escape_char = *escape_char;
		if(twf_CSV_escape_conf->src_delimeter != NULL)
		{
			if(strlen(twf_CSV_escape_conf->src_delimeter) > 0)
			{
				PRINT_VDEBUG("Source delimeter: %c\n", twf_CSV_escape_conf->src_delimeter[0] );
				escape_conf.src_dec_separator = twf_CSV_escape_conf->src_delimeter[0];
			}
		}
		else
		{
			PRINT_VDEBUG("No source delimeter.\n");
		}

		if(twf_CSV_escape_conf->dst_delimeter != NULL)
		{
			if(strlen(twf_CSV_escape_conf->dst_delimeter) > 0)
			{
				PRINT_VDEBUG("Destination delimeter: %c\n", twf_CSV_escape_conf->dst_delimeter[0] );
				escape_conf.dst_dec_separator = twf_CSV_escape_conf->dst_delimeter[0];
			}
		}
		else
		{
			PRINT_VDEBUG("No destination delimeter.\n");
		}

		value_with_null = malloc((twf_value_count + 1)* sizeof(char));
		if(value_with_null == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		memset(value_with_null, 0, (twf_value_count + 1)* sizeof(char));
		memcpy(value_with_null, twf_value, twf_value_count* sizeof(char));


		PRINT_VDEBUG("Before escape value: %s\n. Value type %li.", value_with_null, twf_value_type);
		if( escape_conf.src_dec_separator != 0 && escape_conf.dst_dec_separator != 0 && \
		(twf_value_type == 2 || twf_value_type == 4))
		{
			PRINT_VDEBUG("Dot to coma conversion enabled.\n");
			*twf_escaped_value = CSVEscape(&escape_conf, value_with_null, 'n');
		}
		else
		{
			PRINT_VDEBUG("Dot to coma conversion disabled.\n")
			*twf_escaped_value = CSVEscape(&escape_conf, value_with_null, 't');
		}


		if(*twf_escaped_value == NULL)
		{
			BMD_FOK(BMDSOAP_SERVER_CSV_ESCAPING_ERROR);
		}
		free(value_with_null);
		value_with_null = NULL;
	}/*\else(enable_CSVEscape == 0)*/

	/*Otaczanie calej wartosci metadanej odpowiednim znakiem.*/
	if(m_value_escape != NULL && (  \
	( (twf_value_type == 0 || twf_value_type == 1) && twf_CSV_escape_conf->tws_enable_string_escaping == 1) || \
	( (twf_value_type == 2 || twf_value_type == 4) && twf_CSV_escape_conf->tws_enable_number_escaping == 1) || \
	(twf_value_type == 3 && twf_CSV_escape_conf->tws_enable_date_escaping == 1) ))
	{
		asprintf(&value_with_null,"%s%s%s", m_value_escape, *twf_escaped_value, m_value_escape);
		if(value_with_null == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		free(*twf_escaped_value);
		*twf_escaped_value = value_with_null;
		value_with_null = NULL;
	}

	PRINT_VDEBUG("After escape value: %s\n", *twf_escaped_value);

	free(char_to_escape);
	char_to_escape = NULL;

	free(escape_char);
	escape_char = NULL;

	free(m_value_escape);
	m_value_escape = NULL;

	return BMD_OK;
}


/**
* @author WSZ
* Funkcja bmdsoapGenerateDateTimeString() na podstawie struktury ns_DateTime generuje lancuch znakowy z data w formacie
* timestampa sql (yyyy-mm-dd hh:mm:ss).
*
* @param[in] dateTimeStruct do adres stuktury bmd230__DateTime okreslajacej date i czas
* @param[out] dateTimeString do adres wskaznika, do ktorego przypisany zostania wygenerowana (zaalokowana) lancuch znakowy
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, lub odpowiednia wartosc mniejsza od BMD_OK w przypadku bledu.
*/
long bmdsoapGenerateDateTimeString(struct bmd230__DateTime* dateTimeStruct, char** dateTimeString)
{
char* dateTimeFormat			= "%04li-%02li-%02li %02li:%02li:%02li";
const long dateTimeStringLength		= 20;

	if(dateTimeStruct == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(dateTimeString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*dateTimeString != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	*dateTimeString=(char*)calloc(dateTimeStringLength, sizeof(char));
	if(*dateTimeString == NULL)
		{ BMD_FOK(NO_MEMORY); }

	if(snprintf(*dateTimeString, dateTimeStringLength, dateTimeFormat, dateTimeStruct->year, dateTimeStruct->month, dateTimeStruct->day, dateTimeStruct->hour, dateTimeStruct->minute, dateTimeStruct->second) < 0)
	{
		free(*dateTimeString); *dateTimeString=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return BMD_OK;
}
