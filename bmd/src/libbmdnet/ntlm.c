#include <bmd/common/bmd-common.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdnet/ntlm.h>
#include <bmd/libbmderr/libbmderr.h>
#ifdef WIN32
#pragma warning(disable:4057)
#pragma warning(disable:4100)
#endif

long __write_to_file(char *name,char *buf,long length)
{
#if 0
	FILE *fp=NULL;
	fp=fopen(name,"a+b");
	fwrite(buf,sizeof(char),length,fp);
	fclose(fp);
#endif
	return 0;
}

long bmd_sspi_init(ntlm_auth_t *auth)
{
	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx(&osver);
	if (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 4)
		auth->hSecDll = LoadLibraryA("security.dll");
	else
		auth->hSecDll = LoadLibraryA("secur32.dll");
	if (auth->hSecDll != NULL) 
	{
		INIT_SECURITY_INTERFACE_A pInitSecurityInterface;
		pInitSecurityInterface = (INIT_SECURITY_INTERFACE_A)GetProcAddress(auth->hSecDll,"InitSecurityInterfaceA");
		if (pInitSecurityInterface != NULL)
			auth->pSecFn = pInitSecurityInterface();
	}
	else
		return -2;

	if (auth->pSecFn == NULL)
		return -1; 

	__write_to_file("bmdnet.log","Pobrano uchwyt i tablice funkcji z biblioteki SSPI\r\n",
					(long)strlen("Pobrano uchwyt i tablice funkcji z biblioteki SSPI\r\n"));

	return 0;
}

long bmd_sspi_acquire_method(ntlm_auth_t *auth,char *method)
{
	SECURITY_STATUS status;

	if(method==NULL)
		return -1;
	if(strcmp(method,"NTLM")!=0)
		return -1;

	status=auth->pSecFn->AcquireCredentialsHandleA(NULL,
												   method,
												   SECPKG_CRED_OUTBOUND, 
												   0,
												   NULL,
												   NULL,
												   NULL,
												   &(auth->initial_cred_handle),
												   &(auth->tsDummy)
												   );
	if(status!=SEC_E_OK)
		return -1;

	__write_to_file("bmdnet.log","Pobrano dane o zalogowanym uzytkowniku\r\n",(long)strlen("Pobrano dane o zalogowanym uzytkowniku\r\n"));
	return 0;
}

long ntlm_auth_init(ntlm_auth_t *auth)
{
	long i;

	auth->handler=NULL;
	auth->host=NULL;
	auth->port=-1;
	auth->base_http_request=NULL;	/* request, ktory idzie do serwera, a musi podlegac autoryzacji */
	auth->header_auth=NULL;			/* naglowek ktory okresla autoryzacje */
	
	auth->ntlm_internal_buf=NULL;	/* bufor do wewnetrznych operacji przy autoryzacji NTLM */
	auth->hSecDll=NULL;				/* uchwyt do biblioteki */
	auth->pSecFn=NULL;

	auth->ntlm_auth_result=-1;		/* wynik autoryzacji NTLM */
	auth->ntlm_current_stage=-1;	/* aktualny stadium autoryzacji NTLM */

	auth->internal_buffers=NULL;	/* wewnetrzne bufory do autoryzacji NTLM */
	auth->temp_buffer=NULL;

	auth->internal_buffers=(char **)malloc(4 * sizeof(char *));
	
	for(i=0;i<3;i++)
	{
		auth->internal_buffers[i]=(char *)malloc(32768 * sizeof(char) );
		memset(auth->internal_buffers[i],0, 32768 * sizeof(char) );
	}
	auth->internal_buffers[3]=NULL;
	
	auth->attrs=-1;
	auth->auth_header_resp=NULL;
	return 0;
}

