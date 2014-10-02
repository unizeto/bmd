#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>


/**
*Funkcja pobiera dane asn1 z gniazda
*@param handler uchwyt wskazujący gniazdo połaczenia
*@param buf bufor do którego zostaną zapisane otrzymane dane
*@retval >0 długość otrzymanych danych
*@retval BMD_ERR_NET_READ bład odczytania danych z gniazda
*@retval BMD_ERR_TOO_BIG bład przekroczenia maksymalnego rozmiaru odczytywanych danych
*/
long __bmd_get_metadata(	bmdDatagram_t *dtg,
				char *oid,
				GenBuf_t **value)
{
long i				= 0;
long metadata_found		= 0;
long no_of_metadata		= 0;
MetaDataBuf_t **metadata	= NULL;

	if(dtg==NULL)			{	BMD_FOK_NP(BMD_ERR_OID_NOT_FOUND);	}

	metadata=dtg->sysMetaData;
	no_of_metadata=dtg->no_of_sysMetaData;

	if (metadata)
	{
		for(i=0;i<no_of_metadata;i++)
		{
			if(strcmp(oid,metadata[i]->OIDTableBuf)==0)
			{
				PRINT_DEBUG("LIBBMDPROTOCOLDEBUG Metadata of oid %s found.\n", oid);
				BMD_FOK(set_gen_buf2(metadata[i]->AnyBuf,metadata[i]->AnySize,value));
				metadata_found=1;
				break;
			}

		}

	}

	if(metadata_found==0)
	{
		BMD_FOK_NP(BMD_ERR_OID_NOT_FOUND);
	}

	return BMD_OK;
}

long __bmd_get_metadata_list(	bmdDatagram_t *dtg,
				char *oid,
				GenBufList_t **list)
{
MetaDataBuf_t **metadata	= NULL;
long metadataType		= 0;
long i				= 0;
long metadata_found		= 0;
long no_of_metadata		= 0;


	metadataType=determine_oid_type_str(oid);
	switch(metadataType)
	{
		case SYS_METADATA	:	metadata=dtg->sysMetaData;
						no_of_metadata=dtg->no_of_sysMetaData;
						break;
		case ACTION_METADATA	:	metadata=dtg->actionMetaData;
						no_of_metadata=dtg->no_of_actionMetaData;
						break;
		case ADDITIONAL_METADATA:	metadata=dtg->additionalMetaData;
						no_of_metadata=dtg->no_of_additionalMetaData;
						break;
	}

	*list=(GenBufList_t *)malloc(sizeof(GenBufList_t));
	memset(*list,0,sizeof(GenBufList_t));
	if(metadata)
	{
		for(i=0;i<no_of_metadata;i++)
		{
		        if(strcmp(oid,metadata[i]->OIDTableBuf)==0)
		        {
				(*list)->gbufs=(GenBuf_t **)realloc((*list)->gbufs,sizeof(GenBuf_t *)*((*list)->size+2));
				(*list)->gbufs[(*list)->size]=NULL;
				(*list)->gbufs[(*list)->size+1]=NULL;

				BMD_FOK(set_gen_buf2(metadata[i]->AnyBuf, metadata[i]->AnySize, &((*list)->gbufs[(*list)->size]) ));
				(*list)->size++;
				metadata_found=1;
			}
		}
	}
	else
	{
		return BMD_ERR_OP_FAILED;
	}

	if(metadata_found==0)
	{
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

long bmd2_datagram_get_role_name_list( bmdDatagram_t *dtg,
						GenBufList_t **roleNameList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg,OID_SYS_METADATA_CERT_ROLE_NAME,roleNameList));

	return BMD_OK;
}

long bmd2_datagram_get_role_id_list( bmdDatagram_t *dtg,
						GenBufList_t **roleIdList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg,OID_SYS_METADATA_CERT_ROLE_ID,roleIdList));

	return BMD_OK;
}

