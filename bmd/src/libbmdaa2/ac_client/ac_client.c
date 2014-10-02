/***************************************************************************
 *            ac_client.c
 *  BMD Attribute  Certificate parsing library
 *  Thu Mar  1 13:52:17 CET 2007
 *  Copyright  2007  Marcin Szulga
 *  Email:  mszulga@unet.pl
 ****************************************************************************/

#include <bmd/libbmdaa2/ac_client/ac_client.h>
#include <bmd/libbmderr/libbmderr.h>
/****************
 * ONE CALL API *
 ***************/
long ACGetParam(	GenBuf_t *GenBuf,
			ACParamType_t ACParamType,
			char **AttributeString,
			long *AttributeLong)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ACParamType < 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttributeString == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*AttributeString != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (AttributeLong == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*AttributeLong != 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	switch(ACParamType)
	{
		case AC_PARAM_VERSION:
			BMD_FOK(GetVersionFromAC(GenBuf, AttributeLong));
			break;
		case AC_PARAM_HOLDER_DN:
			BMD_FOK(GetHolderDNFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_HOLDER_SN:
			BMD_FOK(GetHolderSNFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_SN:
			BMD_FOK(GetSNFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_ISSUER_DN:
			BMD_FOK(GetIssuerDNFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_VALIDITY_NB:
			BMD_FOK(GetValidityNBFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_VALIDITY_NA:
			BMD_FOK(GetValidityNAFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_GROUP_POLICYAUTH:
			BMD_FOK(GetAttrGroupPolicyAuthorityFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_GROUPS:
			BMD_FOK(GetAttrGroupGroupsFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_ROLE_ROLEAUTH:
			BMD_FOK(GetAttrRoleRoleAuthorityFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_ROLES:
			BMD_FOK(GetAttrRoleRolesFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_CLEARANCE_POLICYID:
			BMD_FOK(GetAttrClearancePolicyIdFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_CLEARANCES:
			BMD_FOK(GetAttrClearanceClearancesFromAC(GenBuf, AttributeString));
			break;
		case AC_PARAM_UNIZETO_POLICYAUTH:
			BMD_FOK(GetAttrUnizetoPolicyAuthorityFromAC(GenBuf, AttributeString));
			break;
		default:
			BMD_FOK(ERR_AC_UNRECOGNIZED_PARAM_TYPE);
			break;
	}

	return BMD_OK;
}

/************
 * EASY API *
 ***********/
long GetVersionFromAC(	GenBuf_t *GenBuf,
				long *version)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (version == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*version != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetVersionFromACStruct(AC, version));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetHolderDNFromAC(	GenBuf_t *GenBuf,
				char **holderDN)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (holderDN == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*holderDN != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetHolderDNFromACStruct(AC, holderDN));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetHolderSNFromAC(	GenBuf_t *GenBuf,
				char **holderSN)
{
AttributeCertificate_t *AC	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (holderSN == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*holderSN != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetHolderSNFromACStruct(AC, holderSN));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetSNFromAC( GenBuf_t *GenBuf,
			char **SN)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SN == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*SN != 0)				{	BMD_FOK(BMD_ERR_PARAM2);	}


	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetSNFromACStruct(AC, SN));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetIssuerDNFromAC(	GenBuf_t *GenBuf,
				char **IssuerDN)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (IssuerDN == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*IssuerDN != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetIssuerDNFromACStruct(AC, IssuerDN));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}


long GetValidityNBFromAC(	GenBuf_t *GenBuf,
					char **ValidityNB)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ValidityNB == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*ValidityNB != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetValidityNBFromACStruct(AC, ValidityNB));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetValidityNAFromAC(	GenBuf_t *GenBuf,
					char **ValidityNA)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ValidityNA == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*ValidityNA != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetValidityNAFromACStruct(AC, ValidityNA));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrGroupPolicyAuthorityFromAC(	GenBuf_t *GenBuf,
							char **AttrGroupPolicyAuthority)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrGroupPolicyAuthority==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttrGroupPolicyAuthority != 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrGroupPolicyAuthorityFromACStruct(AC, AttrGroupPolicyAuthority));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrGroupGroupsFromAC(	GenBuf_t *GenBuf,
						char **Groups)
{
AttributeCertificate_t *AC	= NULL;
GenBufList_t *GenBufList	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (Groups == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*Groups != 0)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrGroupGroupsFromACStruct(AC, &GenBufList));
	BMD_FOK(GenBufList2string(GenBufList, Groups));

	free_GenBufList(&GenBufList);
	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrRoleRoleAuthorityFromAC(	GenBuf_t *GenBuf,
							char **AttrRoleRoleAuthority)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrRoleRoleAuthority == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttrRoleRoleAuthority != 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrRoleRoleAuthorityFromACStruct(AC, AttrRoleRoleAuthority));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrRoleRolesFromAC(	GenBuf_t *GenBuf,
					char **Roles)
{
AttributeCertificate_t *AC	= NULL;
GenBufList_t *GenBufList	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (Roles == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*Roles != 0)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrRoleRolesFromACStruct(AC, &GenBufList));
	BMD_FOK(GenBufList2string(GenBufList, Roles));

	free_GenBufList(&GenBufList);
	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrClearancePolicyIdFromAC(	GenBuf_t *GenBuf,
							char **AttrClearancePolicyId)
{
AttributeCertificate_t *AC = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrClearancePolicyId == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttrClearancePolicyId != 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrClearancePolicyIdFromACStruct(AC, AttrClearancePolicyId));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}


long GetAttrClearanceClearancesFromAC(	GenBuf_t *GenBuf,
							char **Clearances)
{
AttributeCertificate_t *AC = NULL;
GenBufList_t *GenBufList = NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (Clearances == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*Clearances != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrClearanceClearancesFromACStruct(AC, &GenBufList));
	BMD_FOK(GenBufList2string(GenBufList, Clearances));

	free_GenBufList(&GenBufList);
	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrUnizetoPolicyAuthorityFromAC(	GenBuf_t *GenBuf,
							char **PolicyAuthority)
{
AttributeCertificate_t *AC	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (PolicyAuthority == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*PolicyAuthority != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrUnizetoPolicyAuthorityFromACStruct(AC, PolicyAuthority));
	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

long GetAttrUnizetoUnizetosFromAC(	GenBuf_t *GenBuf,
						UnizetoAttrSyntaxInList_t **UAS)
{
AttributeCertificate_t *AC	= NULL;

	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if (GenBuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (UAS == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*UAS != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetACStructFromAC(GenBuf, &AC));
	BMD_FOK(GetAttrUnizetoUnizetosFromACStruct(AC, UAS));

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, AC, 0);

	return BMD_OK;
}

/************
 * FAST API *
 ***********/
long GetACStructFromAC(	GenBuf_t *GenBuf,
				AttributeCertificate_t **AC)
{
asn_dec_rval_t rval;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AC != NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	rval = ber_decode(0, &asn_DEF_AttributeCertificate,  (void **)AC, GenBuf->buf, GenBuf->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, *AC, 0);
		BMD_FOK(ERR_AC_DECODE_CERTIFICATE_DATA);
	}

	return BMD_OK;
}

long FreeACStruct(	AttributeCertificate_t **AC)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}

	asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate, *AC, 0);
	*AC = NULL;

	return BMD_OK;
}

long GetVersionFromACStruct(	AttributeCertificate_t *AC,
					long *version)
{

	if(AC == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(version == NULL || *version != 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(asn_INTEGER2long(&(AC->acinfo.version), version));

	return BMD_OK;
}

long GetHolderDNFromACStruct(	AttributeCertificate_t *AC,
					char **holderDN)
{
long err = 0;

	if(AC == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(holderDN == NULL || *holderDN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(AC->acinfo.holder.baseCertificateID == NULL)	{	BMD_FOK(ERR_AC_NO_BASE_CERTIFIACTE_ID_PRESENT);	}

	BMD_FOK(GeneralNames2string(&(AC->acinfo.holder.baseCertificateID->issuer), holderDN));

	return err;
}

long GetHolderSNFromACStruct(AttributeCertificate_t *AC, char **holderSN)
{
long err = 0;

	if(AC == NULL)
		return BMD_ERR_PARAM1;
	if(holderSN == NULL || *holderSN != NULL)
		return BMD_ERR_PARAM2;

	if(AC->acinfo.holder.baseCertificateID == NULL)
		return ERR_AC_NO_BASE_CERTIFIACTE_ID_PRESENT;

	err = INTEGER2string(&(AC->acinfo.holder.baseCertificateID->serial), holderSN);
	if(err<0){
		PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting INTEGER_t to string. "
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING;
	}
	return err;
}

long GetSNFromACStruct(AttributeCertificate_t *AC, char **SN)
{
long err = 0;

	if(AC == NULL)
		return BMD_ERR_PARAM1;
	if(SN == NULL || *SN != NULL)
		return BMD_ERR_PARAM2;


	err = INTEGER2string(&(AC->acinfo.serialNumber), SN);
	if(err<0){
		PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting INTEGER_t to string. "
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING;
	}
	return err;
}

long GetIssuerDNFromACStruct(AttributeCertificate_t *AC, char **issuerDN)
{
long err = 0;

	if(AC == NULL)
		return BMD_ERR_PARAM1;
	if(issuerDN == NULL || *issuerDN != NULL)
		return BMD_ERR_PARAM2;

	switch(AC->acinfo.issuer.present){
	case AttCertIssuer_PR_v1Form:
		err = GeneralNames2string(&(AC->acinfo.issuer.choice.v1Form), issuerDN);
		if(err<0){
			PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting GeneralNames_t to string. "
				"Recieved error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING;
		}
		break;
	case AttCertIssuer_PR_v2Form:
		if(AC->acinfo.issuer.choice.v2Form.issuerName == NULL)
			return ERR_AC_NO_V2FORM_ISSUERNAME_PRESENT;
		err = GeneralNames2string(AC->acinfo.issuer.choice.v2Form.issuerName, issuerDN);
		if(err<0){
			PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting GeneralNames_t to string. "
				"Recieved error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING;
		}
		break;
	case AttCertIssuer_PR_NOTHING:
	default:
			return ERR_AC_UNKNOWN_AC_ISSUER_FORMAT;
		break;
	}


	return err;
}

long GetValidityNBFromACStruct(AttributeCertificate_t *AC, char **ValidityNB)
{
long err = 0;

	if(AC == NULL)
		return BMD_ERR_PARAM1;
	if(ValidityNB == NULL || *ValidityNB != NULL)
		return BMD_ERR_PARAM2;


	err = GeneralizedTime2string(&(AC->acinfo.attrCertValidityPeriod.notBeforeTime), ValidityNB);
	if(err<0){
		PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting GeneralizedTime_t to string. "
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AC_CONVERT_GENERALIZEDTIME_T_TO_STRING;
	}
	return err;
}

long GetValidityNAFromACStruct(AttributeCertificate_t *AC, char **ValidityNA)
{
long err = 0;

	if(AC == NULL)
		return BMD_ERR_PARAM1;
	if(ValidityNA == NULL || *ValidityNA != NULL)
		return BMD_ERR_PARAM2;

	err = GeneralizedTime2string(&(AC->acinfo.attrCertValidityPeriod.notAfterTime), ValidityNA);
	if(err<0){
		PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting GeneralizedTime_t to string. "
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AC_CONVERT_GENERALIZEDTIME_T_TO_STRING;
	}
	return err;
}

long GetAttrGroupPolicyAuthorityFromACStruct(	AttributeCertificate_t *AC,
								char **AttrGroupPolicyAuthority)
{
Attribute_t *Attribute			= NULL;
char 	oid_attribute_group[]		= {OID_RFC3281_GROUP};
IetfAttrSyntax_t	*IetfAttrSyntax	= NULL;
asn_dec_rval_t rval;

	if (AC == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrGroupPolicyAuthority == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttrGroupPolicyAuthority != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}


	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_group),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	/* Dla grup Atrybut ma tylko 1 wartosc, zas grupy sa umieszczone wewnatrz IetfAttrSyntax */
	rval = ber_decode(0, &asn_DEF_IetfAttrSyntax,  (void **)&IetfAttrSyntax,
				Attribute->attrValues.list.array[0]->buf,
				Attribute->attrValues.list.array[0]->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_IetfAttrSyntax.free_struct(&asn_DEF_IetfAttrSyntax, IetfAttrSyntax, 0);
		return ERR_AC_DECODE_IETFATTRSYNTAX;
	}
	if(IetfAttrSyntax->policyAuthority == NULL)
		return ERR_AC_POLICYAUTHORITY_NOT_IN_GROUPS;

	BMD_FOK_CHG(GeneralNames2string(IetfAttrSyntax->policyAuthority, AttrGroupPolicyAuthority),
			ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING);

	asn_DEF_IetfAttrSyntax.free_struct(&asn_DEF_IetfAttrSyntax, IetfAttrSyntax, 0);
	return BMD_OK;
}

long GetAttrGroupGroupsFromACStruct(	AttributeCertificate_t *AC,
							GenBufList_t **GenBufList)
{
long err = 0;
Attribute_t *Attribute			= NULL;
char 	oid_attribute_group[]   	= {OID_RFC3281_GROUP};
IetfAttrSyntax_t	*IetfAttrSyntax	= NULL;
asn_dec_rval_t rval;
long i = 0;

	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBufList == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*GenBufList != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_group),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	/* Dla grup Atrybut ma tylko 1 wartosc, zas grupy sa umieszczone wewnatrz IetfAttrSyntax */
	rval = ber_decode(0, &asn_DEF_IetfAttrSyntax,  (void **)&IetfAttrSyntax,
				Attribute->attrValues.list.array[0]->buf,
				Attribute->attrValues.list.array[0]->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_IetfAttrSyntax.free_struct(&asn_DEF_IetfAttrSyntax, IetfAttrSyntax, 0);
		BMD_FOK(ERR_AC_DECODE_IETFATTRSYNTAX);
	}
	if(IetfAttrSyntax->policyAuthority == NULL)	{	BMD_FOK(ERR_AC_POLICYAUTHORITY_NOT_IN_GROUPS);	}

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*GenBufList == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*GenBufList, 0, sizeof(GenBufList_t));

	(*GenBufList)->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t *) * IetfAttrSyntax->values.list.count);
	if((*GenBufList)->gbufs == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*GenBufList)->gbufs, 0, sizeof(GenBuf_t *) * IetfAttrSyntax->values.list.count);

	for(i=0; i<IetfAttrSyntax->values.list.count; i++){
		(*GenBufList)->gbufs[i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}

		switch(IetfAttrSyntax->values.list.array[i]->present)
		{
		case IetfAttrSyntax__values__Member_PR_string:
		case IetfAttrSyntax__values__Member_PR_octets:
			(*GenBufList)->gbufs[i]->buf = (char *)
				malloc (sizeof(char) * IetfAttrSyntax->values.list.array[i]->choice.string.size);
			if((*GenBufList)->gbufs[i]->buf == NULL)	{	BMD_FOK(NO_MEMORY);	}
			memmove((*GenBufList)->gbufs[i]->buf,
				IetfAttrSyntax->values.list.array[i]->choice.string.buf,
				IetfAttrSyntax->values.list.array[i]->choice.string.size);
			(*GenBufList)->gbufs[i]->size = IetfAttrSyntax->values.list.array[i]->choice.string.size;
			break;
		case IetfAttrSyntax__values__Member_PR_oid:
			err = OBJECT_IDENTIFIER2string(&IetfAttrSyntax->values.list.array[i]->choice.oid,
				(char **)&((*GenBufList)->gbufs[i]->buf));
			PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error converting OBJECT_IDENTIFIER to string. "
				"Recieved error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			(*GenBufList)->gbufs[i]->size = 1 + (int)strlen((char *)(*GenBufList)->gbufs[i]->buf);
			return err;
			break;
		case IetfAttrSyntax__values__Member_PR_NOTHING:
		default:
			memset((*GenBufList)->gbufs[i], 0, sizeof(GenBuf_t));
			break;
		}

	}

	(*GenBufList)->size = IetfAttrSyntax->values.list.count;
	asn_DEF_IetfAttrSyntax.free_struct(&asn_DEF_IetfAttrSyntax, IetfAttrSyntax, 0);
	return err;
}

