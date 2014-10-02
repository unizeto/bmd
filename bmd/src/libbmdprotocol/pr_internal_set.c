#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/**
*Funkcja definiuje role z jakimi łaczy się klient do BMD
*@param *role_list wskaźnik do lity ról z jakiemi ma się łączyuć klient
*@param **dtg wskaźnik do datagramu, do którego mają zostać dodane role
*@retval BMD_OK prawidłowe wykonanie funkcji
*@retval BMD_ERR_PARAM1 błędny 1-wszy parametr
*@retval BMD_ERR_PARAM2 błędny 2-gi parametr
*@retval BMD_ERR_OP_FAILED błędne wykonanie dodania roli do datagramu
*/
long bmd2_datagram_set_role_list(	GenBufList_t *role_list,
					bmdDatagram_t **dtg)
{
long i		= 0;
char *roleId	= NULL;

	if(role_list==NULL)		{	return BMD_OK;	}
	if(role_list->size==0)		{	return BMD_OK;	}

	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0;i<role_list->size;i++)
	{
		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserRolesDictionary, 3, role_list->gbufs[i]->buf, 2, &roleId), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_ROLE_ID, roleId, dtg));
		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_ROLE_NAME, role_list->gbufs[i]->buf, dtg));
		free0(roleId);
	}

	/*************/
	/* porzadki */
	/*************/
	free0(roleId);

	return BMD_OK;
}

long bmd2_datagram_set_group_list(	GenBufList_t *group_list,
					bmdDatagram_t **dtg)
{
long i		= 0;
char *groupId	= NULL;

	if(group_list==NULL)		{	return BMD_OK;	}
	if(group_list->size==0)		{	return BMD_OK;	}

	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0;i<group_list->size;i++)
	{
		//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserGroupsDictionary, 3, group_list->gbufs[i]->buf, 2, &groupId), LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND);
		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_ID, groupId, dtg));
		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_NAME, group_list->gbufs[i]->buf, dtg));
		free0(groupId);
	}

	/*************/
	/* porzadki */
	/*************/
	free0(groupId);

	return BMD_OK;
}

long bmd2_datagram_set_class_list(	GenBuf_t *userSerial,
				 	GenBuf_t *userCert_dn,
					bmdDatagram_t **dtg)
{
char *tmp			= NULL;
char **anss			= NULL;
char *userId			= NULL;
char *className			= NULL;
char *classDef			= NULL;
long ansCount			= 0;
long i				= 0;
long defaultClassDefined	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (userSerial==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userCert_dn==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&tmp, "%s%s", userCert_dn->buf, userSerial->buf);
	if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/***************************/
	/* pobranie id uzytkownika */
	/***************************/
	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn::varchar||cert_serial::varchar, accepted FROM users;
	BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UsersDictionary, 5, tmp, 0, &userId), LIBBMDSQL_DICT_USER_VALUE_NOT_FOUND);
	if (userId==NULL)	{	return BMD_OK;	}
	free0(tmp);

	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	BMD_FOK_CHG(getColumnWithCondition(	_GLOBAL_UserClassesDictionary, 0, userId, 2, &ansCount, &anss), LIBBMDSQL_USER_NO_CLASS_DEFINED);

	for (i=0; i<ansCount; i++)
	{

		//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
		BMD_FOK_CHG(getElementWithCondition( _GLOBAL_UserClassesDictionary, 2, anss[i], 3, &className), LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND);

		asprintf(&tmp, "%s|%s", userId, anss[i]);
		if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

		//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
		BMD_FOK_CHG(getElementWithCondition( _GLOBAL_UserClassesDictionary, 4, tmp, 1, &classDef), LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND);

		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CLASS_ID, anss[i], dtg));
		BMD_FOK(bmd2_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CLASS_NAME, className, dtg));

		if (strcmp(classDef, "1")==0)
		{
			BMD_FOK(bmd2_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID, anss[i], dtg));
			BMD_FOK(bmd2_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_DEFAULT_CLASS_NAME, className, dtg));
			defaultClassDefined=1;
		}

		free0(anss[i]);
		free0(classDef);
		free0(className);
	}

	if (defaultClassDefined==0)	{	BMD_FOK(LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED);	}

	/************/
	/* porzadki */
	/************/
	free0(tmp);
	free0(userId);
	free0(anss);
	free0(classDef);
	free0(className);

	return BMD_OK;
}

