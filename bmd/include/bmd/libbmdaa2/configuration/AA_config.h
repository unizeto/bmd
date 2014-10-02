#ifndef _AA_CONFIG_READER_INCLUDED_
#define _AA_CONFIG_READER_INCLUDED_

#include <bmd/libbmdconfig/libbmdconfig.h>

#define ERR_LIBBMDAA_CONFIG_INIT_FAILED						-501
#define ERR_LIBBMDAA_CONFIG_SECTION2DICT_FAILED					-502
#define ERR_LIBBMDAA_CONFIG_LOAD_CONFIGFILE_FAILED				-503
#define ERR_LIBBMDAA_CONFIG_READ_ACCONFIGURATION_FAILED				-504
#define ERR_LIBBMDAA_CONFIG_DESTROY_FAILED					-505
#define ERR_LIBBMDAA_CONFIG_SECTION_VERSION					-506
#define ERR_LIBBMDAA_CONFIG_SECTION_HOLDER					-507
#define ERR_LIBBMDAA_CONFIG_SECTION_ISSUER					-508
#define ERR_LIBBMDAA_CONFIG_SECTION_SIGNATURE					-509
#define ERR_LIBBMDAA_CONFIG_SECTION_SERIALNUMBER				-510
#define ERR_LIBBMDAA_CONFIG_SECTION_ACVALIDITYPERIOD				-511
#define ERR_LIBBMDAA_CONFIG_SECTION_ATTRIBUTES					-512
#define ERR_LIBBMDAA_CONFIG_SECTION_EXTENSIONS					-513
#define ERR_LIBBMDAA_CONFIG_NOT_VALIDATED					-514
#define ERR_LIBBMDAA_CONFIG_READ_ACVERSION_FAILED				-515
#define ERR_LIBBMDAA_CONFIG_READ_ISSUER_DNSNAME_FAILED				-516
#define ERR_LIBBMDAA_CONFIG_SECTION_ROLE					-517
#define ERR_LIBBMDAA_CONFIG_SECTION_CLEARANCE					-518
#define ERR_LIBBMDAA_CONFIG_SECTION_GROUP					-519
#define ERR_LIBBMDAA_CONFIG_SECTION_UNIZETO					-520
#define ERR_LIBBMDAA_CONFIG_READ_ATTROLE_ATTRIBUTEOID				-521
#define ERR_LIBBMDAA_CONFIG_DN_COUNRY_NOT_PRESENT				-522
#define ERR_LIBBMDAA_CONFIG_DN_STATE_NOT_PRESENT				-523
#define ERR_LIBBMDAA_CONFIG_DN_ORGANIZATION_NOT_PRESENT				-524
#define ERR_LIBBMDAA_CONFIG_DN_ORGANIZATIONALUNIT_NOT_PRESENT			-525
#define ERR_LIBBMDAA_CONFIG_READ_ATTROLE_ATTRIBUTEROLEAUTHDN			-526
#define ERR_LIBBMDAA_CONFIG_READ_ATTGROUP_ATTRIBUTEOID				-527
#define ERR_LIBBMDAA_CONFIG_READ_ATTGROUP_ATTRIBUTEGROUPPOLICYAUTHDN 		-528
#define ERR_LIBBMDAA_CONFIG_READ_ATTCLEARANCE_ATTRIBUTEOID			-529
#define ERR_LIBBMDAA_CONFIG_READ_ATTCLEARANCE_POLICYOID				-530
#define ERR_LIBBMDAA_CONFIG_READ_ATTUNIZETO_ATTRIBUTEOID			-531
#define ERR_LIBBMDAA_CONFIG_READ_ATTUNIZETO_ATTRIBUTEUNIZETOPOLICYAUTHDN	-532
#define ERR_LIBBMDAA_CONFIG_READ_HOLDER_SOURCE_POINT_FAILED			-533
#define ERR_LIBBMDAA_CONFIG_READ_ISSUER_DN_FAILED				-534
#define ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_ALGORITHM_FAILED			-535
#define ERR_LIBBMDAA_CONFIG_READ_LIFETIME_PERIOD_FAILED				-536
#define ERR_LIBBMDAA_CONFIG_READ_ACVALIDITYPERIOD_FAILED			-537
#define ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_PRIVATEKEY_FILENAME_FAILED		-538
#define ERR_LIBBMDAA_CONFIG_READ_SIGNATURE_PRIVATEKEY_PASSWORD_FAILED		-539
#define ERR_LIBBMDAA_CONFIG_READ_AASERVERCONFIGURATION_FAILED			-540
#define ERR_LIBBMDAA_CONFIG_SECTION_DATASOURCE					-541
#define ERR_LIBBMDAA_CONFIG_READ_DATASOURCE_PLAINTEXTSOURCEENABLE_FAILED	-542
#define ERR_LIBBMDAA_CONFIG_READ_DATASOURCE_PLAINTEXTFILENAME_FAILED		-543
#define ERR_LIBBMDAA_CONFIG_READ_DATASOURCE_DATABASETYPE_FAILED			-544
#define ERR_LIBBMDAA_CONFIG_READ_DATASOURCE_DATABASECONNECTIONSTRING_FAILED	-545
#define ERR_LIBBMDAA_CONFIG_READ_SERIALNUMBER_FILENAME_FAILED			-546
#define ERR_LIBBMDAA_CONFIG_READ_DATASOURCE_DATABASESOURCEENABLE_FAILED		-547