long GetAttrRoleRoleAuthorityFromACStruct(	AttributeCertificate_t *AC,
								char **AttrRoleRoleAuthority)
{
Attribute_t *Attribute		= NULL;
char 	oid_attribute_role[]	= {OID_RFC3281_ROLE};
RoleSyntax_t *RoleSyntax	= NULL;
asn_dec_rval_t rval;

	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrRoleRoleAuthority==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttrRoleRoleAuthority!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_role),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	/* TODO Zakladam ze certyfikat dla kazdej roli ma wpisane to samo RoleAuthority
	   tak wiec wystarczy pobrac tylko RoleAuthority z pierwszej struktury RoleSyntax */
	rval = ber_decode(0, &asn_DEF_RoleSyntax,  (void **)&RoleSyntax,
				Attribute->attrValues.list.array[0]->buf,
				Attribute->attrValues.list.array[0]->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_RoleSyntax.free_struct(&asn_DEF_RoleSyntax, RoleSyntax, 0);
		BMD_FOK(ERR_AC_DECODE_ROLESYNTAX);
	}
	if(RoleSyntax->roleAuthority == NULL)
	{
		BMD_FOK(ERR_AC_ROLEAUTHORITY_NOT_IN_ROLES);
	}

	BMD_FOK_CHG(GeneralNames2string(RoleSyntax->roleAuthority, AttrRoleRoleAuthority),
			ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING);

	asn_DEF_RoleSyntax.free_struct(&asn_DEF_RoleSyntax, RoleSyntax, 0);

	return BMD_OK;
}

