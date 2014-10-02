drop index assoc_hash_src_idx;
drop index assoc_log_src_idx;
drop index assoc_id_idx;

drop table assoc;

drop index hash_verification_date_time_idx;
drop index hash_verification_status_idx;
drop index hash_id_idx;

drop table hash;

drop sequence hash_id_seq; 

drop index log_verification_date_time_idx;
drop index log_verification_status_idx;
drop index document_filename_idx;
drop index document_size_idx;
drop index log_solution_idx;
drop index log_reason_idx;
drop index log_string_idx;
drop index operation_type_idx;
drop index date_time_end_idx;
drop index service_idx;
drop index log_referto_event_visible;
drop index log_owner_event_visible_idx;
drop index event_code_idx;
drop index log_referto_idx;
drop index log_owner_idx;
drop index log_level_idx;
drop index src_function_idx;
drop index src_line_idx;
drop index src_file_idx;
drop index remote_pid_idx;
drop index date_time_begin_idx;
drop index remote_port_idx;
drop index remote_host_idx;
drop index db_date_time_commit_idx;
drop index log_id_idx;

drop table log;
 
drop sequence log_id_seq;
