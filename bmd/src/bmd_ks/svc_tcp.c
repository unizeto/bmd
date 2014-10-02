#ifndef WIN32
#define LIBBMDKS
#endif
#ifdef LIBBMDKS
#ifndef WIN32
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#endif
#include <bmd/libbmdks/bmd_ks_cli.h>
#else
#include <bmd/bmd_ks/bmd_ks.h>
#endif

#ifdef WIN32
#pragma warning(disable:4127)
#endif

long svc_tcp_sendall(SOCKET s,char *buf,long len)
{
    long total = 0;        
    long bytesleft = len;
    long n;

	if( buf == NULL )
		return SVC_ERROR_PARAM;

    while( total < len ) 
	{
        n = send(s,(const char *)buf+total,bytesleft,0);
        if( n == -1 ) 
			return SVC_ERROR_NET;
        total+=n;bytesleft-=n;
    }

    return SVC_OK;
}

long svc_tcp_recvall(SOCKET s,long timeout_sec,char **buf,long *buf_len)
{
	long nrecv,fdmax,ns;
	char recv_buf[8192];
	fd_set read_fds;
	struct timeval tv;
	long tcp_len=0,len=0;
	long status;
	
	FD_ZERO(&read_fds);
	fdmax=(int)s;

	*buf_len=0;
	if( timeout_sec == 0 )
	{
		tv.tv_sec=5;tv.tv_usec=0; /* default to czekanie 5 sekundy na dane */
	}
	else
	{
		tv.tv_sec=timeout_sec,tv.tv_usec=0;
	}

	memset(recv_buf,0,8192);
		
	FD_SET(s,&read_fds);

	/* tutaj odczyt pierwszych bajtow okreslajacych dlugosc - konkretnie 2 */
	ns=select(fdmax+1,&read_fds,NULL,NULL,&tv);
	if( ns == -1 ) /* blad */
		return SVC_ERROR_NET;
	if( ns == 0 )
		return SVC_ERROR_NET;
	if(FD_ISSET(s,&read_fds))
	{
		char buf[2];
		memset(buf,0,2);
		status=recv(s,buf,2,0);
		if( status != 2 )
			return SVC_ERROR_NET;
		tcp_len=(int)ntohs(*((short *)buf));
	}

	while(1)
	{
		ns=select(fdmax+1,&read_fds,NULL,NULL,&tv);
		if( ns == -1 ) /* blad */
			return SVC_ERROR_NET;
		if( ns == 0 ) /* socket nie byl gotowy */
			break;
		if(FD_ISSET(s,&read_fds))
		{
			nrecv=recv(s,(void *)recv_buf,8192,0);
			if( nrecv == 0 )
			{
				break ; /* nie ma wiecej danych - druga strona wyslala nam FIN */
			}
			if( nrecv == -1 )
				return SVC_ERROR_NET;
			if( nrecv > 0 )
			{
				(*buf)=(char *)realloc((*buf),len+nrecv);
				memmove((*buf)+len,recv_buf,nrecv);
				len+=nrecv;
				memset(recv_buf,0,8192);
				*buf_len=len;
				/* tutaj sprawdzenie dlugosci czy juz nie odebrane calosci - wtedy wyjscie */
				if( len == tcp_len )
				{
					break;
				}
			}
		}
	}
	return SVC_OK;
}

