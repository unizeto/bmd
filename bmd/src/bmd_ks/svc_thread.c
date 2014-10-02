#include <bmd/bmd_ks/bmd_ks.h>

extern svc_p11_t *_GLOBAL_svc_p11;
#ifdef WIN32
extern HANDLE *hThreads;
#else
extern pthread_t *hThreads;
#endif
extern ks_thread_data_t **thread_data;
extern svc_conf_t *svc_conf;

#ifdef WIN32
#pragma warning(disable:4127)
#endif

/* cialo funkcji wykonywanej przez watek do obslugi pojedynczego klienta */
DWORD WINAPI bmd_ks_thread(LPVOID lpParameter)
{
	SOCKET ls,as;
	struct sockaddr_in ci;
	long nr,status,nwrite;
	int ci_size		= 0;
	ks_thread_data_t *td=NULL;
	bmd_ks_request_t *ks_req=NULL;
	bmd_ks_response_t *ks_resp=NULL;
	char *req_buf=NULL,*cert=NULL;
	long req_len=0,cert_length=0;
	#ifndef WIN32
	char *tmp_adr=NULL;
	#endif

	if( lpParameter == NULL )
		return SVC_ERROR_PARAM;

	td=(ks_thread_data_t *)lpParameter;
	ls=td->s;
	nr=td->thread_number;

	ci_size=sizeof(struct sockaddr_in);

	socklen_t           nAddrLen;
	nAddrLen = sizeof(struct sockaddr_in);


	while(1)
	{
		//as=accept(ls,(struct sockaddr *)&ci, &ci_size);
		as=accept(ls,(struct sockaddr *)&ci, &nAddrLen);
		if( as == -1 )
			return SVC_ERROR_NET;
		#ifndef WIN32
		asprintf(&tmp_adr,"Connection from %s:%i",
                         inet_ntoa(ci.sin_addr),ntohs(ci.sin_port));
		svc_log(SVC_OK,SVC_LOG_INFO,tmp_adr,"accept",NULL,0,0);
		free(tmp_adr);tmp_adr=NULL;
		#endif
		status=svc_net_read(as,SVC_MAX_BUF_LEN,&ks_req,&req_buf,&req_len);
		if( status != SVC_OK )
		{
			if( status == SVC_ERROR_FORMAT ) /* request w zlym formacie */
			{
				svc_log(svc_map_err_code(SVC_ERROR_FORMAT),SVC_LOG_ERROR,"Keyservice request has wrong format",
					    "svc_net_read",req_buf,req_len,0);
				status=svc_encode_response(SVC_RESPONSE_SIGN,SVC_RESPONSE_ERR_FORMAT,NULL,0,NULL,0,&ks_resp);
				if( status != SVC_OK )
					return status;
				else /* wyslij informacje od blednym formacie */
				{
					status=svc_net_write(as,ks_resp);
					free0(ks_req);free0(ks_resp);
					closesocket(as);
					return SVC_OK;	
				}
			}
			else /* blad odczytu z gniazda */
			{
				svc_log(svc_map_err_code(SVC_ERROR_NET),SVC_LOG_ERROR,"Unable to read keyservice request",
					    "svc_net_read",NULL,0,0);
			}
			return status; /* stad wychodzimy */
		}

		switch(ks_req->req_type) {
			case SVC_REQUEST_SIGN: 
				status=svc_thread_handle_sign_request(ks_req,req_buf,req_len,nr,&ks_resp);
				if( status != SVC_OK ) /* nie mozna przygotowac ks_resp */
					return status;
				break;
			case SVC_REQUEST_DECRYPT:
				status=svc_thread_handle_decrypt_request(ks_req,req_buf,req_len,nr,&ks_resp);
				if( status != SVC_OK ) /* nie mozna przygotowac ks_resp */
					return status;
				break;
			case SVC_REQUEST_GET_S_CERT: 
				status=svc_thread_handle_getcert_request(SVC_REQUEST_GET_S_CERT,&cert,&cert_length);
				if( status != SVC_OK )
					return status;
				break;
			case SVC_REQUEST_GET_ED_CERT: 
				status=svc_thread_handle_getcert_request(SVC_REQUEST_GET_ED_CERT,&cert,&cert_length);
				if( status != SVC_OK )
					return status;
				break;
			default:
				svc_log(svc_map_err_code(SVC_ERROR_FORMAT),SVC_LOG_ERROR,"Unknown request type was received",
					    "bmd_ks_thread",req_buf,req_len,0);
				status=svc_encode_response(SVC_RESPONSE_UNKNOWN,SVC_RESPONSE_ERR_TYPE,NULL,0,NULL,0,&ks_resp);
				if( status != SVC_OK )
					return status;
				break;
		}	
		if( ( cert !=  NULL ) && ( cert_length > 0 ) )
		{
			nwrite=send(as,(const char *)cert,cert_length,0);
			if( nwrite != cert_length )
				return SVC_ERROR_NET;
			if( ks_req->req_type == SVC_REQUEST_GET_S_CERT )
				svc_log(MSG_SVC_OK,SVC_LOG_INFO,"Sending signing cert to user",
				        NULL,cert,cert_length,0);
			else
				if( ks_req->req_type == SVC_REQUEST_GET_ED_CERT )
					svc_log(MSG_SVC_OK,SVC_LOG_INFO,"Sending encryption/decryption cert to user",
					        NULL,cert,cert_length,0);
		}
		else
		{
			status=svc_net_write(as,ks_resp);
		}
		free0(ks_req);free0(ks_resp);free0(cert);
		closesocket(as);
	}
	return SVC_OK;
}

