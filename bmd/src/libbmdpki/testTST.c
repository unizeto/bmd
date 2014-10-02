#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdts/libbmdts.h>

int main()
{
	GenBuf_t *file_content=NULL;
	GenBuf_t *hash=NULL;
	GenBuf_t *request=NULL;
	GenBuf_t *response=NULL;
	bmd_crypt_ctx_t *hash_context=NULL;
	bmd_crypt_ctx_t *sign_context=NULL;
	//asn_dec_rval_t dec_result;
	//asn_enc_rval_t  enc_result;
	//TimeStampResp_t *resp=NULL;
	GenBuf_t *buf_content=NULL;
	int iter=0;
	TSRespFeatures_t *parametry=NULL;
	
	struct timeval start;
	struct timeval stop;
	struct timezone tz1;
	struct timezone tz2;
	struct tm *start_tm=NULL;
	struct tm *stop_tm=NULL;
	
	memset(&start,0,sizeof(struct timeval));
	memset(&stop,0,sizeof(struct timeval));
	memset(&tz1,0,sizeof(struct timezone));
	memset(&tz2,0,sizeof(struct timezone));
	
	bmd_init();
	printf("-----testTST-----\nTen test wymaga pliku test.txt z dowolna zawartoscia oraz ts.pfx\n");
	
	bmd_set_ctx_hash(&hash_context,BMD_HASH_ALGO_SHA1);
	load_binary_content("test.txt", &file_content);
	bmd_hash_data(file_content, &hash_context, &hash, NULL);
	printf("TEST: wynik generowania requesta: %i\n", _bmd_create_ts_request(hash, BMD_HASH_ALGO_SHA1, &request));

	bmd_set_ctx_file("ts.pfx","12345678", 8, &sign_context);
	

	/*! start pomiaru czasu */	
	gettimeofday(&start, &tz1);
	start_tm=gmtime(&(start.tv_sec));
	printf("start: %2i,%6li\n",start_tm->tm_sec, (long)start.tv_usec );
	for(iter=0; iter<100; iter++)
	{
		response=NULL;
		bmd_generate_timestamp(request, sign_context, &response, NULL);
		free_gen_buf(&response);
	}
	gettimeofday(&stop, &tz2);
	stop_tm=gmtime(&(stop.tv_sec));
	printf("stop:  %2i,%6li\n", stop_tm->tm_sec, (long)stop.tv_usec);
	/*! koniec pomiaru czasu */	
	
	printf("TEST: wynik znakowania czasem: %i\n", bmd_generate_timestamp(request, sign_context, &response, NULL));
	
	if(response != NULL)
	{
		printf("TEST: jest respons\n");
		printf("\nTEST: wynik parsowania odpowiedzi: %i\n", Parse_TimeStampResponse(response, &parametry));
		printf("TEST: wersja: %li \n", parametry->version);
		printf("TEST: oid polityki TSA: %s \n", parametry->TSA_policyId);
		printf("TEST: oid funkcji skrotu: %s ; wartosci skrotu nie wypisuje\n", parametry->hash_alg_oid);
		printf("TEST: czas wygenerowania: %s", ctime((const time_t*)&(parametry->genTime)));
		printf("TEST: numer seryjny: %li \n", parametry->serial);
		printf("TEST: nazwa TSA %s \n", parametry->tsa);
				
		Destroy_TSRespFeatures(&parametry);
		/*
		dec_result=ber_decode(0, &asn_DEF_TimeStampResp, (void**)&resp, response->buf, response->size);
		if(dec_result.code != RC_OK)
		{
			printf("lipa przy dekodowaniu\n");
			return -1;
		}
		
		
		enc_result=der_encode( &asn_DEF_ContentInfo, resp->timeStampToken, 0, 0);
		if(enc_result.encoded < 0)
		{
			printf("dupa1\n");
			return -1;
		}
		buf_content=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		buf_content->size=enc_result.encoded;
		buf_content->buf=(uint8_t*)calloc(buf_content->size, sizeof(uint8_t) );
		if(buf_content->buf == NULL)
		{
			printf("cos z pamiecia\n");
			return -1;
		}
		enc_result=der_encode_to_buffer(&asn_DEF_ContentInfo, resp->timeStampToken, buf_content->buf, (size_t*)&(buf_content->size));
		if(enc_result.encoded < 0)
		{
			printf("dupa2\n");
			return -1;
		}
		//set_gen_buf2(resp->timeStampToken->content.buf, resp->timeStampToken->content.size, &(buf_resp));
		bmd_save_buf(buf_content, "timestamp1.tsr");
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		*/
	}
	else
	{
		printf("TEST: Nie ma responsa\n");
	}
	
	
	free_gen_buf(&hash);
	free_gen_buf(&buf_content);
	free_gen_buf(&file_content);
	free_gen_buf(&request);
	free_gen_buf(&response);
	bmd_ctx_destroy(&hash_context);
	bmd_ctx_destroy(&sign_context);
	
	bmd_end();

	return 0;
}
