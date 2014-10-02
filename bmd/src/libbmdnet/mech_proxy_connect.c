#if 0
/***************************************************************************
 *            mech_plaintcp.c
 *  Plain TCP connection method
 *  Fri Mar  3 13:10:02 2006
 *  Copyright  2006  Lukasz Klimek, Unizeto S.A.
 *  lklimek@certum.pl
 ****************************************************************************/
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdnet/ntlm.h>
#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmderr/libbmderr.h>
#ifdef WIN32
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

long __bmdnet_connectproxy_create(bmdnet_handler_ptr handler)
{
bmdnet_handler_ptr connection ;

	bmdnet_create(BMD_NET_PLAINTCP_METHOD,handler->action_on_error, &connection);

	handler->data = (void **) malloc( sizeof(void *) * 8 );
	handler->data[CONNECTION] = connection;
	handler->data[PROXY_HOST]				= NULL;
	handler->data[PROXY_PORT]				= NULL;
	handler->data[PROXY_SOCKS_DEST_PORT]	= NULL; /** unused */
	handler->data[PROXY_SOCKS_DEST_ADDR]	= NULL; /** unused */
	handler->data[PROXY_USERNAME]			= NULL;
	handler->data[PROXY_PASSWORD]			= NULL;
	handler->data[PROXY_AUTH_TYPE]			= NULL; /* Jak na razie None, Basic lub NTLM (win32 - SSPI) */

	return 0;
};

