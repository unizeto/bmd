#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>

long bmd_info_create(bmd_info_t **bi)
{
	(*bi)=(bmd_info_t *)malloc(sizeof(bmd_info_t));

	memset((*bi),0,sizeof(bmd_info_t));

	(*bi)->roleChosenIndex=-1;
	(*bi)->defaultRoleIndex=-1;
	(*bi)->groupChosenIndex=-1;
	(*bi)->defaultGroupIndex=-1;

	return 0;
};


/**
*Funkcja bmd_info_destroy_forms() niszczy formularze w strukturze bmd_info
*
*@param bmdInfo to wskaznik na strukture bmd_info
*
*@retval BMD_OK jesli sukces
*@retval <0 jesli blad
*/
long bmd_info_destroy_forms(bmd_info_t *bmdInfo)
{
	if(bmdInfo == NULL)		{	return BMD_ERR_PARAM1;	}

	if( bmdInfo->form_info.user_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_form) );
	}
	if( bmdInfo->form_info.user_send_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_send_form) );
	}
	if( bmdInfo->form_info.user_search_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_search_form) );
	}
	if( bmdInfo->form_info.user_view_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_view_form) );
	}
	if( bmdInfo->form_info.user_unvisible_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_unvisible_form) );
	}
	if( bmdInfo->form_info.user_update_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_update_form) );
	}
	if( bmdInfo->form_info.user_history_form )
	{

		free_BMD_attr_list( &(bmdInfo->form_info.user_history_form) );
	}

	return BMD_OK;
}

/**
*Funkcja bmd_info_destroy_actions() niszczy tablice dostepnych akcji w strukturze bmd_info
*
*@param bmdInfo to wskaznik na wstrukture bmd_info
*
*@retval BMD_OK jesli sukces
*@retval <0 jesli blad
*/
long bmd_info_destroy_actions(bmd_info_t *bmdInfo)
{
	if(bmdInfo == NULL)
		{ return BMD_ERR_PARAM1; }

	free(bmdInfo->actions);
	bmdInfo->actions=NULL;
	bmdInfo->no_of_actions=0;

	return BMD_OK;
}

/** Funkcja likwidująca strukturę bmd_info.
Funkcja likwiduje zaalokowane składniki struktury - Added by RO
@note pamięć zwalniana jest też w \c bmd_logout(), więc jest to funkcja awaryjna
*/
long bmd_info_destroy(bmd_info_t **bi)
{

	if ( (*bi) )
	{
		free0( (*bi)->bmd_address);		// tworzone w bmd_info_login_set()
		free0( (*bi)->proxy_info.proxy_url );
		free0( (*bi)->proxy_info.proxy_username );
		free0( (*bi)->proxy_info.proxy_password );

		bmd_info_destroy_forms(*bi);

		bmd_ctx_destroy( &((*bi)->crypto_info.log_ctx) );
		bmd_ctx_destroy( &((*bi)->crypto_info.bmd_ctx) );
		bmd_ctx_destroy( &((*bi)->crypto_info.sig_ctx) );
		bmd_ctx_destroy( &((*bi)->crypto_info.sym_ctx) );
		bmd_genbuf_free( &((*bi)->k0) );
		bmd_genbuf_free( &((*bi)->i) );
		bmd_genbuf_free( &((*bi)->n) );
		bmd_genbuf_free( &((*bi)->cur_ki_mod_n) );
		bmd_genbuf_free( &((*bi)->server_ki_mod_n) );
		bmd_genbuf_free( &((*bi)->hcsb) );
		// role_selection_callback chooseRole - to wskaznik na funkcje - nie trzeba zwalniac?? ;
		free_GenBufList( &((*bi)->roleIdList) ); //commonutils
		free_GenBufList( &((*bi)->roleNameList) ); //commonutils
		(*bi)->roleChosenIndex=-1;
		(*bi)->defaultRoleIndex=-1;

		free_GenBufList( &((*bi)->groupIdList) ); //commonutils
		free_GenBufList( &((*bi)->groupNameList) ); //commonutils
		(*bi)->groupChosenIndex=-1;
		(*bi)->defaultGroupIndex=-1;

		bmd_info_destroy_actions(*bi);

		bmd_genbuf_free( &((*bi)->cert_login_as) );

		free0(*bi);		// rezerwowane przez bmd_info_create()
	}

	return 0;
}

