#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/common/bmd-openssl.h>
#include <bmd/common/bmd-const.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmderr/libbmderr.h>
int main(int argc,char **argv)
{
	unsigned char pass[12]={0x5E,0x78,0x50,0x2F,0x39,0x74,0x5D,0x46,0x61,0x52,0x67,0x36};
	int passlen=12;

	GenBuf_t *input=NULL;
	GenBuf_t *output=NULL;
	char *output_filename=NULL;
	bmd_crypt_ctx_t *ctx=NULL;

	printf("\nGenerator licencji SOE (wersja 1.1)\n\n");

	OpenSSL_add_all_algorithms();

	if(argc == 2)
	{
		printf("Generowanie (szyfrowanie) licencji\n");
		BMD_FOK(bmd_gen_sym_ctx_with_pass(BMD_CRYPT_ALGO_DES3,BMD_HASH_ALGO_SHA1,(char *)pass,passlen,&ctx));
		BMD_FOK(bmd_load_binary_content(argv[1],&input));
		BMD_FOK(bmd_symmetric_encrypt(input,&ctx,&output,NULL));
		asprintf(&output_filename,"%s.enc",argv[1]);
		bmd_save_buf(output,output_filename);
	}
	else if(argc == 3 && strcmp(argv[1], "-d") == 0)
	{
		printf("Deszyfrowanie licencji\n");
		BMD_FOK(bmd_gen_sym_ctx_with_pass(BMD_CRYPT_ALGO_DES3,BMD_HASH_ALGO_SHA1,(char *)pass,passlen,&ctx));
		BMD_FOK(bmd_load_binary_content(argv[2],&input));
		BMD_FOK(bmd_symmetric_decrypt(input, 0 , &ctx,&output,NULL));
		asprintf(&output_filename,"%s.dec",argv[2]);
		bmd_save_buf(output,output_filename);
	}
	else
	{
		printf("Niepoprawne wywolanie programu !\n");
		printf("Szyfrowanie (plik wynikowy ma dodane rozszerzenie .enc):\n bmd_mkcnf.exe plik_zrodla_licencji\n\n");
		printf("Deszyfrowanie (plik wynikowy ma dodane rozszerzenie .dec):\n bmd_mkcnf.exe -d plik_licencji.enc\n");
		return -1;
	}

	bmd_ctx_destroy(&ctx);
	EVP_cleanup();

	return 0;
}
