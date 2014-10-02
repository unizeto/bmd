#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>

/******************************************************************/
/*	przygotowanie struktur do zalogowania do serwer BMD		*/
/*	oraz zalogowanie sie do bmd w imieniu uzytkownika soap	*/
/******************************************************************/
long PrepareDataForBMDLoginAndRequest(	bmd_info_t **bi,
					char* binaryCert,
					long binaryCertLen,
					char *roleId,
					char *groupId,
					long deserialisation_max_memory)
{
long status				= 0;
long bmd_port 				= 0;
char *bmd_adr				= NULL;
char *der				= NULL;
char *pfx				= NULL;
char *pfx_pass				= NULL;
char *bmd_cert				= NULL;
GenBuf_t *cert_login_as2		= NULL;
GenBuf_t cert_login_as;
GenBuf_t *cert_owner			= NULL;


	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (_GLOBAL_soap_certificate_required==1)
	{
		if (binaryCert==NULL || binaryCertLen <= 0)
		{
			PRINT_ERROR("Empty login certificate. Error %i\n", BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE);
			return BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE;
		}
	}
	else
	{
		if (binaryCert==NULL)
		{
			status=bmdconf_get_value(_GLOBAL_konfiguracja,"bmd","der",&der);
			if (status<BMD_OK)
			{
				PRINT_ERROR("Empty login certificate. Error %i\n", BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE);
				return BMDSOAP_SERVER_CONF_READ_ERROR;
			}
		}
	}

	/************************************/
	/*	inicjalizowanie bibliotek bmd	*/
	/************************************/
	status=bmd_init();
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_BMD_INIT_ERROR;
	}

	/************************************/
	/*	stworzenie struktury BMD	*/
	/************************************/
	status=bmd_info_create(bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_BMD_INFO_CREATE_ERROR;
	}

	/************************************************************/
	/*	wczytanie ustawien konfiguracyjnych serwera SOAP	*/
	/************************************************************/
	status=bmdconf_get_value(_GLOBAL_konfiguracja,"bmd","adr",&bmd_adr);
	if (status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","port",&bmd_port);
	if (status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_konfiguracja,"bmd","pfx",&pfx);
	if (status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_konfiguracja,"bmd","pfx_pass",&pfx_pass);
	if (status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_konfiguracja,"bmd","bmd_cert",&bmd_cert);
	if (status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	/******************************************/
	/*	przygotowanie zestawu do logowania	*/
	/******************************************/
	status=bmd_info_login_set(bmd_adr,(int)bmd_port,NULL,-1,NULL,NULL,BMDNET_PROXY_NONE,NULL,bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_LOGIN_SET_ERROR;
	}

	/******************************************************/
	/*	przygotowanie struktur certyfikatu logujacego	*/
	/******************************************************/
	status=bmd_info_set_credentials_pfx(pfx,pfx_pass,strlen(pfx_pass),bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_PFX_SET_ERROR;
	}

	status=bmd_info_set_bmd_cert(bmd_cert,bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_BMD_CERT_SET_ERROR;
	}

	/******************************************************/
	/*	ustawienie certyfikatu logujacego klienta		*/
	/*	soap, w imieniu ktorego loguje sie serwer soap	*/
	/******************************************************/
	if (_GLOBAL_soap_certificate_required==1)
	{
		cert_login_as.buf=binaryCert;
		cert_login_as.size=binaryCertLen;
	}
	else
	{
		if (binaryCert==NULL)
		{
			status=bmd_load_binary_content(der,&cert_login_as2);
			if(status < BMD_OK)
			{
				bmd_end();
				return BMDSOAP_SERVER_NO_DEFAULT_LOGIN_CERTIFICATE;
			}
			cert_login_as.buf=(char *)cert_login_as2->buf;
			cert_login_as.size=cert_login_as2->size;
		}
		else
		{
			cert_login_as.buf=binaryCert;
			cert_login_as.size=binaryCertLen;
		}
	}

	if(cert_login_as.buf == NULL || cert_login_as.size <= 0)
	{
		PRINT_ERROR("Get owner from certificate error. Error %li\n", status);
		bmd_end();
		return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
	}

	/******************************************/
	/*	wydobycie podmiotu z certyfikatu	*/
	/******************************************/
	status=get_owner_from_cert(&cert_login_as,&cert_owner);
	if(status<BMD_OK)
	{
		PRINT_ERROR("Get owner from certificate error. Error %li\n", status);
		bmd_end();
		return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
	}

	PRINT_SOAP("User:\t\t%s\n",cert_owner->buf);

	status=bmd_info_set_cert_login_as(&cert_login_as,bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return BMDSOAP_SERVER_PFX_AS_SET_ERROR;
	}

	/******************************/
	/* zalogowanie do serwera BMD */
	/******************************/
	status=bmd_login(bi);
	if(status<BMD_OK)
	{
		bmd_end();
		return status;
	}

	/************************************/
	/* ewentualna zmiana grupy lub roli */
	/************************************/
	if ((roleId!=NULL) || (groupId!=NULL))
	{

 		status=chooseClientRole( roleId, groupId, bi, deserialisation_max_memory);
		if(status<BMD_OK)
		{
			bmd_end();
			return status;
		}
	}

	/************/
	/* porzadki */
	/************/
	free0(bmd_adr);
	free0(der);
	free0(pfx);
	free0(pfx_pass);
	free0(bmd_cert);
	free_gen_buf(&cert_login_as2);

	return SOAP_OK;
}

long chooseClientRole(	char *roleId,
			char *groupId,
			bmd_info_t **bi,
			long deserialisation_max_memory)
{
GenBuf_t *der_form			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (	(roleId==NULL) &&
		(groupId==NULL))	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	/**********************/
	/* stworzenie zadania */
	/**********************/
	BMD_FOK_NP(bmd_datagram_create(BMD_DATAGRAM_TYPE_CHOOSE_ROLE,&dtg));

	if (roleId!=NULL)
	{
		BMD_FOK_NP(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_ROLE_ID, roleId, &dtg));
	}

	if (groupId!=NULL)
	{
		BMD_FOK_NP(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CURRENT_GROUP_ID, groupId, &dtg));
	}

	BMD_FOK_NP(bmd_datagram_add_to_set(dtg,&dtg_set));

	/********************/
	/* wyslanie zadania */
	/********************/
	BMD_FOK_NP(bmd_send_request(*bi,dtg_set, 100, &resp_dtg_set,1, deserialisation_max_memory));

	/************************/
	/* odebranie odpowiedzi */
	/************************/
	BMD_FOK_NP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/******************************************/
	/* pobranie nowego formularza z datagramu */
	/******************************************/
	free_BMD_attr_list(&((*bi)->form_info.user_form));
	free_BMD_attr_list(&((*bi)->form_info.user_send_form));
	free_BMD_attr_list(&((*bi)->form_info.user_search_form));
	free_BMD_attr_list(&((*bi)->form_info.user_view_form));
	free_BMD_attr_list(&((*bi)->form_info.user_unvisible_form));
	free_BMD_attr_list(&((*bi)->form_info.user_update_form));
	free_BMD_attr_list(&((*bi)->form_info.user_history_form));

	BMD_FOK(bmd2_datagram_get_form(resp_dtg_set->bmdDatagramSetTable[0], &der_form));
	BMD_FOK(bmd_kontrolki_deserialize(der_form, &((*bi)->form_info.user_form)));

	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), SEND_KONTROLKI, &((*bi)->form_info.user_send_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), GRID_KONTROLKI, &((*bi)->form_info.user_view_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), SEARCH_KONTROLKI, &((*bi)->form_info.user_search_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), UNVISIBLE_KONTROLKI, &((*bi)->form_info.user_unvisible_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), UPDATE_KONTROLKI, &((*bi)->form_info.user_update_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bi)->form_info.user_form), HISTORY_KONTROLKI, &((*bi)->form_info.user_history_form)));

	/************/
	/* porzadki */
	/************/
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&der_form);

	return BMD_OK;

}

