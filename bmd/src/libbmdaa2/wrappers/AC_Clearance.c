#include <bmd/libbmdaa2/wrappers/AC_Clearance.h>
#include <bmd/libbmderr/libbmderr.h>
int AC_Clearance_create(const OBJECT_IDENTIFIER_t *ClearancePolicyOid,
			ClassList_t *ClassList,
			Clearance_t *Clearance)
{
int err = 0;
	if(ClearancePolicyOid == NULL)
		return BMD_ERR_PARAM1;
	if(ClassList == NULL)
		return BMD_ERR_PARAM2;
	if(Clearance == NULL)
		return BMD_ERR_PARAM3;

	memset(Clearance,0,sizeof(Clearance_t));
	/* OID to primitive type */
	Clearance->policyId.buf  = ClearancePolicyOid->buf;
	Clearance->policyId.size = ClearancePolicyOid->size;
	/* ClassList */
	Clearance->classList = ClassList;
	return err;
}

int AC_Clearance_create_alloc(const OBJECT_IDENTIFIER_t *ClearancePolicyOid,
			ClassList_t *ClassList,
			Clearance_t **Clearance)
{
int err = 0;
	if(ClearancePolicyOid == NULL)
		return BMD_ERR_PARAM1;
	if(ClassList == NULL)
		return BMD_ERR_PARAM2;
	if(Clearance == NULL || *Clearance != NULL)
		return BMD_ERR_PARAM3;

	*Clearance = (Clearance_t *) malloc (sizeof(Clearance_t));
	if(*Clearance == NULL) return NO_MEMORY;

	err = AC_Clearance_create(ClearancePolicyOid, ClassList, *Clearance);
	if(err<0){
		PRINT_ERROR("LIBBMDAAERR[%s:%i:%s] Error in creating Clearance_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

/* Wywolywac przy kolejnym dodawaniu SecurityCategory do zbioru. */
int AC_ClearanceSet_add_SecurityCategory(	const SecurityCategory_t *SecurityCategory,
							Clearance_t *Clearance)
{
int err		= 0;

	if(SecurityCategory == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if(Clearance == NULL)
	{
		return BMD_ERR_PARAM2;
	}

	if(Clearance->securityCategories == NULL)
	{
		Clearance->securityCategories = (struct Clearance__securityCategories *)malloc(sizeof(struct Clearance__securityCategories));
		if(Clearance->securityCategories == NULL)
		{
			PRINT_ERROR("Memory Error. Error=%i\n",NO_MEMORY);
			return NO_MEMORY;
		}
		memset(Clearance->securityCategories, 0, sizeof(struct Clearance__securityCategories));
	}

	err = asn_set_add(&(Clearance->securityCategories->list), (void *)SecurityCategory);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAAERR Error in adding SecurityCategory to Clearance. Error=%i\n", err);
		return ERR_AA_ASN1_SET_ADD;
	}

	return err;
}
