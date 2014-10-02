#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdasn1_common/DigestInfo.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#define ULONG_OID_TABLE_TOO_SHORT -12

// oidt - wskazanie na strukture identyfikatora
// pOIDTab - tablica z OIDem
// pOIDTabSizeInBytes - rozmiar tablicy w bajtach
// 0 - rowne
// 1 - rozne
// <0- kod bledu

long OBJECT_IDENTIFIER_cmp(	OBJECT_IDENTIFIER_t *oidt,
					char *pOIDTab)
{
long *oidt_cmp		= NULL;
char *oid_string		= NULL;
long oidt_size		= 0;
long res			= 0;

	if (oidt == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	oidt_size = OID_to_ulong_alloc2(oidt, &oidt_cmp);
	if ( oidt_cmp == NULL )		{	BMD_FOK(BMD_ERR_MEMORY);	}


	BMD_FOK(ulong_to_str_of_OID2(	oidt_cmp, oidt_size, &oid_string));

	if ( strcmp(oid_string, pOIDTab)!=0 )
	{
		res = 1;
	}
	else
	{
		res = 0;
	}

	free(oidt_cmp); oidt_cmp=NULL;
	free(oid_string); oid_string=NULL;

	return res;
}

/* Opakowane OID_to_ulong z bmdutils o alokator pamieci	*/
/* Funkcja pobiera typ OBJECT_IDENTIFIER_t i zamienia go na tablice 	*/
/* alokujac przy tym pamiec. Funkcja zwraca rozmiar zaalokowanej tablicy*/
/* lub wartosc ujemna w przypadku bledow.			*/
long OID_to_ulong_alloc2(	OBJECT_IDENTIFIER_t *oid,
					long **arcs)
{
long err		= 0;

	*arcs = (long *) malloc ( sizeof(long) * 2);
	if(*arcs == NULL)		{	BMD_FOK(NO_MEMORY);	}
	err =  OID_to_ulong2(oid, arcs, sizeof(long), 2);

	return err;
}

/*************************************************************************
* Zamiana OBJECT_IDENTIFIER_t na tablice long	 		 *
*************************************************************************/
/* Funkcja zbudowana na bazie przykladu Lev'a Walkina, umieszczonego 	*/
/* w OBJECT_IDENTIFIER.h						*/

/* Do funkcji nalezy podac tablice arcs zawierajaca min 2 sloty		*/
/* tak by mozna bylo w niej umiescic cyfry OIDu				*/
/* !UWAGA! tablica arcs powinna byc zaalokowana dynamicznie, gdyz 	*/
/* w razie zbyt malej ilosci slotow jest ona powiekszana realloc'em	*/
long OID_to_ulong2(	OBJECT_IDENTIFIER_t *oid,
				long **arcs,
				long arc_type_size, long arc_slots)
{
long count			= 0;	                            /* ilosc wypelnionych slotow */
long *tmp			= NULL;

	count = OBJECT_IDENTIFIER_get_arcs(oid, *arcs, arc_type_size, arc_slots);

	/* W razie potrzeby powieksz tablice arcs i ponownie skonwertuj OID */
	if(count > arc_slots)
	{
		arc_slots = count;
		tmp = realloc(*arcs, arc_type_size * arc_slots);
		if(tmp == NULL)
		{
			free(*arcs);
			*arcs = NULL;
			BMD_FOK(NO_MEMORY);
		}
		else
		{
			*arcs = tmp;
		}
		count = OBJECT_IDENTIFIER_get_arcs(oid, *arcs, arc_type_size, arc_slots);

		if(count != arc_slots)
		{
			BMD_FOK(-20); /* FIXME assign error number */
		}
	}

	return count;
}

long ulong_to_str_of_OID2(	long *OIDtable,
					long OIDtable_size,
					char **OIDstring)
{
long err				= 0;
long i				= 0;
char *number			= NULL;
char *tmp			= NULL;
long no_of_slots	= OIDtable_size;

	if(OIDtable == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(OIDtable_size < 2)			{	BMD_FOK(ULONG_OID_TABLE_TOO_SHORT);	}
	if(OIDstring == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*OIDstring != NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	for(i=0; i< no_of_slots; i++)
	{
		asprintf(&number, "%li", OIDtable[i]);
		if(number == NULL)		{	BMD_FOK(NO_MEMORY);	}

		if(i==0)
		{	/* pierwsza cyfra OID */
			*OIDstring = (char *)malloc(sizeof(char) * (1 + strlen(number)) );
			if(*OIDstring == NULL)	{	BMD_FOK(NO_MEMORY);	}
			memset(*OIDstring, 0, sizeof(char) * (1 + strlen(number)) );
			strcpy(*OIDstring, number);
		}
		else
		{
			tmp = (char *) realloc (*OIDstring, 2+strlen(*OIDstring)+strlen(number));
			if(tmp == NULL)
			{
				free(*OIDstring); OIDstring=NULL;
				BMD_FOK(NO_MEMORY);
			}
			else
			{
				*OIDstring = tmp;
			}
			strcat(*OIDstring, ".");
			strcat(*OIDstring, number);
		}
		free(number); number=NULL;
	}

	return err;
}

long OID_cmp2(	void *oid1,
			long sizeoid1,
			void *oid2,
			long sizeoid2)
{
	if (oid1 == oid2)
	{
		return 0; /* szybki test wskaznikow */
	}
	if (oid1 == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (oid2 == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (sizeoid1 != sizeoid2)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	return memcmp(oid1, oid2, sizeoid1);
}



/************************
* Konwertery do stringu *
************************/
long OBJECT_IDENTIFIER2string(	OBJECT_IDENTIFIER_t *OBJECT_IDENTIFIER,
					char **OidString)
{
long err		= 0;
long *OIDtable = NULL;

	err=OID_to_ulong_alloc2(OBJECT_IDENTIFIER, &OIDtable);
	if (err<BMD_OK)	{	BMD_FOK(err);	}
	BMD_FOK(ulong_to_str_of_OID2(OIDtable, err/**sizeof(long)*/, OidString));

	free(OIDtable); OIDtable=NULL;

	return BMD_OK;
}

long INTEGER2string(	INTEGER_t *Integer,
				char **string)
{
long i	= 0;

	/* 3 znaki na reprezentacje bajtu - dwa na cyfry i jeden na spacje */
	*string = (char *) malloc (sizeof(char) * (Integer->size) * 3 );
	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	for(i=0; i<Integer->size; i++)
	{
		if(i==0)
		{
			sprintf(*string, "%02X", Integer->buf[i]);
		}
		else if(i==1)
		{
			sprintf((*string)+i*2, " %02X", Integer->buf[i]);
		}
		else
		{
			sprintf((*string)+i*3-1, " %02X", Integer->buf[i]);
		}
	}

	return BMD_OK;
}

long AnotherName2string(	AnotherName_t *AnotherName,
				char **string)
{
	if(AnotherName == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* TODO */
	asprintf(string, "AnotherName present but reading it not implemented yet!\n");
	if(*string == NULL)			{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}

long OCTET_STRING2string(	OCTET_STRING_t *OCTET_STRING,
					char **string)
{
	if (OCTET_STRING == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*string) != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	*string = (char *) malloc (sizeof(char) * OCTET_STRING->size+1);
	if(*string == NULL)			{	BMD_FOK(NO_MEMORY);	}

	memset(*string, 0, OCTET_STRING->size+1);
	memcpy(*string, OCTET_STRING->buf, OCTET_STRING->size);

	return BMD_OK;
}

long ORAddress2string(	ORAddress_t *ORAddress,
				char **string)
{
	if(ORAddress == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* TODO */
	asprintf(string, "ORAddress present but reading it not implemented yet!\n");
	if(*string == NULL)			{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}

long DirectoryString2string(	DirectoryString_t *DirectoryString,
					char **string)
{
/*long err = 0;*/

	/************************/
	/* walidacja parametrow */
	/************************/
	if (DirectoryString == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(DirectoryString->present)
	{
		case DirectoryString_PR_teletexString:
			BMD_FOK_CHG(OCTET_STRING2string(&(DirectoryString->choice.teletexString), string),
					LIBBMDASN1_COMMON_UTILS_CONV_TELETEX_STR_TO_STR_ERR);
			break;
		case DirectoryString_PR_printableString:
			BMD_FOK_CHG(OCTET_STRING2string(&(DirectoryString->choice.printableString), string),
					LIBBMDASN1_COMMON_UTILS_CONV_PRINTABLE_STR_TO_STR_ERR);
			break;
		case DirectoryString_PR_universalString:
			BMD_FOK_CHG(OCTET_STRING2string(&(DirectoryString->choice.universalString), string),
					LIBBMDASN1_COMMON_UTILS_CONV_UNIVERSAL_STR_TO_STR_ERR);
			break;
		case DirectoryString_PR_utf8String:
			BMD_FOK_CHG(OCTET_STRING2string(&(DirectoryString->choice.utf8String), string),
					LIBBMDASN1_COMMON_UTILS_CONV_UTF8_STR_TO_STR_ERR);
			break;
		case DirectoryString_PR_bmpString:
			BMD_FOK_CHG(OCTET_STRING2string(&(DirectoryString->choice.bmpString), string),
					LIBBMDASN1_COMMON_UTILS_CONV_BMD_STR_TO_STR_ERR);
			break;
		case DirectoryString_PR_NOTHING:
		default:
			asprintf(string, "No string present in DirectoryString Field!\n");
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
	}

	return BMD_OK;
}

long AttributeTypeAndValue2string(	AttributeTypeAndValue_t *AttributeTypeAndValue,
						char **string)
{
long err	= 0;
long i	= 0;
char OID_id_at_CommonName[]				= {OID_ID_AT_COMMONNAME};
char OID_id_at_Surname[]				= {OID_ID_AT_SURNAME};
char OID_id_at_SerialNumber[]				= {OID_ID_AT_SERIALNUMBER};
char OID_id_at_Country[]				= {OID_ID_AT_COUNTRY};
char OID_id_at_Locality[]				= {OID_ID_AT_LOCALITY};
char OID_id_at_StateOrProvince[]			= {OID_ID_AT_STATEORPROVINCE};
char OID_id_at_Organization[]				= {OID_ID_AT_ORGANIZATION};
char OID_id_at_OrganizationalUnit[]			= {OID_ID_AT_ORGANIZATIONALUNIT};
char OID_id_at_Title[]					= {OID_ID_AT_TITLE};
char OID_id_at_Description[]				= {OID_ID_AT_DESCRIPTION};
char OID_id_at_GivenName[]				= {OID_ID_AT_GIVENNAME};
char OID_id_at_GenerationQualifier[]		= {OID_ID_AT_GENERATIONQUALIFIER};
char OID_id_at_DistinguishedNameQualifier[]	= {OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER};
char OID_id_at_Pseudnym[]				= {OID_ID_AT_PSEUDONYM};
char OID_id_at_postalAddress[]			= {OID_ID_AT_POSTALADDRESS};
char OID_id_at_emailAddress[]				= {OID_ID_AT_EMAILADDRESS};

char *LdapPrefix				= NULL;
char *LdapSufix				= NULL;
char *tmp_string				= NULL;
char *tmp2_string				= NULL;
DirectoryString_t *DirectoryString	= NULL;
PostalAddress_t *PostalAddress	= NULL;
EmailAddress_t *EmailAddress		= NULL;

	if(AttributeTypeAndValue == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL || *string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}


	if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_CommonName))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_COMMONNAME_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Surname))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_SURNAME_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_SerialNumber))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_SERIALNUMBER_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Country))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_COUNTRY_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Locality))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_LOCALITY_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_StateOrProvince))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_STATEORPROVINCE_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Organization))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_ORGANIZATION_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_OrganizationalUnit))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_ORGANIZATIONALUNIT_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Title))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_TITLE_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Description))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_DESCRIPTION_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_GivenName))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_GIVENNAME_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_GenerationQualifier))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_GENERATIONQUALIFIER_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_DistinguishedNameQualifier))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_Pseudnym))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_PSEUDONYM_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_postalAddress))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_POSTALADDESS_LDAP_PREFIX);
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type), OID_id_at_emailAddress))
	{
		asprintf(&LdapPrefix, "%s", OID_ID_AT_EMAILADDRESS_LDAP_PREFIX);
	}
	else					{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_ATTRIBUTE_OID);	}

	if(LdapPrefix == NULL)		{	BMD_FOK(NO_MEMORY);	}


	/* Dla postalAddress nie istnieje pojedynczy DirectoryString lecz ich sekwencja
	   skladajaca sie na typ zlozony PostalAddress_t */
	if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_postalAddress))
	{
		BMD_FOK(AttributeValue2PostalAddress(&(AttributeTypeAndValue->value), &PostalAddress));

		asprintf(&tmp_string, "%s<", LdapPrefix);
		if(tmp_string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		for(i=0; i<PostalAddress->list.count; i++)
		{
			BMD_FOK(DirectoryString2string(PostalAddress->list.array[i], &LdapSufix));
			if(i==0)
			{
				asprintf(&tmp2_string, "%s%s", tmp_string, LdapSufix);
			}
			else
			{
				asprintf(&tmp2_string, "%s %s", tmp_string, LdapSufix);
			}
			if(tmp2_string == NULL)		{	BMD_FOK(NO_MEMORY);	}


			free(LdapSufix); LdapSufix=NULL;
			free(tmp_string); tmp_string=NULL;
			asprintf(&tmp_string, "%s", tmp2_string);
			free(tmp2_string); tmp2_string=NULL;
		}
		asprintf(string, "%s>", tmp_string);
		if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		free(tmp_string); tmp_string=NULL;

		asn_DEF_PostalAddress.free_struct(&asn_DEF_PostalAddress, PostalAddress, 0);
		free(LdapPrefix); LdapPrefix=NULL;
	}
	else if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_emailAddress))
	{
		err = AttributeValue2IA5String(&(AttributeTypeAndValue->value), &EmailAddress);
		if(err<0)			{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATTRIBUTE);	}

		err = OCTET_STRING2string(EmailAddress, &LdapSufix);
		if(err<0)			{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_EMAIL_TO_STRING_FROM_ATTRIBUTE);	}

		asprintf(string, "%s<%s>", LdapPrefix, LdapSufix);
		if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, EmailAddress, 0);
		free(LdapPrefix); LdapPrefix=NULL;
		free(LdapSufix); LdapSufix=NULL;
	}
	else
	{ /* Wszystkie inne typy maja obsluge generyczna */
		BMD_FOK(AttributeValue2DirectoryString(&(AttributeTypeAndValue->value), &DirectoryString));
		BMD_FOK(DirectoryString2string(DirectoryString, &LdapSufix));

		asprintf(string, "%s<%s>", LdapPrefix, LdapSufix);
		if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		asn_DEF_DirectoryString.free_struct(&asn_DEF_DirectoryString, DirectoryString, 0);
		/* Dla postalAddress to jest niepoprawne */
		free(LdapPrefix); LdapPrefix=NULL;
		free(LdapSufix); LdapSufix=NULL;
	}

	return BMD_OK;
}