/******************************************************************************/
/*	obsluga zadania sprawdzenia poprawnosci polaczenia z serwerem soap	*/
/******************************************************************************/
int bmd230__testConnection(	struct soap *soap,
				char **result)
{
	PRINT_SOAP("Request:\ttest connection\n");
	*result = (char*) soap_malloc(soap, 100);
	sprintf(*result, "Connected to server. %i",1);
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/************************************************************/
/*	bezpieczne logowanie do BMD i pobranie formularza	*/
/************************************************************/
int bmd230__bmdLogin(	struct soap *soap,
			struct xsd__base64Binary *binaryCert,
			char *userClassId,
			char *roleName,
			char *groupName,
			struct bmd230__loginResponse **loginResponse)
{
long form_count				= 0;
long i					= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
BMD_attr_t **form			= NULL;
char *oid				= NULL;
char *desc				= NULL;

long retVal=0;
char *derUserCert=NULL;
long derUserCertLen=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tlogin and get metadata list\n");

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(binaryCert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
			}
		}
		else
		{
			if(binaryCert != NULL)
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
			}
		}
	}

	/********************/
	/* logowanie do bmd */
	/********************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory) );
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/*********************************/
	/* alokacja pamieci na odpowiedz */
	/*********************************/
	(*loginResponse)=(struct bmd230__loginResponse *)malloc(sizeof(struct bmd230__loginResponse));
	memset((*loginResponse), 0, sizeof(struct bmd230__loginResponse));

	/*************************************/
	/* pobranie informacji o uzytkowniku */
 	/*************************************/
	(*loginResponse)->userInfo=(struct bmd230__singleUserInfo *)malloc(sizeof(struct bmd230__singleUserInfo));
	(*loginResponse)->userInfo->userId=NULL;
	(*loginResponse)->userInfo->userCertIssuer=NULL;
	(*loginResponse)->userInfo->userCertSerial=NULL;
	(*loginResponse)->userInfo->userName=NULL;
	(*loginResponse)->userInfo->userIdentity=NULL;
	(*loginResponse)->userInfo->userAccepted=NULL;

	/***********************************************/
	/* wypelnienie informacji o rolach uzytkownika */
	/***********************************************/
	(*loginResponse)->userInfo->userRoleIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userRoleIdList->__size=bi->roleIdList->size;
	(*loginResponse)->userInfo->userRoleIdList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userRoleIdList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userRoleIdList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userRoleIdList->__ptr[i]), "%s", bi->roleIdList->gbufs[i]->buf);
		if (i==bi->defaultRoleIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultRoleId), "%s", bi->roleIdList->gbufs[i]->buf);
		}
	}

	(*loginResponse)->userInfo->userRoleNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userRoleNameList->__size=bi->roleNameList->size;
	(*loginResponse)->userInfo->userRoleNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userRoleNameList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userRoleNameList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userRoleNameList->__ptr[i]), "%s", bi->roleNameList->gbufs[i]->buf);
		if (i==bi->defaultRoleIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultRoleName), "%s", bi->roleNameList->gbufs[i]->buf);
		}
	}

	/************************************************/
	/* wypelnienie informacji o grupach uzytkownika */
	/************************************************/
	(*loginResponse)->userInfo->userGroupIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userGroupIdList->__size=bi->groupIdList->size;
	(*loginResponse)->userInfo->userGroupIdList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userGroupIdList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userGroupIdList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userGroupIdList->__ptr[i]), "%s", bi->groupIdList->gbufs[i]->buf);
		if (i==bi->defaultGroupIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultGroupId), "%s", bi->groupIdList->gbufs[i]->buf);
		}
	}

	(*loginResponse)->userInfo->userGroupNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userGroupNameList->__size=bi->groupNameList->size;
	(*loginResponse)->userInfo->userGroupNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userGroupNameList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userGroupNameList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userGroupNameList->__ptr[i]), "%s", bi->groupNameList->gbufs[i]->buf);
		if (i==bi->defaultGroupIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultGroupName), "%s", bi->groupNameList->gbufs[i]->buf);
		}
	}

	(*loginResponse)->userInfo->userClassIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userClassIdList->__size=bi->classIdList->size;
	(*loginResponse)->userInfo->userClassIdList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userClassIdList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userClassIdList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userClassIdList->__ptr[i]), "%s", bi->classIdList->gbufs[i]->buf);
		if (i==bi->defaultClassIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultClassId), "%s", bi->classIdList->gbufs[i]->buf);
		}
	}

	(*loginResponse)->userInfo->userClassNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userClassNameList->__size=bi->classNameList->size;;
	(*loginResponse)->userInfo->userClassNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*loginResponse)->userInfo->userClassNameList->__size);
	for (i=0; i<(*loginResponse)->userInfo->userClassNameList->__size; i++)
	{
		asprintf(&((*loginResponse)->userInfo->userClassNameList->__ptr[i]), "%s", bi->classNameList->gbufs[i]->buf);
		if (i==bi->defaultClassIndex)
		{
			asprintf(&((*loginResponse)->userInfo->userDefaultClassName), "%s", bi->classNameList->gbufs[i]->buf);
		}
	}

	(*loginResponse)->userInfo->userSecurityNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*loginResponse)->userInfo->userSecurityNameList->__ptr=NULL;
	(*loginResponse)->userInfo->userSecurityNameList->__size=0;

	/************************************/
	/* pobranie formularza wyszukiwania */
	/************************************/
 	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM_SEARCH,bi,&form,&form_count));

	(*loginResponse)->formSearch=(struct bmd230__mtdsInfo *)malloc(sizeof(struct bmd230__mtdsInfo));
	(*loginResponse)->formSearch->__size=form_count;
	(*loginResponse)->formSearch->__ptr=(struct bmd230__mtdSingleInfo *)malloc(sizeof(struct bmd230__mtdSingleInfo)*(*loginResponse)->formSearch->__size);

	for(i=0;i<form_count;i++)
	{
		bmd_attr_get_oid(i,form,&oid);
		bmd_attr_get_description(i,form,&desc);

		asprintf(&((*loginResponse)->formSearch->__ptr[i].mtdOid), "%s", oid);
		if ((*loginResponse)->formSearch->__ptr[i].mtdOid==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		asprintf(&((*loginResponse)->formSearch->__ptr[i].mtdDesc), "%s", desc);
		if ((*loginResponse)->formSearch->__ptr[i].mtdDesc==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		free0(oid);
		free0(desc);
	}
	form=NULL;
	form_count=0;

	/*******************************/
	/* pobranie formularza wysylki */
	/*******************************/
	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM_SEND,bi,&form,&form_count));

	(*loginResponse)->formSend=(struct bmd230__mtdsInfo *)malloc(sizeof(struct bmd230__mtdsInfo));
	(*loginResponse)->formSend->__size=form_count;
	(*loginResponse)->formSend->__ptr=(struct bmd230__mtdSingleInfo *)malloc(sizeof(struct bmd230__mtdSingleInfo)*(*loginResponse)->formSend->__size);

	for(i=0;i<form_count;i++)
	{
		bmd_attr_get_oid(i,form,&oid);
		bmd_attr_get_description(i,form,&desc);

		asprintf(&((*loginResponse)->formSend->__ptr[i].mtdOid), "%s", oid);
		if ((*loginResponse)->formSend->__ptr[i].mtdOid==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		asprintf(&((*loginResponse)->formSend->__ptr[i].mtdDesc), "%s", desc);
		if ((*loginResponse)->formSend->__ptr[i].mtdDesc==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		free0(oid);
		free0(desc);

	}
	form=NULL;
	form_count=0;

	/**************************************/
	/* pobranie formularza historycznosci */
	/**************************************/
	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM_HISTORY,bi,&form,&form_count));

	(*loginResponse)->formHistory=(struct bmd230__mtdsInfo *)malloc(sizeof(struct bmd230__mtdsInfo));
	(*loginResponse)->formHistory->__size=form_count;
	(*loginResponse)->formHistory->__ptr=(struct bmd230__mtdSingleInfo *)malloc(sizeof(struct bmd230__mtdSingleInfo)*(*loginResponse)->formHistory->__size);

	for(i=0;i<form_count;i++)
	{
		bmd_attr_get_oid(i,form,&oid);
		bmd_attr_get_description(i,form,&desc);

		asprintf(&((*loginResponse)->formHistory->__ptr[i].mtdOid), "%s", oid);
		if ((*loginResponse)->formHistory->__ptr[i].mtdOid==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		asprintf(&((*loginResponse)->formHistory->__ptr[i].mtdDesc), "%s", desc);
		if ((*loginResponse)->formHistory->__ptr[i].mtdDesc==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		free0(oid);
		free0(desc);
	}
	form=NULL;
	form_count=0;

	/********************************/
	/* pobranie formularza update'u */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM_UPDATE,bi,&form,&form_count));

	(*loginResponse)->formUpdate=(struct bmd230__mtdsInfo *)malloc(sizeof(struct bmd230__mtdsInfo));
	(*loginResponse)->formUpdate->__size=form_count;
	(*loginResponse)->formUpdate->__ptr=(struct bmd230__mtdSingleInfo *)malloc(sizeof(struct bmd230__mtdSingleInfo)*(*loginResponse)->formUpdate->__size);

	for(i=0;i<form_count;i++)
	{
		bmd_attr_get_oid(i,form,&oid);
		bmd_attr_get_description(i,form,&desc);

		asprintf(&((*loginResponse)->formUpdate->__ptr[i].mtdOid), "%s", oid);
		if ((*loginResponse)->formUpdate->__ptr[i].mtdOid==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		asprintf(&((*loginResponse)->formUpdate->__ptr[i].mtdDesc), "%s", desc);
		if ((*loginResponse)->formUpdate->__ptr[i].mtdDesc==NULL)	{	SOAP_FOK_LOGOUT(NO_MEMORY);	}

		free0(oid);
		free0(desc);

	}
	form=NULL;
	form_count=0;

	PRINT_SOAP("Status:\t\trequest served correctly\n");

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	return SOAP_OK;
}

