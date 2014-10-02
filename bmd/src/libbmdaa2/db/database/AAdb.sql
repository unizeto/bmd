DROP TABLE uniattr_slot_values;
DROP TABLE uniattr_slot;
DROP TABLE uniattr_main;
DROP TABLE uniattr_slot_schema_main;
DROP TABLE uniattr_slot_def;
DROP TABLE uniattr_syntax_oid;
DROP TABLE uniattr_slot_types;
DROP TABLE group_main;
DROP TABLE clearance_main;
DROP TABLE role_main;
DROP TABLE aaholder_main;
DROP TABLE group_names;
DROP TABLE role_names;
DROP TABLE clearance_classlist;
DROP TABLE clearance_seccateg_stx;
DROP TABLE clearance_seccateg;
DROP TABLE general_name_types;

DROP SEQUENCE uniattr_slot_values_id_seq;
DROP SEQUENCE uniattr_slot_id_seq;
DROP SEQUENCE uniattr_main_id_seq;
DROP SEQUENCE uniattr_slot_schema_main_id_seq;
DROP SEQUENCE uniattr_syntax_oid_id_seq;
DROP SEQUENCE uniattr_slot_def_id_seq;
DROP SEQUENCE uniattr_slot_types_id_seq;
DROP SEQUENCE group_main_id_seq;
DROP SEQUENCE clearance_main_id_seq;
DROP SEQUENCE role_main_id_seq;
DROP SEQUENCE aaholder_main_id_seq;
DROP SEQUENCE group_names_id_seq;
DROP SEQUENCE role_names_id_seq;
DROP SEQUENCE clearance_classlist_id_seq;
DROP SEQUENCE clearance_seccateg_stx_id_seq;
DROP SEQUENCE clearance_seccateg_id_seq;
DROP SEQUENCE general_name_types_id_seq;

/* Changelog:
2006.10.17 - usunieto tabele role_authorities i inserty z nia związane
2006.10.17 - usunieto tabele uniattr_policyauth i inserty z nia zwiazane
*/