long GetAttrRoleRolesFromACStruct(	AttributeCertificate_t *AC,
						GenBufList_t **GenBufList)
{
Attribute_t *Attribute 		= NULL;
char oid_attribute_role[]	= {OID_RFC3281_ROLE};
RoleSyntax_t *RoleSyntax 	= NULL;
asn_dec_rval_t rval;
long i				= 0;

	if (AC == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*GenBufList != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_role),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*GenBufList == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*GenBufList, 0, sizeof(GenBufList_t));

	(*GenBufList)->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t *) * Attribute->attrValues.list.count);
	if((*GenBufList)->gbufs == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*GenBufList)->gbufs, 0, sizeof(GenBuf_t *) * Attribute->attrValues.list.count);

	for(i=0; i<Attribute->attrValues.list.count; i++)
	{
		rval = ber_decode(0, &asn_DEF_RoleSyntax,  (void **)&RoleSyntax,
					Attribute->attrValues.list.array[i]->buf,
					Attribute->attrValues.list.array[i]->size);
		if(rval.code != RC_OK)
		{
			asn_DEF_RoleSyntax.free_struct(&asn_DEF_RoleSyntax, RoleSyntax, 0);
			BMD_FOK(ERR_AC_DECODE_ROLESYNTAX);
		}
		(*GenBufList)->gbufs[i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset((*GenBufList)->gbufs[i], 0, sizeof(GenBuf_t));

		BMD_FOK_CHG(GeneralName2string(&RoleSyntax->roleName, (char **)&((*GenBufList)->gbufs[i]->buf)),
				ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING);

		(*GenBufList)->gbufs[i]->size = 1+ (int)strlen((char *)(*GenBufList)->gbufs[i]->buf);
		asn_DEF_RoleSyntax.free_struct(&asn_DEF_RoleSyntax, RoleSyntax, 0);
		RoleSyntax = NULL;
	}
	(*GenBufList)->size = Attribute->attrValues.list.count;
	return BMD_OK;
}

