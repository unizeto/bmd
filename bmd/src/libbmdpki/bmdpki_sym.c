#include<bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmderr/libbmderr.h>

long bmdpki_symmetric_encrypt(bmd_crypt_ctx_t *ctx_enc, GenBuf_t *data, long is_last, GenBuf_t **output)
{
long status=0;
char *tmp_buf=NULL; /* docelowo bufor z wynikiem */
GenBuf_t *tmp_output=NULL;	/*GenBuf_t z wynikiem (wypelniany na podstawie tmp_buf)*/
long iter=0;
char *single_round_first_part=NULL;
char *single_round_rest=NULL;
long update_size=0;
long final_size=0;
int temp;

	PRINT_INFO("LIBBMDPKIINF BMD symmetric encryption\n");
	if(ctx_enc == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ctx_enc->sym == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( ctx_enc->sym->key == NULL || ctx_enc->sym->IV == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(data == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(output == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(*output != NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	/*jesli przekazany jest pusty genbuf do zaszyfrowania, to moze tak bac tylko w przypadku
		szyfrowania od razu calosci (nie w strumieniu)*/
	if(data->size == 0)
	{
		if( (ctx_enc->sym->sym_stage != BMD_SYM_STAGE_FIRST) || (is_last != BMDPKI_SYM_LAST_CHUNK) )
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-10);
			return -10;
		}
	}

	/*jesli podana pierwsza porcja danych, to inicjowany kontekst szyfrujacy openSSL*/
	if(ctx_enc->sym->sym_stage == BMD_SYM_STAGE_FIRST)
	{
		status=EVP_EncryptInit(&( ctx_enc->sym->sym_ctx ), EVP_des_ede3_cbc(), \
		(unsigned char*)(ctx_enc->sym->key->buf), (unsigned char*)(ctx_enc->sym->IV->buf));
		if(status != 1)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-4);
			return -4;
		}
		status=EVP_CIPHER_CTX_set_key_length(&( ctx_enc->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
		if(status != 1)
		{
			EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-5);
			return -5;
		}
		/*zaznaczam, ze juz pierwsza runda (inicjowanie) sie odbyla*/
		ctx_enc->sym->sym_stage = BMD_SYM_STAGE_NORMAL;
	}

	tmp_buf=(char *)calloc(data->size+EVP_MAX_KEY_LENGTH, 1);
	if(tmp_buf == NULL)
	{
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
		return -6;
	}

	/*ten if tylko dla przypadku jednorundowego szyfrowania danych 0 wielkosci
		Wiadomo, ze warunki sa spelnione, jesli nie wyszedl wczesniej z wartoscia -10; */
	if(data->size == 0)
	{
		/*od razu finalizacja - sam padding bedzie na wyjsciu */
		temp = update_size;
		status=EVP_EncryptFinal(&( ctx_enc->sym->sym_ctx ), (unsigned char*)tmp_buf, &temp);
		update_size = temp;
		if(status != 1)
		{
			free(tmp_buf);
			EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-11);
			return -11;
		}

		/*tworzenie wyjsciowego genbufa*/
		tmp_output=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		if(tmp_output == NULL )
		{
			free(tmp_buf);
			EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
			return -6;
		}
		tmp_output->buf=(char*)calloc(update_size, 1);
		if(tmp_output->buf == NULL )
		{
			free(tmp_output);
			free(tmp_buf);
			EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
			return -6;
		}
		tmp_output->size=update_size;

		for(iter=0; iter<update_size; iter++)
		{
			tmp_output->buf[iter]=tmp_buf[iter];
		}
		free(tmp_buf);
		*output=tmp_output;
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));

		return 0;
	}

	/* niezaleznie, ktora porcja danych została podana, szyfrowanie sie na niej odbywa*/
	temp = update_size;
	status=EVP_EncryptUpdate( &( ctx_enc->sym->sym_ctx ), (unsigned char*)tmp_buf, &temp, (unsigned char*)(data->buf), \
	data->size);
	update_size = temp;
	if(status != 1)
	{
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-7);
		return -7;
	}
	/*po zaszyfrowaniu porcji danych, przypisany jest do final_size rozmiar otrzymane porcji szyforgramu */
	final_size=update_size;

	/* jesli podawana jest ostatnia porcja danych, musi miec miejsce finalizowanie szyfrowania */
	if(is_last == BMDPKI_SYM_LAST_CHUNK)
	{
		/* moga tutaj nastapic dwa przypadki, ktore musza byc rozpatrywane ze wzgledu na fakt,
			iz funkcja ta szyfruje nie tylko strumieniowo, ale rowniez jednym wywolaniem
			cale dane
		1) jesli wynik szyfracji przekazanych danych dal niepusty wynik
			to nastepujaca finalizacja moze dac jeszcze jakies zaszyfrowane dane i wowczas
			nalezy je dolaczyc i calosc trakotwac jako wynik
		 */
		/*jesli szyfracja dala niepusty wynik*/
		if(final_size != 0)
		{
			/*zapamietuje pod wskaznikiem single_round_first_part wynik szyfrowania przed finalizacja*/
			single_round_first_part=tmp_buf;
			tmp_buf=NULL;
			single_round_rest=(char *)calloc(data->size+EVP_MAX_KEY_LENGTH, 1);
			if(single_round_rest == NULL)
			{
				free(single_round_first_part);
				EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
				PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
				return -6;
			}
			/*finalizacja*/
			temp = update_size;
			status=EVP_EncryptFinal(&( ctx_enc->sym->sym_ctx ), (unsigned char*)single_round_rest, &temp);
			update_size = temp;
			if(status != 1)
			{
				free(single_round_rest);
				free(single_round_first_part);
				EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
				PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-8);
				return -8;
			}
			/*jesli finalizacja dala niepusty wynik , to nalezy go dolaczyc do porzednio zaszyfrowanych danych*/
			if(update_size != 0)
			{
				tmp_buf=(char*)calloc(final_size+update_size, 1);
				if(tmp_buf == NULL)
				{
					free(single_round_rest);
					free(single_round_first_part);
					EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
					PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
					return -6;
				}
				for(iter=0; iter<final_size; iter++)
				{
					tmp_buf[iter]=single_round_first_part[iter];
				}
				for(iter=iter; iter<final_size+update_size; iter++)
				{
					tmp_buf[iter]=single_round_rest[iter-final_size];
				}
				free(single_round_first_part);
				free(single_round_rest);
			}
			/*jesli finalizacja dala pusty wynik, to wczesniej zaszyfrowane dane sa juz gotowym wynikiem*/
			else
			{
				tmp_buf=single_round_first_part;
				single_round_first_part=NULL;
				free(single_round_rest);
			}
		}
		/* 2) jesli szyfracja dala pusty wynik to finalizujemy i rezultat samej finalizacji jest wynikiem*/
		else
		{
			temp = update_size;
			status=EVP_EncryptFinal(&( ctx_enc->sym->sym_ctx ), (unsigned char*)tmp_buf, &temp);
			update_size = temp;
			if(status != 1)
			{
				free(tmp_buf);
				EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
				PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-9);
				return -9;
			}
		}
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
		final_size+=update_size;
	}

	/*tworzenie wyjsciowego genbufa*/
	tmp_output=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(tmp_output == NULL )
	{
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
		return -6;
	}
	tmp_output->buf=(char*)calloc(final_size, 1);
	if(tmp_output->buf == NULL )
	{
		free(tmp_output);
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_enc->sym->sym_ctx ));
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",-6);
		return -6;
	}
	tmp_output->size=final_size;

	for(iter=0; iter<final_size; iter++)
	{
		tmp_output->buf[iter]=tmp_buf[iter];
	}

	free(tmp_buf);

	*output=tmp_output;

	return 0;
}