/************************************************************************/
/*	obsluzenie zadania usuniecia pliku okreslonego przez jego id	*/
/************************************************************************/
int bmd230__bmdDeleteFileByID(	struct soap *soap,
				struct xsd__base64Binary *binaryCert,
				char *userClassId,
				char *roleName,
				char *groupName,
				long id,
				/*char* ownerRange,*/
				int *result)
{
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	(*result=-1);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (id==0)
	{
		PRINT_ERROR("SOAPSERVERERR Invalid second parameter value\n");
		return soap_receiver_fault(soap,"Internal error","Invalid second parameter value");
	}

	PRINT_SOAP("Request:\tdelete file with id=%li\n",id);

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}


	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(binaryCert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
			}
		}
		else
		{
			if(binaryCert != NULL)
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
			}
		}
	}

	/************************/
	/*	logowanie do bmd	*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/******************************/
	/*	stworzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE,&dtg));

	SOAP_FOK_LOGOUT(bmd_datagram_set_id((const long *)&id,&dtg));

	/*wyszukiwania dokumentow grupy lub tylko dokumentow uzytkownika*/
	/*if(ownerRange == NULL)
		{ dtg->datagramStatus=BMD_ONLY_GROUP_DOC; }
	else
	{
		if(strcmp(ownerRange, "OWNER") == 0)
			{ dtg->datagramStatus=BMD_ONLY_OWNER_DOC;}
		else
			{ dtg->datagramStatus=BMD_ONLY_GROUP_DOC; }
	}*/
		
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************/
	/*	wyslanie zadania	*/
	/************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);

	(*result)=BMD_OK;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/******************************************************************************/
