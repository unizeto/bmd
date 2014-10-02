/* nawiazywanie polaczenia */
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdnet/ntlm.h>

#ifndef _WIN32
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <bmd/common/global_types.h>
#else
#pragma warning(disable:4100)
#pragma warning(disable:4701)
#pragma warning(disable:4127)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#endif

#define PROXY_CONNECTION_OK_1 "HTTP/1.1 200"
#define PROXY_CONNECTION_OK_2 "HTTP/1.0 200"

/* struktura do polaczen z serwerem proxy SOCKS4A */
typedef struct socks4a_request 
{
	char version;
	char command;
	__int16 destport;
	__int32 destaddr;
} socks4a_request_t;

/* struktura do polaczen z serwerem proxy SOCKS4 */
typedef struct socks4_request
{
	char version;
	char command;
	__int16 destport;
	__int32 destaddr;
} socks4_request_t;

/* struktura opisujaca inicjalny request SOCKS5 */
typedef struct socks5_init_request
{
	char version;
	char nmethod;
} socks5_init_request_t;

/* struktura opisujaca wlasciwy request SOCKS5 */
typedef struct socks5_request
{
	char ver;
	char cmd;
	char rsv;
	char atyp;
} socks5_request_t;

long tcp_connect(bmdnet_handler_ptr handler,char*host,long port)
{
	long status;
#ifndef _WIN32
	char aux[256];
	struct sockaddr_in serv_addr;
	struct hostent hostinfo_buf;
	struct hostent *hostinfo=&hostinfo_buf;
	long one = 1;
	int si_temp; /* to musi byc int - nie zmieniac !!!! */
#else
	struct addrinfo *result = NULL, hints;
	char * portstr=NULL;
#endif
	if (host == NULL || port == 0 || port > 65535)
	{
		return(-1);
	}

	handler->s = (long)socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	if (handler->s <= 0)
		return -1;

#ifndef WIN32
	if (setsockopt(handler->s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)))
	{
		return ERR(ERR_net, "Cannot set SO_REUSEADDR flag on socket", "LK");
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons((short)port);

	if( gethostbyname_r(host, &hostinfo_buf, aux, sizeof(aux), &hostinfo, &si_temp) != 0 )
	{
		PRINT_ERROR("gethostbyname_r failed.\n");
		return(-1);
	}
	if (hostinfo==NULL) {
		PRINT_ERROR("Hostname not found.\n");
		return(-1);
	}
	serv_addr.sin_addr=*(struct in_addr *)hostinfo->h_addr;
	status = connect(handler->s, (const struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if (status)
	{
		PRINT_ERROR("Cannot connect to host.\n");
		return(-1);
	}
#else
	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	portstr = calloc(6,sizeof(char));
	_itoa_s(port, portstr, 6,10);
	status = getaddrinfo(host, portstr, &hints, &result);
	if( status == WSAHOST_NOT_FOUND )
	{
		BMD_FOK(BMD_ERR_NET_RESOLV);
	}
	else
		if( status != 0 )
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	if (result == NULL)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	status = connect(handler->s, result->ai_addr,(long)result->ai_addrlen);
	if (status)
	{
		PRINT_ERROR("Cannot connect to host.\n");
		return(-1);
	}
#endif

	return BMD_OK;
};

long tcp_connect_http_proxy(bmdnet_handler_ptr handler,char *host,long port)
{
	long status;
	char *proxy_buffer=NULL;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( host == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};
	
	if( handler->proxy_adr == NULL ) { BMD_FOK(BMD_ERR_FORMAT);};
	if( handler->proxy_type != BMDNET_PROXY_HTTP ) { BMD_FOK(BMD_ERR_FORMAT);};
	if( handler->proxy_auth_type == BMDNET_PROXY_AUTH_USERPASS ) { BMD_FOK(BMD_ERR_FORMAT);};

	status=tcp_connect(handler,handler->proxy_adr,handler->proxy_port);
	if( status != BMD_OK )
	{
		PRINT_ERROR("Failed to connect with proxy at %s:%li\n",handler->proxy_adr,handler->proxy_port);
		BMD_FOK(BMD_ERR_NET_CONNECT);
	}

	if( handler->proxy_auth_type == BMDNET_PROXY_AUTH_NONE )
	{
		char http_headers_buf[65536];
		asprintf(&proxy_buffer,"CONNECT %s:%li HTTP/1.1\r\nHost: %s:%li\r\nUser-agent: %s\r\nProxy-Connection: %s\r\n\r\n",
				host,port,host,port,
				"viewer/1.0",
				"Keep-Alive");
		bmdnet_send(handler,(void *)proxy_buffer, (long)strlen(proxy_buffer));
		free0(proxy_buffer);

		memset(http_headers_buf,0,65536);
		status = bmdnet_recv(handler,http_headers_buf,65536);
		if (status < 0)
		{
			PRINT_ERROR("Failed to receive resonse from proxy server\n");
			BMD_FOK(BMD_ERR_NET_READ);
		}
		if(strstr(http_headers_buf,"\r\n\r\n")==NULL)
		{
			PRINT_ERROR("Bad response from proxy server\n");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		if(	(strstr(http_headers_buf,PROXY_CONNECTION_OK_1) == NULL ) &&
			(strstr(http_headers_buf,PROXY_CONNECTION_OK_2) == NULL ) )
		{
			PRINT_ERROR("Connection through proxy not granted\n");
			BMD_FOK(BMD_ERR_NOT_ALLOWED);
		}
	}
	else
	if( handler->proxy_auth_type == BMDNET_PROXY_AUTH_NTLM )
	{
		#ifdef WIN32
		char *real_buffer=NULL,*proxy_host=NULL;
		long proxy_port;
		ntlm_auth_t auth;
		asprintf(&real_buffer,"CONNECT %s:%i HTTP/1.0\r\nUser-agent: %s\r\nHost: %s:%i\r\n%s",
				 host,port,"viewer/1.0",host,port,
				 "Content-Length: 0\r\nProxy-Connection: Keep-Alive\r\nPragma: no-cache\r\n"
				);

		proxy_host=strdup(handler->proxy_adr);
		proxy_port=handler->proxy_port;
		
		BMD_FOK(ntlm_auth_init(&auth));
		BMD_FOK(bmd_sspi_init(&auth));
		BMD_FOK(bmd_sspi_acquire_method(&auth,"NTLM"));

		status=ntlm_auth_set(proxy_host,proxy_port,
							 real_buffer,
							 "Proxy-Authorization: ", /* TO MA DUZE ZNACZENIE */
							 &handler,
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
	if( handler->proxy_auth_type == BMDNET_PROXY_AUTH_BASIC )
	{
		char *userpass=NULL;
		char *tmp=NULL;
		char *buffer=NULL;
		char http_headers_buf[65536];
		if( ( handler->proxy_user == NULL) || (handler->proxy_pass == NULL ) )
		{
			PRINT_ERROR("Not enough data to perform authorization\n");
			BMD_FOK(BMD_ERR_NODATA);
		}
		/* zakodowanie nazwy do BASE64 "nazwa_usera:haslo_usera" */
		asprintf(&tmp,"%s:%s",handler->proxy_user,handler->proxy_pass);
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
		bmdnet_send(handler, buffer, (long)strlen(buffer));
		free(buffer);buffer=NULL;
		memset(http_headers_buf,0,65536);
		BMD_FOK_CHG(bmdnet_recv(handler,http_headers_buf,65536),-3);
		if(strstr(http_headers_buf,"\r\n\r\n")==NULL)
		{
			BMD_FOK(-1);
		}
		if(	(strstr(http_headers_buf,PROXY_CONNECTION_OK_1)==NULL) &&
			(strstr(http_headers_buf,PROXY_CONNECTION_OK_2)==NULL) )
		{
			BMD_FOK(-1);
		}
	}
	else
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}
	return BMD_OK;
}

long tcp_connect_socks_proxy(bmdnet_handler_ptr handler,char *host,long port)
{
	long status;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( host == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};

	status=tcp_connect(handler,handler->proxy_adr,handler->proxy_port);
	if( status != BMD_OK )
	{
		PRINT_ERROR("Failed to connect with proxy at %s:%li\n",handler->proxy_adr,handler->proxy_port);
		BMD_FOK(BMD_ERR_NET_CONNECT);
	}

	if( handler->proxy_type == BMDNET_PROXY_SOCKS4 )
	{
		socks4_request_t request_header;
		char complete_request[1024];
		char *buffer=NULL;
		long response_code			= 0;
		long length				= 0;
		long request_header_size		= 0;

		memset(complete_request,0,256);
		request_header.version=4;
		request_header.command=1;
		request_header.destport=htons((u_short)port);
		request_header.destaddr=inet_addr(host);
		request_header_size=sizeof(request_header);
		memmove(complete_request,(char *)(&request_header),request_header_size);
		if(handler->proxy_user)
		{
			length=(long)strlen((char *)handler->proxy_user);
			memmove(complete_request+request_header_size,(char *)handler->proxy_user,length);
			bmdnet_send(handler, complete_request,length+request_header_size+1);
		}
		else
		{
			bmdnet_send(handler,complete_request,request_header_size+1);
		}
		buffer=(char *)malloc(10000);
		memset(buffer,0,10000);
		bmdnet_recv(handler,buffer,1024);
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
	}
	else
	if( handler->proxy_type == BMDNET_PROXY_SOCKS4A )
	{
		socks4a_request_t request_header;
		char complete_request[1024];
		char *buffer=NULL;
		long response_code			= 0;
		long length				= 0;
		long request_header_size		= 0;

		memset(complete_request,0,256);
		request_header.version=4;
		request_header.command=1;
		request_header.destport=htons((u_short)port);
		request_header.destaddr=inet_addr("0.0.0.1");
		request_header_size=sizeof(request_header);
		memmove(complete_request,(char *)(&request_header),request_header_size);
		if(handler->proxy_user)
		{
			length=(long)strlen((char *)handler->proxy_user);
			memmove(complete_request+request_header_size,(char *)handler->proxy_user,length);
			memmove(complete_request+request_header_size+length+1,host,strlen(host));
			bmdnet_send(handler, complete_request,length+request_header_size+(long)strlen(host)+2);
		}
		else
		{
			memmove(complete_request+request_header_size+1,host,strlen(host));
			bmdnet_send(handler,complete_request,request_header_size+(long)strlen(host)+2);
		}
		buffer=(char *)malloc(10000);
		memset(buffer,0,10000);
		bmdnet_recv(handler,buffer,1024);
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
	}
	else
	if( handler->proxy_type == BMDNET_PROXY_SOCKS5 )
	{
		long i;
		long ip_flag					= 1;
		char *buffer					= NULL;
		socks5_init_request_t init_request_header;
		socks5_request_t request_header;
		char complete_request[1024];
		char initial_request[1024];
		__int32 tmp1;
		__int16 tmp2;

		/* wysylanie inicjalnego requesta z deklaracja metod */
		memset(initial_request,0,1024);init_request_header.version=0x05;init_request_header.nmethod=1;
		memmove(initial_request,(char *)(&init_request_header),sizeof(init_request_header));
		if(handler->proxy_auth_type == BMDNET_PROXY_AUTH_BASIC )
		{
			initial_request[sizeof(init_request_header)]=0x02;
		}
		BMD_FOK_CHG(bmdnet_send(handler,initial_request,sizeof(init_request_header)+1), -1);
	
		buffer=(char *)malloc(1024);
		memset(buffer,0,1024);
	
		BMD_FOK_CHG(bmdnet_recv(handler,buffer,1023),-1);
	
		if( ((unsigned char)buffer[1]) == 0xFF) /* zadna metoda autoryzacji nie jest wspierana - patrz RFC */
		{
			BMD_FOK(-1);
		}
	
		free(buffer);buffer=NULL;

		/* faza autoryzacji user/password */
		if(handler->proxy_auth_type == BMDNET_PROXY_AUTH_BASIC )
		{
			buffer=(char *)malloc(1024);
			memset(buffer,0,1024);
			buffer[0]=0x01;buffer[1]=(char)strlen((char *)handler->proxy_user);
			memmove(buffer+2,handler->proxy_user,strlen(handler->proxy_user));
			buffer[2+strlen(handler->proxy_user)]=(char)strlen((char *)handler->proxy_pass);
			memmove(buffer+3+strlen((char *)handler->proxy_user),(char *)handler->proxy_pass,
				strlen((char *)handler->proxy_pass));
			BMD_FOK_CHG(bmdnet_send(handler,buffer,(long)strlen(buffer)),-1);

			memset(buffer,0,1024);
			BMD_FOK_CHG(bmdnet_recv(handler,buffer,1023),-1);
	
			if(buffer[1]!=0x00)
			{
				BMD_FOK(-1);
			}
		}
		memset(complete_request,0,256);
		request_header.ver=0x05;request_header.cmd=1;request_header.rsv=0x00;
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
			status=bmdnet_send(handler,complete_request,sizeof(request_header)+6);
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
			BMD_FOK_CHG(bmdnet_send(handler,complete_request,sizeof(request_header)+ 
			(long)strlen(host)+3),-1);
			free(tmp);tmp=NULL;
		}
		buffer=(char *)malloc(1024);
		memset(buffer,0,1024);
		BMD_FOK_CHG(bmdnet_recv(handler,buffer,1023),-1);
	
		if(buffer[1]==0x00) /* OK*/
			return 0;
		/* jakis blad - nie interesuje nas jaki */
		return -1;
	}
	else
	{
		PRINT_ERROR("Unsupported proxy_type\n");
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	return BMD_OK;
}

long bmdnet_connect(bmdnet_handler_ptr handler, char *host, long port)
{
	long status=0;
	
	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( host == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};
	if( port == 0 ) { BMD_FOK(BMD_ERR_PARAM3);};
	
	if( ( handler->proxy_adr == NULL ) && ( handler->proxy_type == BMDNET_PROXY_NONE ) )
		status = tcp_connect(handler,host,port);
	else
	{
		switch(handler->proxy_type) {
			case BMDNET_PROXY_HTTP:
				tcp_connect_http_proxy(handler,host,port);
				break;
			case BMDNET_PROXY_SOCKS4:
			case BMDNET_PROXY_SOCKS4A:
			case BMDNET_PROXY_SOCKS5:
				tcp_connect_socks_proxy(handler,host,port);
				break;
			default:
				PRINT_DEBUG("Unknown proxy type\n");
				BMD_FOK(BMD_ERR_UNIMPLEMENTED);
		};
	}
	
	return status;
}
