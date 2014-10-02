/** 
@file
Mechanism is not used and not supported. 
You must implement new style of handler->data[] array.

*/

/***************************************************************************
 *            mech_null.c
 *  NULL connection method
 *  Fri Mar  3 13:10:02 2006
 *  Copyright  2006  Lukasz Klimek, Unizeto S.A.
 *  lklimek@certum.pl
 ****************************************************************************/

#include <bmd/common/bmd-common.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <assert.h>

#define SOCKET_DATA 0

/*
#error openssl_bio is not supported
*/
long __bmdnet_openssl_bio_connect(bmdnet_handler_ptr handler,char*host, long port)
{
	char * uri;
	long status;
	BIO * conn = NULL;

	assert ( host != NULL && port > 0 );
	
	PRINT_VDEBUG("OpenSSL: connecting to server %s:%i\n",host,port);


	//asprintf(&uri, "%s:%u",host,port);
	/** @bug: na stala dlugosc sie alokuje - nie powinno byc bledu, ale ...*/
	uri=(char *)malloc(strlen(host)+12);
	memset(uri,0,strlen(host)+11);
	sprintf(uri,"%s:%u",host,port);
	
	conn = BIO_new_connect(uri);
	free (uri);
	if (conn == NULL) {
		PRINT_DEBUG("BIO_new_connect returned NULL\n");
		return -1;
	}
	/** @bug BIO_do_connect returns 1 even if domain name does not exist! */
	status = BIO_do_connect(conn);  
	if (status != 1) {
		PRINT_DEBUG ("Status = %i\n",status);
		return status;	
	}
	PRINT_VDEBUG("Connection succeeded. conn = %p\n",conn);
	handler->data[0] = conn;
	
	return 0;
};

long __bmdnet_openssl_bio_send(bmdnet_handler_ptr handler, void * buffer, long buffer_len)
{
	long nwritten = 0, written_now = 0;
	_bmdnet_socketblock(BIO_get_fd ((BIO*)handler->data[0], NULL),1);
	
	PRINT_VDEBUG("OpenSSL send(%p,%p (%s), %i\n",handler, buffer, (char*)buffer, 
		buffer_len);
	
	do {
		written_now = BIO_write ( (BIO*) handler->data[0], 
			((char*)buffer)+nwritten, buffer_len - nwritten );
		
		if (written_now <= 0 
			&& ! BIO_should_retry((BIO*) handler->data[0])) {
				return nwritten;
		} else if (written_now <= 0) {
			written_now = 0;
		}
		
		nwritten += written_now;
	} while (nwritten < buffer_len);
	
	return nwritten;
};

long __bmdnet_openssl_bio_recv(bmdnet_handler_ptr handler, void * buffer,long buffer_size, 
	long blocking)
{
	long nread = 0;

	_bmdnet_socketblock(BIO_get_fd ((BIO*)handler->data[0], NULL),blocking);

	PRINT_VDEBUG("OpenSSL recv(%p, %p=(%s), %i)\n",handler, buffer, (char*) buffer, 
		buffer_size);

	nread = BIO_read( (BIO*)handler->data[0], buffer, buffer_size);

	if (nread < 1) {
		if (BIO_should_retry((BIO*)handler->data[0]) ) {
			return 0;
		} else {
			return -1;
		};
	}

	return nread;
};

long __bmdnet_openssl_bio_bind (bmdnet_handler_ptr handler, char * net_interface, 
	long port, long maxconn, char * serverCertFilename)
{
	char * uri;
	assert ( handler != NULL && port > 0 && maxconn > 0 );
	
	PRINT_VDEBUG("OpenSSL: bind (%p, %p (%s), %u, %i, %p (%s))\n", handler, 
		net_interface, net_interface, port, maxconn, serverCertFilename, 
		serverCertFilename);
	
	/** @bug - tak jak wyzej... */
	uri=(char *)malloc(strlen(net_interface)+12);
	memset(uri,0,strlen(net_interface)+11);
	sprintf(uri,"%s:%u",net_interface,port);

	if (uri == NULL) {
#ifndef _WIN32
		PRINT_DEBUG ("%s:%i: error in %s: uri == NULL after call to asprintf()\n",
			__FILE__, __LINE__, __FUNCTION__);
#endif
		return -1;	
	}
#ifndef _WIN32
	PRINT_VDEBUG("%s: uri=%s\n",__FUNCTION__, uri);
#endif

	handler->data[SOCKET_DATA] = BIO_new_accept(uri);
	free(uri);

	if (handler->data[SOCKET_DATA] == NULL) {
		PRINT_DEBUG ("Bind failed: %s\n", ERR_error_string(ERR_get_error(),NULL));
		return -1;
	}
	
	/* First call to BIO_accept() sets up accept BIO, eg. binds to port */
	if (BIO_do_accept(handler->data[SOCKET_DATA]) <= 0) {
#ifndef _WIN32
		PRINT_DEBUG("%s(): BIO_do_accept() failed \n",__FUNCTION__);
#endif
		return -1;
	}
	
#ifndef _WIN32
	PRINT_VDEBUG("%s returns 0\n",__FUNCTION__);
#endif

	return 0;
}