long RelativeDistinguishedName2string(	RelativeDistinguishedName_t *RelativeDistinguishedName,
							char **string)
{
long i			= 0;
char *tmpString	= NULL;
char *oldString	= NULL;

	if(RelativeDistinguishedName == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL || *string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* lista RelativeDistinguishedName zawiera tylko jeden element! */
	/* ale na wypadek gdyby ktos nie chcial przestrzegac norm	*/
	/* sprawdzam wszystko w forze.					*/
	for(i=0; i<RelativeDistinguishedName->list.count; i++)
	{
		BMD_FOK(AttributeTypeAndValue2string(RelativeDistinguishedName->list.array[i], &tmpString));
		if(i==0)
		{
			asprintf(string, "%s", tmpString);
			if(*string == NULL)		{	BMD_FOK(NO_MEMORY);	}
			free(tmpString); tmpString=NULL;
		}
		else
		{
			asprintf(&oldString, "%s", *string);
			if(oldString == NULL)		{	BMD_FOK(NO_MEMORY);	}
			free(*string); *string=NULL;

			asprintf(string, "%s == %s", oldString, tmpString);
			if(*string == NULL)		{	BMD_FOK(NO_MEMORY);	}

			free(oldString); oldString=NULL;
			free(tmpString); tmpString=NULL;
		}
	}

	return BMD_OK;
}

long RDNSequence2string(	RDNSequence_t *RDNSequence,
				char **string)
{
long i			= 0;
char *tmpString		= NULL;
char *oldString		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (RDNSequence == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* lista zawiera tyle elementow ile jest pol nazwy wyroznionej DN */
	for(i=0; i<RDNSequence->list.count; i++)
	{
		BMD_FOK_CHG(RelativeDistinguishedName2string(RDNSequence->list.array[i], &tmpString),
				LIBBMDASN1_COMMON_UTILS_CONV_RELATIVE_DISTINGUISHED_NAME_TO_STR);

		if(i==0)
		{
			asprintf(string, "%s", tmpString);
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(tmpString);
		}
		else
		{
			asprintf(&oldString, "%s", *string);
			if(oldString == NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(*string);

			/* Tu mozna dodawac separator oddzielajacy poszczegolne czlony DN */
			asprintf(string, "%s %s", oldString, tmpString);
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

			free0(oldString);
			free0(tmpString);
		}
	}

	return BMD_OK;
}

long RDNSequence2sortedString(	RDNSequence_t *RDNSequence,
				char **string)
{
long i			= 0;
long ile_posortowanych	= 0;
long z			= 0;
char *oldString		= NULL;

char **stringArray=NULL;
char *tmpStringPtr=NULL;
long stringCounter = 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (RDNSequence == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	stringCounter=RDNSequence->list.count;

	if(stringCounter == 0)
		{ return BMD_OK; }

	stringArray=(char**)calloc(stringCounter, sizeof(char*));
	if(stringArray == NULL)
		{ BMD_FOK(NO_MEMORY); }

	for(i=0; i<stringCounter; i++)
	{
		BMD_FOK_CHG(RelativeDistinguishedName2string(RDNSequence->list.array[i], &(stringArray[i])),
				LIBBMDASN1_COMMON_UTILS_CONV_RELATIVE_DISTINGUISHED_NAME_TO_STR);
	}

	if(stringCounter > 1) //sortowanie wykonywac, gdy przynajmniej 2 stringi w tablicy
	{
		ile_posortowanych = 0;
		for(z=0; z<stringCounter-1; z++) //zmian n-1
		{
			for(i=0; i<(stringCounter-1-ile_posortowanych); i++)
			{
				//najwiekszy alfabetycznie na koniec tablicy
				if(strcmp(stringArray[i], stringArray[i+1]) > 0 )
				{
					tmpStringPtr=stringArray[i];
					stringArray[i]=stringArray[i+1];
					stringArray[i+1]=tmpStringPtr;
					tmpStringPtr=NULL;
				}
			}
			ile_posortowanych++;
		}
	}

	/* lista zawiera tyle elementow ile jest pol nazwy wyroznionej DN */
	for(i=0; i<stringCounter; i++)
	{
		if(i==0)
		{
			asprintf(string, "%s", stringArray[i]);
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(stringArray[i]);
		}
		else
		{
			asprintf(&oldString, "%s", *string);
			if(oldString == NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(*string);

			/* Tu mozna dodawac separator oddzielajacy poszczegolne czlony DN */
			asprintf(string, "%s %s", oldString, stringArray[i]);
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

			free0(oldString);
			free0(stringArray[i]);
		}
	}

	free(stringArray); stringArray=NULL;

	return BMD_OK;
}



long Time2string(	Time_t *Time,
			char **string)
{
long err = 0;

	if (Time == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(Time->present)
	{
		case Time_PR_utcTime:
			err = UTCTime2string(&(Time->choice.utcTime), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_UTC_TIME_TO_STR);	}
			break;
		case Time_PR_generalTime:
			err = GeneralizedTime2string(&(Time->choice.generalTime), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_GENERALIZED_TIME_TO_STR);	}
			break;
		case Time_PR_NOTHING:
		default:
			BMD_FOK(LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_TIME_TYPE);
		break;
	}

	return BMD_OK;
}

long GeneralizedTime2string(	GeneralizedTime_t *GeneralizedTime,
					char **string)
{
struct tm GMTime;

	if(GeneralizedTime == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	asn_GT2time(GeneralizedTime, &GMTime, 0);
	// flaga 0 na koncu oznacza, ze wartosc UTC ma byc przeliczona na localtime (uwzglednione przesuniecie)
	// dla 1 podana bylaby czysta wartosc UTC bez zadnych przesuniec
	//GMTime = gmtime(&tmpTime);
	asprintf(string, "%d-%s%d-%s%d %s%d:%s%d:%s%d",
		GMTime.tm_year+1900,
		(GMTime.tm_mon+1<10)?"0":"",	GMTime.tm_mon+1,
		(GMTime.tm_mday<10)?"0":"",	GMTime.tm_mday,
		(GMTime.tm_hour<10)?"0":"",	GMTime.tm_hour,
		(GMTime.tm_min<10)?"0":"",	GMTime.tm_min,
		(GMTime.tm_sec<10)?"0":"",	GMTime.tm_sec);
	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}

long UTCTime2string(	UTCTime_t *UTCTime,
				char **string)
{
struct tm GMTime;

	if(UTCTime == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asn_UT2time(UTCTime, &GMTime, 0);

	// flaga 0 na koncu oznacza, ze wartosc UTC ma byc przeliczona na localtime (uwzglednione przesuniecie)
	// dla 1 podana bylaby czysta wartosc UTC bez zadnych przesuniec
	//GMTime = gmtime(&tmpTime);
	asprintf(string, "%d-%s%d-%s%d %s%d:%s%d:%s%d",
		GMTime.tm_year+1900,
		(GMTime.tm_mon+1<10)?"0":"",	GMTime.tm_mon+1,
		(GMTime.tm_mday<10)?"0":"",	GMTime.tm_mday,
		(GMTime.tm_hour<10)?"0":"",	GMTime.tm_hour,
		(GMTime.tm_min<10)?"0":"",	GMTime.tm_min,
		(GMTime.tm_sec<10)?"0":"",	GMTime.tm_sec);
	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}

long Name2string(	Name_t *Name,
			char **string)
{
	/***********************/
	/* walidacja prametrow */
	/***********************/
	if (Name == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(Name->present)
	{
		case Name_PR_rdnSequence:
			BMD_FOK_CHG(RDNSequence2string(&(Name->choice.rdnSequence), string), LIBBMDASN1_COMMON_UTILS_CONV_RDN_SEQUENCE_TO_STR);
			break;
		case Name_PR_NOTHING:
		default:
			asprintf(string, "Name field empty!\n");
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
	}

	return BMD_OK;
}

long Name2sortedString(	Name_t *Name,
			char **string)
{
	/***********************/
	/* walidacja prametrow */
	/***********************/
	if (Name == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (string == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(Name->present)
	{
		case Name_PR_rdnSequence:
			BMD_FOK_CHG(RDNSequence2sortedString(&(Name->choice.rdnSequence), string), LIBBMDASN1_COMMON_UTILS_CONV_RDN_SEQUENCE_TO_STR);
			break;
		case Name_PR_NOTHING:
		default:
			asprintf(string, "Name field empty!\n");
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
	}

	return BMD_OK;
}


long GetAttrFromRDNSequence(	RDNSequence_t *RDNSequence,
					RDNSequenceAttrType_t RDNSequenceAttrType,
					char **AttributeString)
{
int err													= 0;
int i													= 0;
int k 													= 0;
AttributeValue_t *AttributeValue						= NULL;
char* OID_id_at_PostalAdress							= OID_ID_AT_POSTALADDRESS;

	if(RDNSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(RDNSequenceAttrType < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(AttributeString == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*AttributeString != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(RDNSequenceAttrType != RDNSEQ_ATTR_POSTALADDRESS)
	{
		err = GetAttributeValueFromRDNSequence(RDNSequence, RDNSequenceAttrType, &AttributeValue);
		if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_GET_ATTVALUE_FROM_RDNSEQUENCE);	}

		err = AttributeValue2String(AttributeValue, AttributeString);
		if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_STR);	}
	}
	else
	{
		for(i=0; i<RDNSequence->list.count; i++)
		{
			for(k=0; k<RDNSequence->list.array[i]->list.count; k++)
			{
				if(!OBJECT_IDENTIFIER_cmp(&(RDNSequence->list.array[i]->list.array[k]->type),OID_id_at_PostalAdress))
				{
					BMD_FOK(AttributeTypeAndValue2string(RDNSequence->list.array[i]->list.array[k], AttributeString));
				}
			}
		}
	}

	asn_DEF_AttributeValue.free_struct(&asn_DEF_AttributeValue, AttributeValue, 0);
	return BMD_OK;
}

long AttributeValue2String(	AttributeValue_t *AttributeValue,
					char **string)
{
long err = 0;
DirectoryString_t *DirectoryString = NULL;

	if(AttributeValue == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	err = AttributeValue2DirectoryString(AttributeValue, &DirectoryString);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_DIRECTORY_STR);	}

	err = DirectoryString2string(DirectoryString, string);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_DIRECTORY_STR_TO_STR);	}

	asn_DEF_DirectoryString.free_struct(&asn_DEF_DirectoryString, DirectoryString, 0);

	return BMD_OK;
}

long EDIPartyName2string(	EDIPartyName_t *EDIPartyName,
					char **string)
{
	if(EDIPartyName == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* TODO */
	asprintf(string, "EDIPartyName present but reading it not implemented yet!\n");
	if(*string == NULL)		{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}

long GeneralName2string(	GeneralName_t *GeneralName,
				char **string)
{
long err = 0;

	if(GeneralName == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(GeneralName->present)
	{
		case GeneralName_PR_otherName:
			err = AnotherName2string(&(GeneralName->choice.otherName), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_ANOTHER_NAME_TO_STR);	}
			break;

		case GeneralName_PR_rfc822Name:
			err = OCTET_STRING2string(&(GeneralName->choice.rfc822Name), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_OCTET_STR_TO_STR);	}
			break;

		case GeneralName_PR_dNSName:
			err = OCTET_STRING2string(&(GeneralName->choice.dNSName), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_IA5_STR_TO_STR);	}
			break;

		case GeneralName_PR_x400Address:
			err = ORAddress2string(&(GeneralName->choice.x400Address), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_OR_ADDRESS_TO_STR);	}
			break;

		case GeneralName_PR_directoryName:
			err = Name2string(&(GeneralName->choice.directoryName), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_NAME_TO_STR);	}
			break;

		case GeneralName_PR_ediPartyName:
			err = EDIPartyName2string(&(GeneralName->choice.ediPartyName), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_EDI_PARTY_NAME_TO_STR);	}
			break;

		case GeneralName_PR_uniformResourceIdentifier:
			err = OCTET_STRING2string(&(GeneralName->choice.uniformResourceIdentifier), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_UNIFORM_RESOURCE_IDENTIFIER_TO_STR);	}
			break;

		case GeneralName_PR_iPAddress:
			err = OCTET_STRING2string(&(GeneralName->choice.iPAddress), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_IP_ADDRESS_TO_STR);	}
			break;

		case GeneralName_PR_registeredID:
			err = OBJECT_IDENTIFIER2string(&(GeneralName->choice.registeredID), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_REGISTERED_ID_TO_STR);	}
			break;

		case GeneralName_PR_NOTHING:
		default:
			asprintf(string, "No name present in GeneralName Field!\n");
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
	}

	return BMD_OK;
}

