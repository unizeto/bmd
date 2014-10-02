#ifndef _BMD_KS_CLI_INCLUDED_
#define _BMD_KS_CLI_INCLUDED_

#ifdef WIN32
#include <windows.h>
#endif

/*
	BMD KeyService client-side API:
	Version: 1.0 
	Author: Marek Silewicz
*/

#define SVC_MAX_CRYPTO_DATA_LENGTH 1024
#define SVC_MAX_BUF_LEN            8192

/* error codes */
#define SVC_OK				0
#define SVC_ERROR_PARAM		1
#define SVC_ERROR_DISK		2
#define SVC_ERROR_MEM		3
#define SVC_ERROR_FORMAT	4
#define SVC_ERROR_FAILED	5
#define SVC_ERROR_NODATA	6
#define SVC_ERROR_NET		7

#define SVC_REQUEST_SIGN			0	
#define SVC_RESPONSE_SIGN			1
#define SVC_REQUEST_DECRYPT			2
#define SVC_RESPONSE_DECRYPT		3
#define SVC_RESPONSE_UNKNOWN		4
#define SVC_REQUEST_GET_S_CERT		5
#define SVC_RESPONSE_GET_S_CERT		6
#define SVC_REQUEST_GET_ED_CERT		7
#define SVC_RESPONSE_GET_ED_CERT	8

#define SVC_RESPONSE_OK					0
#define SVC_RESPONSE_ERR_TYPE			1
#define SVC_RESPONSE_ERR_FORMAT			2
#define SVC_RESPONSE_ERR_UNAVAILABLE	3
#define SVC_RESPONSE_ERR_INTERNAL		4

#ifndef WIN32
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#define SOCKADDR struct sockaddr *
#ifndef SOCKET_DEFINED
#define SOCKET_DEFINED
typedef long SOCKET;
#endif
#endif

/* structure for BMD Keyservice request */
typedef struct bmd_ks_request {
        char req_type;							/* '1' - signature, '2' - decrypting */
        char data_length[4];					/* plain data length */
        char data[SVC_MAX_CRYPTO_DATA_LENGTH];	/* plain data */
} bmd_ks_request_t;

/* structure form BMD KeyService response */
typedef struct bmd_ks_response {		
        char resp_type;	
		char status;
        char orig_data_length[4];
        char data[SVC_MAX_CRYPTO_DATA_LENGTH];
        char resp_data_length[4];
        char resp_data[SVC_MAX_CRYPTO_DATA_LENGTH];
} bmd_ks_response_t;

/* creates BMD Keyservice request */
long svc_encode_request(char type,char *data,long data_length,bmd_ks_request_t **ks_req);
/* serializes BMD Keyservice request */
long svc_serialize_request(bmd_ks_request_t *req,char **buf,long *len);
/* deserialize BMD Keyservice response */
long svc_decode_response(char *buf,long len,bmd_ks_response_t **response);

/* wyslanie danych do KS */
long svc_tcp_sendall(SOCKET s,char *buf,long len);
/* odebranie danych z KS */
long svc_tcp_recvall(SOCKET s,long timeout_sec,char **buf,long *buf_len);

#endif
