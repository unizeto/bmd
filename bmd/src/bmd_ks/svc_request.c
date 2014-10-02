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

long svc_encode_request(char type,char *data,long data_length,bmd_ks_request_t **ks_req)
{
	if( ( type != SVC_REQUEST_SIGN ) && ( type != SVC_REQUEST_DECRYPT) && 
		( type != SVC_REQUEST_GET_S_CERT ) && ( type != SVC_REQUEST_GET_ED_CERT ) )
		return SVC_ERROR_PARAM;
	if( ( data == NULL ) && ( type != SVC_REQUEST_GET_S_CERT ) && ( type != SVC_REQUEST_GET_ED_CERT ) )
		return SVC_ERROR_PARAM;
	if( ( ( data_length == 0 ) || ( data_length > SVC_MAX_CRYPTO_DATA_LENGTH ) ) && 
		( ( type != SVC_REQUEST_GET_S_CERT ) && ( type != SVC_REQUEST_GET_ED_CERT ) ) )
		return SVC_ERROR_PARAM;
	if( ks_req == NULL )
		return SVC_ERROR_PARAM;
	if( (*ks_req) != NULL )
		return SVC_ERROR_PARAM;

	/* @todo - 64bit */
	if( sizeof(int) > 4 ) /* zalozenie ze rozmiar jest zawsze 4 */
		return SVC_ERROR_FORMAT; 

	(*ks_req)=(bmd_ks_request_t *)malloc(sizeof(bmd_ks_request_t));
	if( (*ks_req) == NULL )
		return SVC_ERROR_MEM;
	memset(*ks_req,0,sizeof(bmd_ks_request_t));
	
	(*ks_req)->req_type=type;
	if( ( type == SVC_REQUEST_GET_S_CERT ) || ( type == SVC_REQUEST_GET_ED_CERT ) )
	{
		/* dummy data */
		long tmp=1;
		memmove((*ks_req)->data_length,&tmp,sizeof(int));
		memmove((*ks_req)->data,"A",1);	
	}
	else
	{
		memmove((*ks_req)->data_length,&data_length,sizeof(int));
		memmove((*ks_req)->data,data,data_length);
	}
	return SVC_OK;
}

long svc_serialize_request(bmd_ks_request_t *req,char **buf,long *len)
{
	long nlen;
	long req_len;
	long out_len;
	short tmp;

	if( req == NULL )
		return SVC_ERROR_PARAM;
	if( buf == NULL )
		return SVC_ERROR_PARAM;
	if( (*buf) != NULL )
		return SVC_ERROR_PARAM;
	if( len == NULL )
		return SVC_ERROR_PARAM;

	/* @todo - 64bit */
	if( sizeof(int) > 4 )
		return SVC_ERROR_FORMAT;

	req_len=*(long *)req->data_length;

	out_len=2+1+4+req_len; /* dlugosc danych na 2 bajtach, bajt typu, bajty dlugosci i dlugosc */

	(*buf)=(char *)malloc(out_len+2);
	if( (*buf) == NULL )
		return SVC_ERROR_MEM;
	memset(*buf,0,out_len+1);

	/* dlugosc danych na pierwszych dwoch bajtach */
	tmp=htons((short)(1+4+req_len));
	memmove(*buf,&tmp,sizeof(short));
	/* typ */
	(*buf)[2]=req->req_type;
	/* dlugosc danych i dane */
	nlen=htonl(req_len);
	memmove((*buf)+3,&nlen,sizeof(int));
	memmove((*buf)+7,req->data,req_len);
	*len=out_len;

	return SVC_OK;
}

/* dane z gniazda, ale bez pierwszych dwohc bajtow okreslajacych dlugosc */
long svc_decode_request(char *buf,long len,bmd_ks_request_t **req)
{
	long tmp;

	if( buf == NULL )
		return SVC_ERROR_PARAM;
	if( len > SVC_MAX_BUF_LEN )
		return SVC_ERROR_PARAM;
	if( req == NULL )
		return SVC_ERROR_PARAM;
	if( (*req) != NULL )
		return SVC_ERROR_PARAM;

	if( len <= 5 ) /* bajt na typ + 4 bajty na dlugosc ale brak danych - minimalna poprawna dlugosc to 6 */
		return SVC_ERROR_FORMAT;

	if( sizeof(int) > 4 ) /* zalozenie ze rozmiar jest zawsze 4 */
		return SVC_ERROR_FORMAT;  

	(*req)=(bmd_ks_request_t *)malloc(sizeof(bmd_ks_request_t));
	if( (*req) == NULL )
		return SVC_ERROR_MEM;
	memset(*req,0,sizeof(bmd_ks_request_t));
	
	/* typ requesta */
	(*req)->req_type=buf[0];
	/* dlugosc danych */
	tmp=ntohl(*(long *)(((unsigned char *)buf)+1));
	memmove((*req)->data_length,&tmp,sizeof(int));
	/* dane */
	memmove((*req)->data,(unsigned char *)buf+5,tmp);

    return SVC_OK;
}