/** @test untested */
long __bmdnet_connectproxy_connect(bmdnet_handler_ptr handler,char*host, long port)
{
	long status;
	char * buffer=NULL;
	char *proxy_auth_type=NULL;
	char *connection_proxy_ok="HTTP/1.1 200";
	char *connection_proxy_ok_1="HTTP/1.0 200";
	char http_headers_buf[64*1024];

	if (handler->data[PROXY_HOST] == NULL)	{	BMD_FOK(-1);	}
	if (handler->data[PROXY_PORT] == NULL)	{	BMD_FOK(-1);	}

	if (handler->data[PROXY_AUTH_TYPE])
	{
		proxy_auth_type=strdup((char *)handler->data[PROXY_AUTH_TYPE]);
	}

	PRINT_VDEBUG("LIBBMDNETINF Connecting to proxy server %s:%li...\n",
				(char *) handler->data[PROXY_HOST], 
				*((long *)handler->data[PROXY_PORT])); 


	status = bmdnet_connect(handler->data[CONNECTION], handler->data[PROXY_HOST], *((long *)handler->data[PROXY_PORT]));
	if (status != 0)
	{
		PRINT_DEBUG("LIBBMDNETERR Cannot connect to proxy server %s:%li. Error=%li\n",
				(char *) handler->data[PROXY_HOST], 
				*((long *)handler->data[PROXY_PORT]), 
				status);
		BMD_FOK(-2);
	}

	PRINT_VDEBUG("LIBBMDNETINF Connection with proxy established\n");
	if(proxy_auth_type==NULL) /* brak autoryzacji */
	{
		buffer=NULL;
		asprintf(&buffer,"CONNECT %s:%li HTTP/1.1\r\nHost: %s:%li\r\nUser-agent: %s\r\nProxy-Connection: %s\r\n\r\n",
				host,port,host,port,
				"viewer/1.0","Keep-Alive");
		bmdnet_send(handler->data[CONNECTION], buffer, (long)strlen(buffer));
		free(buffer);buffer=NULL;

		memset(http_headers_buf,0,64*1024-1);
		status = bmdnet_recv(handler->data[CONNECTION], http_headers_buf, 64*1024-1, 1);
		if (status < 0)
		{
			BMD_FOK(-3);
		}
		if(strstr(http_headers_buf,"\r\n\r\n")==NULL)
		{
			BMD_FOK(-1);
		}
		if(	(strstr(http_headers_buf,connection_proxy_ok)==NULL) &&
			(strstr(http_headers_buf,connection_proxy_ok_1)==NULL) )
		{
			BMD_FOK(-1);
		}
	}
	else
		if(strcmp(proxy_auth_type,"ntlm")==0)
		{
			#ifdef WIN32
			char *real_buffer=NULL,*proxy_host=NULL;
			long proxy_port;
			ntlm_auth_t auth;
			asprintf(&real_buffer,"CONNECT %s:%i HTTP/1.0\r\nUser-agent: %s\r\nHost: %s:%i\r\n%s",
					 host,port,"viewer/1.0",host,port,
					 "Content-Length: 0\r\nProxy-Connection: Keep-Alive\r\nPragma: no-cache\r\n"
					);

			proxy_host=strdup((char *)handler->data[PROXY_HOST]);
			proxy_port=*((long *)handler->data[PROXY_PORT]);
			BMD_FOK(ntlm_auth_init(&auth));
			BMD_FOK(bmd_sspi_init(&auth));
			BMD_FOK(bmd_sspi_acquire_method(&auth,"NTLM"));

			status=ntlm_auth_set(proxy_host,proxy_port,
								 real_buffer,
								 "Proxy-Authorization: ", /* TO MA DUZE ZNACZENIE */
								 &(bmdnet_handler_ptr)handler->data[CONNECTION],
								 "Proxy-Authenticate: ",
								 "HTTP/1.1 200 ",
								 &auth);
			status=ntlm_auth_execute(&auth);
			free(proxy_host);proxy_host=NULL;
			free(real_buffer);real_buffer=NULL;
			if(status==0)
			{
				if(auth.ntlm_auth_result==0)
					return 0;
				else
					BMD_FOK(-1);
			}
			else
			{
				if ( status == -29000000)
				{
					BMD_FOK(status);
				}
				BMD_FOK(-1);
			}
			#else
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
			#endif
		}
		else
			if(strcmp(proxy_auth_type,"basic")==0)
			{
				char *userpass=NULL;
				char *tmp=NULL;
				if (handler->data[PROXY_USERNAME] == NULL || handler->data[PROXY_PASSWORD] == NULL)
				{
					BMD_FOK(-1);
				}
				/* zakodowanie nazwy do BASE64 "nazwa_usera:haslo_usera" */
				asprintf(&tmp,"%s:%s",(char *)handler->data[PROXY_USERNAME],(char *)handler->data[PROXY_PASSWORD]);
				userpass=(char *)spc_base64_encode((unsigned char *)tmp,strlen(tmp),0);
				buffer=NULL;
				asprintf(&buffer,"CONNECT %s:%li HTTP/1.1\r\nHost: %s:%li\r\n%s\r\n%s\r\n%s%s\r\n\r\n",
						host,
						port,
						host,
						port,
						"User-agent: viewer/1.0",
						"Proxy-Connection: Keep-Alive",
						"Proxy-Authorization: Basic ",
						userpass);
				free(tmp);tmp=NULL;free(userpass);userpass=NULL;
				bmdnet_send(handler->data[CONNECTION], buffer, (long)strlen(buffer));
				free(buffer);buffer=NULL;
				memset(http_headers_buf,0,64*1024-1);
				BMD_FOK_CHG(bmdnet_recv(handler->data[CONNECTION], http_headers_buf, 64*1024-1, 1),	-3);
				if(strstr(http_headers_buf,"\r\n\r\n")==NULL)
				{
					BMD_FOK(-1);
				}
				if(	(strstr(http_headers_buf,connection_proxy_ok)==NULL) &&
					(strstr(http_headers_buf,connection_proxy_ok_1)==NULL) )
				{
					BMD_FOK(-1);
				}
			}
			else
			{
				BMD_FOK(BMD_ERR_UNIMPLEMENTED);
			}
	free(proxy_auth_type);proxy_auth_type=NULL;
	return 0;
};

long __bmdnet_connectproxy_send(bmdnet_handler_ptr handler, void * buffer, long buffer_size)
{
	return bmdnet_send (handler->data[CONNECTION], buffer, buffer_size);
};


