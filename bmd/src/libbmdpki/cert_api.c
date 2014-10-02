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

#include <bmd/libbmdasn1_common/SigPolicy/SignerAndVerifierRules.h>
#include <bmd/libbmdasn1_common/AuthorityKeyIdentifier.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>


/**
Funkcja bmd_verify_cert_with_crl() sprawdza waznosc certyfikatu przy pomocy listy CRL.
1) Jesli crlFilePath == NULL i crlURL == NULL,
	to adres, z ktorego nalezy pobrac CRL, bedzie poszukiwany w strukturze sprawdzanego certyfikatu
	W przypadku jego braku funkcja zwroci blad.
2) Jesli crlURL != 0 i crlFilePath == NULL, to lista CRL bedzie pobierana spod wskazanego adresu, natomiast adres do listy CRL
	zapisany w strukturze certyfikatu (jesli jest) zostanie zignorowany.
	Gdyby nie mozna pobrac listy CRL, funkcja zwroci blad.
3) Jesli crlFilePath != NULL i crlURL == NULL, to lista CRL zostanie wczytana ze wskazanego pliku, natomiast adres do listy CRL
	zapisany w strukturze certyfikatu (jesli jest) zostanie zignorowany.
	Gdyby nie mozna wczytac listy CRL, funkcja zwroci blad.
4) Jesli crlURL != NULL i crlFilePath != NULL, to crlFilePath zostanie zignorowany (crlURL jest traktowany priorytetowo).
	Gdyby nie mozna pobrac listy CRL, funkcja zwroci blad.

@param cert to bufor ze sprawdzanym certyfikatem
@param [optional] crlFilePath to sciezka do pliku z lista CRL
@param [optional] crlURL to adres URL, spod ktorego bedzie sciagnieta lista CRL
*/
long bmd_verify_cert_with_crl(GenBuf_t *cert, char* crlURL, char* crlFilePath)
{
long i					= 0;
long j					= 0;
long status					= 0;
long is_on_crl				= 0;
long flag_crl_ext_present		= 0;
long flag_downloaded			= 0;
char crlDP_oid[]				= {"2.5.29.31"};
Certificate_t *asn1_cert		= NULL;
asn_dec_rval_t rval;
Extensions_t *extensions		= NULL;
Extension_t *ext				= NULL;
CRLDistributionPoints_t *crl_dp	= NULL;
DistributionPointName_t *dp_name	= NULL;
DistributionPoint_t *dp			= NULL;
GeneralName_t *gn				= NULL;
GenBuf_t *gb				= NULL;
char *tmp					= NULL;

CertificateList_t *crl_list		= NULL;
CertificateSerialNumber_t *cert_number	= NULL;
long crl_serial_number,cert_serial_number;

	PRINT_INFO("LIBBMDPKIINF Verifying certificate with CRL\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(cert==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_cert,cert->buf,cert->size);
	if(rval.code!=RC_OK)
	{
		BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR);
	}

	asn_INTEGER2long(&(asn1_cert->tbsCertificate.serialNumber),&cert_serial_number);
	if(crlURL==NULL && crlFilePath==NULL)
	{
		extensions=asn1_cert->tbsCertificate.extensions;
		for(i=0;i<extensions->list.count;i++)
		{
			ext=extensions->list.array[i];
			if(OBJECT_IDENTIFIER_cmp(&(ext->extnID),crlDP_oid)==0)
			{
				rval=ber_decode(NULL,&asn_DEF_CRLDistributionPoints,(void **)&crl_dp,ext->extnValue.buf,ext->extnValue.size);
				if(rval.code!=RC_OK)
				{
					BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR);
				}
				flag_crl_ext_present=1;
				break;
			}
		}

		if(flag_crl_ext_present==0)
		{
			BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_CRL_EXT_ABSENT);
		}

		for(i=0;i<crl_dp->list.count;i++)
		{
			dp=crl_dp->list.array[i];
			dp_name=dp->distributionPoint;
			if(dp_name->present==DistributionPointName_PR_fullName)
			{
				for(j=0;j<dp_name->choice.fullName.list.count;j++)
				{
					gn=dp_name->choice.fullName.list.array[j];
					GeneralName2string(gn,&tmp);
					gb=get_from_url(tmp);
					if(gb)
					{
						flag_downloaded=1;
						break;
					}
				}
				if(flag_downloaded==1)
				{
					asn_DEF_CRLDistributionPoints.free_struct(&asn_DEF_CRLDistributionPoints,crl_dp,0);crl_dp=NULL;
					break;
				}
			}
		}
	}
	else if(crlURL != NULL)
	{
		gb=get_from_url(crlURL);
	}
	else /*crlFilePath != NULL*/
	{
		status=bmd_load_binary_content(crlFilePath,&gb);
		if(status!=BMD_OK)
		{
			BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CRL_ERROR);
		}
	}

	if ( (gb==NULL) || (gb->size==0) )														// w przypadku gdy nie da sie pobrac listy CRL z serwera to zwraca blad
	{
		BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_NO_CRL_IN_FILE);
	}

	/************************/
	/*	zdekodowanie CRL	*/
	/************************/
	rval=ber_decode(NULL,&asn_DEF_CertificateList,(void **)&(crl_list),gb->buf,gb->size);
	if(rval.code!=RC_OK)
	{
		BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_CRL_DECODE_ERROR);
	}

	if(crl_list->tbsCertList.revokedCertificates != NULL) //jesli pusta lista crl, to znaczy, ze jest ok
	{
		for(i=0;i<crl_list->tbsCertList.revokedCertificates->list.count;i++)
		{
			cert_number=&(crl_list->tbsCertList.revokedCertificates->list.array[i]->userCertificate);
			asn_INTEGER2long(cert_number,&crl_serial_number);
			if(crl_serial_number==cert_serial_number)
			{
				is_on_crl=1;
				break;
			}
		}
	}

	free_gen_buf(&gb);
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_cert,0);asn1_cert=NULL;
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList,crl_list,0);crl_list=NULL;
	if(is_on_crl==1)
	{
		BMD_FOK(BMD_VERIFY_LOGIN_CERTIFICATE_ON_CRL);
	}
	return BMD_OK;
}