long GetAttrClearancePolicyIdFromACStruct(	AttributeCertificate_t *AC,
								char **PolicyId)
{
Attribute_t *Attribute			= NULL;
char 	oid_attribute_clearance[]	= {OID_SYS_METADATA_BMD_SEC_LEVELS};
Clearance_t	*Clearance			= NULL;
asn_dec_rval_t rval;

	if (AC == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (PolicyId == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*PolicyId != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_clearance),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	rval = ber_decode(0, &asn_DEF_Clearance,  (void **)&Clearance,
				Attribute->attrValues.list.array[0]->buf,
				Attribute->attrValues.list.array[0]->size);
	if(rval.code != RC_OK)
	{
		BMD_FOK(ERR_AC_DECODE_CLEARANCE);
	}
	/* Oid polityki bezpieczenstwa */
	BMD_FOK(OBJECT_IDENTIFIER2string(&Clearance->policyId, PolicyId));

	asn_DEF_Clearance.free_struct(&asn_DEF_Clearance, Clearance, 0);
	return BMD_OK;
}

long GetAttrClearanceClearancesFromACStruct(	AttributeCertificate_t *AC,
								GenBufList_t **GenBufList)
{
Attribute_t *Attribute			= NULL;
char 	oid_attribute_clearance[]	= {OID_SYS_METADATA_BMD_SEC_LEVELS};
Clearance_t	*Clearance			= NULL;
asn_dec_rval_t rval;
long i					= 0;

	if (AC == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBufList == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*GenBufList != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_clearance),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*GenBufList == NULL) return NO_MEMORY;
	memset(*GenBufList, 0, sizeof(GenBufList_t));

	/* mnoznik x 3, bo wartosc atrybutu sklada sie z trojki OID polityki, ClassList i SecurityCategory */
	(*GenBufList)->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t *) * Attribute->attrValues.list.count * 3);
	if((*GenBufList)->gbufs == NULL) return NO_MEMORY;
	memset((*GenBufList)->gbufs, 0, sizeof(GenBuf_t *) * Attribute->attrValues.list.count * 3);

	for(i=0; i<Attribute->attrValues.list.count; i++)
	{
		rval = ber_decode(0, &asn_DEF_Clearance,  (void **)&Clearance,
					Attribute->attrValues.list.array[i]->buf,
					Attribute->attrValues.list.array[i]->size);
		if(rval.code != RC_OK)
		{
			asn_DEF_Clearance.free_struct(&asn_DEF_Clearance, Clearance, 0);
			BMD_FOK(ERR_AC_DECODE_CLEARANCE);
		}
		(*GenBufList)->gbufs[3*i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[3*i] == NULL) 	{	BMD_FOK(NO_MEMORY);	}
		memset((*GenBufList)->gbufs[3*i], 0, sizeof(GenBuf_t));

		(*GenBufList)->gbufs[3*i+1] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[3*i+1] == NULL) 	{	BMD_FOK(NO_MEMORY);	}
		memset((*GenBufList)->gbufs[3*i+1], 0, sizeof(GenBuf_t));

		(*GenBufList)->gbufs[3*i+2] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[3*i+2] == NULL) 	{	BMD_FOK(NO_MEMORY);	}
		memset((*GenBufList)->gbufs[3*i+2], 0, sizeof(GenBuf_t));

		if(Clearance->classList == NULL)		{	BMD_FOK(ERR_AC_CLASSLIST_NOT_IN_CLEARANCES);	}
		if(Clearance->securityCategories == NULL)	{	BMD_FOK(ERR_AC_SECURITY_CATEGORIES_NOT_IN_CLEARANCES);	}

		/*Class List*/
		BMD_FOK_CHG(ClassList2string(Clearance->classList,(char **)&((*GenBufList)->gbufs[3*i]->buf)),
				ERR_AC_CONVERT_CLEARANCE_T_TO_STRING);

		(*GenBufList)->gbufs[3*i]->size = 1+ (int)strlen((char *)(*GenBufList)->gbufs[3*i]->buf);

		/* SecurityCategory type */
		BMD_FOK(OBJECT_IDENTIFIER2string(&Clearance->securityCategories->list.array[0]->type,(char **)&((*GenBufList)->gbufs[3*i+1]->buf)));
		(*GenBufList)->gbufs[3*i+1]->size = 1+(int)strlen((char *)(*GenBufList)->gbufs[3*i+1]->buf);

		/* SecurityCategory value */
		BMD_FOK_CHG(Any2UTF8String2String(&Clearance->securityCategories->list.array[0]->value, (char **)&((*GenBufList)->gbufs[3*i+2]->buf)),
				ERR_AC_CONVERT_SECURITYCATEGORY_VALUE_TO_STRING);
		(*GenBufList)->gbufs[3*i+2]->size = 1+(int)strlen((char *)(*GenBufList)->gbufs[3*i+2]->buf);


		asn_DEF_Clearance.free_struct(&asn_DEF_Clearance, Clearance, 0);
		Clearance = NULL;
	}

	(*GenBufList)->size = Attribute->attrValues.list.count * 3;
	return BMD_OK;
}

