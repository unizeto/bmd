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

long __bmdnet_socks4a_create(bmdnet_handler_ptr handler)
{
	/* create a new TCP connection */
	bmdnet_handler_ptr connection ;

	bmdnet_create(BMD_NET_PLAINTCP_METHOD,handler->action_on_error, &connection);
	
	handler->data = (void**) malloc( sizeof(void *)*6 );
	handler->data[CONNECTION]				= connection;
	handler->data[PROXY_HOST]				= NULL;
	handler->data[PROXY_PORT]				= NULL;
	handler->data[PROXY_SOCKS_DEST_PORT]	= NULL; /** unused */
	handler->data[PROXY_SOCKS_DEST_ADDR]	= NULL; /** unused */
	handler->data[PROXY_USERNAME]			= NULL;
	return 0;
};

typedef struct socks4a_request 
{
	char version;
	char command;
	__int16 destport;
	__int32 destaddr;
} socks4a_request_t;

/** @test untested */
long __bmdnet_socks4a_connect(bmdnet_handler_ptr handler,char*host, long port)
{
	long status;
	char * buffer=NULL;
	long response_code;
	socks4a_request_t request_header;
	long request_header_size;
	char complete_request[1024]; /** @bug to musi byc dynamicznie ustalane */
	long length;

	if (handler->data[PROXY_HOST] == NULL || handler->data[PROXY_PORT] == NULL) 
	{
		return -1;
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
		return -2;
	}

	PRINT_VDEBUG("LIBBMDNETINF Connection with proxy established.\n"); 
	
	memset(complete_request,0,256);
	request_header.version=4;
	request_header.command=1;
	request_header.destport=htons((u_short)port/*(__int16)*((long *)handler->data[PROXY_SOCKS_DEST_PORT])*/);
	request_header.destaddr=inet_addr("0.0.0.1");
	request_header_size=sizeof(request_header);
	memmove(complete_request,(char *)(&request_header),request_header_size);
	if(handler->data[PROXY_USERNAME])
	{
		length=(long)strlen((char *)handler->data[PROXY_USERNAME]);
		memmove(complete_request+request_header_size,(char *)handler->data[PROXY_USERNAME],length);
		memmove(complete_request+request_header_size+length+1,host,strlen(host));
		bmdnet_send(handler->data[CONNECTION], complete_request,length+request_header_size+(long)strlen(host)+2);
	}
	else
	{
		memmove(complete_request+request_header_size+1,host,strlen(host));
		bmdnet_send(handler->data[CONNECTION],complete_request,request_header_size+(long)strlen(host)+2);
	}
	buffer=(char *)malloc(10000);
	memset(buffer,0,10000);
	bmdnet_recv(handler->data[CONNECTION],buffer,1024,1);
	if((long)buffer[0]==0)
	{
		response_code=(long)buffer[1];
		switch(response_code) 
		{
			case 90:
				return 0;
			case 91:
				return -1;
			case 92:
				return -2;
			case 93:
				return -3;
		}
	}
	else
	{
		return -1;
	}
	return 0;
};

long __bmdnet_socks4a_send(bmdnet_handler_ptr handler, void * buffer, long buffer_size)
{
	return bmdnet_send (handler->data[CONNECTION], buffer, buffer_size);
};


long __bmdnet_socks4a_recv(bmdnet_handler_ptr handler, void * buffer,long buffer_size, 
	long blocking)
{
	return bmdnet_recv (handler->data[CONNECTION], buffer, buffer_size, blocking);
};


long __bmdnet_socks4a_close(bmdnet_handler_ptr handler)
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

/** Ustawianie opcji proxy typu SOCKS4A

@param handler uchwyt połączenia
@param option opcja do ustawienia:
* 1 - PROXY HOST; musi być ustawione przed connect'em
* 2 - PROXY PORT; musi być ustawione przed connect'em

@param value wartość do ustawienia
@param value_size rozmiar tejże wartości (nie używany)

@return 0 w wypadku powodzenia, wartość niezerowa w wypadku błędu
*/

long __bmdnet_socks4a_set_option(bmdnet_handler_ptr handler, 
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
					return 2;
				}
			}
		}
	}
	return 0;
}

#ifndef WIN32
const bmdnet_method_t __bmdnet_method_SOCKS4A = {
	BMD_NET_SOCKS4A_METHOD,
	
	NULL, /*  _bmdnet_plaintcp_init, */
	NULL, /*__bmdnet_plaintcp_destroy, */
	
	__bmdnet_socks4a_create,
	__bmdnet_socks4a_connect,
	__bmdnet_socks4a_close,
	
	__bmdnet_socks4a_send, 
	__bmdnet_socks4a_recv,
	
	NULL, /* _bmdnet_plaintcp_bind, */
	NULL, /* __bmdnet_plaintcp_accept */
	__bmdnet_socks4a_set_option,
};
#else

bmdnet_method_t __bmdnet_method_SOCKS4A;
void init_bmdnet_method_SOCKS4A()
{
	bmdnet_method_t *tmp_method=NULL;
	tmp_method=(bmdnet_method_t *)malloc(sizeof(bmdnet_method_t));

	tmp_method->id=BMD_NET_SOCKS4A_METHOD;
	tmp_method->init=NULL;
	tmp_method->destroy=NULL;
	tmp_method->create=__bmdnet_socks4a_create;
	tmp_method->connect=__bmdnet_socks4a_connect;
	tmp_method->close=__bmdnet_socks4a_close;
	tmp_method->send=__bmdnet_socks4a_send;
	tmp_method->recv=__bmdnet_socks4a_recv;
	tmp_method->bind=NULL;
	tmp_method->accept=NULL;
	tmp_method->set_option = __bmdnet_socks4a_set_option;

	memmove(&__bmdnet_method_SOCKS4A,tmp_method,sizeof(bmdnet_method_t));
}

#endif
#endif
