/* Funkcje nie eksportowalne do API biblioteki, ale uzywane wewnetrznie */
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
//#define PRINT_INFO
#pragma warning(disable:4100)
#endif

long get_owner_from_cert(	GenBuf_t *cert,
				GenBuf_t **owner)
{
char *dn		= NULL;
char *serial_as_string	= NULL;
char *owner_as_string	= NULL;
long unused		= 0;

	PRINT_INFO("LIBBMDINF Getting owner from certificate\n");
	BMD_FOK(X509CertGetAttr(cert,X509_ATTR_ISSUER_DN,&dn,&unused));
	BMD_FOK(X509CertGetAttr(cert,X509_ATTR_SN,&serial_as_string,&unused));

	asprintf(&owner_as_string,"%s%s",dn,serial_as_string);
	if(owner_as_string==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	BMD_FOK(set_gen_buf2((char *)owner_as_string,(int)strlen(owner_as_string)+1,owner));

	/************/
	/* porzadki */
	/************/
	free0(dn);
	free0(serial_as_string);
	free0(owner_as_string);

	return BMD_OK;
}

long is_metadata_oid_present(	BMD_attr_t **kontrolki,
					long no_of_kontrolki,
					MetaDataBuf_t *single_metadata)
{
long i;
long metadata_found=0;

	PRINT_INFO("LIBBMDINF Determining if metadata iod is present\n");
	for(i=0;i<no_of_kontrolki;i++)
	{
		if(strcmp(kontrolki[i]->oid->buf, single_metadata->OIDTableBuf)==0)
		{
			metadata_found=1;
			break;
		}
	}
	return metadata_found;
}

/* sprawdza czy datagram przygotowany przez uzytkownika ma te metadane,ktore moze miec
   !!! TO SAMO sprawdzenie musi byc w serwerze !!!
 */
/**
 * Funkcja sprawdza, czy datagram przygotowany przez użytkownika
 * ma te metadane, które moze mieć. <b>Uwaga! <c>TO SAMO</c>
 * sprawdzenie musi być na serwerze.</b>
 *
 * \param bmd_info Struktura z informacjami dot. połączenia z BMD.
 * \param datagram Sprawdzany datagram.
 *
 * \retval BMD_OK Funkcja zakończyła się bez błędów.
 * \retval -1 Wskaźnik do datagramu wskazuje na NULL.
 * \retval -2 Błędne metadane systemowe.
 * \retval -3 Błędne metadane użytkownika.
 * \retval -4 Błędne metadane dodatkowe.
 * */
long check_metadata_in_datagram(bmd_info_t *bmd_info,bmdDatagram_t *datagram)
{
BMD_attr_t **form=NULL;
long no_of_form;

	PRINT_INFO("LIBBMDINF Checking if metada is in datagram\n");
	switch(datagram->datagramType)
	{
		case BMD_DATAGRAM_TYPE_INSERT:
			PRINT_INFO("LIBBMDINF Datagram request type: PutBmdData\n");
			form=bmd_info->form_info.user_send_form;
			no_of_form=0;
			while(form[no_of_form])
			    no_of_form++;
			break;
		case BMD_DATAGRAM_TYPE_SEARCH:
			PRINT_INFO("LIBBMDINF Datagram request type: SelectBmdData\n");
			form=bmd_info->form_info.user_search_form;
			no_of_form=0;
			while(form[no_of_form])
			    no_of_form++;
			break;
		case BMD_DATAGRAM_TYPE_GET:
			PRINT_INFO("LIBBMDINF Datagram request type: GetBmdData\n");
			return BMD_OK;
		case BMD_DATAGRAM_TYPE_GET_PKI:
			PRINT_INFO("LIBBMDINF Datagram request type: GetBmdDataWithProofs\n");
			return BMD_OK;
		case BMD_DATAGRAM_TYPE_DELETE:
			PRINT_INFO("LIBBMDINF Datagram request type: DelBmdData\n");
			return BMD_OK;
		case BMD_DATAGRAM_GET_CGI:
			PRINT_INFO("LIBBMDINF Datagram request type: GetBmdDataCGI\n");
			return BMD_OK;
		case BMD_DATAGRAM_GET_CGI_PKI:
			PRINT_INFO("LIBBMDINF Datagram request type: GetBmdDataCGIPKI\n");
			return BMD_OK;
	};

	if(datagram)
	{
#if 0
		/* sprawdz metadane systemowe */
		if(datagram->sysMetaData)
		{
			for(i=0;i<datagram->no_of_sysMetaData;i++)
				if(is_metadata_oid_present(form,no_of_form,datagram->sysMetaData[i])==0)
					return -2;
		}

		/* sprawdz metadane uzytkownika */
		if(datagram->actionMetaData)
		{
			for(i=0;i<datagram->no_of_actionMetaData;i++)
				if(is_metadata_oid_present(form,no_of_form,datagram->actionMetaData[i])==0)
					return -3;
		}

		/* sprawdz metadane dodatkowe */
		if(datagram->additionalMetaData)
		{
			for(i=0;i<datagram->no_of_additionalMetaData;i++)
				if(is_metadata_oid_present(form,no_of_form,datagram->additionalMetaData[i])==0)
					return -4;
		}

		/* sprawdz metadane PKI */
		/** @todo sprawdzac w inny sposob metadane PKI heheheh af1n */
		/*
		if(datagram->pkiMetaData)
		{
			for(i=0;i<datagram->no_of_pkiMetaData;i++)
				if(is_metadata_oid_present(form,no_of_form,datagram->pkiMetaData[i])==0)
					return -5;
		}
		*/
#endif /* #if 0 */
	}
	else
	{
		PRINT_ERROR("LIBBMDERR Invalid datagram value\n");
		return -1;
	}

	return BMD_OK;
}

/**
 * Funkcja kasuje listę metadanych.
 *
 * \param[out] list Lista metadanych do usunięcia.
 * \param[in] no_of_list Ilość elementów listy.
 *
 * \retval BMD_OK Funkcja zakończyła się bez błędów.
 * */
long delete_metadata_list(MetaDataBuf_t ***list,long no_of_list)
{
long i;

	PRINT_INFO("LIBBMDINF Deleting metadata list\n");
	if(*list)
	{
		for(i=0;i<no_of_list;i++)
		{
			if((*list)[i])
			{
				if( ((*list)[i])->AnyBuf )
				{
					free( ((*list)[i])->AnyBuf );
					((*list)[i])->AnyBuf=NULL;
				}
				if( ((*list)[i])->OIDTableBuf )
				{
					free( ((*list)[i]->OIDTableBuf) );
					((*list)[i])->OIDTableBuf=NULL;
				}
				free((*list)[i]);
				(*list)[i]=NULL;
			}
		}
		free(*list);
		*list=NULL;
	}
	return BMD_OK;
}

long strip_metadata_in_datagram(bmdDatagram_t **datagram)
{
	PRINT_INFO("LIBBMDINF Stripping metadata in datagram\n");
	switch((*datagram)->datagramType)
	{
		case BMD_DATAGRAM_TYPE_INSERT:
			PRINT_INFO("LIBBMDINF Datagram type: PutBmdData\n");
			return BMD_OK;
// 		case BMD_DATAGRAM_TYPE_SEARCH:
		case BMD_DATAGRAM_TYPE_SEARCH:
			PRINT_INFO("LIBBMDINF Datagram type: SelectBmdData\n");
			return BMD_OK;
		case BMD_DATAGRAM_GET_CGI:
			PRINT_INFO("LIBBMDINF Datagram type: GetBmdDataCGI\n");
			/*delete_metadata_list(&((*datagram)->additionalMetaData),(*datagram)->no_of_additionalMetaData);
			delete_metadata_list(&((*datagram)->actionMetaData),(*datagram)->no_of_actionMetaData);
			delete_metadata_list(&((*datagram)->pkiMetaData),(*datagram)->no_of_pkiMetaData);
			delete_metadata_list(&((*datagram)->sysMetaData),(*datagram)->no_of_sysMetaData);*/
			break;
		case BMD_DATAGRAM_GET_CGI_PKI:
			PRINT_INFO("LIBBMDINF Datagram type: GetBmdDataCGIPKI\n");
			/*delete_metadata_list(&((*datagram)->additionalMetaData),(*datagram)->no_of_additionalMetaData);
			delete_metadata_list(&((*datagram)->actionMetaData),(*datagram)->no_of_actionMetaData);
			delete_metadata_list(&((*datagram)->pkiMetaData),(*datagram)->no_of_pkiMetaData);
			delete_metadata_list(&((*datagram)->sysMetaData),(*datagram)->no_of_sysMetaData);*/
			break;
		case BMD_DATAGRAM_TYPE_GET:
			PRINT_INFO("LIBBMDINF Datagram type: GetBmdData\n");
			/*delete_metadata_list(&((*datagram)->additionalMetaData),(*datagram)->no_of_additionalMetaData);
			delete_metadata_list(&((*datagram)->actionMetaData),(*datagram)->no_of_actionMetaData);
			delete_metadata_list(&((*datagram)->pkiMetaData),(*datagram)->no_of_pkiMetaData);
			delete_metadata_list(&((*datagram)->sysMetaData),(*datagram)->no_of_sysMetaData);*/
			break;
		case BMD_DATAGRAM_TYPE_GET_PKI:
			PRINT_INFO("LIBBMDINF Datagram type: GetBmdDataWithProofs\n");
			/*delete_metadata_list(&((*datagram)->additionalMetaData),(*datagram)->no_of_additionalMetaData);
			delete_metadata_list(&((*datagram)->actionMetaData),(*datagram)->no_of_actionMetaData);
			delete_metadata_list(&((*datagram)->pkiMetaData),(*datagram)->no_of_pkiMetaData);
			delete_metadata_list(&((*datagram)->sysMetaData),(*datagram)->no_of_sysMetaData);*/
			break;
		case BMD_DATAGRAM_TYPE_DELETE:
			PRINT_INFO("LIBBMDINF Datagram type: DelBmdData\n");
			/*delete_metadata_list(&((*datagram)->additionalMetaData),(*datagram)->no_of_additionalMetaData);
			delete_metadata_list(&((*datagram)->actionMetaData),(*datagram)->no_of_actionMetaData);
			delete_metadata_list(&((*datagram)->pkiMetaData),(*datagram)->no_of_pkiMetaData);
			delete_metadata_list(&((*datagram)->sysMetaData),(*datagram)->no_of_sysMetaData);*/
			break;
	};
	return BMD_OK;
}

/**
 * Funkcja pobiera nazwę pliku, odcinając ścieżkę dostępu.
 * \param[in] input Nazwa pliku ze ścieżką dostępu.
 * \param[out] output Nazwa pliku, bez ścieżki dostepu.
 *
 * \retval BMD_OK Funkcja zakończyła się bez błędów.
 * \retval -1 Niepoprawne dane na wejściu.
 * */
long get_filename(char *input,char **output)
{
	//PRINT_INFO("LIBBMDINF Getting filename,\n");
	#ifdef WIN32
	char c='\\';
	#else
	char c='/';
	#endif
	char *pos=NULL;

	if(input==NULL)
		return -1;

	pos=strrchr(input,c);
	if(pos==NULL)
	{
		asprintf(output,"%s",input);
		return BMD_OK;
	}
	else
	{
		pos++;
		asprintf(output,"%s",pos);
	}

	return BMD_OK;
}

long add_unvisible_kontrolki(	bmd_info_t *bmd_info,
					bmdDatagram_t **datagram)
{
long i,j,status;
long oid_type;
long count;

	count=0;
	while(bmd_info->form_info.user_unvisible_form[count])
	{
		count++;
	}
	if(count>0)
	{
		for(i=0;i<count;i++)
		{
			oid_type=determine_oid_type_str(bmd_info->form_info.user_unvisible_form[i]->oid->buf);

			j=0;
			if(bmd_info->form_info.user_unvisible_form[i]->valuesList)
			{
			    while(bmd_info->form_info.user_unvisible_form[i]->valuesList[j])
			    {
				status=PR2_bmdDatagram_add_metadata(bmd_info->form_info.user_unvisible_form[i]->oid->buf,
										bmd_info->form_info.user_unvisible_form[i]->valuesList[j]->buf,
										bmd_info->form_info.user_unvisible_form[i]->valuesList[j]->size,
										(EnumMetaData_t )oid_type,*datagram,0,0,0, 0);
				j++;
			    }
			}
		}
	}
	return BMD_OK;
}

long bmd_add_hash_to_dtg(char *hash,bmdDatagram_t **dtg)
{
long tmp_length;

	PRINT_INFO("LIBBMDINF Adding hash to datagram\n");
	tmp_length=(int)strlen(hash);

	BMD_FOK(set_gen_buf2((const char *) hash, tmp_length, &((*dtg)->protocolDataFileId)));

// 	(*dtg)->protocolDataFileId=(GenBuf_t *)malloc(sizeof(GenBuf_t));
// 	(*dtg)->protocolDataFileId->buf=(char *)malloc(tmp_length+2);
// 	memset((*dtg)->protocolDataFileId->buf,0,tmp_length+1);
// 	memcpy((*dtg)->protocolDataFileId->buf,hash,tmp_length);
// 	(*dtg)->protocolDataFileId->size=tmp_length+1;
	return 0;
}

long bmd_add_hash_to_dtg_as_data(char *hash,bmdDatagram_t **dtg)
{
long tmp_length;

	PRINT_INFO("LIBBMDINF Adding hash to datagram as protocal data\n");
	tmp_length=(int)strlen(hash);
	(*dtg)->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	(*dtg)->protocolData->buf=(char *)malloc(tmp_length+2);
	memset((*dtg)->protocolData->buf,0,tmp_length+1);
	memcpy((*dtg)->protocolData->buf,hash,tmp_length);
	(*dtg)->protocolData->size=tmp_length+1;
	return 0;
}