long GeneralNames2string(	GeneralNames_t *GeneralNames,
					char **string)
{
long i				= 0;
char *string_separator	= "|";
char *tmp_string		= NULL;
char *tmp_full_string	= NULL;

	if(GeneralNames == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0; i<GeneralNames->list.count; i++)
	{
		BMD_FOK(GeneralName2string(GeneralNames->list.array[i], &tmp_string));

		if (i==0)
		{
			asprintf(&tmp_full_string, "%s", tmp_string);
			if (tmp_full_string==NULL) {	BMD_FOK(NO_MEMORY);	}
		}
		else
		{
			asprintf(&tmp_full_string, "%s%s%s", *string, string_separator, tmp_string);
			if (tmp_full_string==NULL) {	BMD_FOK(NO_MEMORY);	}
		}

		free(tmp_string); tmp_string=NULL;
		free(*string); *string=NULL;

		asprintf(string, "%s", tmp_full_string);
		if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		free(tmp_full_string); tmp_full_string=NULL;
	}
	return BMD_OK;
}

long CRLDistributionPoints2string(	CRLDistributionPoints_t *CRLDistributionPoints,
						char **string)
{
long err			= 0;
long i				= 0;
char *tmp_string		= NULL;
char *string_separator	= "||";
char *tmp_full_string	= NULL;

	if(CRLDistributionPoints == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	for(i=0; i<CRLDistributionPoints->list.count; i++)
	{
		if(CRLDistributionPoints->list.array[i]->distributionPoint == NULL)
		{
			continue;
		}

		err = DistributionPointName2string(CRLDistributionPoints->list.array[i]->distributionPoint, &tmp_string);
		if(err < 0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_DISTRIBUTION_POINT_TO_STR);	}

		if(i==0)
		{
			asprintf(&tmp_full_string, "%s", tmp_string);
			if(tmp_full_string == NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else
		{
			asprintf(&tmp_full_string, "%s%s%s", *string, string_separator, tmp_string);
			if(tmp_full_string == NULL)	{	BMD_FOK(NO_MEMORY);	}
		}

		free(tmp_string); tmp_string=NULL;
		free(*string); *string=NULL;

		asprintf(string, "%s", tmp_full_string);
		if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

		free(tmp_full_string); tmp_full_string=NULL;
	}

	return BMD_OK;
}

long DistributionPointName2string(	DistributionPointName_t *DistributionPointName,
						char **string)
{
long err = 0;

	if(DistributionPointName == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(DistributionPointName->present)
	{
		case DistributionPointName_PR_fullName:
			err = GeneralNames2string(&(DistributionPointName->choice.fullName), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_FULL_NAME_STR);	}
			break;

		case DistributionPointName_PR_nameRelativeToCRLIssuer:
			err = RelativeDistinguishedName2string(&(DistributionPointName->choice.nameRelativeToCRLIssuer), string);
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_NAME_REL_TO_CRL_ISSUER_STR);	}
			break;

		case DistributionPointName_PR_NOTHING:
		default:
			asprintf(string, "No name present in DistributionPointName Field!\n");
			if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}
			break;
	}

	return BMD_OK;
}

