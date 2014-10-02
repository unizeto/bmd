#include <bmd/libbmdaa2/pki/AA_pki.h>
#include <bmd/libbmderr/libbmderr.h>


/************************
 * Funkcje OpenSSL	*
 ***********************/
long ReadEvpPkeyFromPEM_private(char *keyfilename, char *password, EVP_PKEY **key)
{
long err = 0;
FILE *fp = NULL;
EVP_PKEY *errp = NULL;
	if(keyfilename == NULL)
		return BMD_ERR_PARAM1;
	if(password == NULL)
		return BMD_ERR_PARAM2;
	if(key == NULL || *key != NULL)
		return BMD_ERR_PARAM3;

	fp = fopen(keyfilename, "r");
	if(fp == NULL){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in opening private key file. Filename: %s. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, keyfilename, err);
		return ERR_AA_OPEN_FILE;
	}
	/* Odczyt klucza prywatnego z pliku z hasłem (PEM) */
	errp = PEM_read_PrivateKey(fp, key, NULL, password);
	if(errp == NULL){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in reading private key. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		print_openssl_errors();
		return ERR_AA_READ_PEM_PRIVATE_KEY;
	}
	fclose(fp);fp=NULL;
	return err;
}

long SignBuffer(ANY_t *AnyBuf, ACSignatureEnum_t SignatureAlgorithm, 
			char *keyfilename, char *password, GenBuf_t **signaturebuf)
{
long err			= 0;
EVP_MD_CTX mdctx;
char *digest_algorithm	= "SHA1";
const EVP_MD *md	= NULL;
EVP_PKEY *private_key	= NULL;

long chunk_size = SIGNATURE_INPUT_READ_BUF_SIZE;
long offset		= 0;
long remaining_bytes	= 0;
long please_read	= 0;
long signature_size	= 0;
unsigned int ui_temp	= 0;

	if(AnyBuf == NULL || AnyBuf->buf == NULL || AnyBuf->size < 0)
		return BMD_ERR_PARAM1;
	if(SignatureAlgorithm < 0)
		return BMD_ERR_PARAM2;
	if(keyfilename == NULL)
		return BMD_ERR_PARAM3;
	if(password == NULL)
		return BMD_ERR_PARAM4;
	if(signaturebuf == NULL || *signaturebuf != NULL)
		return BMD_ERR_PARAM5;

	OpenSSL_add_all_algorithms();
	/* wczytaj klucz prywatny z pliku */
	err = ReadEvpPkeyFromPEM_private(keyfilename, password, &private_key);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in reading private key from PEM. Filename: %s. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, keyfilename, err);
			return err;
	}
	*signaturebuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*signaturebuf == NULL) return NO_MEMORY;
	memset(*signaturebuf, 0, sizeof(GenBuf_t));

	/* Ustaw string z algorytmem skrótu */
	switch(SignatureAlgorithm){
	case SIGNATURE_FAKED:
			(*signaturebuf)->size = 128;
			(*signaturebuf)->buf = (char *) malloc (128);
			if((*signaturebuf)->buf == NULL) return NO_MEMORY;
			memset((*signaturebuf)->buf, 0xFF, 128);
			if(*signaturebuf == NULL) return NO_MEMORY;
			return err;
			break;
	case SIGNATURE_SHAWITHRSA:
	case SIGNATURE_UNDEFINED:
	default:
			digest_algorithm = "SHA1";
			break;
	}
	/* pobierz obiekt algorytmu f.skrotu */
	md = EVP_get_digestbyname(digest_algorithm);
        if(!md) {
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error: unknown message digest. Algorithm: %s. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, digest_algorithm, err);
		return ERR_AA_OPENSSL_UNKNOWN_DIGEST;
	}
	/* Faza init */
	memset(&mdctx, 0, sizeof(EVP_MD_CTX));
        EVP_MD_CTX_init(&mdctx);
	err = EVP_SignInit_ex(&mdctx, md, NULL);
	if(!err){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in EVP_SignInit_ex. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_OPENSSL_EVP_INIT_STAGE_FAILED;
	}

	/* Faza update */
	offset = 0;
	please_read = 0;
	remaining_bytes = AnyBuf->size;
	PRINT_VDEBUG("Any->size = %d\n", AnyBuf->size);
	while(remaining_bytes > 0){
		if(remaining_bytes >= chunk_size) please_read = chunk_size;
		else please_read = remaining_bytes;

		PRINT_VDEBUG("offset = %li\t please_read = %li\n", offset, please_read);
		#ifndef WIN32 
		err = EVP_SignUpdate(&mdctx, (void *)(AnyBuf->buf)+offset, please_read);
#else
		err = EVP_SignUpdate(&mdctx, (EVP_MD_CTX *)(AnyBuf->buf)+offset, please_read);
#endif
		if(!err){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in EVP_SignUpdate. "
				"Recieved error code = %li.\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AA_OPENSSL_EVP_UPDATE_STAGE_FAILED;
		}
		if(remaining_bytes >= chunk_size){
			offset += chunk_size;
			remaining_bytes -= chunk_size;
		}
		else {
			offset += remaining_bytes;
			remaining_bytes = 0;
		}
	}
	/* pobierz rozmiar bufora podpisu */
	(*signaturebuf)->size = EVP_PKEY_size(private_key);
	PRINT_VDEBUG("Signature size: %li\n", (*signaturebuf)->size);
	(*signaturebuf)->buf = (char *) malloc (sizeof(char)*(*signaturebuf)->size);
	if(*signaturebuf == NULL) return NO_MEMORY;
	memset((*signaturebuf)->buf, 0, sizeof(char)* (*signaturebuf)->size);
		
	/* Faza final */
	ui_temp = signature_size;
	err = EVP_SignFinal(&mdctx, (unsigned char*)((*signaturebuf)->buf), &ui_temp, private_key);
	signature_size = ui_temp;
	if(!err){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in final stage of signing data. "
			"Recieved error code = %li.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_OPENSSL_EVP_FINAL_STAGE_FAILED;
	}
	(*signaturebuf)->size = signature_size;
        EVP_MD_CTX_cleanup(&mdctx);
	EVP_PKEY_free(private_key);
	return err;
}

void print_openssl_errors(void)
{
int flags, line;	/*Do openssl*/
char *data, *file;
long code;
	
	ERR_load_crypto_strings();
	code = ERR_get_error_line_data((const char **)&file, &line, (const char **)&data, &flags);

	while (code) {
		PRINT_DEBUG("error code: %li in %s line %i.\n", code, file, line);
		PRINT_DEBUG("Human readable string: %s\n", ERR_error_string(code, NULL));
		if (data && (flags & ERR_TXT_STRING))
		{
			PRINT_DEBUG("error data: %s\n", data);
		}
		code = ERR_get_error_line_data((const char **)&file, &line, (const char **)&data, &flags);
	}
	ERR_free_strings();
}


