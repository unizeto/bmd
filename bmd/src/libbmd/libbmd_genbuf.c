#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdbase64/libbmdbase64.h>

/**
 * Funkcja alokuje i ustawia zawartość bufora generycznego na podstawie
 * danych wejściowych.
 *
 * \param[in] content Binarna zawartość.
 *
 * \param[in] length Długość binarnej zawartości.
 *
 * \param[out] buf bufor generyczny z ustawioną zawartością.
 *
 * \retval BMD_OK Funkcja zakończyła się bez błędów.
 * \retval -1 Nie udało się ustawić bufora generycznego.
 * */
long bmd_genbuf_set(const char *content,const long length,GenBuf_t **buf)
{

	PRINT_INFO("LIBBMDINF Allocating and initializing genbuf\n");

	BMD_FOK(set_gen_buf((char *)content,(long *)&length,buf));
	return BMD_OK;
}

/**
 * Funkcja zwalnia bufor generyczny. Po wywołaniu tej funkcji pod adres
 * wskazujący na bufor przypisywana jest wartość NULL.
 *
 * \param[in] buf Adres bufora generycznego do zwolnienia.
 *
 * \return Kod błędu lub wartość BMD_OK.
 * */
long bmd_genbuf_free(GenBuf_t **buf)
{
	PRINT_INFO("LIBBMDINF bmd_genbuf_free\n");

	free_gen_buf(buf);
	return BMD_OK;
}