long Any2UTF8String2String(	ANY_t *Any,
					char **string)
{
asn_dec_rval_t rval;
UTF8String_t *UTF8String = NULL;

	/* Deserializuje bufor */
	rval = ber_decode( 0, &asn_DEF_UTF8String,(void **)&UTF8String, Any->buf, Any->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_UTF8String.free_struct(&asn_DEF_UTF8String, UTF8String, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_UTF8_STRING_ERR);
	}
	*string = (char *) malloc (sizeof(char) * (1+ UTF8String->size));
	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*string, 0, sizeof(char) * (1+ UTF8String->size));
	memmove(*string, UTF8String->buf, UTF8String->size);

	asn_DEF_UTF8String.free_struct(&asn_DEF_UTF8String, UTF8String, 0);

	return BMD_OK;
}

long Any2INTEGER2String(	ANY_t *Any,
				char **string)
{
asn_dec_rval_t rval;
INTEGER_t *Integer	= NULL;
long tmp_long		= 0;
long err			= 0;

	/* Deserializuje bufor */
	rval = ber_decode( 0, &asn_DEF_INTEGER,(void **)&Integer, Any->buf, Any->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, Integer, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_INTEGER_ERR);
	}
	err = asn_INTEGER2long(Integer, &tmp_long);
	if(err < 0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_INTEGER_TO_LONG);	}

	asprintf(string, "%li", tmp_long);
	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, Integer, 0);
	return BMD_OK;
}


