#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
typedef long (__cdecl *gewc_f)(dictionary_data_t *,long,char *,long,char **);
typedef dictionary_data_t* (__cdecl *gggd_f)(void);
#endif

long bmd2_create_login_response(	bmd_crypt_ctx_t *sig_ctx,
					bmd_crypt_ctx_t *enc_ctx,
					GenBuf_t *k,
					GenBuf_t *i,
					GenBuf_t *n,
			       		GenBuf_t *der_form,
					GenBuf_t *bmdKeyIV,
					uint32_t request_id,
					GenBufList_t *role_list,
					GenBufList_t *group_list,
					GenBufList_t *security_list,
					GenBuf_t *user_chosen_role,
					GenBuf_t *user_chosen_group,
					GenBuf_t *serial,
					GenBuf_t *cert_dn,
					GenBuf_t *hcsb,
					long *actions,
					long no_of_actions,
					char *server_label,
			       		GenBuf_t **output)
{
GenBuf_t *ki_mod_n		= NULL;
bmdDatagram_t *dtg		= NULL;
GenBuf_t *tmp			= NULL;
GenBuf_t *tmp1			= NULL;
char *groupName			= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(sig_ctx==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(enc_ctx==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(k==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(k->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(k->size<=0)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(i==NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(i->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(i->size<=0)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(n==NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(n->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(n->size<=0)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(der_form==NULL)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(der_form->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(der_form->size<=0)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(bmdKeyIV==NULL)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if(bmdKeyIV->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if(bmdKeyIV->size<=0)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if(output==NULL)			{	BMD_FOK(BMD_ERR_PARAM10);	}
	if((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM10);	}

	/*******************************************/
	/* obliczenie parametrow kryptograficznych */
	/*******************************************/
	BMD_FOK(bmd_compute_ki_mod_n(k,i,n,&ki_mod_n));

	/************************/
	/*	stworz datagram	*/
	/************************/
	dtg=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	PR_bmdDatagram_init(dtg);
	dtg->datagramType=BMD_DATAGRAM_TYPE_GET_FORM_RESPONSE;

	BMD_FOK(bmd2_datagram_set_ki_mod_n(ki_mod_n,&dtg));
	free_gen_buf(&ki_mod_n);
	BMD_FOK(bmd2_datagram_set_n(n,&dtg));
	BMD_FOK(bmd2_datagram_set_i(i,&dtg));
	BMD_FOK(bmd2_datagram_set_form(der_form,&dtg));
	BMD_FOK(bmd2_datagram_set_symkey(bmdKeyIV,&dtg));
	BMD_FOK(bmd2_datagram_set_symkey_hash( hcsb, &dtg));

	/*******************************************/
	/* ustaw role, symkey i akcje w datagramie */
	/*******************************************/
	BMD_FOK(bmd2_datagram_set_server_label( server_label, &dtg));
	BMD_FOK(bmd2_datagram_set_role_list( role_list, &dtg));
	BMD_FOK(bmd2_datagram_set_group_list( group_list, &dtg));
 	BMD_FOK(bmd2_datagram_set_class_list( serial, cert_dn, &dtg));
	BMD_FOK(bmd2_datagram_set_security_list( security_list, &dtg));

#ifndef WIN32
	//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users||groups.id FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id) WHERE groups.location_id=%s;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UserGroupsDictionary, 2, user_chosen_group->buf, 3, &groupName), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
#else
	{
		gewc_f funkcja=NULL;
		long status;
		void *hLib=LoadLibraryA("libbmdglobals.dll");
		if( hLib == NULL )
		{
			BMD_FOK(BMD_ERR_DISK);
		}
		funkcja=(gewc_f)GetProcAddress(hLib,"getElementWithCondition");
		if( funkcja == NULL )
		{
			BMD_FOK(BMD_ERR_FORMAT);
		}
		status=funkcja(_GLOBAL_UserGroupsDictionary,2,user_chosen_group->buf,3,&groupName);
		FreeLibrary(hLib);
		if( status != BMD_OK )
		{
			BMD_FOK(status);
		};
	}
#endif
	BMD_FOK(bmd2_datagram_set_current_role_and_group(user_chosen_role->buf, user_chosen_group->buf, &dtg));
	BMD_FOK(bmd2_datagram_set_actions( actions, no_of_actions, &dtg));

	/******************************************/
	/*	serializuj datagram odpowiedzi	*/
	/******************************************/
	BMD_FOK(PR2_bmdDatagram_serialize(dtg,&tmp));

	/************************************/
	/*	podpisz i zasyzfruj dane	*/
	/************************************/
	BMD_FOK(bmd_sign_data(tmp,sig_ctx,&tmp1,NULL));
	BMD_FOK(bmd_encrypt_data(tmp1,enc_ctx,output,NULL));

	/********************************/
	/* zapisanie datagramu do pliku */
	/********************************/
	if (_GLOBAL_debug_level==-5)
	{
		FILE *fd				= NULL;
		fd = fopen("/tmp/dtg/login_resp.dtg","w+");
		if (fd!=NULL)
		{
			PR_bmdDatagram_print(dtg, WITH_TIME, fd);
			fclose(fd);
		}
		else
		{
			PRINT_ERROR("BMDSERVERERR Error in openning / creating file %s\n","/tmp/dtg/login_resp.dtg");
		}
	}
	set_prefix_from_int(request_id, (*output)->size, output);

	/******************/
	/*	porzadki	*/
	/******************/
// 	PR2_bmdDatagram_free(&dtg);
	free_gen_buf(&tmp);
	free_gen_buf(&tmp1);
	free(groupName); groupName=NULL;

	return BMD_OK;
}

long bmd2_decode_login_response(	GenBuf_t **net_input,
					bmd_crypt_ctx_t *dec_ctx,
					GenBuf_t *client_bmd_cert,
					GenBuf_t **bmd_cert,
					GenBuf_t *k0,
					GenBuf_t **k,
					GenBuf_t **n,
			       		GenBuf_t **i,
					GenBuf_t **der_form,
					bmd_crypt_ctx_t **sym_ctx,
					uint32_t *request_id,
					GenBufList_t **roleIdList,
					GenBufList_t **roleNameList,
					long *defaultRoleIndex,
					GenBufList_t **groupIdList,
					GenBufList_t **groupNameList,
					long *defaultGroupIndex,
					GenBufList_t **classIdList,
					GenBufList_t **classNameList,
					long *defaultClassIndex,
			       		GenBuf_t **hcsb,
					long **actions,
					long *no_of_actions,
					char **server_label,
					long *servver)
{
long status				= 0;
GenBuf_t *tmp			= NULL;
GenBuf_t *tmp1			= NULL;
GenBuf_t *tmp2			= NULL;
bmdDatagram_t *dtg		= NULL;
GenBuf_t *client_ki_mod_n	= NULL;

	strip_prefix(net_input,request_id,&tmp);

	if(*request_id==0) /* jest blad */
	{
		status=PR2_bmdDatagram_deserialize(tmp,&dtg);
		if (servver && dtg)
		{	*servver=dtg->protocolVersion;
		}
		BMD_FOK(status);

		if (dtg)
		{
			status=dtg->datagramStatus;
		}
		else
		{
			status = BMD_ERR_OP_FAILED;
		}
		BMD_FOK(status);

		free_gen_buf(&tmp);
		PR2_bmdDatagram_free(&dtg);

		return status; /* BMD_ERR_FORMAT - klient wyslal nieprawidlowe dane,
						  BMD_ERR_OP_FAILED - serwer przyslal nieprawidlowe dane

				*/
	}

	status=bmd_decrypt_data(&tmp, 0, dec_ctx,&tmp1, NULL, 0);
	PRINT_GK("DBG Status zdeszyfrowania odpowiedzi na logowanie %li\n", status);
	if(status!=BMD_OK)
	{
		free_gen_buf(&tmp);
		free_gen_buf(&tmp1);
		return status;
	}
	BMD_FOK(bmd_get_cert_and_content_from_signature(tmp1,bmd_cert,&tmp2));

	if(client_bmd_cert->size!=(*bmd_cert)->size)						{	BMD_FOK(BMD_ERR_CREDENTIALS);	}
	if(memcmp(client_bmd_cert->buf,(*bmd_cert)->buf,client_bmd_cert->size)!=0)	{	BMD_FOK(BMD_ERR_CREDENTIALS);	}

 	status=PR2_bmdDatagram_deserialize(tmp2,&dtg);
	if (servver)
	{
		*servver=dtg->protocolVersion;
	}
	BMD_FOK(status);
	free_gen_buf(&tmp);
	free_gen_buf(&tmp1);
	free_gen_buf(&tmp2);


	BMD_FOK(bmd2_datagram_get_ki_mod_n(dtg,k));
	BMD_FOK(bmd2_datagram_get_i(dtg,i));
	BMD_FOK(bmd2_datagram_get_n(dtg,n));
	BMD_FOK(bmd_compute_ki_mod_n(k0,*i,*n,&client_ki_mod_n));

	if(bmd_compare_bn(client_ki_mod_n,*k)!=0) /* SA ROZNE KI_MOD_N */		{	BMD_FOK(BMD_ERR_PROTOCOL);	}

    	BMD_FOK(bmd2_datagram_get_form(dtg,der_form));

    	/*BMD_FOK(bmd2_datagram_get_oids(dtg,oids));*/

    	BMD_FOK(bmd2_datagram_get_symkey(dtg,&tmp));

	/********************************************/
    	/* pobierz symkey i akcje z datagramu */
	/********************************************/
	BMD_FOK(bmd2_datagram_get_symkey_hash(dtg,hcsb));
	bmd2_datagram_get_actions(dtg, actions, no_of_actions);
	BMD_FOK(bmd2_datagram_get_server_label(dtg, server_label));

	/* pobieranie dostepnych rol (identyfikatorow i odpowiadajacych im nazw)  */
	BMD_FOK(bmd2_datagram_get_role_id_list(dtg,roleIdList));
	BMD_FOK(bmd2_datagram_get_role_name_list(dtg,roleNameList));

	/* pobieranie dostepnych grup (identyfikatorow i odpowiadajacych im nazw)  */
	BMD_FOK(bmd2_datagram_get_group_id_list(dtg, groupIdList));
	BMD_FOK(bmd2_datagram_get_group_name_list(dtg, groupNameList));

	/* pobieranie dostepnych class (identyfikatorow i odpowiadajacych im nazw)  */
	BMD_FOK(bmd2_datagram_get_class_id_list(dtg, classIdList));
	BMD_FOK(bmd2_datagram_get_class_name_list(dtg, classNameList));

	/* pobieranie indeksu domyslnej roli */
	bmd2_datagram_get_current_role_index(dtg, *roleIdList, defaultRoleIndex);

	/* pobieranie indeksu domyslnej grupy */
	bmd2_datagram_get_current_group_index(dtg, *groupIdList, defaultGroupIndex);

	/* pobieranie indeksu domyslnej klasy */
	bmd2_datagram_get_current_class_index(dtg, *classIdList, defaultClassIndex);

	BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,dec_ctx,sym_ctx));
	free_gen_buf(&tmp);
	free_gen_buf(&client_ki_mod_n);
	PR2_bmdDatagram_free(&dtg);

	return BMD_OK;
}
