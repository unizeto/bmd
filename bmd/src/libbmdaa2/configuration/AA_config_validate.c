#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmderr/libbmderr.h>

/**********************************************************
 VALIDATE: Walidacja danych konfiguracyjnych.
**********************************************************/
int ConfigData_validate(ConfigData_t *ConfigData)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(ConfigData == NULL)			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACConfiguration_validate(ConfigData->ACConfiguration));
	BMD_FOK(AAServerConfiguration_validate(ConfigData->AAServerConfiguration));

	return BMD_OK;
}

int ACConfiguration_validate(	ACConfiguration_t *ACConfiguration)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(ACConfiguration == NULL)		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACVersion_validate(ACConfiguration->ACVersionSection));
	BMD_FOK(ACHolder_validate(ACConfiguration->ACHolderSection));
	BMD_FOK(ACIssuer_validate(ACConfiguration->ACIssuerSection));
	BMD_FOK(ACSignature_validate(ACConfiguration->ACSignatureSection));
	BMD_FOK(ACSerialNumber_validate(ACConfiguration->ACSerialNumberSection));
	BMD_FOK(ACAttCertValidityPeriod_validate(ACConfiguration->ACAttCertValidityPeriodSection));
	BMD_FOK(ACAttributes_validate(ACConfiguration->ACAttributesSection));
	BMD_FOK(ACExtensions_validate(ACConfiguration->ACExtensionsSection));

	return BMD_OK;
}

int AAServerConfiguration_validate(AAServerConfiguration_t *AAServerConfiguration)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(AAServerConfiguration == NULL)		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(AADataSource_validate(AAServerConfiguration->DataSourceSection));

	return BMD_OK;
}

int AADataSource_validate(DataSource_t *DataSource)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(DataSource == NULL)					{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
// 	if(DataSource->plaintext_source_enabled < 0) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
// 	if(DataSource->plaintext_filename == NULL) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
// 	if(DataSource->database_source_enabled < 0) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
// 	if(	DataSource->plaintext_source_enabled == AABOOLEAN_NO &&
// 		DataSource->database_source_enabled == AABOOLEAN_NO)	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(DataSource->database_type < 0) 			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(DataSource->database_connection_string == NULL) 		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACDistinguishedName_validate(	const char *dn_prefix,
						ACDistinguishedName_t *ACDistinguishedName)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (dn_prefix == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ACDistinguishedName == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ACDistinguishedName->country == NULL)		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if (ACDistinguishedName->state == NULL)		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if (ACDistinguishedName->organization == NULL)	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if (ACDistinguishedName->organizational_unit == NULL)	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACVersion_validate(ACVersion_t *ACVersion)
{
	if(ACVersion == NULL)					{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACHolder_validate(ACHolder_t *ACHolder)
{
	if(ACHolder == NULL)					{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACHolder->ac_holder_source == NULL) 		{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACIssuer_validate(ACIssuer_t *ACIssuer)
{
	if(ACIssuer == NULL)					{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACIssuer->dnsname == NULL) 			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACIssuer->dn == NULL) 				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACSignature_validate(ACSignature_t *ACSignature)
{
	if(ACSignature == NULL)					{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACSignature->choice_signature_format < 0) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACSignature->privatekey_filename == NULL) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACSignature->privatekey_password == NULL) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACSerialNumber_validate(ACSerialNumber_t *ACSerialNumber)
{
	if(ACSerialNumber == NULL)				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACSerialNumber->serial_number_filename == NULL) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACAttCertValidityPeriod_validate(ACAttCertValidityPeriod_t *ACAttCertValidityPeriod)
{
	if(ACAttCertValidityPeriod == NULL)			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->choice_ac_lifetime < 0) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_years < 0){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_months < 0){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_days < 0)	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_hours < 0){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_minutes < 0){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttCertValidityPeriod->validity_period_seconds < 0){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACAttributes_validate(ACAttributes_t *ACAttributes)
{
	if(ACAttributes == NULL)				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACAttributeRole_validate(ACAttributes->ACAttributeRole));
	BMD_FOK(ACAttributeClearance_validate(ACAttributes->ACAttributeClearance));
	BMD_FOK(ACAttributeGroup_validate(ACAttributes->ACAttributeGroup));
	BMD_FOK(ACAttributeUnizeto_validate(ACAttributes->ACAttributeUnizeto));

	return BMD_OK;
}

int ACAttributeRole_validate(ACAttributeRole_t *ACAttributeRole)
{
	if(ACAttributeRole == NULL)				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeRole->attribute_role_oid == NULL) {	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeRole->role_authority == NULL) 	{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACDistinguishedName_validate("att_role_authority_dn", ACAttributeRole->role_authority));

	return BMD_OK;
}

int ACAttributeClearance_validate(ACAttributeClearance_t *ACAttributeClearance)
{
	if(ACAttributeClearance == NULL)			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeClearance->attribute_clearance_oid == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeClearance->clearance_policy_oid == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

int ACAttributeGroup_validate(ACAttributeGroup_t *ACAttributeGroup)
{
	if(ACAttributeGroup == NULL)				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeGroup->attribute_group_oid == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeGroup->group_policy_authority == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACDistinguishedName_validate("att_group_policy_authority_dn", ACAttributeGroup->group_policy_authority));

	return BMD_OK;
}

int ACAttributeUnizeto_validate(ACAttributeUnizeto_t *ACAttributeUnizeto)
{
	if(ACAttributeUnizeto == NULL)			{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeUnizeto->attribute_unizeto_oid == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}
	if(ACAttributeUnizeto->unizeto_policy_authority == NULL){	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	BMD_FOK(ACDistinguishedName_validate("att_unizeto_policy_authority_dn", ACAttributeUnizeto->unizeto_policy_authority));

	return BMD_OK;
}

int ACExtensions_validate(ACExtensions_t *ACExtensions)
{
	if(ACExtensions != NULL)				{	BMD_FOK(ERR_LIBBMDAA_CONFIG_NOT_VALIDATED);	}

	return BMD_OK;
}

