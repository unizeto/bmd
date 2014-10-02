#include <bmd/libbmdaa2/db/common/AC_db_common.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
	#ifndef open
		#define open _open
	#endif
	#ifndef read
		#define read _read
	#endif
	#ifndef close
		#define close _close
	#endif
#endif

/*
 * Funkcja rozbija string "string" na grupę podstringow, ktore umieszczone sa w alokowanej wewnatrz
 * funkcji w strukturze SubstringsGenBuf_t. Rozbicie następuje przy użyciu separatora "separator".
 */

long SplitString(const char *string, const char *separator, SubstringsGenBuf_t **splitted_substrings)
{
long err = 0;
GenBuf_t *ParsingBuff = NULL;
char *split_regexp = NULL;

	asprintf(&split_regexp, "[^%s]+", separator);
	if(split_regexp == NULL) return NO_MEMORY;

	*splitted_substrings = (SubstringsGenBuf_t *) malloc (sizeof(SubstringsGenBuf_t));
	if(*splitted_substrings == NULL) return NO_MEMORY;

	err = String2GenBuf_alloc(string, &ParsingBuff);
	if(err<0) return err;

	err = find_substrings(ParsingBuff, split_regexp, *splitted_substrings, SINGLELINE, YES);
	switch (err)
	{
		case PCRE_ERROR_NOMATCH: 	break;
		case 0:				break; /* wszystko OK wiec wyjdz */
		default:			return err; break;
		/* no default yet */
	}
	if(split_regexp != NULL)
	{
		free(split_regexp);
		split_regexp = NULL;
	}
	free_GenBuf(&ParsingBuff);
	return err;
}

/* Funkcja tworzy sekwencje GeneralNames (1 elementową zawierajacy element typu Name_t) z nazwą
   organu podaną w  ACDistinguishedName */
long AC_ACDistinguishedName2GeneralNames(const ACDistinguishedName_t *ACDistinguishedName, 
				GeneralNames_t **GeneralNames)
{
long err = 0;
Name_t *Name = NULL;
GeneralName_t *GeneralName = NULL;

	if(ACDistinguishedName == NULL)
		return BMD_ERR_PARAM1;
	if(GeneralNames == NULL || *GeneralNames != NULL)
		return BMD_ERR_PARAM2;
	
	Name=(Name_t *)malloc(sizeof(Name_t));
	if( Name == NULL )
		BMD_FOK(BMD_ERR_MEMORY);
	memset(Name,0,sizeof(Name_t));
	Name->present = Name_PR_rdnSequence;
	err = AC_NameSetAddValue(DN_VALUE_COUNTRY, 
			ACDistinguishedName->country, 
			Name);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting country into Name set.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_NameSetAddValue(DN_VALUE_STATEORPROVINCE, 
			ACDistinguishedName->state, 
			Name);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting state into Name set.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_NameSetAddValue(DN_VALUE_ORGANIZATION, 
			ACDistinguishedName->organization, 
			Name);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting organization into Name set.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_NameSetAddValue(DN_VALUE_ORGANIZATIONALUNIT, 
			ACDistinguishedName->organizational_unit, 
			Name);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting organizational unit into Name set.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = AC_GeneralName_create_alloc((void *)Name, GeneralName_PR_directoryName, &GeneralName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in inserting Name into GeneralName.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* Name_t zostało już podpięte zawartością pod GenralNeme_t więc samą strukturę Name_t	*/
	/* można zwolnić.									*/
	if(Name) {free(Name); Name = NULL;}
	(*GeneralNames)=(GeneralNames_t *)malloc(sizeof(GeneralNames_t));
	if( (*GeneralNames) == NULL )
		return BMD_ERR_MEMORY;
	memset(*GeneralNames,0,sizeof(GeneralNames_t));
	err=asn_set_add(&((*GeneralNames)->list),GeneralName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAADBERR[%s:%i:%s] Error in adding GeneralName to GeneralNames.\n"
		"Recieved error code = %li\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

long UnizetoAttrSyntaxInList_free_ptr(UnizetoAttrSyntaxInList_t **UnizetoAttrSyntaxInList)
{
long err = 0;
long i = 0;
	if(UnizetoAttrSyntaxInList == NULL || *UnizetoAttrSyntaxInList == NULL)
		return BMD_ERR_PARAM1;
	
	for(i=0; i<(*UnizetoAttrSyntaxInList)->size; i++){
		UnizetoAttrSyntaxIn_free_ptr(&((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn[i]));
	}
	if((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn){free((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn);
					(*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn = NULL; }
	if(*UnizetoAttrSyntaxInList) {free(*UnizetoAttrSyntaxInList); *UnizetoAttrSyntaxInList = NULL;}
	return err;
}

long UnizetoAttrSyntaxIn_free_ptr(UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn)
{
long err = 0;
	if(UnizetoAttrSyntaxIn == NULL || *UnizetoAttrSyntaxIn == NULL)
		return BMD_ERR_PARAM1;
	
	free_GenBuf(&((*UnizetoAttrSyntaxIn)->SyntaxOid));
	SlotList_free_ptr(&((*UnizetoAttrSyntaxIn)->SlotList));
	if(*UnizetoAttrSyntaxIn) {free(*UnizetoAttrSyntaxIn); *UnizetoAttrSyntaxIn = NULL;}
	return err;
}

long SlotList_free_ptr(SlotListIn_t **SlotList)
{
long err = 0;
long i = 0;
	if(SlotList == NULL || *SlotList == NULL)
		return BMD_ERR_PARAM1;

	for(i=0; i<(*SlotList)->size; i++){
		SlotIn_free_ptr(&((*SlotList)->Slots[i]));
	}
	if((*SlotList)->Slots) {free((*SlotList)->Slots); (*SlotList)->Slots = NULL;}
	if(*SlotList) {free(*SlotList); *SlotList = NULL;}
	return err;
}

long SlotIn_free_ptr(SlotIn_t **SlotIn)
{
long err = 0;
	if(SlotIn == NULL || *SlotIn == NULL)
		return BMD_ERR_PARAM1;
	
	free_GenBuf(&((*SlotIn)->OidString));
	free_GenBufList(&((*SlotIn)->ValuesList));
	if(*SlotIn) {free(*SlotIn); *SlotIn = NULL;}
	return err;
}
