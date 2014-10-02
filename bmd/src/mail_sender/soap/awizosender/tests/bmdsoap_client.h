#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include "soapH.h"
#include "ns.nsmap"
int parseOptions(int argc, char *argv[], char **serverURL, int *ssl_enabled,
					  int *authenticate, char **keyfile, char **keyfile_passwd,
					  char **cacert, char **capath);
int freeOptions(char **keyfile, char **keyfile_passwd, char **cacert, char **capath);
