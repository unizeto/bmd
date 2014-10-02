#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmderr/libbmderr.h>

/**********************************************************
 PRINT: Prezentacja danych konfiguracyjnych.
**********************************************************/
int ConfigData_print(ConfigData_t *ConfigData)
{
int err = 0;
	printf("Attribute Authority Server Configuration summary:\n");
	if(ConfigData == NULL){
		printf("No configuration data present!\n");
	}
	else {
		err = ACConfiguration_print(ConfigData->ACConfiguration);
		if(err<0){
			PRINT_DEBUG("LIBBMDAACONFERR[%s:%i:%s] Error in reading attribute certificate configuration data!.\n"
				"Please check your configuration file.\n",
				__FILE__, __LINE__, __FUNCTION__);
			return err;
		}
		err = AAServerConfiguration_print(ConfigData->AAServerConfiguration);
		if(err<0){
			PRINT_DEBUG("LIBBMDAACONFERR[%s:%i:%s] Error in reading aattribute authority server "
				"configuration data!.\n"
				"Please check your configuration file.\n",
				__FILE__, __LINE__, __FUNCTION__);
			return err;
		}
	}
	return err;
}

int ACConfiguration_print(ACConfiguration_t *ACConfiguration)
{
int err = 0;
	printf("*************************************************\n");
	printf("Attribute certificate configuration summary:\n");
	if(ACConfiguration == NULL){
		printf("Attribute certificate configuration is empty.\n");
	}
	else {
		err = ACVersion_print(ACConfiguration->ACVersionSection);
		if(err<0) return err;
		err = ACHolder_print(ACConfiguration->ACHolderSection);
		if(err<0) return err;
		err = ACIssuer_print(ACConfiguration->ACIssuerSection);
		if(err<0) return err;
		err = ACSignature_print(ACConfiguration->ACSignatureSection);
		if(err<0) return err;
		err = ACSerialNumber_print(ACConfiguration->ACSerialNumberSection);
		if(err<0) return err;
		err = ACAttCertValidityPeriod_print(ACConfiguration->ACAttCertValidityPeriodSection);
		if(err<0) return err;
		err = ACAttributes_print(ACConfiguration->ACAttributesSection);
		if(err<0) return err;
		err = ACExtensions_print(ACConfiguration->ACExtensionsSection);
		if(err<0) return err;
	}
	printf("*************************************************\n");
	return err;
}

int AAServerConfiguration_print(AAServerConfiguration_t *AAServerConfiguration)
{
int err = 0;
	printf("*************************************************\n");
	printf("Attribute Authority Server configuration summary:\n");
	if(AAServerConfiguration == NULL){
		printf("Attribute Authority Server configuration is empty.\n");
	}
	else {
		err = AADataSource_print(AAServerConfiguration->DataSourceSection);
		if(err<0) return err;
	}
	printf("*************************************************\n");
	return err;
}

int AADataSource_print(DataSource_t *DataSource)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Data Source                    |\n");
	printf("----------------------------------\n");
	if(DataSource == NULL){
		printf("Data Source configuration is empty.\n");
	}
	else {
// 		switch(DataSource->plaintext_source_enabled){
// 		case AABOOLEAN_YES:
// 			printf("plaintext_source_enabled = yes\n");
// 			break;
// 		case AABOOLEAN_NO:
// 			printf("plaintext_source_enabled = no\n");
// 			break;
// 		default:
// 			printf("plaintext_source_enabled = <undefined>\n");
// 			break;
// 		}
// 		printf("plaintext_filename = %s\n", (DataSource->plaintext_filename)?
// 							DataSource->plaintext_filename:"<NULL>");
// 		switch(DataSource->database_source_enabled){
// 		case AABOOLEAN_YES:
// 			printf("database_source_enabled = yes\n");
// 			break;
// 		case AABOOLEAN_NO:
// 			printf("database_source_enabled = no\n");
// 			break;
// 		default:
// 			printf("database_source_enabled = <undefined>\n");
// 			break;
// 		}

		switch(DataSource->database_type){
		case AADATABASETYPE_POSTGRESQL:
			printf("database_type = postgresql\n");
			break;
		case AADATABASETYPE_ORACLE:
			printf("database_type = oracle\n");
			break;
		case AADATABASETYPE_DB2:
					printf("database_type = db2\n");
					break;
		default:
			printf("database_type = <undefined>\n");
			break;
		}
		printf("database_connection_string = %s\n", (DataSource->database_connection_string)?
							DataSource->database_connection_string:"<NULL>");
	}
	return err;
}


