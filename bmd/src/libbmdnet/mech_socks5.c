#if 0
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <bmd/common/bmd-common.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

long __bmdnet_socks5_create(	bmdnet_handler_ptr handler)
{
	/* create a new TCP connection */
	bmdnet_handler_ptr connection ;

	bmdnet_create(BMD_NET_PLAINTCP_METHOD,handler->action_on_error, &connection);

	handler->data = (void**) malloc( sizeof(void *)*8 );
	handler->data[CONNECTION]				= connection;
	handler->data[PROXY_HOST]				= NULL;
	handler->data[PROXY_PORT]				= NULL;
	handler->data[PROXY_SOCKS_DEST_PORT]	= NULL; /** unused */
	handler->data[PROXY_SOCKS_DEST_ADDR]	= NULL; /** unused */
	handler->data[PROXY_USERNAME]			= NULL;
	handler->data[PROXY_PASSWORD]			= NULL;
	handler->data[PROXY_AUTH_TYPE]			= NULL; /* Jak na razie None, Basic lub NTLM (win32 - SSPI) */
	return 0;
};

typedef struct socks5_init_request
{
	char version;
	char nmethod;
} socks5_init_request_t; /* max do 255 bajtow wspieranych metod dodac przy budowaniu */

typedef struct socks5_request
{
	char ver;
	char cmd;
	char rsv;
	char atyp;
} socks5_request_t; /* dest_atr oraz port __int16 dodac do complete_request*/

