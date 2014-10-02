#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>
#include <bmd/common/bmd-os_specific.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>

#include <bmd/libbmd/libbmd_internal.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include "../klient/vs2005/klient/VersionNo.h"
#include <bmd/libbmderr/libbmderr.h>

long bmd_datagram_create(	const long type,
					bmdDatagram_t **datagram)
{
	PRINT_INFO("LIBBMDINF Creating datagram\n");
	(*datagram)=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	if(*datagram==NULL)
	{
		PRINT_ERROR("LIBBMDERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return -1;
	}
	PR_bmdDatagram_init(*datagram);

	(*datagram)->datagramType=type;
	(*datagram)->protocolVersion=BMD_PROTOCOL_VERSION; //wersja protokolu 1.03.00.00 zdefiniowana w bmd-const.h

	return BMD_OK;
}


long bmd_datagram_add_data(	const GenBuf_t *data,
					const char *data_name,
					bmdDatagram_t **datagram)
{
long tmp_length		= 0;

	PRINT_INFO("LIBBMDINF Adding data to datagram\n");
	BMD_FOK(set_gen_buf(data->buf,(long *)&(data->size),&((*datagram)->protocolData)));

	tmp_length=(long)strlen(data_name);
	BMD_FOK(set_gen_buf((char *)data_name,&tmp_length,&((*datagram)->protocolDataFilename)));

	return BMD_OK;
}


long bmd_datagram_add_file(	const char *file,
					bmdDatagram_t **datagram)
{
long status			= 0;
long tmp_length	= 0;
GenBuf_t *file_content		= NULL;
char *filename			= NULL;

	PRINT_INFO("LIBBMDINF Adding file to datagram\n");
	bmd_load_binary_content(file,&file_content);
	if(file_content==NULL)
		return -1;
	BMD_FOK(set_gen_buf(file_content->buf,(long *)&(file_content->size),&((*datagram)->protocolData)));
	free_gen_buf(&file_content);

	get_filename((char *)file,&filename);
	PRINT_INFO("bmd_datagram_add_file %s\n",filename);
	if(filename==NULL)
		return -3;

	tmp_length=(long)strlen(filename)+1;
	BMD_FOK(set_gen_buf((char *)filename,&tmp_length,&((*datagram)->protocolDataFilename)));
	free(filename);filename=NULL;
	if(status!=0)
		return -4;

	return BMD_OK;
}


/******************************************************
*	okreslenie nazwy pliku przesylanego lobowo
* @param filename to sciezka do pliku (koniecznie zakodowana w UTF-8)
******************************************************/
long bmd_datagram_set_filename(	const char *filename, bmdDatagram_t **dtg)
{
	PRINT_INFO("LIBBMDINF Setting filename in datagram\n");
	/******************************/
	/*	walidacja pametrow	*/
	/******************************/
	if(filename==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(set_gen_buf2(filename,(long)strlen(filename),&((*dtg)->protocolDataFilename)));

	return BMD_OK;
}

long bmd_datagram_add_metadata(	EnumMetaData_t type,
						const char *oid,
						GenBuf_t *value,
						bmdDatagram_t **datagram,
						long twf_copy_ptr)
{
long status			= 0;

	PRINT_INFO("LIBBMDINF Adding metadata to datagram\n");

	if(value == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(datagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }

	if(twf_copy_ptr < 0 || twf_copy_ptr > 1)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	status=PR2_bmdDatagram_add_metadata((char*)oid,value->buf,value->size,type,*datagram,0,0,0,twf_copy_ptr);
	if(twf_copy_ptr == 1)
	{
		value->buf = NULL;
		value->size = 0;
	}

	if(status!=0)
	{
		BMD_FOK(-2);
	}
	return BMD_OK;
}


long bmd_datagram_add_metadata_2(	const char *oid,
					GenBuf_t * value,
					bmdDatagram_t **datagram,
					long twf_copy_ptr)
{
long status				= 0;
long type				= 0;

	//PRINT_INFO("LIBBMDINF Adding metadata to datagram (2)\n");
	if (value == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (datagram == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (twf_copy_ptr < 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (twf_copy_ptr > 1)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	type=determine_oid_type_str((char *)oid);
	BMD_FOK(type);

	status=PR2_bmdDatagram_add_metadata((char *)oid,value->buf,value->size,type,*datagram,0,0,0,twf_copy_ptr);
	if(twf_copy_ptr == 1)
	{
		value->buf = NULL;
		value->size = 0;
	}

	BMD_FOK(status);

	return BMD_OK;
}


long bmd_datagram_add_metadata_char(	const char *oid,
					char *value,
					bmdDatagram_t **datagram)
{
	return bmd2_datagram_add_metadata_char(oid, value, datagram);
}

long bmd_datagram_add_metadata_char_type(	const char *oid,
						long type,
						char *value,
						bmdDatagram_t **datagram)
{
GenBuf_t *gb	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (oid == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (value == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (datagram == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(set_gen_buf2(value, (long)strlen(value), &gb));
	BMD_FOK(PR2_bmdDatagram_add_metadata((char *)oid, gb->buf, gb->size,type, *datagram, 0, 0, 0, 0));

	/************/
	/* porzadki */
	/************/
	free_gen_buf(&gb);

	return BMD_OK;
}

long bmd_datagram_add_metadata_2_with_params(	const char *oid,
								GenBuf_t * value,
								bmdDatagram_t **datagram,
								EnumMetaData_t metadataType,
								long myId,
								long ownerType,
								long ownerId,
								long twf_copy_ptr)
{
long status		= 0;

	PRINT_INFO("LIBBMDINF Adding metadata to datagram (2)\n");

	if(value == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(datagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(twf_copy_ptr < 0 || twf_copy_ptr > 1)	{	BMD_FOK(BMD_ERR_PARAM7);	}

	if (metadataType==UNDEFINED_METADATA)
	{
		metadataType=determine_oid_type_str((char *)oid);
		BMD_FOK(metadataType);
	}

	status=PR2_bmdDatagram_add_metadata((char *)oid,value->buf,value->size,metadataType,*datagram, myId, ownerType, ownerId, twf_copy_ptr);
	if(twf_copy_ptr == 1)
	{
		value->buf = NULL;
		value->size = 0;
	}

	BMD_FOK(status);

	return BMD_OK;
}


long bmd_datagram_get_metadata(	EnumMetaData_t type,
				const char *oid,
				bmdDatagram_t *datagram,
				long pos,
				GenBuf_t **metadata_value)
{
long i				= 0;
long metadata_found		= 0;
long no_of_metadata		= 0;
MetaDataBuf_t **metadata	= NULL;

	PRINT_INFO("LIBBMDINF Getting metadata from datagram\n");

	if(metadata_value == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(*metadata_value != NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	switch(type)
	{
		case ADDITIONAL_METADATA:
			metadata=datagram->additionalMetaData;
			no_of_metadata=datagram->no_of_additionalMetaData;

			break;
		case ACTION_METADATA:
			metadata=datagram->actionMetaData;
			no_of_metadata=datagram->no_of_actionMetaData;

			break;
		case SYS_METADATA:
			metadata=datagram->sysMetaData;
			no_of_metadata=datagram->no_of_sysMetaData;

			break;
		case PKI_METADATA:
			metadata=datagram->pkiMetaData;
			no_of_metadata=datagram->no_of_pkiMetaData;

			break;
		default:

			return -2;
	}

	if(metadata)
	{
		for(i=0;i<no_of_metadata;i++)
		{
			if(strcmp(oid,metadata[i]->OIDTableBuf)==0)
			{
				metadata_found++;
				if ((metadata_found==pos) || (pos==0) || (pos==-1))
				{
					BMD_FOK(set_gen_buf2(metadata[i]->AnyBuf, metadata[i]->AnySize, metadata_value));
					break;
				}
			}
		}
	}
	else
	{
		return -4;
	}

	if (pos>0)
	{
		if((metadata_found==0) || (metadata_found!=pos))
		{
			return -5;
		}
	}
	else
	{
		if(metadata_found==0)
		{
			return -5;
		}
	}

	return BMD_OK;
}

long bmd_datagram_get_metadata_ownerType(	EnumMetaData_t type,
							const char *oid,
							bmdDatagram_t *datagram,
							long pos,
							long *ownerType)
{
long i				= 0;
long metadata_found		= 0;
long no_of_metadata		= 0;
MetaDataBuf_t **metadata	= NULL;

	PRINT_INFO("LIBBMDINF Getting metadata's ownerType from datagram\n");

	switch(type)
	{
		case ADDITIONAL_METADATA:
			metadata=datagram->additionalMetaData;
			no_of_metadata=datagram->no_of_additionalMetaData;

			break;
		case ACTION_METADATA:
			metadata=datagram->actionMetaData;
			no_of_metadata=datagram->no_of_actionMetaData;

			break;
		case SYS_METADATA:
			metadata=datagram->sysMetaData;
			no_of_metadata=datagram->no_of_sysMetaData;

			break;
		case PKI_METADATA:
			metadata=datagram->pkiMetaData;
			no_of_metadata=datagram->no_of_pkiMetaData;

			break;
		default:
			return -2;
	}

	if(metadata)
	{
		for(i=0;i<no_of_metadata;i++)
		{
			if(strcmp(oid,metadata[i]->OIDTableBuf)==0)
			{
				metadata_found++;
				if ((metadata_found==pos) || (pos==0) || (pos==-1))
				{
					(*ownerType)=metadata[i]->ownerType;
				}
			}
		}
	}
	else
	{
		return -4;
	}
	if (pos>0)
	{
		if((metadata_found==0) || (metadata_found!=pos))
		{
			return -5;
		}
	}
	else
	{
		if(metadata_found==0)
		{
			return -5;
		}
	}

	return BMD_OK;
}


long bmd_datagram_get_metadata_2(	const char *oid,
						bmdDatagram_t *datagram,
						GenBuf_t **metadata_value)
{
long type				= 0;

	PRINT_INFO("LIBBMDINF Getting metadata from datagram (2)\n");

	if (oid==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (datagram==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (metadata_value == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*metadata_value != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	type=determine_oid_type_str((char *)oid);
	BMD_FOK(type);

	BMD_FOK(bmd_datagram_get_metadata(type,oid,datagram,0,metadata_value));

	return BMD_OK;
}

long bmd_datagram_get_id(	const bmdDatagram_t *datagram,
					long *id)
{
long tmp		= 0;

	PRINT_INFO("LIBBMDINF Getting id from datagram\n");
	if(datagram && id)
	{
		if(datagram->protocolDataFileId)
		{
			tmp=strtol((const char *)datagram->protocolDataFileId->buf,(char **)NULL,10);
			*id=tmp;
		}
		else
			return -2;
	}
	else
		return -1;

	return BMD_OK;
}


long bmd_datagram_set_id(	const long *id,
					bmdDatagram_t **datagram)
{
long length	= 0;
char *tmp		= NULL;

	PRINT_INFO("LIBBMDINF Setting id in datagram\n");
	if(id)
	{
		if( (*id)>0)
		{
			asprintf(&tmp,"%li",*id);
			length=(long)strlen(tmp)+1;
			BMD_FOK(set_gen_buf((char *)tmp,&length,&((*datagram)->protocolDataFileId)));
			free(tmp);tmp=NULL;
		}
		else
			return -2;
	}
	else
		return -1;

	return BMD_OK;
}


long bmd_datagram_get_filename(	bmdDatagram_t *dtg,
						char **filename)
{
	PRINT_INFO("LIBBMDINF Getting filename from datagram\n");
	if(dtg==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(filename==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*filename)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(dtg->protocolDataFilename==NULL)	{	BMD_FOK(BMD_ERR_FORMAT);	}

	BMD_FOK(genbuf_to_char(dtg->protocolDataFilename,filename));

	return BMD_OK;
}


long bmd_datagram_save_file(	const char *filename,
					const bmdDatagram_t *datagram)
{
long fd			= 0;
long nwrite			= 0;
char *name_pointer	= NULL;
char *name_from_datagram= NULL;
mode_t old_mask;

	PRINT_INFO("LIBBMDINF Saving file from datagram\n");
	if(datagram->protocolData==NULL)
		return -1;
	if(datagram->protocolDataFilename==NULL)
		return -2;

	if(filename==NULL)
	{
		genbuf_to_char(datagram->protocolDataFilename,&name_from_datagram);
		name_pointer=name_from_datagram;
	}
	else
		name_pointer=(char *)filename;

	old_mask=umask(0);
	fd=open(name_pointer,O_CREAT|O_WRONLY|O_BINARY|O_TRUNC,0600);
	if(fd!=-1)
	{
		nwrite=write(fd,datagram->protocolData->buf,datagram->protocolData->size);
		if(nwrite!=datagram->protocolData->size)
			return -2;
		close(fd);
	}
	else
	{
		PRINT_ERROR("Blad: Nie mozna otworzyc pliku %s\n",name_pointer);
		return -1;
	}
	if(name_from_datagram)
	{
		free(name_from_datagram);name_from_datagram=NULL;
	}
	return BMD_OK;
}


long bmd_datagram_get_file(	bmdDatagram_t *dtg,
					GenBuf_t **file)
{
	PRINT_INFO("LIBBMDINF Getting file from datagram\n");

	if(dtg==NULL)
		return BMD_ERR_PARAM1;
	if(file==NULL)
		return BMD_ERR_PARAM2;
	if((*file)!=NULL)
		return BMD_ERR_PARAM2;

	if(dtg->protocolData==NULL)
		return BMD_ERR_FORMAT;

	(*file)=dtg->protocolData;

	return BMD_OK;
}


long bmd_datagram_free(	bmdDatagram_t **datagram)
{
	PR2_bmdDatagram_free(datagram);
	return BMD_OK;
}


long bmd_datagram_list_free(	bmdDatagram_t ***datagrams)
{
long i		= 0;

	while((*datagrams)[i]!=NULL)
	{
		bmd_datagram_free(&((*datagrams)[i]));
		i++;
	}
	free(*datagrams);
	*datagrams=NULL;
	return BMD_OK;
}


long bmd_datagram_add_to_set(	bmdDatagram_t *datagram,
					bmdDatagramSet_t **datagrams_set)
{
long status		= 0;

	status=PR2_bmdDatagramSet_add(datagram,datagrams_set);
	if(status!=BMD_OK)
		return BMD_ERR_OP_FAILED;

	return status;
}


long bmd_datagram_add_limits(	long from,
					long offset,
					bmdDatagram_t **datagram)
{
long length		= 0;
char *tmp		= NULL;

	if(from<0)
		return -1;
	if(offset<0)
		return -1;

	asprintf(&tmp,"%li|%li",from,offset);

	length=(long)strlen(tmp);
	length++;
	BMD_FOK(set_gen_buf((char *)tmp,&length,&((*datagram)->protocolDataFileId)));
	free(tmp);tmp=NULL;

	return 0;
}


long bmd_datagram_set_status(	long status,
				long sql_operator,
				bmdDatagram_t **datagram)
{
    	if(datagram==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
    	if((*datagram)==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	switch(status)
	{
        	case BMD_ONLY_OWNER_DOC:
             		(*datagram)->datagramStatus=BMD_REQ_DB_SELECT_ONLY_OWNER;
            		break;
        	case BMD_ONLY_GROUP_DOC:
             		(*datagram)->datagramStatus=BMD_REQ_DB_SELECT_ALL_SYSMTD;
            		break;
		case BMD_ONLY_CREATOR_DOC:
             		(*datagram)->datagramStatus=BMD_REQ_DB_SELECT_ONLY_CREATOR;
			break;
		case BMD_OWNER_OR_CREATOR_DOC:
			(*datagram)->datagramStatus=BMD_REQ_DB_SELECT_CREATOR_OR_OWNER;
			break;
        	default:
            		BMD_FOK(BMD_ERR_PARAM1);
    	}

	switch(sql_operator)
	{
		case BMD_QUERY_ILIKE:
			(*datagram)->datagramStatus|=BMD_REQ_DB_SELECT_ILIKE_QUERY;
			break;
		case BMD_QUERY_LIKE:

			(*datagram)->datagramStatus|=BMD_REQ_DB_SELECT_LIKE_QUERY;
			break;
		case BMD_QUERY_EXACT_ILIKE:

			(*datagram)->datagramStatus|=BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY;
			break;
		case BMD_QUERY_EQUAL:

			(*datagram)->datagramStatus|=BMD_REQ_DB_SELECT_EQUAL_QUERY;
			break;
		default:

			BMD_FOK(BMD_ERR_PARAM2);
    }

    return BMD_OK;
}


long bmd_datagram_determine_pki_metadata_types(	bmdDatagram_t *datagram)
{
long was_signature		= 0;
long was_timestamp		= 0;
long i				= 0;

char signature_oid[]		= {OID_PKI_METADATA_SIGNATURE};
char timestamp_oid[]		= {OID_PKI_METADATA_TIMESTAMP};
char timestamp_from_sig_oid[]	= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};


	if(datagram->no_of_pkiMetaData==0)
	{
		return PLAIN_PKI_METADATA;
	}
	else
	{
		for(i=0;i<datagram->no_of_pkiMetaData;i++)
		{
			if(!strcmp(datagram->pkiMetaData[i]->OIDTableBuf,signature_oid))
			{
				was_signature=1;
			}

			if(	!strcmp(datagram->pkiMetaData[i]->OIDTableBuf,timestamp_oid) ||
 				!strcmp(datagram->pkiMetaData[i]->OIDTableBuf, timestamp_from_sig_oid))
			{
				was_timestamp=1;
			}

		}
		if( (was_signature==0) && (was_timestamp==0) )
		{
			return PLAIN_PKI_METADATA;
		}
		if( (was_signature==0) && (was_timestamp==1) )
		{
			return TIMESTAMP_PKI_METADATA;
		}
		if( (was_signature==1) && (was_timestamp==0) )
		{
			return SIGNATURE_PKI_METADATA;
		}
		if( (was_signature==1) && (was_timestamp==1) )
		{
			return SIG_TS_PKI_METADATA;
		}
	}

	return BMD_OK;
}
