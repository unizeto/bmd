#include "logH.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>
#include <conf_sections.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_soap_configuration(bmd_conf **konfiguracja)
{
char *error_string=NULL;
char *conf_file=NULL;
int status;

	PRINT_DEBUG("SOAPLOGCLIENTINF Loading log client configuration\n");
	status=bmdconf_init(konfiguracja);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("SOAPLOGCLIENTERR Error while initializing configuration\n");
		return -1;
	}

	status=bmdconf_add_section2dict(*konfiguracja,"soap",ws_DICT_SOAP,BMDCONF_SECTION_REQ);
	if(status!=BMD_OK)
	{
		PRINT_VDEBUG("SOAPLOGCLIENTERR Error while section to dictionary\n");
		return -1;
	}

	status=bmdconf_load_file("soap_client.conf",*konfiguracja,&error_string);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("SOAPLOGCLIENTERR Error while loading configuration file %s\n",conf_file);
		return -1;
	}

	return BMD_OK;
}

void configuration(bmd_conf *konfiguracja, char **host)
{
	int status=0;
	status=bmdconf_get_value(konfiguracja,"soap","host",host);

	PRINT_DEBUG("SOAPLOGSRVINF configuration: host %s\n",*host);
}

int connection(struct soap *soap)
{

    soap_init2(soap,SOAP_C_UTFSTRING,SOAP_C_UTFSTRING);

    return 0;
}

