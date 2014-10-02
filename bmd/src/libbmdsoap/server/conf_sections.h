#ifndef SOAP_SERVER_CONF_H_
#define SOAP_SERVER_CONF_H_

static const char * ws_DICT_BMD[] =
{
	"adr 0",
	"port 0",
	"der 0",
	"pfx 0",
	"pfx_pass 0",
	"bmd_cert 0",
	"#max_datagram_in_set_transmission 0",
	"#serialisation_type 1 \"^0|1|2$\"",
	"#no_of_serialisation_threads 0",
	"#deserialisation_max_memory 0",
	""
};

static const char * ws_DICT_SSL[] =
{
	"enable_ssl 0",
	"enable_ssl_authentication 0",
	"ssl_certificate 0",
	"ssl_certificate_password 0",
	"#ssl_cacert_file 0",
	"#ssl_capath_dir 0",
	""
};

static const char * ws_DICT_TIMESTAMP[] =
{
	"#host 0",
	"#port 0",
	""
};

static const char * ws_DICT_SOAP[] =
{
	"accept_timeout 0",
	"send_timeout 0",
	"recv_timeout 0",
	"max_connections 0",
	"#max_metadata_value_length 0",
	"host 0",
	"port 0",
	"temp_dir 0",
	"certificate_required 1 \"^0|1$\" ",
	"stoptransaction_timeout 0",
	"stream_timeout 0",
	"search_oid1 0",
	"search_oid2 0",
	"#search_oid3 0",
	"#start_transaction_required 1 \"^0|1$\"",
	"#empty_transaction_impossible 1 \"^0|1$\"",
	"#idle_transaction_timeout 0",
	"#clean_orphan_directories 1 \"^0|1$\"",
	"#stop_transaction_check_proc 1 \"^0|1$\"",
	"#proc_dir 0",
	"%#archpack_conf 0",
	"#get_parameter_oid 1 \"^([0-9]+\\.)*[0-9]+$\" ",
	"#get_parameter_update_reason 1 \"^.+$\" ",
	""
};

static const char * ws_DICT_WSSECURITY[] =
{
	"enable_wssecurity 1 \"^0|1$\" ",
	"wsse_ca_path 0 ",
	""
};

static const char * ws_DICT_CSV[] =
{
	"#csv_delimiter 0",
	"#csv_enable_escape 0",
	"#csv_char_to_escape 0",
	"#csv_escape_char 0",
	"#csv_value_surrounding_char 0",
	"#csv_enable_string_escaping 0",
	"#csv_enable_number_escaping 0",
	"#csv_enable_date_escaping 0",
	"#csv_src_delimeter 0",
	"#csv_dst_delimeter 0",
	""
};

static const char * ws_DICT_ZIP_PACKAGE[] =
{
	"zip_signature_directory_name 0",
	"zip_dvcs_directory_name 0",
	"zip_attachments_directory_name 0",
	""
};

static const char * ws_DICT_CONVERT[] =
{
	"#pdf_file_type 0",
	"#xml_file_type 0",
	"#xsl_pattern 0",
	"#xsl_pattern_dictionary 0",
	"#add_pdf_to_zip 0",
	"#add_pdf_to_archive 0",
	"#press_in_dir 0",
	"#press_out_dir 0",
	"#press_in_filename 0",
	"#press_out_filename 0",
	"#press_timeout 0",
	"#press_date_metadata 0",
	"#press_date_separator_in 0",
	"#press_date_separator_out 0",
	""
};

static const char * ws_DICT_FUNCTIONALITIES_BEHAVIOUR[] =
{
	"#getInvoicePack_select_type 0",
	"#getHTMLImage_select_type 0",
	"#verifyInvoice_select_type 0",
	"#verifyInvoiceWithDetails_select_type 0",
	"#countInvoice_select_type 0",
	"#getFile_select_type 0",
	"#SendAdviceWithParameters_select_type 0",
	""
};

#endif /*ESP_CLIENT_CONF_H_*/
