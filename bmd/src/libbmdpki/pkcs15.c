#include <bmd/libbmdasn1_common/pkcs15/PKCS15Token.h>
#include <bmd/libbmdasn1_common/ContentInfo.h>
#include <bmd/libbmdasn1_common/pkcs15/PBKDF2-params.h>
#include <bmd/libbmdasn1_common/pkcs15/RSAPrivateKey.h>
#include <bmd/libbmdasn1_common/pkcs15/PKCS15RSAPrivateKeyObject.h>
#include <bmd/libbmdasn1_common/pkcs15/RSAPublicKey.h>
#include <bmd/common/bmd-openssl.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/* deszyfrowanie z wylaczonym paddingiem - bo dane maja wlasciwa dlugosc */
long _pkcs15_decrypt(const char *phase,char *inp,long inp_len,
					char *key,long key_len,char *iv,
					char *out,long *out_len,long padding,long debug)
{
	int si_temp			= 0;
	long status			= 0;
	char padded[32];
	long padded_length		= 0;
	EVP_CIPHER_CTX ctx;
	
	EVP_CIPHER_CTX_init(&ctx);
	status=EVP_DecryptInit(&ctx,EVP_des_ede3_cbc(), (unsigned char*)key, (unsigned char*)iv);
	if( debug ) printf("%s EVP_DecryptInit %i\n",phase,status);
	EVP_CIPHER_CTX_set_padding(&ctx,padding);
	status=EVP_CIPHER_CTX_set_key_length(&ctx,key_len);
	if( debug ) printf("%s EVP_CIPHER_CTX_set_key_length %i\n",phase,status);
	si_temp = *out_len;
	status=EVP_DecryptUpdate(&ctx,(unsigned char*)out, &si_temp, (unsigned char*)inp, inp_len);
	*out_len = si_temp;
	if( debug ) printf("%s EVP_DecryptUpdate %i\n",phase,status);
	si_temp = padded_length;
	status=EVP_DecryptFinal(&ctx,(unsigned char*)padded, &si_temp);
	padded_length = si_temp;
	if( debug ) printf("%s EVP_DecryptFinal %i\n",phase,status);
	
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	return 0;
}

long _pkcs15_decode_buf(char *buf,long length,PKCS15Token_t **p15,long debug)
{
	asn_dec_rval_t rval;
	ContentInfo_t *ci=NULL;

	rval=ber_decode(NULL,&asn_DEF_ContentInfo,(void **)&ci,buf,length);
	if( rval.code == RC_OK )
	{
		if( debug ) printf("Successfully decoded ContentInfo wrapper\n");
	}
	else
	{
		if( debug ) printf("Bad format - unable to decode ContentInfo\n");
		return -1;
	}

	rval=ber_decode(NULL,&asn_DEF_PKCS15Token,(void **)p15,ci->content.buf,ci->content.size);
	if( rval.code == RC_OK )
	{
		if( debug ) printf("Successfully decoded PKCS15Token structure\n");
	}
	else
	{
		if( debug ) printf("Bad format - PKCS15Token has wrong format\n");
		return -1;
	}

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo,ci,0);ci=NULL;
	
	return 0;
}

