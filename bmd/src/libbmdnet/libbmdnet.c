#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdnet/libbmdnet.h>

#ifndef _WIN32
#include <bmd/common/global_types.h>
#else
#include <windows.h>
#include <winsock2.h>
#pragma warning(disable:4100)
#endif

#include <assert.h>
#include <stdlib.h>

long bmdnet_set_progress_callback( bmdnet_handler_ptr *handler, bmdnet_progress_callback_t pcallback )
{
	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1); };
	if( (*handler) == NULL ) { BMD_FOK(BMD_ERR_PARAM1); }

	(*handler)->pcallback=pcallback;

	return BMD_OK;
}

long bmdnet_set_timeouts(long connect_timeout,long read_timeout,long write_timeout,
						 bmdnet_handler_ptr handler)
{
	if( handler == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};

	handler->connect_timeout=connect_timeout;
	handler->read_timeout=read_timeout;
	handler->write_timeout=write_timeout;

	return BMD_OK;
}

long bmdnet_set_connection_params(const char *proxy_adr,long proxy_port,long proxy_type,
								  long proxy_auth_type,const char *user,const char *pass,
								  bmdnet_handler_ptr handler)
{
	if( proxy_adr == NULL ) { BMD_FOK(BMD_ERR_PARAM1);};

	asprintf(&(handler->proxy_adr),"%s",proxy_adr);
	handler->proxy_port=proxy_port;
	handler->proxy_type=proxy_type;
	handler->proxy_auth_type=proxy_auth_type;
	if( user != NULL )
		asprintf(&(handler->proxy_user),"%s",user);
	if( pass != NULL )
		asprintf(&(handler->proxy_pass),"%s",pass);

	return BMD_OK;
}