long ntlm_auth_set(char *host,long port,char *base_http_request,char *header_auth,bmdnet_handler_ptr *handler,char *auth_header_resp,
				  char *connection_accepted_string,
				  ntlm_auth_t *auth)
{
	auth->host=strdup(host);
	auth->port=port;
	auth->base_http_request=strdup(base_http_request);
	auth->header_auth=strdup(header_auth);
	auth->handler=handler;
	auth->auth_header_resp=strdup(auth_header_resp);
	auth->connection_accepted_string=strdup(connection_accepted_string);
	return 0;
}

/* wysyla poczatkowy request */
long _bmd_ntlm_initial_request_send(ntlm_auth_t *auth)
{
	long status=-1;
	char *tmp=NULL;
	asprintf(&tmp,"%s\r\n",auth->base_http_request);
	__write_to_file("bmdnet.log","-----------------------INITIAL REQUEST START-------------------------------\r\n",
					(long)strlen("-----------------------INITIAL REQUEST START-------------------------------\r\n"));
	__write_to_file("bmdnet.log",tmp,(long)strlen(tmp));
	__write_to_file("bmdnet.log","-----------------------INITIAL REQUEST STOP-------------------------------\r\n",
					(long)strlen("-----------------------INITIAL REQUEST STOP-------------------------------\r\n"));
	status=bmdnet_send(*(auth->handler),tmp,(long)strlen(tmp));
	if(status<0)
		return -1;

	__write_to_file("bmdnet.log","Poslano INITIAL REQUEST\r\n",(long)strlen("Poslano INITIAL REQUEST\r\n"));
	return 0;
}

/* odbiera odpowiedz na poczatkowy request oraz sprawdza czy jest autoryzacja NTLM */
long _bmd_ntlm_initial_request_recv(ntlm_auth_t *auth)
{
	long status;
	
	char *tmp=NULL;
	char *pos_auth=NULL,*pos_auth_ntlm=NULL;

	auth->temp_buffer=(char *)malloc(32768 * sizeof(char) );
	memset(auth->temp_buffer, 0, 32768 * sizeof(char) );
	status=bmdnet_recv(*(auth->handler),auth->temp_buffer, 32768 * sizeof(char));
	if(status<0)
		return -1;
	__write_to_file("bmdnet.log","----------------------------INITIAL RESPONSE START-----------------------\r\n",
					(long)strlen("----------------------------INITIAL RESPONSE START-----------------------\r\n"));
	__write_to_file("bmdnet.log",auth->temp_buffer,status);
	__write_to_file("bmdnet.log","----------------------------INITIAL RESPONSE STOP-----------------------\r\n",
					(long)strlen("----------------------------INITIAL RESPONSE STOP-----------------------\r\n"));
	__write_to_file("bmdnet.log","Odebrano INITIAL RESPONSE\r\n",(long)strlen("Odebrano INITIAL RESPONSE\r\n"));

	/* czy jest jakas autoryzacja wymagana */
	pos_auth=strstr(auth->temp_buffer,auth->auth_header_resp);

	/* czy jest autoryzacja NTLM */
	asprintf(&tmp,"%s%s",auth->auth_header_resp,"NTLM");
	pos_auth_ntlm=strstr(auth->temp_buffer,tmp);
	
	if( (pos_auth!=NULL) && (pos_auth_ntlm==NULL) )
		return -1;

	if(pos_auth==NULL)
		return 1;
	free(auth->temp_buffer);auth->temp_buffer=NULL;

	__write_to_file("bmdnet.log","Jedna z wymaganych metod autoryzacji jest NTLM\r\n",
					(long)strlen("Jedna z wymaganych metod autoryzacji jest NTLM\r\n"));
	return 0;
}

