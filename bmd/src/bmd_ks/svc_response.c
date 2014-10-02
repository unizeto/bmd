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

long svc_encode_response(char resp_type,char status,char *od,long odl,
                        char *cd,long cdl,bmd_ks_response_t **response)
{
	if( ( resp_type != SVC_RESPONSE_SIGN ) && ( resp_type != SVC_RESPONSE_DECRYPT) && 
		( resp_type != SVC_RESPONSE_UNKNOWN )
	  )
		return SVC_ERROR_PARAM;
	/*if( od == NULL )
		return SVC_ERROR_PARAM;
	if( ( odl == 0 ) || ( odl > SVC_MAX_CRYPTO_DATA_LENGTH ) )
		return SVC_ERROR_PARAM;
	if( ( cdl == 0 ) || ( cdl > SVC_MAX_CRYPTO_DATA_LENGTH ) )
		return SVC_ERROR_PARAM;
	if( cd == NULL )
		return SVC_ERROR_PARAM;*/
	if( odl > SVC_MAX_CRYPTO_DATA_LENGTH )
		return SVC_ERROR_PARAM;
	if( cdl > SVC_MAX_CRYPTO_DATA_LENGTH )
		return SVC_ERROR_PARAM;
	if( response == NULL )
		return SVC_ERROR_PARAM;
	if( (*response) != NULL )
		return SVC_ERROR_PARAM;

	if( sizeof(int) > 4 ) /* zalozenie ze rozmiar jest zawsze 4 */
		return SVC_ERROR_FORMAT; 

	(*response)=(bmd_ks_response_t *)malloc(sizeof(bmd_ks_response_t));
	if( (*response) == NULL )
		return SVC_ERROR_MEM;
	memset(*response,0,sizeof(bmd_ks_response_t));

	/* typ odpowiedzi i jej status */
	(*response)->resp_type=resp_type;
	(*response)->status=status;
	/* dlugosci danych oryginalnych i sieciowych */
	if( odl > 0 )
	{
		memmove((*response)->orig_data_length,&odl,sizeof(int));
	}
	else
	{
		long tmp;
		tmp=1;
		memmove((*response)->orig_data_length,&tmp,sizeof(int));
	}
	if( cdl > 0 )
	{
		memmove((*response)->resp_data_length,&cdl,sizeof(int));
	}
	else
	{
		long tmp;
		tmp=1;
		memmove((*response)->orig_data_length,&tmp,sizeof(int));
	}
	/* dane */
	if( od != NULL )
		memmove((*response)->data,od,odl);
	else
		memmove((*response)->data,"0",1);
	if( cd != NULL )
		memmove((*response)->resp_data,cd,cdl);
	else
		memmove((*response)->resp_data,"0",1);

    return SVC_OK;
}

long svc_serialize_response(bmd_ks_response_t *resp,char **buf,long *len)
{
	/* zalozenie ze kod odpowiedzi bedzie realizowany przez serwer - wiec nie uodparniam sie na zlosliwosc */
	long nlen;
	long rodl,rcdl;
	long out_len;
	short tmp;

	if( resp == NULL )
		return SVC_ERROR_PARAM;
	if( buf == NULL )
		return SVC_ERROR_PARAM;
	if( (*buf) != NULL )
		return SVC_ERROR_PARAM;
	if( len == NULL )
		return SVC_ERROR_PARAM;

	if( sizeof(int) > 4 )
		return SVC_ERROR_FORMAT;

	rodl=*(long *)resp->orig_data_length;
	rcdl=*(long *)resp->resp_data_length;

	out_len=2+1+1+4+rodl+4+rcdl; /* calk_dlugosc+typ+status+dlugosc+dane+dlugosc+dane */
	(*buf)=(char *)malloc(out_len+2);
	if( (*buf) == NULL )
		return SVC_ERROR_PARAM;
	memset(*buf,0,out_len+1);

	/* dlugosc danych */
	tmp=htons((short)(1+1+4+rodl+4+rcdl));
	memmove(*buf,&tmp,sizeof(short));
	/* typ i status */
	(*buf)[2]=resp->resp_type;
	(*buf)[3]=resp->status;
	/* dane oryginalne */
	nlen=htonl(rodl);
	memmove((*buf)+4,&nlen,sizeof(int));
	memmove((*buf)+8,resp->data,rodl);
	/* crypto dane */
	nlen=htonl(rcdl);
	memmove((*buf)+8+rodl,&nlen,sizeof(int));
	memmove((*buf)+8+rodl+4,resp->resp_data,rcdl);

	*len=out_len;

	return SVC_OK;
}

/* dane z gniazda ale bez pierwszych dwoch bajtow okreslajacych dlugosc */
long svc_decode_response(char *buf,long len,bmd_ks_response_t **response)
{
	long hodl,hcdl;

	if( buf == NULL )
		return SVC_ERROR_PARAM;
	if( ( len == 0 ) || ( len > SVC_MAX_BUF_LEN ) )
		return SVC_ERROR_PARAM;
	if( response == NULL )
		return SVC_ERROR_PARAM;
	if( (*response) != NULL )
		return SVC_ERROR_PARAM;

	/* @todo - 64bit */
	if( sizeof(int) > 4 ) /* zalozenie ze rozmiar jest zawsze 4 */
		return SVC_ERROR_FORMAT; 

	if( len <= 11 ) /* typ, status, dlugosc danych, jeden bajt danych, dlugosc, jeden bajt - 12 to min. poprawna dlugosc */
		return SVC_ERROR_FORMAT;
	(*response)=(bmd_ks_response_t *)malloc(sizeof(bmd_ks_response_t));
	if( (*response) == NULL )
		return SVC_ERROR_MEM;
	memset(*response,0,sizeof(bmd_ks_response_t));

	/* typ i status odpowiedzi */
	(*response)->resp_type=buf[0];
	(*response)->status=buf[1];
	/* dlugosc i oryginalne dane */
	hodl=ntohl(*(long *)(buf+2));
	if( ( hodl + 2 ) > len ) /* ktos jest zlosliwy ;-) */
		return SVC_ERROR_FORMAT;
	memmove((*response)->orig_data_length,&hodl,sizeof(int));
	if( ( hodl + 6 ) > len ) /* ktos jest zlosliwy ;-) */
		return SVC_ERROR_FORMAT;
	memmove((*response)->data,buf+6,hodl);
	/* dlugosc i crypto dane */
	hcdl=ntohl(*(long *)(buf+6+hodl));
	if( ( hcdl + 6 + hodl ) > len )
		return SVC_ERROR_FORMAT;
	memmove((*response)->resp_data_length,&hcdl,sizeof(int));
	memmove((*response)->resp_data,buf+6+hodl+4,hcdl);

	return SVC_OK;
}
