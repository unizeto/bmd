
BEGIN;


CREATE SEQUENCE log_id_seq INCREMENT 1 MINVALUE 1;

CREATE TABLE log 
(
			   id integer PRIMARY KEY,
		  remote_host varchar NOT NULL,
		  remote_port varchar NOT NULL,
	      date_time_begin timestamp without time zone,
		   remote_pid varchar NOT NULL,

		     src_file varchar,
		     src_line varchar,
		 src_function varchar,

		    log_level varchar NOT NULL,
		    log_owner varchar NOT NULL,
		  log_referto varchar,
		      service varchar NOT NULL,
		date_time_end timestamp without time zone NOT NULL,
	       operation_type varchar NOT NULL,
		   log_string varchar,
		   log_reason varchar,
		 log_solution varchar,
        	document_size varchar,
            document_filename varchar,
		   event_code varchar,
      log_owner_event_visible varchar,
    log_referto_event_visible varchar,
		   user_data1 varchar,
		   user_data2 varchar,
		   user_data3 varchar,
		   user_data4 varchar,
		   user_data5 varchar,

	  db_date_time_commit timestamp with time zone NOT NULL DEFAULT now(),
	  verification_status integer,
       verification_date_time varchar,
		   hash_value varchar
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


CREATE SEQUENCE hash_id_seq INCREMENT 1 MINVALUE 1;

CREATE TABLE hash 
(
                     id integer PRIMARY KEY,
             hash_value varchar NOT NULL,
    verification_status integer,
 verification_date_time varchar,
              sig_value oid
);

CREATE UNIQUE INDEX hash_id_idx                     ON hash(id);
CREATE INDEX        hash_verification_status_idx    ON hash(verification_status);
CREATE INDEX        hash_verification_date_time_idx ON hash(verification_date_time);


CREATE TABLE assoc 
(
       id integer REFERENCES hash(id) NOT NULL,
  log_src integer UNIQUE REFERENCES log(id),
 hash_src integer UNIQUE REFERENCES hash(id)
);

CREATE INDEX         assoc_id_idx       ON assoc(id);
CREATE UNIQUE INDEX  assoc_log_src_idx  ON assoc(log_src);
CREATE UNIQUE INDEX  assoc_hash_src_idx ON assoc(hash_src);

CREATE SEQUENCE events_table_behavior_id_seq INCREMENT 1 MINVALUE 1;

CREATE TABLE events_table_behavior 
(
            id integer PRIMARY KEY,
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

CREATE INDEX evnets_table_behavior_id_idx ON events_table_behavior(id);


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


COMMIT;
