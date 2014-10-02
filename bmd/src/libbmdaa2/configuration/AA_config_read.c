#include "uniac_cnf_sections.h"
#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmderr/libbmderr.h>

/**********************************************************
 READ: Odczyt danych konfiguracyjnych.
**********************************************************/
int ConfigData_read(const char *filename, ConfigData_t **ConfigData)
{
int err = 0;
bmd_conf *configuration	= NULL;
char *errorString=NULL;

	PRINT_VDEBUG("LIBBMDAA2INF Reading configuration from file\n");
	if(filename == NULL)
	{
		PRINT_DEBUG("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ConfigData == NULL || *ConfigData != NULL)
	{
		PRINT_VDEBUG("LIBBMDAA2INF Invalid second parameter value\n");
		/*return BMD_ERR_PARAM2;*/
	}

	*ConfigData = (ConfigData_t *) malloc (sizeof(ConfigData_t));
	if(*ConfigData == NULL)
	{
		PRINT_DEBUG("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}

	memset(*ConfigData, 0, sizeof(ConfigData_t));

	/* Zainicjalizuj konfiguracje */
	err = bmdconf_init(&configuration);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAA2ERR Error in bmdconf_init. Error=%i\n",err);
		return ERR_LIBBMDAA_CONFIG_INIT_FAILED;
	}
	/* Wczytaj slowniki */
	err = bmdconf_add_section2dict(configuration, "version", _GLOBAL_dict_uniac_version, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "version", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "holder", _GLOBAL_dict_uniac_holder, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n","holder", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "issuer", _GLOBAL_dict_uniac_issuer, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "issuer", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "signature", _GLOBAL_dict_uniac_signature, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n","signature", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "serial_number", _GLOBAL_dict_uniac_serialnumber, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error= %i\n","serial_number", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "ac_validity_period", _GLOBAL_dict_uniac_acvalidityperiod, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "ac_validity_period", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "attribute_role", _GLOBAL_dict_uniac_attributerole, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "attribute_role", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "attribute_group", _GLOBAL_dict_uniac_attributegroup, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "attribute_group", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "attribute_clearance", _GLOBAL_dict_uniac_attributeclearance, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "attribute_clearance", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "attribute_unizeto", _GLOBAL_dict_uniac_attributeunizeto, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "attribute_unizeto", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "extensions", _GLOBAL_dict_uniac_extensions, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "extensions", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "data_source", _GLOBAL_dict_uniac_datasource, BMDCONF_SECTION_REQ);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in adding dictionary for section [%s]. Error=%i\n", "data_source", err);
		return ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED;
	}

	/* Wczytaj plik konfiguracyjny */
	err = bmdconf_load_file(filename, configuration, &errorString);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAACONFERR Cannot load configuration file %s! Error=%i\n%s\n",filename, err, errorString);
		free(errorString); errorString=NULL;
		return ERR_LIBBMDAA_CONFIG_LOAD_CONFIGFILE_FAILED;
	}
	/* Tu nastepuje wczytanie danych konfiguracyjnych do wlasciwych struktur danych */
	err = ACConfiguration_read(configuration, &((*ConfigData)->ACConfiguration));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Cannot read ACConfiguration! Error=%i\n",err);
		return ERR_LIBBMDAA_CONFIG_READ_ACCONFIGURATION_FAILED;
	}
	/* Wczytanie opcji konfiguracyjnych serwera AA */
	err = AAServerConfiguration_read(configuration, &((*ConfigData)->AAServerConfiguration));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Cannot read AAServerConfiguration! Error=%i\n",err);
		return ERR_LIBBMDAA_CONFIG_READ_AASERVERCONFIGURATION_FAILED;
	}
	/* Zniszcz konfiguracje */
	err=bmdconf_destroy(&configuration);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in destroying configuration dictionary. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_DESTROY_FAILED;
	}
	PRINT_VDEBUG("LIBBMDAACONFINF [done]\n");
	return err;
}

