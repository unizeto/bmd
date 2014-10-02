#ifndef _LIBBMDNET_H
#define _LIBBMDNET_H
    
#ifdef WIN32
	#ifdef LIBBMDNET_EXPORTS
		#define LIBBMDNET_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDNET_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDNET_SCOPE
		#endif
	#endif
#else
	#define LIBBMDNET_SCOPE
#endif /* ifdef WIN32*/

#ifdef _WIN32
#define MSG_DONTWAIT 1
#if !(defined(_WINSOCK2API_) || defined(_WINSOCKAPI_))
#include <winsock2.h>
#endif /*winsock2.h*/
#endif

#define BMDNET_PROXY_NONE		0
#define BMDNET_PROXY_HTTP		1
#define BMDNET_PROXY_SOCKS4		2
#define BMDNET_PROXY_SOCKS4A	3
#define BMDNET_PROXY_SOCKS5		4

#define BMDNET_PROXY_AUTH_NONE		0
#define BMDNET_PROXY_AUTH_USERPASS	1
#define BMDNET_PROXY_AUTH_BASIC		2
#define BMDNET_PROXY_AUTH_NTLM		3

#ifndef _REFRESH_CALLBACK
	#define _REFRESH_CALLBACK
	typedef long (*bmdnet_progress_callback_t) (long total_bytes,long done_bytes);
#endif

typedef struct __bmdnet_handler_struct {
#ifdef WIN32
	SOCKET s;
#else
	long s;
#endif
	/* parametry serwera proxy */
	char *proxy_adr;
	long proxy_port;
	long proxy_type;
	long proxy_auth_type;
	char *proxy_user;
	char *proxy_pass;

	/* wartosci poszczegolnych timeout'ow */
	long connect_timeout;
	long read_timeout;
	long write_timeout;

	/* adres i port klienta */
	char* stringClientHost;
	char* stringClientPort;

	/* callback do odswiezania GUI */
	bmdnet_progress_callback_t pcallback;
} *bmdnet_handler_ptr;

#if defined(__cplusplus)
extern "C" {
#endif
     
LIBBMDNET_SCOPE long bmdnet_init();
LIBBMDNET_SCOPE long bmdnet_destroy();
LIBBMDNET_SCOPE long bmdnet_create(bmdnet_handler_ptr * handler);
LIBBMDNET_SCOPE long bmdnet_close(bmdnet_handler_ptr * handler);
LIBBMDNET_SCOPE long bmdnet_connect(bmdnet_handler_ptr handler, char *host,long port);
LIBBMDNET_SCOPE long bmdnet_send(bmdnet_handler_ptr handler,void *const buffer, long buffer_len);
LIBBMDNET_SCOPE long bmdnet_recv(bmdnet_handler_ptr handler, void *buffer,long buffer_size);
LIBBMDNET_SCOPE long bmdnet_recv_min(bmdnet_handler_ptr handler,void *buffer, long min_size,long max_size);
LIBBMDNET_SCOPE long bmdnet_server(bmdnet_handler_ptr handler,
								   char *net_interface, long port,long maxconn,char *serverCertFilename,
								   void *(*server_func) (bmdnet_handler_ptr handler));
LIBBMDNET_SCOPE long bmdnet_logserver(bmdnet_handler_ptr handler,
								   char *net_interface, long port,long maxconn,char *serverCertFilename,
								   void *(*server_func) (bmdnet_handler_ptr handler));
LIBBMDNET_SCOPE long bmdnet_set_timeouts(long connect_timeout,long read_timeout,long write_timeout,
										 bmdnet_handler_ptr handler);
LIBBMDNET_SCOPE long bmdnet_set_connection_params(const char *proxy_adr,long proxy_port,long proxy_type,
								  long proxy_auth_type,const char *user,const char *pass,
								  bmdnet_handler_ptr handler);
#if defined(__cplusplus)
}
#endif
#endif	/* _LIBBMDNET_H */
    