/**
*Funkcja bmd2_datagram_get_default_role_index() odnajduje dla domyslnej roli indeks w liscie dostepnych rol
*/
long bmd2_datagram_get_default_role_index(bmdDatagram_t *dtg, GenBufList_t *roleIdList, long *index)
{
	long iter=0;
	long defaultRoleIndexInDatagram=-1;

	if(dtg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(roleIdList == NULL)
		{ return BMD_ERR_PARAM2; }
	if(index == NULL)
		{ return BMD_ERR_PARAM3; }

	for(iter=0; iter<dtg->no_of_sysMetaData; iter++)
	{
		if(strcmp(dtg->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID) == 0)
		{
			defaultRoleIndexInDatagram=iter;
			break;
		}
	}

	if(defaultRoleIndexInDatagram == -1)
	{
		return BMD_ERR_OID_NOT_FOUND;
	}

	for(iter=0; iter<roleIdList->size; iter++)
	{
		if(roleIdList->gbufs[iter]->size == dtg->sysMetaData[defaultRoleIndexInDatagram]->AnySize)
		{
			if(strcmp(roleIdList->gbufs[iter]->buf, dtg->sysMetaData[defaultRoleIndexInDatagram]->AnyBuf) == 0)
			{
				*index=iter;
				return BMD_OK;
			}
		}
	}

	return BMD_ERR_OP_FAILED;
}

/**
*Funkcja bmd2_datagram_get_current_role_index() odnajduje dla aktualnej roli indeks w liscie dostepnych rol
*/
long bmd2_datagram_get_current_role_index(bmdDatagram_t *dtg, GenBufList_t *roleIdList, long *index)
{
	long iter=0;
	long currentRoleIndexInDatagram=-1;

	if(dtg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(roleIdList == NULL)
		{ return BMD_ERR_PARAM2; }
	if(index == NULL)
		{ return BMD_ERR_PARAM3; }

	for(iter=0; iter<dtg->no_of_sysMetaData; iter++)
	{
		if(strcmp(dtg->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_CURRENT_ROLE_ID) == 0)
		{
			currentRoleIndexInDatagram=iter;
			break;
		}
	}

	if(currentRoleIndexInDatagram == -1)
	{
		return BMD_ERR_OID_NOT_FOUND;
	}

	for(iter=0; iter<roleIdList->size; iter++)
	{
		if(roleIdList->gbufs[iter]->size == dtg->sysMetaData[currentRoleIndexInDatagram]->AnySize)
		{
			if(strcmp(roleIdList->gbufs[iter]->buf, dtg->sysMetaData[currentRoleIndexInDatagram]->AnyBuf) == 0)
			{
				*index=iter;
				return BMD_OK;
			}
		}
	}

	return BMD_ERR_OP_FAILED;
}


long bmd2_datagram_get_group_name_list( bmdDatagram_t *dtg,
						GenBufList_t **groupNameList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg, OID_SYS_METADATA_CERT_GROUP_NAME, groupNameList));

	return BMD_OK;
}

long bmd2_datagram_get_group_id_list(	bmdDatagram_t *dtg,
					GenBufList_t **groupIdList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg, OID_SYS_METADATA_CERT_GROUP_ID, groupIdList));

	return BMD_OK;
}

long bmd2_datagram_get_class_name_list(	bmdDatagram_t *dtg,
					GenBufList_t **classNameList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg, OID_ACTION_METADATA_CERT_CLASS_NAME, classNameList));

	return BMD_OK;
}


long bmd2_datagram_get_class_id_list(	bmdDatagram_t *dtg,
					GenBufList_t **classIdList)
{
	BMD_FOK(__bmd_get_metadata_list(dtg, OID_ACTION_METADATA_CERT_CLASS_ID, classIdList));

	return BMD_OK;
}

