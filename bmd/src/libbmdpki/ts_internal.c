#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
//#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdasn1_common/PKIStatusInfo.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

//by WSZ
#ifndef WIN32
#include <sys/file.h> /*dla widocznosci funkcji flock()*/
#endif
//by WSZ

#ifndef TRUE
#define TRUE 1
#endif

long __bmd_decode_timestampresp(	GenBuf_t *input,
						GenBuf_t **buf)
{
asn_dec_rval_t rval;
TimeStampResp_t *timestamp_resp=NULL;
PKIStatusInfo_t *pki_status_info=NULL;
PKIStatus_t pki_status;
long pki_status_as_long;
long status;

	if(input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*buf)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	rval = ber_decode(0, &asn_DEF_TimeStampResp,  (void **)&timestamp_resp, input->buf, input->size);
	if(rval.code!=RC_OK)		{	BMD_FOK(BMD_ERR_FORMAT);	}

	if( timestamp_resp == NULL )	{	BMD_FOK(BMD_ERR_FORMAT);	}

	pki_status_info=&(timestamp_resp->status);
	if( pki_status_info == NULL )	{	BMD_FOK(BMD_ERR_FORMAT);	}

	pki_status=pki_status_info->status;
	status=asn_INTEGER2long(&pki_status,&pki_status_as_long);
	if( status != 0 )			{	BMD_FOK(BMD_ERR_FORMAT);	}

	switch(pki_status_as_long)
	{
		case PKIStatus_granted: break;
		case PKIStatus_grantedWithMods: break;
		case PKIStatus_rejection: return BMD_ERR_OP_FAILED;
		case PKIStatus_waiting: return BMD_ERR_OP_FAILED;
		case PKIStatus_revocationWarning:  return BMD_ERR_OP_FAILED;
		case PKIStatus_revocationNotification: return BMD_ERR_OP_FAILED;
	};

	if( ( pki_status_as_long == PKIStatus_granted ) || ( pki_status_as_long == PKIStatus_grantedWithMods ) )
	{
		BMD_FOK(asn1_encode(&asn_DEF_TimeStampToken,(void *)timestamp_resp->timeStampToken, NULL, buf));
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp,timestamp_resp,0);
		timestamp_resp=NULL;
	}
	else	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	return 0;
}


/**----------------------------------------------------------------------------------------*/
/**----------------------------------------------------------------------------------------*/
/** funkcje prywatne do naszego TSA (by WSZ)*/
/**----------------------------------------------------------------------------------------*/
/**----------------------------------------------------------------------------------------*/