long bmdpki_symmetric_decrypt(	bmd_crypt_ctx_t *ctx_dec,
						GenBuf_t *enc_buf,
						long is_last,
						GenBuf_t **output)
{
long status					=0;
long final_size				=0;
int temp					=0;
long update_size				=0;
long iter					=0;
char *tmp_buf				=NULL; /* docelowo bufor z wynikiem */
GenBuf_t *tmp_output			=NULL;
char *single_round_first_part		=NULL;
char *single_round_rest			=NULL;

	PRINT_INFO("LIBBMDPKIINF BMD symmetric decryption\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (ctx_dec == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ctx_dec->sym == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ( ctx_dec->sym->key==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ctx_dec->sym->IV == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (enc_buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (output == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*output != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (enc_buf->size == 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*jesli podana pierwsza porcja szyfrogramu, to inicjowany kontekst deszyfrujacy openSSL*/
	if(ctx_dec->sym->sym_stage == BMD_SYM_STAGE_FIRST)
	{
		status=EVP_DecryptInit(&( ctx_dec->sym->sym_ctx ), EVP_des_ede3_cbc(), \
		(unsigned char*)(ctx_dec->sym->key->buf), (unsigned char*)(ctx_dec->sym->IV->buf));
		if(status != 1)	{	BMD_FOK(-4);	}

		status=EVP_CIPHER_CTX_set_key_length(&( ctx_dec->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
		if(status != 1)
		{
			EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
			BMD_FOK(-5);
		}
		/*zaznaczam, ze juz pierwsza runda (inicjowanie) sie odbyla*/
		ctx_dec->sym->sym_stage = BMD_SYM_STAGE_NORMAL;
	}

	tmp_buf=(char *)calloc(enc_buf->size+EVP_MAX_KEY_LENGTH, 1);
	if(tmp_buf == NULL)
	{
		EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
		BMD_FOK(-6);
	}

	/* niezaleznie, ktora porcja szyfrogramu została podana, deszyfrowanie sie na niej odbywa*/
	temp = update_size;
	status=EVP_DecryptUpdate( &( ctx_dec->sym->sym_ctx ), (unsigned char*)tmp_buf, &temp, \
	(unsigned char*)(enc_buf->buf), enc_buf->size);
	update_size = temp;
	if(status != 1)
	{
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
		BMD_FOK(-7);
	}

	/*po zdeszyfrowaniu porcji szyfrogramu, przypisany jest do final_size rozmiar otrzymanych danych */
	final_size=update_size;

	/* jesli podawana jest ostatnia porcja danych, musi miec miejsce finalizowanie deszyfrowania */
	if(is_last == BMDPKI_SYM_LAST_CHUNK)
	{

		/* moga tutaj nastapic dwa przypadki, ktore musza byc rozpatrywane ze wzgledu na fakt,
			iz funkcja ta deszyfruje nie tylko strumieniowo, ale rowniez jednym wywolaniem
			caly szyfrogram
		1) jesli wynik deszyfracji przekazanych zaszyfrowanych danych dal niepusty wynik
			to nastepujaca finalizacja moze dac jeszcze jakies zdeszyfrowane dane i wowczas
			nalezy je dolaczyc i calosc trakotwac jako wynik
		 */
		/*jesli deszyfracja dala niepusty wynik*/
		if(final_size != 0)
		{

			/*zapamietuje pod wskaznikiem single_round_first_part wynik deszyfrowania przed finalizacja*/
			single_round_first_part=tmp_buf;
			tmp_buf=NULL;
			single_round_rest=(char *)calloc(enc_buf->size+EVP_MAX_KEY_LENGTH, 1);
			if(single_round_rest == NULL)
			{
				free(single_round_first_part);
				EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
				BMD_FOK(-6);
			}
			/*finalizacja*/

			temp = update_size;
			status=EVP_DecryptFinal(&( ctx_dec->sym->sym_ctx ), (unsigned char*)single_round_rest, &temp);
			update_size = temp;
			if(status != 1)
			{
				long err=ERR_get_error();
				char err_str[1024];
				char *error=NULL;
				memset(err_str,0,1024);
				error=ERR_error_string(err,err_str);
				PRINT_DEBUG("MSILEWICZ_OPENSSL_ERR %li %s\n",ERR_get_error(),error);
				perror("error ");
				free(single_round_rest);
				free(single_round_first_part);
				EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
				BMD_FOK(-8);
			}

			/*jesli finalizacja dala niepusty wynik , to nalezy go dolaczyc do porzednio zdeszyfrowanych danych*/
			if(update_size != 0)
			{

				tmp_buf=(char*)calloc(final_size+update_size, 1);
				if(tmp_buf == NULL)
				{
					free(single_round_rest);
					free(single_round_first_part);
					EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
					BMD_FOK(-6);
				}

				for(iter=0; iter<final_size; iter++)
				{
					tmp_buf[iter]=single_round_first_part[iter];
				}

				for(iter=iter; iter<final_size+update_size; iter++)
				{
					tmp_buf[iter]=single_round_rest[iter-final_size];
				}

				free(single_round_first_part);
				free(single_round_rest);
			}
			/*jesli finalizacja dala pusty wynik, to wczesniej zdeszyfrowane sa juz gotowym wynikiem*/
			else
			{

				tmp_buf=single_round_first_part;
				single_round_first_part=NULL;
				free(single_round_rest);
			}
		}
		/* 2) jesli deszyfracja dala pusty wynik to finalizujemy i rezultat samej finalizacji jest wynikiem*/
		else
		{

			temp = update_size;
			status=EVP_DecryptFinal(&( ctx_dec->sym->sym_ctx ), (unsigned char*)tmp_buf, &temp);
			update_size = temp;
			if(status != 1)
			{
				free(tmp_buf);
				EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
				BMD_FOK(-9);
			}
		}

		EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
		final_size+=update_size;
	}

	/*tworzenie wyjsciowego genbufa*/
	tmp_output=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(tmp_output == NULL )
	{
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
		BMD_FOK(-6);
	}

	tmp_output->buf=(char*)calloc(final_size, 1);
	if(tmp_output->buf == NULL )
	{
		free(tmp_output);
		free(tmp_buf);
		EVP_CIPHER_CTX_cleanup(&( ctx_dec->sym->sym_ctx ));
		BMD_FOK(-6);
	}

	tmp_output->size=final_size;

	for(iter=0; iter<final_size; iter++)
	{

		tmp_output->buf[iter]=tmp_buf[iter];
	}

	free(tmp_buf);
	*output=tmp_output;

	return 0;
}

