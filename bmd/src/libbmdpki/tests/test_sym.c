#include <bmd/libbmdpki/libbmdpki.h>

int genbuf_chunk(GenBuf_t *gb, int index_start, int index_stop , GenBuf_t **out)
{
	int iter=0;
	GenBuf_t *tmp_out=NULL;

	if(gb == NULL)
	{
		return -1;
	}
	if(gb->buf == NULL)
	{
		return -2;
	}
	if(gb->size == 0)
	{
		return -3;
	}

	tmp_out=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(tmp_out == NULL)
		{ return -4; }
	tmp_out->buf=(unsigned char *)calloc(index_stop-index_start+1, sizeof(unsigned char));
	if(tmp_out->buf == NULL)
	{
		free(tmp_out);
		return -5;
	}
	for(iter=0; iter<=index_stop-index_start; iter++)
	{
		tmp_out->buf[iter]=gb->buf[index_start+iter];
	}

	tmp_out->size=index_stop-index_start+1;
	*out=tmp_out;

	return 0;
}



int main()
{
int status=0;
GenBuf_t *klucz=NULL;
GenBuf_t *IV=NULL;
bmd_crypt_ctx_t *ctx_enc=NULL;
bmd_crypt_ctx_t *ctx_dec=NULL;

GenBuf_t *input=NULL;
GenBuf_t *output=NULL;
FILE *plik=NULL;
int update_size=0;
GenBuf_t *gb_chunk=NULL;
unsigned char duzy_bufor[100];
int rozmiar_enc=0;

/** ustwienie kontekstu szyfrujacego i deszyfrujacego o tym samym kluczu i IV, tryb wielorundowy */
printf("\n --- ustwienie kontekstu szyfrujacego i deszyfrujacego ---\n\n");
/*ustawiam kontekst szyfrujący z loso wygenerowanym kluczem i IV*/
status=bmd_set_ctx_sym(&ctx_enc,BMD_CRYPT_ALGO_DES3,NULL,NULL);
if(status != BMD_OK)
{
	printf("puf 0\n");
	return -1;
}
/*ustawiam wielorundowy tryb szyfrowania*/
ctx_enc->sym->sym_params=BMD_SYM_MULTI_ROUND;
/* kopiuje klucz szyfrujacy */
status=set_gen_buf2(ctx_enc->sym->key->buf, ctx_enc->sym->key->size, &klucz);
if(status != BMD_OK)
{
	printf("puf 1\n");
	return -1;
}
/*kopiuje IV wykorzystany przy szyfrowaniu*/
status=set_gen_buf2(ctx_enc->sym->IV->buf, ctx_enc->sym->IV->size, &IV);
if(status != BMD_OK)
{
	printf("puf 2\n");
	return -1;
}
/*ustawiam kontekst deszyfrujący*/
status=bmd_set_ctx_sym(&ctx_dec,BMD_CRYPT_ALGO_DES3,NULL,NULL);
if(status != BMD_OK)
{
	printf("puf 3\n");
	return -1;
}

/*czyszczę loso wygenerowany klucz i IV kontekstu deszyfrującego*/
free_gen_buf( &(ctx_dec->sym->key) );
free_gen_buf( &(ctx_dec->sym->IV) );
/*ustawiam w kontekscie deszyfrującym klucz i IV z kontekstu szyfrujacego  */
ctx_dec->sym->key=klucz;
ctx_dec->sym->IV=IV;

/**   szyforwanie <---> deszyfrowanie */
printf("\n---- szyfrowanie ----\n");

/* laduje szyfrowany plik*/
if (bmd_load_binary_content("dane.txt", &input) != BMD_OK)
{
	printf("brak pliku dane.txt!!!\n");
	return -1;
}
plik = fopen("zaszyfrowane.txt","w");
/*
genbuf_chunk(input, 0, 7 , &gb_chunk);
status=bmdpki_symmetric_encrypt(ctx_enc, gb_chunk, BMDPKI_SYM_NOT_LAST_CHUNK, &output);
printf("step 1: %i\n", status);
printf("step 1 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&output);
free_gen_buf(&gb_chunk);

genbuf_chunk(input, 7, 15 , &gb_chunk);
status=bmdpki_symmetric_encrypt(ctx_enc, gb_chunk, BMDPKI_SYM_NOT_LAST_CHUNK, &output);
printf("step 2: %i\n", status);
printf("step 2 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&output);
free_gen_buf(&gb_chunk);

genbuf_chunk(input, 8, 15 , &gb_chunk);
*/
status=bmdpki_symmetric_encrypt(ctx_enc, input, BMDPKI_SYM_LAST_CHUNK, &output);
printf("step 3: %i\n", status);
printf("step 3 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&output);
//free_gen_buf(&gb_chunk);



/*test - bezposrednio openSSL*/
/*
status=EVP_EncryptInit(&( ctx_enc->sym->sym_ctx ), EVP_des_ede3_cbc(), ctx_enc->sym->key->buf, ctx_enc->sym->IV->buf);
printf("status init: %i\n", status);
status=EVP_CIPHER_CTX_set_key_length(&( ctx_enc->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
printf("status set key length: %i\n", status);

genbuf_chunk(input, 0, 6 , &gb_chunk);
status=EVP_EncryptUpdate(&( ctx_enc->sym->sym_ctx ),duzy_bufor,&rozmiar_enc,gb_chunk->buf,gb_chunk->size);
printf("status update 1: %i\n", status);
printf("rozmiar enc 1: %i\n", rozmiar_enc);
fwrite(duzy_bufor, 1, rozmiar_enc, plik);
free_gen_buf(&gb_chunk);
	
genbuf_chunk(input, 7, 23, &chunk_enc);
status=EVP_EncryptUpdate(&( ctx_enc->sym->sym_ctx ),duzy_bufor,&rozmiar_enc,gb_chunk->buf,gb_chunk->size);
printf("status update 2: %i\n", status);
printf("rozmiar enc 2: %i\n", rozmiar_enc);
fwrite(duzy_bufor, 1, rozmiar_enc, plik);
free_gen_buf(&gb_chunk);

status=EVP_EncryptFinal(&( ctx_enc->sym->sym_ctx ), duzy_bufor,&rozmiar_enc );
printf("status final : %i\n", status);
printf("rozmiar final : %i\n", rozmiar_enc);
fwrite(duzy_bufor, 1, rozmiar_enc, plik);

EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
*/

free_gen_buf(&input);
fclose(plik);
printf("\n\n--- koniec szyfrowania ----\n\n");
bmd_ctx_destroy(&ctx_enc);



/** deszyfrowanie */
printf("\n\n---- deszyfrowanie ----\n\n");
bmd_load_binary_content("zaszyfrowane.txt", &input);
plik=fopen("zdeszyfrowane.txt","w");

genbuf_chunk(input, 0, 7 , &gb_chunk);
printf("rozmiar calego szyfrogramu: %i\n", input->size);
printf("step 1: %i\n", bmdpki_symmetric_decrypt(ctx_dec, gb_chunk, BMDPKI_SYM_NOT_LAST_CHUNK, &output));
printf("step 1 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&gb_chunk);
free_gen_buf(&output);
/*
genbuf_chunk(input, 8, 15 , &gb_chunk);
printf( "step 2: %i\n", bmdpki_symmetric_decrypt(ctx_dec, gb_chunk, BMDPKI_SYM_NOT_LAST_CHUNK, &output));
printf("step 2 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&gb_chunk);
free_gen_buf(&output);
*/

genbuf_chunk(input, 8, 15 , &gb_chunk);
printf( "step 3: %i\n", bmdpki_symmetric_decrypt(ctx_dec, gb_chunk, BMDPKI_SYM_LAST_CHUNK, &output));
printf("step 3 size: %i\n", output->size);
fwrite(output->buf, 1, output->size, plik);
free_gen_buf(&gb_chunk);
free_gen_buf(&output);


/*testy besposrednio na opneSSL*/
/*
status=EVP_DecryptInit(&( ctx_dec->sym->sym_ctx ), EVP_des_ede3_cbc(), ctx_dec->sym->key->buf, ctx_dec->sym->IV->buf);
printf("init: %i\n", status);
status=EVP_CIPHER_CTX_set_key_length(&( ctx_dec->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
printf("key length: %i\n", status);

genbuf_chunk(input, 0, 8 , &gb_chunk);
status=EVP_DecryptUpdate( &( ctx_dec->sym->sym_ctx ), duzy_bufor, &update_size, gb_chunk->buf, gb_buf->size);
printf("update: %i\n", status);
printf("update size %i\n", update_size);
fwrite(duzy_bufor, 1, update_size, plik);
free_gen_buf(&gb_chunk);

genbuf_chunk(input, 9, 15 , &gb_chunk);
status=EVP_DecryptUpdate( &( ctx_dec->sym->sym_ctx ), duzy_bufor, &update_size, gb_chunk->buf, gb_chunk->size);
printf("update: %i\n", status);
printf("update size: %i\n", update_size);
fwrite(duzy_bufor, 1, update_size, plik);
free_gen_buf(&gb_chunk);

genbuf_chunk(input, 16, 23 , &gb_chunk);
status=EVP_DecryptUpdate( &( ctx_dec->sym->sym_ctx ), duzy_bufor, &update_size, gb_chunk->buf, gb_chunk->size);
printf("update: %i\n", status);
printf("update size: %i\n", update_size);
fwrite(duzy_bufor, 1, update_size, plik);
free_gen_buf(&gb_chunk);

status=EVP_DecryptFinal(&( ctx_dec->sym->sym_ctx ), duzy_bufor, &update_size);
printf("final: %i\n", status);
printf("final size: %i\n", update_size);
fwrite(duzy_bufor, 1, update_size, plik);

EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
*/


fclose(plik);
printf("\n\n--- koniec deszyfrowania ----\n\n");
bmd_ctx_destroy(&ctx_dec);
free_gen_buf(&input);

return 0;
}