#define CONFIG_VERSION_2_0_0_STR	"2.0.0"
#define CONFIG_VERSION_2_0_1_STR	"2.0.1"
#define CONFIG_VERSION_2_0_2_STR	"2.0.2"
#define CONFIG_VERSION_2_0_3_STR	"2.0.3"

#define HOLDER_PRESENT_IN_PKC_STR	"PKC"
#define SIGNATURE_SHA1_WITH_RSA_STR	"SHA1withRSA"
#define SIGNATURE_FAKED_STR		"faked"
#define VALIDITY_SESSION_STR		"session"
#define VALIDITY_PERIOD_STR		"period"

#define BOOLEAN_YES_STR			"yes"
#define BOOLEAN_NO_STR			"no"

#define DATA_SOURCE_DATABASETYPE_POSTGRESQL	"postgresql"
#define DATA_SOURCE_DATABASETYPE_DB2	"db2"
#define DATA_SOURCE_DATABASETYPE_ORACLE		"oracle"

/* Struktury pomocnicze */
typedef struct ACDistinguishedName {
	char *country;
	char *state;
	char *organization;
	char *organizational_unit;
	/* itp */
	} ACDistinguishedName_t;

typedef enum ACGeneralNameEnum {
	GENERAL_NAME_UNDEFINED,
	GENERAL_NAME_DIRECTORYNAME,
	GENERAL_NAME_DNSNAME
	} ACGeneralNameEnum_t;

typedef enum ACSignatureEnum {
	SIGNATURE_UNDEFINED,
	SIGNATURE_SHAWITHRSA,
	SIGNATURE_FAKED,
	} ACSignatureEnum_t;

typedef enum ACValidityLifetimeEnum {
	ACVALIDITY_UNDEFINED,
	ACVALIDITY_SESSION,
	ACVALIDITY_PERIOD
	} ACValidityLifetimeEnum_t;

typedef enum AABooleanEnum {
	AABOOLEAN_UNDEFINED,
	AABOOLEAN_NO,
	AABOOLEAN_YES
	} AABooleanEnum_t;

typedef enum AADatabaseTypeEnum {
	AADATABASETYPE_UNDEFINED,
	AADATABASETYPE_POSTGRESQL,
	AADATABASETYPE_ORACLE,
	AADATABASETYPE_DB2
	} AADatabaseTypeEnum_t;

/* Pola certyfikatu atrybutów */
typedef struct ACVersion {
		long ac_version;
	} ACVersion_t;

typedef struct ACHolder {
		char *ac_holder_source;
	} ACHolder_t;

typedef struct ACIssuer {
	ACGeneralNameEnum_t choice_generalname_format;		/* Name or dNSName */
	ACDistinguishedName_t *dn;
	char *dnsname;
	char *uniqueidentifier;
	} ACIssuer_t;

typedef struct ACSignature {
	ACSignatureEnum_t choice_signature_format;		/* only SHA with RSA implemented now */
	char *privatekey_filename;
	char *privatekey_password;
	} ACSignature_t;

typedef struct ACSerialNumber {
		char *serial_number_filename;
	} ACSerialNumber_t;

typedef struct ACAttCertValidityPeriod {
	ACValidityLifetimeEnum_t choice_ac_lifetime;
	long validity_period_years;
	long validity_period_months;
	long validity_period_days;
	long validity_period_hours;
	long validity_period_minutes;
	long validity_period_seconds;
	} ACAttCertValidityPeriod_t;

typedef struct ACAttributeRole {
	char *attribute_role_oid;
	ACDistinguishedName_t *role_authority;
	} ACAttributeRole_t;

