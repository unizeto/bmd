#include <stdio.h>
#ifdef _WIN32
#	include <io.h>
#endif
#include <sys/types.h>
#include <libbmddvcs.h>
#include <libbmdcms.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cryptlib.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#define OID_SHA_1		1,3,14,3,2,26
unsigned long OID_sha_1[] = {OID_SHA_1};

#define wynik(x) printf("[%04d] Wynik: %d\n", __LINE__, x);

/* laduje klucz prywatny z pliku w formacie DER PKCS#8 */
RSA *PKCS8RSAkey_from_file(const char *filename)
{
	RSA *rsa = NULL;
	BIO *key = NULL;
	PKCS8_PRIV_KEY_INFO *pkcs8key = NULL;
	EVP_PKEY *pkey;
	key = BIO_new(BIO_s_file());
	if ( key == NULL )
		return NULL;
	BIO_read_filename(key, filename);
	pkcs8key = d2i_PKCS8_PRIV_KEY_INFO_bio(key, NULL);
	if ( pkcs8key == NULL )
		return NULL;
	pkey = EVP_PKCS82PKEY(pkcs8key);
//	pkey = d2i_PrivateKey_bio(key, NULL);
	if ( pkey == NULL )
		return NULL;
	rsa = EVP_PKEY_get1_RSA(pkey);
	return rsa;
}
// FROM libbmdpkcs15
int convert_component(BIGNUM *source,unsigned char **dest,int *dest_length)
{
	int component_length;
	int i,j;
	
	component_length=(source->dmax-1)*4;
	(*dest)=(unsigned char *)malloc(component_length);
	if((*dest)==NULL)
		return -1;
	i=source->dmax-2;
	j=0;
	while(i>=0)
	{
		if(source->d[i]!=0)
		{
			(*dest)[j++]=source->d[i]>>24;
			(*dest)[j++]=source->d[i]>>16;
			(*dest)[j++]=source->d[i]>>8;
			(*dest)[j++]=source->d[i]&0x000000FF;
		}
		else
			(*dest)[j++]=0;
		i--;
	}
	(*dest_length)=component_length*8; /* dlugosc w bitach */
	return 1;
}
// FROM libbmdpkcs15
int RSA_convert_key(RSA *openssl_key,CRYPT_CONTEXT *cryptKey,int type)
{
	CRYPT_PKCINFO_RSA cl_key;
	unsigned char *n,*e,*d,*p,*q,*u,*e1,*e2;
	int n_length,e_length,d_length,p_length,q_length,u_length,e1_length,e2_length;
	int status;
	
	convert_component(openssl_key->n,&n,&n_length);
	convert_component(openssl_key->e,&e,&e_length);
	convert_component(openssl_key->d,&d,&d_length);
	convert_component(openssl_key->p,&p,&p_length);
	convert_component(openssl_key->q,&q,&q_length);
	convert_component(openssl_key->iqmp,&u,&u_length);
	convert_component(openssl_key->dmp1,&e1,&e1_length);
	convert_component(openssl_key->dmq1,&e2,&e2_length);
	
	cryptInitComponents(&cl_key,type);
	
	cryptSetComponent(cl_key.n,n,n_length);
	cryptSetComponent(cl_key.e,e,e_length);
	cryptSetComponent(cl_key.d,d,d_length);
	cryptSetComponent(cl_key.p,p,p_length);
	cryptSetComponent(cl_key.q,q,q_length);
	cryptSetComponent(cl_key.u,u,u_length);
	cryptSetComponent(cl_key.e1,e1,e1_length);
	cryptSetComponent(cl_key.e2,e2,e2_length);
	
	free(n);free(e);free(d);free(p);free(q);free(u);free(e1);free(e2);
	status = cryptCreateContext(cryptKey, CRYPT_UNUSED,
		CRYPT_ALGO_RSA);
	if ( status )
		return status;
	status=cryptSetAttributeString(*cryptKey,CRYPT_CTXINFO_LABEL,"aaa",3);
	if ( status )
		return status;
	status = cryptSetAttributeString(*cryptKey,
		CRYPT_CTXINFO_KEY_COMPONENTS, &cl_key,
		sizeof(CRYPT_PKCINFO_RSA)); 
	if ( status )
		return status;
	return 0;
}

int main(int argc, char **argv)
{
	unsigned long oid_id_data[] = {OID_CMS_ID_DATA};
	GenBuf_t *outbuf = NULL;
	GenBuf_t *outdvcs = NULL;
	GenBuf_t *outdvcscert = NULL;
	CRYPT_CERTIFICATE cert = 0;
	CRYPT_CONTEXT privKey = 0;
	GenBuf_t *CertBuf = NULL;
	RSA *rsa = NULL;
	const char tekst[] = "Ala ma kota, kot ma Ale";
	GenBuf_t *buf = malloc(sizeof(GenBuf_t));

	buf->size = sizeof(tekst);
	buf->buf= malloc(sizeof(tekst));
	memcpy(buf->buf, tekst, sizeof(tekst));
	wynik(cryptInit());

	wynik(read_GenBuf_from_file("certX.der.cer", &CertBuf));
	wynik(cryptImportCert(CertBuf->buf, CertBuf->size, CRYPT_UNUSED, &cert));

	// zaladuj klucz prywatny
	wynik((rsa = PKCS8RSAkey_from_file("certX.der.key"))==NULL);
	wynik(RSA_convert_key(rsa, &privKey, CRYPT_KEYTYPE_PRIVATE));
	wynik(CMSSignData(
		&cert, &privKey, NULL, NULL, buf, CMSSIGN_HASH_SHA1, &outbuf));

	wynik(DVCSRequest_VSD(
		&cert, &privKey, outbuf, 0, &outdvcs));

	{
		CRYPT_CERTIFICATE *certs[2] = {&cert, &cert};
		wynik(DVCSRequest_CPKC(
			&cert, &privKey, certs, 2, 0, &outdvcscert));
	}
	wynik(free_GenBuf(&CertBuf));
	CertBuf = NULL;
	wynik(cryptDestroyContext(privKey));
	wynik(cryptDestroyContext(cert));

	wynik(cryptEnd());
	{
		FILE *plik = fopen("dvcsreq.sig", "wb");
		fwrite(outdvcs->buf, 1, outdvcs->size, plik);
		fclose(plik);
		plik = fopen("dvcsreqcert.sig", "wb");
		fwrite(outdvcscert->buf, 1, outdvcscert->size, plik);
		fclose(plik);
	}
	{
		char buf[4096];
		int s;
		asn_dec_rval_t rval;
		FILE *plik = fopen("dvcsreq_in.bin", "rb");
		s = fread(buf, 1, 4096, plik);
		fclose(plik);
		printf("%d\n", s);

	}

}