bmdnet_handler_ptr __bmdnet_openssl_bio_accept (bmdnet_handler_ptr handler)
{
	bmdnet_handler_ptr hdl = NULL;
	assert ( handler != NULL );
	assert ( handler->data != NULL );
	assert ( handler->data[0] != NULL );
	
	if ( handler == NULL || handler->data[SOCKET_DATA] == NULL ) {
		return NULL;
	}
	
	if (BIO_do_accept(handler->data[SOCKET_DATA]) <= 0) {
		return NULL;
	}
	
	hdl = (bmdnet_handler_ptr) malloc (sizeof (struct __bmdnet_handler_struct));
	
	if (hdl == NULL) {
		PRINT_DEBUG("NULL handler \n");
		return NULL;
	}
	
	memcpy (hdl, handler, sizeof (struct __bmdnet_handler_struct));
	
	hdl->data = (void**) malloc( sizeof(void*) *1);
	
	hdl->data[SOCKET_DATA] = BIO_pop(handler->data[SOCKET_DATA]);
	if (hdl->data[SOCKET_DATA] == NULL) {
		free(hdl);
		return NULL;
	}
	return hdl;
}

long __bmdnet_openssl_bio_init()
{
	long status=0;

	PRINT_VDEBUG("OpenSSL initialization\n");
	status = SSL_library_init();
	
	if (status) {
		return status; 
	}
	
	SSL_load_error_strings(); 
	
	return 0;
}


long __bmdnet_openssl_bio_destroy()
{
	ERR_remove_state(0);
	ERR_free_strings();
	EVP_cleanup();
	PRINT_VDEBUG("OpenSSL: destroy\n");
	return -1;
}

long __bmdnet_openssl_bio_close(bmdnet_handler_ptr handler)
{
	/** 
	    @bug Nie można tak zamknąć połączenia w serwerze, tzn.
	    w serwerze po accept zawsze do dziecka wysyłamy handler 
	    a w rodzicu robimy close. Niestety to nie działa w 
	    OpenSSL.
	*/
	BIO_free(handler->data[0]);
	free(handler->data);
	PRINT_VDEBUG("OpenSSL close(%p)\n",handler);
	return 0;
}


long 	__bmdnet_openssl_bio_create(bmdnet_handler_ptr handler)
{
	bmd_assert(handler->data == NULL, "LK");
	handler->data = (void**) malloc(sizeof(void*)*1);
	handler->data[SOCKET_DATA] = (void*) malloc(sizeof(void*));
	return 0;
}

#ifndef WIN32
const bmdnet_method_t __bmdnet_method_OPENSSL_BIO = {
	BMD_NET_OPENSSL_BIO_METHOD,
	
	__bmdnet_openssl_bio_init,
	__bmdnet_openssl_bio_destroy,
	
	__bmdnet_openssl_bio_create,
	__bmdnet_openssl_bio_connect,
	__bmdnet_openssl_bio_close,
	
	__bmdnet_openssl_bio_send, 
	__bmdnet_openssl_bio_recv,
	
	__bmdnet_openssl_bio_bind, 
	__bmdnet_openssl_bio_accept
};

#else

bmdnet_method_t __bmdnet_method_OPENSSL_BIO;
void init_bmdnet_method_OPENSSL_BIO()
{
	bmdnet_method_t *tmp_method=NULL;
	tmp_method=(bmdnet_method_t *)malloc(sizeof(bmdnet_method_t));

	tmp_method->id=BMD_NET_OPENSSL_BIO_METHOD;
	tmp_method->init=__bmdnet_openssl_bio_init;
	tmp_method->destroy=__bmdnet_openssl_bio_destroy;
	
	tmp_method->create=__bmdnet_openssl_bio_create;
	tmp_method->connect=__bmdnet_openssl_bio_connect;
	tmp_method->close=__bmdnet_openssl_bio_close;
	tmp_method->send=__bmdnet_openssl_bio_send;
	tmp_method->recv=__bmdnet_openssl_bio_recv;
	tmp_method->bind=__bmdnet_openssl_bio_bind;
	tmp_method->accept=__bmdnet_openssl_bio_accept;

	memmove(&__bmdnet_method_OPENSSL_BIO,tmp_method,sizeof(bmdnet_method_t));
}

#endif
