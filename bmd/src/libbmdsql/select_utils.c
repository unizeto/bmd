#include <bmd/libbmdsql/select.h>
#include <time.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>
#include <bmd/libbmderr/libbmderr.h>

/* podajemy wypelnione sloty OIDa (tablica long) i ich ilosc,*/
/* f. zwraca string z nazwa kolumny w tabeli, ktora odpowiada danemu oidowi */
/* Funkcja uzywana jest przy zapytaniu SELECT w celu konwersji OIDow zwiazanych z polami */
/* metadanych na nazwy odpowiednich kolumn w tabei 					 */
long OID_to_column_name(	char *OIDTableBuf,
					char **column_name)
{
long err								= 0;
char oid_sys_metadata_crypto_objects_filename[]		= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME};
char oid_sys_metadata_crypto_objects_timestamp[]	= {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP};
char oid_sys_metadata_crypto_objects_filesize[]		= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE};

char oid_sys_metadata_visible[]				= {OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE};
char oid_sys_metadata_present[]				= {OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT};
char oid_sys_metadata_file_type[]				= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE};
char oid_sys_metadata_corresponding_id[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID};

char oid_sys_metadata_crypto_objects_filesize_less[]	= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_MORE};
char oid_sys_metadata_crypto_objects_filesize_more[]	= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_LESS};
char oid_sys_metadata_crypto_objects_timestamp_less[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE};
char oid_sys_metadata_crypto_objects_timestamp_more[] = {OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS};

// char *add_metadata_type = NULL;

	if(OIDTableBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(column_name == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*column_name != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/* porownanie OIDu z wzorcami i przypisanie column_name odpowiedniego stringu */
	if (!strcmp(OIDTableBuf, oid_sys_metadata_visible))
	{
			asprintf(column_name, " crypto_objects.visible ");
	}
	else if (!strcmp(OIDTableBuf, oid_sys_metadata_present))
	{
			asprintf(column_name, " crypto_objects.present ");
	}
	else if (!strcmp(OIDTableBuf, oid_sys_metadata_file_type))
	{
			asprintf(column_name, " crypto_objects.file_type ");
	}
	else if (!strcmp(OIDTableBuf,oid_sys_metadata_corresponding_id))
	{
			asprintf(column_name, " crypto_objects.corresponding_id ");
	}
	else if (!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_filename))
	{
			asprintf(column_name, " crypto_objects.name ");
	}
	else if (	!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_timestamp) ||
			!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_timestamp_less) ||
			!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_timestamp_more) )
	{
			asprintf(column_name, " crypto_objects.insert_date ");
	}
	else if (	!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_filesize) ||
			!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_filesize_less) ||
			!strcmp(OIDTableBuf, oid_sys_metadata_crypto_objects_filesize_more))
	{
		asprintf(column_name, " crypto_objects.filesize ");
	}
	/* kazdy nie dopasowany wczesniej OID jest traktowany jako potencjalny plug-in - czyli additional metadata */
	else
	{
// 			add_metadata_type = ulong_table_to_string(OIDTableBuf, OIDTableBufSize);
			asprintf(column_name, " add_metadata_types.type=\'%s\' AND add_metadata.add_metadata ", OIDTableBuf);
// 			if(add_metadata_type) {free(add_metadata_type); add_metadata_type=NULL;}
	}
	return err;
}

char *ulong_table_to_string(	long *OIDTableBuf,
					long OIDTableBufSize)
{
long no_of_slots=OIDTableBufSize/sizeof(long);
long i				= 0;
char *oid_string		= NULL;
char *oid_string_tmp 	= NULL;

	asprintf(&oid_string, "%li", OIDTableBuf[0]);
	if(oid_string == NULL)
		return NULL;

	for(i=1; i<no_of_slots; i++)
	{
		asprintf(&oid_string_tmp, "%s.%li", oid_string, OIDTableBuf[i]);
		if(oid_string_tmp == NULL)
		{
			return NULL;
		}
		/* dotychczasowy tmp juz nie potrzebny */
		free(oid_string); oid_string=NULL;

		/* utworz nowy tmp */
		oid_string = (char *) malloc (1+strlen(oid_string_tmp));
		if(oid_string == NULL)
		{
			return NULL;
		}
		strcpy(oid_string, oid_string_tmp);
		/* dotychczasowy oid_string juz nie potrzebny */
		free(oid_string_tmp); oid_string_tmp=NULL;
	}
	return oid_string;
}

