#include <stdio.h>
#ifdef _WIN32
#	include <io.h>
#endif
#include <sys/types.h>
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
	GenBuf_t *addoutbuf = NULL;
	GenBuf_t *addsig = NULL;
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
	wynik(CMSSignDataAddSignature(
		&cert, &privKey, NULL, NULL, outbuf, CMSSIGN_HASH_MD5, &addsig));

	wynik(free_GenBuf(&CertBuf));
	CertBuf = NULL;
	wynik(cryptDestroyContext(privKey));
	wynik(cryptDestroyContext(cert));

	wynik(read_GenBuf_from_file("cert.der", &CertBuf));
	wynik(cryptImportCert(CertBuf->buf, CertBuf->size, CRYPT_UNUSED, &cert));
	wynik(CMSSignDataAddCryptCert(&cert, outbuf, &addoutbuf));
	wynik(cryptDestroyContext(cert));

	wynik(cryptEnd());
	{
		FILE *plik = fopen("content.sig", "wb");
		fwrite(outbuf->buf, 1, outbuf->size, plik);
		fclose(plik);
		plik = fopen("addcontent.sig", "wb");
		fwrite(addoutbuf->buf, 1, addoutbuf->size, plik);
		fclose(plik);
		plik = fopen("addsig.sig", "wb");
		fwrite(addsig->buf, 1, addsig->size, plik);
		fclose(plik);
	}

}
/* 
int main(int argc, char *argv[])
{
	ContentInfo_t *contentInfo = NULL;
	SignedData_t *sigData = NULL;
	GenBuf_t *buf = NULL;
	int err;
	FILE *plik = NULL;

	contentInfo = malloc(sizeof(ContentInfo_t));
	memset(contentInfo, 0, sizeof(ContentInfo_t));

	sigData = malloc(sizeof(SignedData_t));
	memset(sigData, 0, sizeof(SignedData_t));
	// dodanie algorytmu do identyfikatorow
	{
		DigestAlgorithmIdentifier_t *digestAlg = NULL;
		digestAlg = malloc(sizeof(DigestAlgorithmIdentifier_t));
		memset(digestAlg, 0, sizeof(DigestAlgorithmIdentifier_t));
		wynik(AlgorithmIdentifier_set_OID(
			digestAlg,
			OID_sha_1,
			sizeof(OID_sha_1)/sizeof(*OID_sha_1)));
		wynik(SignedData_add_DigestAlgorithm(sigData, digestAlg));
	}
	// ustawienie wersji
	{
		SignedData_set_CMSVersion(sigData, CMSVersion_v1);
	}
	// dodanie certyfikatu
	{
		GenBuf_t *FileBuf = NULL;
		Certificate_t *cert = NULL;
//		cert = malloc(sizeof(Certificate_t));
//		memset(cert, 0, sizeof(Certificate_t));
//		printf("Cert %d\n", (unsigned int)cert);
		wynik(read_GenBuf_from_file("cert.der", &FileBuf));
		wynik(ber_decode(0, &asn_DEF_Certificate, &cert, FileBuf->buf, FileBuf->size));
		free_GenBuf(&FileBuf);
		{
			// zrobienie kopii obiektu- tu certyfikatu (przyklad)
		GenBuf_t *CopyBuf = NULL;
		Certificate_t *certcopy = NULL;
		wynik(asn_cloneContent(&asn_DEF_Certificate, cert, &certcopy));
		der_encode_X509_certificate(certcopy, &CopyBuf);
		wynik(write_GenBuf_to_file("copycert.der", CopyBuf));
		wynik(free_GenBuf(&CopyBuf));
		wynik(SignedData_add_Certificate(sigData, cert));
		}
//		return 0;
	}
	// dodanie certyfikatu 2
	{
		GenBuf_t *FileBuf = NULL;
		Certificate_t *cert = NULL;
		wynik(read_GenBuf_from_file("certX.der.cer", &FileBuf));
		wynik(ber_decode(0, &asn_DEF_Certificate, &cert, FileBuf->buf, FileBuf->size));
		free_GenBuf(&FileBuf);
		wynik(SignedData_add_Certificate(sigData, cert));
	}
	// dodanie zawartosci
	{
		unsigned long data_id[] = {1, 2, 840, 113549, 1, 7, 1};
		const char tekst[] = "Ala ma kota, kot ma Ale";
		GenBuf_t *buf = malloc(sizeof(GenBuf_t));
		buf->size = sizeof(tekst);
		buf->buf= malloc(sizeof(tekst));
		memcpy(buf->buf, tekst, sizeof(tekst));
//		wynik(SignedData_set_ContentType(sigData, data_id, sizeof(data_id)/sizeof(*data_id)));
//		wynik(SignedData_set_Content(sigData, buf));
		wynik(SignedData_set_Data(sigData, buf));
	}
	// utworzenie SignerInfo
	{
		// zaladuj certyfikat
		CRYPT_CERTIFICATE cert = 0;
		CRYPT_CONTEXT privKey = 0;
		CRYPT_CONTEXT sessKey = 0;
		GenBuf_t *CertBuf = NULL;
		GenBuf_t *KeyBuf = NULL;
		RSA *rsa = NULL;
		SignerInfo_t *signerInfo;

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
//		wynik(read_GenBuf_from_file("certX.der.key", &KeyBuf));
//		wynik(cryptCreateContext(&sessKey, CRYPT_UNUSED, CRYPT_ALGO_NONE));
//		wynik(cryptImportKey(KeyBuf->buf, KeyBuf->size, sessKey, privKey));
		signerInfo = malloc(sizeof(SignerInfo_t));
		memset(signerInfo, 0, sizeof(SignerInfo_t));
		wynik(SignerInfo_generate_Signature(signerInfo, &cert, &privKey, CMSSIGN_HASH_SHA1, buf));
		wynik(cryptDestroyContext(privKey));
		wynik(cryptDestroyContext(cert));
		wynik(cryptEnd());
		wynik(SignedData_add_SignerInfo(sigData, signerInfo));
	}
	// wrzucanie do pliku sameo signeddata
	err = der_encode_SignedData(sigData, &buf);
	printf("Kod powrotu: %d\n", err);
	printf("Rozmiar bufora: %d\n", buf->size);
	if ( err == 0 )
	{
		plik = fopen("data.sig", "wb");
		fwrite(buf->buf, 1, buf->size, plik);
		fclose(plik);
	}
	else
		return err;
	// wrzucanie do pliku content z signed data
	ContentInfo_put_SignedData(contentInfo, sigData);
	free_GenBuf(&buf);
	err = der_encode_ContentInfo(contentInfo, &buf);
	printf("Kod powrotu: %d\n", err);
	printf("Rozmiar bufora: %d\n", buf->size);
	if ( err == 0 )
	{
		plik = fopen("content.sig", "wb");
		fwrite(buf->buf, 1, buf->size, plik);
		fclose(plik);
	}
	else
		return err;

	return err;
}

*/