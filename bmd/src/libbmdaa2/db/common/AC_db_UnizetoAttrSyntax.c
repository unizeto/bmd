#include <bmd/libbmdaa2/db/common/AC_db_UnizetoAttrSyntax.h>
#include <bmd/libbmderr/libbmderr.h>

long AC_MakeAttribute_UnizetoAttrSyntax(UnizetoAttrSyntaxInList_t *UnizetoAttrSyntaxInList,
			AttributesSequence_t *AttributesSequence,
			const ConfigData_t *ConfigData)
{
long err = 0;
Attribute_t *Attribute = NULL;
UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn = NULL;
UnizetoAttrSyntax_t *UnizetoAttrSyntax = NULL;
ANY_t *ANY_UnizetoAttrSyntax = NULL;
long i = 0;

	if(UnizetoAttrSyntaxInList == NULL)
		return BMD_ERR_PARAM1;
	if(AttributesSequence == NULL)
		return BMD_ERR_PARAM2;
	if(ConfigData == NULL)
		return BMD_ERR_PARAM3;

	err = AC_CreateEmptyAttributeSet(&Attribute);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBFILEERR[%s:%i:%s] Error in creating empty Attribute.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_Attribute_set_TypeOid(ConfigData->ACConfiguration->ACAttributesSection->
			ACAttributeUnizeto->attribute_unizeto_oid, Attribute);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBFILEERR[%s:%i:%s] Error in setting Attribute Type OID "
		"in Attribute.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	for(i=0; i<UnizetoAttrSyntaxInList->size; i++){
		err = AC_UnizetoAttrSyntax_build(UnizetoAttrSyntaxInList->UnizetoAttrSyntaxIn[i],
					 &UnizetoAttrSyntax, ConfigData);
		if(err<0){
			PRINT_DEBUG("LIBBMDAADBFILEERR[%s:%i:%s] Error in creating UnizetoAttrSyntax_t.\n"
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		/*asn_fprint(stdout, &asn_DEF_UnizetoAttrSyntax, UnizetoAttrSyntax);*/
		ANY_UnizetoAttrSyntax=ANY_new_fromType(&asn_DEF_UnizetoAttrSyntax,UnizetoAttrSyntax);
		if( ANY_UnizetoAttrSyntax == NULL )
			BMD_FOK(BMD_ERR_OP_FAILED);
		asn_DEF_UnizetoAttrSyntax.free_struct(&asn_DEF_UnizetoAttrSyntax,UnizetoAttrSyntax,0);
		UnizetoAttrSyntax=NULL;
		/* Tyle wartosci atrybutu ile wpisow w pliku */
		err=asn_set_add(&(Attribute->attrValues.list),ANY_UnizetoAttrSyntax);
		if(err<0){
			PRINT_DEBUG("LIBBMDAADBFILEERR[%s:%i:%s] Error in adding value to Attribute.\n"
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		/* Jesli wszystko poszlo dobrze to przygotuj ANY jako wskaznik	*/
		/* pod nowa strukture DERowa (stara zawartosc juz zostala pod-	*/
		/* pieta do listy i mozemy wykorzytac wskaznik na nowo.		*/
		ANY_UnizetoAttrSyntax = NULL;	 /*dla walidatora funkcji derenc_alloc	*/
		UnizetoAttrSyntaxIn_free_ptr(&UnizetoAttrSyntaxIn);
	}
	err = asn_set_add(&(AttributesSequence->list),Attribute);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBFILEERR[%s:%i:%s] Error in adding UnizetoAttrSyntax to Attributes Sequence.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

long AC_UnizetoAttrSyntax_build(UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn,
				UnizetoAttrSyntax_t **UnizetoAttrSyntax,
				const ConfigData_t *ConfigData)
{
long err = 0;
GeneralNames_t		*PolicyAuthority = NULL;
OBJECT_IDENTIFIER_t	*SyntaxOid = NULL;
Slot_t			*Slot = NULL;
long i = 0;

	if(UnizetoAttrSyntaxIn == NULL)
		return BMD_ERR_PARAM1;
	if(UnizetoAttrSyntax == NULL || *UnizetoAttrSyntax != NULL)
		return BMD_ERR_PARAM2;
	if(ConfigData == NULL)
		return BMD_ERR_PARAM3;

	/* Organ wystawiajacy atrybut */
	err = AC_ACDistinguishedName2GeneralNames(ConfigData->ACConfiguration->ACAttributesSection->
			ACAttributeUnizeto->unizeto_policy_authority, &PolicyAuthority);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating Policy Authority General Names.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* Oid skladni atrybutu */
	err = String2OID_alloc((char *)UnizetoAttrSyntaxIn->SyntaxOid->buf, &SyntaxOid);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating UnizetoAttrSyntax syntax Oid.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* Pusty UnizetoAttributeSyntax */
	err = AC_UnizetoAttrSyntax_create_alloc(PolicyAuthority,
					SyntaxOid,
					UnizetoAttrSyntax);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in creating UnizetoAttrSyntax.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* Zawartosc SyntaxOid zostala odziedziczona przez UnizetoAttrSyntax wiec strukture 	*/
	/* SyntaxOid mozna zwolnic */
	if(SyntaxOid) {free(SyntaxOid); SyntaxOid = NULL;}
	/* Dodajemy sloty */
	if(UnizetoAttrSyntaxIn->SlotList){
		for(i=0; i<UnizetoAttrSyntaxIn->SlotList->size; i++){
			Slot = NULL;
			err = AC_Slot_build(UnizetoAttrSyntaxIn->SlotList->Slots[i],
							&Slot, ConfigData);
			if(err<0){
				PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in building Slot_t.\n"
				"Recieved error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
				return err;
			}
			/*asn_fprint(stdout, &asn_DEF_Slot, Slot);*/
			err=asn_set_add(&((*UnizetoAttrSyntax)->slots.list),Slot);
			if( err != 0 )
				BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}
	return err;
}

long AC_Slot_build(	SlotIn_t *SlotIn,
				Slot_t **Slot,
				const ConfigData_t *ConfigData)
{
ANY_t *any					= NULL;
OBJECT_IDENTIFIER_t *SlotTypeOid	= NULL;
long i					= 0;


	if (SlotIn == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SlotIn->OidString == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SlotIn->OidString->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (Slot == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*Slot != NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ConfigData == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	/* Oid typu slotu */
	BMD_FOK(String2OID_alloc((char *)SlotIn->OidString->buf, &SlotTypeOid));
	/* Pusty Slot */
	BMD_FOK(AC_Slot_create_alloc(SlotTypeOid, Slot));
	/* Zawartosc SlotTypeOid zostala odziedziczona przez Slot wiec strukture 	*/
	/* SlotTypeOid mozna zwolnic */
	free(SlotTypeOid); SlotTypeOid = NULL;

	/* Dodajemy wartosci do Slotu. Na razie obslugiwane sa wartosci typu string(UTF8) i integer */
	if(SlotIn->ValuesList != NULL)
	{
		for(i=0; i<SlotIn->ValuesList->size; i++)
		{
			if(!strcmp((char *)SlotIn->OidString->buf, OID_UNIZETO_STX_INTEGER))
			{
				any = NULL;
				BMD_FOK(String2INTEGER2ANY_alloc((char *)SlotIn->ValuesList->gbufs[i]->buf, &any));
			}
			else if(!strcmp((char *)SlotIn->OidString->buf, OID_UNIZETO_STX_UTF8STRING))
			{
				any = NULL;
				BMD_FOK(String2UTF8String2ANY_alloc((char *)SlotIn->ValuesList->gbufs[i]->buf, &any));
			}
			else
			{
				BMD_FOK(ERR_AA_ATTRIBUTESDB_UNIZETO_UDEF_SLOT_STX);
			}
			BMD_FOK(AC_Slot_add_AnyValue(any, *Slot));
		}
	}
	return BMD_OK;
}

