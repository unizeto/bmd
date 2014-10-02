#include <bmd/libbmdzlib/libbmdzlib.h>

extern long _GLOBAL_debug_level;

int main(int argc, char **argv)
{
int status		= 0;
char **file_list	= NULL;

	_GLOBAL_debug_level=20;
	file_list=(char **)malloc(sizeof(char *)*3);

	file_list[0]=(char *)malloc(strlen("Makefile"));
	memset(file_list[0], 0, strlen("Makefile"));
	memcpy(file_list[0], "Makefile", strlen("Makefile"));

	file_list[1]=(char *)malloc(strlen("Makefile.am"));
	memset(file_list[1], 0, strlen("Makefile.am"));
	memcpy(file_list[1], "Makefile.am", strlen("Makefile.am"));

	file_list[2]=(char *)malloc(strlen("Makefile.in"));
	memset(file_list[2], 0, strlen("Makefile.in"));
	memcpy(file_list[2], "Makefile.in", strlen("Makefile.in"));

	status=zipCompressFile(file_list, 3, argv[1]);
	printf("status: %i\n",status);

	return BMD_OK;
}
