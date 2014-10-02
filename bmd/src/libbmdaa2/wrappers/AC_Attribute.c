#include <bmd/libbmdaa2/wrappers/AC_Attribute.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_CreateEmptyAttributeSet(Attribute_t **Attribute)
{
int err = 0;
	if(Attribute == NULL || *Attribute != NULL)
		return BMD_ERR_PARAM1;
	*Attribute = (Attribute_t *) malloc (sizeof(Attribute_t));
	if(*Attribute == NULL) return NO_MEMORY;
	memset(*Attribute,0,sizeof(Attribute_t));
	return err;
}

int AC_Attribute_set_TypeOid(char *OidType, Attribute_t *Attribute)
{
int err = 0;
	if(OidType == NULL)
		return BMD_ERR_PARAM1;
	if(Attribute == NULL)
		return BMD_ERR_PARAM2;

	err = String2OID(OidType, &(Attribute->attrType));
	if(err<0){
		PRINT_DEBUG("LIBAAERR[%s:%i:%s] Error in setting OID in Attribute Type. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
	}
	return err;
}
