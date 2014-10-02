#include "bmd230H.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdsoap/libbmdsoap.h>
#include <conf_sections.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_soap_configuration(bmd_conf **konfiguracja)
{
char *error_string=NULL;
char *conf_file=NULL;
int status;

	PRINT_DEBUG("SOAPCLIENTINF Loading client configuration\n");
	status=bmdconf_init(konfiguracja);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while initializing configuration\n");
		return -1;
	}

	status=bmdconf_add_section2dict(*konfiguracja,"soap",ws_DICT_SOAP,BMDCONF_SECTION_REQ);
	if(status!=BMD_OK)
	{
		PRINT_VDEBUG("SOAPCLIENTERR Error while section to dictionary\n");
		return -1;
	}

	status=bmdconf_load_file("soap_client.conf",*konfiguracja,&error_string);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while loading configuration file %s\n",conf_file);
		return -1;
	}

	return BMD_OK;
}

void configuration(	bmd_conf *konfiguracja,
			char **host,
			char **keyfile,
			char **keyfile_passwd,
			char **keyfile_ssl,
			char **keyfile_passwd_ssl,
			char **cacert,
			char **capath,
			long *ssl_enabled)
{
	int status=0;
	status=bmdconf_get_value(konfiguracja,"soap","host",host);
	status=bmdconf_get_value(konfiguracja,"soap","keyfile",keyfile);
	status=bmdconf_get_value(konfiguracja,"soap","keyfile_ssl",keyfile_ssl);
	status=bmdconf_get_value(konfiguracja,"soap","password_ssl",keyfile_passwd_ssl);
	status=bmdconf_get_value_int(konfiguracja,"soap","ssl",(long int *)ssl_enabled);
	//status=bmdconf_get_value(konfiguracja,"soap","cacert",cacert);
	//status=bmdconf_get_value(configuration,"soap","capath",capath);

	/* komunikaty o ustawieniach serwera */
	printf("configuration: host %s\n",*host);
	printf("configuration: BMD keyfile %s\n",*keyfile);
	printf("configuration: SSL keyfile %s\n",*keyfile_ssl);
	printf("configuration: SSL keyfile's password %s\n",*keyfile_passwd_ssl);
	printf("configuration: SSL enabled %li\n",*ssl_enabled);
	//PRINT_VDEBUG("SOAPSRVINF[%s:%i:%s] konfiguracja: plik zaufanych kluczy %s\n",__FILE__,__LINE__,__FUNCTION__,*cacert);
	//PRINT_VDEBUG("SOAPSRVINF[%s:%i:%s] konfiguracja: ścieżka przechowywania certyfikatów %s\n",__FILE__,__LINE__,__FUNCTION__,*capath);
}

int connection(	struct soap *soap,
		int ssl_enabled,
		int authenticate,
		char *keyfile_ssl,
		char *keyfile_passwd_ssl,
		char *cacert,
		char *capath)
{
int status=0;

	if (ssl_enabled==1)
	{
		soap_ssl_init();
	}
 	soap_init(soap);

	soap_imode(soap, SOAP_C_UTFSTRING);
  	soap_omode(soap, SOAP_C_UTFSTRING);

	soap->fdimereadopen = dime_read_open;
	soap->fdimereadclose = dime_read_close;
	soap->fdimeread = dime_read;

	soap->dime_id_format = "uuid:UNIZETO-%d";
	authenticate=0;

	if (ssl_enabled==1)
	{

// 		printf("\n\nconfiguration: SSL keyfile: %s\n",keyfile_ssl);
// 		printf("configuration: SSL keyfile's password: %s\n",keyfile_passwd_ssl);
// 		printf("configuration: SSL enabled: %i\n",ssl_enabled);

		status=soap_ssl_client_context(	soap,
						SOAP_SSL_NO_AUTHENTICATION,//SOAP_SSL_DEFAULT,
						keyfile_ssl,//"client.pem",//keyfile_ssl,
						keyfile_passwd_ssl,//"password",//keyfile_passwd_ssl,
 						NULL,//"cacert",
						NULL,//"cert_path",
						NULL);
		if (status!=0)
		{
			soap_print_fault(soap, stderr);
			soap_end(soap);
			soap_done(soap);
			printf("status: %i\n", status);
			return status;
		}
	}
	return 0;
}



void *dime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *options)
{
  return handle;
}


size_t dime_read(struct soap *soap, void *handle, char *buf, size_t len)
{
  return fread(buf, 1, len, (FILE*)handle);
}


void dime_read_close(struct soap *soap, void *handle)
{
  fclose((FILE*)handle);
}

