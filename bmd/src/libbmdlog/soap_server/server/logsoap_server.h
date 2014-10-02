#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "logH.h"
#include "log.nsmap"
int parseOptions(int argc, char *argv[], int *ssl_enabled, int *authenticate,
			char **keyfile, char **keyfile_passwd, char **cacert, char **capath);
int freeOptions(char **keyfile, char **keyfile_passwd, char **cacert, char **capath);