long ulong_table_to_string2(	long *OIDTableBuf,
					long OIDTableBufSize,
					char **string)
{
long no_of_slots=OIDTableBufSize/sizeof(long);
long i				= 0;
char *temp_string		= NULL;

	if(string == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM3);
	}

	if ((*string)!=NULL)
	{
		free(*string); (*string)=NULL;
	}

	for(i=0; i<no_of_slots; i++)
	{
		if (i==0)
		{
			asprintf(string, "%li", OIDTableBuf[i]);
		}
		else
		{
			asprintf(&temp_string, "%s.%li", (*string), OIDTableBuf[i]);
			free(*string); *string = NULL;
			*string = temp_string; temp_string = NULL;
		}

	}

	if(*string == NULL)
	{
		BMD_FOK(NO_MEMORY);
	}

	return BMD_OK;
}

long ConcatenateStrings(char **src, char *add)
{
char *tmp = NULL;
	if(src == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM1);
	}
	if(add == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM2);
	}

	if(*src == NULL){
		asprintf(src, "%s", add);
		if(*src == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}
	else {
		asprintf(&tmp,"%s", *src);
		{
			BMD_FOK(NO_MEMORY);
		}
		free(*src); *src=NULL;

		asprintf(src, "%s%s", tmp, add);
		if(*src == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		free(tmp); tmp = NULL;
	}
	return BMD_OK;
}

long OIDsufixToOperatorType(char *OIDStringSufix, char **relation_operator)
{
	if(OIDStringSufix == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(relation_operator == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*relation_operator != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(!strcmp(OIDStringSufix, "1"))
	{
		asprintf(relation_operator, "%s", "<");
		if(*relation_operator == NULL)	{	BMD_FOK(NO_MEMORY);	}
	}
	else if(!strcmp(OIDStringSufix, "2")){
		asprintf(relation_operator, "%s", ">");
		if(*relation_operator == NULL)	{	BMD_FOK(NO_MEMORY);	}
	}
	else
	{
		asprintf(relation_operator, "%s", "=");
		if(*relation_operator == NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	return BMD_OK;
}

long SplitOidStringToPrefixAndSufix(	char *OIDstring,
					char **OIDstringPrefix,
					char **OIDstringSufix)
{
char *c			= NULL;
char *last_dot		= NULL;
char *tmpOIDstring	= NULL;
long dot_offset		= 0;
long count		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(OIDstring == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(OIDstringPrefix == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*OIDstringPrefix != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(OIDstringSufix == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*OIDstringSufix != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}


	asprintf(&tmpOIDstring, "%s", OIDstring);
	if (tmpOIDstring==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/*Znajdz lokalizacje ostatniej kropki*/
	c = tmpOIDstring;
	while(*c != 0)
	{
		if(*c == '.')
		{
			last_dot = c;
			dot_offset = count;
		}
		c++;
		count++;
	}
	*(tmpOIDstring+dot_offset) = 0;
	/* w miejsce ostatniej kropki wstaw NULL i uzyskaj w ten sposob 2 stringi */
	asprintf(OIDstringPrefix, "%s", tmpOIDstring);
	if(*OIDstringPrefix == NULL)	{	BMD_FOK(NO_MEMORY);	}

	asprintf(OIDstringSufix, "%s", tmpOIDstring+dot_offset+1);
	if(*OIDstringSufix == NULL)	{	BMD_FOK(NO_MEMORY);	}

	/************/
	/* porzadki */
	/************/
	free0(tmpOIDstring);


	return BMD_OK;
}




