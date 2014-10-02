#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmderr/libbmderr.h>

/**********************************************************
 FREE: Zwalnianie danych konfiguracyjnych.
**********************************************************/
int ConfigData_free(ConfigData_t **ConfigData)
{
int err = 0;

	if(ConfigData == NULL || *ConfigData == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	ACConfiguration_free(&((*ConfigData)->ACConfiguration));
	AAServerConfiguration_free(&((*ConfigData)->AAServerConfiguration));
	free(*ConfigData);
	*ConfigData = NULL;
	return err;
}

int AAServerConfiguration_free(	AAServerConfiguration_t **AAServerConfiguration)
{
	if (AAServerConfiguration == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*AAServerConfiguration == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	AADataSource_free(&((*AAServerConfiguration)->DataSourceSection));
	free(*AAServerConfiguration);
	*AAServerConfiguration = NULL;
	return BMD_OK;
}
int AADataSource_free(DataSource_t **DataSource)
{
	if (DataSource == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*DataSource == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

// 	AC_free_ptr((void *)&((*DataSource)->plaintext_filename));
	AC_free_ptr((void *)&((*DataSource)->database_connection_string));
	free(*DataSource); *DataSource = NULL;

	return BMD_OK;
}

int ACConfiguration_free(ACConfiguration_t **ACConfiguration)
{
int err = 0;

	if(ACConfiguration == NULL || *ACConfiguration == NULL)
	{
		return BMD_ERR_PARAM1;
	}

	ACVersion_free(&((*ACConfiguration)->ACVersionSection));
	ACHolder_free(&((*ACConfiguration)->ACHolderSection));
	ACIssuer_free(&((*ACConfiguration)->ACIssuerSection));
	ACSignature_free(&((*ACConfiguration)->ACSignatureSection));
	ACSerialNumber_free(&((*ACConfiguration)->ACSerialNumberSection));
	ACAttCertValidityPeriod_free(&((*ACConfiguration)->ACAttCertValidityPeriodSection));
	ACAttributes_free(&((*ACConfiguration)->ACAttributesSection));
	ACExtensions_free(&((*ACConfiguration)->ACExtensionsSection));
	free(*ACConfiguration);
	*ACConfiguration = NULL;
	return err;
}

int AC_free_ptr(void **ptr)
{
int err = 0;

	if(ptr == NULL || *ptr == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	free(*ptr);
	*ptr = NULL;
	return err;
}

int ACDistinguishedName_free(ACDistinguishedName_t **ACDistinguishedName)
{
int err = 0;

	if(ACDistinguishedName == NULL || *ACDistinguishedName == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACDistinguishedName)->country));
	AC_free_ptr((void *)&((*ACDistinguishedName)->state));
	AC_free_ptr((void *)&((*ACDistinguishedName)->organization));
	AC_free_ptr((void *)&((*ACDistinguishedName)->organizational_unit));
	free(*ACDistinguishedName);
	*ACDistinguishedName = NULL;
	return err;
}

int ACVersion_free(ACVersion_t **ACVersion)
{
int err = 0;

	if(ACVersion == NULL || *ACVersion == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	free(*ACVersion);
	*ACVersion = NULL;
	return err;
}

int ACHolder_free(ACHolder_t **ACHolder)
{
int err = 0;
	if(ACHolder == NULL || *ACHolder == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACHolder)->ac_holder_source));
	free(*ACHolder);
	*ACHolder = NULL;
	return err;
}

int ACIssuer_free(ACIssuer_t **ACIssuer)
{
int err = 0;

	if(ACIssuer == NULL || *ACIssuer == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	ACDistinguishedName_free(&((*ACIssuer)->dn));
	AC_free_ptr((void *)&((*ACIssuer)->dnsname));
	AC_free_ptr((void *)&((*ACIssuer)->uniqueidentifier));
	free(*ACIssuer);
	*ACIssuer = NULL;
	return err;
}

int ACSignature_free(ACSignature_t **ACSignature)
{
int err = 0;

	if(ACSignature == NULL || *ACSignature == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACSignature)->privatekey_filename));
	AC_free_ptr((void *)&((*ACSignature)->privatekey_password));
	free(*ACSignature);
	*ACSignature = NULL;
	return err;
}

int ACSerialNumber_free(ACSerialNumber_t **ACSerialNumber)
{
int err = 0;
	if(ACSerialNumber == NULL || *ACSerialNumber == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACSerialNumber)->serial_number_filename));
	free(*ACSerialNumber);
	*ACSerialNumber = NULL;
	return err;
}

int ACAttCertValidityPeriod_free(ACAttCertValidityPeriod_t **ACAttCertValidityPeriod)
{
int err = 0;

	if(ACAttCertValidityPeriod == NULL || *ACAttCertValidityPeriod == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	free(*ACAttCertValidityPeriod);
	*ACAttCertValidityPeriod = NULL;
	return err;
}

int ACAttributes_free(ACAttributes_t **ACAttributes)
{
int err = 0;

	if(ACAttributes == NULL || *ACAttributes == NULL)
	{
		return BMD_ERR_PARAM1;
	}

	ACAttributeRole_free(&((*ACAttributes)->ACAttributeRole));
	ACAttributeClearance_free(&((*ACAttributes)->ACAttributeClearance));
	ACAttributeGroup_free(&((*ACAttributes)->ACAttributeGroup));
	ACAttributeUnizeto_free(&((*ACAttributes)->ACAttributeUnizeto));
	free(*ACAttributes);
	*ACAttributes = NULL;
	return err;
}

int ACAttributeRole_free(ACAttributeRole_t **ACAttributeRole)
{
int err = 0;

	if(ACAttributeRole == NULL || *ACAttributeRole == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACAttributeRole)->attribute_role_oid));
	ACDistinguishedName_free(&((*ACAttributeRole)->role_authority));
	free(*ACAttributeRole);
	*ACAttributeRole = NULL;
	return err;
}

int ACAttributeClearance_free(ACAttributeClearance_t **ACAttributeClearance)
{
int err = 0;

	if(ACAttributeClearance == NULL || *ACAttributeClearance == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACAttributeClearance)->attribute_clearance_oid));
	AC_free_ptr((void *)&((*ACAttributeClearance)->clearance_policy_oid));
	free(*ACAttributeClearance);
	*ACAttributeClearance = NULL;
	return err;
}

int ACAttributeGroup_free(ACAttributeGroup_t **ACAttributeGroup)
{
int err = 0;

	if(ACAttributeGroup == NULL || *ACAttributeGroup == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACAttributeGroup)->attribute_group_oid));
	ACDistinguishedName_free(&((*ACAttributeGroup)->group_policy_authority));
	free(*ACAttributeGroup);
	*ACAttributeGroup = NULL;
	return err;
}

int ACAttributeUnizeto_free(ACAttributeUnizeto_t **ACAttributeUnizeto)
{
int err = 0;

	if(ACAttributeUnizeto == NULL || *ACAttributeUnizeto == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	AC_free_ptr((void *)&((*ACAttributeUnizeto)->attribute_unizeto_oid));
	ACDistinguishedName_free(&((*ACAttributeUnizeto)->unizeto_policy_authority));
	free(*ACAttributeUnizeto);
	*ACAttributeUnizeto = NULL;
	return err;
}

int ACExtensions_free(ACExtensions_t **ACExtensions)
{
int err = 0;

	if(ACExtensions == NULL || *ACExtensions == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	free(*ACExtensions);
	*ACExtensions = NULL;
	return err;
}