long bmd2_datagram_set_security_list(	GenBufList_t *security_list,
							bmdDatagram_t **dtg)
{
long i		= 0;

	if(security_list==NULL)		{	return BMD_OK;	}
	if(security_list->size==0)	{	return BMD_OK;	}

	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0;i<security_list->size;i++)
	{
		BMD_FOK(PR2_bmdDatagram_add_metadata(OID_RFC3281_CLEARANCE,security_list->gbufs[i]->buf, security_list->gbufs[i]->size,SYS_METADATA,*dtg,0,0,0, 0));
	}

	return BMD_OK;
}

long bmd2_datagram_set_k(	GenBuf_t *k,
					bmdDatagram_t **dtg)
{
char k_OID[]	= {OID_SYS_METADATA_DTG_K};

	/************************/
	/* walidacja parametrow */
	/************************/
	if(k==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(k->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(k->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(PR2_bmdDatagram_add_metadata(k_OID,k->buf,k->size,SYS_METADATA,*dtg,0,0,0, 0));

	return BMD_OK;
}

long bmd2_datagram_set_i(	GenBuf_t *i,
					bmdDatagram_t **dtg)
{
long status;
char i_OID[]	= {OID_SYS_METADATA_DTG_I};

    if(i==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(i->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(i->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}

    if(dtg==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
    if((*dtg)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

    status=PR2_bmdDatagram_add_metadata(i_OID,i->buf,i->size,SYS_METADATA,*dtg,0,0,0, 0);
    if(status!=0)	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

    return BMD_OK;
}

long bmd2_datagram_set_n(	GenBuf_t *n,
					bmdDatagram_t **dtg)
{
long status;
char n_OID[]	= {OID_SYS_METADATA_DTG_N};

    if(n==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(n->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(n->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
    if((*dtg)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

    status=PR2_bmdDatagram_add_metadata(n_OID,n->buf,n->size,SYS_METADATA,*dtg,0,0,0, 0);
    if(status!=0)	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

    return BMD_OK;
}

long bmd2_datagram_set_ki_mod_n(	GenBuf_t *ki_mod_n,
						bmdDatagram_t **dtg)
{
long status;
char ki_mod_n_OID[]	= {OID_SYS_METADATA_DTG_KI_MOD_N};

	if(ki_mod_n==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ki_mod_n->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ki_mod_n->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	status=PR2_bmdDatagram_add_metadata(ki_mod_n_OID,ki_mod_n->buf,ki_mod_n->size,SYS_METADATA,*dtg,0,0,0, 0);
	if(status!=0)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	return BMD_OK;
}

long bmd2_datagram_set_form(	GenBuf_t *form,
					bmdDatagram_t **dtg)
{
char form_OID[]	= {OID_SYS_METADATA_DTG_FORM};

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(form==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(form->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(form->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(PR2_bmdDatagram_add_metadata(form_OID,form->buf,form->size,SYS_METADATA,*dtg,0,0,0, 0));

	return BMD_OK;
}

long bmd2_datagram_set_oids(	GenBuf_t *oids,
					bmdDatagram_t **dtg)
{
long status;
char oids_OID[]={OID_SYS_METADATA_DTG_OIDS};

    if(oids==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(oids->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(oids->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}

    if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
    if((*dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

    status=PR2_bmdDatagram_add_metadata(oids_OID,oids->buf,oids->size,SYS_METADATA,*dtg,0,0,0, 0);
    if(status!=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

    return BMD_OK;
}

long __set_symkey_as_mtd(	GenBuf_t *sym_key,
					MetaDataBuf_t **mtd_buf)
{
char sym_key_OID[]={OID_SYS_METADATA_DTG_SYMKEY};

	(*mtd_buf)=(MetaDataBuf_t *)malloc(sizeof(MetaDataBuf_t));
	memset((*mtd_buf),0,sizeof(MetaDataBuf_t));

	(*mtd_buf)->OIDTableBuf=(char *)malloc(sizeof(char)*(strlen(sym_key_OID)+1));
	memset((*mtd_buf)->OIDTableBuf, 0, sizeof(char)*(strlen(sym_key_OID)+1));
	memcpy((*mtd_buf)->OIDTableBuf, sym_key_OID, sizeof(char)*(strlen(sym_key_OID)));

	(*mtd_buf)->AnyBuf=(char *)malloc(sym_key->size);
	memmove((*mtd_buf)->AnyBuf,sym_key->buf,sym_key->size);

	(*mtd_buf)->AnySize=sym_key->size;

	return BMD_OK;
}

long bmd2_datagram_set_symkey(	GenBuf_t *sym_key,
						bmdDatagram_t **dtg)
{
	if(sym_key==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if((*dtg)->sysMetaData==NULL)
	{
		PRINT_VDEBUG("Ustawiam klucz jako pierwsza metadana\n");
		(*dtg)->sysMetaData=(MetaDataBuf_t **)malloc(sizeof(MetaDataBuf_t *)*1);
		__set_symkey_as_mtd(sym_key,&((*dtg)->sysMetaData[0]));
		(*dtg)->no_of_sysMetaData=1;
	}
	else
	{
		PRINT_VDEBUG("Ustawiam klucz jako kolejna metadana\n");
		(*dtg)->sysMetaData=realloc((*dtg)->sysMetaData,((*dtg)->no_of_sysMetaData+1)*sizeof(MetaDataBuf_t));
		__set_symkey_as_mtd(sym_key,&((*dtg)->sysMetaData[(*dtg)->no_of_sysMetaData]));
		(*dtg)->no_of_sysMetaData++;
	}

	return BMD_OK;
}

/**
* Funkcja wyszukuje i podmienia w datagramie klucz symetryczny. Jesli w datagramie nie byl ustawiony klucz symetryczny, 
* to przekazywany do funkcji klucz symetryczny zostanie dodany do datagramu.
*/
long bmd2_datagram_update_symkey(	GenBuf_t *sym_key,
						bmdDatagram_t **dtg)
{
long i						= 0;
char sym_key_OID[]	= {OID_SYS_METADATA_DTG_SYMKEY};
long symkeyUpdatedFlag	= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(sym_key==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key->size<=0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)->sysMetaData==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)->no_of_sysMetaData<=0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0;i<(*dtg)->no_of_sysMetaData;i++)
	{
		if(strcmp(sym_key_OID,(*dtg)->sysMetaData[i]->OIDTableBuf)==0)
		{
			free((*dtg)->sysMetaData[i]->AnyBuf);
			(*dtg)->sysMetaData[i]->AnyBuf=(char *)malloc(sym_key->size+2);
			memset((*dtg)->sysMetaData[i]->AnyBuf,0,sym_key->size+1);
			memmove((*dtg)->sysMetaData[i]->AnyBuf,sym_key->buf,sym_key->size);
			(*dtg)->sysMetaData[i]->AnySize=sym_key->size;

			symkeyUpdatedFlag=1;
			break;
		}
	}

	if(symkeyUpdatedFlag == 0)
	{
		BMD_FOK(bmd2_datagram_set_symkey(sym_key, dtg));
	}
	
	return BMD_OK;
}

long bmd2_datagram_set_server_label(	char *server_label,
						bmdDatagram_t **dtg)
{
char server_label_OID[]	= {OID_SYS_METADATA_SERVER_LABEL};

	/************************/
	/* walidacja parametrow */
	/************************/
	if(server_label==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(PR2_bmdDatagram_add_metadata(server_label_OID,server_label, (long)strlen(server_label),SYS_METADATA,*dtg,0,0,0, 0));

	return BMD_OK;
}

long bmd2_datagram_set_actions(	long *actions,
				long no_of_actions,
				bmdDatagram_t **dtg)
{
char *action_str			= NULL;
long i				= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	for (i=0; i<no_of_actions; i++)
	{
		asprintf(&action_str, "%li", actions[i]);

		BMD_FOK(PR2_bmdDatagram_add_metadata(OID_SYS_METADATA_ACTION, (char*)action_str, (long)strlen(action_str), SYS_METADATA,*dtg,0,0,0, 0));

		free(action_str); action_str=NULL;
	}

	return BMD_OK;
}

long bmd2_datagram_set_current_role_and_group(	char *roleName,
						char *groupId,
						bmdDatagram_t **dtg)
{
char *roleId	= NULL;
char *groupName	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleName==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (groupId==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
	BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserRolesDictionary, 3, roleName, 2, &roleId), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
	BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_ROLE_ID, roleId, dtg));
	BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_ROLE_NAME, roleName, dtg));

	//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);
	BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserGroupsDictionary, 2, groupId, 3, &groupName), LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND);
	BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_GROUP_ID, groupId, dtg));
	BMD_FOK(bmd2_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_GROUP_NAME, groupName, dtg));

	/************/
	/* porzadki */
	/************/
	free0(roleId);
	free0(groupName);

	return BMD_OK;
}

long bmd2_datagram_set_symkey_hash(	GenBuf_t *sym_key_hash,
						bmdDatagram_t **dtg)
{
char symkey_hash_OID[]	= {OID_SYS_METADATA_DTG_SYMKEY_HASH};

	/************************/
	/* walidacja parametrow */
	/************************/
	if(sym_key_hash==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key_hash->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sym_key_hash->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(PR2_bmdDatagram_add_metadata(symkey_hash_OID,sym_key_hash->buf,sym_key_hash->size, SYS_METADATA,*dtg,0,0,0, 0));

	return BMD_OK;
}