/** @test untested */
long __bmdnet_socks5_connect(	bmdnet_handler_ptr handler,
					char*host,
					long port)
{
	long status					= 0;
	long i						= 0;
	long ip_flag					= 1;
	char *buffer					= NULL;
	socks5_init_request_t init_request_header;
	socks5_request_t request_header;
	char complete_request[1024];
	char initial_request[1024]; /** @bug to musi byc dynamicznie ustalane */
	__int32 tmp1;
	__int16 tmp2;

	if (handler->data[PROXY_HOST] == NULL)	{	BMD_FOK(-1);	}
	if (handler->data[PROXY_PORT] == NULL)	{	BMD_FOK(-1);	}

	if(handler->data[PROXY_AUTH_TYPE])
	{
		if(strcmp((char *)handler->data[PROXY_AUTH_TYPE],"basic")!=0)
		{
			BMD_FOK(-1);
		}
		else
		{
			if( (handler->data[PROXY_PASSWORD]==NULL) || (handler->data[PROXY_USERNAME]==NULL) )
			{
				BMD_FOK(-1);
			}
		}
	}

	PRINT_VDEBUG("LIBBMDNETINF Connecting to proxy server %s:%li...\n",
				(char*) handler->data[PROXY_HOST], 
				*((long*)handler->data[PROXY_PORT])); 


	status = bmdnet_connect(handler->data[CONNECTION], handler->data[PROXY_HOST], *((long*)handler->data[PROXY_PORT]));
	if (status != 0)
	{
		PRINT_DEBUG("LIBBMDNETERR Cannot connect to proxy server %s:%li. Error=%li\n",
				(char*) handler->data[PROXY_HOST], 
				*((long*)handler->data[PROXY_PORT]), 
				status);
		BMD_FOK(-2);
	}

	PRINT_VDEBUG("LIBBMDNETINF Connection with proxy established.\n");

	/* wysylanie inicjalnego requesta z deklaracja metod */
	memset(initial_request,0,1024);init_request_header.version=0x05;init_request_header.nmethod=1;
	memmove(initial_request,(char *)(&init_request_header),sizeof(init_request_header));
	if(handler->data[PROXY_AUTH_TYPE])
	{
		if(strcmp((char *)handler->data[PROXY_AUTH_TYPE],"basic")==0)
		{
			initial_request[sizeof(init_request_header)]=0x02;
		}
	}
	BMD_FOK_CHG(bmdnet_send(handler->data[CONNECTION],initial_request,sizeof(init_request_header)+1), -1);

	buffer=(char *)malloc(1024);
	memset(buffer,0,1024);

	BMD_FOK_CHG(bmdnet_recv(handler->data[CONNECTION],buffer,1023,1),-1);

	if( ((unsigned char)buffer[1]) == 0xFF) /* zadna metoda autoryzacji nie jest wspierana - patrz RFC */
	{
			BMD_FOK(-1);
	}

	free(buffer);buffer=NULL;

	/* faza autoryzacji user/password */
	if(handler->data[PROXY_AUTH_TYPE])
	{
		if(strcmp((char *)handler->data[PROXY_AUTH_TYPE],"basic")==0)
		{
			buffer=(char *)malloc(1024);
			memset(buffer,0,1024);
			buffer[0]=0x01;buffer[1]=(char)strlen((char *)handler->data[PROXY_USERNAME]);
			memmove(buffer+2,(char *)handler->data[PROXY_USERNAME],strlen((char *)handler->data[PROXY_USERNAME]));
			buffer[2+strlen((char *)handler->data[PROXY_USERNAME])]=(char)strlen((char *)handler->data[PROXY_PASSWORD]);
			memmove(buffer+3+strlen((char *)handler->data[PROXY_USERNAME]),(char *)handler->data[PROXY_PASSWORD],
				    strlen((char *)handler->data[PROXY_PASSWORD]));

			BMD_FOK_CHG(bmdnet_send(handler->data[CONNECTION],buffer,(long)strlen(buffer)),-1);

			memset(buffer,0,1024);
			BMD_FOK_CHG(bmdnet_recv(handler->data[CONNECTION],buffer,1023,1),-1);

			if(buffer[1]!=0x00)
			{
				BMD_FOK(-1);
			}
		}
	}

	memset(complete_request,0,256);request_header.ver=0x05;request_header.cmd=1;request_header.rsv=0x00;
	for(i=0;i<(long)strlen(host);i++)
	{
		if(isalpha(host[i])) /* jesli wystapi jakas litera uznaje ze to adres domenowy */
		{
			ip_flag=0;
			break;
		}
	}
	if(ip_flag==1)
	{
		request_header.atyp=0x01; /* to jest IP */
	}
	else
	{
		request_header.atyp=0x03; /* to jest domena */
	}
	memmove(complete_request,(char *)(&request_header),sizeof(request_header));

	if(ip_flag==1)
	{
		tmp1=inet_addr(host);
	}
	tmp2=htons((u_short)port);
	if(ip_flag==1)
	{
		memmove(complete_request+sizeof(request_header),&tmp1,4);
		memmove(complete_request+sizeof(request_header)+4,&tmp2,2);
		status=bmdnet_send(handler->data[CONNECTION],complete_request,sizeof(request_header)+6);
		if( status < 0 )
		{
			return -1;
		}
	}
	else
	{
		char *tmp=NULL;
		tmp=(char *)malloc(strlen(host)+3);
		tmp[0]=(char)strlen(host);
		memmove(tmp+1,host,strlen(host));
		memmove(complete_request+sizeof(request_header),tmp,strlen(host)+1);
		memmove(complete_request+sizeof(request_header)+strlen(host)+1,&tmp2,2);
		BMD_FOK_CHG(bmdnet_send(handler->data[CONNECTION],complete_request,sizeof(request_header)+ 
		(long)strlen(host)+3),-1);
		free(tmp);tmp=NULL;
	}

	buffer=(char *)malloc(1024);
	memset(buffer,0,1024);
	BMD_FOK_CHG(bmdnet_recv(handler->data[CONNECTION],buffer,1023,1),-1);

	if(buffer[1]==0x00) /* OK*/
		return 0;
	/* jakis blad - nie interesuje nas jaki */
	return -1;
};

long __bmdnet_socks5_send(	bmdnet_handler_ptr handler,
					void * buffer,
					long buffer_size)
{
	return bmdnet_send (handler->data[CONNECTION], buffer, buffer_size);
};


long __bmdnet_socks5_recv(	bmdnet_handler_ptr handler,
					void * buffer,
					long buffer_size, 
					long blocking)
{
	return bmdnet_recv (handler->data[CONNECTION], buffer, buffer_size, blocking);
};


