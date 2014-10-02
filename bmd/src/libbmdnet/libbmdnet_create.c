/* utworzenie struktury handlera oraz jest usuniecie */
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmderr/libbmderr.h>

#ifndef WIN32
#define closesocket close
#endif

long bmdnet_create(bmdnet_handler_ptr *handler)
{
	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};

	(*handler)=(bmdnet_handler_ptr)malloc(sizeof(struct __bmdnet_handler_struct));
	if( (*handler) == NULL )
		return BMD_ERR_MEMORY;
	memset( *handler, 0, sizeof(struct __bmdnet_handler_struct) );
	
	return 0;
}

long bmdnet_close(bmdnet_handler_ptr * handler)
{
	long status;

	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};
	if( (*handler) == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};

	status=closesocket((*handler)->s);

	free0((*handler)->stringClientHost);
	free0((*handler)->stringClientPort);

	free0 (*handler);

	return BMD_OK;
}