int ACConfiguration_read(bmd_conf *configuration, ACConfiguration_t **ACConfiguration)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading aclient configuration\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACConfiguration == NULL || *ACConfiguration != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACConfiguration = (ACConfiguration_t *) malloc (sizeof(ACConfiguration_t));
	if(*ACConfiguration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACConfiguration, 0, sizeof(ACConfiguration_t));

	err = ACVersion_read(configuration, &((*ACConfiguration)->ACVersionSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [version] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_VERSION;
	}
	err = ACHolder_read(configuration, &((*ACConfiguration)->ACHolderSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [holder] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_HOLDER;
	}
	err = ACIssuer_read(configuration, &((*ACConfiguration)->ACIssuerSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [issuer] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_ISSUER;
	}
	err = ACSignature_read(configuration, &((*ACConfiguration)->ACSignatureSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [signature] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_SIGNATURE;
	}
	err = ACSerialNumber_read(configuration, &((*ACConfiguration)->ACSerialNumberSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [serial number] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_SERIALNUMBER;
	}
	err = ACAttCertValidityPeriod_read(configuration, &((*ACConfiguration)->ACAttCertValidityPeriodSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading [ac validity period] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_ACVALIDITYPERIOD;
	}
	err = ACAttributes_read(configuration, &((*ACConfiguration)->ACAttributesSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [attribute *] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_ATTRIBUTES;
	}
	err = ACExtensions_read(configuration, &((*ACConfiguration)->ACExtensionsSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [extensions] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_EXTENSIONS;
	}
	return err;
}

int AAServerConfiguration_read(bmd_conf *configuration, AAServerConfiguration_t **AAServerConfiguration)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading aserver configuration\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(AAServerConfiguration == NULL || *AAServerConfiguration != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}


	*AAServerConfiguration = (AAServerConfiguration_t *) malloc (sizeof(AAServerConfiguration_t));
	if(*AAServerConfiguration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*AAServerConfiguration, 0, sizeof(AAServerConfiguration_t));

	/* Na razie opcje konfiguracyjne serwera ograniczają się do określenia źródła pochodzenia danych */
	err = AADataSource_read(configuration, &((*AAServerConfiguration)->DataSourceSection));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [data_source] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_DATASOURCE;
	}
	return err;
}

int AADataSource_read(	bmd_conf *configuration,
				DataSource_t **DataSource)
{
// char *AAPlainTextSourceEnableString = NULL;
// char *AADatabaseSourceEnableString 	= NULL;
char *AADatabaseType			= NULL;


	PRINT_VDEBUG("LIBBMDAACONFINF Reading data source\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (configuration == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (DataSource == NULL )		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*DataSource != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	*DataSource = (DataSource_t *) malloc (sizeof(DataSource_t));
	memset(*DataSource, 0, sizeof(DataSource_t));

// 	BMD_FOK(bmdconf_get_value(configuration,"data_source","plaintext_source_enabled", &AAPlainTextSourceEnableString));

// 	if(!strcmp(AAPlainTextSourceEnableString, BOOLEAN_YES_STR))
// 	{
// 		(*DataSource)->plaintext_source_enabled = AABOOLEAN_YES;
// 	}
// 	else
// 	{
// 		(*DataSource)->plaintext_source_enabled = AABOOLEAN_NO;
// 	}

// 	BMD_FOK(bmdconf_get_value(configuration,"data_source","plaintext_filename", &((*DataSource)->plaintext_filename)));
// 	BMD_FOK(bmdconf_get_value(configuration,"data_source","database_source_enabled", &AADatabaseSourceEnableString));

// 	if(!strcmp(AADatabaseSourceEnableString, BOOLEAN_YES_STR))
// 	{
// 		(*DataSource)->database_source_enabled = AABOOLEAN_YES;
// 	}
// 	else
// 	{
// 		(*DataSource)->database_source_enabled = AABOOLEAN_NO;
// 	}

	BMD_FOK(bmdconf_get_value(configuration,"data_source","database_type", &AADatabaseType));

	if(!strcmp(AADatabaseType, DATA_SOURCE_DATABASETYPE_POSTGRESQL))
	{
		(*DataSource)->database_type = AADATABASETYPE_POSTGRESQL;
	}
	else if(!strcmp(AADatabaseType, DATA_SOURCE_DATABASETYPE_ORACLE))
	{
		(*DataSource)->database_type = AADATABASETYPE_ORACLE;
	}
	else if(!strcmp(AADatabaseType, DATA_SOURCE_DATABASETYPE_DB2))
	{
		(*DataSource)->database_type = AADATABASETYPE_DB2;
	}
	//ustawiamy db2
	else
	{
		(*DataSource)->database_type = AADATABASETYPE_DB2;
	}


	//else
	//{
	//	(*DataSource)->database_type = AADATABASETYPE_POSTGRESQL;
	//}

	BMD_FOK(bmdconf_get_value(configuration,"data_source","database_connection_string",	&((*DataSource)->database_connection_string)));


// 	free(AAPlainTextSourceEnableString); AAPlainTextSourceEnableString = NULL;
// 	free(AADatabaseSourceEnableString); AADatabaseSourceEnableString = NULL;
	free(AADatabaseType); AADatabaseType = NULL;
	return BMD_OK;
}


int ACDistinguishedName_read(	bmd_conf *configuration,
					const char *section, const char *dn_prefix,
					ACDistinguishedName_t **ACDistinguishedName)
{
char *DN_part		= NULL;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading distinguished name\n");
	if (configuration == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (section == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (dn_prefix == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (ACDistinguishedName == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*ACDistinguishedName != NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	*ACDistinguishedName = (ACDistinguishedName_t *) malloc (sizeof(ACDistinguishedName_t));
	if(*ACDistinguishedName == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*ACDistinguishedName, 0, sizeof(ACDistinguishedName_t));

	/* PObierz Country */
	asprintf(&DN_part, "%s_C", dn_prefix);
	if(DN_part == NULL)			{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmdconf_get_value(configuration, section, DN_part, &((*ACDistinguishedName)->country)),
			ERR_LIBBMDAA_CONFIG_DN_COUNRY_NOT_PRESENT);

	free(DN_part); DN_part=NULL;

	/* PObierz State */
	asprintf(&DN_part, "%s_ST", dn_prefix);
	if(DN_part == NULL) 	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmdconf_get_value(configuration, section, DN_part, &((*ACDistinguishedName)->state)),
			ERR_LIBBMDAA_CONFIG_DN_STATE_NOT_PRESENT);
	free(DN_part); DN_part=NULL;

	/* Pobierz Organization */
	asprintf(&DN_part, "%s_O", dn_prefix);
	if(DN_part == NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmdconf_get_value(configuration, section, DN_part,&((*ACDistinguishedName)->organization)),
			ERR_LIBBMDAA_CONFIG_DN_ORGANIZATION_NOT_PRESENT);
	free(DN_part); DN_part=NULL;

	/* Pobierz Organizational Unit */
	asprintf(&DN_part, "%s_OU", dn_prefix);
	if(DN_part == NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmdconf_get_value(configuration, section, DN_part,&((*ACDistinguishedName)->organizational_unit)),
			ERR_LIBBMDAA_CONFIG_DN_ORGANIZATIONALUNIT_NOT_PRESENT);

	free(DN_part); DN_part=NULL;

	return BMD_OK;
}

int ACVersion_read(bmd_conf *configuration, ACVersion_t **ACVersion)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading version\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACVersion == NULL || *ACVersion != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACVersion = (ACVersion_t *) malloc (sizeof(ACVersion_t));
	if(*ACVersion == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACVersion, 0, sizeof(ACVersion_t));

	err=bmdconf_get_value_int(configuration,"version","ac_version",&((*ACVersion)->ac_version));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading ac_version! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVERSION_FAILED;
	}
	return err;
}

int ACHolder_read(bmd_conf *configuration, ACHolder_t **ACHolder)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading holder\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACHolder == NULL || *ACHolder != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACHolder = (ACHolder_t *) malloc(sizeof(ACHolder_t));
	if(*ACHolder == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACHolder, 0, sizeof(ACHolder_t));

	err=bmdconf_get_value(configuration,"holder","ac_holder_source",&((*ACHolder)->ac_holder_source));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading holder source point! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_HOLDER_SOURCE_POINT_FAILED;
	}
	return err;
}

int ACIssuer_read(bmd_conf *configuration, ACIssuer_t **ACIssuer)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading issuer\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACIssuer == NULL || *ACIssuer != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACIssuer = (ACIssuer_t *) malloc (sizeof(ACIssuer_t));
	if(*ACIssuer == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACIssuer, 0, sizeof(ACIssuer_t));

	/* Na razie nie odczytuje z pliku formatu nazy wystawcy AC */
	(*ACIssuer)->choice_generalname_format = GENERAL_NAME_DNSNAME;

	err=bmdconf_get_value(configuration,"issuer","issuer_dns_name",&((*ACIssuer)->dnsname));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading issuer dns_name! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ISSUER_DNSNAME_FAILED;
	}
	err = ACDistinguishedName_read(configuration, "issuer", "issuer_dn",&((*ACIssuer)->dn));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading issuer dn! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ISSUER_DN_FAILED;
	}
	return err;
}

int ACSignature_read(bmd_conf *configuration, ACSignature_t **ACSignature)
{
int err = 0;
char *ACSignatureAlgorithm = NULL;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading issuer\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACSignature == NULL || *ACSignature != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = bmdconf_get_value(configuration,"signature","signature_algorithm", &ACSignatureAlgorithm);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading signature_algorithm! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_ALGORITHM_FAILED;
	}

	*ACSignature = (ACSignature_t *) malloc (sizeof(ACSignature_t));
	if(*ACSignature == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACSignature, 0, sizeof(ACSignature_t));

	if(!strcmp(ACSignatureAlgorithm, SIGNATURE_SHA1_WITH_RSA_STR))
	{
		(*ACSignature)->choice_signature_format = SIGNATURE_SHAWITHRSA;
	}
	else if(!strcmp(ACSignatureAlgorithm, SIGNATURE_FAKED_STR))
	{
		(*ACSignature)->choice_signature_format = SIGNATURE_FAKED;
	}
	else
	{
		(*ACSignature)->choice_signature_format = SIGNATURE_SHAWITHRSA;
	}
	err=bmdconf_get_value(configuration,"signature","privatekey_filename",&((*ACSignature)->privatekey_filename));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading signature privatekey_filename! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_PRIVATEKEY_FILENAME_FAILED;
	}
	err=bmdconf_get_value(configuration,"signature","privatekey_password",&((*ACSignature)->privatekey_password));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading signature privatekey_password! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_PRIVATEKEY_PASSWORD_FAILED;
	}
	if(ACSignatureAlgorithm) {free(ACSignatureAlgorithm); ACSignatureAlgorithm = NULL;}
	return err;
}

