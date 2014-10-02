#include <bmd/libbmdaa2/wrappers/AC_GeneralName.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_GeneralName_create_alloc(void *value,
			GeneralName_PR name_type,
			GeneralName_t **GeneralName)
{
int err = 0;
	if(value == NULL)
		return BMD_ERR_PARAM1;
	if(name_type < 0)
		return BMD_ERR_PARAM2;
	if(GeneralName == NULL || *GeneralName != NULL)
		return BMD_ERR_PARAM3;
	
	*GeneralName = (GeneralName_t *) malloc (sizeof(GeneralName_t));
	if(*GeneralName == NULL) return NO_MEMORY;
		
	err = AC_GeneralName_create(value, name_type, *GeneralName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating Generalname_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return err;
	}
	return err;
}

int AC_GeneralName_create(void *value,
			GeneralName_PR name_type,
			GeneralName_t *GeneralName)
{
int err = 0;
	if(value == NULL)
		return BMD_ERR_PARAM1;
	if(name_type < 0)
		return BMD_ERR_PARAM2;
	if(GeneralName == NULL)
		return BMD_ERR_PARAM3;

	memset(GeneralName,0,sizeof(GeneralName_t));
	/* Deskryptor wezla */
	GeneralName->present = name_type;
	/* Wartosc wezla */
	switch (name_type)
	{
		case GeneralName_PR_dNSName:
			err = AC_IA5String_clone_pointers((IA5String_t *)value,
											  &(GeneralName->choice.dNSName));
			if(err<0){
				PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning IA5String_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
				return err;
			}
			break;
		case GeneralName_PR_directoryName:
			err = AC_Name_clone_pointers((Name_t *)value,
										 &(GeneralName->choice.directoryName));
			if(err<0){
				PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning Name_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
				return err;
			}
			break;
		case GeneralName_PR_uniformResourceIdentifier:
			err = AC_IA5String_clone_pointers((IA5String_t *)value,
											  &(GeneralName->choice.uniformResourceIdentifier));
			if(err<0){
				PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning IA5String_t. "
					"Recieved error code = %i\n",
					__FILE__, __LINE__, __FUNCTION__, err);
				return err;
			}
			break;
		case GeneralName_PR_registeredID:		
			err = AC_OBJECT_IDENTIFIER_clone_pointers((OBJECT_IDENTIFIER_t *)value,
													  &(GeneralName->choice.registeredID));
			if(err<0){
				PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning OBJECT_IDENTIFIER. "
					"Recieved error code = %i\n",
					__FILE__, __LINE__, __FUNCTION__, err);
				return err;
			}
			break;
		case GeneralName_PR_NOTHING:
		default:	
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Inrecognized GeneralName type!\n",
				__FILE__, __LINE__, __FUNCTION__);
				return ERR_AA_UNRECOGNIZED_GENERALNAME_TYPE;
			break;
	}
	return err;
}

int AC_GeneralName_clone_pointers(GeneralName_t *src,
			GeneralName_t *dest)
{
	int err = 0;
	if(src == NULL)
		return BMD_ERR_PARAM1;
	if(dest == NULL)
		return BMD_ERR_PARAM2;

	memset(dest,0,sizeof(GeneralName_t));
	/* Deskryptor wezla */
	dest->present = src->present;
	/* Wartosc wezla */
	switch (src->present)
	{
		case GeneralName_PR_dNSName:
		err = AC_IA5String_clone_pointers(&(src->choice.dNSName),
					&(dest->choice.dNSName));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning IA5String_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		break;
	case GeneralName_PR_directoryName:
		err = AC_Name_clone_pointers(&(src->choice.directoryName),
					&(dest->choice.directoryName));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning Name_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		break;
		case GeneralName_PR_uniformResourceIdentifier:
		err = AC_IA5String_clone_pointers(&(src->choice.uniformResourceIdentifier),
					&(dest->choice.uniformResourceIdentifier));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning IA5String_t. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		break;
		case GeneralName_PR_registeredID:		
		err = AC_OBJECT_IDENTIFIER_clone_pointers(&(src->choice.registeredID),
					&(dest->choice.registeredID));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning OBJECT_IDENTIFIER. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		break;
	case GeneralName_PR_NOTHING:
	default:	
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Inrecognized GeneralName type!\n",
			__FILE__, __LINE__, __FUNCTION__);
			return ERR_AA_UNRECOGNIZED_GENERALNAME_TYPE;
		break;
	}
	return err;
}