long GetAttrUnizetoPolicyAuthorityFromACStruct(	AttributeCertificate_t *AC,
								char **PolicyAuthority)
{
Attribute_t *Attribute				= NULL;
char oid_attribute_unizeto[]			= {OID_UNIZETOATTRSYNTAX};
UnizetoAttrSyntax_t *UnizetoAttrSyntax	= NULL;
asn_dec_rval_t rval;

	if (AC == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (PolicyAuthority == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*PolicyAuthority != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_unizeto),
			ERR_AC_GET_ATTRIBUTE_FROM_AC);

	rval = ber_decode(0, &asn_DEF_UnizetoAttrSyntax,  (void **)&UnizetoAttrSyntax,
				Attribute->attrValues.list.array[0]->buf,
				Attribute->attrValues.list.array[0]->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_UnizetoAttrSyntax.free_struct(&asn_DEF_UnizetoAttrSyntax, UnizetoAttrSyntax, 0);
		BMD_FOK(ERR_AC_DECODE_UNIZETOATTRSYNTAX);
	}
	if(UnizetoAttrSyntax->policyAuthority == NULL)
	{
		BMD_FOK(ERR_AC_POLICYAUTHORITY_NOT_IN_UNIZETOATTRSYNTAX);
	}

	BMD_FOK_CHG(GeneralNames2string(UnizetoAttrSyntax->policyAuthority, PolicyAuthority),
			ERR_AC_CONVERT_GENERALNAMES_T_TO_STRING);

	asn_DEF_UnizetoAttrSyntax.free_struct(&asn_DEF_UnizetoAttrSyntax, UnizetoAttrSyntax, 0);
	return BMD_OK;
}