int ACSerialNumber_read(bmd_conf *configuration, ACSerialNumber_t **ACSerialNumber)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading serial number\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACSerialNumber == NULL || *ACSerialNumber != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACSerialNumber = (ACSerialNumber_t *) malloc(sizeof(ACSerialNumber_t));
	if(*ACSerialNumber == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACSerialNumber, 0, sizeof(ACSerialNumber_t));

	err=bmdconf_get_value(configuration,"serial_number","serial_number_filename",&((*ACSerialNumber)->serial_number_filename));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading serial number filename! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_SERIALNUMBER_FILENAME_FAILED;
	}
	return err;
}

int ACAttCertValidityPeriod_read(bmd_conf *configuration, ACAttCertValidityPeriod_t **ACAttCertValidityPeriod)
{
int err = 0;
char *ACSignatureAlgorithm = NULL;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading validity period of certificate\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttCertValidityPeriod == NULL || *ACAttCertValidityPeriod != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttCertValidityPeriod = (ACAttCertValidityPeriod_t *) malloc (sizeof(ACAttCertValidityPeriod_t));
	if(*ACAttCertValidityPeriod == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttCertValidityPeriod, 0, sizeof(ACAttCertValidityPeriod_t));

	err = bmdconf_get_value(configuration,"ac_validity_period","ac_lifetime", &ACSignatureAlgorithm);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute certificate lifetime period! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_LIFETIME_PERIOD_FAILED;
	}
	if(!strcmp(ACSignatureAlgorithm, VALIDITY_SESSION_STR))
	{
		(*ACAttCertValidityPeriod)->choice_ac_lifetime = ACVALIDITY_SESSION;
	}
	else if(!strcmp(ACSignatureAlgorithm, VALIDITY_PERIOD_STR))
	{
		(*ACAttCertValidityPeriod)->choice_ac_lifetime = ACVALIDITY_PERIOD;
	}
	else
	{
		(*ACAttCertValidityPeriod)->choice_ac_lifetime = ACVALIDITY_SESSION;
	}

	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_years",&((*ACAttCertValidityPeriod)->validity_period_years));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_years! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_months",&((*ACAttCertValidityPeriod)->validity_period_months));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_months! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_days",&((*ACAttCertValidityPeriod)->validity_period_days));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_days! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_hours",&((*ACAttCertValidityPeriod)->validity_period_hours));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_hours! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_minutes",
				&((*ACAttCertValidityPeriod)->validity_period_minutes));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_minutes! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	err=bmdconf_get_value_int(configuration,"ac_validity_period","validity_period_seconds",&((*ACAttCertValidityPeriod)->validity_period_seconds));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading validity_period_seconds! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED;
	}
	if(ACSignatureAlgorithm) {free(ACSignatureAlgorithm); ACSignatureAlgorithm = NULL;}
	return err;
}

