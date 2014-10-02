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
		   hash_value varchar(1024)
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


