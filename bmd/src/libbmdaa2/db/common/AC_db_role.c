#include <bmd/libbmdaa2/db/common/AC_db_role.h>
#include <bmd/libbmderr/libbmderr.h>
long AC_MakeAttribute_RoleSyntax(	GenBufList_t *RolesGenBufList,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{
RoleSyntax_t *RoleSyntax	= NULL;
ANY_t *ANY_RoleSyntax		= NULL;
Attribute_t *Attribute		= NULL;
long i				= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(RolesGenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AC_CreateEmptyAttributeSet(&Attribute));
	BMD_FOK(AC_Attribute_set_TypeOid(ConfigData->ACConfiguration->ACAttributesSection->ACAttributeRole->attribute_role_oid, Attribute));
	/* Wartosci atrybutu RoleSyntax jest tyle ile ról */
	for(i=0; i<RolesGenBufList->size; i++)
	{
		BMD_FOK(AC_RoleSyntax_build(RolesGenBufList->gbufs[i], &RoleSyntax, ConfigData));

		/*asn_fprint(stdout, &asn_DEF_RoleSyntax, RoleSyntax);*/
		ANY_RoleSyntax=ANY_new_fromType(&asn_DEF_RoleSyntax,RoleSyntax);
		if( ANY_RoleSyntax == NULL )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		asn_DEF_RoleSyntax.free_struct(&asn_DEF_RoleSyntax,RoleSyntax,0);RoleSyntax=NULL;
		/* Tyle wartosci atrybutu ile ról */
		BMD_FOK(asn_set_add(&(Attribute->attrValues.list),ANY_RoleSyntax));
		/* Jesli wszystko poszlo dobrze to przygotuj ANY jako wskaznik	*/
		/* pod nowa strukture DERowa (stara zawartosc juz zostala pod-	*/
		/* pieta do listy i mozemy wykorzytac wskaznik na nowo.		*/
		ANY_RoleSyntax = NULL;	/* dla walidatora funkcji derenc_alloc	*/
	}

	BMD_FOK(asn_set_add(&(AttributesSequence->list),Attribute));

	return BMD_OK;
}

long AC_RoleSyntax_build(const GenBuf_t *RoleGenBuf,
			RoleSyntax_t **RoleSyntax,
			const ConfigData_t *ConfigData)
{
long err = 0;
GeneralNames_t *RoleAuthority = NULL;
IA5String_t *Role = NULL;
GeneralName_t *RoleGeneralName = NULL;

	if(RoleGenBuf == NULL)
		return BMD_ERR_PARAM1;
	if(RoleSyntax == NULL || *RoleSyntax != NULL)
		return BMD_ERR_PARAM2;
	if(ConfigData == NULL)
		return BMD_ERR_PARAM3;

	err = AC_ACDistinguishedName2GeneralNames(ConfigData->ACConfiguration->ACAttributesSection->
			ACAttributeRole->role_authority, &RoleAuthority);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating Role Authority General Names.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	Role=(IA5String_t *)malloc(sizeof(IA5String_t));
	if( Role == NULL )
		return BMD_ERR_MEMORY;
	memset(Role,0,sizeof(IA5String_t));
	err=OCTET_STRING_fromBuf((OCTET_STRING_t *)Role,(const char *)RoleGenBuf->buf,RoleGenBuf->size);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating Role IA5String.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_GeneralName_create_alloc((void *)Role, GeneralName_PR_uniformResourceIdentifier,
					&RoleGeneralName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting IA5String into GeneralName.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* IA5String_t zostało już podpięte zawartością pod GenralNeme_t	*/
	/* więc samą strukturę Name_t można zwolnić.				*/
	if(Role) {free(Role); Role = NULL;}
	err = AC_RoleSyntax_create_alloc(RoleAuthority, RoleGeneralName, RoleSyntax);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating RoleSytax from GeneralNames "
		" and GeneralName.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* RoleGeneralName zostało już podpięte zawartością pod RoleSyntax	*/
	/* więc samą strukturę GeneralName_t można zwolnić.			*/
	if(RoleGeneralName) {free(RoleGeneralName); RoleGeneralName = NULL;}
	return err;
}


