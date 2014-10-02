#include <bmd/libbmdaa2/wrappers/AC_SignatureValue.h>
#include <bmd/libbmderr/libbmderr.h>

/*! UWAGA !*/
/* Jako ze biblioteka OpenSSL moze miec wycieki pamieci, do testow serwera AC pod katem		*/
/* wyciekow mozna uzyc w pliku konfiguracyjnym jako wartość zmiennej signature_algorithm	*/
/* stringu "faked" i wowczas w tej funcji generowanie podpisu					*/
/* przy uzyciu openssla jest wylaczone, a zamiast tego sa wstawiane na sztywno 2 bajty FF FF	*/

int AC_SignatureValue_create(ANY_t *DerInput, BIT_STRING_t *SignatureValue, ConfigData_t *ConfigData)
{
int err = 0;
GenBuf_t *SignatureGenBuf = NULL;

	if(DerInput == NULL)
		return BMD_ERR_PARAM1;
	if(SignatureValue == NULL)
		return BMD_ERR_PARAM2;
	if(ConfigData == NULL)
		return BMD_ERR_PARAM3;
	
	memset(SignatureValue,0,sizeof(BIT_STRING_t));
	err = SignBuffer(DerInput, 
			ConfigData->ACConfiguration->ACSignatureSection->choice_signature_format,
			ConfigData->ACConfiguration->ACSignatureSection->privatekey_filename,
			ConfigData->ACConfiguration->ACSignatureSection->privatekey_password,
			&SignatureGenBuf);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in signing Attribute Certificate. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	SignatureValue->buf = (uint8_t *)SignatureGenBuf->buf;
	SignatureValue->size = SignatureGenBuf->size;
	SignatureValue->bits_unused = 0;
	free(SignatureGenBuf); SignatureGenBuf = NULL;
	return err;
}

int AC_SignatureValue_create_alloc(ANY_t *DerInput, BIT_STRING_t *SignatureValue, ConfigData_t *ConfigData)
{
int err = 0;

	if(DerInput == NULL)
		return BMD_ERR_PARAM1;
	if(SignatureValue == NULL)
		return BMD_ERR_PARAM2;
	if(ConfigData == NULL)
		return BMD_ERR_PARAM3;

	memset(SignatureValue,0,sizeof(BIT_STRING_t));
	err = AC_SignatureValue_create(DerInput, SignatureValue, ConfigData);
	if(err < 0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating SignatureValue (BIT_STRING_t). "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}