long __bmdnet_socks5_close(	bmdnet_handler_ptr handler)
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

	if (handler->data[PROXY_SOCKS_DEST_PORT] != NULL)
	{
		free(handler->data[PROXY_SOCKS_DEST_PORT]);
	}

	if (handler->data[PROXY_SOCKS_DEST_ADDR] != NULL)
	{
		free(handler->data[PROXY_SOCKS_DEST_ADDR]);
	}

	if (handler->data[CONNECTION] != NULL)
	{
		free(handler->data[CONNECTION]);
	}

	if (handler->data[PROXY_USERNAME] != NULL)
	{
		free(handler->data[PROXY_USERNAME]);
	}

	return status;
}

/** Ustawianie opcji proxy typu SOCKS5

@param handler uchwyt połączenia
@param option opcja do ustawienia:
* 1 - PROXY HOST; musi być ustawione przed connect'em
* 2 - PROXY PORT; musi być ustawione przed connect'em

@param value wartość do ustawienia
@param value_size rozmiar tejże wartości (nie używany)

@return 0 w wypadku powodzenia, wartość niezerowa w wypadku błędu
*/

long __bmdnet_socks5_set_option(	bmdnet_handler_ptr handler, 
						const long option,
						const void * const value,
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
			handler->data[PROXY_PORT] = (long*) malloc(sizeof(long));
			* ((long*) handler->data[PROXY_PORT]) = * ((long*) value);
		}
		else
		{
			if (option==PROXY_SOCKS_DEST_PORT)
			{
					handler->data[PROXY_SOCKS_DEST_PORT]=(long *)malloc(sizeof(long));
					* ((long*) handler->data[PROXY_SOCKS_DEST_PORT])=* ((long*) value);
			}
			else
			{
				if(option==PROXY_SOCKS_DEST_ADDR)
				{
					handler->data[PROXY_SOCKS_DEST_ADDR]=strdup(value);
				}
				else
				{
					if(option==PROXY_USERNAME)
					{
						handler->data[PROXY_USERNAME]=strdup(value);
					}
					else
						if(option==PROXY_PASSWORD)
						{
							handler->data[PROXY_PASSWORD]=strdup(value);
						}
						else
							if(option==PROXY_AUTH_TYPE)
							{
								handler->data[PROXY_AUTH_TYPE]=strdup(value);
							}
							else
								return 2;
				}
			}
		}
	}
	return 0;
}

#ifndef WIN32
const bmdnet_method_t __bmdnet_method_SOCKS5 = {
	BMD_NET_SOCKS5_METHOD,

	NULL, /*  _bmdnet_plaintcp_init, */
	NULL, /*__bmdnet_plaintcp_destroy, */

	__bmdnet_socks5_create,
	__bmdnet_socks5_connect,
	__bmdnet_socks5_close,

	__bmdnet_socks5_send,
	__bmdnet_socks5_recv,

	NULL, /* _bmdnet_plaintcp_bind, */
	NULL, /* __bmdnet_plaintcp_accept */
	__bmdnet_socks5_set_option,
};
#else

bmdnet_method_t __bmdnet_method_SOCKS5;
void init_bmdnet_method_SOCKS5()
{
	bmdnet_method_t *tmp_method=NULL;
	tmp_method=(bmdnet_method_t *)malloc(sizeof(bmdnet_method_t));

	tmp_method->id=BMD_NET_SOCKS5_METHOD;
	tmp_method->init=NULL;
	tmp_method->destroy=NULL;
	tmp_method->create=__bmdnet_socks5_create;
	tmp_method->connect=__bmdnet_socks5_connect;
	tmp_method->close=__bmdnet_socks5_close;
	tmp_method->send=__bmdnet_socks5_send;
	tmp_method->recv=__bmdnet_socks5_recv;
	tmp_method->bind=NULL;
	tmp_method->accept=NULL;
	tmp_method->set_option = __bmdnet_socks5_set_option;

	memmove(&__bmdnet_method_SOCKS5,tmp_method,sizeof(bmdnet_method_t));
}

#endif
#endif
