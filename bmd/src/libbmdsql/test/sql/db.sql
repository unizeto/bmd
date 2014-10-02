/****************************************/
/*	SLOWNIKI			*/
/****************************************/

CREATE TABLE db_info (
	id		integer PRIMARY KEY,
	db_version	varchar(256),
	profiled_for	varchar(256),
	created		TIMESTAMP with time zone
);
CREATE SEQUENCE db_info_id_seq INCREMENT 1 MINVALUE 1;
INSERT INTO db_info (id, db_version, profiled_for, created)
	    VALUES  (nextval('db_info_id_seq'), '1.0.1', 'generic BMD', 'now');

CREATE TABLE users (
	id		integer PRIMARY KEY,
	dn		varchar(256)
);
CREATE SEQUENCE users_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX users_id_index ON users (id);
CREATE INDEX users_dn_index ON users(dn);

CREATE TABLE doc_types (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE doc_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX doc_types_type_index ON doc_types(type);

CREATE TABLE software_types (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,   -- oid informujacy o typie
	description	varchar(256),    -- identyfikator w postaci string
	sql_cast_string varchar(256)
);
CREATE SEQUENCE software_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX software_types_type_index ON software_types(type);

CREATE TABLE add_metadata_types (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE add_metadata_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX add_metadata_types_type_index ON add_metadata_types(type);


CREATE TABLE pki_types (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)

);
CREATE SEQUENCE pki_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX pki_types_type_index ON pki_types(type);

CREATE TABLE groups (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE groups_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX groups_type_index ON groups(type);

CREATE TABLE sec_levels (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE sec_levels_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX sec_levels_type_index ON sec_levels(type);

CREATE TABLE sec_categories (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE sec_categories_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX sec_categories_type_index ON sec_categories(type);


/****************************************/
/*	CRYPTO OBJECTS			*/
/****************************************/

CREATE TABLE crypto_objects (
	id			INTEGER	PRIMARY KEY,
	name			TEXT,
	filesize		INTEGER NOT NULL,
	raster			oid,		-- oid (postgresowy) pliku zlozony do BMD jako large object
	symkey			oid,
	fk_owner		INTEGER REFERENCES users(id),
	fk_software_types 	INTEGER REFERENCES software_types(id),
	insert_date		TIMESTAMP with time zone
);
CREATE SEQUENCE crypto_objects_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX crypto_objects_fk_owner_index ON crypto_objects (fk_owner);
CREATE INDEX crypto_objects_fk_software_types_index ON crypto_objects (fk_software_types);
CREATE INDEX crypto_objects_insert_date_index ON crypto_objects (insert_date);
CREATE INDEX crypto_objects_name_index ON crypto_objects(name);

/***********************************************************/
CREATE TABLE relation_types (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE relation_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX relation_types_type_index ON doc_types(type);

/* TODO zastanowic sie nad indeksami */
/* Tabele projektowane na wyrost - kolejne funkcjonalności beda dodawane sukcesywnie */
CREATE TABLE cases (
	id			INTEGER	PRIMARY KEY,
	case_type		varchar(256),
	case_code		varchar(256) UNIQUE,
	case_code_passwd	varchar(256),
	case_title		varchar(256),
	case_description	varchar(256),
	case_start_date		TIMESTAMP with time zone,
	case_last_update	TIMESTAMP with time zone
);
CREATE SEQUENCE cases_id_seq INCREMENT 1 MINVALUE 1;
/* Specjalna sekwencja na automatyczne nadawanie numerów sprawom */
/* Wniosek - kody spraw nie mogą liczbami całkowitymi		*/
CREATE SEQUENCE cases_case_code_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX cases_case_code_index ON cases (case_code);


/* TODO zastanowic sie nad indeksami */
CREATE TABLE co_cases (
	id			INTEGER	PRIMARY KEY,
	fk_crypto_objects	INTEGER REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	fk_cases		INTEGER REFERENCES cases(id) NOT NULL,
	fk_crypto_object_parent INTEGER,	/*REFERENCES crypto_objects(id) wpis zakomentowany */
						/* po usunieciu rodzica dokumenty od niego zalezne moga istniec */
	fk_relation_types	INTEGER REFERENCES relation_types(id),
	last_update		TIMESTAMP with time zone,
	visible			INTEGER
);
CREATE SEQUENCE co_cases_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX co_cases_fk_crypto_objects_index ON crypto_objects (id);
CREATE INDEX co_cases_fk_cases ON cases (id);
/***********************************************************/
/****************************************/
/*	ROLES AND ACTIONS		*/
/****************************************/
CREATE TABLE roles (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE roles_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX roles_type_index ON roles(type);

CREATE TABLE actions (
	id		integer	PRIMARY KEY,
	type		varchar(256) UNIQUE,
	description	varchar(256),
	sql_cast_string varchar(256)
);
CREATE SEQUENCE actions_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX actions_type_index ON roles(type);

CREATE TABLE roles_and_actions (
	fk_roles_id	integer REFERENCES roles(id) NOT NULL,
	fk_actions_id	integer REFERENCES actions(id) NOT NULL,
	PRIMARY KEY (fk_roles_id, fk_actions_id)
);
CREATE SEQUENCE roles_and_actions_id_seq INCREMENT 1 MINVALUE 1;


/****************************************/
/*	USR METADATA			*/
/****************************************/

CREATE TABLE crypto_objects_doc_types (
	fk_crypto_objects		integer REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	fk_doc_types			integer REFERENCES doc_types(id),
	PRIMARY KEY (fk_crypto_objects, fk_doc_types)
);
CREATE INDEX crypto_objects_doc_types_fk_crypto_objects_index ON crypto_objects_doc_types(fk_crypto_objects);
CREATE INDEX crypto_objects_doc_types_fk_doc_types_index ON crypto_objects_doc_types(fk_doc_types);

CREATE TABLE usr_description (
	id				integer		PRIMARY KEY,
 	fk_crypto_objects		integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
 	data_description		varchar(256)
);
CREATE SEQUENCE usr_description_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX usr_description_data_description_index ON usr_description(data_description);
CREATE INDEX usr_description_fk_crypto_objects_index ON usr_description(fk_crypto_objects);

CREATE TABLE usr_keywords (
	id			integer		PRIMARY KEY,
	fk_crypto_objects	integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	keyword			varchar(256)
);
CREATE SEQUENCE usr_keywords_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX usr_keywords_fk_crypto_objects_index ON usr_keywords(fk_crypto_objects);
CREATE INDEX usr_keywords_keyword_index ON usr_keywords(keyword);


/****************************************/
/*	ADDITIONAL METADATA		*/
/****************************************/

CREATE TABLE add_metadata (
	id				integer		PRIMARY KEY,
	fk_crypto_objects		integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	add_metadata_value		varchar(256),
	fk_add_metadata_types 		integer		REFERENCES add_metadata_types(id) NOT NULL,
	update_date			TIMESTAMP with time zone,
	update_reason			varchar(256),
	visible				integer
);
CREATE SEQUENCE add_metadata_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX add_metadata_fk_crypto_objects_index ON add_metadata (fk_crypto_objects);
CREATE INDEX add_metadata_amd_value_types ON add_metadata (add_metadata_value, fk_add_metadata_types);

/****************************************/
/*	SYS METADATA			*/
/****************************************/

CREATE TABLE sys_metadata_clearances (
	fk_sec_levels		integer		REFERENCES sec_levels(id),
	fk_sec_categories	integer		REFERENCES sec_categories(id),
	fk_crypto_objects	integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	PRIMARY KEY (fk_sec_levels, fk_sec_categories, fk_crypto_objects)
);
CREATE INDEX sys_metadata_clearances_fk_crypto_objects_index ON sys_metadata_clearances(fk_crypto_objects);
CREATE INDEX sys_metadata_clearances_fk_sec_levels_index ON sys_metadata_clearances(fk_sec_levels);
CREATE INDEX sys_metadata_clearances_fk_sec_categories_index ON sys_metadata_clearances(fk_sec_categories);

CREATE TABLE sys_metadata_groups (
	fk_groups		integer		REFERENCES groups(id),
	fk_crypto_objects	integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	PRIMARY KEY (fk_groups, fk_crypto_objects)
);
CREATE INDEX sys_metadata_groups_fk_crypto_objects_index ON sys_metadata_groups(fk_crypto_objects);
CREATE INDEX sys_metadata_groups_fk_groups_index ON sys_metadata_groups(fk_groups);

CREATE TABLE groups_graph (
	src			integer		REFERENCES groups(id),
	dest			integer		REFERENCES groups(id),
	PRIMARY KEY (src, dest)
);
/* indeksy zbedne */

CREATE TABLE sys_metadata_roles (
	fk_roles		integer		REFERENCES roles(id),
	fk_crypto_objects	integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	PRIMARY KEY (fk_roles, fk_crypto_objects)
);
CREATE INDEX sys_metadata_roles_fk_crypto_objects_index ON sys_metadata_roles(fk_crypto_objects);
CREATE INDEX sys_metadata_roles_fk_roles_index ON sys_metadata_roles(fk_roles);


/****************************************/
/*	PROOFS CERTIFICATES		*/
/****************************************/
/* relacja do tej tabeli przez identyfikator umieszczony jako metadana PKI z OID certyfikatu */
CREATE TABLE prf_certs (
	id				integer		PRIMARY KEY,
	version				integer		NOT NULL,
	serialNumber			varchar(256)	NOT NULL,
	subjectDN			varchar(1024)	NOT NULL,
	signatureAlgOID			varchar(256)	NOT NULL,
	issuerDN			varchar(1024)	NOT NULL,
	validitynotBefore		TIMESTAMP with time zone NOT NULL,
	validitynotAfter		TIMESTAMP with time zone NOT NULL,
	cert				oid
);
CREATE SEQUENCE prf_certs_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX prf_certs_all_index ON prf_certs (	version, serialNumber, subjectDN, signatureAlgOID, issuerDN, validitynotBefore, validitynotAfter);

/****************************************/
/*	PROOFS CRLs			*/
/****************************************/
/* relacja do tej tabeli poprzez issuerDN i issuerSN oraz najbardziej aktualny czas */
CREATE TABLE prf_crls (
	id				integer		PRIMARY KEY,
	version				integer		NOT NULL,
	CRLserialNumber			varchar(256)	NOT NULL,
	deltaCRL			varchar(256)	NOT NULL,
	signatureAlgOID			varchar(256)	NOT NULL,
	issuerDN			varchar(1024)	NOT NULL,
	thisUpdate			TIMESTAMP with time zone NOT NULL,
	nextUpdate			TIMESTAMP with time zone NOT NULL,
	numberOfRevokedEntries		integer,
	crl				oid
);
CREATE SEQUENCE prf_crls_id_seq INCREMENT 1 MINVALUE 1;

/****************************************/
/*	PROOFS ARLs		*/
/****************************************/
/* relacja do tej tabeli poprzez issuerDN i issuerSN oraz najbardziej aktualny czas */
CREATE TABLE prf_arls (
	id				integer		PRIMARY KEY,
	version				integer		NOT NULL,
	ARLserialNumber			varchar(256)	NOT NULL,
	deltaARL			varchar(256)	NOT NULL,
	signatureAlgOID			varchar(256)	NOT NULL,
	issuerDN			varchar(1024)	NOT NULL,
	thisUpdate			TIMESTAMP with time zone NOT NULL,
	nextUpdate			TIMESTAMP with time zone NOT NULL,
	numberOfRevokedEntries		integer,
	arl				oid
);
CREATE SEQUENCE prf_arls_id_seq INCREMENT 1 MINVALUE 1;

/****************************************/
/*	PROOFS TIMESTAMPSs		*/
/****************************************/
/* relacja do tej tabeli poprzez fk_crypto_objects - tu sa znaczniki	*/
/* czasu wygenerowane zarowno przez klienta jak i serwer BMD		*/
/* patrz flaga userOrSystem						*/
CREATE TABLE prf_timestamps (
	id				integer		PRIMARY KEY,
	fk_crypto_objects		integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	version				integer		NOT NULL,
	policyOID			varchar(256)	NOT NULL,
	serialNumber			varchar(256)	NOT NULL,
	genTime				TIMESTAMP with time zone NOT NULL,
	timestampCertValidTo		TIMESTAMP with time zone NOT NULL,
	tsaDN				varchar(1024)	NOT NULL,
	tsaSN				varchar(256)	NOT NULL,
	standard			integer		NOT NULL,
	userOrSystem			integer		NOT NULL,
	conserved			integer		NOT NULL,
	timestamp			oid
);
CREATE SEQUENCE prf_timestamps_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX prf_timestamps_fk_crypto_objects_index ON prf_timestamps (fk_crypto_objects);

/****************************************/
/*	PROOFS SIGNATUREs		*/
/****************************************/
CREATE TABLE prf_signatures (
	id				integer		PRIMARY KEY,
	fk_crypto_objects		integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	standard			integer		NOT NULL,
	userOrsystem			integer		NOT NULL,
	signature			oid
);
CREATE SEQUENCE prf_signatures_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX prf_signatures_fk_crypto_objects_index ON prf_signatures (fk_crypto_objects);

/****************************************/
/*	PROOFS CONNECTION TABLES	*/
/****************************************/
CREATE TABLE co_prf_certs (
	fk_crypto_objects	integer 	REFERENCES crypto_objects(id) 	ON DELETE CASCADE NOT NULL,
	fk_prf_certs		integer		REFERENCES prf_certs(id) 	NOT NULL,
	PRIMARY KEY (fk_crypto_objects, fk_prf_certs)
);	
CREATE SEQUENCE co_prf_certs_id_seq INCREMENT 1 MINVALUE 1;
CREATE INDEX co_prf_certs_fk_crypto_objects_index ON co_prf_certs (fk_crypto_objects);
CREATE INDEX co_prf_certs_fk_prf_certs_index ON co_prf_certs (fk_prf_certs);

/********************************/
/*	     SLOWNIKI		*/
/********************************/

/************ GRUPY *************/
INSERT INTO groups (id, type, description, sql_cast_string) 
	VALUES ( nextval('groups_id_seq'), '1.2.616.1.113527.4.3.2.3.0', 'undefined', 'undefined');
INSERT INTO groups (id, type, description, sql_cast_string) 
	VALUES ( nextval('groups_id_seq'), '1.2.616.1.113527.4.3.2.3.1', 'pracownicy etatowi', 'undefined');
INSERT INTO groups (id, type, description, sql_cast_string) 
	VALUES ( nextval('groups_id_seq'), '1.2.616.1.113527.4.3.2.3.2', 'wspolpracownicy', 'undefined');
INSERT INTO groups (id, type, description, sql_cast_string) 
	VALUES ( nextval('groups_id_seq'), '1.2.616.1.113527.4.3.2.3.3', 'kierownicy projektow', 'undefined');

/************ SOFTWARE TYPES *************/
INSERT INTO software_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('software_types_id_seq'), '1.2.616.1.113527.4.3.1.5.0', 'undefined', 'undefined');
INSERT INTO software_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('software_types_id_seq'), '1.2.616.1.113527.4.3.1.5.1', 'word', 'undefined');
INSERT INTO software_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('software_types_id_seq'), '1.2.616.1.113527.4.3.1.5.2', 'excel', 'undefined');
INSERT INTO software_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('software_types_id_seq'), '1.2.616.1.113527.4.3.1.5.3', 'plain text', 'undefined');

/************ DOC TYPES *************/
INSERT INTO doc_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('doc_types_id_seq'), '1.2.616.1.113527.4.3.1.4.0', 'undefined', 'undefined');
INSERT INTO doc_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('doc_types_id_seq'), '1.2.616.1.113527.4.3.1.4.1', 'wewnetrzny', 'undefined');
INSERT INTO doc_types (id, type, description, sql_cast_string) 
	VALUES ( nextval('doc_types_id_seq'), '1.2.616.1.113527.4.3.1.4.2', 'zewnetrzny', 'undefined');