/**
*Funckja bmd_info_load_forms() laduje formularze do struktury bmdInfo na podstawie odpowiedzi serwera
*
*@param bmdInfo to wskaznik na strukture bmd_info
*@param dtg to wskaznik na datagram odpowiedzi serwera
*
*@retval 0 jesli sukces
*@retval <0 jesli blad
*/
long bmd_info_load_forms(bmd_info_t *bmdInfo, bmdDatagram_t *dtg)
{
	long retVal=0;
	GenBuf_t *derForms=NULL;

	if(bmdInfo == NULL)
		{ return -1; }
	if(dtg == NULL)
		{ return -2; }

	retVal=bmd2_datagram_get_form(dtg, &derForms);
	if(retVal != BMD_OK)
	{
		return -11;
	}

	retVal=bmd_kontrolki_deserialize(derForms, &(bmdInfo->form_info.user_form) );
	free_gen_buf(&derForms);
	if(retVal  != BMD_OK)
	{
		return -12;
	}

	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), SEND_KONTROLKI, &(bmdInfo->form_info.user_send_form) );
	if(retVal != BMD_OK)
		{ return -13; }
	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), GRID_KONTROLKI, &(bmdInfo->form_info.user_view_form) );
	if(retVal != BMD_OK)
		{ return -14; }
	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), SEARCH_KONTROLKI, &(bmdInfo->form_info.user_search_form) );
	if(retVal != BMD_OK)
		{ return -15; }
	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), UNVISIBLE_KONTROLKI, &(bmdInfo->form_info.user_unvisible_form) );
	if(retVal != BMD_OK)
		{ return -16; }
	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), UPDATE_KONTROLKI, &(bmdInfo->form_info.user_update_form) );
	if(retVal != BMD_OK)
		{ return -17; }
	retVal=create_specific_kontrolki( (BMD_attr_t**)(bmdInfo->form_info.user_form), HISTORY_KONTROLKI, &(bmdInfo->form_info.user_history_form) );
	if(retVal != BMD_OK)
		{ return -18; }

	return 0;
}

long bmd_info_login_set(	const char *bmd_address,
					const long bmd_port,
					const char *proxy_url,
					const long proxy_port,
					const char *proxy_username,
					const char *proxy_password,
					const long proxy_type,
					const char *proxy_auth_type,
					bmd_info_t **bi)
{
	asprintf(&((*bi)->bmd_address),"%s",bmd_address);
	(*bi)->bmd_port=bmd_port;

	if(proxy_url)
	{
		asprintf(&((*bi)->proxy_info.proxy_url),"%s",proxy_url);
	}
	if(proxy_username)
	{
		asprintf(&((*bi)->proxy_info.proxy_username),"%s",proxy_username);
	}
	if(proxy_password)
	{
		asprintf(&((*bi)->proxy_info.proxy_password),"%s",proxy_password);
	}
	(*bi)->proxy_info.proxy_port=proxy_port;
	(*bi)->proxy_info.proxy_type=proxy_type;
	if(proxy_auth_type)
	{
		asprintf(&((*bi)->proxy_info.proxy_auth_type),"%s",proxy_auth_type);
	}

	return BMD_OK;
}


