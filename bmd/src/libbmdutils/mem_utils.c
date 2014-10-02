#include <bmd/libbmdutils/libbmdutils.h>
#include <stdlib.h>
#include <sys/types.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4310)
#pragma warning(disable:4100)
#endif

#ifndef WIN32
#include <wchar.h>
#include <dirent.h>
#endif

/**
Funkcja tworzy nowy genbuf ale kopiuje do niego sam wskaznik. Nie jest alokowany bufor na zawartosc!
Wskaznik wejsciowy jest po tej operacji nulowany na wszelki wypadek.
*/
long set_gen_buf_ptr(char **buf, long size, GenBuf_t **output)
{


	if ( buf == NULL )      {       BMD_FOK(BMD_ERR_PARAM1);        }
	if ( output == NULL )   {       BMD_FOK(BMD_ERR_PARAM3);        }

	(*output)=NULL;
	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if((*output)==NULL)
	{
		BMD_FOK(NO_MEMORY);
	}

        (*output)->buf=(char *)*buf;
        (*output)->size=size;

	*buf = NULL;

        return BMD_OK;

}

/**
 * Funkcja wypełnia strukturę GenBuf_t (output) ilością (size) bajtów z (buf).

 * @param[in] buf - adres bufora źródłowego.
 * @param[in] size - rozmiar bufora źródłowego.
 * @param[out] output - adres wezła struktury GenBuf_t.
 * @param[out] status - status zakończenia
 * - BMD_OK - zakończenie pomyślne.
 * - BMD_ERR_MEMORY - błąd alokacji pamięci.
 * @note funkcja nie sprawdza poprawności przekazywanych argumentów
 * @note funkcja jest wywoływana z poziomu funkcji set_gen_buf2
*/

long set_gen_buf(const char *buf, long *size, GenBuf_t **output)
{
	if ( buf == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ( size == NULL )	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ( output == NULL )	{	BMD_FOK(BMD_ERR_PARAM3);	}

	(*output)=NULL;
	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));

	if((*output)==NULL)
	{
		BMD_FOK(NO_MEMORY);
	}
	(*output)->buf=NULL;
	(*output)->size=0;

	/* 1 bajt nadmiaru ze względu bezpieczeństwa. Użytkownik funkcji powinien zakładać,
	 * że tego bajtu nie ma! :-) */

	(*output)->buf=(char *)malloc( (*size + 1)*sizeof(char) );
	if((*output)->buf == NULL)
	{
		free_gen_buf(output);
		BMD_FOK(NO_MEMORY);
	}

	memset((*output)->buf,0,(*size + 1) * sizeof(char) );
	memcpy((*output)->buf,buf,*size);
	(*output)->size=*size;

	return BMD_OK;
}


long set_empty_gen_buf(	GenBuf_t **dest)
{
long len			= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(dest==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*dest)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	*dest=NULL;
	len = 1;
	BMD_FOK(set_gen_buf("\0",&len,dest));

	return BMD_OK;
}


/**
 * Funkcja wypełnia strukturę GenBuf_t (dest) ilością (src_size) bajtów z (src).

 * @param[in] src - adres bufora źródłowego.
 * @param[in] src_size - długość bufora źródłowego.
 * @param[out] dest - adres struktury GenBuf_t.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_PARAM1 - niepoprawny adres źródła (src).
 * @retval BMD_ERR_PARAM2 - niepoprawny rozmiar źródła (src_size).
 * @retval BMD_ERR_PARAM3 - niepoprawny adres struktury GenBuf_t.
 * @retval BMD_ERR_PARAM3 - struktura ma już przydzieloną pamięć.
*/

