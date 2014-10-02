#include <bmd/libbmdaa2/wrappers/AC_RelativeDistinguishedName.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_CreateRelativeDistinguishedNameFromString(DNValueType_t DNValueType, 
				const char *DNValue, 
				RelativeDistinguishedName_t **RelativeDistinguishedName)
{
int err = 0;
AttributeTypeAndValue_t *AttributeTypeAndValue = NULL;

	if(DNValueType < 0)
		return BMD_ERR_PARAM1;
	if(DNValue == NULL)
		return BMD_ERR_PARAM2;
	if(RelativeDistinguishedName == NULL || *RelativeDistinguishedName != NULL)
		return BMD_ERR_PARAM3;
	
	/* Utworz puste RelativeDistinguishedName */
	*RelativeDistinguishedName = (RelativeDistinguishedName_t *) malloc
			(sizeof(RelativeDistinguishedName_t));
	if(*RelativeDistinguishedName == NULL) return NO_MEMORY;
	memset(*RelativeDistinguishedName,0,sizeof(RelativeDistinguishedName_t));

	/* Utworz wypelniony AttributeTypeAndValue */
	err = AC_CreateAttributeTypeAndValueFromString(DNValueType, 
				DNValue, 
				&AttributeTypeAndValue);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAINF[%s:%i:%s] "
				"Error in creating AttributeTypeAndValue from string. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_ASN1_SET_ADD;
	}
	/* Utworz sekwencje jednoelementowa RelativeDistinguishedName	*/
	/* dodajac uprzednio utworzone AttributeTypeAndValue		*/
	/* (w tak jak to definiuje RFC) 				*/
	err = asn_set_add(&((*RelativeDistinguishedName)->list), AttributeTypeAndValue);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAINF[%s:%i:%s] "
				"Error in adding RelativeDistinguishedName to RDNSequence. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_ASN1_SET_ADD;
	}
	return err;
}
