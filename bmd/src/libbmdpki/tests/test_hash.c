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
GenBuf_t *input=NULL;
GenBuf_t *output=NULL;
GenBuf_t *chunk=NULL;

bmd_crypt_ctx_t *ctx_hash=NULL;
bmd_set_ctx_hash(&ctx_hash, BMD_HASH_ALGO_SHA1);
ctx_hash->hash->hash_params=BMD_HASH_MULTI_ROUND;

if (bmd_load_binary_content("dane.txt", &input) != BMD_OK)
{
	printf("Brak pliku z danymi do wyliczenia skrótu.\n");
	return -1;
}
printf("input size: %i\n", input->size);
printf("\n\n-----------\n\n");

genbuf_chunk(input, 0, 8 , &chunk);
printf("chunk size 1: %i\n", chunk->size);
status=bmd_hash_data(chunk, &ctx_hash, &output, NULL);
printf("status step 1: %i\n", status);
printf("output 1: %p\n", output);
//printf("output size 1: %i\n", output->size);
bmd_save_buf(output, "hash1.txt");
free_gen_buf(&output);
free_gen_buf(&chunk);
printf("\n\n-----------\n\n");
genbuf_chunk(input, 9, 19 , &chunk);
printf("chunk size 2: %i\n", chunk->size);
status=bmd_hash_data(chunk, &ctx_hash, &output, NULL);
printf("status step 2: %i\n", status);
printf("output 2: %p\n", output);
//printf("output size 2: %i\n", output->size);
bmd_save_buf(output, "hash2.txt");
free_gen_buf(&output);
free_gen_buf(&chunk);
printf("\n\n------final-----\n\n");
status=bmd_hash_data(NULL, &ctx_hash, &output, NULL);
printf("status step 3: %i\n", status);
printf("output size 3: %i\n", output->size);
bmd_save_buf(output, "hash3.txt");
free_gen_buf(&output);
free_gen_buf(&chunk);


bmd_ctx_destroy(&ctx_hash);
free_gen_buf(&input);

return 0;
}