/*	obsluzenie zadania oznaczenia czasem danych przyslanych przez klienta	*/
/******************************************************************************/
int bmd230__bmdGetTimeStamp(	struct soap *soap,
				struct bmd230__fileInfo *input,
				struct bmd230__bmdTimestamp **output)
{
GenBuf_t *response	= NULL;
GenBuf_t input2;
long status			= 0;
char *host			= NULL;
long port		= 0;
char *genTime		= NULL;
ConservationConfig_t ConservationConfig;

	PRINT_SOAP("Request:\tget timestamp\n");
	memset(&ConservationConfig, 0, sizeof(ConservationConfig_t)); 

	status=bmdconf_get_value(_GLOBAL_konfiguracja,"timestamp","host",&host);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}
	status=bmdconf_get_value_int(_GLOBAL_konfiguracja,"timestamp","port",&port);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	if ((host!=NULL) && (port>0))
	{
		do
		{
			bmd_crypt_ctx_t *hash_ctx=NULL;
			GenBuf_t *hash=NULL;
			bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1);
			input2.buf=(char *)input->file->__ptr;
			input2.size=input->file->__size;


			bmd_hash_data(&input2,&hash_ctx,&hash,NULL);
			bmd_timestamp_data(hash,BMD_HASH_ALGO_SHA1,host,port,0,NULL,0,NULL,NULL,2, 0, &response);

			status=prfParseTimestampGenTime(response, &genTime, &ConservationConfig);
			if(status<BMD_OK)
			{
				PRINT_ERROR("LIBBMDSQLERR Error in parsing Timestamp gen time. Error=%li\n", status);
				return status;
			}

			(*output)=(struct bmd230__bmdTimestamp *)soap_malloc(soap,sizeof(struct bmd230__bmdTimestamp));
			(*output)->timestamp=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
			(*output)->timestamp->__ptr=(unsigned char *)soap_malloc(soap,response->size+2);
			memset((*output)->timestamp->__ptr, 0, response->size+1);
			(*output)->timestamp->__size=response->size;
			memmove((*output)->timestamp->__ptr, response->buf, response->size);

			(*output)->genTime=(char * )soap_malloc(soap,sizeof(char *)*22);
			memset((*output)->genTime, 0, 21);
			memmove((*output)->genTime, genTime, 19);

		}while(0);
	}
	else
	{
		(*output)=(struct bmd230__bmdTimestamp *)soap_malloc(soap,sizeof(struct bmd230__bmdTimestamp));
		(*output)->timestamp=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
		(*output)->timestamp->__ptr=(unsigned char *)soap_malloc(soap,0);
		memset((*output)->timestamp->__ptr, 0, 0);
		(*output)->timestamp->__size=0;

		(*output)->genTime=(char * )soap_malloc(soap,sizeof(char *)*strlen("Service unavailable"));
		memset((*output)->genTime, 0, strlen("Service unavailable"));
		memmove((*output)->genTime, "Service unavailable", strlen("Service unavailable"));

		PRINT_ERROR("SOAPSERVERERR Service not available\n");
		return soap_receiver_fault(soap,"Error","Service unavailable");
	}

	/*if(err_message!=NULL)
	{
		(*output)=(struct bmd230__bmdTimestamp *)soap_malloc(soap,sizeof(struct bmd230__bmdTimestamp));
		(*output)->timestamp=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
		(*output)->timestamp->__ptr=(char *)soap_malloc(soap,0);
		memset((*output)->timestamp->__ptr, 0, 0);
		(*output)->timestamp->__size=0;

		(*output)->genTime=(char * )soap_malloc(soap,sizeof(char *)*strlen(err_message));
		memset((*output)->genTime, 0, strlen(err_message));
		memmove((*output)->genTime, err_message, strlen(err_message));
		PRINT_ERROR("SOAPSERVERERR Service not available\n");
		return soap_receiver_fault(soap,"Error",err_message);
	}*/

	free(host); host=NULL;
	free(genTime); genTime=NULL;
	return SOAP_OK;
}