long _ntlm_first_stage(ntlm_auth_t *auth)
{
	SECURITY_STATUS status=-1;
	auth->temp_buffer=(char *)malloc(32768 * sizeof(char) );
	memset(auth->temp_buffer,0,32768 * sizeof(char) );

	auth->ntlm_desc_1.ulVersion = SECBUFFER_VERSION;
	auth->ntlm_desc_1.cBuffers  = 1;
	auth->ntlm_desc_1.pBuffers  = &(auth->ntlm_1);
	auth->ntlm_1.cbBuffer   = 32768 * sizeof(char);
	auth->ntlm_1.BufferType = SECBUFFER_TOKEN;
	auth->ntlm_1.pvBuffer   = auth->internal_buffers[0]; 

	status=auth->pSecFn->InitializeSecurityContextA(&(auth->initial_cred_handle),
													NULL,
													NULL,
													ISC_REQ_CONFIDENTIALITY|ISC_REQ_REPLAY_DETECT|ISC_REQ_CONNECTION,
													0,
													SECURITY_NETWORK_DREP,
													NULL, 
													0,
													&(auth->next_cred_handle),
													&(auth->ntlm_desc_1),
													&(auth->attrs),
													&(auth->tsDummy)
												   );
	if (auth->ntlm_1.cbBuffer > 32768)
		return -29000000;
	if (status == SEC_I_COMPLETE_AND_CONTINUE || status == SEC_I_COMPLETE_NEEDED )
	{
		status=-1;
		status=auth->pSecFn->CompleteAuthToken(&(auth->next_cred_handle), &(auth->ntlm_desc_1));
		return 0;
	}
	else
		if(status==SEC_I_CONTINUE_NEEDED)
		{
			char *base64_encoded=NULL;
			long code;
			char *request_again=NULL;

			__write_to_file("bmdnet.log","Przygotowano pierwszy token autoryzacji NTLM\r\n",
							(long)strlen("Przygotowano pierwszy token autoryzacji NTLM\r\n"));
			/* robmy tak aby zrobic dobrze czyli tak jak IE  na pale ustawic flagi */
#ifdef _SAME_NTLM_AS_IE_
			int tmp;
			char *p=NULL;
			p=(char *)auth->ntlm_desc_1.pBuffers[0].pvBuffer;
			tmp=0xa208b207;
			p=p+12;
			*((int *)p)=tmp; /*TW Slabe miejsce. Nie wiem gdzie to jest przypisywane*/
#endif
			base64_encoded=spc_base64_encode(	(char *)auth->ntlm_desc_1.pBuffers[0].pvBuffer,
								 auth->ntlm_desc_1.pBuffers[0].cbBuffer,0);

			asprintf(&request_again,"%sProxy-Authorization: NTLM %s\r\n\r\n",
					 auth->base_http_request,
					 base64_encoded);

			bmdnet_close(auth->handler);
			bmdnet_create(auth->handler);
			bmdnet_connect(*(auth->handler),auth->host,auth->port);
			
			__write_to_file("bmdnet.log","--------------- Wysylanie Message Type 1 -------------------\r\n",
							(long)strlen("--------------- Wysylanie Message Type 1 -------------------\r\n"));
			__write_to_file("bmdnet.log",request_again,(long)strlen(request_again));
			__write_to_file("bmdnet.log","--------------- Wysylanie Message Type 1 -------------------\r\n",
							(long)strlen("--------------- Wysylanie Message Type 1 -------------------\r\n"));
			code=bmdnet_send(*(auth->handler),request_again,(long)strlen(request_again));
			if(code<0)
				return -1;
			__write_to_file("bmdnet.log","Poslano Message Type 1\r\n",(long)strlen("Poslano Message Type 1\r\n"));
			free(base64_encoded);base64_encoded=NULL;

			code=bmdnet_recv(*(auth->handler),auth->temp_buffer,32768);
			if(code<0)
				return -1;

			__write_to_file("bmdnet.log","Odebrano bufor z prawdopodobna wiadomoscia Message Type 2\r\n",
							(long)strlen("Odebrano bufor z prawdopodobna wiadomoscia Message Type 2\r\n"));
		}
		else
			if(status!=SEC_E_OK)
				return -1;

	return 0;
}