long ClassList2string(	ClassList_t *ClassList,
				char **string)
{
uint8_t unmarked		= 0x80;
uint8_t unclassified	= 0x40;
uint8_t restricted	= 0x20;
uint8_t confidential	= 0x10;
uint8_t secret		= 0x08;
uint8_t topsecret		= 0x04;

	if(ClassList == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(string == NULL ||
		*string != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ClassList->buf[0] & unmarked)
	{
		asprintf(string, "%s", CLASS_LIST_UNMARKED_STR);
	}
	else if(ClassList->buf[0] & unclassified)
	{
		asprintf(string, "%s", CLASS_LIST_UNCLASSIFIED_STR);
	}
	else if(ClassList->buf[0] & restricted)
	{
		asprintf(string, "%s", CLASS_LIST_RESTRICTED_STR);
	}
	else if(ClassList->buf[0] & confidential)
	{
		asprintf(string, "%s", CLASS_LIST_CONFIDENTIAL_STR);
	}
	else if(ClassList->buf[0] & secret)
	{
		asprintf(string, "%s", CLASS_LIST_SECRET_STR);
	}
	else if(ClassList->buf[0] & topsecret)
	{
		asprintf(string, "%s", CLASS_LIST_TOP_SECRET_STR);
	}
	else
	{
		asprintf(string, "%s", "unknown");
	}

	if(*string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	return BMD_OK;
}


/************************
 * PRO: narzedzia asn1c *
 ***********************/
/* PRo: wykonanie kopii obiektu zrodlowego o podanym typie */
long asn_cloneContent(	asn_TYPE_descriptor_t *td,
				void *src,
				void **dst)
{
	GenBuf_t *buf	= NULL;
	asn_dec_rval_t rval;

	if(td == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(src == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(dst == NULL )
		{ BMD_FOK(BMD_ERR_PARAM3); }

	BMD_FOK(asn1_encode(td,src, NULL, &buf));
	rval=ber_decode(0, td, dst, buf->buf, buf->size);
	free_gen_buf(&buf);
	if( rval.code != RC_OK )
	{
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

/* PRo: Rezerwuj bufor na podstawie zwroconego kodu powrotu */
long alloc_EncodeBuffer(	asn_enc_rval_t rv, size_t twf_prefix_size, GenBuf_t **buf)
{
GenBuf_t *tempBuf=NULL;

	if (buf == NULL)	{ BMD_FOK(BMD_ERR_PARAM1); }
	if (*buf != NULL)	{ BMD_FOK(BMD_ERR_PARAM1); }

	/* wykonaj sprawdzenie kodu powrotu i zwolnij zasoby, jeeli bd */
	if (rv.encoded == -1)
	{
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR);
	}

	tempBuf = (GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(tempBuf == NULL)
	{
		BMD_FOK(NO_MEMORY);
	}

	tempBuf->size = (long)((size_t)rv.encoded + twf_prefix_size);
	tempBuf->buf = calloc(rv.encoded + twf_prefix_size, 1);
	if(tempBuf->buf == NULL)
	{
		free_gen_buf(&tempBuf);
		BMD_FOK(NO_MEMORY);
	}

	*buf=tempBuf;
	return BMD_OK;
}

long prepare_digest_info(	GenBuf_t *digest,
					GenBuf_t **output)
{
asn_enc_rval_t encodingResult;
DigestInfo_t *digest_info	= NULL;
long OID_sha[]			= {1,3,14,3,2,26};
char test[1024];
size_t test_length		= 1023;

	/************************/
	/* walidacja parametrow */
	/************************/
	if( digest == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( output == NULL )		{	BMD_FOK(BMD_ERR_PARAM2);	}

	memset(test,0,1024);

	digest_info=(DigestInfo_t *)calloc(1, sizeof(DigestInfo_t));
	if(digest_info == NULL)
	{
		return BMD_ERR_MEMORY;
	}

	if(OCTET_STRING_fromBuf( &(digest_info->digest),(char*)digest->buf,digest->size) != 0)
	{
		free(digest_info);
		return BMD_ERR_OP_FAILED;
	}

	digest_info->digestAlgorithm.parameters=(ANY_t *)calloc(1, sizeof(ANY_t));
	if(digest_info->digestAlgorithm.parameters == NULL)
	{
		asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digest_info, 1);
		free(digest_info);
		return BMD_ERR_MEMORY;
	}

	digest_info->digestAlgorithm.parameters->buf=(unsigned char *)malloc(2);
	if( digest_info->digestAlgorithm.parameters->buf == NULL )
		return BMD_ERR_MEMORY;
	digest_info->digestAlgorithm.parameters->buf[0]=0x05;
	digest_info->digestAlgorithm.parameters->buf[1]=0x00;
	digest_info->digestAlgorithm.parameters->size=2;

	if(OBJECT_IDENTIFIER_set_arcs((OBJECT_IDENTIFIER_t *)&(digest_info->digestAlgorithm),OID_sha,sizeof(long),sizeof(OID_sha)/sizeof(long)) != 0)
	{
		asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digest_info, 1);
		free(digest_info);
		return BMD_ERR_OP_FAILED;
	}


	encodingResult=der_encode_to_buffer(&asn_DEF_DigestInfo, digest_info, test, &test_length);
	asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digest_info, 1);
	free(digest_info);
	if(encodingResult.encoded < 0)
	{
		return BMD_ERR_OP_FAILED;
	}

	set_gen_buf2(test,(int)test_length,output);

	return 0;
}

/* PRo: Zwalniacz bufora generycznego */
long free_GenBuf(	GenBuf_t **GenBuf)
{
	if(GenBuf == NULL || *GenBuf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	free((*GenBuf)->buf); (*GenBuf)->buf=NULL;
	free(*GenBuf); *GenBuf=NULL;

	return BMD_OK;
}

long free_GenBufList(	GenBufList_t **GenBufList)
{
long i=0;

	if (GenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*GenBufList == NULL)	{	return BMD_OK;	}

	for(i=0; i<(*GenBufList)->size; i++)
	{
		free_GenBuf(&((*GenBufList)->gbufs[i]));
	}
	free((*GenBufList)->gbufs); (*GenBufList)->gbufs = NULL;
	free(*GenBufList); *GenBufList = NULL;

	return BMD_OK;
}
/******************************
 * Rozne przydatne konwertery *
 *****************************/
long AttributeValue2DirectoryString(	AttributeValue_t *AttributeValue,
						DirectoryString_t **DirectoryString)
{
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_DirectoryString,  (void **)DirectoryString,AttributeValue->buf, AttributeValue->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_DirectoryString.free_struct(&asn_DEF_DirectoryString, *DirectoryString, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_DIRECTORY_STRING_DATA_ERR);
	}

	return BMD_OK;
}

long AttributeValue2PostalAddress(	AttributeValue_t *AttributeValue,
						PostalAddress_t **PostalAddress)
{
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_PostalAddress,  (void **)PostalAddress,AttributeValue->buf, AttributeValue->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_PostalAddress.free_struct(&asn_DEF_PostalAddress, *PostalAddress, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_POSTAL_ADDRESS_DATA_ERR);
	}

	return BMD_OK;
}

long AttributeValue2IA5String(	AttributeValue_t *AttributeValue,
					IA5String_t **IA5String)
{
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_IA5String,  (void **)IA5String,AttributeValue->buf, AttributeValue->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_IA5String.free_struct(&asn_DEF_IA5String, *IA5String, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_IA5_DATA_ERR);
	}

	return BMD_OK;
}

long ExtensionValue2CRLDistributionPoints(	OCTET_STRING_t *ExtensionValue,
							CRLDistributionPoints_t **CRLDistributionPoints)
{
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_CRLDistributionPoints,  (void **)CRLDistributionPoints,ExtensionValue->buf, ExtensionValue->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_CRLDistributionPoints.free_struct(&asn_DEF_CRLDistributionPoints, *CRLDistributionPoints, 0);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DECODE_CRL_DISTPOINTS_DATA_ERR);
	}

	return BMD_OK;
}


