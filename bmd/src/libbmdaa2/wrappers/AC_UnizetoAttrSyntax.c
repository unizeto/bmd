#include<bmd/libbmdaa2/wrappers/AC_UnizetoAttrSyntax.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_UnizetoAttrSyntax_create(GeneralNames_t *policyAuthority,
				OBJECT_IDENTIFIER_t *SyntaxId,
				UnizetoAttrSyntax_t *UnizetoAttrSyntax)
{
int err = 0;
	if(policyAuthority == NULL)
		return BMD_ERR_PARAM1;
	if(SyntaxId == NULL)
		return BMD_ERR_PARAM2;
	if(UnizetoAttrSyntax == NULL)
		return BMD_ERR_PARAM3;

	memset(UnizetoAttrSyntax,0,sizeof(UnizetoAttrSyntax_t));
	UnizetoAttrSyntax->policyAuthority = policyAuthority;

	/* OID składni atrybutu */
	UnizetoAttrSyntax->syntaxId.buf  = SyntaxId->buf;
	UnizetoAttrSyntax->syntaxId.size = SyntaxId->size;
	return err;
}

int AC_UnizetoAttrSyntax_create_alloc(GeneralNames_t *policyAuthority,
				OBJECT_IDENTIFIER_t *SyntaxId,
				UnizetoAttrSyntax_t **UnizetoAttrSyntax)
{
int err = 0;
	if(policyAuthority == NULL)
		return BMD_ERR_PARAM1;
	if(SyntaxId == NULL)
		return BMD_ERR_PARAM2;
	if(UnizetoAttrSyntax == NULL || *UnizetoAttrSyntax != NULL)
		return BMD_ERR_PARAM3;
	
	*UnizetoAttrSyntax = (UnizetoAttrSyntax_t *) malloc (sizeof(UnizetoAttrSyntax_t));
	if(*UnizetoAttrSyntax == NULL) return NO_MEMORY;

	err = AC_UnizetoAttrSyntax_create(policyAuthority, SyntaxId, *UnizetoAttrSyntax);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating UnizetoAttrSyntax_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}