/************ SECURITY LEVELS *************/
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.0', 'unmarked', 'undefined');
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.1', 'unclassified', 'undefined');
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.2', 'restricted', 'undefined');
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.3', 'confidential', 'undefined');
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.4', 'secret', 'undefined');
INSERT INTO sec_levels (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_levels_id_seq'), '1.2.616.1.113527.4.3.2.4.5', 'topsecret', 'undefined');

/************ SECURITY CATEGORIES *************/
INSERT INTO sec_categories (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_categories_id_seq'), '1.2.616.1.113527.4.3.2.1.0', 'undefined', 'undefined');
INSERT INTO sec_categories (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_categories_id_seq'), '1.2.616.1.113527.4.3.2.1.1', 'niejawne - tajemnica projektowa', 'undefined');
INSERT INTO sec_categories (id, type, description, sql_cast_string) 
	VALUES (nextval('sec_categories_id_seq'), '1.2.616.1.113527.4.3.2.1.2', 'niejawne - tajemnica sluzbowa', 'undefined');

/************ PKI OBJECTS *************/
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.0', 'undefined', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.1', 'signature', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.2', 'timestamp', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.3', 'qualified_certificate', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.4', 'attribute_certificate', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.5', 'crl', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.6', 'arl', 'undefined');
INSERT INTO pki_types (id, type, description, sql_cast_string) 
	VALUES (nextval('pki_types_id_seq'), '1.2.616.1.113527.4.4.7', 'ocsp', 'undefined');

INSERT INTO add_metadata_types (id, type, description, sql_cast_string) 
	VALUES (nextval('add_metadata_types_id_seq'), '1.2.616.1.113527.4.3.4.0', 'undefined', 'undefined');

INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.0', 'undefined', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.1', 'ma format', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.2', 'ma wersje', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.3', 'jest wersja', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.4', 'ma czesc', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.5', 'jest czescia', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.6', 'ma odniesienie', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.7', 'odnosi sie do', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.8', 'ma zrodlo', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.9', 'wymaga', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.10', 'jest wymagany', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.11', 'ma dekretacje', 'undefined');
INSERT INTO relation_types (id, type, description, sql_cast_string) 
	VALUES (nextval('relation_types_id_seq'), '1.2.616.1.113527.4.3.1.7.12', 'jest dekretacja', 'undefined');


INSERT INTO roles (id, type, description, sql_cast_string)
	VALUES (nextval('roles_id_seq'), '1.2.616.1.113527.4.3.1.8.0', 'undefined', 'undefined');
INSERT INTO roles (id, type, description, sql_cast_string)
	VALUES (nextval('roles_id_seq'), '1.2.616.1.113527.4.3.1.8.1', 'administrator', 'undefined');
INSERT INTO roles (id, type, description, sql_cast_string)
	VALUES (nextval('roles_id_seq'), '1.2.616.1.113527.4.3.1.8.2', 'gosc', 'undefined');


INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (1, '1.2.616.1.113527.4.3.1.9.1', 'BMDDatagram__datagramType_requestGetFormData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (2, '1.2.616.1.113527.4.3.1.9.2', 'BMDDatagram__datagramType_requestSelectBmdData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (3, '1.2.616.1.113527.4.3.1.9.3', 'BMDDatagram__datagramType_requestGetBmdData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (4, '1.2.616.1.113527.4.3.1.9.4', 'BMDDatagram__datagramType_requestGetBmdData_lo', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (5, '1.2.616.1.113527.4.3.1.9.5', 'BMDDatagram__datagramType_requestGetBmdDataWithProofs', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (6, '1.2.616.1.113527.4.3.1.9.6', 'BMDDatagram__datagramType_requestGetBmdDataWithProofs_lo', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (7, '1.2.616.1.113527.4.3.1.9.7', 'BMDDatagram__datagramType_requestPutBmdData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (8, '1.2.616.1.113527.4.3.1.9.8', 'BMDDatagram__datagramType_requestPutBmdData_lo', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (9, '1.2.616.1.113527.4.3.1.9.9', 'BMDDatagram__datagramType_requestDelBmdData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (10, '1.2.616.1.113527.4.3.1.9.10', 'BMDDatagram__datagramType_requestOpenSession', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (11, '1.2.616.1.113527.4.3.1.9.11', 'BMDDatagram__datagramType_requestCloseSession', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (12, '1.2.616.1.113527.4.3.1.9.12', 'BMDDatagram__datagramType_requestGetBmdDataCGI', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (13, '1.2.616.1.113527.4.3.1.9.13', 'BMDDatagram__datagramType_requestGetBmdDataCGIPKI', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (14, '1.2.616.1.113527.4.3.1.9.14', 'BMDDatagram__datagramType_requestUpdateBmdData', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (15, '1.2.616.1.113527.4.3.1.9.15', 'BMDDatagram__datagramType_requestGetBmdMetadata', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (16, '1.2.616.1.113527.4.3.1.9.16', 'BMDDatagram__datagramType_requestGetBmdMetadataWithTime', 'undefined');
INSERT INTO actions (id, type, description, sql_cast_string)
	VALUES (17, '1.2.616.1.113527.4.3.1.9.17', 'BMDDatagram__datagramType_requestGetBmdOnlyFile', 'undefined');

INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 1);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 2);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 3);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 4);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 5);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 6);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 7);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 8);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 9);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 10);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 11);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 12);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 13);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 14);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 15);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 16);
INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (2, 17);

INSERT INTO roles_and_actions (fk_roles_id, fk_actions_id)
	VALUES (3, 2);