void *bmd_ks_thread_unix(LPVOID lpParameter)
{
	DWORD status;

	status=bmd_ks_thread(lpParameter);
	/* dorobic jakies logowanie status tutaj  */	
	return NULL;
}

long svc_create_threads(SOCKET ls)
{
	long i;
#ifndef WIN32
	long status;
#endif

	thread_data=(ks_thread_data_t **)malloc(sizeof(ks_thread_data_t)*(svc_conf->max_threads+2));
	if( thread_data == NULL )
		return SVC_ERROR_MEM;
	memset(thread_data,0,sizeof(ks_thread_data_t)*(svc_conf->max_threads+1));

	#ifdef WIN32
    	/* tutaj utworzenie watkow - obe beda wolac accept */
    	hThreads=(HANDLE *)malloc(sizeof(HANDLE)*svc_conf->max_threads);
	if( hThreads == NULL )
		return SVC_ERROR_MEM;
    	memset(hThreads,0,sizeof(HANDLE)*svc_conf->max_threads);
#else
	hThreads=(pthread_t *)malloc(sizeof(pthread_t)*svc_conf->max_threads);
	if( hThreads == NULL )
		return SVC_ERROR_MEM;
	memset(hThreads,0,sizeof(pthread_t)*svc_conf->max_threads);
#endif
    	for(i=0;i<svc_conf->max_threads;i++)
    	{
		thread_data[i]=(ks_thread_data_t *)malloc(sizeof(ks_thread_data_t));
		if( thread_data[i] == NULL )
			return SVC_ERROR_MEM;
		memset(thread_data[i],0,sizeof(ks_thread_data_t));
		thread_data[i]->thread_number=i;
		thread_data[i]->s=ls;

#ifdef WIN32
		hThreads[i]=CreateThread(NULL,0,bmd_ks_thread,(LPVOID)thread_data[i],0,NULL);
        	if (hThreads[i] == INVALID_HANDLE_VALUE)
        	{
			svc_reportstatus(SERVICE_STOPPED,1,0,0);
			return SVC_ERROR_FAILED;
		}
#else
		status=pthread_create(&(hThreads[i]),NULL,bmd_ks_thread_unix,(void *)thread_data[i]);
		if( status != 0 )
		{
			return SVC_ERROR_FAILED;
		}	
#endif
	}

	return SVC_OK;
}

