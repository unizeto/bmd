#ifndef _SOAP_LOGSERVER_CONF_H_
#define _SOAP_LOGSERVER_CONF_H_

static const char* ws_DICT_SOAP[] = {
	"send_timeout 0",
	"recv_timeout 0",
	"max_connections 0",
	"host 0",
	"port 0",
	"db_host 0",
	"db_port 0",
	"db_name 0",
	"db_library 0",
	"db_user 0",
	"db_passwd 0",
	"#bmd_servers 1 \"(^([a-zA-Z0-9_-]+:[a-zA-Z0-9_-]+)+$)|(^([a-zA-Z0-9_-]*)$)\"",
	""
};

#endif /* _SOAP_LOGSERVER_CONF_H_ */