/**
*Funkcja bmd2_datagram_get_default_group_index() odnajduje dla domyslnej grupy indeks w liscie dostepnych grup
*/
long bmd2_datagram_get_default_group_index(	bmdDatagram_t *dtg,
						GenBufList_t *groupIdList,
						long *index)
{
long iter			= 0;
long defaultGroupIndexInDatagram= -1;

	if(dtg == NULL)		{ return BMD_ERR_PARAM1; }
	if(groupIdList == NULL)	{ return BMD_ERR_PARAM2; }
	if(index == NULL)	{ return BMD_ERR_PARAM3; }

	for(iter=0; iter<dtg->no_of_sysMetaData; iter++)
	{
		if(strcmp(dtg->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID) == 0)
		{
			defaultGroupIndexInDatagram=iter;
			break;
		}
	}

	if(defaultGroupIndexInDatagram == -1)
	{
		return BMD_ERR_OID_NOT_FOUND;
	}

	for(iter=0; iter<groupIdList->size; iter++)
	{
		if(groupIdList->gbufs[iter]->size == dtg->sysMetaData[defaultGroupIndexInDatagram]->AnySize)
		{
			if(strcmp(groupIdList->gbufs[iter]->buf, dtg->sysMetaData[defaultGroupIndexInDatagram]->AnyBuf) == 0)
			{
				*index=iter;
				return BMD_OK;
			}
		}
	}

	return BMD_ERR_OP_FAILED;
}