long GetAttrUnizetoUnizetosFromACStruct(	AttributeCertificate_t *AC,
							UnizetoAttrSyntaxInList_t **UAS)
{
long i						= 0;
Attribute_t *Attribute				= NULL;
char oid_attribute_unizeto[]			= {OID_UNIZETOATTRSYNTAX};
UnizetoAttrSyntax_t *UnizetoAttrSyntax	= NULL;
asn_dec_rval_t rval;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (AC == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (UAS == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*UAS != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetAttributeFromACStruct(AC, &Attribute, oid_attribute_unizeto));

	*UAS = (UnizetoAttrSyntaxInList_t *) malloc (sizeof(UnizetoAttrSyntaxInList_t));
	if(*UAS == NULL) 		{	BMD_FOK(NO_MEMORY);	}
	memset(*UAS, 0, sizeof(UnizetoAttrSyntaxInList_t));

	(*UAS)->UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t **) malloc (sizeof(UnizetoAttrSyntaxIn_t *) * Attribute->attrValues.list.count);
	if((*UAS)->UnizetoAttrSyntaxIn == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*UAS)->UnizetoAttrSyntaxIn, 0,sizeof(UnizetoAttrSyntaxIn_t *) * Attribute->attrValues.list.count);

	for(i=0; i<Attribute->attrValues.list.count; i++)
	{
		rval = ber_decode(0, &asn_DEF_UnizetoAttrSyntax,  (void **)&UnizetoAttrSyntax,
					Attribute->attrValues.list.array[i]->buf,
					Attribute->attrValues.list.array[i]->size);

		if(rval.code != RC_OK)
		{
			asn_DEF_UnizetoAttrSyntax.free_struct(&asn_DEF_UnizetoAttrSyntax, UnizetoAttrSyntax, 0);
			BMD_FOK(ERR_AC_DECODE_UNIZETOATTRSYNTAX);
		}
		BMD_FOK_CHG(UnizetoAttrSyntax2UnizetoAttrSyntaxIn(UnizetoAttrSyntax, &((*UAS)->UnizetoAttrSyntaxIn[i])),
				ERR_AC_GET_ATTRIBUTE_FROM_AC);
		asn_DEF_UnizetoAttrSyntax.free_struct(&asn_DEF_UnizetoAttrSyntax, UnizetoAttrSyntax, 0);
		UnizetoAttrSyntax = NULL;
	}
	(*UAS)->size = Attribute->attrValues.list.count;

	return BMD_OK;
}