long bmd_info_set_role_callback(	role_selection_callback function,
						bmd_info_t **bi)
{
	PRINT_INFO("LIBBMDINF Setting role callback\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(function==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*bi)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*bi)->chooseRole=function;

	return BMD_OK;
}


long bmd_info_set_selected_role(	bmd_info_t **bi)
{
	PRINT_INFO("LIBBMDINF Setting selected role\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*bi)==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	if((*bi)->chooseRole!=NULL)
	{
		(*bi)->roleChosenIndex=(*bi)->chooseRole((*bi)->roleNameList,0);
		if((*bi)->roleChosenIndex==-1)
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	else
	{
		if((*bi)->roleNameList)
		{
			if((*bi)->roleNameList->gbufs)
			{
				if((*bi)->roleNameList->gbufs[0])
				{
					(*bi)->roleChosenIndex=0;
				}
				else
				{
					BMD_FOK(BMD_ERR_OP_FAILED);
				}
			}
			else
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
		}
		else
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	return BMD_OK;
}


long bmd_info_set_cert_login_as(	GenBuf_t *cert_login_as,
						bmd_info_t **bi)
{
	PRINT_INFO("LIBBMDINF Setting certificate as login\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(cert_login_as==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*bi)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(set_gen_buf2(cert_login_as->buf,cert_login_as->size,&((*bi)->cert_login_as)));

	return BMD_OK;
}


long bmd_info_set_credentials_pfx(	char *pfx_filename,
						char *password,
						long pass_length,
						bmd_info_t **bi)
{
	PRINT_INFO("LIBBMDINF Setting credential pfx\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(pfx_filename==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*bi)==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}

	BMD_FOK(bmd_set_ctx_file(pfx_filename,password,pass_length,&((*bi)->crypto_info.log_ctx)));

	return BMD_OK;
}


long bmd_info_set_bmd_cert(	char *cert_filename,
					bmd_info_t **bi)
{
GenBuf_t *tmp		= NULL;

	PRINT_INFO("LIBBMDINF Setting BMD certificate\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(cert_filename==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bi==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*bi)==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_load_binary_content(cert_filename,&tmp));

	BMD_FOK(bmd_set_ctx_cert(&((*bi)->crypto_info.bmd_ctx),tmp));

	free_gen_buf(&tmp);

	return BMD_OK;
}


long bmd_info_get_form(	long form_type,
			bmd_info_t *bi,
			BMD_attr_t ***form,
			long *form_count)
{
long status		= BMD_OK;
long i			= 0;

	PRINT_INFO("LIBBMDINF Getting form infomation\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if( 	(form_type!=BMD_FORM_SEND) && (form_type!=BMD_FORM_SEARCH) &&
		(form_type!=BMD_FORM_GRID) && (form_type!=BMD_FORM_UNVISIBLE) &&
		(form_type!=BMD_FORM_UPDATE) && (form_type!=BMD_FORM) &&
		(form_type!=BMD_FORM_UPLOAD) && (form_type!=BMD_FORM_HISTORY)
	)
		{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(bi==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(form==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*form)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	switch(form_type)
	{
		case BMD_FORM:
			if(bi->form_info.user_form!=NULL)
			{
				 (*form)=bi->form_info.user_form;
			}
			else
			{
				status=BMD_NO_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_SEND:
			if(bi->form_info.user_send_form!=NULL)
			{
				(*form)=bi->form_info.user_send_form;
			}
			else
			{
				status=BMD_NO_SEND_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_SEARCH:
			if(bi->form_info.user_search_form!=NULL)
			{
				(*form)=bi->form_info.user_search_form;
			}
			else
			{
				status=BMD_NO_SEARCH_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_GRID:
			if(bi->form_info.user_view_form!=NULL)
			{
				(*form)=bi->form_info.user_view_form;
			}
			else
			{
				status=BMD_NO_GRID_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_UNVISIBLE:
			if(bi->form_info.user_unvisible_form!=NULL)
			{
				(*form)=bi->form_info.user_unvisible_form;
			}
			else
			{
				status=BMD_NO_UNVISIBLE_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_UPDATE:
			if(bi->form_info.user_update_form!=NULL)
			{
				(*form)=bi->form_info.user_update_form;
			}
			else
			{
				status=BMD_NO_UPDATE_FORM_DATA;
				(*form)=NULL;
			}
			break;
		case BMD_FORM_HISTORY:
			if(bi->form_info.user_history_form!=NULL)
			{
				(*form)=bi->form_info.user_history_form;
			}
			else
			{
				status=BMD_NO_HISTORY_FORM_DATA;
				(*form)=NULL;
			}
			break;
		default :
			BMD_FOK(BMD_WRONG_FORM_TYPE);
			break;
	};

	if((*form)!=NULL)
	{
		i=0;
		while((*form)[i])
		{
			i++;
		}

		(*form_count)=i;
	}

    	BMD_FOK(status);

	return BMD_OK;
}