long Name2RDNSequence(	Name_t *Name,
				RDNSequence_t **RDNSequence)
{
long err = 0;

	if(Name == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(RDNSequence == NULL ||
		*RDNSequence != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(Name->present != Name_PR_rdnSequence)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_NAME_HAS_NO_RDNSEQUENCE_FIELD);	}

	err = asn_cloneContent(&asn_DEF_RDNSequence,(void *)&(Name->choice.rdnSequence),(void **)RDNSequence);
	if(err<0)		{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_RDNSEQUENCE);	}

	return BMD_OK;
}

long GetAttributeValueFromRDNSequence(RDNSequence_t *RDNSequence, RDNSequenceAttrType_t RDNSequenceAttrType,
				AttributeValue_t **AttributeValue)
{
long err	= 0;
long i		= 0;
long k 	= 0;

	if(RDNSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(RDNSequenceAttrType < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(AttributeValue == NULL ||
		*AttributeValue != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	for(i=0; i<RDNSequence->list.count; i++)
	{
		for(k=0; k<RDNSequence->list.array[i]->list.count; k++)
		{
			err = GetValueFromAttributeTypeAndValue(RDNSequence->list.array[i]->list.array[k],RDNSequenceAttrType, AttributeValue);
			if(err<0 && err != LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT)
			{
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATT_TYPE_AND_VALUE);
			}
			else
			{
				if(err == BMD_OK)
				{
					return BMD_OK;
				}
			}
		}
	}

	if((i+1 == RDNSequence->list.count) && ((k+1 == RDNSequence->list.array[i]->list.count)))
	{
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
	}

	return BMD_OK;
}

long GetValueFromAttributeTypeAndValue(	AttributeTypeAndValue_t *AttributeTypeAndValue,
							RDNSequenceAttrType_t RDNSequenceAttrType,
							AttributeValue_t **AttributeValue)
{
long err = 0;
char* OID_id_at_CommonName						= OID_ID_AT_COMMONNAME;
char* OID_id_at_Surname							= OID_ID_AT_SURNAME;
char* OID_id_at_SerialNumber					= OID_ID_AT_SERIALNUMBER;
char* OID_id_at_Country							= OID_ID_AT_COUNTRY;
char* OID_id_at_Locality						= OID_ID_AT_LOCALITY;
char* OID_id_at_StateOrProvince					= OID_ID_AT_STATEORPROVINCE;
char* OID_id_at_Organization					= OID_ID_AT_ORGANIZATION;
char* OID_id_at_OrganizationalUnit				= OID_ID_AT_ORGANIZATIONALUNIT;
char* OID_id_at_Title							= OID_ID_AT_TITLE;
char* OID_id_at_Description						= OID_ID_AT_DESCRIPTION;
char* OID_id_at_GivenName						= OID_ID_AT_GIVENNAME;
char* OID_id_at_GenerationQualifier				= OID_ID_AT_GENERATIONQUALIFIER;
char* OID_id_at_DistinguishedNameQualifier		= OID_ID_AT_DISTINGUISHEDNAMEQUALIFIER;
char* OID_id_at_Pseudnym						= OID_ID_AT_PSEUDONYM;
char* OID_id_at_PostalAdress					= OID_ID_AT_POSTALADDRESS;


	if (AttributeTypeAndValue == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (RDNSequenceAttrType < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (AttributeValue == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*AttributeValue != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	switch(RDNSequenceAttrType)
	{
		case RDNSEQ_ATTR_SURNAME:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_Surname))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,	(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SURNAME);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_GIVENNAME:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_GivenName))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_GIVENNAME);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_COMMONNAME:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_CommonName))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COMMONNAME);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
		break;

		case RDNSEQ_ATTR_ORGANIZATION:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_Organization))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATION);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_COUNTRY:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_Country))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COUNTRY);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_LOCALITY:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_Locality))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue,(void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_LOCALITY);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_ORGANIZATIONALUNIT:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_OrganizationalUnit))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue, (void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATIONALUNIT);	}
			}
			else
			{
#ifdef WIN32
				BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
				BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			}
			break;

		case RDNSEQ_ATTR_SERIALNUMBER:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_SerialNumber))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue, (void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SERIALNUMBER);	}
			}
			else
			{
				PRINT_DEBUG("%s. Error: %i\n",GetErrorMsg(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT), LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
				return LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT;
			}
			break;

		case RDNSEQ_ATTR_PSEUDONYM:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_Pseudnym))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue, (void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_PSEUDONYM);	}
			}
			else
			{
				PRINT_DEBUG("%s. Error: %i\n",GetErrorMsg(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT), LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
				return LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT;
			}
			break;

		case RDNSEQ_ATTR_STATE:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_StateOrProvince))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue, (void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_STATE);	}
			}
			else
			{
				PRINT_DEBUG("%s. Error: %i\n",GetErrorMsg(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT), LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
				return LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT;
			}
			break;

		case RDNSEQ_ATTR_POSTALADDRESS:
			if(!OBJECT_IDENTIFIER_cmp(&(AttributeTypeAndValue->type),OID_id_at_PostalAdress))
			{
				err = asn_cloneContent(&asn_DEF_AttributeValue, (void *)&(AttributeTypeAndValue->value), (void **)AttributeValue);
				if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_POSTALADDRESS);	}
			}
			else
			{
				PRINT_DEBUG("%s. Error: %i\n",GetErrorMsg(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT), LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
				return LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT;
			}
			break;

		default:
#ifdef WIN32
			BMD_FOK_NP(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#else
			BMD_FOK(LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT);
#endif
			break;
	}

	return err;
}