long __ntlm_get_first_server_token(ntlm_auth_t *auth)
{
	char *tmp=NULL;
	char *pos=NULL;
	char *base64_decoded=NULL;
	long i;
	size_t size;
	int err;

	asprintf(&tmp,"%s%s ",auth->auth_header_resp,"NTLM");
	
#if 0
	{
		long fd;
		fd=open("ntlm_get_first_server_token.log",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,0666);
		write(fd,auth->temp_buffer,strlen(auth->temp_buffer));
		close(fd);
	}
#endif

	pos=strstr(auth->temp_buffer,tmp);
	if(pos==NULL)
		return -1;


	__write_to_file("bmdnet.log","Odpowiedz zawierala wiadomosc Message Type 2\r\n",
					(long)strlen("Odpowiedz zawierala wiadomosc Message Type 2\r\n"));
	pos=pos+strlen(tmp);

	for(i=0;i<1024;i++)
	{
		if(pos[i]!='\r')
			auth->internal_buffers[1][i]=pos[i];
		else
			break;
	}

	base64_decoded=(char *)malloc(32768 * sizeof(char));
	memset(base64_decoded,0,32768 * sizeof(char));
	base64_decoded=spc_base64_decode((unsigned char *)auth->internal_buffers[1],&size,1,&err);
	free(auth->internal_buffers[1]);
	auth->internal_buffers[1]=(char *)malloc(32768 * sizeof(char));
	memmove(auth->internal_buffers[1],base64_decoded,size);
	free(base64_decoded);base64_decoded=NULL;

	auth->ntlm_desc_2.ulVersion  = SECBUFFER_VERSION;
	auth->ntlm_desc_3.ulVersion  = SECBUFFER_VERSION;

	auth->ntlm_desc_2.cBuffers   = 1;
	auth->ntlm_desc_3.cBuffers   = 1;

	auth->ntlm_desc_2.pBuffers   = &(auth->ntlm_2);
	auth->ntlm_desc_3.pBuffers   = &(auth->ntlm_3);

	auth->ntlm_2.BufferType = SECBUFFER_TOKEN;
	auth->ntlm_2.pvBuffer   = auth->internal_buffers[1];
	auth->ntlm_2.cbBuffer   = (unsigned long)size;
	auth->ntlm_3.BufferType = SECBUFFER_TOKEN;
	auth->ntlm_3.pvBuffer   = auth->internal_buffers[2];
	auth->ntlm_3.cbBuffer   = 32768;


	return 0;

}

