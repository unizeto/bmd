#include <bmd/libbmdaa2/wrappers/AC_SecurityCategory.h>
#include <bmd/libbmderr/libbmderr.h>
int AC_SecurityCategory_create(	const OBJECT_IDENTIFIER_t *SecCatType,
						ANY_t *SecCatValue,
						SecurityCategory_t *SecurityCategory)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (SecCatType == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SecCatValue == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (SecurityCategory == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	memset(SecurityCategory,0,sizeof(SecurityCategory_t));
	SecurityCategory->type.buf   = SecCatType->buf;
	SecurityCategory->type.size  = SecCatType->size;
	SecurityCategory->value.buf  = SecCatValue->buf;
	SecurityCategory->value.size = SecCatValue->size;

	return BMD_OK;
}

int AC_SecurityCategory_create_alloc(	const OBJECT_IDENTIFIER_t *SecCatType,
							ANY_t *SecCatValue,
							SecurityCategory_t **SecurityCategory)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (SecCatType == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SecCatValue == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (SecurityCategory == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*SecurityCategory != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	*SecurityCategory = (SecurityCategory_t *) malloc (sizeof(SecurityCategory_t));
	if(*SecurityCategory == NULL) 	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(AC_SecurityCategory_create(SecCatType, SecCatValue, *SecurityCategory));

	return BMD_OK;
}
