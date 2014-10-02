#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include "bmd230H.h"
#include "bmd230.nsmap"
#include "../../../include/bmd/libbmdconfig/libbmdconfig.h"

int parseOptions(int argc, char *argv[], char **serverURL, int *ssl_enabled,
					  int *authenticate, char **keyfile, char **keyfile_passwd,
					  char **cacert, char **capath);
int freeOptions(char **keyfile, char **keyfile_passwd, char **cacert, char **capath);

//CommonTest.c
int load_soap_configuration(bmd_conf **konfiguracja);

void configuration(	bmd_conf *konfiguracja,
			char **host,
			char **keyfile,
			char **keyfile_passwd,
			char **keyfile_ssl,
			char **keyfile_passwd_ssl,
			char **cacert,
			char **capath,
			long *ssl_enabled);

int connection(	struct soap *soap,
		int ssl_enabled,
		int authenticate,
		char *keyfile_ssl,
		char *keyfile_passwd_ssl,
		char *cacert,
		char *capath);