/********************************************************/
/* SLOWNIKI 						*/
/********************************************************/
CREATE SEQUENCE general_name_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE general_name_types (
	id		integer		DEFAULT nextval('general_name_types_id_seq') PRIMARY KEY,
	type		varchar(256)	UNIQUE,		-- oid informujacy o typie
	description	varchar(256),			-- identyfikator w postaci string
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

CREATE SEQUENCE clearance_seccateg_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE clearance_seccateg (
	id		integer		DEFAULT nextval('clearance_seccateg_id_seq') PRIMARY KEY,
	type		varchar(256),
	value		varchar(256),
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

CREATE SEQUENCE clearance_seccateg_stx_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE clearance_seccateg_stx (
	id		integer		DEFAULT nextval('clearance_seccateg_stx_id_seq') PRIMARY KEY,
	type		varchar(256),
	value		varchar(256),
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

CREATE SEQUENCE clearance_classlist_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE clearance_classlist (
	id		integer		DEFAULT nextval('clearance_classlist_id_seq') PRIMARY KEY,
	type		varchar(256),
	value		varchar(256),
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

CREATE SEQUENCE role_names_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE role_names (
	id		integer		DEFAULT nextval('role_names_id_seq') PRIMARY KEY,
	type		varchar(256),
	value		varchar(256),
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

CREATE SEQUENCE group_names_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE group_names (
	id		integer		DEFAULT nextval('group_names_id_seq') PRIMARY KEY,
	type		varchar(256),
	value		varchar(256),
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

/********************************************************/
/* UZYTKOWNICY AA					*/
/********************************************************/
CREATE SEQUENCE aaholder_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE aaholder_main (
	id		integer		DEFAULT nextval('aaholder_main_id_seq') PRIMARY KEY,
	pkc_serial	varchar(256),					-- serial PKC zwiazanego z AC
	-- ponizsze 2 pola sluza do wyszukiwania, pozostale do zbierania statystyk
	pkcissuer_type	integer		REFERENCES general_name_types(id),-- typ pola PKC Issuer Value
	pkcissuer_value	varchar(1024),					-- wartosc pola PKC Issuer Value
	ldap_c		varchar(8),					-- country of PKC Issuer
	ldap_l		varchar(256),					-- locality of PKC Issuer
	ldap_o		varchar(256),					-- organization of PKC Issuer
	ldap_ou		varchar(256),					-- organizational unit of PKC Issuer
	ldap_cn		varchar(256),					-- common name of PKC Issuer
	ldap_email	varchar(256),					-- email of PKC Issuer
	ldap_st		varchar(256),					-- state of PKC Issuer
	dtcreat		timestamp	with time zone,
	dtalter		timestamp	with time zone
);

/********************************************************/
/* ROLE							*/
/********************************************************/
/* Glowna tabela z atrybutami ROLE */
CREATE SEQUENCE role_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE role_main (
	id			integer		DEFAULT nextval('role_main_id_seq') PRIMARY KEY,
	fk_aaholder_main	integer		REFERENCES aaholder_main(id),
	fk_role_names		integer		REFERENCES role_names(id),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

/********************************************************/
/* CLEARANCE						*/
/********************************************************/
CREATE SEQUENCE clearance_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE clearance_main (
	id			integer		DEFAULT nextval('clearance_main_id_seq') PRIMARY KEY,
	fk_aaholder_main	integer		REFERENCES aaholder_main(id),
	fk_clearance_classlist	integer		REFERENCES clearance_classlist(id),
	fk_clearance_seccateg_stx	integer	REFERENCES clearance_seccateg_stx(id),
	fk_clearance_seccateg	integer		REFERENCES clearance_seccateg(id),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

/********************************************************/
/* GROUP						*/
/********************************************************/
/* Glowna tabela z atrybutami GROUP */
CREATE SEQUENCE group_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE group_main (
	id			integer		DEFAULT nextval('group_main_id_seq') PRIMARY KEY,
	fk_aaholder_main	integer		REFERENCES aaholder_main(id),
	fk_group_names		integer		REFERENCES group_names(id),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

/********************************************************/
/* UNIZETO ATTR SYNTAX					*/
/********************************************************/
CREATE SEQUENCE uniattr_slot_types_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_slot_types (
	id			integer		DEFAULT nextval('uniattr_slot_types_id_seq') PRIMARY KEY,
	slottypeoid		varchar(256)	UNIQUE,
	description		varchar(256),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

/* Tabela z opisem slotu bedacego czescia skladowa UnizetoAttrSyntax */
CREATE SEQUENCE uniattr_slot_def_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_slot_def (
	id			integer		DEFAULT nextval('uniattr_slot_def_id_seq') PRIMARY KEY,
	fk_uniattr_slot_types	integer		REFERENCES uniattr_slot_types(id),
	noofvalues		integer,
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

/* Pomocnicza tabela z OIDem i definicja skladni atrybutu UnizetoAttrSyntax */
CREATE SEQUENCE uniattr_syntax_oid_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_syntax_oid (
	id				integer		DEFAULT nextval('uniattr_syntax_oid_id_seq') PRIMARY KEY,
	syntaxoid			varchar(256),
	description			varchar(256),
	noofslots			integer,
	dtcreat				timestamp	with time zone,
	dtalter				timestamp	with time zone
);

/* Tabela łącząca uniattr_syntax_oid z uniattr_slot_def */
CREATE SEQUENCE uniattr_slot_schema_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_slot_schema_main (
	id				integer		DEFAULT nextval('uniattr_slot_schema_main_id_seq') PRIMARY KEY,
	fk_uniattr_syntax_oid		integer		REFERENCES uniattr_syntax_oid(id),
	fk_uniattr_slot_def		integer		REFERENCES uniattr_slot_def(id),
	dtcreat				timestamp	with time zone,
	dtalter				timestamp	with time zone
);

CREATE SEQUENCE uniattr_main_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_main (
	id			integer		DEFAULT nextval('uniattr_main_id_seq') PRIMARY KEY,
	fk_aaholder_main	integer		REFERENCES aaholder_main(id),
	fk_uniattr_syntax_oid	integer		REFERENCES uniattr_syntax_oid(id),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

CREATE SEQUENCE uniattr_slot_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_slot (
	id			integer 	DEFAULT nextval('uniattr_slot_id_seq') PRIMARY KEY,
	fk_uniattr_main		integer		REFERENCES uniattr_main(id),
	fk_uniattr_slot_types	integer		REFERENCES uniattr_slot_types(id),
	dtcreat			timestamp	with time zone,
	dtalter			timestamp	with time zone
);

CREATE SEQUENCE uniattr_slot_values_id_seq INCREMENT 1 MINVALUE 1;
CREATE TABLE uniattr_slot_values (
	ID				integer		DEFAULT nextval('uniattr_slot_values_id_seq') PRIMARY KEY,
	fk_uniattr_slot			integer		REFERENCES uniattr_slot(id),
	value				varchar,
	dtcreat				timestamp	with time zone,
	dtalter				timestamp	with time zone
);

/********************************************************/
/* DICTIONARIES						*/
/********************************************************/
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.0', 'undefined', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.1', 'otherName', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.2', 'rfc822Name', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.3', 'dNSName', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.4', 'x400Address', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.5', 'directoryName', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.6', 'ediPartyName', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.7', 'uniformResourceIdentifier', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.8', 'ipAddress', 'now', 'now');
INSERT INTO general_name_types (id, type, description, dtcreat, dtalter) 
	VALUES (nextval('general_name_types_id_seq'), '1.2.3.4.9', 'registeredID', 'now', 'now');

INSERT INTO clearance_seccateg (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_seccateg_id_seq'), '1.2.616.1.113527.4.3.2.1.0', 'undefined', 'now', 'now');
INSERT INTO clearance_seccateg (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_seccateg_id_seq'), '1.2.616.1.113527.4.3.2.1.1', 'niejawne - tajemnica projektowa', 'now', 'now');
INSERT INTO clearance_seccateg (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_seccateg_id_seq'), '1.2.616.1.113527.4.3.2.1.2', 'niejawne - tajemnica sluzbowa', 'now', 'now');

INSERT INTO clearance_seccateg_stx (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_seccateg_stx_id_seq'), '1.2.3.4.5.6.7.8.0', 'SecCat - Syntax - Undefined', 'now', 'now');
INSERT INTO clearance_seccateg_stx (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_seccateg_stx_id_seq'), '1.2.3.4.5.6.7.8.9', 'SecCat - Syntax - String', 'now', 'now');

INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.0', 'unmarked', 'now', 'now');
INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.1', 'unclassified', 'now', 'now');
INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.2', 'restricted', 'now', 'now');
INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.3', 'confidential', 'now', 'now');
INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.4', 'secret', 'now', 'now');
INSERT INTO clearance_classlist (id, type, value, dtcreat, dtalter)
	VALUES (nextval('clearance_classlist_id_seq'), '1.2.616.1.113527.4.3.2.4.5', 'topsecret', 'now', 'now');

INSERT INTO role_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('role_names_id_seq'), '1.2.616.1.113527.4.3.2.5.0', 'undefined', 'now', 'now');
INSERT INTO role_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('role_names_id_seq'), '1.2.616.1.113527.4.3.2.5.1', 'administrator', 'now', 'now');
INSERT INTO role_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('role_names_id_seq'), '1.2.616.1.113527.4.3.2.5.2', 'operator', 'now', 'now');

INSERT INTO group_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('group_names_id_seq'), '1.2.616.1.113527.4.3.2.3.0', 'undefined', 'now', 'now');
INSERT INTO group_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('group_names_id_seq'), '1.2.616.1.113527.4.3.2.3.1', 'pracownicy etatowi', 'now', 'now');
INSERT INTO group_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('group_names_id_seq'), '1.2.616.1.113527.4.3.2.3.2', 'wspolpracownicy', 'now', 'now');
INSERT INTO group_names (id, type, value, dtcreat, dtalter)
	VALUES (nextval('group_names_id_seq'), '1.2.616.1.113527.4.3.2.3.3', 'kierownicy projektow', 'now', 'now');

/********************************************************/
/* EXAMPLE DATA						*/
/********************************************************/
/* przykladowy posiadacz certyfikatu atrybutów */
INSERT INTO aaholder_main (id, pkc_serial, pkcissuer_type, pkcissuer_value, ldap_c, ldap_l, ldap_o,
				ldap_ou, ldap_cn, ldap_email, ldap_st, dtcreat, dtalter)
	VALUES (nextval('aaholder_main_id_seq'), '00 AB CD 12', 4, 'unizeto.pl', 'PL', 'Szczecin', 
	'Unizeto SA', 'deployment', 'bbanan', 'bbanan@gazeta.pl', 'Zachodniopomorskie', 'now', 'now');
INSERT INTO aaholder_main (id, pkc_serial, pkcissuer_type, pkcissuer_value, ldap_c, ldap_l, ldap_o,
				ldap_ou, ldap_cn, ldap_email, ldap_st, dtcreat, dtalter)
	VALUES (nextval('aaholder_main_id_seq'), '03 4A 1C', 6, 'C=<PL> O=<Unizeto Sp. z o.o.> CN=<Certum Level I>', 'PL', 'Szczecin', 
	'Unizeto SA', 'deployment', 'bbanan', 'bbanan@gazeta.pl', 'Zachodniopomorskie', 'now', 'now');


/* przykladowe role dla bbanan */
INSERT INTO role_main (id, fk_aaholder_main, fk_role_names, dtcreat, dtalter)
	VALUES (nextval('role_main_id_seq'), 1, 2, 'now', 'now');
INSERT INTO role_main (id, fk_aaholder_main, fk_role_names, dtcreat, dtalter)
	VALUES (nextval('role_main_id_seq'), 2, 2, 'now', 'now');
INSERT INTO role_main (id, fk_aaholder_main, fk_role_names, dtcreat, dtalter)
	VALUES (nextval('role_main_id_seq'), 2, 3, 'now', 'now');

/* przykladowe clearance dla bbanan */
INSERT INTO clearance_main (id, fk_aaholder_main, fk_clearance_classlist, fk_clearance_seccateg_stx,
	fk_clearance_seccateg, dtcreat, dtalter)
	VALUES(nextval('clearance_main_id_seq'), 1, 2, 2, 2, 'now', 'now');

INSERT INTO clearance_main (id, fk_aaholder_main, fk_clearance_classlist, fk_clearance_seccateg_stx,
	fk_clearance_seccateg, dtcreat, dtalter)
	VALUES(nextval('clearance_main_id_seq'), 2, 2, 2, 2, 'now', 'now');

INSERT INTO clearance_main (id, fk_aaholder_main, fk_clearance_classlist, fk_clearance_seccateg_stx,
	fk_clearance_seccateg, dtcreat, dtalter)
	VALUES(nextval('clearance_main_id_seq'), 2, 3, 2, 3, 'now', 'now');

/* przykladowe grupy dla bbanan */
INSERT INTO group_main (id, fk_aaholder_main, fk_group_names, dtcreat, dtalter)
	VALUES (nextval('group_main_id_seq'), 1, 2, 'now', 'now');
INSERT INTO group_main (id, fk_aaholder_main, fk_group_names, dtcreat, dtalter)
	VALUES (nextval('group_main_id_seq'), 2, 2, 'now', 'now');
INSERT INTO group_main (id, fk_aaholder_main, fk_group_names, dtcreat, dtalter)
	VALUES (nextval('group_main_id_seq'), 2, 3, 'now', 'now');


/************************/
/* UnizetoAttrSyntax	*/
/************************/

/* SLOWNIKI */
/* przykladowe  typy slotow */
INSERT INTO uniattr_slot_types (id, slottypeoid, description, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_types_id_seq'), '1.2.3.4.5', 'integer', 'now', 'now');
INSERT INTO uniattr_slot_types (id, slottypeoid, description, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_types_id_seq'), '1.2.3.4.6', 'octet string', 'now', 'now');


/*
 * MODUL DEFINICJI SLOTOW
 */
/* przykladowe definicje slotów */
INSERT INTO uniattr_slot_def (id, fk_uniattr_slot_types, noofvalues, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_def_id_seq'), 1, 4, 'now', 'now');
INSERT INTO uniattr_slot_def (id, fk_uniattr_slot_types, noofvalues, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_def_id_seq'), 2, 1, 'now', 'now');

/* przykladowy OID skladni atrybutu */
INSERT INTO uniattr_syntax_oid (id, syntaxoid, description, noofslots, dtcreat, dtalter)
	VALUES (nextval('uniattr_syntax_oid_id_seq'), '1.2.3.4.5', 'Pelnomocnictwo', 2, 'now','now');


/* Zbior 2 slotów to cały UnizetoAttrSyntax - w tabeli ponizej dołączymy jeszcze OID i liczbe slotów */
INSERT INTO uniattr_slot_schema_main (id, fk_uniattr_syntax_oid, fk_uniattr_slot_def, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_schema_main_id_seq'), 1, 1, 'now', 'now');

INSERT INTO uniattr_slot_schema_main (id, fk_uniattr_syntax_oid, fk_uniattr_slot_def, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_schema_main_id_seq'), 1, 2, 'now', 'now');



/*
 * MODUL DANYCH 
 */
/* przykladowy pusty kontener Unizeto Attr Syntax dla bbanan */
INSERT INTO uniattr_main (id, fk_aaholder_main, fk_uniattr_syntax_oid, dtcreat, dtalter)
	VALUES (nextval('uniattr_main_id_seq'), 1, 1, 'now', 'now');
INSERT INTO uniattr_main (id, fk_aaholder_main, fk_uniattr_syntax_oid, dtcreat, dtalter)
	VALUES (nextval('uniattr_main_id_seq'), 2, 1, 'now', 'now');


/* przykladowy pusty slot dla UnizetoAttrSyntax */
INSERT INTO uniattr_slot (id, fk_uniattr_main, fk_uniattr_slot_types, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_id_seq'), 1, 1, 'now', 'now');
INSERT INTO uniattr_slot (id, fk_uniattr_main, fk_uniattr_slot_types, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_id_seq'), 1, 2, 'now', 'now');

INSERT INTO uniattr_slot (id, fk_uniattr_main, fk_uniattr_slot_types, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_id_seq'), 2, 1, 'now', 'now');
INSERT INTO uniattr_slot (id, fk_uniattr_main, fk_uniattr_slot_types, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_id_seq'), 2, 2, 'now', 'now');

/* przykladowy integer dodany do slotu */
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 1, 123, 'now', 'now');
/* przykladowy string dodany do slotu */
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 2, 'atrybut-string-1', 'now', 'now');

INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 3, 123, 'now', 'now');
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 3, 456, 'now', 'now');
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 3, 789, 'now', 'now');

INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 4, 'atrybut-string-2', 'now', 'now');
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 4, 'atrybut-string-3', 'now', 'now');
INSERT INTO uniattr_slot_values (id, fk_uniattr_slot, value, dtcreat, dtalter)
	VALUES (nextval('uniattr_slot_values_id_seq'), 4, 'atrybut-string-4', 'now', 'now');