long _pkcs15_get_pbkdf2_key(PKCS15EnvelopedData_t *p15_env,const char *pass,long pass_len,
                           char **key,long *key_len,long debug)
{
	asn_dec_rval_t rval;
	PBKDF2_params_t *pbkdf=NULL;
	AlgorithmIdentifier_t *ai=NULL;
	OCTET_STRING_t *IV=NULL;
	long status;
	char *salt=NULL;
	long salt_len,iter_count;
	char pbkdf2_key[EVP_MAX_KEY_LENGTH];
	char b1[1024],b2[1024],b3[1024];
	long b1_l,b2_l,b3_l;
	
	if( p15_env->recipientInfos.list.array[0]->choice.pwri.keyDerivationAlgorithm )
	{
		rval=ber_decode(NULL,&asn_DEF_PBKDF2_params,(void **)&pbkdf,
		                p15_env->recipientInfos.list.array[0]->choice.pwri.keyDerivationAlgorithm->parameters->buf,
						p15_env->recipientInfos.list.array[0]->choice.pwri.keyDerivationAlgorithm->parameters->size);
		if( rval.code == RC_OK )
		{
			salt_len=pbkdf->salt.choice.specified.size;
			salt=(char *)malloc(salt_len * sizeof(char));
			memmove(salt,pbkdf->salt.choice.specified.buf,salt_len);
			asn_INTEGER2long(&(pbkdf->iterationCount), &iter_count);
			
			status=PKCS5_PBKDF2_HMAC_SHA1(	pass,pass_len, (unsigned char*)salt, salt_len,iter_count,
							EVP_CIPHER_key_length(EVP_des_ede3_cbc()), 
							(unsigned char*)pbkdf2_key);
			if( status != 1 )
			{
				printf("failed to generate KEK\n");
				return -1;
			}
			rval=ber_decode(NULL,&asn_DEF_AlgorithmIdentifier,(void **)&ai,
							p15_env->recipientInfos.list.array[0]->choice.pwri.keyEncryptionAlgorithm.parameters->buf,
							p15_env->recipientInfos.list.array[0]->choice.pwri.keyEncryptionAlgorithm.parameters->size);
			if( rval.code == RC_OK )
			{
				/* pobranie od razu wektora IV dla klucza ktory zaraz bedzie zdeszyfrowany */
				rval=ber_decode(NULL,&asn_DEF_OCTET_STRING,(void **)&IV,
								ai->parameters->buf,
								ai->parameters->size
								);
				if( rval.code != RC_OK )
					return -1;
			}
			else
				return -1;
			
			_pkcs15_decrypt("decrypting n-th block", (char *)(p15_env->recipientInfos.list.array[0]->
			choice.pwri.encryptedKey.buf+24),8, pbkdf2_key,24, (char *)(p15_env->recipientInfos.list.array[0]->
			choice.pwri.encryptedKey.buf+16), b1,&b1_l,0,1);
			_pkcs15_decrypt("decrypting first n-1 blocks", (char *)(p15_env->recipientInfos.list.array[0]->
			choice.pwri.encryptedKey.buf),24, pbkdf2_key,24,b1, b2,&b2_l,0,1);
			memmove(b2+24,b1,b1_l);b2_l=32;memset(b3,0,32);
			_pkcs15_decrypt("decrypting using KEK",b2,b2_l,
							pbkdf2_key,24,(char *)IV->buf,
							b3,&b3_l,1,1);
			/* tutaj sprawdzenie poprawnosci klucza w celu powiedzenia ze bledne haslo np. */
			(*key)=(char *)malloc(24 * sizeof(char));
			memmove(*key,b3+4,24);*key_len=24;
			asn_DEF_AlgorithmIdentifier.free_struct(&asn_DEF_AlgorithmIdentifier,ai,0);ai=NULL;
			asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,IV,0);IV=NULL;
			asn_DEF_PBKDF2_params.free_struct(&asn_DEF_PBKDF2_params,pbkdf,0);pbkdf=NULL;
		}
		else
		{
			if( debug ) printf("Unable to get salt. Quitting\n");
			return -1;
		}
	}
	else
	{
		if( debug ) printf("Bad PWRI format\n");
		return -1;
	}

	return 0;
}

long _pkcs15_get_privatekey(PKCS15Token_t *p15,const char *pass,long pass_len,
                           char **rsa_priv_key,long *rsa_priv_len,long debug)
{
	long status,i;
	asn_dec_rval_t rval;
	PKCS15EnvelopedData_t *p15_env=NULL;
	OCTET_STRING_t *IV=NULL;
	char *kek=NULL;
	long kek_length;
	
	for(i=0;i<p15->pkcs15Objects.list.count;i++)
	{
		if( p15->pkcs15Objects.list.array[i]->present == PKCS15Objects_PR_privateKeys )
		{
			if( p15->pkcs15Objects.list.array[i]->choice.privateKeys.present == PKCS15PrivateKeys_PR_objects )
			{
				p15_env=&(p15->pkcs15Objects.list.array[i]->choice.privateKeys.choice.objects.list.array[0]->choice.privateRSAKey.typeAttributes.value.direct_protected);
				break;
			}
		}
	}
	
	if( p15_env == NULL )
		return -1;

	status=_pkcs15_get_pbkdf2_key(p15_env,pass,pass_len,&kek,&kek_length,debug);
	rval=ber_decode(NULL,&asn_DEF_OCTET_STRING,(void **)&IV,
	                p15_env->encryptedContentInfo.contentEncryptionAlgorithm.parameters->buf,
					p15_env->encryptedContentInfo.contentEncryptionAlgorithm.parameters->size);
	if(rval.code != RC_OK )
		return -1;
	
	(*rsa_priv_key)=(char *)malloc(2*p15_env->encryptedContentInfo.encryptedContent->size);
	
	_pkcs15_decrypt("RSA decrypt",
					(char*)p15_env->encryptedContentInfo.encryptedContent->buf,
					p15_env->encryptedContentInfo.encryptedContent->size,
					kek,kek_length,(char*)IV->buf,
					*rsa_priv_key,rsa_priv_len,0,1);
	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,IV,0);IV=NULL;
	
	return 0;
}