long UnizetoAttrSyntax2UnizetoAttrSyntaxIn(UnizetoAttrSyntax_t *UnizetoAttrSyntax, UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn)
{
long err = 0;
long i = 0;
	if(UnizetoAttrSyntax == NULL)
		return BMD_ERR_PARAM1;
	if(UnizetoAttrSyntaxIn == NULL || *UnizetoAttrSyntaxIn != NULL)
		return BMD_ERR_PARAM2;

	*UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t *) malloc (sizeof(UnizetoAttrSyntaxIn_t));
	if(*UnizetoAttrSyntaxIn == NULL) return NO_MEMORY;
	memset(*UnizetoAttrSyntaxIn, 0, sizeof(UnizetoAttrSyntaxIn_t));

	(*UnizetoAttrSyntaxIn)->SyntaxOid = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if((*UnizetoAttrSyntaxIn)->SyntaxOid == NULL) return NO_MEMORY;
	memset((*UnizetoAttrSyntaxIn)->SyntaxOid, 0, sizeof(GenBuf_t));

	/* SyntaxOid */
	err = OBJECT_IDENTIFIER2string(&UnizetoAttrSyntax->syntaxId, (char **)&((*UnizetoAttrSyntaxIn)->SyntaxOid->buf));
	if(err<0){
		PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error in converting OBJECT_IDENTIFIER to string. "
			"Recieved error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	(*UnizetoAttrSyntaxIn)->SyntaxOid->size = 1+(int)strlen((char *)(*UnizetoAttrSyntaxIn)->SyntaxOid->buf);

	/* SlotList */
	(*UnizetoAttrSyntaxIn)->SlotList = (SlotListIn_t *) malloc (sizeof(SlotListIn_t));
	if((*UnizetoAttrSyntaxIn)->SlotList == NULL) return NO_MEMORY;
	memset((*UnizetoAttrSyntaxIn)->SlotList, 0, sizeof(SlotListIn_t));

	(*UnizetoAttrSyntaxIn)->SlotList->Slots = (SlotIn_t **) malloc (sizeof(SlotIn_t *)
							* UnizetoAttrSyntax->slots.list.count);
	if((*UnizetoAttrSyntaxIn)->SlotList->Slots == NULL) return NO_MEMORY;
	memset((*UnizetoAttrSyntaxIn)->SlotList->Slots, 0,
		sizeof(SlotIn_t *) * UnizetoAttrSyntax->slots.list.count);

	for(i=0; i<UnizetoAttrSyntax->slots.list.count; i++){
		err = Slot2SlotIn(UnizetoAttrSyntax->slots.list.array[i], &((*UnizetoAttrSyntaxIn)->SlotList->Slots[i]));
		if(err<0){
			PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error in converting Slot_t to SlotIn_t. "
				"Recieved error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
	}
	(*UnizetoAttrSyntaxIn)->SlotList->size = UnizetoAttrSyntax->slots.list.count;
	return err;
}

long Slot2SlotIn(	Slot_t *Slot,
			SlotIn_t **SlotIn)
{
long err = 0;
long i = 0;

	if (Slot == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (SlotIn == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*SlotIn != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	*SlotIn = (SlotIn_t *) malloc (sizeof(SlotIn_t));
	if(*SlotIn == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*SlotIn, 0, sizeof(SlotIn_t));

	(*SlotIn)->OidString = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if((*SlotIn)->OidString == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*SlotIn)->OidString, 0, sizeof(GenBuf_t));

	/* Slot Type */
	BMD_FOK(OBJECT_IDENTIFIER2string(&Slot->slotType, (char **)&((*SlotIn)->OidString->buf)));
	(*SlotIn)->OidString->size = 1+(int)strlen((char *)(*SlotIn)->OidString->buf);

	/* Slot Values */
	(*SlotIn)->ValuesList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if((*SlotIn)->ValuesList == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*SlotIn)->ValuesList, 0, sizeof(GenBufList_t));

	if(Slot->slotValues == NULL)	{	BMD_FOK(err);	}

	(*SlotIn)->ValuesList->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t) * Slot->slotValues->list.count);
	if((*SlotIn)->ValuesList->gbufs == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*SlotIn)->ValuesList->gbufs, 0, sizeof(GenBuf_t) * Slot->slotValues->list.count);

	for(i=0; i<Slot->slotValues->list.count; i++)
	{
		(*SlotIn)->ValuesList->gbufs[i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*SlotIn)->ValuesList->gbufs[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset((*SlotIn)->ValuesList->gbufs[i], 0, sizeof(GenBuf_t));

		if(!strcmp((char *)(*SlotIn)->OidString->buf, OID_UNIZETO_STX_INTEGER))
		{
			BMD_FOK(Any2INTEGER2String(Slot->slotValues->list.array[i], (char **)&((*SlotIn)->ValuesList->gbufs[i]->buf)));
			(*SlotIn)->ValuesList->gbufs[i]->size = 1+(int)strlen((char *)(*SlotIn)->ValuesList->gbufs[i]->buf);
		}
		else if(!strcmp((char *)(*SlotIn)->OidString->buf, OID_UNIZETO_STX_UTF8STRING))
		{
			BMD_FOK(Any2UTF8String2String(Slot->slotValues->list.array[i], (char **)&((*SlotIn)->ValuesList->gbufs[i]->buf)));
			(*SlotIn)->ValuesList->gbufs[i]->size = 1+(int)strlen((char *)(*SlotIn)->ValuesList->gbufs[i]->buf);
		}
		else
		{
			BMD_FOK(ERR_AC_UNIZETO_UDEF_SLOT_STX);
		}
	}
	(*SlotIn)->ValuesList->size = Slot->slotValues->list.count;
	return err;
}