long String2OID_alloc(	const char *String,
				OBJECT_IDENTIFIER_t **oid)
{
long err = 0;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(oid == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*oid != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*oid = (OBJECT_IDENTIFIER_t *) malloc (sizeof(OBJECT_IDENTIFIER_t));
	if(*oid == NULL)		{	BMD_FOK(NO_MEMORY);	}

	memset(*oid, 0, sizeof(OBJECT_IDENTIFIER_t));

	err = String2OID((char *)String, *oid);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OID);	}

	return BMD_OK;
}

long String2IA5String2ANY(	const char *String,
					ANY_t *any)
{
	long err			= 0;
	IA5String_t IA5String;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	memset(&IA5String, 0, sizeof(IA5String_t));
	err = OCTET_STRING_fromBuf(&IA5String, String, (int)strlen(String));
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING);	}

	err=ANY_fromType(any,&asn_DEF_IA5String,&IA5String);
	if( err != 0 )
		return LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR;

	asn_DEF_IA5String.free_struct(&asn_DEF_IA5String, &IA5String, 1);

	return BMD_OK;
}

long String2UTF8String2ANY(	const char *String,
					ANY_t *any)
{
	long err			= 0;
	UTF8String_t UTF8String;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	memset(&UTF8String, 0, sizeof(UTF8String_t));
	err = OCTET_STRING_fromBuf(&UTF8String, String, (int)strlen(String));
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING); }

	err=ANY_fromType(any,&asn_DEF_UTF8String,&UTF8String);
	if( err != 0 )
	{
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR);
	}

	asn_DEF_UTF8String.free_struct(&asn_DEF_UTF8String, &UTF8String, 1);

	return BMD_OK;
}

long String2UTF8String2ANY_alloc(	const char *String,
						ANY_t **any)
{
long err = 0;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL ||
		*any != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*any = (ANY_t *) malloc (sizeof(ANY_t));
	if(*any == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*any, 0, sizeof(ANY_t));

	err = String2UTF8String2ANY(String, *any);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY);	}

	return BMD_OK;
}


long String2OCTET_STRING2ANY(	const char *String,
					ANY_t *any)
{
	long err		= 0;
	OCTET_STRING_t OCTET_string;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	memset(&OCTET_string, 0, sizeof(OCTET_STRING_t));
	err = OCTET_STRING_fromBuf(&OCTET_string, String, (int)strlen(String));
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING);	}

	err=ANY_fromType(any,&asn_DEF_OCTET_STRING,&OCTET_string);
	if( err != 0 ) {	BMD_FOK(LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR);	}

	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, &OCTET_string, 1);

	return BMD_OK;
}

long String2OCTET_STRING2ANY_alloc(	const char *String,
						ANY_t **any)
{
long err = 0;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL ||
		*any != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*any = (ANY_t *) malloc (sizeof(ANY_t));
	if(*any == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*any, 0, sizeof(ANY_t));

	err = String2OCTET_STRING2ANY(String, *any);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY);	}

	return BMD_OK;
}


long String2INTEGER2ANY(	const char *String,
				ANY_t *any)
{
	long err		= 0;
	INTEGER_t 	IntegerBuf;
	long IntegerNumber;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	IntegerNumber = strtol( String, (char **)NULL, 10);
	memset(&IntegerBuf, 0, sizeof(INTEGER_t));

	err = asn_long2INTEGER(&IntegerBuf, IntegerNumber);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_LONG_TO_INTEGER);	}

	err=ANY_fromType(any,&asn_DEF_INTEGER,&IntegerBuf);
	if( err != 0 ) {	BMD_FOK(LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR);	}

	asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, &IntegerBuf, 1);

	return BMD_OK;
}

long String2INTEGER2ANY_alloc(	const char *String,
					ANY_t **any)
{
long err = 0;

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(any == NULL ||
		*any != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*any = (ANY_t *) malloc (sizeof(ANY_t));
	if(*any == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*any, 0, sizeof(ANY_t));

	err = String2INTEGER2ANY(String, *any);
	if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY);	}

	return BMD_OK;
}


int CU_buff_composer_reentrant(	const void *encoder_buffer,
						size_t size,
						void *out_buff)
{
GenBuf_t *gen_buf = (GenBuf_t *) out_buff;

	if ((encoder_buffer != NULL) &&
		(size != 0) && (out_buff == NULL))	{	BMD_FOK(BMD_ERR_PARAM1);	}

	memmove ((char *)(gen_buf->buf + gen_buf->size), encoder_buffer, size);
	gen_buf->size += (int)size;

	return BMD_OK;
}

long CutString2GenBufList_alloc(	const char *String,
						GenBufList_t **GenBufList)
{
long err				= 0;
char *tmp				= NULL;
GenBuf_t **tmp_gbufs_tab	= NULL;
char *string			= NULL;
char *old_ptr			= NULL;
#ifdef WIN32
long i					= 0;
long rows				= 0;
char **strings			= NULL;
#endif

	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBufList == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*GenBufList == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*GenBufList, 0, sizeof(GenBufList_t));

	asprintf(&string, "%s", String);
	if(string == NULL)	{	BMD_FOK(NO_MEMORY);	}

	old_ptr = string;
	/* wydobÄ…dĹş stringi oddzielone kropkami */
#ifdef WIN32		// (ROL)
	bmd_strsep(string, ',', &strings,&rows);
	if (rows < 0)		{	BMD_FOK(err);	}
#endif
	do
	{
		//free(tmp);
#ifndef WIN32
		tmp = strsep(&string, ",");
#else
		if (i==rows)
			tmp = NULL;
		else
			tmp = strings[i++];
#endif
		if(tmp!=NULL)
		{ /* jesli string nie jest pusty to dodaj go do listy */
			(*GenBufList)->size++;
			tmp_gbufs_tab = (GenBuf_t **) realloc ((*GenBufList)->gbufs,sizeof(GenBuf_t *) * (*GenBufList)->size);
			if(tmp_gbufs_tab == NULL)	{	BMD_FOK(NO_MEMORY);	}
			else
			{
				(*GenBufList)->gbufs = tmp_gbufs_tab;
			}
			err = String2GenBuf_alloc(tmp, &((*GenBufList)->gbufs[(*GenBufList)->size-1]));
			if(err<0)	{	BMD_FOK(LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OCTET_STR);	}
		}
	} while(tmp != NULL);

#ifdef WIN32
	free(strings);
#endif
	free(old_ptr); old_ptr=NULL;

	return BMD_OK;
}

