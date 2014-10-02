#include "read_ac.h"
#include <bmd/libbmdaa2/ac_client/ac_client.h>
#include <bmd/libbmdcms/libbmdcms.h>

#define ATTRIBUTE_CERTIFICATE_FILENAME "bbanan1024.acer"

extern long _GLOBAL_debug_level;

int main(void)
{
int err			= 0;
GenBuf_t *GenBuf	= NULL;
long version		= 0;
char *SerialNumber	= NULL;
char *AttributeString	= NULL;
long AttributeLong	= 0;
char *HolderSN		= NULL;
char *HolderDN		= NULL;


char *SubjectDN		= NULL;
char *IssuerDN		= NULL;
char *SignatureAlgorithmOID = NULL;
char *ValidityNB	= NULL;
char *ValidityNA	= NULL;
char *SubjectSurname	= NULL;
char *SubjectGivenName	= NULL;
char *SubjectCommonName	= NULL;
char *CRLDistPoints	= NULL;

	_GLOBAL_debug_level = 2;
	err = read_GenBuf_from_file(ATTRIBUTE_CERTIFICATE_FILENAME, &GenBuf);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading attribute "
			"certificate file. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/*printf("INFO[%s:%i:%s] Attribute Certificate file read. Returned error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);*/
	/*****************************************************************************************************************/
	err = ACGetParam(GenBuf, AC_PARAM_VERSION, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading version from attribute "
			"certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Attribute certificate\n");
	printf("*********************\n");
	printf("Filename: %s\n", ATTRIBUTE_CERTIFICATE_FILENAME);
	printf("Version: %ld\n", AttributeLong);
	AttributeLong = 0;

	err = ACGetParam(GenBuf, AC_PARAM_SN, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading serial number from attribute "
			"certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Serial number: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_HOLDER_SN, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading holder PKC serial number from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Holder PKC Serial number: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_HOLDER_DN, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading holder PKC DN from attribute "
			"certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Holder PKC Distinguished Name: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_ISSUER_DN, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading issuer DN from attribute "
			"certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Issuer Distinguished Name: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_VALIDITY_NB, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading validity not before from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Validity (not before): %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_VALIDITY_NA, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading validity not after from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Validity (not after): %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_GROUP_POLICYAUTH, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading policy authority from group "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Policy Authority: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_GROUPS, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading groups from group from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Groups: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_ROLE_ROLEAUTH, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading groups from group from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Role Authority: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_ROLES, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading roles from role from "
			"attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Roles: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }


	err = ACGetParam(GenBuf, AC_PARAM_CLEARANCE_POLICYID, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading policy id from clearance "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Clearance Policy Id: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_CLEARANCES, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading clearances from clearance "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("Clearances: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = ACGetParam(GenBuf, AC_PARAM_UNIZETO_POLICYAUTH, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading policy authority from UnizetoAttrSyntax "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("UnizetoPolicyAuthority: %s\n", AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	UnizetoAttrSyntaxInList_t *UAS = NULL;
	GenBufList_t *RolesList = NULL;
	int i = 0;

	err = GetAttrUnizetoUnizetosFromAC(GenBuf, &UAS);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in UnizetoAttrSyntaxInList from UnizetoAttrSyntax "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	err = UnizetoAttrSyntaxInList2GenBufList(UAS, &RolesList, OID_UNIZETOATTRSYNTAX_ROLE_STR, OID_UNIZETO_STX_UTF8STRING_STR);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in extracting Roles from UnizetoAttrSyntax. "
			"from attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	for(i=0; i<RolesList->size; i++)
		printf("Role[%i] = %s\n", i, RolesList->gbufs[i]->buf);

	free_GenBufList(&RolesList);
	UnizetoAttrSyntaxInList_free_ptr(&UAS);


	free_gen_buf(&GenBuf);
	return err;
}
