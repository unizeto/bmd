#define _WINSOCK2API_
#include <bmd/common/bmd-os_specific.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmderr/libbmderr.h>
#ifdef WIN32
#pragma warning(disable:4100)
#endif

long __bmd_low_level_create_net_handler(const char * proxy_host, const unsigned long proxy_port,
										const char *proxy_username,const char *proxy_pass,
										const unsigned long proxy_type,
										const char *proxy_auth_type,
										bmdnet_handler_ptr *session_handle)
{
    long status;

	status=bmdnet_create(session_handle);
	if( status != 0 )
		return BMD_ERR_OP_FAILED;

    if( proxy_host != NULL )
    {
		long auth_type=BMDNET_PROXY_AUTH_NONE;
		if( proxy_auth_type != NULL )
		{
			if( strcmp(proxy_auth_type,"ntlm") == 0 )
				auth_type=BMDNET_PROXY_AUTH_NTLM;
			else
			if( strcmp(proxy_auth_type,"basic") == 0 )
				auth_type=BMDNET_PROXY_AUTH_BASIC;
			else
			{
				PRINT_ERROR("unsupported auth type for proxy\n");
				BMD_FOK(BMD_ERR_UNIMPLEMENTED);
			}
		}
		status=bmdnet_set_connection_params(proxy_host,(long)proxy_port,(long)proxy_type,auth_type,
				                            proxy_username,proxy_pass,*session_handle);
		if( status != BMD_OK )
		{
			PRINT_ERROR("Failed to set connection parameters\n");
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
    return BMD_OK;
}

long __bmd_low_level_connect_send(	bmdnet_handler_ptr session_handle,
						char *adr,
						long port,
						GenBuf_t *request)
{
long status		= 0;

	status=bmdnet_connect (session_handle,adr,port);
	switch (status)
	{
		case 0:
			break;
		#ifdef WIN32
		case BMD_ERR_NET_RESOLV:
			return BMD_ERR_NET_RESOLV;
		case -29000000:
			return -29000000;
		#endif
		default:
			return BMD_ERR_NET_CONNECT;
	}

	status=bmdnet_send (session_handle, request->buf,request->size);
	if(status<request->size)
	{
		return BMD_ERR_NET_WRITE;
	}

	return BMD_OK;
}

long __bmd_low_level_recv(bmdnet_handler_ptr session_handle,GenBuf_t **net_data)
{
	if(session_handle == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(net_data == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*net_data != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	/*(*net_data)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if( (*net_data)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}
	memset(net_data, 0, sizeof(GenBuf_t));*/

	/*(*net_data)->buf=NULL;*/

	BMD_FOK(bmd_get_from_socket(session_handle, net_data));

	/*status=asn1_get_from_socket(session_handle,&((*net_data)->buf));
	if(status<0)
	return status;*/
	/*(*net_data)->size=status;*/

	return BMD_OK;
}

/* wykonuje niskopoziomowe logowanie do serwera BMD - dzialanie funkcji konczy sie na odebraniu danych z gniazda */
long bmd_low_level_login(	const char *adr,
				const long port,
				bmd_crypt_ctx_t *sig_ctx,
				bmd_crypt_ctx_t *enc_ctx,
				GenBuf_t **k,
				GenBuf_t ** net_data,
				const char * proxy_host,
				const unsigned long proxy_port,
				const char *proxy_username,
				const char *proxy_pass,
				const unsigned long proxy_type,
				const char *proxy_auth_type,
				GenBuf_t *cert_login_as)
{
GenBuf_t *request_buffer			= NULL;
bmdnet_handler_ptr session_handle		= NULL;
long status						= 0;

	status=bmd2_create_login_request(sig_ctx,enc_ctx,cert_login_as,k,&request_buffer);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDERR %s. Error=%li\n",GetErrorMsg(status),status);
		switch(status)
		{
			case BMD_ERR_PKI_SIGN: return BMD_ERR_PKI_SIGN;
			case BMD_ERR_PKI_ENCRYPT: return BMD_ERR_PKI_ENCRYPT;
			default: return BMD_ERR_USER_OP_FAILED;
		};
	}
	status=__bmd_low_level_create_net_handler(	proxy_host,
									proxy_port,
									proxy_username,
									proxy_pass,
									proxy_type,
									proxy_auth_type,
									&session_handle);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDERR %s. Error=%li\n",GetErrorMsg(status),status);
		return status;
	}

	status=__bmd_low_level_connect_send(session_handle,(char *)adr,port,request_buffer);
	switch (status)
	{
		case BMD_OK:
			break;
		#ifdef WIN32
		case BMD_ERR_NET_RESOLV:
			return BMD_ERR_NET_RESOLV;
		case -29000000:
			return -29000000;
		#endif
		default:
			return BMD_ERR_NET_CONNECT;
	}
	free_gen_buf(&request_buffer);
	status=__bmd_low_level_recv(session_handle,net_data);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDERR %s. Error=%li\n",GetErrorMsg(status),status);
		return status;
	}

	bmdnet_close(&session_handle);
	return BMD_OK;
}


long bmd_login(bmd_info_t **bmd_info)													// funkcja dla zachowania wstecznej kompatybilnosci
{
	BMD_FOK(bmd_login2(bmd_info, NULL));

	return BMD_OK;
}


long bmd_login2(	bmd_info_t **bmd_info,
			long *servver)										// funkcja dodatkowo zwraca w parametrze numer wersji serwera do którego próbuje się logować
{
GenBuf_t *net_data	= NULL;
GenBuf_t *bmd_cert	= NULL;
GenBuf_t *der_form	= NULL;

	BMD_FOK(bmd_low_level_login(	(*bmd_info)->bmd_address,			/* adres serwera */
						(*bmd_info)->bmd_port,				/* port serwera */
						(*bmd_info)->crypto_info.log_ctx,		/* kontekst ktorym podpisany zostanie request logowania */
						(*bmd_info)->crypto_info.bmd_ctx,		/* kontekst ktorym zaszyfrowany zostanie podpisany request logowania sie */
						&((*bmd_info)->k0),				/* wartosc k0 wygenerowana przy tworzeniu requesta */
						&net_data,					/* dane odebrane od serwera */
						(*bmd_info)->proxy_info.proxy_url,		/* adres proxy */
						(*bmd_info)->proxy_info.proxy_port,		/* port proxy */
						(*bmd_info)->proxy_info.proxy_username,		/* uzytkownik proxy */
						(*bmd_info)->proxy_info.proxy_password,		/* haslo uzytkownika proxy */
						(*bmd_info)->proxy_info.proxy_type,             /* typ proxy */
						(*bmd_info)->proxy_info.proxy_auth_type,
                				(*bmd_info)->cert_login_as));

	BMD_FOK(bmd2_decode_login_response(	&net_data,						/* dane odebrane od serwera */
				       		(*bmd_info)->crypto_info.log_ctx,		/* kontekst ktory zdeszyfruje odpowiedz */
				       		(*bmd_info)->crypto_info.bmd_ctx->file->cert,	/* certyfikat ktory klient posiada - do porownania */
				       		&bmd_cert,						/* certyfikat ktory sprzedalo BMD - moze byc ze nie potrzebny */
				       		(*bmd_info)->k0,					/* wartosc k0 */
				       		&((*bmd_info)->server_ki_mod_n),		/* wartosc ki_mod_n wyliczona przez serwer */
				       		&((*bmd_info)->n),				/* wartosc n */
				       		&((*bmd_info)->i),				/* wartosc i */
				       		&der_form,						/* formularz w DER */
				       		&((*bmd_info)->crypto_info.sym_ctx),	/* kontekst symetryczny */
				       		(uint32_t *)&((*bmd_info)->request_id),	/* identyfikator requesta */
							&((*bmd_info)->roleIdList),			/* lista identyfikatorow rol */
							&((*bmd_info)->roleNameList),			/* lista nazw rol */
							&((*bmd_info)->defaultRoleIndex),		/* indeks domyslnej roli*/
							&((*bmd_info)->groupIdList),		/* lista identyfikatorow grup */
							&((*bmd_info)->groupNameList),		/* lista nazw grup */
							&((*bmd_info)->defaultGroupIndex),	/* indeks domyslnej grupy */
							&((*bmd_info)->classIdList),		/* lista identyfikatorow grup */
							&((*bmd_info)->classNameList),		/* lista nazw grup */
							&((*bmd_info)->defaultClassIndex),	/* indeks domyslnej grupy */
				       		&((*bmd_info)->hcsb),
						&((*bmd_info)->actions),
						&((*bmd_info)->no_of_actions),
						&((*bmd_info)->server_label),
					   	servver));

	/******************************************************************************/
	/*	jesli jestesmy tutaj to na pewno zgadzaja sie wartosc ki_mod_n		*/
	/*	wiec wartosc od serwera nalezy ustawic na biezaca wartosc ki_mod_n	*/
	/******************************************************************************/
	BMD_FOK(set_gen_buf2((*bmd_info)->server_ki_mod_n->buf, (*bmd_info)->server_ki_mod_n->size, &((*bmd_info)->cur_ki_mod_n)));

	/************************************************************************/
	/*	jesli OK wszystko to ustaw bmd_ctx na certyfikat z odpowiedzi	*/
	/************************************************************************/
	BMD_FOK(bmd_ctx_destroy(&((*bmd_info)->crypto_info.bmd_ctx)));
	BMD_FOK(bmd_set_ctx_cert(&((*bmd_info)->crypto_info.bmd_ctx),bmd_cert));

	free_gen_buf(&bmd_cert);
	free_gen_buf(&net_data);
	BMD_FOK(bmd_kontrolki_deserialize(der_form, &((*bmd_info)->form_info.user_form)));

	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), SEND_KONTROLKI, &((*bmd_info)->form_info.user_send_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), GRID_KONTROLKI, &((*bmd_info)->form_info.user_view_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), SEARCH_KONTROLKI, &((*bmd_info)->form_info.user_search_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), UNVISIBLE_KONTROLKI, &((*bmd_info)->form_info.user_unvisible_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), UPDATE_KONTROLKI, &((*bmd_info)->form_info.user_update_form)));
	BMD_FOK(create_specific_kontrolki(	(BMD_attr_t**)((*bmd_info)->form_info.user_form), HISTORY_KONTROLKI, &((*bmd_info)->form_info.user_history_form)));

	if (der_form!=NULL)
	{
		free_gen_buf(&der_form);
	}

	return BMD_OK;
}