typedef struct ACAttributeClearance {
	char *attribute_clearance_oid;
	char *clearance_policy_oid;
	} ACAttributeClearance_t;

typedef struct ACAttributeGroup {
	char *attribute_group_oid;
	ACDistinguishedName_t *group_policy_authority;
	} ACAttributeGroup_t;

typedef struct ACAttributeUnizeto {
	char *attribute_unizeto_oid;
	ACDistinguishedName_t *unizeto_policy_authority;
	} ACAttributeUnizeto_t;


typedef struct ACAttributes {
	ACAttributeRole_t *ACAttributeRole;
	ACAttributeClearance_t *ACAttributeClearance;
	ACAttributeGroup_t *ACAttributeGroup;
	ACAttributeUnizeto_t *ACAttributeUnizeto;
	} ACAttributes_t;

/*struct Zero { };*/

typedef struct ACExtensions
#ifndef WIN32
{
	/* empty structure */
}
#endif
ACExtensions_t;

typedef struct ACConfiguration {
	ACVersion_t *ACVersionSection;
	ACHolder_t *ACHolderSection;
	ACIssuer_t *ACIssuerSection;
	ACSignature_t *ACSignatureSection;
	ACSerialNumber_t *ACSerialNumberSection;
	ACAttCertValidityPeriod_t *ACAttCertValidityPeriodSection;
	ACAttributes_t *ACAttributesSection;
	ACExtensions_t *ACExtensionsSection;
	} ACConfiguration_t;

typedef struct DataSource {
/* 	AABooleanEnum_t plaintext_source_enabled; */
/* 	char *plaintext_filename; */
/* 	AABooleanEnum_t database_source_enabled; */
	AADatabaseTypeEnum_t database_type;
	char *database_connection_string;
	} DataSource_t;

typedef struct AAServerConfiguration {
	DataSource_t *DataSourceSection;
	} AAServerConfiguration_t;

typedef struct ConfigData {
	ACConfiguration_t *ACConfiguration;
	AAServerConfiguration_t *AAServerConfiguration;
	/* opcje serwera tutaj */
} ConfigData_t;