/*
odczytywanie kolejnego numeru seryjnego znaczika czasu (z pliku)
sn to adres zmiennej, do ktorej zostanie przypisana odczytana wartosc numeru seryjnego
*/
long _read_ts_serial(char *file_name, long *sn, FILE **file)
{
char bufor[100]; //na wyrost... i tak nie bedziemy obslugiwac liczb 100-cyfrowych
char *tmp=NULL;
FILE *plik=NULL;
long num=0;
long fd=0;

	if(file_name == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sn == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(file == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*file != NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	plik=fopen(file_name, "r");
	if(plik==NULL)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	//blokowanie
	fd=fileno(plik);
#ifndef WIN32
	flock(fd, LOCK_EX);
#endif

	memset(bufor, 0, 100);
	if( (fscanf(plik, "%s", bufor)) == EOF)
	{
		fclose(plik);
		PRINT_DEBUG("LIBBMDPKI Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}

	tmp=strdup((const char*)bufor);


	num=strtol((const char*)tmp, NULL, 0);
	free(tmp);
	if(num <= 0)
	{
		fclose(plik);
		PRINT_DEBUG("LIBBMDPKI Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	*file=plik;
	*sn=num;
	return BMD_OK;

}

/*
zapis (do pliku) numeru seryjnego znacznika czasu, ktory zostanie nadany nastepnym razem
*/
long _write_ts_serial(char *file_name, long sn, FILE **file)
{
FILE *plik=NULL;

	if(file_name == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(file == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	plik=fopen(file_name, "w");
	if(plik == NULL)
	{
		fclose(*file); *file=NULL;
		PRINT_DEBUG("LIBBMDPKI Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}
	if( (fprintf(plik, "%li", sn)) == 0 )
	{
		fclose(plik);
		fclose(*file); *file=NULL;
		PRINT_DEBUG("LIBBMDPKI Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}
	fclose(plik);
	fclose(*file); //zniesienie blokady
	*file=NULL;

	return BMD_OK;
}

/*
Funkcja tworzy odpowiedni status odpowiedzi znacznika czasu.
PKIStatus_e zdefiniowany w PKIStatus.h wygenerowanym przez Lev Walkina
new_status to adres zmiennej status w strukturze PKIStatusInfo
*/
long _Create_PKIStatus(PKIStatus_e flag, PKIStatus_t *new_status)
{
PKIStatus_t pki_status;
	memset(&pki_status, 0, sizeof(PKIStatus_t)); /*ja pierdole!!!! zapomnialem!!!! ahhhh!!!*/

	if(new_status == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	switch(flag)
	{
		case PKIStatus_granted:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_granted);
			break;
		}
		case PKIStatus_grantedWithMods:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_grantedWithMods);
			break;
		}
		case PKIStatus_rejection:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_rejection);
			break;
		}
		case PKIStatus_waiting:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_waiting);
			break;
		}
		case PKIStatus_revocationWarning:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_revocationWarning);
			break;
		}
		case PKIStatus_revocationNotification:
		{
			asn_long2INTEGER(&pki_status, (long)PKIStatus_revocationNotification);
			break;
		}
		default: { return BMD_ERR_PARAM1; }
	}

	*new_status=pki_status;
	return BMD_OK;
}

/*
Funkcja alokuje i przygotowuje odpowiednie PKIFailureInfo (powod nieoznakowania znacznikiem czasu)*/
long _Create_PKIFailureInfo(PKIFailureInfo_e flag, PKIFailureInfo_t **new_failureinfo)
{
BIT_STRING_t *fail_info=NULL;

	if(new_failureinfo == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(*new_failureinfo != NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	fail_info=(BIT_STRING_t*)calloc(1, sizeof(BIT_STRING_t));
	if(fail_info == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	//wykorzystane 26 bitow (od 6 do 31 bit) (alokuje 32 bity - 6 bitow nieuzywanych)
	fail_info->size=4;
	fail_info->bits_unused=6;
	fail_info->buf=(uint8_t*)calloc(fail_info->size, sizeof(uint8_t));
	if(fail_info->buf == NULL)
	{
		free(fail_info);
		PRINT_DEBUG("LIBBMDPKI Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	switch(flag)
	{
		case PKIFailureInfo_badAlg:
		{
			//0 : 6 nieuzywanych
			fail_info->buf[3]=64;
			break;
		}
		case PKIFailureInfo_badRequest:
		{
			//2 : 6 nieuz.
			fail_info->buf[2]=1;
			break;
		}
		case PKIFailureInfo_badDataFormat:
		{
			//5
			fail_info->buf[2]=8;
			break;
		}
		case PKIFailureInfo_timeNotAvailable:
		{
			//14
			fail_info->buf[1]=16;
			break;
		}
		case PKIFailureInfo_unacceptedPolicy:
		{
			//15
			fail_info->buf[1]=32;
			break;
		}
		case PKIFailureInfo_unacceptedExtension:
		{
			//16
			fail_info->buf[1]=64;
			break;
		}
		case PKIFailureInfo_addInfoNotAvailable:
		{
			//17
			fail_info->buf[1]=128;
			break;
		}
		case PKIFailureInfo_systemFailure:
		{
			//25
			fail_info->buf[0]=128;
			break;
		}
		default:
		{
			free(fail_info->buf);
			free(fail_info);
			PRINT_DEBUG("LIBBMDPKI Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
	}

	*new_failureinfo=fail_info;
	return BMD_OK;
}


/*
funkcja koduje do DER strukture TSTInfo i wynik kodowania umieszcza w OCTET_STRING
*/
long _encode_TSTInfo_to_eContent(TSTInfo_t *tst_info, OCTET_STRING_t **econtent)
{
OCTET_STRING_t *octet=NULL;
GenBuf_t *tmp_buf=NULL;
long status;
	if(tst_info == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(econtent == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(*econtent != NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	status=asn1_encode(&asn_DEF_TSTInfo,tst_info, NULL, &tmp_buf);
	if( status != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKI asn1_encode error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	octet=(OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
	if(octet == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	status=OCTET_STRING_fromBuf(octet,(const char *)tmp_buf->buf,tmp_buf->size);
	free_gen_buf(&tmp_buf);
	if( status != 0 )
		return BMD_ERR_OP_FAILED;

	*econtent=octet;

	return BMD_OK;
}

/*
funkcja na podstawie struktury TSTInfo tworzy strukture EncapsulatedContentInfo
*/
long _Create_encapContentInfo(	TSTInfo_t *tst_info,
						EncapsulatedContentInfo_t** encap)
{
OBJECT_IDENTIFIER_t *oid_tstinfo		= NULL;
EncapsulatedContentInfo_t *nowy_encap	= NULL;
OCTET_STRING_t *econtent			= NULL;
long oid_tab[]					= { TSTINFO_OID_LONG };
long ret_val					= 0;

	if(tst_info == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(encap == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*encap != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if( (_encode_TSTInfo_to_eContent(tst_info, &econtent)) != BMD_OK )
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	oid_tstinfo=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(oid_tstinfo == NULL)
	{
		free(econtent->buf);
		free(econtent);
		BMD_FOK(BMD_ERR_MEMORY);
	}
	ret_val=OBJECT_IDENTIFIER_set_arcs(oid_tstinfo, oid_tab, sizeof(oid_tab[0]), sizeof(oid_tab)/sizeof(long));
	if(ret_val != 0)
	{
		free(econtent->buf);
		free(econtent);
		free(oid_tstinfo);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	nowy_encap=(EncapsulatedContentInfo_t*)calloc(1, sizeof(EncapsulatedContentInfo_t));
	if(nowy_encap == NULL)
	{
		free(econtent->buf);
		free(econtent);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, oid_tstinfo, 1);
		free(oid_tstinfo);
		BMD_FOK(BMD_ERR_MEMORY);
	}
	nowy_encap->eContent=econtent;
	nowy_encap->eContentType=*oid_tstinfo;
	free(oid_tstinfo); //opakowanie zwolnione

	*encap=nowy_encap;
	return BMD_OK;
}

/*Funkcja koduje do DER odpowiedz znacznika czasu i wynik kodowania zapisuje w GenBuf*/
long _encode_TimeStampResp(TimeStampResp_t* ts_resp, GenBuf_t** result)
{
	long status;

	if(ts_resp == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(result == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(*result != NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	status=asn1_encode(&asn_DEF_TimeStampResp,ts_resp, NULL, result);
	if( status != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKI Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

/*implementacja by WSZ (wedlug mnie najwydajniejsza implementacja porowniania OIDow)*/
long cmp_OID_with_longTab(	OBJECT_IDENTIFIER_t *oid,
					long *tab,
					long N)
{
long oid_tab[100]; //na wyrost
long ret_val=0;
long iter=0;

	//ret_val mowi, ile faktycznie bylo socketow w OID (probuje odczytac 100)
	ret_val=OBJECT_IDENTIFIER_get_arcs(oid, oid_tab, sizeof(long), 100);
	if(ret_val < 0)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	//jesli porownywane oidy roznych dlugosci
	if(ret_val != N)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	for(iter=0; iter<ret_val; iter++)
	{
		if(tab[iter] != oid_tab[iter])
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	return BMD_OK;
}

long cmp_OID_with_charTab(	OBJECT_IDENTIFIER_t *oid,
					char *tab)
{
long oid_tab[100]; //na wyrost
long ret_val=0;
long iter=0;
long *oid_table		= NULL;
long oid_table_size	= 0;

	//ret_val mowi, ile faktycznie bylo socketow w OID (probuje odczytac 100)
	ret_val=OBJECT_IDENTIFIER_get_arcs(oid, oid_tab, sizeof(long), 100);
	if(ret_val < 0)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	BMD_FOK(str_of_OID_to_ulong((const char *)tab, &oid_table, &oid_table_size));

	if(ret_val != (long)(oid_table_size/sizeof(long)))
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	for(iter=0; iter<ret_val; iter++)
	{
		if(oid_table[iter] != oid_tab[iter])
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	return BMD_OK;
}


/*
Funkcja tworzy negatywna odpowiedz w przypadku odrzucenia zadania znacznika czasu
*/
long _Create_negativeResponse(PKIStatus_e status_enum, PKIFailureInfo_e failInfo_enum, GenBuf_t **negativeResp)
{
	PKIStatusInfo_t pki_statusinfo;
	PKIFailureInfo_t *fail_info=NULL;
	TimeStampResp_t *ts_resp=NULL;
	GenBuf_t *buf_resp=NULL;

	if(status_enum <= PKIStatus_grantedWithMods) //tylko odpowiedzi odmowne
	{
		PRINT_DEBUG("LIBBMDPKI Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(negativeResp == NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(*negativeResp != NULL)
	{
		PRINT_DEBUG("LIBBMDPKI Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	memset(&pki_statusinfo, 0, sizeof(PKIStatusInfo_t)); //koniecznie wyzerowac

	if(_Create_PKIStatus(status_enum, &(pki_statusinfo.status)) != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKI Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	if(_Create_PKIFailureInfo(failInfo_enum, &fail_info) != BMD_OK)
	{
		//tylko czyszczenie wnetrza
		asn_DEF_PKIStatusInfo.free_struct(&asn_DEF_PKIStatusInfo,&pki_statusinfo, 1);
		PRINT_DEBUG("LIBBMDPKI Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	pki_statusinfo.failInfo=fail_info;
	ts_resp=(TimeStampResp_t*)calloc(1, sizeof(TimeStampResp_t));
	if(ts_resp == NULL)
	{
		//tylko czyszczenie wnetrza
		asn_DEF_PKIStatusInfo.free_struct(&asn_DEF_PKIStatusInfo,&pki_statusinfo, 1);
		PRINT_DEBUG("LIBBMDPKI Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	ts_resp->status=pki_statusinfo;
	if( _encode_TimeStampResp(ts_resp, &buf_resp) != BMD_OK )
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
		free(ts_resp);
		PRINT_DEBUG("LIBBMDPKI Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
	free(ts_resp);

	*negativeResp=buf_resp;
	return BMD_OK;
}
