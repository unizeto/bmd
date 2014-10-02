#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/**
 * Funkcja konwertuje zmienną łańcuchową przechowywaną w strukturze
 * GenBuf na typ OCTET_STRING i przekazuje ją do DER'a.
 * (serializacja).

 * @param[in] GenBuf - struktura GenBuf.
 * @param[in,out] UserDataANYBuf - adres zmiennej typu ANY_t.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
 * @retval -d - błąd konwersji.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
*/

long GenBuf2Octetstring2Any(const GenBuf_t *GenBuf, ANY_t *UserDataANYBuf)
{
long err = 0;
OCTET_STRING_t 	UserDataOctetStringBuf;

	memset(&UserDataOctetStringBuf,0,sizeof(OCTET_STRING_t));
	err=OCTET_STRING_fromBuf(&UserDataOctetStringBuf,(const char *)GenBuf->buf,(int)GenBuf->size);
	if(err < 0) return err;

	memset(UserDataANYBuf,0,sizeof(ANY_t));
	err=ANY_fromType(UserDataANYBuf,&asn_DEF_OCTET_STRING,&UserDataOctetStringBuf);
	if(err < 0) return err;

	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, &UserDataOctetStringBuf, 1);
	return err;
}

/**
 * Funkcja konwertuje zawartość zmiennej przechowywanej w UserDataANYBuf
 * na zmienną łańcuchową przechowywaną w strukturze GenBuf.
 * (desarializacja)

 * @param[in] UserDataANYBuf - adres zmiennej ANY_t.
 * @param[in,out] GenBuf - adres struktury GenBuf.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
 * @retval -d - błąd konwersji.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
 */
long Any2Octetstring2GenBuf(ANY_t *UserDataANYBuf, GenBuf_t *GenBuf)
{
long err = 0;
/*static asn_codec_ctx_t    s_codec_ctx;*/
asn_codec_ctx_t *opt_codec_ctx = NULL;
/*size_t rd;*/
asn_dec_rval_t rval;
OCTET_STRING_t *MetadataOctetStringBuf = NULL;

	if(UserDataANYBuf == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM1);
	}

	if(GenBuf == NULL)
	{
		 BMD_FOK(BMD_ERR_PARAM2);
	}

	/* Deserializuje bufor */
	rval = ber_decode(	opt_codec_ctx,
				&asn_DEF_OCTET_STRING,
				(void **)&MetadataOctetStringBuf,
				UserDataANYBuf->buf,
				UserDataANYBuf->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, MetadataOctetStringBuf, 0);
		return rval.code;
	}

	/* W OCTET_STRING mam juz wszystko, konwertyje na GenBuf i zwalniam zasoby */
	GenBuf->buf = (char *) MetadataOctetStringBuf->buf;
	GenBuf->size = MetadataOctetStringBuf->size;

	/* zwalniam tylko strukture MetadataOctetStringBuf bo zawartosc zostala podpieta pod GenBuf */
	free(MetadataOctetStringBuf);	MetadataOctetStringBuf = NULL;
	return err;
}


/**
 * Funkcja konwertuje zawartość zmiennej przechowywanej w UserDataANYBuf
 * na zmienną łańcuchową.

 * @param[in] UserDataANYBuf - adres zmiennej ANY_t.
 * @param[out] string - adres łańcucha znaków.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
 * @retval -d - błąd konwersji.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
 */

long Any2Octetstring2String(ANY_t *UserDataANYBuf, char **string)
{
long err = 0;
asn_dec_rval_t rval;
OCTET_STRING_t *MetadataOctetStringBuf = NULL;

	/* Deserializuje bufor */
	rval = ber_decode( 0, &asn_DEF_OCTET_STRING,
			(void **)&MetadataOctetStringBuf,
			UserDataANYBuf->buf,
			UserDataANYBuf->size);
	if(rval.code != RC_OK) {
		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, MetadataOctetStringBuf, 0);
		return rval.code;
	}
	/* W OCTET_STRING mam juz wszystko, konwertyje na string i zwalniam zasoby */
	*string = (char *) malloc (sizeof(char) * (1+ MetadataOctetStringBuf->size));
	if(*string == NULL) return NO_MEMORY;

	memset(*string, 0, sizeof(char) * (1+ MetadataOctetStringBuf->size));
	memmove(*string, MetadataOctetStringBuf->buf, MetadataOctetStringBuf->size);

	free(MetadataOctetStringBuf->buf);	MetadataOctetStringBuf->buf = NULL;
	free(MetadataOctetStringBuf);		MetadataOctetStringBuf      = NULL;
	return err;
}

/**
 * Funkcja wyszukuje metadane na podstawie OID-u i umieszcza je
 * w strukturze GenBuf_t.

 * @param[in] oid - identyfikator objektu.
 * @param[in] oid_size - rozmiar identyfikatora objektu.
 * @param[in] metadata - struktura zawierająca matadane.
 * @param[in] nof_metadata - ilość metadanych.
 * @param[out] buf - adres bufora GenBuf_t.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
 * @retval -1 - błąd wyszukiwania.
 * @note funkcja zwraca kod powrotu == 0 mimo iż nie wyszukuje prawidłowych metadanych na podstawie OID-u
 */

long metadata_get_value_by_oid(	char *oid,
						MetaDataBuf_t **metadata,
						long nof_metadata,
						GenBuf_t **buf)
{
long i			= 0;
MetaDataBuf_t *cmeta;
long s			= 0;

	for( i=0; i<nof_metadata; i++ )
	{
		cmeta=metadata[i];
		if (strcmp(cmeta->OIDTableBuf, oid)==0)
		{
			s=set_gen_buf2( cmeta->AnyBuf, cmeta->AnySize, buf );
			bmd_assert(s==0, af);
			return(0);
		}
	}
	return(-1);
}

/**
funkcja long IsOidFormat(char *string) sprawdza, czy przekazywany string jest oidem (w notacji liczbowo-kropkowej)

@retval 1 prawidlowy format oidu
@retval 0 nieprawidlowy format oidu
@retval <0 blad
*/
long IsOidFormat(char *string)
{
long iter=		0;
long lastCharDot=	0;
long stringLength=	0;

	if(string == NULL)
		{ return -1; }

	if( (stringLength=(long)strlen(string)) <= 0 )
		{ return 0; }

	for(iter=0; iter<stringLength; iter++)
	{
		if( iter==0 ) //pierwszy znak nie moze byc '.'
		{
			if( string[iter] >= 48 && string[iter] <= 57 ) //[0-9]
				{ lastCharDot=0; }
			else
				{ return 0; }
		}
		else
		{
			if( string[iter] >= 48 && string[iter] <= 57 ) //[0-9]
				{ lastCharDot=0; }
			else if(string[iter] == '.')
			{
				if(lastCharDot > 0) //jesli poprzedni znak tez byl '.'
					{ return 0; }
			}
			else
				{ return 0; }
		}
	}

	if(lastCharDot > 0) //jesli ostatni znka byl '.'
		{ return 0; }

	return 1;
}