int ACDistinguishedName_print(const char *dn_prefix, ACDistinguishedName_t *ACDistinguishedName)
{
int err = 0;
	if(dn_prefix == NULL){
		printf("DN prefix = <NULL>\n");
		return BMD_ERR_PARAM1;
	}
	if(ACDistinguishedName == NULL){
		printf("Distinguished name of %s is empty.\n", dn_prefix);
		return err;
	}

	printf("%s_C = %s\n", dn_prefix, (ACDistinguishedName->country)?
				ACDistinguishedName->country:"<NULL>");
	printf("%s_ST = %s\n", dn_prefix, (ACDistinguishedName->state)?
				ACDistinguishedName->state:"<NULL>");
	printf("%s_O = %s\n", dn_prefix, (ACDistinguishedName->organization)?
				ACDistinguishedName->organization:"<NULL>");
	printf("%s_OU = %s\n", dn_prefix, (ACDistinguishedName->organizational_unit)?
				ACDistinguishedName->organizational_unit:"<NULL>");
	return err;
}

int ACVersion_print(ACVersion_t *ACVersion)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Version Section                |\n");
	printf("----------------------------------\n");
	if(ACVersion == NULL){
		printf("Version configuration is empty.\n");
	}
	else {
		printf("ac_version = %li\n", ACVersion->ac_version);
	}
	return err;
}

int ACHolder_print(ACHolder_t *ACHolder)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Holder Section                 |\n");
	printf("----------------------------------\n");
	if(ACHolder == NULL){
		printf("Holder configuration is empty.\n");
	}
	else {
		printf("ac_holder_source = %s\n", (ACHolder->ac_holder_source)?
							ACHolder->ac_holder_source:"<NULL>");
	}
	return err;
}

int ACIssuer_print(ACIssuer_t *ACIssuer)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Issuer Section                 |\n");
	printf("----------------------------------\n");
	if(ACIssuer == NULL){
		printf("Issuer configuration is empty.\n");
	}
	else {
		printf("issuer_dns_name = %s\n", (ACIssuer->dnsname)?ACIssuer->dnsname:"<NULL>");
		err = ACDistinguishedName_print("issuer_dn",
			ACIssuer->dn);
		if(err<0) return err;
	}
	return err;
}
int ACSignature_print(ACSignature_t *ACSignature)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Signature Section              |\n");
	printf("----------------------------------\n");
	if(ACSignature == NULL){
		printf("Signature configuration is empty.\n");
	}
	else {
		switch(ACSignature->choice_signature_format){
		case SIGNATURE_SHAWITHRSA:
			printf("signature_algorithm = SHA1withRSA\n");
			break;
		case SIGNATURE_FAKED:
			printf("signature_algorithm = faked\n");
			break;
		default:
			printf("signature_algorithm = <undefined>\n");
			break;
		}
		printf("privatekey_filename = %s\n", (ACSignature->privatekey_filename)?
							ACSignature->privatekey_filename:"<NULL>");
		printf("privatekey_password = %s\n", (ACSignature->privatekey_password)?
							ACSignature->privatekey_password:"<NULL>");
	}
	return err;
}
int ACSerialNumber_print(ACSerialNumber_t *ACSerialNumber)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Serial Number Section          |\n");
	printf("----------------------------------\n");
	if(ACSerialNumber == NULL){
		printf("Serial Number configuration is empty.\n");
	}
	else {
		printf("serial_number_filename = %s\n", (ACSerialNumber->serial_number_filename)?
							ACSerialNumber->serial_number_filename:"<NULL>");
	}
	return err;
}
int ACAttCertValidityPeriod_print(ACAttCertValidityPeriod_t *ACAttCertValidityPeriod)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Validity Period Section        |\n");
	printf("----------------------------------\n");
	if(ACAttCertValidityPeriod == NULL){
		printf("AC Validity Period configuration is empty.\n");
	}
	else {
		switch(ACAttCertValidityPeriod->choice_ac_lifetime){
		case ACVALIDITY_SESSION:
			printf("ac_lifetime = session\n");
			break;
		case ACVALIDITY_PERIOD:
			printf("ac_lifetime = period\n");
			break;
		default:
			printf("ac_lifetime = <undefined>\n");
		}
		printf("validity_period_years = %li\n", ACAttCertValidityPeriod->validity_period_years);
		printf("validity_period_months = %li\n", ACAttCertValidityPeriod->validity_period_months);
		printf("validity_period_days = %li\n", ACAttCertValidityPeriod->validity_period_days);
		printf("validity_period_hours = %li\n", ACAttCertValidityPeriod->validity_period_hours);
		printf("validity_period_minutes = %li\n", ACAttCertValidityPeriod->validity_period_minutes);
		printf("validity_period_seconds = %li\n", ACAttCertValidityPeriod->validity_period_seconds);
	}
	return err;
}