long svc_thread_handle_sign_request(bmd_ks_request_t *ks_req,char *req_buf,long req_len,long nr,
								   bmd_ks_response_t **ks_resp)
{
	long status;
	char *crypto_data=NULL;
	long crypto_length;
	char *error=NULL,*keyid_w=NULL;

	status=svc_pkcs11_sign(ks_req->data,*(ks_req->data_length),
						   _GLOBAL_svc_p11->hSigSes[nr],_GLOBAL_svc_p11->hSigKey[nr],
						   _GLOBAL_svc_p11->pFunctionList,
						   &crypto_data,&crypto_length,&error);
	if( status != SVC_OK )
	{
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_pkcs11_sign",req_buf,req_len,0);
		status=svc_encode_response(SVC_RESPONSE_SIGN,SVC_RESPONSE_ERR_INTERNAL,ks_req->data,
								   *(ks_req->data_length),
		                           NULL,0,ks_resp);
		if( status != SVC_OK )
			return status;
	}
	else
	{
		svc_log(MSG_SVC_SIGN_OK,SVC_LOG_INFO,svc_conf->sig_key_id,"svc_pkcs11_sign",req_buf,req_len,0);
		status=svc_encode_response(SVC_RESPONSE_SIGN,SVC_RESPONSE_OK,ks_req->data,
								   *(ks_req->data_length),
								   crypto_data,crypto_length,ks_resp);
		if( status != SVC_OK )
			return status;
	}
	free0(crypto_data);free0(keyid_w);free0(error);

	return SVC_OK;
}

long svc_thread_handle_decrypt_request(bmd_ks_request_t *ks_req,char *req_buf,long req_len,
									  long nr,bmd_ks_response_t **ks_resp)
{
	long status;
	char *crypto_data=NULL;
	long crypto_length;
	char *error=NULL,*keyid_w=NULL;

	status=svc_pkcs11_decrypt(ks_req->data,*(ks_req->data_length),_GLOBAL_svc_p11->hDecSes[nr],_GLOBAL_svc_p11->hDecKey[nr],
							  _GLOBAL_svc_p11->pFunctionList,&crypto_data,&crypto_length,&error);
	if( status != SVC_OK )
	{
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_pkcs11_decrypt",req_buf,req_len,0);
		status=svc_encode_response(SVC_RESPONSE_DECRYPT,SVC_RESPONSE_ERR_INTERNAL,
			                       ks_req->data,*(ks_req->data_length),
		                           NULL,0,ks_resp);
		if( status != SVC_OK )
			return status;
	}
	else
	{
		svc_log(MSG_SVC_DECRYPT_OK,SVC_LOG_INFO,svc_conf->dec_key_id,"svc_pkcs11_decrypt",req_buf,req_len,0);
		status=svc_encode_response(SVC_RESPONSE_DECRYPT,SVC_RESPONSE_OK,ks_req->data,
								   *(ks_req->data_length),crypto_data,crypto_length,ks_resp);
		if( status != SVC_OK )
			return status;
	}

	free0(crypto_data);free0(error);free0(keyid_w);

	return SVC_OK;
}

long svc_thread_handle_getcert_request(char req_type,char **cert,long *cert_length)
{
	short tmp;

	if( req_type == SVC_REQUEST_GET_S_CERT )
	{
		tmp=htons((short)_GLOBAL_svc_p11->sig_cert_length);
		(*cert)=(char *)malloc(_GLOBAL_svc_p11->sig_cert_length+2);
		if( (*cert) == NULL )
			return SVC_ERROR_MEM;
		memmove(*cert,&tmp,sizeof(short));
		memmove(*cert+2,_GLOBAL_svc_p11->sig_cert,_GLOBAL_svc_p11->sig_cert_length);
		*cert_length=_GLOBAL_svc_p11->sig_cert_length+2;
	}
	else
		if( req_type == SVC_REQUEST_GET_ED_CERT )
		{
			if( _GLOBAL_svc_p11->ed_cert == NULL )
			{
				tmp=htons(9);
				(*cert)=(char *)malloc(11);
				memmove(*cert,&tmp,sizeof(short));
				memmove(*cert+2,"UNDEFINED",9);
				*cert_length=11;
				return SVC_OK;
			}
			tmp=htons((short)_GLOBAL_svc_p11->ed_cert_length);
			(*cert)=(char *)malloc(_GLOBAL_svc_p11->ed_cert_length+2);
			if( (*cert) == NULL )
				return SVC_ERROR_MEM;
			memmove(*cert,&tmp,sizeof(short));
			memmove(*cert+2,_GLOBAL_svc_p11->ed_cert,_GLOBAL_svc_p11->ed_cert_length);
			*cert_length=_GLOBAL_svc_p11->ed_cert_length+2;
		}
		else
			return SVC_ERROR_FORMAT;

	return SVC_OK;
}
