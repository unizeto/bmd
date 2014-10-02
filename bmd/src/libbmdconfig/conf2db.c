#ifndef WIN32

#include <bmd/libbmdconfig/libbmdconfig.h>

int main(int argc, char *argv[])
{
char *error=NULL;
int ret_status=0;

if(argc<=2)
{
	printf("Niepoprawne wywołanie conf2db\n");
	printf("conf2db  <conf_file_name>  <db_connection_params>\nPrzykładowe wywołanie: conf2db test_file.conf \"host=localhost dbname=testconf\"\n");
}
else
{
	if(argc==3)
	{
		ret_status=bmdconf_file2db(argv[1], argv[2], &error);
		if(ret_status==0)
		{
			printf("\nDONE\n\n");
		}
		else
		{
			printf("\nOPERATION FAILED\n\n%s", error);
			printf("FILE: '%s'\n", argv[1]);
			printf("DATABASE: '%s'\n\n", argv[2]);
			free(error);
		}
	}
	else
	{
		printf("Niepoprawne wywołanie conf2db\n");
		printf("conf2db  <conf_file_name>  <db_connection_params>\nPrzykładowe wywołanie: conf2db test_file.conf \"host=localhost dbname=testconf\"\n");
	}
}
return 0;

}

#endif /*ifndef WIN32*/