/** Uwaga pod wskaznik Attribute nie jest kopiowana cala zawartosc atrybutu, a jedynie zostaje on
 ustawiony na odpowiedznie miejsce w strukturze AC */
long GetAttributeFromACStruct(	AttributeCertificate_t *AC,
						Attribute_t **Attribute,
						char *attribute_oid)
{
long i	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(AC == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(Attribute == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( *Attribute != NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(attribute_oid == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	for(i=0; i<AC->acinfo.attributes.list.count; i++)
	{

		if(!OBJECT_IDENTIFIER_cmp(&(AC->acinfo.attributes.list.array[i]->attrType), attribute_oid))
		{
			*Attribute = AC->acinfo.attributes.list.array[i];
			return BMD_OK;
		}
	}

	if(i == AC->acinfo.attributes.list.count)
	{
		BMD_FOK(ERR_AC_ATTRIBUTE_NOT_FOUND_IN_AC);
	}

	return BMD_OK;
}

long GenBufList2string(GenBufList_t *GenBufList, char **string)
{
long err = 0;
long i = 0;
char *tmp = NULL;

	if(GenBufList == NULL)
		return BMD_ERR_PARAM1;
	if(string == NULL || *string != NULL)
		return BMD_ERR_PARAM2;

	for(i=0; i<GenBufList->size; i++){
		if(i == 0){
			asprintf(string, "%s", (char *)GenBufList->gbufs[i]->buf);
			if(*string == NULL) return NO_MEMORY;
		}
		else {
			asprintf(&tmp, "%s", *string);
			if(tmp == NULL) return NO_MEMORY;
			if(*string) {free(*string); *string = NULL;}
			asprintf(string, "%s | %s", tmp, (char *)GenBufList->gbufs[i]->buf);
			if(*string == NULL) return NO_MEMORY;
			if(tmp) {free(tmp); tmp = NULL;}
		}
	}
	return err;
}

long UnizetoAttrSyntaxInList2GenBufList(UnizetoAttrSyntaxInList_t *UAS, GenBufList_t **GenBufList, char *UASTypeOID, char *SlotOID)
{
long err = 0;
long i = 0, j = 0;
GenBufList_t *tmpPtr = NULL;

	if(UAS == NULL)
		return BMD_ERR_PARAM1;
	if(GenBufList == NULL || *GenBufList != NULL)
		return BMD_ERR_PARAM2;
	if(UASTypeOID == NULL)
		return BMD_ERR_PARAM3;
	if(SlotOID == NULL)
		return BMD_ERR_PARAM4;

	for(i=0; i<UAS->size; i++){
		if(!strcmp((char *)UAS->UnizetoAttrSyntaxIn[i]->SyntaxOid->buf, UASTypeOID)){
			for(j=0; j<UAS->UnizetoAttrSyntaxIn[i]->SlotList->size; j++){
				if(!strcmp((char *)UAS->UnizetoAttrSyntaxIn[i]->SlotList->Slots[j]->OidString->buf, SlotOID)){
					tmpPtr = UAS->UnizetoAttrSyntaxIn[i]->SlotList->Slots[j]->ValuesList;
					if(tmpPtr == NULL) return err;
					err = GenBufList_clone(tmpPtr, GenBufList);
					if(err < 0){
						PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error in clonning GenBufList. "
								"Recieved error code = %li\n",
								__FILE__, __LINE__, __FUNCTION__, err);
						return err;
					}
				}
			}
		}
	}
	return err;
}

long GenBufList_clone(GenBufList_t *in, GenBufList_t **out)
{
long err = 0;
long i = 0;
	if(in == NULL)
		return BMD_ERR_PARAM1;
	if(out == NULL || *out != NULL)
		return BMD_ERR_PARAM2;


	*out = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*out == NULL) return NO_MEMORY;
	memset(*out, 0, sizeof(GenBufList_t));

	(*out)->size = in->size;
	(*out)->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t *) * in->size);
	if((*out)->gbufs == NULL) return NO_MEMORY;
	memset((*out)->gbufs, 0, sizeof(GenBuf_t *) * in->size);
	for(i=0; i<in->size; i++){
		err = GenBuf_clone(in->gbufs[i], &((*out)->gbufs[i]));
		if(err < 0){
			PRINT_DEBUG("LIBBMDACERR[%s:%i:%s] Error in clonning GenBuf. "
					"Recieved error code = %li\n",
					__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
	}
	return err;
}

long GenBuf_clone(GenBuf_t *in, GenBuf_t **out)
{
long err = 0;
	if(in == NULL)
		return BMD_ERR_PARAM1;
	if(out == NULL || *out != NULL)
		return BMD_ERR_PARAM2;

	*out = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*out == NULL) return NO_MEMORY;
	memset(*out, 0, sizeof(GenBuf_t));

	(*out)->buf = (char *) malloc (sizeof(char) * in->size);
	if((*out)->buf == NULL) return NO_MEMORY;
	memset((*out)->buf, 0, sizeof(char) * in->size);
	memmove((*out)->buf, in->buf, sizeof(char) * in->size);
	(*out)->size = in->size;
	return err;
}
