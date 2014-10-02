/* odbierania danych */

#ifdef WIN32
#pragma warning(disable:4127)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmderr/libbmderr.h>

#define SINGLE_WAIT	200

long bmd_tcp_recvall(long s,long timeout_sec,char *buf,long buf_size,bmdnet_progress_callback_t pcb)
{
	long nrecv,fdmax,ns=0,count=0,i=0;
	fd_set read_fds;
	struct timeval tv;

	/* oczekiwanie w petli na gotowosc gniazda - wraz z wywolaniem callbacka */
	if( timeout_sec == 0 )
		count=5000/200;
	else
		count=(timeout_sec*1000)/200;

	/* count iteracja kazda po SINGLE_WAIT mili sekund */
	for(i=0;i<count;i++)
	{
		FD_ZERO(&read_fds);
		fdmax=(int)s;
		FD_SET((unsigned long)s,&read_fds);
		memset(&tv,0,sizeof(struct timeval));
		tv.tv_usec=SINGLE_WAIT*1000;
		ns=select(fdmax+1,&read_fds,NULL,NULL,&tv);
		if( ns == -1 )
		{
			BMD_FOK(BMD_ERR_NET_READ);
		}
		else
			if( ns == 0 )
			{
				if( pcb )
					pcb(buf_size,0);
				continue;
			}
			else
				break;
	}
	if( ns == 0 )
	{
		BMD_FOK(BMD_ERR_NET_READ);
	}
	
	/* tutaj juz na pewno da sie cos z gniazda odebrac */
	if(FD_ISSET(s,&read_fds))
	{
		nrecv=recv(s,(void *)buf,buf_size,0);
		if( nrecv == 0 )
			return 0;
		else
			if( nrecv == -1 )
			{
				BMD_FOK(BMD_ERR_NET_READ);
			}
		return nrecv;
	}
	else
	{
		BMD_FOK(BMD_ERR_NET_READ);
	}

	return BMD_OK;
}

long bmdnet_recv(bmdnet_handler_ptr handler,void * buffer,long buffer_size)
{
	long status;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( buffer == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};
	if (buffer_size == 0)
		return 0;

	status=bmd_tcp_recvall((long)handler->s,360,buffer,buffer_size,handler->pcallback);

	return status;
}

long bmdnet_recv_min(bmdnet_handler_ptr handler,void *buffer,long min_size,long max_size)
{
	long i=0,nrecv=0;
	char *ptr=NULL;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( buffer == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};

	ptr=(char *)buffer;
	i=0;

	while ( i < min_size )
	{
		nrecv = bmdnet_recv(handler, ptr,max_size-i);
		if (nrecv < 1)
		{
			PRINT_ERROR("LIBBMDNETERR Not all data in buffer taken care of\n");
			return BMD_ERR_NET_READ;
		}
		i+=nrecv;
		ptr+=nrecv;
	}

	return i;
}
