#include <stdio.h>
#ifdef _WIN32
#	include <io.h>
#endif
#include <sys/types.h>
#include <libbmdcms.h>

#define wynik(x) printf("[%04d] Wynik: %d\n", __LINE__, x);
#define NUMCERTS 1

int main(int argc, char **argv)
{
	const char tekst[] = "Ala ma kota, kot ma Ale";
	GenBuf_t *buf = malloc(sizeof(GenBuf_t));
	GenBuf_t *CertBuf = NULL;
	GenBuf_t *out = NULL;
	CRYPT_CERTIFICATE cert = 0;
	CRYPT_CERTIFICATE **certtab = 0;
	int i;


	buf->size = sizeof(tekst);
	buf->buf= malloc(sizeof(tekst));
	memcpy(buf->buf, tekst, sizeof(tekst));

	wynik(cryptInit());

	wynik(read_GenBuf_from_file("certX.der.cer", &CertBuf));
	wynik(cryptImportCert(CertBuf->buf, CertBuf->size, CRYPT_UNUSED, &cert));
	wynik(free_GenBuf(&CertBuf));
	CertBuf = NULL;
	certtab = malloc(sizeof(CRYPT_CERTIFICATE*)*NUMCERTS);
	certtab[0] = &cert;
	wynik(CMSEnvelopedData(certtab, NUMCERTS, buf, CRYPT_ALGO_3DES, 0, &out));

	for ( i=0; i<NUMCERTS; i++ )
		wynik(cryptDestroyContext(*certtab[i]));
	wynik(cryptEnd());
	{
		FILE *plik = fopen("koperta.env", "wb");
		fwrite(out->buf, 1, out->size, plik);
		fclose(plik);
	}
}