long bmd_genbuf_hex_dump(GenBuf_t *input,char **output,char sep,long uppercase)
{
long flag			=-1;
long i				= 0;
char octet[4];
unsigned char uc_temp 		= 0;
unsigned int ui_temp		= 0;

	PRINT_INFO("LIBBMDINF bmd_genbuf_hex_dump\n");

	if(sep==0)
	{
		flag=0;
	}
	else
	{
		flag=1;
	}

	if(input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*output)=(char *)malloc( (input->size*3 + 1) * sizeof(char) );
	if(*output == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset((*output), 0, (input->size*3 + 1 ) * sizeof(char) );

	memset(octet, 0, sizeof(char)*4);
	for(i=0; i < input->size - 1; i++)
	{
		memset(octet,0,sizeof(char)*4);
		memcpy(&uc_temp, input->buf + i*sizeof(char), sizeof(char));
		ui_temp = uc_temp;

		if(flag)
		{
			if(uppercase==1)
			{
				sprintf(octet,"%02X%c", ui_temp, sep);
			}
			else
			{
				sprintf(octet,"%02x%c", ui_temp, sep);
			}
		}
		else
		{
			if(uppercase==1)
			{
				sprintf(octet,"%02X", ui_temp);
			}
			else
			{
				sprintf(octet,"%02x", ui_temp);
			}
		}
		strcat(*output,octet);
		memset(octet,0,sizeof(char)*4);
	}

	memcpy(&uc_temp, input->buf + i*sizeof(char), sizeof(char));
	ui_temp = uc_temp;

	memset(octet, 0, sizeof(char) * 4);

	if(uppercase)
	{
		sprintf(octet,"%02X", ui_temp);
	}
	else
	{
		sprintf(octet,"%02x", ui_temp);
	}

	strcat(*output,octet);

	PRINT_VDEBUG("LIBBMDVDEBUG Hash value: %s\n", *output);

	return BMD_OK;
}

/**
* @author WSz
* Funkcja konwertuje string w postaci hexadecymalnej do postaci binarnej
* @param input[in] string w postaci hexadecymalnej (dopuszzczalne tylko znaki 0-9A-Fa-f bez zadnych separatorow)
* Dla pustego input zwracany jest pusty bufor output;
* @param output[out] bufor z uzyskana postacia binarna
* @return Funkcja zwraca BMD_OK lub odpowiednia wartosc ujemna (kod bledu).
*/
long bmd_hex_genbuf_bin_dump(char *input, GenBuf_t **output)
{
long iter		= 0;
long hexStringLen	= 0;
char* binString = NULL;
long binStringLen	= 0;


	if(input == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(output == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*output != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	hexStringLen = (long)strlen(input);
	if(hexStringLen == 0)
	{
		*output = (GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		if(*output == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
		else // dla pustego input zwraca pusty bufor
			{ return BMD_OK; }
	}
	
	// wymagana parzysta wartosc cyfr (po dwie na bajt)
	if(hexStringLen % 2 != 0)
		{ BMD_FOK(HEX_TO_BINARY_CONVERTION_ERROR); }
	
	for(iter=0; iter<hexStringLen; iter++)
	{
		if(input[iter] >= '0' && input[iter] <= '9')
			{ /*ok*/ }
		else if(input[iter] >= 'A' && input[iter] <= 'F')
			{ /*ok*/ }
		else if(input[iter] >= 'a' && input[iter] <= 'f')
			{ /*ok*/ }
		else
			{ BMD_FOK(HEX_TO_BINARY_CONVERTION_ERROR); }
	}

	binStringLen = hexStringLen/2;
	binString = (char*)calloc(binStringLen + 1, sizeof(char));
	if(binString == NULL)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	
	for(iter=0; iter<binStringLen; iter++)
	{
		// pierwsz cyfra (4 starsze bity)
		if(input[iter*2] >= '0' && input[iter*2] <= '9')
		{
			binString[iter] = (input[iter*2]-'0') << 4;
		}
		else if(input[iter*2] >= 'A' && input[iter*2] <= 'F')
		{
			binString[iter] = (input[iter*2]- 'A' + 10) << 4;
		}
		else
		{
			binString[iter] = (input[iter*2]- 'a' + 10) << 4;
		}
		
		// druga cyfra (4 mlodsze bity)
		if(input[iter*2 + 1] >= '0' && input[iter*2 + 1] <= '9')
		{
			binString[iter] |= input[iter*2 + 1] - '0';
		}
		else if(input[iter*2 + 1] >= 'A' && input[iter*2 + 1] <= 'F')
		{
			binString[iter] |= input[iter*2 + 1] - 'A' + 10;
		}
		else
		{
			binString[iter] |= input[iter*2 + 1] - 'a' + 10;
		}
	}
	
	*output = (GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(*output == NULL)
	{
		free(binString); binString=NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	(*output)->size = binStringLen;
	(*output)->buf = binString;
	binString = NULL;
	return BMD_OK;
}

#if 0
// OBSOLETE (funkcje openSSL ucinaja pierwszy bajt jesli jest wartosci 0x00)
long bmd_hex_genbuf_bin_dump_obsolete(	char *input,
				GenBuf_t **output)
{
BIGNUM *tmp_bn	= NULL;
long status	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (output==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	status=BN_hex2bn(&tmp_bn,input);
	if(status==0)		{	BMD_FOK(HEX_TO_BINARY_CONVERTION_ERROR);	}

	(*output)=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
	if((*output)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	(*output)->size=BN_num_bytes(tmp_bn);
	(*output)->buf=(char *)calloc((*output)->size, sizeof(char));
	if((*output)->buf==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	status=BN_bn2bin(tmp_bn, (unsigned char*)(*output)->buf);
	if(status<=0)		{	BMD_FOK(HEX_TO_BINARY_CONVERTION_ERROR);	}

	/************/
	/* porzadki */
	/************/
	BN_clear_free(tmp_bn);

	return BMD_OK;
}
#endif /*if 0*/


long bmd_genbuf_set_from_file(char *filename,GenBuf_t **output)
{

	PRINT_INFO("LIBBMDINF Setting genbuf from file\n");

	if(filename)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_load_binary_content(filename,output));

	return BMD_OK;
}

long bmd_genbuf_get_buf(GenBuf_t *gb,char **buf)
{
	PRINT_INFO("LIBBMDINF Getting genbuf from buffer\n");
	if(gb==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*buf)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	*buf=gb->buf;

	return BMD_OK;
}

long bmd_genbuf_get_size(GenBuf_t *gb,long *size)
{
	PRINT_INFO("LIBBMDINF Getting genbuf's size\n");

	if(gb==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(size==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	*size=gb->size;

	return BMD_OK;
}

/**
* @author WSz
* Funkcja bmd_convert_hex2base64() odzyskuje dane binarne z postaci hex, a nastepnie koduje dane binarne w kodzie base64.
* @param[in] hexString to lancuch znakowy z danymi w postaci hex (zapisanej za pomoca 0-9 i a-f)
* @param[in] hexSeparation okresla czy w lancuchu hexString wykorzystane sa separatory (do celow sprawdzenia poprawnosci operacji)
* Wartosci: WITH_SEPARATOR (hexString z separatorami) oraz WITHOUT_SEPARATOR (hexString bez separatorow).
* @param[out] base64Buffer to alokowany bufor z zakodowanymi danymi (w zapisie base64)
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, lub kod bledu (wartosci ujemna).
*/
long bmd_convert_hex2base64(char* hexString, HexSeparation_e hexSeparation, GenBuf_t** base64Buffer)
{
GenBuf_t *binaryBuffer	= NULL;
char* base64String		= NULL;

	if(hexString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(base64Buffer == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*base64Buffer != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	
	
	BMD_FOK(bmd_hex_genbuf_bin_dump(hexString, &binaryBuffer));
	
	if(hexSeparation == WITHOUT_SEPARATOR)
	{
		// na jeden bajt binarny przypadaja 2 bajty hex (dwa znaki)
		if((size_t)(binaryBuffer->size * 2) != strlen(hexString))
		{
			free_gen_buf(&binaryBuffer);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	else // WITH_SEPARATOR
	{
		// na jeden bajt binarny przypadaja 3 bajty hex (dwa znaki + separator)
		// po ostatnim znaku hex nie ma separatora, dlatego strlen()+1 aby "wyrownac do trojki"
		if((size_t)(binaryBuffer->size * 3) != (strlen(hexString) + 1))
		{
			free_gen_buf(&binaryBuffer);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	
	base64String=(char*)spc_base64_encode((unsigned char*)binaryBuffer->buf, binaryBuffer->size, 0);
	free_gen_buf(&binaryBuffer);
	if(base64String == NULL)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	*base64Buffer=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
	if(*base64Buffer == NULL)
	{
		free(base64String);
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	(*base64Buffer)->size=(long)strlen(base64String);
	(*base64Buffer)->buf=base64String;
	base64String=NULL;
	
	return BMD_OK;
}
