#include <bmd/libbmdaa2/db/common/AC_db_clearance.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/AttributesSequence.h>
#include <bmd/common/bmd-sds.h>
long AC_MakeAttribute_Clearance(	GenBufList_t *ClearancesGenBufList,
                        		AttributesSequence_t *AttributesSequence,
                        		const ConfigData_t *ConfigData)
{
long i			= 0;
Attribute_t *Attribute 	= NULL;
Clearance_t *Clearance 	= NULL;
ANY_t *ANY_Clearance 	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(ClearancesGenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AC_CreateEmptyAttributeSet(&Attribute));
	BMD_FOK(AC_Attribute_set_TypeOid(ConfigData->ACConfiguration->ACAttributesSection->ACAttributeClearance->attribute_clearance_oid, Attribute));

	/************************************************************************************/
	/*	wartosci atrybutu Clearance jest tyle ile clearance w pliku attributes.db	*/
	/************************************************************************************/
	for(i=0; i<ClearancesGenBufList->size; i=i+3)
	{
		BMD_FOK(AC_Clearance_build(	ClearancesGenBufList->gbufs[i],
							ClearancesGenBufList->gbufs[i+1],
							ClearancesGenBufList->gbufs[i+2],
							&Clearance,
							ConfigData));

		/*asn_fprint(stdout, &asn_DEF_Clearance, Clearance);*/
		ANY_Clearance=ANY_new_fromType(&asn_DEF_Clearance,Clearance);
		if( ANY_Clearance == NULL )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		asn_DEF_Clearance.free_struct(&asn_DEF_Clearance,Clearance,1);Clearance=NULL;
		/* Tyle wartosci atrybutu ile ról */

		BMD_FOK(asn_set_add(&(Attribute->attrValues.list),ANY_Clearance));

		/* Jesli wszystko poszlo dobrze to przygotuj ANY jako wskaznik  */
		/* pod nowa strukture DERowa (stara zawartosc juz zostala pod-  */
		/* pieta do listy i mozemy wykorzytac wskaznik na nowo.         */
		ANY_Clearance = NULL;    /*dla walidatora funkcji derenc_alloc  */
	}

	BMD_FOK(asn_set_add(&(AttributesSequence->list),Attribute));

	return BMD_OK;
}

long AC_Clearance_build(	const GenBuf_t *ClassListString,
					const GenBuf_t *SecurityCategoryTypeString,
					const GenBuf_t *SecurityCategoryValueString,
					Clearance_t **Clearance,
					const ConfigData_t *ConfigData)
{
OBJECT_IDENTIFIER_t     *PolicyOid			= NULL;
SecurityCategory_t      *SecurityCategory 	= NULL;
OBJECT_IDENTIFIER_t     *SecurityCategoryType	= NULL;
ANY_t                   *SecurityCategoryValue	= NULL;
ClassList_t             *ClassList			= NULL;
long cl							= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (ClassListString == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ClassListString->size < 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SecurityCategoryTypeString == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (SecurityCategoryTypeString->size < 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (SecurityCategoryValueString == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (SecurityCategoryValueString->size < 0){	BMD_FOK(BMD_ERR_PARAM3);	}
	if (Clearance == NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*Clearance != NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(ConfigData == NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}

	/* Clearance policy OID */
	BMD_FOK(String2OID_alloc(ConfigData->ACConfiguration->ACAttributesSection->ACAttributeClearance->clearance_policy_oid, &PolicyOid));

	/* Konwersja stringu classlist na integer */
	if (strcmp(CLASS_LIST_UNMARKED_STR, 		(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_UNMARKED;
	else if (strcmp(CLASS_LIST_UNCLASSIFIED_STR,	(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_UNCLASSIFIED;
	else if (strcmp(CLASS_LIST_RESTRICTED_STR,	(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_RESTRICTED;
	else if (strcmp(CLASS_LIST_CONFIDENTIAL_STR,	(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_CONFIDENTIAL;
	else if (strcmp(CLASS_LIST_SECRET_STR,		(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_SECRET;
	else if (strcmp(CLASS_LIST_TOP_SECRET_STR,	(char *)ClassListString->buf) == 0)	cl = CLASS_LIST_TOP_SECRET;
	else														cl = CLASS_LIST_UNMARKED;

	/* BIT STRING Class List */
	BMD_FOK(AC_ClassList_create_alloc(cl, &ClassList));

	/* OID składni etykiety bezpieczeństwa (Security Type Value OID) */

 	BMD_FOK(String2OID_alloc((char *)SecurityCategoryTypeString->buf, &SecurityCategoryType));

	/* Etykieta bezpieczenstwa w polu SecurityCategory Type */
	BMD_FOK(String2UTF8String2ANY_alloc((char *)SecurityCategoryValueString->buf, &SecurityCategoryValue));

	/* Tworze SecurityCategory, składający się z ClassList oraz etykiety: SecurityCategoryType */
	/* i SecurityCategoryValue */
 	BMD_FOK(AC_SecurityCategory_create_alloc(SecurityCategoryType, SecurityCategoryValue, &SecurityCategory));

	/* Zawartość SecurityCategoryType oraz SecurityCategoryValue została odziedziczona      */
	/* przez SecurityCategory i same wskazniki moga byc zwolnione.                          */
	free(SecurityCategoryType); SecurityCategoryType = NULL;
	free(SecurityCategoryValue); SecurityCategoryValue = NULL;

	/* Podpinam wszystko pod Clearance */
	BMD_FOK(AC_Clearance_create_alloc(PolicyOid, ClassList, Clearance));

	/* Zawartosc PolicyOid została odziedziczona przez Clearance wiec sam wskaznik  */
	/* mozna juz zwolnic.                                                           */
      free(PolicyOid); PolicyOid = NULL;

	/* I dorzucam SecurityCategory */
	BMD_FOK(AC_ClearanceSet_add_SecurityCategory(SecurityCategory, *Clearance));

	return BMD_OK;
}