int ACAttributes_read(bmd_conf *configuration, ACAttributes_t **ACAttributes)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading validity period of certificate\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttributes == NULL || *ACAttributes != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttributes = (ACAttributes_t *) malloc (sizeof(ACAttributes_t));
	if(*ACAttributes == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttributes, 0, sizeof(ACAttributes_t));

	err = ACAttributeRole_read(configuration,	&((*ACAttributes)->ACAttributeRole));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [attribute_role] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_ROLE;
	}
	err = ACAttributeClearance_read(configuration,	&((*ACAttributes)->ACAttributeClearance));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [attribute_clearance] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_CLEARANCE;
	}
	err = ACAttributeGroup_read(configuration,	&((*ACAttributes)->ACAttributeGroup));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [attribute_group] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_GROUP;
	}
	err = ACAttributeUnizeto_read(configuration,	&((*ACAttributes)->ACAttributeUnizeto));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading one of the [attribute_unizeto] section. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_SECTION_UNIZETO;
	}
	return err;
}

int ACAttributeRole_read(bmd_conf *configuration, ACAttributeRole_t **ACAttributeRole)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading role attribute\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttributeRole == NULL || *ACAttributeRole != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttributeRole = (ACAttributeRole_t *) malloc (sizeof(ACAttributeRole_t));
	if(*ACAttributeRole == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttributeRole, 0, sizeof(ACAttributeRole_t));

	err=bmdconf_get_value(configuration,"attribute_role","att_role_oid",&((*ACAttributeRole)->attribute_role_oid));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute role OID! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTROLE_ATTRIBUTEOID;
	}
	err = ACDistinguishedName_read(configuration, "attribute_role", "att_role_authority_dn",&((*ACAttributeRole)->role_authority));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute role authority distinguished name! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTROLE_ATTRIBUTEROLEAUTHDN;
	}
	return err;
}

