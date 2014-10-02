#include "gen_name.h"


extern long _GLOBAL_debug_level;
/*SQL_validator_data_t *_GLOBAL_validator_data=NULL;*/


int main(int argc, char *argv[])
{
int err = 0;
Name_t *Name = NULL;

	_GLOBAL_debug_level = 2;

	err =  AC_CreateEmptyNameSet(&Name);
	if(err<0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in creating empty name set\n",
				__FILE__, __LINE__, __FUNCTION__);
	}

	err = AC_NameSetAddValue(DN_VALUE_COMMONNAME, "Ala Kowalska", Name);
	if(err<0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in adding string to name set\n",
				__FILE__, __LINE__, __FUNCTION__);
	}
	err = AC_NameSetAddValue(DN_VALUE_ORGANIZATION, "Unizeto", Name);
	if(err<0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in adding string to name set\n",
				__FILE__, __LINE__, __FUNCTION__);
	}
	asn_fprint(stdout, &asn_DEF_Name, Name);

	err = AC_Name_free_ptr(&Name);
	if(err<0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in freeing name set\n",
				__FILE__, __LINE__, __FUNCTION__);
	}
	printf("end\n");
	return err;
}


