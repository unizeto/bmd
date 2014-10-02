#include <bmd/bmd_ks/bmd_ks.h>

extern svc_conf_t *svc_conf;

/* wolane przez serwer do odczytu danych z gniazda */
long svc_net_read(SOCKET s,long max_length,bmd_ks_request_t **ks_req,char **dbuf,long *dlen)
{
	char *buf=NULL;
	long status;
	long length;

	if( ks_req == NULL )
		return SVC_ERROR_PARAM;
	if( ( max_length == 0 ) || ( max_length > SVC_MAX_BUF_LEN ) )
		return SVC_ERROR_PARAM;
	
	status=svc_tcp_recv(s,&buf,&length);
	if( status != SVC_OK )
		return SVC_ERROR_NET;

	if( svc_conf->log_level == SVC_LOG_LEVEL_DEBUG )
	{
		if( dbuf != NULL )
		{
			(*dbuf)=(char *)malloc(length);
			if( (*dbuf) == NULL )
				return SVC_ERROR_MEM;
			memmove(*dbuf,buf,length);
			if( dlen != NULL )
				*dlen=length;
		}
	}
	status=svc_decode_request(buf,length,ks_req);
	free0(buf);
	if( status != SVC_OK )
		return SVC_ERROR_FORMAT;

    return SVC_OK;
}

/* wolane przez serwera do zapisu danych do gniazda */
long svc_net_write(SOCKET s,bmd_ks_response_t *ks_resp)
{
	long status;
	char *buf=NULL;
	long len;

	if( ks_resp == NULL )
		return SVC_ERROR_PARAM;

	status=svc_serialize_response(ks_resp,&buf,&len);
	if( status != SVC_OK )
		return SVC_ERROR_FORMAT;

	status=svc_tcp_send(s,(char *)buf, len);
	free0(buf);
	if( status != SVC_OK )
		return SVC_ERROR_NET;

    return SVC_OK;    
}

long svc_net_initialize(char **error)
{
#ifdef WIN32
	WORD wVersionRequested;
    WSADATA wsaData;
	long status;

	wVersionRequested=MAKEWORD( 2, 2 );
    status=WSAStartup(wVersionRequested,&wsaData);
    if ( status != 0 ) 
    {
		svc_gen_err_desc(error,"Failed to start winsock with 2.2 version");
        svc_reportstatus(SERVICE_STOPPED,1,0,0);
        return SVC_ERROR_FAILED;
    }
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
    {
		svc_gen_err_desc(error,"Failed to start winsock with 2.2 version");
        svc_reportstatus(SERVICE_STOPPED,1,0,0);
        WSACleanup( );
        return SVC_ERROR_FAILED;
    }
#endif
	return SVC_OK;
}

long svc_net_create_listen_socket(SOCKET *s,const char *adr,const long port,const long lq,char **error)
{
	long status;
	struct sockaddr_in info;
	
	if( s == NULL )
		return SVC_ERROR_PARAM;
	if( adr == NULL )
		return SVC_ERROR_PARAM;
	
	*s=socket(AF_INET,SOCK_STREAM,0);
	if( (*s) == INVALID_SOCKET ) 
	{
		svc_gen_err_desc(error,"Cannot create socket of AF_INET, SOCK_STREAM");
#ifdef WIN32
		svc_reportstatus(SERVICE_STOPPED,1,0,0);
		WSACleanup();
#endif
		return SVC_ERROR_FAILED;
	}
	
	info.sin_family = AF_INET;
	info.sin_addr.s_addr = inet_addr(adr);
	info.sin_port = htons((u_short)port);

#ifdef WIN32
	status=bind(*s,(SOCKADDR*)&info,sizeof(info));
#else
	status=bind(*s,(const struct sockaddr *)&info,sizeof(info));
#endif
	if( status == SOCKET_ERROR )
	{
		svc_gen_err_desc(error,"Failed to bind port %i at address %s",port,adr);
#ifdef WIN32
		svc_reportstatus(SERVICE_STOPPED,1,0,0);
		WSACleanup();
#endif
		return SVC_ERROR_FAILED;
	}

	status=listen(*s,lq);
	if( status == SOCKET_ERROR )
	{
		svc_gen_err_desc(error,"Failed to set socket in listen state");
#ifdef WIN32
		svc_reportstatus(SERVICE_STOPPED,1,0,0);
		WSACleanup();
#endif
		return SVC_ERROR_FAILED;
	}

	return SVC_OK;
}

long svc_tcp_send(SOCKET s,char *data,long data_length)
{
	long status;

	if( data == NULL )
		return SVC_ERROR_PARAM;

	status=svc_tcp_sendall(s,data,data_length);
	if( status != SVC_OK )
		return SVC_ERROR_NET;

	return SVC_OK;
}

long svc_tcp_recv(SOCKET s,char **data,long *data_length)
{
	long status;

	if( data == NULL )
		return SVC_ERROR_PARAM;
	if( (*data) != NULL )
		return SVC_ERROR_PARAM;
	if( data_length == NULL )
		return SVC_ERROR_PARAM;

	status=svc_tcp_recvall(s,svc_conf->timeout_sec,data,data_length);
	if( status != SVC_OK )
		return SVC_ERROR_NET;

	return SVC_OK;
}