int ACAttributes_print(ACAttributes_t *ACAttributes)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Attributes Section             |\n");
	printf("----------------------------------\n");
	if(ACAttributes == NULL){
		printf("Attributes configuration is empty.\n");
	}
	else {
		err = ACAttributeRole_print(ACAttributes->ACAttributeRole);
		if(err<0) return err;
		err = ACAttributeClearance_print(ACAttributes->ACAttributeClearance);
		if(err<0) return err;
		err = ACAttributeGroup_print(ACAttributes->ACAttributeGroup);
		if(err<0) return err;
		err = ACAttributeUnizeto_print(ACAttributes->ACAttributeUnizeto);
		if(err<0) return err;
	}
	return err;
}

int ACAttributeRole_print(ACAttributeRole_t *ACAttributeRole)
{
int err = 0;
	if(ACAttributeRole == NULL)
		return BMD_ERR_PARAM1;
	printf("[ Attribute Role                 ]\n");
	if(ACAttributeRole == NULL){
		printf("Attribute Role configuration is empty.\n");
	}
	else {
		printf("att_role_oid = %s\n",
		(ACAttributeRole->attribute_role_oid)?
			ACAttributeRole->attribute_role_oid:"<NULL>");
		err = ACDistinguishedName_print("att_role_authority_dn",
			ACAttributeRole->role_authority);
		if(err<0) return err;
	}
	return err;
}

int ACAttributeClearance_print(ACAttributeClearance_t *ACAttributeClearance)
{
int err = 0;
	if(ACAttributeClearance == NULL)
		return BMD_ERR_PARAM1;
	printf("[ Attribute Clearance            ]\n");
	if(ACAttributeClearance == NULL){
		printf("Attribute Clearance configuration is empty.\n");
	}
	else {
		printf("att_clearance_oid = %s\n",
		(ACAttributeClearance->attribute_clearance_oid)?
			ACAttributeClearance->attribute_clearance_oid:"<NULL>");
		printf("att_clearance_policy_oid = %s\n",
		(ACAttributeClearance->clearance_policy_oid)?
			ACAttributeClearance->clearance_policy_oid:"<NULL>");
	}
	return err;
}

int ACAttributeGroup_print(ACAttributeGroup_t *ACAttributeGroup)
{
int err = 0;
	if(ACAttributeGroup == NULL)
		return BMD_ERR_PARAM1;
	printf("[ Attribute Group                ]\n");
	if(ACAttributeGroup == NULL){
		printf("Attribute Group configuration is empty.\n");
	}
	else {
		printf("att_group_oid = %s\n",
		(ACAttributeGroup->attribute_group_oid)?
			ACAttributeGroup->attribute_group_oid:"<NULL>");
		err = ACDistinguishedName_print("att_group_policy_authority_dn",
			ACAttributeGroup->group_policy_authority);
		if(err<0) return err;
	}
	return err;
}

int ACAttributeUnizeto_print(ACAttributeUnizeto_t *ACAttributeUnizeto)
{
int err = 0;
	if(ACAttributeUnizeto == NULL)
		return BMD_ERR_PARAM1;
	printf("[ Attribute Unizeto              ]\n");
	if(ACAttributeUnizeto == NULL){
		printf("Attribute Unizeto configuration is empty.\n");
	}
	else {
		printf("att_unizeto_oid = %s\n",
		(ACAttributeUnizeto->attribute_unizeto_oid)?
			ACAttributeUnizeto->attribute_unizeto_oid:"<NULL>");
		err = ACDistinguishedName_print("att_unizeto_policy_authority_dn",
			ACAttributeUnizeto->unizeto_policy_authority);
		if(err<0) return err;
	}
	return err;
}

int ACExtensions_print(ACExtensions_t *ACExtensions)
{
int err = 0;
	printf("----------------------------------\n");
	printf("| Extensions Section             |\n");
	printf("----------------------------------\n");
	if(ACExtensions == NULL){
		printf("Extensions configuration is empty.\n");
	}
	else {
		printf("Presentation of the future config data from Extensions section.\n");
	}
	return err;
}

