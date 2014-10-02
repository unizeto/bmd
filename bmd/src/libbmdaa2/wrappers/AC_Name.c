#include <bmd/libbmdaa2/wrappers/AC_Name.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_NameSetAddValue(DNValueType_t DNValueType, const char *DNValue, Name_t *Name)
{
int err = 0;
	if(DNValueType < 0)
		return BMD_ERR_PARAM1;
	if(DNValue == NULL)
		return BMD_ERR_PARAM2;
	if(Name == NULL)
		return BMD_ERR_PARAM3;
	/* Utworz element wewnatrz listy RDNSequence znajdujacej sie w Name 	*/
	/* zawierajacy podany w DNValue string.					*/
	err = AC_RDNSequenceSetAddValue(DNValueType, DNValue, &(Name->choice.rdnSequence));
	if(err<0){
		PRINT_DEBUG("LIBAAERR[%s:%i:%s] Error in adding DN value to RDNSequence. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
	}
	return err;
}

int AC_Name_clone_pointers(Name_t *src, Name_t *dest)
{
int err = 0;
	if(src == NULL)
		return BMD_ERR_PARAM1;
	if(dest == NULL)
		return BMD_ERR_PARAM2;

	memset(dest,0,sizeof(Name_t));
	dest->present = src->present;
	switch (src->present)
	{
	case Name_PR_rdnSequence:	/* TODO */
		err = AC_RDNSequence_clone_pointers(&(src->choice.rdnSequence), 
					&(dest->choice.rdnSequence));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning RDNSequence_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		break;
	case Name_PR_NOTHING:
	default:	
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Inrecognized Name type!\n",
			__FILE__, __LINE__, __FUNCTION__);
			return ERR_AA_UNRECOGNIZED_NAME_TYPE;
		break;
	}
	return err;
}
