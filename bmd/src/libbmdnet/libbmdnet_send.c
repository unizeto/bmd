/* wysylanie danych */
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmderr/libbmderr.h>

long bmdnet_send(bmdnet_handler_ptr handler,void * const buffer,long buffer_len)
{
	long total=0,bytesleft=buffer_len,n;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( buffer == NULL ) { BMD_FOK(BMD_ERR_PARAM2);};

	if (handler->pcallback)
		handler->pcallback(buffer_len,0);
    while( total < buffer_len ) 
	{
        n = send(handler->s,(const char *)buffer+total,bytesleft,0);
        if( n == -1 ) { BMD_FOK(BMD_ERR_NET_WRITE); };
        total+=n;bytesleft-=n;
		if (handler->pcallback) 
			handler->pcallback(buffer_len,total);
    }
	return total;
}