long _bmd_verify_RsaWithSHA1_signature(GenBuf_t *signature,GenBuf_t *der,GenBuf_t *pub_key)
{
RSA *RSA_pubkey=NULL;
char *b=NULL;
GenBuf_t *hash=NULL;
long status;
long verification_flag=-1;
bmd_crypt_ctx_t *hash_ctx=NULL;

	PRINT_INFO("LIBBMDPKIINF Verifying RSA with SHA1 signature\n");
	if(signature==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(pub_key==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	b=pub_key->buf;
	RSA_pubkey=d2i_RSAPublicKey(NULL,(const unsigned char **)&b,pub_key->size);
	if(RSA_pubkey!=NULL)
	{

		status=bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		status=bmd_hash_data(der,&hash_ctx,&hash,NULL);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		status=RSA_verify(NID_sha1, (unsigned char*)(hash->buf), hash->size, (unsigned char*)(signature->buf), \
		signature->size,RSA_pubkey);
		if(status==1)
		{
			verification_flag=BMD_OK;
		}
		else
		{
			verification_flag=BMD_ERR_PKI_VERIFY;
		}
	}
	else
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid format. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	bmd_ctx_destroy(&hash_ctx);
	free_gen_buf(&hash);
	RSA_free(RSA_pubkey);RSA_pubkey=NULL;

	return verification_flag;
}

/*bawimy sie*/
long _bmd_verify_RsaWithMD5_signature(GenBuf_t *signature,GenBuf_t *der,GenBuf_t *pub_key)
{
RSA *RSA_pubkey=NULL;
char *b=NULL;
GenBuf_t *hash=NULL;
long status;
long verification_flag=-1;
bmd_crypt_ctx_t *hash_ctx=NULL;

	PRINT_INFO("LIBBMDPKIINF Verifying RSA with SHA1 signature\n");
	if(signature==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(pub_key==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	b=pub_key->buf;
	RSA_pubkey=d2i_RSAPublicKey(NULL,(const unsigned char **)&b,pub_key->size);
	if(RSA_pubkey!=NULL)
	{

		status=bmd_set_ctx_hash(&hash_ctx, BMD_HASH_ALGO_MD5);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		status=bmd_hash_data(der,&hash_ctx,&hash,NULL);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		status=RSA_verify(NID_md5, (unsigned char*)hash->buf, hash->size, (unsigned char*)signature->buf, \
		signature->size,RSA_pubkey);
		if(status==1)
			verification_flag=BMD_OK;
		else
			verification_flag=BMD_ERR_PKI_VERIFY;
	}
	else
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid format. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	bmd_ctx_destroy(&hash_ctx);
	free_gen_buf(&hash);
	RSA_free(RSA_pubkey);RSA_pubkey=NULL;

	return verification_flag;
}



long _bmd_get_signature_from_certificate(Certificate_t *cert,GenBuf_t **signature,GenBuf_t **der)
{
	long status;

	PRINT_INFO("LIBBMDPKIINF Getting signature from certificate\n");
	if(cert==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(signature==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*signature)!=NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	status=asn1_encode(&asn_DEF_TBSCertificate,(void *)&(cert->tbsCertificate), NULL, der);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=set_gen_buf2((char*)(cert->signature.buf), cert->signature.size, signature);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

    return BMD_OK;
}

long bmd_verify_cert_with_cert(GenBuf_t *cert,GenBuf_t *issuer_cert)
{
Certificate_t *asn1_cert=NULL,*asn1_issuer_cert=NULL;
GenBuf_t *pub_key=NULL,*signature=NULL,*der=NULL;
asn_dec_rval_t rval;
long status;

	PRINT_INFO("LIBBMDPKIINF Verifying certificate with other certificate\n");
	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_cert,cert->buf,cert->size);
	if(rval.code!=RC_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in ber decoding. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_issuer_cert,issuer_cert->buf,issuer_cert->size);
	if(rval.code!=RC_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Errorin ber decoding. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=GetPubKeyFromCert(issuer_cert,&pub_key);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in getting public key from certificate. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=_bmd_get_signature_from_certificate(asn1_cert,&signature,&der);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in getting signature from certificate. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=_bmd_verify_RsaWithSHA1_signature(signature,der,pub_key);
	if(status != 0) //sprawdzamy dodatkowo md5
	{
		status=_bmd_verify_RsaWithMD5_signature(signature,der,pub_key);
	}

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_cert,0);asn1_cert=NULL;
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_issuer_cert,0);asn1_issuer_cert=NULL;
	free_gen_buf(&pub_key);
	free_gen_buf(&der);
	free_gen_buf(&signature);

	return status;
}

long bmd_get_validity_time_from_cert(GenBuf_t *cert,time_t *notBefore,time_t *notAfter)
{
asn_dec_rval_t rval;
long status				= BMD_OK;
Certificate_t *asn1_cert	= NULL;
Time_t *not_before		= NULL;
Time_t *not_after			= NULL;
time_t tmp_time;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (cert==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if (notBefore==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM2;
	}

	if (notAfter==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM3;
	}


	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_cert,cert->buf,cert->size);
	if(rval.code!=RC_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n", GetErrorMsg(BMD_CERTIFICATE_DECODE_ERROR),
									 BMD_CERTIFICATE_DECODE_ERROR);
		return BMD_CERTIFICATE_DECODE_ERROR;
	}

	not_before=&asn1_cert->tbsCertificate.validity.notBefore;
	not_after=&asn1_cert->tbsCertificate.validity.notAfter;

	switch(not_before->present)
	{
		case Time_PR_NOTHING:
			status=BMD_CERTIFICATE_TIME_BEFORE_NOT_PRESENT; break;
		case Time_PR_utcTime:
			tmp_time=asn_UT2time(&(not_before->choice.utcTime),NULL,0);
			if(tmp_time==-1)
			{
				status=BMD_CERTIFICATE_UT_TO_TIME_DECODE_PROBLEM;
			}
			else
			{
				*notBefore=tmp_time;
			}
			break;
		case Time_PR_generalTime:
			tmp_time=asn_GT2time(&(not_before->choice.generalTime),NULL,0);
			if(tmp_time==-1)
			{
				status=BMD_CERTIFICATE_GT_TO_TIME_DECODE_PROBLEM;
			}
			else
			{
				*notBefore=tmp_time;
			}
			break;
	}

	switch(not_after->present)
	{
		case Time_PR_NOTHING:
			status=BMD_CERTIFICATE_TIME_AFTER_NOT_PRESENT;
			break;
		case Time_PR_utcTime:
			tmp_time=asn_UT2time(&(not_after->choice.utcTime),NULL,0);
			if(tmp_time==-1)
			{
				status=BMD_CERTIFICATE_UT_TO_TIME_DECODE_PROBLEM;
			}
			else
			{
				*notAfter=tmp_time;
			}
			break;
		case Time_PR_generalTime:
			tmp_time=asn_GT2time(&(not_after->choice.generalTime),NULL,0);
			if(tmp_time==-1)
			{
				status=BMD_CERTIFICATE_GT_TO_TIME_DECODE_PROBLEM;
			}
			else
			{
				*notAfter=tmp_time;
			}
			break;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_cert,0);asn1_cert=NULL;
	return status;
}

long __pki_bmd_genbuf_hex_dump(GenBuf_t *input,char **output,char sep,long uppercase)
{
long flag			=-1;
long i				= 0;
char octet[4];
unsigned char uc_temp		= 0;
unsigned int ui_temp		= 0;

	if(sep==0)
	{
		flag=0;
	}
	else
	{
		flag=1;
	}

	if(input==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter error. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->buf==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter error. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->size<=0)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter error. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(output==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter error. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*output)!=NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter error. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	(*output)=(char *)malloc( (input->size*3 + 1) * sizeof(char) );
	if(*output == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset((*output), 0, (input->size*3 + 1) * sizeof(char));
	PRINT_VDEBUG("LIBBMDPKIVDEBUG input->size: %li\n", input->size);

	for(i=0;i<input->size-1;i++)
	{
		memset(octet, 0, 4 * sizeof(char));

		memcpy(&uc_temp, input->buf + i, sizeof(char));
		ui_temp = uc_temp;
		if(flag)
		{
			if(uppercase==1)
			{
				sprintf(octet,"%02X%c", ui_temp, sep);
			}
			else
			{
				sprintf(octet,"%02x%c", ui_temp, sep);
			}
		}
		else
		{
			if(uppercase==1)
			{
				sprintf(octet,"%02X%c", ui_temp, sep);
			}
			else
			{
				sprintf(octet,"%02x%c", ui_temp, sep);
			}
		}
		strcat(*output,octet);
	}

	memset(octet, 0, 4 * sizeof(char));

	memcpy(&uc_temp, input->buf + i, sizeof(char));
	ui_temp = uc_temp;

	if(uppercase)
	{
		sprintf(octet,"%02X", ui_temp);
	}
	else
	{
		sprintf(octet,"%02x", ui_temp);
	}

	strcat(*output,octet);

	PRINT_VDEBUG("LIBBMDPKIVDEBUG Output: %s\n", *output);

	return BMD_OK;
}

long bmd_cert_get_serial(GenBuf_t *cert,char **serial)
{
asn_dec_rval_t rval;
long status;
Certificate_t *asn1_cert=NULL;
GenBuf_t *tmp=NULL;


	PRINT_INFO("LIBBMDPKIINF Getting certificate's serial number\n");
	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_cert,cert->buf,cert->size);
	if(rval.code!=RC_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=set_gen_buf2((char*)(asn1_cert->tbsCertificate.serialNumber.buf), \
	asn1_cert->tbsCertificate.serialNumber.size,&tmp);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	status=__pki_bmd_genbuf_hex_dump(tmp, serial, ' ', 1);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_cert,0);asn1_cert=NULL;
 	PRINT_INFO("LIBBMDPKIINF serial = %s\n",*serial);
	return BMD_OK;
}