#if defined(__cplusplus)
extern "C" {
#endif
	/**********************************************************
	READ: Odczyt danych konfiguracyjnych.
	**********************************************************/
	/* Funkcje eksportowalne */
	int ConfigData_read(const char *filename, ConfigData_t **ConfigData);
	/* Funkcje prywatne */
	int ACConfiguration_read(bmd_conf *configuration, ACConfiguration_t **ACConfiguration);
	int AAServerConfiguration_read(bmd_conf *configuration, AAServerConfiguration_t **AAServerConfiguration);
	int AADataSource_read(bmd_conf *configuration, DataSource_t **DataSource);
	int ACDistinguishedName_read(bmd_conf *configuration,
				const char *section, const char *dn_prefix,
				ACDistinguishedName_t **ACDistinguishedName);
	int ACVersion_read(bmd_conf *configuration, ACVersion_t **ACVersion);
	int ACHolder_read(bmd_conf *configuration, ACHolder_t **ACHolder);
	int ACIssuer_read(bmd_conf *configuration, ACIssuer_t **ACIssuer);
	int ACSignature_read(bmd_conf *configuration, ACSignature_t **ACSignature);
	int ACSerialNumber_read(bmd_conf *configuration, ACSerialNumber_t **ACSerialNumber);
	int ACAttCertValidityPeriod_read(bmd_conf *configuration, ACAttCertValidityPeriod_t **ACAttCertValidityPeriod);
	int ACAttributes_read(bmd_conf *configuration, ACAttributes_t **ACAttributes);
	int ACAttributeRole_read(bmd_conf *configuration, ACAttributeRole_t **ACAttributeRole);
	int ACAttributeClearance_read(bmd_conf *configuration, ACAttributeClearance_t **ACAttributeClearance);
	int ACAttributeGroup_read(bmd_conf *configuration, ACAttributeGroup_t **ACAttributeGroup);
	int ACAttributeUnizeto_read(bmd_conf *configuration, ACAttributeUnizeto_t **ACAttributeUnizeto);
	int ACExtensions_read(bmd_conf *configuration, ACExtensions_t **ACExtensions);

	/**********************************************************
	VALIDATE: Walidacja danych konfiguracyjnych.
	**********************************************************/
	/* Funkcje eksportowalne */
	int ConfigData_validate(ConfigData_t *ConfigData);
	/* Funkcje prywatne */
	int ACConfiguration_validate(ACConfiguration_t *ACConfiguration);
	int AAServerConfiguration_validate(AAServerConfiguration_t *AAServerConfiguration);
	int AADataSource_validate(DataSource_t *DataSource);
	int ACDistinguishedName_validate(const char *dn_prefix, ACDistinguishedName_t *ACDistinguishedName);
	int ACVersion_validate(ACVersion_t *ACVersion);
	int ACHolder_validate(ACHolder_t *ACHolder);
	int ACIssuer_validate(ACIssuer_t *ACIssuer);
	int ACSignature_validate(ACSignature_t *ACSignature);
	int ACSerialNumber_validate(ACSerialNumber_t *ACSerialNumber);
	int ACAttCertValidityPeriod_validate(ACAttCertValidityPeriod_t *ACAttCertValidityPeriod);
	int ACAttributes_validate(ACAttributes_t *ACAttributes);
	int ACAttributeRole_validate(ACAttributeRole_t *ACAttributeRole);
	int ACAttributeClearance_validate(ACAttributeClearance_t *ACAttributeClearance);
	int ACAttributeGroup_validate(ACAttributeGroup_t *ACAttributeGroup);
	int ACAttributeUnizeto_validate(ACAttributeUnizeto_t *ACAttributeUnizeto);
	int ACExtensions_validate(ACExtensions_t *ACExtensions);
	/**********************************************************
	FREE: Zwalnianie danych konfiguracyjnych.
	**********************************************************/
	/* Funkcje eksportowalne */
	int ConfigData_free(ConfigData_t **ConfigData);
	/* Funkcje prywatne */
	int ACConfiguration_free(ACConfiguration_t **ACConfiguration);
	int AAServerConfiguration_free(AAServerConfiguration_t **AAServerConfiguration);
	int AADataSource_free(DataSource_t **DataSource);
	int AC_free_ptr(void **ptr);
	int ACDistinguishedName_free(ACDistinguishedName_t **ACDistinguishedName);
	int ACVersion_free(ACVersion_t **ACVersion);
	int ACHolder_free(ACHolder_t **ACHolder);
	int ACIssuer_free(ACIssuer_t **ACIssuer);
	int ACSignature_free(ACSignature_t **ACSignature);
	int ACSerialNumber_free(ACSerialNumber_t **ACSerialNumber);
	int ACAttCertValidityPeriod_free(ACAttCertValidityPeriod_t **ACAttCertValidityPeriod);
	int ACAttributes_free(ACAttributes_t **ACAttributes);
	int ACAttributeRole_free(ACAttributeRole_t **ACAttributeRole);
	int ACAttributeClearance_free(ACAttributeClearance_t **ACAttributeClearance);
	int ACAttributeGroup_free(ACAttributeGroup_t **ACAttributeGroup);
	int ACAttributeUnizeto_free(ACAttributeUnizeto_t **ACAttributeUnizeto);
	int ACExtensions_free(ACExtensions_t **ACExtensions);

	/**********************************************************
	PRINT: Prezentacja danych konfiguracyjnych.
	**********************************************************/
	/* Funkcje eksportowalne */
	int ConfigData_print(ConfigData_t *ConfigData);
	/* Funkcje prywatne */
	int ACConfiguration_print(ACConfiguration_t *ACConfiguration);
	int AAServerConfiguration_print(AAServerConfiguration_t *AAServerConfiguration);
	int AADataSource_print(DataSource_t *DataSource);
	int ACDistinguishedName_print(const char *dn_prefix, ACDistinguishedName_t *ACDistinguishedName);
	int ACVersion_print(ACVersion_t *ACVersion);
	int ACHolder_print(ACHolder_t *ACHolder);
	int ACIssuer_print(ACIssuer_t *ACIssuer);
	int ACSignature_print(ACSignature_t *ACSignature);
	int ACSerialNumber_print(ACSerialNumber_t *ACSerialNumber);
	int ACAttCertValidityPeriod_print(ACAttCertValidityPeriod_t *ACAttCertValidityPeriod);
	int ACAttributes_print(ACAttributes_t *ACAttributes);
	int ACAttributeRole_print(ACAttributeRole_t *ACAttributeRole);
	int ACAttributeClearance_print(ACAttributeClearance_t *ACAttributeClearance);
	int ACAttributeGroup_print(ACAttributeGroup_t *ACAttributeGroup);
	int ACAttributeUnizeto_print(ACAttributeUnizeto_t *ACAttributeUnizeto);
	int ACExtensions_print(ACExtensions_t *ACExtensions);

#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AA_CONFIG_READER_INCLUDED_*/