long __bmdnet_connectproxy_recv(bmdnet_handler_ptr handler, void * buffer,long buffer_size, 
	long blocking)
{
	return bmdnet_recv (handler->data[CONNECTION], buffer, buffer_size, blocking);
};


long __bmdnet_connectproxy_close(bmdnet_handler_ptr handler)
{
	long status = 0;
	if (handler->data[CONNECTION] != NULL)
	{
		status = bmdnet_close((struct __bmdnet_handler_struct **)&( handler->data[CONNECTION] ));
	}

	if (handler->data[PROXY_HOST] != NULL)
	{
		free(handler->data[PROXY_HOST]);
	}

	if (handler->data[PROXY_PORT] != NULL)
	{
		free(handler->data[PROXY_PORT]);
	}

	if (handler->data[CONNECTION] != NULL)
	{
		free(handler->data[CONNECTION]);
	}

	return status;
}

/** Ustawianie opcji proxy typu CONNECT

@param handler uchwyt połączenia
@param option opcja do ustawienia:
* 1 - PROXY HOST; musi być ustawione przed connect'em
* 2 - PROXY PORT; musi być ustawione przed connect'em

@param value wartość do ustawienia
@param value_size rozmiar tejże wartości (nie używany)

@return 0 w wypadku powodzenia, wartość niezerowa w wypadku błędu
*/

long __bmdnet_connectproxy_set_option(bmdnet_handler_ptr handler, 
		const long option, const void * const value,
		const long value_size)
{
	if (option == PROXY_HOST)
	{
		handler->data[PROXY_HOST] = strdup(value);
	}
	else
	{
		if (option == PROXY_PORT)
		{
			handler->data[PROXY_PORT] = (long *) malloc(sizeof(long));
			* ((long *) handler->data[PROXY_PORT]) = * ((long *) value);
		}
		else
		{
			if (option == PROXY_AUTH_TYPE)
			{
				handler->data[PROXY_AUTH_TYPE]=strdup(value);
			}
			else
				if (option == PROXY_USERNAME)
				{
					handler->data[PROXY_USERNAME]=strdup(value);
				}
				else
					if (option == PROXY_PASSWORD)
					{
						handler->data[PROXY_PASSWORD]=strdup(value);
					}
					else
						return 2;
		}
	}
	return 0;
}

#ifndef WIN32
const bmdnet_method_t __bmdnet_method_CONNECT_PROXY = {
	BMD_NET_CONNECT_PROXY_METHOD,

	NULL, /*  _bmdnet_plaintcp_init, */
	NULL, /*__bmdnet_plaintcp_destroy, */

	__bmdnet_connectproxy_create,
	__bmdnet_connectproxy_connect,
	__bmdnet_connectproxy_close,

	__bmdnet_connectproxy_send,
	__bmdnet_connectproxy_recv,

	NULL, /* _bmdnet_plaintcp_bind, */
	NULL, /* __bmdnet_plaintcp_accept */
	__bmdnet_connectproxy_set_option,
};
#else

bmdnet_method_t __bmdnet_method_CONNECT_PROXY;
void init_bmdnet_method_CONNECT_PROXY()
{
	bmdnet_method_t *tmp_method=NULL;
	tmp_method=(bmdnet_method_t *)malloc(sizeof(bmdnet_method_t));

	tmp_method->id=BMD_NET_CONNECT_PROXY_METHOD;
	tmp_method->init=NULL;
	tmp_method->destroy=NULL;
	tmp_method->create=__bmdnet_connectproxy_create;
	tmp_method->connect=__bmdnet_connectproxy_connect;
	tmp_method->close=__bmdnet_connectproxy_close;
	tmp_method->send=__bmdnet_connectproxy_send;
	tmp_method->recv=__bmdnet_connectproxy_recv;
	tmp_method->bind=NULL;
	tmp_method->accept=NULL;
	tmp_method->set_option = __bmdnet_connectproxy_set_option;

	memmove(&__bmdnet_method_CONNECT_PROXY,tmp_method,sizeof(bmdnet_method_t));
}

#endif
#endif