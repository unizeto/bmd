#include <bmd/libbmdaa2/db/common/AC_db_IetfAttrSyntax.h>
#include <bmd/libbmderr/libbmderr.h>

long AC_MakeAttribute_IetfAttrSyntax(	GenBufList_t *GroupsGenBufList,
							AttributesSequence_t *AttributesSequence,
							const ConfigData_t *ConfigData)
{
IetfAttrSyntax_t *IetfAttrSyntax	= NULL;
ANY_t *ANY_IetfAttrSyntax		= NULL;
Attribute_t *Attribute			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(GroupsGenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AC_IetfAttrSyntax_build(GroupsGenBufList, &IetfAttrSyntax, ConfigData));

	/*asn_fprint(stdout, &asn_DEF_IetfAttrSyntax, IetfAttrSyntax);*/
	ANY_IetfAttrSyntax=ANY_new_fromType(&asn_DEF_IetfAttrSyntax,IetfAttrSyntax);
	if( ANY_IetfAttrSyntax == NULL )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	asn_DEF_IetfAttrSyntax.free_struct(&asn_DEF_IetfAttrSyntax,IetfAttrSyntax,0);IetfAttrSyntax=NULL;
	BMD_FOK(AC_CreateEmptyAttributeSet(&Attribute));

	BMD_FOK(AC_Attribute_set_TypeOid(ConfigData->ACConfiguration->ACAttributesSection->ACAttributeGroup->attribute_group_oid, Attribute));

	/* Grupy mają tylko jedną wertość atrybutu, lista grup umieszczona jest
	   wewnątrz IetfAttrSyntax */
	BMD_FOK(asn_set_add(&(Attribute->attrValues.list),ANY_IetfAttrSyntax));
	BMD_FOK(asn_set_add(&(AttributesSequence->list),Attribute));

	return BMD_OK;
}


long AC_IetfAttrSyntax_build(	const GenBufList_t *GenBufList,
					IetfAttrSyntax_t **IetfAttrSyntax,
					const ConfigData_t *ConfigData)
{
long err				= 0;
long i				= 0;
GeneralNames_t *GeneralNames	= NULL;

	if (GenBufList == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (IetfAttrSyntax == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*IetfAttrSyntax != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AC_ACDistinguishedName2GeneralNames(ConfigData->ACConfiguration->ACAttributesSection->ACAttributeGroup->group_policy_authority, &GeneralNames));

	(*IetfAttrSyntax)=(IetfAttrSyntax_t *)malloc(sizeof(IetfAttrSyntax_t));
	if( (*IetfAttrSyntax) == NULL )	{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(*IetfAttrSyntax,0,sizeof(IetfAttrSyntax_t));
	(*IetfAttrSyntax)->policyAuthority=GeneralNames;
	for(i=0; i<GenBufList->size; i++)
	{
		BMD_FOK(AC_IetfAttrSyntax_add_Group(IETF_GROUP_UTF8, GenBufList->gbufs[i], *IetfAttrSyntax));
	}
	return err;
}