/***************************************************/
/*	utworzenie katalogu i plików tymczasowych  */
/***************************************************/

void *dime_write_open(struct soap *soap, const char *id, const char *type, const char *options)
{
  /* ----------------------- */

      FILE* handle   = NULL;
      char* dirname  = NULL;
      char* path     = NULL;

  /* ----------------------- */

    asprintf(&dirname, "tmp_%i", (int)getpid());

    mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    asprintf(&path, "%s/%s", dirname, options+4);

    handle = fopen(path, "wb");

    if (!handle)
     {
        soap->error = SOAP_EOF;
        soap->errnum = errno; // get reason
     }

    free(dirname);

  return (void*)handle;

}


/*******************************************************/
/*	zamknięcie poszczególnych plików tymczasowych  */
/*******************************************************/


void dime_write_close(struct soap *soap, void *handle)
{
   fclose((FILE*)handle);
}


/*******************************************************/
/*	zapis do plików tymczasowych                   */
/*******************************************************/


int dime_write(struct soap *soap, void *handle, const char *buf, size_t len)
{

  /* ---------------- */

     size_t nwritten;

  /* ---------------- */

    while (len)
     {
       nwritten = fwrite(buf, 1, len, (FILE*)handle);

       if (!nwritten)
        {
          soap->errnum = errno; // get reason

          return SOAP_EOF;
        }

       len -= nwritten;
       buf += nwritten;
     }

   return SOAP_OK;
}

/*******************************************************/
/*	pobieranie uchwytu do odczytywanego pliku      */
/*******************************************************/

void *dime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *options)
{
  return handle;
}


/*******************************************************/
/*	odczyt z pliku                                 */
/*******************************************************/


size_t dime_read(struct soap *soap, void *handle, char *buf, size_t len)
{
  return fread(buf, 1, len, (FILE*)handle);
}


/*******************************************************/
/*	zamknięcie odczytywanego pliku                 */
/*******************************************************/


void dime_read_close(struct soap *soap, void *handle)
{
  fclose((FILE*)handle);
}

