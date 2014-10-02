#include <bmd/libbmdaa2/wrappers/AC_IetfAttrSyntax.h>
#include <bmd/libbmderr/libbmderr.h>

/* Uwaga!!!: Dodanie pierwszego elementu okreslonego typu implikuje konieczność
   przestrzegania typów przy kolejnych wstawianiach (zgodnie z RFC3281). 
   Zawartość GroupValue jest kopiowana do struktury IetfAttrSyntax wiec 
   mozna ja zwolnic. */
int AC_IetfAttrSyntax_add_Group(EnumIetfAttrSyntaxGroupType_t EnumIetfAttrSyntaxGroupType, 
						GenBuf_t *GroupValue, 
						IetfAttrSyntax_t *IetfAttrSyntax)
{
int err = 0;
IetfAttrSyntax__values__Member_t *IetfAttrSyntaxValueChoice = NULL;
EnumIetfAttrSyntaxGroupType_t ValueSetMandatoryType;
		
	if(EnumIetfAttrSyntaxGroupType < 0)
		return BMD_ERR_PARAM1;
	if(GroupValue == NULL || GroupValue->buf == NULL || GroupValue->size < 0)
		return BMD_ERR_PARAM2;
	if(IetfAttrSyntax == NULL)
		return BMD_ERR_PARAM3;

	/* Jesli w zbiorze są już jakieś elementy to sprawdzaj ich typ */
	if(IetfAttrSyntax->values.list.size > 0){
		switch(IetfAttrSyntax->values.list.array[0]->present){
		case IetfAttrSyntax__values__Member_PR_octets:
			ValueSetMandatoryType = IETF_GROUP_OCTET_STRING;
			break;
		case IetfAttrSyntax__values__Member_PR_oid:
			ValueSetMandatoryType = IETF_GROUP_OID;
			break;
		case IetfAttrSyntax__values__Member_PR_string:
			ValueSetMandatoryType = IETF_GROUP_UTF8;
			break;
		case IetfAttrSyntax__values__Member_PR_NOTHING:
		default:
			ValueSetMandatoryType = IETF_GROUP_UNDEFINED;
			break;
		}
		/* wszystkie typy zbioru muszą byc jednakowe !!!*/
		if(ValueSetMandatoryType != EnumIetfAttrSyntaxGroupType){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in adding GroupValue to IetfValuesList!\n"
				"Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AA_IETFATTRIBUTEVALUE_NOT_COMPATIBLE; 
		}
	}
	/* Konwersja GenBufa na odpowiedni prymityw ASN1 */
	err = GroupValue2ValueChoice(GroupValue, EnumIetfAttrSyntaxGroupType,
				&IetfAttrSyntaxValueChoice);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in adding GroupValue to IetfValuesList!\n"
			"Recieved error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_CREATE_OCTET_STRING_FROM_GROUP;
	}	
	/* Dodanie elementu do zbioru */
	err = asn_set_add(&(IetfAttrSyntax->values.list), IetfAttrSyntaxValueChoice);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAINF[%s:%i:%s] Error in adding IetfAttrSyntaxValueChoice "
				"to IetfAttrSyntax. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_ASN1_SET_ADD;
	}
	return err;
}

int GroupValue2ValueChoice(GenBuf_t *GroupValue, 
		EnumIetfAttrSyntaxGroupType_t EnumIetfAttrSyntaxGroupType,
		IetfAttrSyntax__values__Member_t **IetfAttrSyntaxValueChoice)
{
int err = 0;
	if(GroupValue == NULL || GroupValue->buf == NULL || GroupValue->size < 0)
		return BMD_ERR_PARAM1;
	if(EnumIetfAttrSyntaxGroupType < 0)
		return BMD_ERR_PARAM2;
	if(IetfAttrSyntaxValueChoice == NULL 
		|| *IetfAttrSyntaxValueChoice != NULL)
		return BMD_ERR_PARAM3;
	
	*IetfAttrSyntaxValueChoice = (IetfAttrSyntax__values__Member_t *) 
			malloc (sizeof (IetfAttrSyntax__values__Member_t));
	if(*IetfAttrSyntaxValueChoice == NULL) return NO_MEMORY;
	memset(*IetfAttrSyntaxValueChoice, 0, 
		sizeof(IetfAttrSyntax__values__Member_t));
		
	switch(EnumIetfAttrSyntaxGroupType){
	case IETF_GROUP_OCTET_STRING:
		(*IetfAttrSyntaxValueChoice)->present=IetfAttrSyntax__values__Member_PR_octets;
		memset(&((*IetfAttrSyntaxValueChoice)->choice.octets),0,sizeof(OCTET_STRING_t));
		err=OCTET_STRING_fromBuf(&((*IetfAttrSyntaxValueChoice)->choice.octets),
								 (const char *)GroupValue->buf, GroupValue->size);
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating OCTET_STRING from GroupValue!\n"
				"Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AA_CREATE_OCTET_STRING_FROM_GROUP;
		}
		break;
	case IETF_GROUP_OID:
		(*IetfAttrSyntaxValueChoice)->present=IetfAttrSyntax__values__Member_PR_oid;
		if((int)(strlen((char *)GroupValue->buf)+1) !=  GroupValue->size){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Oid string in GroupValue has different strlen " 
			"than GenGuf size!\n",
			__FILE__, __LINE__, __FUNCTION__);
			return ERR_AA_WRONG_GENBUF_WITH_OID_STRING;
		}
		err = String2OID((char *)GroupValue->buf, 
			&((*IetfAttrSyntaxValueChoice)->choice.oid));
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating OBJECT_IDENTIFIER from "
			"GroupValue!\n Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AA_CREATE_OBJECT_IDENTIFIER_FROM_GROUP;
		}
		break;
	case IETF_GROUP_UTF8:
		(*IetfAttrSyntaxValueChoice)->present=IetfAttrSyntax__values__Member_PR_string;
		memset(&((*IetfAttrSyntaxValueChoice)->choice.string),0,sizeof(UTF8String_t));
		err=OCTET_STRING_fromBuf((OCTET_STRING_t *)&((*IetfAttrSyntaxValueChoice)->choice.string),
								 (const char *)GroupValue->buf,GroupValue->size);
		if(err<0){
			PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating UTF8String from GroupValue!\n"
				"Recieved error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AA_CREATE_UTF8STRING_FROM_GROUP;
		}
		break;
	case IETF_GROUP_UNDEFINED:
	default:
		(*IetfAttrSyntaxValueChoice)->present=IetfAttrSyntax__values__Member_PR_NOTHING;
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Unsupported GroupValue!\n"
			"Recieved error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_UNSUPPORTED_GROUP_TYPE;
		break;
	}
	return err;
}