long set_gen_buf2 (	const char * src,
			long src_size,
			GenBuf_t **dest)
{

long len			= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(src==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(src_size<=0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(dest==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*dest)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	*dest=NULL;
	len = src_size;

	BMD_FOK(set_gen_buf(src,&len,dest));

	return BMD_OK;
}

/*funkcja bmd_clone_genbuf_content() klonuje zawartosc bufora src do bufora dest tzn.:
	a) ustawia taki sam rozmiar (pole size)
	b) alokuje pamiec (o rozmiarze src->size ) dla pola buf i przekopiowuje zawartosc z src->buf do dest->buf

@param src to wskaznik na bufor, ktorego zawartosc ma zostac sklonowana
@param dest to wskaznik na bufor, ktore zawartosc ma zostac odpowiednio wypelniona

@retval 0 w przypadku sukcesu
@retval -1 jesli za src podano NULL
@retval -2 jesli za dest podano NULL
@retval -3 jesli bufor dest nie jest pusty
@retval -4 jesli rozmiar bufora src jest ustawiony na wartosc ujemna
@retval -10 w przypadku prblomu z alokacja pamieci
*/

long bmd_clone_genbuf_content(	GenBuf_t *src,
						GenBuf_t *dest)
{
	if(src == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(src->size < 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	dest->buf=(char *)calloc(src->size, sizeof(uint8_t));
	if(dest->buf == NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	dest->size=src->size;
	memcpy(dest->buf, src->buf, src->size);

	return BMD_OK;
}


/**
 * Funkcja zwalnia listę wartosci atrybutów (list).
 *
 * @param[in,out] list - adres listy atrybutów.
 * @note funkcja wymaga postawienia strażnika na
 *  końcu listy równego NULL.
 * @note funkcja nie sprawdza poprawności przekazywanych
 *  argumentów.
*/

/* zwalnia liste wartosci atrybutow o rozmiarze wskazywanym przez size */
void free_BMD_attr_value_list(BMD_attr_value ***list)
{
	long i;

	i=0;
	if((*list)!=NULL)
	{
	    while((*list)[i])
	    {
		free_gen_buf(&((*list)[i]));
		i++;
	    }
	}
	free(*list);*list=NULL;
}


/**
 * Funkcja zwalnia pamięć z pojedyńczego atrybutu.
 *
 * @param[in,out] attr - adres listy atrybutów.
 *
*/

void free_BMD_attr(BMD_attr_t **attr)
{
    long i;

    if( (*attr)!=NULL )
    {
	free_gen_buf(&((*attr)->label));
	free_gen_buf(&((*attr)->regexp));
	free_gen_buf(&((*attr)->tooltip));
	free_gen_buf(&((*attr)->depValueList));
	free_gen_buf(&((*attr)->controlName));
	i=0;
	if((*attr)->valuesList)
	{
	    while((*attr)->valuesList[i])
	    {
		free_gen_buf(&((*attr)->valuesList[i]));
		i++;
	    }
	    free((*attr)->valuesList);
	}

	i=0;
	if((*attr)->priorityList)
	{
	    free0((*attr)->priorityList);
	}

	free_gen_buf(&((*attr)->oid));
    }
}

/**
 * Funkcja zwalnia listę atrybutów.
 *
 * @param[in,out] list - adres listy atrybutów.
 * @note funkcja nie sprawdza poprawności przekazywanych
 *  argumentów
*/


void free_BMD_attr_list(BMD_attr_t ***list)
{
long i=0;

	if(*list)
	{
		if((*list)!=NULL)
		{
			while((*list)[i])
			{
				free_BMD_attr(&((*list)[i]));
				i++;
			}
		}
		free(*list);*list=NULL;
	}
	return;
}

/**
 * Funkcja zwalnia strukturę GenBuf_t (bufor).
 *
 * @param[in,out] bufor - adres struktury GenBuf_t.
 *
*/

void free_gen_buf(GenBuf_t **bufor)
{
	if(bufor != NULL)
	{
		if( (*bufor)!=NULL )
		{
			if((*bufor)->buf!=NULL)
			{
				free((*bufor)->buf); (*bufor)->buf=NULL;
			}
			free(*bufor);*bufor=NULL;
		}
	}
}

void free_certificate_attributes(	certificate_attributes_t **UserAttributes)
{
long i		= 0;

	free_gen_buf(&((*UserAttributes)->id));
	free_gen_buf(&((*UserAttributes)->name));
	free_gen_buf(&((*UserAttributes)->identityId));
	free_gen_buf(&((*UserAttributes)->serial));
	free_gen_buf(&((*UserAttributes)->cert_dn));
	free_gen_buf(&((*UserAttributes)->accepted));

	free_gen_buf(&((*UserAttributes)->user_chosen_role));
	free_gen_buf(&((*UserAttributes)->user_chosen_group));

	if ((*UserAttributes)->groups!=NULL)
	{
		if ((*UserAttributes)->groups->gbufs!=NULL)
		{
			for (i=0; i<(*UserAttributes)->groups->size; i++)
			{
				free_gen_buf(&((*UserAttributes)->groups->gbufs[i]));
			}
			free((*UserAttributes)->groups->gbufs); (*UserAttributes)->groups->gbufs=NULL;
			free((*UserAttributes)->groups); (*UserAttributes)->groups=NULL;
		}
	}

	if ((*UserAttributes)->roles!=NULL)
	{
		if ((*UserAttributes)->roles->gbufs!=NULL)
		{
			for (i=0; i<(*UserAttributes)->roles->size; i++)
			{
				free_gen_buf(&((*UserAttributes)->roles->gbufs[i]));
			}
			free((*UserAttributes)->roles->gbufs); (*UserAttributes)->roles->gbufs=NULL;
			free((*UserAttributes)->roles); (*UserAttributes)->roles=NULL;
		}
	}

	if ((*UserAttributes)->security!=NULL)
	{
		if ((*UserAttributes)->security->gbufs!=NULL)
		{
			for (i=0; i<(*UserAttributes)->security->size; i++)
			{
				free_gen_buf(&((*UserAttributes)->security->gbufs[i]));
			}

			free((*UserAttributes)->security->gbufs); (*UserAttributes)->security->gbufs=NULL;
			free((*UserAttributes)->security); (*UserAttributes)->security=NULL;
		}
	}

	free((*UserAttributes)); (*UserAttributes)=NULL;
}



#if 0
// OBSOLETE !!! OBSOLETE !!! OBSOLETE !!!
/**
 * Funkcja konwertuje szeroki znak na pojedyńczy (c) zgodny z polską
 * stroną kodową używaną przez systemy Windows.
 *
 * @param[in] wc - szeroki znak (2 bajty).
 * @param[out] c  - pojedyńczy znak zgodny z polską stroną kodową.
 *
 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
*/

long wc2c_cp1250(wchar_t wc,char *c)
{
	switch(wc) {
	    case 0x0105:
			*c=(char)0xb9;
			break;
	    case 0x0107:
			*c=(char)0xe6;
			break;
	    case 0x0119:
			*c=(char)0xea;
			break;
		case 0x0142:
			*c=(char)0xb3;
			break;
		case 0x0144:
			*c=(char)0xf1;
			break;
		case 0x00f3:
			*c=(char)0xf3;
			break;
		case 0x015b:
			*c=(char)0x9c;
			break;
		case 0x017a:
			*c=(char)0x9f;
			break;
		case 0x017c:
			*c=(char)0xbf;
			break;
		case 0x0104:
			*c=(char)0xa5;
			break;
		case 0x0106:
			*c=(char)0xc6;
			break;
		case 0x0118:
			*c=(char)0xca;
			break;
		case 0x0141:
			*c=(char)0xa3;
			break;
		case 0x0143:
			*c=(char)0xd1;
			break;
		case 0x00d3:
			*c=(char)0xd3;
			break;
		case 0x015a:
			*c=(char)0x8c;
			break;
		case 0x0179:
			*c=(char)0x8f;
			break;
		case 0x017b:
			*c=(char)0xaf;
			break;
		default:
			*c=(char)wc;
	}
	return BMD_OK;
}


/**
 * Funkcja konwertuje szeroki znak na pojedyńczy (c) występujący
 * w jezyku polskim zgodnie ze standardem ISO 8859-2.
 *
 * @param[in] wc - szeroki znak.
 * @param[out] c - pojedyńczy znak zgodny ze standardem ISO 8859-2.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
*/

long wc2c_iso88592(wchar_t wc,char *c)
{
	switch(wc) {
		case 0x0105:
			*c=(char)0xb1;
			break;
		case 0x0107:
			*c=(char)0xe6;
			break;
		case 0x0119:
			*c=(char)0xea;
			break;
		case 0x0142:
			*c=(char)0xb3;
			break;
		case 0x0144:
			*c=(char)0xf1;
			break;
		case 0x00f3:
			*c=(char)0xf3;
			break;
		case 0x015b:
			*c=(char)0xb6;
			break;
		case 0x017a:
			*c=(char)0xbc;
			break;
	    case 0x017c:
			*c=(char)0xbf;
			break;
		case 0x0104:
			*c=(char)0xaa;
			break;
		case 0x0106:
			*c=(char)0xc6;
			break;
		case 0x0118:
			*c=(char)0xca;
			break;
		case 0x0141:
			*c=(char)0xa3;
			break;
		case 0x0143:
			*c=(char)0xd1;
			break;
		case 0x00d3:
			*c=(char)0xd3;
			break;
		case 0x015a:
			*c=(char)0xa6;
			break;
		case 0x0179:
			*c=(char)0xac;
			break;
		case 0x017b:
			*c=(char)0xaf;
			break;
		default:
			*c=(char)wc;
	}
	return BMD_OK;
}

/**
 * Funkcja konwertuje szeroki znak na ciag znaków (c) występujacych
 * w jezyku polskim zgodnych z kodowaniem UTF-8.
 *
 * @param[in] wc - szeroki znak.
 * @param[out] c - łańcuch znaków (max 2 znaki) zakodowany w UTF-8.
 * @param[out] count - liczba znaków w lańcuchu (1 lub 2).

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
*/
long wc2utf8(wchar_t wc,char *c,long *count)
{
	long j=0;
	switch(wc) {
		case 0x0105:
			c[j]=(char)0xc4;c[j+1]=(char)0x85;*count=2;
			break;
		case 0x0107:
			c[j]=(char)0xc4;c[j+1]=(char)0x87;*count=2;
			break;
		case 0x0119:
			c[j]=(char)0xc4;c[j+1]=(char)0x99;*count=2;
			break;
		case 0x0142:
			c[j]=(char)0xc5;c[j+1]=(char)0x82;*count=2;
			break;
		case 0x0144:
			c[j]=(char)0xc5;c[j+1]=(char)0x84;*count=2;
			break;
		case 0x00f3:
			c[j]=(char)0xc3;c[j+1]=(char)0xb3;*count=2;
			break;
		case 0x015b:
			c[j]=(char)0xc5;c[j+1]=(char)0x9b;*count=2;
			break;
		case 0x017a:
			c[j]=(char)0xc5;c[j+1]=(char)0xba;*count=2;
			break;
		case 0x017c:
			c[j]=(char)0xc5;c[j+1]=(char)0xbc;*count=2;
			break;
		case 0x0104:
			c[j]=(char)0xc4;c[j+1]=(char)0x84;*count=2;
			break;
		case 0x0106:
			c[j]=(char)0xc4;c[j+1]=(char)0x86;*count=2;
			break;
		case 0x0118:
			c[j]=(char)0xc4;c[j+1]=(char)0x98;*count=2;
			break;
		case 0x0141:
			c[j]=(char)0xc5;c[j+1]=(char)0x81;*count=2;
			break;
		case 0x0143:
			c[j]=(char)0xc5;c[j+1]=(char)0x83;*count=2;
			break;
		case 0x00d3:
			c[j]=(char)0xc3;c[j+1]=(char)0x93;*count=2;
			break;
		case 0x015a:
			c[j]=(char)0xc5;c[j+1]=(char)0x9a;*count=2;
			break;
		case 0x0179:
			c[j]=(char)0xc5;c[j+1]=(char)0xb9;*count=2;
			break;
		case 0x017b:
			c[j]=(char)0xc5;c[j+1]=(char)0xbb;*count=2;
			break;
		default:
			c[j]=(char)wc;*count=1;
			break;
	}

	return BMD_OK;
}


/**
 * Funkcja konwertuje łańcuchy znaków przechowywane w zmiennych typu
 * szeroki znak (wchar_t) na łańcuchy znaków typu pojedyńczy znak (char) zgodnie z
 * wybranym standardem kodowania (CP1250, ISO88592, UTF8).

 * @param[in] wcs - łańcuch znaków typu (wchar_t).
 * @param[in] code_page - wybrany sposób kodowania.
 * @param[out] output - łańcuch znaków typu (char).

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_PARAM1 - niepoprawny adres łańcucha źródłowego (wcs).
 * @retval BMD_ERR_PARAM2 - łańcuch wyjściowy jest już zainicjowany.
 * @retval BMD_ERR_PARAM3 - niepoprawny adres struktury GenBuf_t.
 * @retval BMD_ERR_UNIMPLEMENTED - wybrano nieobsługiwany sposób kodowania.
 * @retval BMD_ERR_OP_FAILED - nieprawidłowa długość łańcucha źródłowego.
 * @retval BMD_ERR_MEMORY - błąd alokacji łańcucha wyjściwego.
*/
long wcs2char(	const wchar_t *wcs,
			char **output,
			long code_page)
{
long length;
long i=0;
long j=0;
long count=0;

	PRINT_INFO("LIBBMDUTILSINF Converting wchar_t to char\n");
	if(wcs==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if( (code_page!=BMD_CODE_PAGE_CP1250) && (code_page!=BMD_CODE_PAGE_ISO88592) && (code_page!=BMD_CODE_PAGE_UTF8) )
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	length=(int)wcslen(wcs);
	if(length<0)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	(*output)=(char *)malloc(2*length+2);
	if((*output)==NULL)
	{
		BMD_FOK(BMD_ERR_MEMORY);
	}
	memset(*output,0,2*length+1);

	for(i=0;i<length;i++)
	{
		switch(code_page)
		{
			case BMD_CODE_PAGE_CP1250:
				wc2c_cp1250(wcs[i],&((*output)[i]));
				break;
			case BMD_CODE_PAGE_ISO88592:
				wc2c_iso88592(wcs[i],&((*output)[i]));
				break;
			case BMD_CODE_PAGE_UTF8:
				wc2utf8(wcs[i],&((*output)[j]),&count);
				j=j+count;
		}
	}
	return BMD_OK;
}


long char2wcs(const char *src,wchar_t **dest)
{
#ifndef WIN32
	/*PRINT_INFO("LIBBMDUTILSINF Converting char to wchar_t\n");*/
	size_t n = mbstowcs(NULL, src, 0);
	(*dest)=(wchar_t *)malloc((n+1)*sizeof(wchar_t));
	if(*dest==NULL)
	{
		PRINT_ERROR("LIBBMDUTILSERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	if(mbstowcs(*dest, src, n + 1) != n)
	{
		PRINT_ERROR("LIBBMDUTILSERR Conversion failed\n");
		/*free(dest);*/
		return BMD_ERR_OP_FAILED;
	}
#endif
	return BMD_OK;
}

#endif //OBSOLETE!!! OBSOLETE!!! OBSOLETE!!!

/**
 * Funkcja wypełnia węzeł struktury GenBuf_t (buffer) zawartością pliku (filename).
 * @param[in] filename - nazwa pliku.
 * @param[out] buffer - adres struktury GenBuf_t.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_DISK - błąd otwarcia pliku.
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci.
 * @note funkcja nie sprawdza poprawności
 *  przekazywanych argumentów.
*/
long bmd_load_binary_content(const char *filename, GenBuf_t **buffer)
{
long fd;
struct stat file_info;
long nread;
PRINT_TEST("filename: |%s|\n", filename);
#ifdef WIN32
	if( (fd=open(filename,O_RDONLY|_O_BINARY)) == -1)	{	BMD_FOK(BMD_ERR_DISK);	}
#else

	if( (fd=open(filename,O_RDONLY)) == -1)			{	perror("error "); BMD_FOK(BMD_ERR_DISK);	}
#endif

	if( (fstat(fd,&file_info)==-1) )			{	BMD_FOK(BMD_ERR_DISK);	}

	*buffer=NULL;
	*buffer=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if ( *buffer == NULL )		{	BMD_FOK(NO_MEMORY);	}

	(*buffer)->buf=NULL;
	(*buffer)->buf=(char *)malloc(sizeof(char)*file_info.st_size);
	if( (*buffer)->buf == NULL )	{	BMD_FOK(NO_MEMORY);	}

	(*buffer)->size=file_info.st_size;
	memset((*buffer)->buf,0,(*buffer)->size);
	nread=read(fd,(*buffer)->buf,(*buffer)->size);
	if ( nread == -1 )		{	BMD_FOK(NO_MEMORY);	}

	close(fd);
	return BMD_OK;
}

/**
 * Funkcja wypełnia węzeł struktury GenBuf_t (buffer) zawartością pliku (filename)
 * zapisanej w formie szerokich znaków (wchar_t).

 * @param[in] filename - nazwa pliku.
 * @param[out] buffer - adres struktury GenBuf_t.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_DISK - błąd otwarcia pliku.
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
*/

#ifdef WIN32

long bmd_load_binary_content_wide(const wchar_t *filename, GenBuf_t **buffer)
{
GenBuf_t *tmpBuffer=NULL;
long fd=-1;
struct stat file_info;
long nread=-1;



	if( (fd=_wopen(filename, O_RDONLY|_O_BINARY)) == -1)
	{
		PRINT_ERROR("LIBBMDUTILSERR Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}

	if( (fstat(fd,&file_info)==-1) )
	{
		close(fd);
		PRINT_ERROR("LIBBMDUTILSERR Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}

	tmpBuffer=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
	if ( tmpBuffer == NULL )
	{
		close(fd);
		PRINT_ERROR("LIBBMDUTILSERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}

	tmpBuffer->buf=(char *)calloc(file_info.st_size, sizeof(char));
	if( tmpBuffer->buf == NULL )
	{
		free(tmpBuffer);
		close(fd);
		PRINT_ERROR("LIBBMDUTILSERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}

	tmpBuffer->size=file_info.st_size;
	nread=read(fd,tmpBuffer->buf, tmpBuffer->size);
	if ( nread == -1 )
	{
		free(tmpBuffer->buf);
		free(tmpBuffer);
		close(fd);
		PRINT_ERROR("LIBBMDUTILSERR Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}
	close(fd);

	*buffer=tmpBuffer;
	tmpBuffer=NULL;
	return BMD_OK;
}

#endif //ifdef WIN32


long bmd_file_move(	char *source_path,
			char *dest_path)
{
GenBuf_t *buffer	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (source_path==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dest_path==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_load_binary_content(source_path, &buffer));
	BMD_FOK(bmd_save_buf(buffer, dest_path));
	BMD_FOK(unlink(source_path));

	/************/
	/* porzadki */
	/************/
	free_gen_buf(&buffer);

	return BMD_OK;
}


/**
 * Funkcja zwalnia elementy struktury SubstringsGenBuf_t.

 * @param[in] substrings_genbuf - adres struktury SubstringsGenBuf.
 * @return status zakończenia operacji.
 * @retval 0 - zakończenie pomyślne.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
*/

long free_substrings(SubstringsGenBuf_t *substrings_genbuf)
{
	long i = 0;
	long err = 0;
	for (i=0; i<substrings_genbuf->no_of_substrings; i++)
	{
		free(substrings_genbuf->substrings_table[i]);
		substrings_genbuf->substrings_table[i] = NULL;
	}
	free(substrings_genbuf->substrings_table);
	substrings_genbuf->substrings_table = NULL;
	substrings_genbuf->no_of_substrings = 0;

	return err;
}

/**
 * Funkcja dokonuje zrzutu zawartości węzła GenBuf_t (buf) do pliku o nazwie (filename).

 * @param[in] buf - bufor źródłowy.
 * @param[in,out] filename - nazwa pliku docelowego.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_PARAM1 - niepoprawny adres węzła GenBuf_t (buf) lub
 *                          niepoprawna zawartość węzła GenBuf_t (buf) lub
 * 		            niepoprawny rozmiar zawartości węzła GenBuf_t (buf).
 * @retval BMD_ERR_PARAM2 - niepoprawna nazwa pliku docelowego (filename).
 * @retval BMD_ERR_DISK - błąd deskryptora pliku (filename) lub
 *		          błąd zapisu do pliku (filename).
 * @retval BMD_ERR_PARAM3 - niepoprawny adres struktury GenBuf_t.
 * @retval BMD_ERR_PARAM3 - struktura ma już przydzielona pamięć.
*/
long bmd_save_buf(	GenBuf_t *buf,
			char *filename)
{
	long fd;
	long nwrite;

	PRINT_INFO("LIBBMDUTILSINFO bmd_save_buf\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(buf->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(buf->size<0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(filename==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	fd=open(filename,O_CREAT|O_TRUNC|O_BINARY|O_WRONLY,0666);
	if(fd<0)		{	perror("error ");BMD_FOK(BMD_ERR_DISK);	}

	nwrite=write(fd,buf->buf,buf->size);
	if(nwrite!=buf->size)
	{
		close(fd);
		BMD_FOK(BMD_ERR_DISK);
	}

	close(fd);

	return BMD_OK;
}

#ifdef WIN32
/**
 * Funkcja dokonuje zrzutu zawartości GenBuf_t (buf) do pliku o nazwie (filename).

 * @param[in] buf - bufor źródłowy.
 * @param[in,out] filename - nazwa pliku docelowego.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_PARAM1 - niepoprawny adres węzła GenBuf_t (buf) lub
 *                          niepoprawna zawartość węzła GenBuf_t (buf) lub
 * 		            niepoprawny rozmiar zawartości węzła GenBuf_t (buf).
 * @retval BMD_ERR_PARAM2 - niepoprawna nazwa pliku docelowego (filename).
 * @retval BMD_ERR_DISK - błąd deskryptora pliku (filename) lub
 *		          błąd zapisu do pliku (filename).
 * @retval BMD_ERR_PARAM3 - niepoprawny adres struktury GenBuf_t.
 * @retval BMD_ERR_PARAM3 - struktura ma już przydzielona pamięć.
*/
long bmd_save_buf_wide(GenBuf_t *buf,wchar_t *filename)
{
	long fd;
	long nwrite;

	if(buf==NULL)
		return BMD_ERR_PARAM1;
	if(buf->buf==NULL)
		return BMD_ERR_PARAM1;
	if(buf->size<0)
		return BMD_ERR_PARAM1;

	if(filename==NULL)
		return BMD_ERR_PARAM2;

	fd=_wopen(filename,O_CREAT|O_TRUNC|O_BINARY|O_WRONLY,0666);
	if(fd<0)
		return BMD_ERR_DISK;

	nwrite=write(fd,buf->buf,buf->size);
	if(nwrite!=buf->size)
		return BMD_ERR_DISK;

	close(fd);

	return BMD_OK;
}
#endif //ifdef WIN32


/**
by WSZ
funkcja bmd_strmerge laczy dwa lancuchy znakowe (konkatenacja jest dynamicznie alokowana)
Funckcje mozna wykorzystac rowniez do kopiowania lancucha (jako drugi argument wystarczy podac pusty string)
@arg str1 to pierszy konkatenowany lancuch znakowy
@arg str2 to drugi konkatenowany lancuch znakowy
@arg dest to adres wskaznika na alokowana konkatenacje dwoch przekazywanych lancuchow znakowych

@retval 0 jesli operacja sie powiodla
@retval -1 jesli za str1 przekazany NULL
@retval -2 jesli za str2 przekazany NULL
@retval -3 jesli za dest przekazany NULL
@retval -4 jesli wartosc wyluskana z dest nie jest NULL
@retval -5 jesli wystapil problem z alokacja pamieci

*/
long bmd_strmerge(char *str1, char *str2, char **dest)
{
	char *new_Str=NULL;
	long len_str1=0;
	long len_str2=0;
	long len=0;
	long iter=0;

	if(str1 == NULL)
		{ return -1; }
	if(str2 == NULL)
		{ return -2; }
	if(dest == NULL)
		{ return -3; }
	if(*dest != NULL)
		{ return -4; }

	len_str1=(int)strlen(str1);
	len_str2=(int)strlen(str2);
	len=len_str1+len_str2+1;

	new_Str=(char*)calloc(len+1, sizeof(char));
	if(new_Str == NULL)
		{ return -5; }

	for(iter=0; iter<len; iter++)
	{
		if(iter<len_str1)
		{
			new_Str[iter]=str1[iter];
		}
		else
		{
			new_Str[iter]=str2[iter-len_str1];
		}
	}

	*dest=new_Str;
	return 0;
}


/**
by WSZ
funkcja get_file_line() zwraca kolejne linie pliku
@arg plik to wskaznik na przygotowana strukture FILE (plik otwarty za pomoca fopen badz wynik wxFFile::fp() )

@retval jednalinie pliku gdy sukces (jesli zakonczenia windows, to linia ze znakiem \r na koncu)
@retval	 "\n" dla pustej lini (\n lub \r\n)
@retval	 NULL, jesli EOF
*/
char* get_file_line(FILE *plik)
{
	char *temp=NULL;
	char *znak=NULL;
	char *buff=NULL;

	if( plik == NULL)
		{ return NULL; }

	if( (bmd_strmerge(" \0","", &znak)) != 0)
		{ return NULL; }
	if( (bmd_strmerge(" \0","", &buff)) != 0 )
	{
		free(znak);
		return NULL;
	}

	if((znak[0]=(char)fgetc(plik))==EOF)
	{
		free(buff);
		free(znak);
		return NULL;
	}
	else
	{
		buff[0]=znak[0];
		if( (strcmp(buff,"\r")) == 0 )
		{
			if((znak[0]=(char)fgetc(plik))!=EOF)
			{
				if( (strcmp(znak,"\n")) ==0)
				{
					free(znak);
					buff[0]='\n';
					return buff;
				}
				else
				{
					if( (bmd_strmerge(buff, znak, &temp)) != 0)
					{
						free(buff);
						free(znak);
						return NULL;
					}
					free(buff);
					buff=temp;
					temp=NULL;
				}
			}
			else
			{
				free(znak);
				return buff;
			}
		}
		else
		{
			if( (strcmp(buff, "\n")) == 0)
			{
				free(znak);
				return buff;
			}
		}

		while((znak[0]=(char)fgetc(plik))!='\n')
		{
			if(znak[0]!=EOF)
			{
				if( (bmd_strmerge(buff, znak, &temp)) != 0 )
				{
					free(znak);
					free(buff);
					return NULL;
				}
				free(buff); buff=NULL;
				buff=temp; temp=NULL;
			}
			/*jesli zlapie EOF w trakcie odczytu lini*/
			else
				{ break; }
		}
		free(znak);
		return buff;
	}
}

/**
Funkcja bmd_merge_genbufs() sluzy do laczenia dwoch buforow w jeden.
@note
jesli oba bufory puste. funkcja alokuje pusty bufor
jesli jeden pelny bufor, funkcja trworzy jego kopie
jesli oba bufory wypelnione, to funkcja laczy

@arg gb1 to wskaznik na pierwszy bufor do zlaczenia
@arg gb2 to wskaznik na drugi bufor do polaczenia
@arg output to adres wskaznika, pod ktorym zaalokowany zostanie "polaczony" bufor

@retval 0 w przypadku sukcesu
@retval -1 jesli za gb1 podano NULL
@retval -2 jesli pole gb1->buff jest NULLem a gb1->size jest wieksze od zera
@retval -3 jesli za gb2 podano NULL
@retval -4 jesli pole gb2->buff jest NULLem a gb2->size jest wieksze od zera
@retval -5 jesli wystapil problem z alokacja pamieci
@retval -6,-7,-8,-9 jesli wystapil problem z alokacja pamieci
*/
long bmd_merge_genbufs(	GenBuf_t *gb1,
				GenBuf_t* gb2,
				GenBuf_t **output)
{
GenBuf_t *new_gb	= NULL;
long ret_val		= 0;


	if(gb1 == NULL)					{	return -1;	}
	if(gb1->buf == NULL && gb1->size > 0)	{	return -2;	}
	if(gb2 == NULL)					{	return -3;	}
	if(gb2->buf == NULL && gb2->size > 0)	{	return -4;	}

	if(gb1->size == 0 && gb2->size == 0) //oba bufory puste
	{
		new_gb=(GenBuf_t*)calloc(1, sizeof(GenBuf_t)); //pola buf i size ustawione na 0
		if(new_gb == NULL)			{	return -5;	}
		else
		{
			*output=new_gb;
			return 0;
		}
	}

	if(gb1->size > 0 && gb2->size == 0 ) //tylko wypelniony pierwszy bufor
	{
		ret_val=set_gen_buf2(gb1->buf, gb1->size, &new_gb);
		if(ret_val != 0)				{	return -6;	}
		else
		{
			*output=new_gb;
			return 0;
		}
	}

	if(gb1->size == 0 && gb2->size > 0) //tylko wypelniony drugi bufor
	{
		ret_val=set_gen_buf2(gb2->buf, gb2->size, &new_gb);
		if(ret_val != 0)
			{ return -7; }
		else
		{
			*output=new_gb;
			return 0;
		}
	}

	//oba bufory pelne
	new_gb=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_gb == NULL)
		{ return -8; }
	new_gb->size=gb1->size + gb2->size;
	new_gb->buf=(char*)calloc(new_gb->size, 1);
	if( new_gb->buf == NULL )
	{
		free(new_gb);
		return -9;
	}
	memcpy(new_gb->buf, gb1->buf, gb1->size);
	memcpy( new_gb->buf+gb1->size, gb2->buf, gb2->size);

	*output=new_gb;
	return 0;
}

#ifndef WIN32
long remove_dir(char *location)
{
DIR *ptr				= NULL;
struct dirent *dirp		= NULL;
char *path				 =NULL;

	ptr=opendir(location);
	if (ptr==NULL)
	{
		BMD_FOK(-1);
	}
	while((dirp = readdir(ptr))!=NULL)
	{
		if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0)) continue;
		asprintf(&path, "%s/%s",location,dirp->d_name);
		unlink(path);
	}
	closedir(ptr);

	remove(location);
	return BMD_OK;
}

long count_files_in_dir(char *location)
{
DIR *ptr				= NULL;
struct dirent *dirp		= NULL;
long count				= 0;

	ptr=opendir(location);
	if (ptr==NULL)
	{
		return -1;
	}
	while((dirp = readdir(ptr))!=NULL)
	{
		if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0)) continue;
		count++;
	}
	closedir(ptr);

	return count;
}
#endif /*ifndef WIN32*/


long load_translation(char *translate_file, translate_t **translate, long *translate_count)
{
FILE *fd		= NULL;
char line[500];
char **pair		= NULL;
long pair_count		= 0;
long i			= 0;

	(*translate_count)=0;

	if ((fd=fopen(translate_file,"r"))==NULL)
	{
		return -1;
	}
	else
	{
		for(;;)
		{
			strcpy(line, "");
			if (fgets(line,500,fd)==NULL)	{	break;	}
			if (strlen(line)==0)	{	break;	}

			if ((line[0]=='#') || (line[0]==' ') || (strlen(line)<=0))
			{
				continue;
			}

			bmd_strsep(line, ':', &pair, &pair_count);

			if (pair_count>=2)
			{
				pair[pair_count-1][strlen(pair[pair_count-1])-1]='\0';
				(*translate_count)++;
				(*translate)=(translate_t *)realloc((*translate), sizeof(translate_t)*(1+(*translate_count)));
				asprintf(&((*translate)[(*translate_count)-1].from), "%s", pair[0]);

				for (i=1; i<pair_count; i++)
				{
					if (i==1)
					{
						asprintf(&((*translate)[(*translate_count)-1].to), "%s", pair[i]);
					}
					else
					{
						asprintf(&((*translate)[(*translate_count)-1].to), "%s:%s", (*translate)[(*translate_count)-1].to, pair[i]);
					}
				}
			}
			bmd_strsep_destroy(&pair, pair_count);
		}
	}
	fclose(fd);

	return BMD_OK;
}