/**
*Funkcja bmd2_datagram_get_current_group_index() odnajduje dla aktualnej grupy indeks w liscie dostepnych grup
*/
long bmd2_datagram_get_current_group_index(	bmdDatagram_t *dtg,
						GenBufList_t *groupIdList,
						long *index)
{
long iter=0;
long currentGroupIndexInDatagram=-1;

	if(dtg == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(groupIdList == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(index == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	for(iter=0; iter<dtg->no_of_sysMetaData; iter++)
	{
		if(strcmp(dtg->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_CURRENT_GROUP_ID) == 0)
		{
			currentGroupIndexInDatagram=iter;
			break;
		}
	}

	if(currentGroupIndexInDatagram == -1)
	{
		return BMD_ERR_OID_NOT_FOUND;
	}

	for(iter=0; iter<groupIdList->size; iter++)
	{
		if(groupIdList->gbufs[iter]->size == dtg->sysMetaData[currentGroupIndexInDatagram]->AnySize)
		{
			if(strcmp(groupIdList->gbufs[iter]->buf, dtg->sysMetaData[currentGroupIndexInDatagram]->AnyBuf) == 0)
			{
				*index=iter;
				return BMD_OK;
			}
		}
	}

	return BMD_ERR_OP_FAILED;
}

long bmd2_datagram_get_current_class_index(	bmdDatagram_t *dtg,
						GenBufList_t *classIdList,
						long *index)
{
long iter=0;
long currentClassIndexInDatagram=-1;

	if(dtg == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(classIdList == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(index == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	for(iter=0; iter<dtg->no_of_actionMetaData; iter++)
	{
		if(strcmp(dtg->actionMetaData[iter]->OIDTableBuf, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID) == 0)
		{
			currentClassIndexInDatagram=iter;
			break;
		}
	}

	if(currentClassIndexInDatagram == -1)
	{

		return BMD_ERR_OID_NOT_FOUND;
	}

	for(iter=0; iter<classIdList->size; iter++)
	{

		if(classIdList->gbufs[iter]->size == dtg->actionMetaData[currentClassIndexInDatagram]->AnySize)
		{

			if(strcmp(classIdList->gbufs[iter]->buf, dtg->actionMetaData[currentClassIndexInDatagram]->AnyBuf) == 0)
			{

				*index=iter;
				return BMD_OK;
			}
		}
	}

	return BMD_ERR_OP_FAILED;
}


long bmd2_datagram_get_k(	bmdDatagram_t *dtg,
					GenBuf_t **k)
{
char k_OID[]	= {OID_SYS_METADATA_DTG_K};

	BMD_FOK(__bmd_get_metadata(dtg,k_OID,k));

	return BMD_OK;
}

long bmd2_datagram_get_i(	bmdDatagram_t *dtg,
					GenBuf_t **i)
{
char i_OID[]	= {OID_SYS_METADATA_DTG_I};

    BMD_FOK(__bmd_get_metadata(dtg,i_OID,i));

    return BMD_OK;
}

long bmd2_datagram_get_n(	bmdDatagram_t *dtg,
					GenBuf_t **n)
{
char n_OID[]	= {OID_SYS_METADATA_DTG_N};

    BMD_FOK(__bmd_get_metadata(dtg,n_OID,n));

    return BMD_OK;
}

long bmd2_datagram_get_ki_mod_n(	bmdDatagram_t *dtg,
						GenBuf_t **ki_mod_n)
{
char ki_mod_n_OID[]	= {OID_SYS_METADATA_DTG_KI_MOD_N};

    BMD_FOK(__bmd_get_metadata(dtg,ki_mod_n_OID,ki_mod_n));

    return BMD_OK;
}

long bmd2_datagram_get_form(	bmdDatagram_t *dtg,
					GenBuf_t **form)
{
char form_OID[]	= {OID_SYS_METADATA_DTG_FORM};

	BMD_FOK(__bmd_get_metadata(dtg,form_OID,form));

	return BMD_OK;
}

long bmd2_datagram_get_oids(	bmdDatagram_t *dtg,
					GenBuf_t **oids)
{
char oids_OID[]	= {OID_SYS_METADATA_DTG_OIDS};

    BMD_FOK(__bmd_get_metadata(dtg,oids_OID,oids));

    return BMD_OK;
}

long bmd2_datagram_get_symkey(	bmdDatagram_t *dtg,
				GenBuf_t **sym_key)
{
char sym_key_OID[]	= {OID_SYS_METADATA_DTG_SYMKEY};

	/************************/
	/* walidacja parametrow */
	/************************/
	if (dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dtg->no_of_sysMetaData<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dtg->sysMetaData==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (sym_key==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*sym_key)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(__bmd_get_metadata(dtg,sym_key_OID,sym_key));

	return BMD_OK;
}

long bmd2_datagram_get_server_label(	bmdDatagram_t *dtg,
							char **server_label)
{
char server_label_OID[]	= {OID_SYS_METADATA_SERVER_LABEL};
GenBuf_t *GenBuf_temp = NULL;

	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	BMD_FOK(__bmd_get_metadata(dtg,server_label_OID, &GenBuf_temp));
	*server_label = GenBuf_temp->buf;
	GenBuf_temp->buf = NULL;
	GenBuf_temp->size = 0;
	free_gen_buf(&GenBuf_temp);

	return BMD_OK;
}

long bmd2_datagram_get_actions(	bmdDatagram_t *dtg,
						long **actions,
						long *no_of_actions)
{
char action_OID[]		= {OID_SYS_METADATA_ACTION};
GenBufList_t *list	= NULL;
long i			= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*no_of_actions)=0;

	BMD_FOK_NP(__bmd_get_metadata_list(dtg, action_OID, &list));

	(*actions)=(long *)malloc(sizeof(long)*(list->size+1));
	memset((*actions), 0, sizeof(long)*(list->size+1));
	(*no_of_actions)=list->size;


	for (i=0; i<list->size; i++)
	{
		(*actions)[i]=strtol((char *)list->gbufs[i]->buf, (char**)NULL, 10);
		free_gen_buf(&(list->gbufs[i]));
	}

	free(list); list=NULL;

	return BMD_OK;
}

long bmd2_datagram_get_symkey_hash(bmdDatagram_t *dtg,GenBuf_t **sym_key_hash)
{
char symkey_hash_OID[]	= {OID_SYS_METADATA_DTG_SYMKEY_HASH};

    if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

    BMD_FOK(__bmd_get_metadata(dtg,symkey_hash_OID,sym_key_hash));

    return BMD_OK;
}



long bmd2_datagram_get_cert_login_as(	bmdDatagram_t *dtg,
					GenBuf_t **cert_login_as)
{
char cert_login_as_OID[]={OID_SYS_METADATA_CERT_LOGIN_AS};
long status;

	if(cert_login_as==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*cert_login_as)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}


	if (*cert_login_as==NULL)
	{

		status=__bmd_get_metadata(dtg,cert_login_as_OID,cert_login_as);
		if(status==BMD_ERR_OID_NOT_FOUND)
		{

			*cert_login_as = NULL;
			return BMD_OK;
		}
	}

	BMD_FOK(status);

	return BMD_OK;
}
