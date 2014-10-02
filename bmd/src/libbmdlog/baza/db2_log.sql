CREATE SEQUENCE log_id_seq INCREMENT BY 1 MINVALUE 1;
--TODO: zamienić duże varchar na long varchar
CREATE TABLE log 
(
			   id integer PRIMARY KEY NOT NULL,
		  remote_host varchar(32) NOT NULL,
		  remote_port varchar(16) NOT NULL,
	      date_time_begin timestamp,
		   remote_pid varchar(16) NOT NULL,

		     src_file varchar(32),
		     src_line varchar(32),
		 src_function varchar(32),

		    log_level varchar(32) NOT NULL,
		    log_owner varchar(32) NOT NULL,
		  log_referto varchar(32),
		      service varchar(32) NOT NULL,
		date_time_end timestamp NOT NULL,
	       operation_type varchar(32) NOT NULL,
		   log_string xml,
		   log_reason varchar(64),
		 log_solution varchar(64),
        	document_size varchar(32),
            document_filename varchar(32),
		   event_code varchar(32),
      log_owner_event_visible varchar(32),
    log_referto_event_visible varchar(32),
		   user_data1 varchar(8),
		   user_data2 varchar(8),
		   user_data3 varchar(8),
		   user_data4 varchar(8),
		   user_data5 varchar(8),
	  db_date_time_commit timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,
	  verification_status integer,
       verification_date_time varchar(128),
		   hash_value varchar(512)
);

CREATE UNIQUE INDEX log_id_idx                     ON log(id);
CREATE UNIQUE INDEX db_date_time_commit_idx        ON log(db_date_time_commit);
CREATE INDEX        remote_host_idx                ON log(remote_host);
CREATE INDEX        remote_port_idx                ON log(remote_port);
CREATE INDEX        date_time_begin_idx            ON log(date_time_begin);
CREATE INDEX        remote_pid_idx                 ON log(remote_pid);
CREATE INDEX        src_file_idx                   ON log(src_file);
CREATE INDEX        src_line_idx                   ON log(src_line);
CREATE INDEX        src_function_idx               ON log(src_function);
CREATE INDEX        log_level_idx                  ON log(log_level);
CREATE INDEX        log_owner_idx                  ON log(log_owner);
CREATE INDEX	    log_referto_idx		   ON log(log_referto);
CREATE INDEX	    event_code_idx		   ON log(event_code);
CREATE INDEX	    log_owner_event_visible_idx	   ON log(log_owner_event_visible);
CREATE INDEX	    log_referto_event_visible	   ON log(log_referto_event_visible);
CREATE INDEX        service_idx                    ON log(service);
CREATE INDEX        date_time_end_idx              ON log(date_time_end);
CREATE INDEX        operation_type_idx             ON log(operation_type);
CREATE INDEX        log_string_idx                 ON log(log_string);
CREATE INDEX        log_reason_idx                 ON log(log_reason);
CREATE INDEX        log_solution_idx               ON log(log_solution);
CREATE INDEX        document_size_idx              ON log(document_size);
CREATE INDEX        document_filename_idx          ON log(document_filename);
CREATE INDEX        log_verification_status_idx    ON log(verification_status);
CREATE INDEX        log_verification_date_time_idx ON log(verification_date_time);


CREATE SEQUENCE hash_id_seq INCREMENT BY 1 MINVALUE 1;

CREATE TABLE hash 
(
                     id integer PRIMARY KEY NOT NULL,
             hash_value varchar(512) NOT NULL,
    verification_status integer,
 verification_date_time varchar(128),
              sig_value bigint
);

CREATE UNIQUE INDEX hash_id_idx                     ON hash(id);
CREATE INDEX        hash_verification_status_idx    ON hash(verification_status);
CREATE INDEX        hash_verification_date_time_idx ON hash(verification_date_time);


CREATE TABLE assoc 
(
       id integer REFERENCES hash(id) NOT NULL,
  log_src integer REFERENCES log(id),
 hash_src integer REFERENCES hash(id)
);
--SELECT NAME, IID, TBNAME FROM SYSIBM.SYSINDEXES WHERE TBNAME= 'assoc'
CREATE INDEX         assoc_id_idx       ON assoc(id);
CREATE INDEX  		 assoc_log_src_idx  ON assoc(log_src);
CREATE INDEX  		 assoc_hash_src_idx ON assoc(hash_src);