long bmd_logout(	bmd_info_t **bmd_info,
			long deserialisation_max_memory)
{
bmdDatagram_t *logout_request		= NULL;
bmdDatagramSet_t *logout_request_set	= NULL;
bmdDatagramSet_t *logout_response_set	= NULL;

	PRINT_INFO("LIBBMDINF bmd_logout.\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (bmd_info==NULL)			{	return BMD_OK;	}
	if ((*bmd_info)==NULL)			{	return BMD_OK;	}
	if (deserialisation_max_memory < 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	logout_request=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	BMD_FOK(PR_bmdDatagram_init(logout_request));
	logout_request->datagramType=BMD_DATAGRAM_TYPE_CLOSE_SESSION;

	BMD_FOK(bmd_datagram_add_to_set(logout_request,&logout_request_set));

	/*Tutaj wystarczy 100 na paczke bo i tak jest jeden datagram tylko.*/


	BMD_FOK(bmd_send_request(	*bmd_info,
					logout_request_set,
					100,
					&logout_response_set,
					1,
					deserialisation_max_memory));

	bmd_info_destroy_forms(*bmd_info);
	bmd_ctx_destroy(&((*bmd_info)->crypto_info.log_ctx));
	bmd_ctx_destroy(&((*bmd_info)->crypto_info.bmd_ctx));
	bmd_ctx_destroy(&((*bmd_info)->crypto_info.sig_ctx));
	bmd_ctx_destroy(&((*bmd_info)->crypto_info.sym_ctx));

	free((*bmd_info)->bmd_address);free((*bmd_info)->proxy_info.proxy_username);free((*bmd_info)->proxy_info.proxy_password);

	free0( (*bmd_info)->proxy_info.proxy_url );// - RO
	// role_selection_callback chooseRole; nie ma co zwalniać, bo to tylko wskaznik do funkcji; - RO
	//bmd_info( (*bmd_info)->chooseRole = NULL;	// - mozna by conajwyzej zamazać

	/* niszczenie listy identyfikatorow rol */
	free_GenBufList( &((*bmd_info)->roleIdList) ); //commonutils
	/* niszczenie listy nazw rol */
	free_GenBufList( &((*bmd_info)->roleNameList) ); //commonutils
	(*bmd_info)->roleChosenIndex=-1;
	(*bmd_info)->defaultRoleIndex=-1;

	/* niszczenie listy identyfikatorow grup */
	free_GenBufList( &((*bmd_info)->groupIdList) ); //commonutils
	/* niszczenie listy nazw grup*/
	free_GenBufList( &((*bmd_info)->groupNameList) ); //commonutils
	(*bmd_info)->groupChosenIndex=-1;
	(*bmd_info)->defaultGroupIndex=-1;

	bmd_info_destroy_actions(*bmd_info);

	free_gen_buf(&((*bmd_info)->k0));
	free_gen_buf(&((*bmd_info)->i));
	free_gen_buf(&((*bmd_info)->n));

	free_gen_buf(&((*bmd_info)->cur_ki_mod_n));
	free_gen_buf(&((*bmd_info)->server_ki_mod_n));

	free(*bmd_info);(*bmd_info)=NULL;
	PR2_bmdDatagramSet_free(&logout_request_set);
	PR2_bmdDatagramSet_free(&logout_response_set);

	return BMD_OK;
}
