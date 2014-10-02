#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
//#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/**
Funkcja bmd_hash_data w trybie wielorundowym alokuje tylko bufor wyjsciowy przy ostatnim wywolaniu z NULL na wejsciu.
Tryb wielorundowy ustawia sie recznie w kontekscie hashujacym.
*/

long bmd_hash_data(GenBuf_t *input,bmd_crypt_ctx_t **ctx,GenBuf_t **output,void *opt)
{
long status		= 0;
long flag		=-1;
unsigned int ui_temp	= 0;
	
	if(ctx==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	BMD_FOK(bmd_check_ctx(*ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_HASH));
	
	if((*ctx)->hash->hash_params==BMD_HASH_SINGLE_ROUND) /* jesli jest jeden przebieg to bufor nie moze byc NULL */
	{
		if(input==NULL)
		{
			PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
		if(input->buf==NULL)
		{
			PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
		if(input->size<=0)
		{
			PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
	}
	else
	{/* jesli wiele przebiegow to wtedy podanie NULL swiadczy o tym ze byc DigestFinal */
		if(input==NULL)
		{
			(*ctx)->hash->hash_stage=BMD_HASH_STAGE_LAST;
		}
	}
	
	if(output==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	
	if( (*output)!=NULL )
    	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	
	/* jednoprzebiegowe lub wieloprzebiegowe liczenie skrotu */
	if((*ctx)->hash->hash_params==BMD_HASH_SINGLE_ROUND)
	{
		flag=BMD_HASH_SINGLE_ROUND;
	}
	else
	{
		flag=BMD_HASH_MULTI_ROUND;
	}
	
	/* przy pierwszym wywolaniu nalezy stworzyc kontekst */
	if( (flag==BMD_HASH_SINGLE_ROUND) || ( ((*ctx)->hash->hash_stage==BMD_HASH_STAGE_FIRST) && ((*ctx)->hash->hash_params==BMD_HASH_MULTI_ROUND) ) )
	{
		if((*ctx)->hash->hash_alg==BMD_HASH_ALGO_SHA1)
		{
			(*ctx)->hash->md=(EVP_MD *)EVP_sha1();
			if((*ctx)->hash->md==NULL)
			{
				PRINT_DEBUG("LIBBMDPKIERR Not available. Error=%i\n",BMD_ERR_NOTAVAIL);
				return BMD_ERR_NOTAVAIL;
			}
		}
		else
		{
			if((*ctx)->hash->hash_alg==BMD_HASH_ALGO_MD5)
			{
				(*ctx)->hash->md=(EVP_MD *)EVP_md5();
				if((*ctx)->hash->md==NULL)
				{
					PRINT_DEBUG("LIBBMDPKIERR Not available. Error=%i\n",BMD_ERR_NOTAVAIL);
					return BMD_ERR_NOTAVAIL;
				}
			}
			else
			{
				PRINT_DEBUG("LIBBMDPKIERR Method unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
				return BMD_ERR_UNIMPLEMENTED;
			}
		}

		(*ctx)->hash->md_ctx=EVP_MD_CTX_create();
		status=EVP_DigestInit((*ctx)->hash->md_ctx,(*ctx)->hash->md);
		if(status==0)
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		(*ctx)->hash->hash_stage=BMD_HASH_STAGE_NORMAL;
	}
	
	if( (flag==BMD_HASH_SINGLE_ROUND) || ( ((*ctx)->hash->hash_stage==BMD_HASH_STAGE_NORMAL) && ((*ctx)->hash->hash_params==BMD_HASH_MULTI_ROUND) ) )
	{
		
		status=EVP_DigestUpdate((*ctx)->hash->md_ctx,input->buf,input->size);
		if(status==0)
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		
		(*ctx)->hash->hash_stage=BMD_HASH_STAGE_NORMAL;
	}
	
	if( (flag==BMD_HASH_SINGLE_ROUND) || ( ((*ctx)->hash->hash_stage==BMD_HASH_STAGE_LAST) && ((*ctx)->hash->hash_params==BMD_HASH_MULTI_ROUND) ) )
	{
		if((*ctx)->hash->hash_size==0) /* jesli pierwszy raz wolamy to wykonanie DigestFinal */
		{
			if( ((*ctx)->hash->md_ctx==NULL) || ((*ctx)->hash->md==NULL) )
			{
				PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_NOTINITED);
				return BMD_ERR_NOTINITED;
			}
			ui_temp = (*ctx)->hash->hash_size;
			status=EVP_DigestFinal((*ctx)->hash->md_ctx, (unsigned char*)((*ctx)->hash->hash_value),&ui_temp);
			(*ctx)->hash->hash_size = ui_temp;
			if(status==0)
			{
				PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}

			status=set_gen_buf2((*ctx)->hash->hash_value,(*ctx)->hash->hash_size,output);
			if(status!=0)
			{
				PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
	
			(void)EVP_MD_CTX_destroy((*ctx)->hash->md_ctx);(*ctx)->hash->md_ctx=NULL;
			(*ctx)->hash->hash_stage=BMD_HASH_STAGE_COMPLETE;
		}
		else /* w przeciwnym wypadku po prostu umiesc wartosc skrotu w buforze wynikowym */
		{
			status=set_gen_buf2((*ctx)->hash->hash_value,(*ctx)->hash->hash_size,output);
			if(status!=0)
			{
				PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
		}
	}
	
	return BMD_OK;
}

long bmd_hash_as_string(GenBuf_t *input,char **output,char sep,long uppercase)
{
unsigned int ui_temp		= 0;
long i				= 0;
unsigned char uc_temp		= 0;
char octet[4];

	PRINT_DEBUG("LIBBMDPKIINF bmd_hash_as_string\n");

	if(input==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(output==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*output)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*output=(char*)calloc(input->size * 3 + 1, sizeof(char));
	if(*output == NULL)		{	BMD_FOK(NO_MEMORY);	}

	memset(octet, 0, sizeof(char) * 4);	

	for(i=0;i<input->size-1;i++)
	{
		memset(octet, 0, sizeof(char)*4 );
		memcpy(&uc_temp, input->buf + i, sizeof(char));
		ui_temp = uc_temp;

		if(sep != 0)
		{
			if(uppercase != 0)
			{
				sprintf(octet,"%02X%c", ui_temp, sep);
			}
			else
			{
				sprintf(octet,"%02x%c", ui_temp, sep);
			}
			sprintf((*output) + i*3, "%s", octet);
		}
		else
		{
			if(uppercase != 0)
			{
				sprintf(octet,"%02X", ui_temp);
			}
			else
			{
				sprintf(octet,"%02x", ui_temp);
			}
			sprintf((*output) + i*2, "%s", octet);
		}
	}

	memset(octet,0,sizeof(char) * 4);
	memcpy(&uc_temp, input->buf + i, sizeof(char));
	ui_temp = uc_temp;

	if(uppercase != 0)
	{
		sprintf(octet,"%02X", ui_temp);
	}
	else
	{
		sprintf(octet,"%02x", ui_temp);
	}

	if(sep != 0)
	{
		sprintf((*output) + i*3, "%s", octet);
	}
	else
	{
		sprintf((*output) + i*2, "%s", octet);
	}
	
	return BMD_OK;
}

/**
 * Funckja /c bmd_string_as_sha1 służy do zamiany skrótu podanego jako
 * ciąg znaków wyprodukowany przez program sha1sum na bufor generyczny.
 * \param strhash Ciąg znaków, zapis szesnastkowy 160 bitów skrótu sha1 zakończony
 * znakiem \0.
 * \param outhash Adres do wyzerowanego wskaźnika na strukturę GenBuf_t. Funkcja
 * zaalokuje niezbędną pamięć i zapisze do nowo zaalokowanej struktury skrót.
 *
 * \retval -1 Podany ciąg znaków nie jest skrótem sha1.
 * \retval -2 Podano nieprawidłowy drugi parametr - to nie jest wyzerowany
 * wskaźnik na bufor generyczny. 
 * \retval -3 Problemy z alokowaniem pamięci.
 * */
long bmd_string_as_sha1(const char *strhash, GenBuf_t **outhash) /*AK*/
{
char octet[3]="";
long i			= 0;
long tmp		= 0;

	 PRINT_DEBUG("LIBBMDPKIINF bmd_string_as_sha1\n");

	if(strhash == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(strlen(strhash) != 40)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(outhash == NULL) 		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( *outhash != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*outhash) = (GenBuf_t*) malloc(sizeof(GenBuf_t));
	if (*outhash == NULL)		{		BMD_FOK(NO_MEMORY);		}
	memset(*outhash, 0, sizeof(GenBuf_t));

	/*sha1 ma 20 bajtów*/
	(*outhash)->buf = (char*) malloc ( sizeof(char)*(20 + 1));
	if ((*outhash)->buf == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset((*outhash)->buf, 0, sizeof(char)*(20 + 1));

	memset(octet, 0, 3 * sizeof(char));
	/*każde 2 znaki na liczbę*/

	for (i = 0; i < (long)strlen(strhash); i+=2)
	{
		/*skopiuj do octetu dwa znaki*/
		memcpy(octet, strhash + i, 2*sizeof(char));
		/*zamień zawartosc oktetu na long*/
		tmp = strtol(octet, NULL, 16);
		/*dodaj zawartosc do outhasha*/
		(*outhash)->buf[i/2] = (char)tmp;
		(*outhash)->size++;
	}
	return BMD_OK;
}


/*
by WSZ

funckja bmd_hash_SignaturePolicy() pozwala wyliczyć skrot z polityki podpisu w sposób wymagany
przez rfc 3126 ( patrz Singned Atribute - Signature Policy Identifier)...
Robimy to kompatybilnie z ProCertum API, czyli
wycinamy z polityki podpisu TLV SignPolicyHash, tak uzyskana polityke kodujemy do DER i wycinamy TL calosci.
Dopiero wynik tych operacji przetwarzany jest w funkcji skrotu.

Argument wejsciowy:
@arg sigPolicy to wskaznik na bufor z zakodowana w DER struktura polityki podpisu (zawartosc pliku *.spol)
Argument wyjściowy:
@arg sigPolicyHash to adres wskaznika na alokowany bufor z wyliczonym skrotem

Funkcja zwraca:
@retval 0 w przypadku sukcesu
Odpowiednia wartosc ujemna w przypadku niepowodzenia:
@retval -1 jesli za sigPolicy podano NULL
@retval -2 jesli za sigPolicyHash podano NULL
@retval -3 jesli wartosc wyluskana z sigPolicyHash nie jest wyNULLowana
@retval -4 jesli nie mozna zdekodowac polityki podpisu
@retval -5, -7 jesli nie mozna zakodowac do DER okrojonej polityki podpisu
@retval -6, -8 w przypadku problemow z alokacja pamieci
@retval -9 jesli nie mozna ustawic kontekstu hashujacego
@retval -10 jesli nie mozna wyliczyc skrotu

*/
long bmd_hash_SignaturePolicy(GenBuf_t *sigPolicy, GenBuf_t** sigPolicyHash)
{

	long ret_val=0;
	bmd_crypt_ctx_t *hash_ctx=NULL;
	SignaturePolicy_t *SP=NULL;
	GenBuf_t *derSigPolicy=NULL;
	GenBuf_t *cutDER=NULL;
	GenBuf_t *hash=NULL;
	long status;

	if(sigPolicy == NULL)
		{ return -1; }
	if(sigPolicyHash == NULL)
		{ return -2; }
	if(*sigPolicyHash != NULL)
		{ return -3; }
		
	ret_val=Decode_spol(sigPolicy, &SP);
	if(ret_val != 0)
		{ return -4; }
	
	//wyrzynam skrot
	if(SP->signPolicyHash != NULL)
	{
		asn_DEF_SignPolicyHash.free_struct(&asn_DEF_SignPolicyHash, SP->signPolicyHash, 1);
		free(SP->signPolicyHash);
		SP->signPolicyHash=NULL;
	}

	//koduje polityke podpisu z wycietym hashem
	status=asn1_encode(&asn_DEF_SignaturePolicy, SP, NULL, &derSigPolicy);
	if( status != BMD_OK )
		return -5;
	
	Destroy_spol(&SP);

	//odcinam 4 pierwsze bajty
	ret_val=set_gen_buf2(derSigPolicy->buf + 4, derSigPolicy->size - 4, &cutDER);
	free_gen_buf(&derSigPolicy);
	if(ret_val != BMD_OK)
		{ return -8; }

	ret_val=bmd_set_ctx_hash(&hash_ctx, BMD_HASH_ALGO_SHA1);
	if(ret_val != BMD_OK)
	{
		free_gen_buf(&cutDER);
		return -9;
	}

	ret_val=bmd_hash_data(cutDER, &hash_ctx, &hash, NULL);
	free_gen_buf(&cutDER);
	bmd_ctx_destroy(&hash_ctx);
	
	if(ret_val != BMD_OK)
	{
		return -10;
	}
	free_gen_buf(&derSigPolicy);

	*sigPolicyHash=hash;
	return 0;

#if 0 /*ponizsza implementacja kopiuje skrot zywcem z polityki podpisu (nie oblicza go)*/
	SignaturePolicy_t *SP=NULL;
	long ret_val=0;

	if(sigPolicy == NULL)
		{ return -1; }
	if(sigPolicyHash == NULL)
		{ return -2; }
	if(*sigPolicyHash != NULL)
		{ return -3; }
		
	ret_val=Decode_spol(sigPolicy, &SP);
	if(ret_val != 0)
		{ return -4; }

	if(SP->signPolicyHash == NULL)
	{
		Destroy_spol(&SP);
		return -5;
	}

	ret_val=set_gen_buf2(SP->signPolicyHash->buf, SP->signPolicyHash->size, sigPolicyHash);
	Destroy_spol(&SP);
	if(ret_val != BMD_OK)
	{
		return -6;
	}
	else
	{
		return 0;
	}
#endif 
}