long String2GenBuf_alloc(const char *String, GenBuf_t **GenBuf)
{
	if(String == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	*GenBuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*GenBuf == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(*GenBuf, 0, sizeof(GenBuf_t));

	(*GenBuf)->buf = (char *) malloc (sizeof(char) * (1+strlen(String)));
	if((*GenBuf)->buf == NULL)	{	BMD_FOK(NO_MEMORY);	}

	(*GenBuf)->size = 1+(int)strlen(String);
	memmove((*GenBuf)->buf, String, 1+strlen(String));

	return BMD_OK;
}

/**<added by <WSZ> > */

/**funkcja bmd_strsep wydobywa podlancuchy odseparowane danym znakiem
@ARG src to zrodlowy lancych znakow
@ARG sep to znak separacyjny
@ARG dest to ampersand do tablicy znalezionych podstringow
@ARG to adres zmiennej int, w ktorej zapisana zostanie ilosc znalezionych substringow
@RETURN 0 jesli ok
	-1 jesli jakis argument zostal zle podany (src badz dest)
COMMENT:  	w przypadku braku separatora w lancuchu, dest na wyjsciu przechowywac bedzie kopie stringa src
		jesli separatory beda ze soba sasiadowac, to "nic" pomiedzy nimi traktowane jest jako lanuch pusty = "\0"
		jesli pierwszy znank badanego lancucha zawiera separator, to interpretacja jest taka, jakby, przed
		separatorem znajdowal sie lanuch pusty (patrz wyzej) ... analogicznie dla konca lancucha
		jesli podany src jest lanuchem pustym, to funkcja konczy sie powodzeniem ,a N przyjmuje wartosc 0  */
long bmd_strsep(	char *src,
			char sep,
			char*** dest,
			long *N)
{
long len		= 0;
long len_temp		= 0;
long iter			= 0;
long i				= 0;
long sub_num			= 0;
char** result		= NULL;
char* substring		= NULL;
long sub_num_2ndrun	= 0;
char *temp			= NULL;

	if(src==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dest==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(*dest!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	len = (int)strlen(src);

	/*jesli pusty lancuch*/
	if(len==0)
	{
		*N=0;
		return 0;
	}

	for(iter=0; iter<len; iter++)
	{
		if(src[iter]==sep)
		{
			sub_num++;
		}
	}
	result=(char**)calloc(sub_num+1, sizeof(char*));
	/*jesli brak separatora, to funkcja kopiuje caly string*/
	if( sub_num == 0 )
	{
		len_temp=(int)strlen(src);
		result[0]=(char*)calloc(len_temp+1, sizeof(char));
		for(i=0; i<len_temp; i++)
		{
			result[0][i]=src[i];
		}
		result[0][i]='\0';
		*dest=result;
		*N=1;
		return 0;
	}


	/*na wstepie pusty lanuch - jedynie \0*/
	substring=(char*)calloc(1,sizeof(char));

	for(iter=0; iter<len; iter++)
	{
		/*jesli napotkany separator*/
		if(src[iter]==sep)
		{
			/*przed separatorem nie bylo nic, badz z separatorem(przed nim) sasiaduje separator
				daje to lancuch pusty - dlugosci 1 z zawartoscia \0*/
			result[sub_num_2ndrun]=substring;
			sub_num_2ndrun++;
			substring=(char*)calloc(1,sizeof(char));
		}
		else
		{
			len_temp=(int)strlen(substring);
			/* dlugosc starego + nowy znak i NULL*/
			/* zrezygnowalem z realloca bo Walus sie pluje i mnie to wpieniac zaczyna*/
			temp=substring;
			substring=(char*)calloc(len_temp+2, sizeof(char));
			for(i=0; i<len_temp; i++)
			{
				substring[i]=temp[i];
			}
			substring[len_temp]=src[iter];
			substring[len_temp+1]='\0';
			free(temp);
			temp=NULL;
		}
	}

	/*ostatni substring do zapisu*/
	result[sub_num_2ndrun]=substring;

	*dest=result;
	*N=sub_num+1;

	return BMD_OK;
}

/**funkcja bratnia dla bmd_strsep
zwalnia tablice odnalezionych substringow (funkcja bmd_strsep())
@ARG src to ampersand do tablicy lanuchow przechowujacej substringi
@ARG N to ilosc substringow przechowywanych w tablicy substringow
@RETURN zawsze 0, czyli ok.
*/
long bmd_strsep_destroy(	char*** src,
				long N)
{
long iter		= 0;
char **temp=*src;

	if(src!=NULL)
	{
		if(*src!=NULL)
		{
			for(iter=0; iter<N; iter++)
			{
				free( temp[iter] );
			}
			free( temp );
			*src=NULL;
		}
	}

	return BMD_OK;
}


/**funkcja inicjuje OBJECT_IDENTIFIER_t na podstawie lancucha w formacie liczb oddzielonych kropkami
@ARG str to lancuch z oidem
@ARG oid to odpowiednio inicjalizowany OBJECT_IDENTIFIER (oid musi byc zmienna statyczna badz wczesniej zaalokowany)

@RETURN 0 jesli ok.
	-1 jesli ktorys z argumentow zle przekazany badz pusty string z oidem
	-2 jesli cos nie tak z formatem oid'u
	-3 jesli problem z ustawieniem oid'u
*/
long String2OID(	char *str,
			OBJECT_IDENTIFIER_t *oid)
{
char** tab_substr	= NULL;
long* tab_long	= NULL;
char *err		= NULL;
long num		= 0;
long ret		= 0;
long iter		= 0;

	if(str==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(oid==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_strsep(str, '.', &tab_substr, &num));

	if(num==0)
	{
		BMD_FOK(-1);
	}

	tab_long=(long*)calloc(num, sizeof(long));
	if(tab_long==NULL)
	{
		bmd_strsep_destroy(&tab_substr, num);
		BMD_FOK(NO_MEMORY);
	}

	for(iter=0; iter<num; iter++)
	{
		tab_long[iter]=strtol(tab_substr[iter], &err, 0);
		/*sprawdzanie, czy przy zwroconym zero nie bylo bledu - niedozwolone znaki*/
		if(tab_long[iter]==0)
		{
			if(*err!='\0')
			{
				bmd_strsep_destroy(&tab_substr,num);
				free(tab_long);
				BMD_FOK(-2);
			}
		}
	}

	ret=OBJECT_IDENTIFIER_set_arcs(oid, (void*)tab_long, sizeof(tab_long[0]), num);
	/*niezaleznie od wyniku tworzenia oid'u mozna zwolnic wszelka zaalokowana pamiec*/
	bmd_strsep_destroy(&tab_substr, num);
	free(tab_long);

	if(ret!=0)
	{
		BMD_FOK(-3);
	}

	return BMD_OK;
}

long asn1_encode(void *td, void *src, GenBuf_t *twf_prefix, GenBuf_t **der)
{
	asn_enc_rval_t encResult;
	size_t tmp_size			= 0;
	size_t prefix_size		= 0;

	if(td == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(src == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);        }
	if(der == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}


	if(twf_prefix == NULL)
	{
		prefix_size = 0;
	}
	else
	{
		if(twf_prefix->buf != NULL)
		{
			prefix_size = twf_prefix->size;
		}
		else
		{
			prefix_size = 0;
		}
	}

	/*prefix_size dodatkowe bajty na poczatku bufora na wartosc twf_prefix*/
	BMD_FOK(alloc_EncodeBuffer(der_encode(td,src, 0, NULL), prefix_size, der));

	if(prefix_size > 0)
	{
		memcpy( (*der)->buf, twf_prefix->buf, twf_prefix->size);
	}

	tmp_size=(*der)->size - prefix_size;
	/*Zapisujemy wynik do bufora omijajac twf_prefix na poczatku bufora*/
	encResult=der_encode_to_buffer(td, src, (*der)->buf + prefix_size, &tmp_size);
	if( ((*der)->size - prefix_size) != (int)(encResult.encoded))
	{
		free_gen_buf(der);
		BMD_FOK(LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR);
	}

	return BMD_OK;
}