long _ntlm_second_stage(ntlm_auth_t *auth)
{
	long code;
	SECURITY_STATUS status;
	char *pos=NULL;
	char *base64_encoded_type_3=NULL;
	code=__ntlm_get_first_server_token(auth);
	
	if(code!=0)
		return -1;

	status=-1;
	status = auth->pSecFn->InitializeSecurityContextA(&(auth->initial_cred_handle),
													  &(auth->next_cred_handle),
													  NULL,
													  ISC_REQ_CONFIDENTIALITY |ISC_REQ_REPLAY_DETECT |ISC_REQ_CONNECTION,
													  0, 
													  SECURITY_NETWORK_DREP,
													  &(auth->ntlm_desc_2),
													  0,
													  &(auth->next_cred_handle), 
													  &(auth->ntlm_desc_3),
													  &auth->attrs, 
													  &(auth->tsDummy)
													 );
	if(status!=SEC_E_OK)
		return -1;
	

	__write_to_file("bmdnet.log","Wygenerowano wiadomosc Message Type 3\r\n",
					(long)strlen("Wygenerowano wiadomosc Message Type 3\r\n"));

	free(auth->temp_buffer);auth->temp_buffer=NULL;

	base64_encoded_type_3=spc_base64_encode(	(char *)auth->ntlm_desc_3.pBuffers[0].pvBuffer,
							auth->ntlm_desc_3.pBuffers[0].cbBuffer,0);
	asprintf(&(auth->temp_buffer),"%sProxy-Authorization: NTLM %s\r\n\r\n",
			 auth->base_http_request,base64_encoded_type_3);
	free(base64_encoded_type_3);base64_encoded_type_3=NULL;

	__write_to_file("bmdnet.log","--------------- Wysylanie Message Type 3 -------------------\r\n",
							(long)strlen("--------------- Wysylanie Message Type 3 -------------------\r\n"));
	__write_to_file("bmdnet.log",auth->temp_buffer,(long)strlen(auth->temp_buffer));
	__write_to_file("bmdnet.log","--------------- Wysylanie Message Type 3 -------------------\r\n",
					(long)strlen("--------------- Wysylanie Message Type 3 -------------------\r\n"));

	code=bmdnet_send(*(auth->handler),auth->temp_buffer,(long)strlen(auth->temp_buffer));
	if(code<0)
		return -1;

	__write_to_file("bmdnet.log","Poslano Message Type 3\r\n",(long)strlen("Poslano Message Type 3\r\n"));
	free(auth->temp_buffer);
	auth->temp_buffer=(char *)malloc(32768 * sizeof(char));
	memset(auth->temp_buffer,0,32768 * sizeof(char));

	code=bmdnet_recv(*(auth->handler),auth->temp_buffer, 32768 * sizeof(char));
	if(code<0)
		return -1;


	__write_to_file("bmdnet.log","Odebrano finalna odpowiedz serwera\r\n",
					(long)strlen("Odebrano finalna odpowiedz serwera\r\n"));

	__write_to_file("bmdnet.log",auth->temp_buffer,(long)strlen(auth->temp_buffer));

	#if 0
	{
		long fd;
		fd=open("ntlm_get_message_3.log",O_CREAT|O_TRUNC|O_WRONLY|O_BINARY,0666);
		write(fd,auth->temp_buffer,strlen(auth->temp_buffer));
		close(fd);
	}
	#endif

	pos=strstr(auth->temp_buffer,auth->connection_accepted_string);
	if(pos)
	{
		auth->ntlm_auth_result=0;
		__write_to_file("bmdnet.log","Autoryzacja udana\r\n",(long)strlen("Autoryzacja udana\r\n"));
	}
	else
	{
		auth->ntlm_auth_result=-1;
		__write_to_file("bmdnet.log","Autoryzacja nieudana\r\n",(long)strlen("Autoryzacja nieudana\r\n"));
	}
	return 0;
}

long ntlm_auth_execute(ntlm_auth_t *auth)
{
	long code=-1;
	
	code=_bmd_ntlm_initial_request_send(auth);
	if(code!=0)
		return -1;

	code=_bmd_ntlm_initial_request_recv(auth);
	if(code==1)
	{
		auth->ntlm_auth_result=0;
		return 0; /* nie ma autoryzacji */
	}
	if(code!=0)
		return -1; /* cos nie tak z odbiorem lub niewspierany schemat autoryzacji */

	code=_ntlm_first_stage(auth);
	if(code!=0)
	{	
		if (code == -29000000)
			return code;
		return -1;
	}
	code=_ntlm_second_stage(auth);
	if(code!=0)
		return -1;
	return 0;
}

long ntlm_auth_cleanup(ntlm_auth_t *auth)
{
	long i=0;
	free(auth->host);
	free(auth->base_http_request);	
	free(auth->header_auth);
	
	free(auth->ntlm_internal_buf);

	while(auth->internal_buffers[i]!=NULL)
	{
		free(auth->internal_buffers[i]);
		i++;
	}
	free(auth->internal_buffers);
	free(auth->temp_buffer);
	free(auth->auth_header_resp);
	free(auth->connection_accepted_string);
	FreeLibrary(auth->hSecDll);
	return 0;
}
