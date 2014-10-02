#include <bmd/libbmdaa2/wrappers/AC_AttributeTypeAndValue.h>
#include <bmd/libbmderr/libbmderr.h>
/* TODO - czesc z elementow AttributeTypeAndValue->value zakodowac do UTF8String */

int AC_CreateAttributeTypeAndValueFromString(DNValueType_t DNValueType, 
				const char *DNValue, 
				AttributeTypeAndValue_t **AttributeTypeAndValue)
{
int err = 0;
char *OIDstring = NULL;
	if(DNValueType < 0)
		return BMD_ERR_PARAM1;
	if(DNValue == NULL)
		return BMD_ERR_PARAM2;
	if(AttributeTypeAndValue == NULL || *AttributeTypeAndValue != NULL)
		return BMD_ERR_PARAM3;
	
	*AttributeTypeAndValue = (AttributeTypeAndValue_t *) malloc (sizeof(AttributeTypeAndValue_t));
	if(*AttributeTypeAndValue == NULL) return NO_MEMORY;
	AC_AttributeTypeAndValue_init(*AttributeTypeAndValue);
		
	/* przydziel odpowiednie OIDy dla attribute type */
	switch(DNValueType){
	case DN_VALUE_COMMONNAME:		OIDstring = OID_STR_COMMONNAME;		break;
	case DN_VALUE_SURNAME:			OIDstring = OID_STR_SURNAME;		break;
	case DN_VALUE_SERIALNUMBER:		OIDstring = OID_STR_SERIALNUMBER;	break;
	case DN_VALUE_COUNTRY:			OIDstring = OID_STR_COUNTRY;		break;
	case DN_VALUE_LOCALITY:			OIDstring = OID_STR_LOCALITY;		break;
	case DN_VALUE_STATEORPROVINCE:		OIDstring = OID_STR_STATEORPROVINCE;	break;
	case DN_VALUE_ORGANIZATION:		OIDstring = OID_STR_ORGANIZATION;	break;
	case DN_VALUE_ORGANIZATIONALUNIT:	OIDstring = OID_STR_ORGANIZATIONALUNIT;	break;
	case DN_VALUE_TITLE:			OIDstring = OID_STR_TITLE;		break;
	case DN_VALUE_DESCRIPTION:		OIDstring = OID_STR_DESCRIPTION;	break;
	case DN_VALUE_GIVENNAME:		OIDstring = OID_STR_GIVENNAME;		break;
	case DN_VALUE_GENERATIONQUALIFIER:	OIDstring = OID_STR_GENERATIONQUALIFIER;break;
	case DN_VALUE_DISTINGUISHEDNAMEQUALIFIER: OIDstring = OID_STR_DISTINGUISHEDNAMEQUALIFIER; break;
	case DN_VALUE_PSEUDONYM:		OIDstring = OID_STR_PSEUDONYM;		break;
	case DN_VALUE_UNDEFINED:
	default:				return ERR_AA_UNRECOGNIZED_DN_TYPE;	break;
	}
	err = String2OID(OIDstring, &((*AttributeTypeAndValue)->type));
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in converting string (%s) to OID\n. "
			"Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, OIDstring, err);
	}
	/* Na razie nie obslugujemy innych elementow DN niz UTF8Stringi 	*/
	/* W przyszlosci TU bedzie switch i w zaleznosci od elementu 	*/
	/* skladowego bedzie rozne kodowanie.				*/
	err = String2UTF8String2ANY(DNValue, &((*AttributeTypeAndValue)->value));
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in converting string (%s) to ANY\n. "
			"Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, DNValue, err);
	}
	return err;
}

int AC_AttributeTypeAndValue_init(AttributeTypeAndValue_t *AttributeTypeAndValue)
{
int err = 0;
	if(AttributeTypeAndValue == NULL)
		return BMD_ERR_PARAM1;
	memset(AttributeTypeAndValue, 0, sizeof(AttributeTypeAndValue_t));
	return err;
}