int ACAttributeClearance_read(bmd_conf *configuration, ACAttributeClearance_t **ACAttributeClearance)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading clearance attribute\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttributeClearance == NULL || *ACAttributeClearance != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttributeClearance = (ACAttributeClearance_t *) malloc (sizeof(ACAttributeClearance_t));
	if(*ACAttributeClearance == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttributeClearance, 0, sizeof(ACAttributeClearance_t));

	err=bmdconf_get_value(configuration, "attribute_clearance", "att_clearance_oid",&((*ACAttributeClearance)->attribute_clearance_oid));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute clearance OID! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTCLEARANCE_ATTRIBUTEOID;
	}
	err=bmdconf_get_value(configuration, "attribute_clearance", "att_clearance_policy_oid",&((*ACAttributeClearance)->clearance_policy_oid));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute clearance policy OID! Error= %i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTCLEARANCE_POLICYOID;
	}
	return err;
}

int ACAttributeGroup_read(bmd_conf *configuration, ACAttributeGroup_t **ACAttributeGroup)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading group attribute\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttributeGroup == NULL || *ACAttributeGroup != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttributeGroup = (ACAttributeGroup_t *) malloc (sizeof(ACAttributeGroup_t));
	if(*ACAttributeGroup == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttributeGroup, 0, sizeof(ACAttributeGroup_t));

	err=bmdconf_get_value(configuration,"attribute_group","att_group_oid",&((*ACAttributeGroup)->attribute_group_oid));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute group OID! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTGROUP_ATTRIBUTEOID;
	}
	err = ACDistinguishedName_read(configuration, "attribute_group", "att_group_policy_authority_dn",&((*ACAttributeGroup)->group_policy_authority));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute group policy Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTGROUP_ATTRIBUTEGROUPPOLICYAUTHDN;
	}
	return err;
}
int ACAttributeUnizeto_read(bmd_conf *configuration, ACAttributeUnizeto_t **ACAttributeUnizeto)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading unizeto attribute\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACAttributeUnizeto == NULL || *ACAttributeUnizeto != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*ACAttributeUnizeto = (ACAttributeUnizeto_t *) malloc (sizeof(ACAttributeUnizeto_t));
	if(*ACAttributeUnizeto == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Memory error. Error=%i\n", NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*ACAttributeUnizeto, 0, sizeof(ACAttributeUnizeto_t));

	err=bmdconf_get_value(configuration, "attribute_unizeto", "att_unizeto_oid",&((*ACAttributeUnizeto)->attribute_unizeto_oid));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute Unizeto OID! Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTUNIZETO_ATTRIBUTEOID;
	}
	err = ACDistinguishedName_read(configuration, "attribute_unizeto", "att_unizeto_policy_authority_dn",&((*ACAttributeUnizeto)->unizeto_policy_authority));
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Error in reading attribute unizeto policy authority dn. Error=%i\n", err);
		return ERR_LIBBMDAA_CONFIG_READ_ATTUNIZETO_ATTRIBUTEUNIZETOPOLICYAUTHDN;
	}
	return err;
}

int ACExtensions_read(bmd_conf *configuration, ACExtensions_t **ACExtensions)
{
int err = 0;

	PRINT_VDEBUG("LIBBMDAACONFINF Reading extensions\n");
	if(configuration == NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid first parameter value. Error=%i\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ACExtensions == NULL || *ACExtensions != NULL)
	{
		PRINT_DEBUG("LIBBMDAACONFERR Invalid second parameter value. Error=%i\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* Funkcja obecnie nie wykonuje zadnych operacji */
	/* Jesli dodane zostana jakies pola w sekcji [extensions] to ich odczyt
	nalezy umiescic tu */
	return err;
}
