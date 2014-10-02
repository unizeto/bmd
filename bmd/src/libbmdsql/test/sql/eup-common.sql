/*  !!!!!!!!!!!!!!!!!!!!!!!!!!! Nie mieszac z e-FAKTURA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* dodanie domyslnych wartosci dla kategorii bezpieczenstwa */
INSERT INTO groups (id,type,description,sql_cast_string) 
	VALUES(nextval('groups_id_seq'),'1.3.6.1.5.5.7.10.4.0','undefined', 'undefined');
INSERT INTO sec_levels (id,type,description,sql_cast_string) 
	VALUES(nextval('sec_levels_id_seq'),'2.5.1.5.55.0','unmarked', 'undefined');

/* dodanie metadanych specyficznych dla ESP */
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.30','Imie petenta', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.31','Nazwisko petenta', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.32','PESEL', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.33','Adres zameldowania', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.34','E-mail', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.35','Numer dziennika', 'varchar');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.36','Data przyjecia', 'timestamp');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.37','Data wyslania awiza', 'timestamp');
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.38','Urzedowe potwierdzenie', 'varchar');

/* dodanie metadanej stanowiacej unikalny identyfikator w bazie - numer pozostal ten sam z e-FAKTURY dla wygody */
INSERT INTO add_metadata_types (id,type,description,sql_cast_string) 
		VALUES (nextval('add_metadata_types_id_seq'),'1.2.616.113527.4.3.3.23','E-invoice hash', 'varchar');