CREATE TABLE KODY_PODSYSTEM (
		KODY_PODSYSTEM_ID INTEGER NOT NULL GENERATED ALWAYS AS IDENTITY ( START WITH 1 INCREMENT BY 1 MINVALUE 1 MAXVALUE 2147483647 NO CYCLE CACHE 20 NO ORDER ), 
		KOD VARCHAR(3) NOT NULL, 
		NAZWA VARCHAR(255) NOT NULL
	)
DATA CAPTURE NONE 
IN USERSPACE1
COMPRESS NO;

ALTER TABLE KODY_PODSYSTEM ADD CONSTRAINT SQL140825182947400 PRIMARY KEY (KODY_PODSYSTEM_ID);

ALTER TABLE KODY_PODSYSTEM ADD CONSTRAINT SQL140825182947550 UNIQUE (KOD);

INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('AIP','Aplikacja Innych Podmiot�w');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('APL',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('ASR',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('AUA','Aplikacja Us�ugodawc�w i Aptek');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('HDM','Hurtownia Danych - Monitorowanie');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('IKP','Internetowe Konto Pacjenta');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('PPD',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('PPP',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('RDM',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAA','System Adminstracji - Administracja');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAU','System Administracji - Audyt');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAZ','System Administracji - Zabezpieczenia i Prywatno��');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGO','System Gromadzenia Danych Medycznych - Osobista Dokumentacja Medyczna');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGR','System Gromadzenia Danych Medycznych - Recepty');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGS','System Gromadzenia Danych Medycznych - Skierowania i Zwolnienia');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGZ','System Gromadzenia Danych Medycznych - Zdarzenia Medyczne');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SUS',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SRR',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SRS',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SWD',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SWN','System Wykrywania Nadu�y�');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('WRL',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('WRU',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('ZDP',' ');



CREATE SEQUENCE events_table_behavior_id_seq INCREMENT BY 1 MINVALUE 1;

CREATE TABLE events_table_behavior 
(
            id integer PRIMARY KEY NOT NULL,
   events_info varchar(256) NOT NULL,
       q_fatal integer,
       v_fatal integer,
       q_deny  integer,
       v_deny  integer,
       q_done  integer,
       v_done  integer,
       q_try   integer,
       v_try   integer,
       q_debug integer,
       v_debug integer
);

--CREATE INDEX evnets_table_behavior_id_idx ON events_table_behavior(id);


INSERT INTO events_table_behavior values(0,'Błędy połączenia sieciowego',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(1,'Błąd zbindowania gniazda',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(2,'Błąd rozpoczęcia działania serwera',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(3,'Błąd uruchomienia pluginu',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(4,'Błąd zalogowania',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(5,'Błąd obsłużenia żądania',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(6,'Dowolne błędy zwracane przez funkcje',0,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(7,'Błąd obsługi datagramu',0,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(8,'Brak dostępu na poziomie logowania',0,0,1,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(9,'Niepoprawne dane',0,0,0,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(10,'Błąd weryfikacji ścieżki certyfikatów',0,0,0,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(11,'Poprawne zalogowanie',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(12,'Poprawne obsłużenie żądania',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(13,'Poprawne wykonanie działania pluginu',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(14,'Poprawne rozpoczęcie działania serwera',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(15,'Poprawne obsłużenie datagramu',0,0,0,0,0,1,0,0,0,0);
INSERT INTO events_table_behavior values(16,'Rozpoczęcie działania serwera',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(17,'Próba rozpoczęcia działania pluginu',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(18,'Próba obsłużenia żądania',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(19, 'Próba zalogowania do BMD',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(20,'Próba obsłużenia datagramu',0,0,0,0,0,0,0,1,0,0);
INSERT INTO events_table_behavior values(21,'Informowanie o rozpoczęciu działania funkcji',0,0,0,0,0,0,0,0,1,1);
INSERT INTO events_table_behavior values(22,'Zapytanie wysyłane do bazy danych',0,0,0,0,0,0,0,0,1,1);