long _pkcs15_get_certificate(PKCS15Token_t *p15,GenBuf_t **cert_der,long debug)
{
	Certificate_t *cert=NULL;
	long i;

	for(i=0;i<p15->pkcs15Objects.list.count;i++)
	{
		if( p15->pkcs15Objects.list.array[i]->present == PKCS15Objects_PR_certificates )
		{
			if( p15->pkcs15Objects.list.array[i]->choice.certificates.present == PKCS15Certificates_PR_objects )
			{
				cert=&(p15->pkcs15Objects.list.array[i]->choice.certificates.choice.objects.list.array[0]->typeAttributes.value.direct);
				break;
			}
		}
	}
	
	if( cert != NULL )
		asn1_encode(&asn_DEF_Certificate,(void *)cert, NULL, cert_der);
	else
	{
		if( debug ) printf("Nie mozna znalezc certyfikatu\n");
		return -1;
	}
	
	return 0;
}

long _pkcs15_convert_privkey(char *rsa_priv_key,long rsa_priv_key_len,GenBuf_t *cert_der,
							GenBuf_t **converted_key,long debug)
{
	PKCS15RSAPrivateKeyObject_t *pkcs15_privkey=NULL;
	RSAPrivateKey_t *pkcs1_privkey=NULL;
	RSAPublicKey_t *pubkey=NULL;
	Certificate_t *cert=NULL;
	asn_dec_rval_t rval;
	long version=0;
	INTEGER_t *tmp=NULL;
	
	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&cert,cert_der->buf,cert_der->size);
	if( rval.code != RC_OK )
	{
		if( debug ) printf("Bad certificate format\n");
		return -1;
	}
	
	rval=ber_decode(NULL,&asn_DEF_RSAPublicKey,(void **)&pubkey,
					cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey.buf,
	                cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey.size);
	if( rval.code != RC_OK )
	{
		if( debug ) printf("Bad RSAPublicKey format\n");
		return -1;
	}
	
	rval=ber_decode(NULL,&asn_DEF_PKCS15RSAPrivateKeyObject,(void **)&pkcs15_privkey,rsa_priv_key,rsa_priv_key_len);
	if( rval.code != RC_OK )
	{
		if( debug ) printf("Bad PKCS15PrivateKeyobject\n");
		return -1;
	}
		
	pkcs1_privkey=(RSAPrivateKey_t *)malloc(sizeof(RSAPrivateKey_t));
	memset(pkcs1_privkey,0,sizeof(RSAPrivateKey_t));
	asn_long2INTEGER(&(pkcs1_privkey->version),version);
	tmp=&(pkcs1_privkey->modulus);
	asn_cloneContent(&asn_DEF_INTEGER,&(pubkey->modulus),(void **)&tmp);
	tmp=&(pkcs1_privkey->publicExponent);
	asn_cloneContent(&asn_DEF_INTEGER,&(pubkey->publicExponent),(void **)&tmp);
	tmp=&(pkcs1_privkey->prime1);
	asn_cloneContent(&asn_DEF_INTEGER,pkcs15_privkey->prime1,(void **)&tmp);
	tmp=&(pkcs1_privkey->prime2);
	asn_cloneContent(&asn_DEF_INTEGER,pkcs15_privkey->prime2,(void **)&tmp);
	tmp=&(pkcs1_privkey->exponent1);
	asn_cloneContent(&asn_DEF_INTEGER,pkcs15_privkey->exponent1,(void **)&tmp);
	tmp=&(pkcs1_privkey->exponent2);
	asn_cloneContent(&asn_DEF_INTEGER,pkcs15_privkey->exponent2,(void **)&tmp);
	tmp=&(pkcs1_privkey->coefficient);
	asn_cloneContent(&asn_DEF_INTEGER,pkcs15_privkey->coefficient,(void **)&tmp);
	
	/* obliczenie privateExponent, czyli "d" */
	{
	BIGNUM *BN_prime1=NULL,*BN_prime2=NULL,*BN_prime1_1=NULL;
	BIGNUM *BN_prime2_1=NULL,*BN_prime_mul=NULL,*BN_d=NULL,*BN_e=NULL;
	BN_CTX *BN_ctx_tmp1=NULL,*BN_ctx_tmp2=NULL;
	char d[1024];
	long d_len;
	
	BN_prime1=BN_bin2bn(pkcs15_privkey->prime1->buf,pkcs15_privkey->prime1->size,NULL);
	BN_prime2=BN_bin2bn(pkcs15_privkey->prime2->buf,pkcs15_privkey->prime2->size,NULL);
	BN_prime1_1=BN_new();BN_prime2_1=BN_new();
	BN_sub(BN_prime1_1,BN_prime1,BN_value_one());
	BN_sub(BN_prime2_1,BN_prime2,BN_value_one());
	BN_prime_mul=BN_new();
	BN_ctx_tmp1=BN_CTX_new();
	BN_mul(BN_prime_mul,BN_prime1_1,BN_prime2_1,BN_ctx_tmp1);
	BN_d=BN_new();
	BN_e=BN_bin2bn(pkcs1_privkey->publicExponent.buf,pkcs1_privkey->publicExponent.size,NULL);
	BN_ctx_tmp2=BN_CTX_new();
	BN_mod_inverse(BN_d,BN_e,BN_prime_mul,BN_ctx_tmp2);
	d_len=BN_bn2bin(BN_d,(unsigned char*)d);
	
	pkcs1_privkey->privateExponent.buf=(uint8_t *)malloc(d_len * sizeof(uint8_t));
	pkcs1_privkey->privateExponent.size=d_len;
	memmove(pkcs1_privkey->privateExponent.buf,d,d_len);

	}
	
	asn1_encode(&asn_DEF_RSAPrivateKey,(void *)pkcs1_privkey, NULL, converted_key);
	
	return 0;
}

long _pkcs15_create_pkcs12(GenBuf_t *key,GenBuf_t *cert,const char *pass,GenBuf_t **pkcs12,long debug)
{
	char *p=NULL;
	char *p1=NULL;
	EVP_PKEY *rsa_evp=NULL;
	X509 *cert_x509=NULL;
	PKCS12 *p12=NULL;
	BIO *mem=NULL;
	char buf[32768];
	long buf_len=0;
	
	p=key->buf;
	rsa_evp=d2i_PrivateKey(EVP_PKEY_RSA,&rsa_evp,(const unsigned char **)(&p),key->size);
	if( rsa_evp == NULL )
		return -1;
	p1=cert->buf;
	cert_x509=d2i_X509(NULL,(const unsigned char **)(&p1),cert->size);
	if( cert_x509 == NULL )
		return -1;

	p12=PKCS12_create((char *)pass,NULL,rsa_evp,cert_x509,NULL,0,0,0,0,0);
	if( p12 == NULL )
		return -1;

	mem=BIO_new(BIO_s_mem());
	if( mem == NULL )
		return -1;
	i2d_PKCS12_bio(mem,p12);
	buf_len=BIO_read(mem,buf,32768);
	
	(*pkcs12)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	(*pkcs12)->buf=(char *)malloc(buf_len * sizeof(char));
	memmove((*pkcs12)->buf,buf,buf_len);
	(*pkcs12)->size=buf_len;
	
	BIO_free(mem);
	PKCS12_free(p12);
	X509_free(cert_x509);
	EVP_PKEY_free(rsa_evp);
	
	return 0;
}

long PKCS15toPKCS12(GenBuf_t *p15,const char *pass,long pass_len,GenBuf_t **p12,long debug)
{
	long status;
	PKCS15Token_t *p15_token=NULL;
	char *buf=NULL;
	long len;
	GenBuf_t *cert=NULL;
	GenBuf_t *key=NULL;
	
	status=_pkcs15_decode_buf(p15->buf,p15->size,&p15_token,debug);
	if( status != 0 )
		return status;
	status=_pkcs15_get_privatekey(p15_token,pass,pass_len,&buf,&len,debug);
	if( status != 0 )
		return status;
	status=_pkcs15_get_certificate(p15_token,&cert,debug);
	if( status != 0 )
		return status;
	status=_pkcs15_convert_privkey(buf,len,cert,&key,debug);
	if( status != 0 )
		return status;
	free(buf);buf=NULL;
	status=_pkcs15_create_pkcs12(key,cert,pass,p12,debug);
	if( status != 0 )
		return status;
	asn_DEF_PKCS15Token.free_struct(&asn_DEF_PKCS15Token,p15_token,0);p15_token=NULL;
	free(cert->buf);free(cert);free(key->buf);free(key);
	
	return 0;
}
