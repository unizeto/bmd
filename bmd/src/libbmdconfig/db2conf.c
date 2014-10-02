#ifndef WIN32

#include <bmd/libbmdconfig/libbmdconfig.h>

int main(int argc, char *argv[])
{
char *error=NULL;
int ret_status=0;

if(argc<=2)
{
	printf("Niepoprawne wywołanie db2conf\n");
	printf("db2conf  <db_connection_params>  <conf_file_name>\nPrzykładowe wywołanie: db2conf \"host=localhost dbname=testconf\" dump.conf\n");
}
else
{
	if(argc==3)
	{
		ret_status=bmdconf_db2file(argv[1], argv[2], &error);
		if(ret_status==0)
		{
			printf("\nDONE\n\n");
		}
		else
		{
			printf("\nOPERATION FAILED\n\n%s", error);
			printf("DATABASE: '%s'\n", argv[1]);
			printf("FILE: '%s'\n\n", argv[2]);
			free(error);
		}
	}
	else
	{
		printf("Niepoprawne wywołanie db2conf\n");
		printf("db2conf  <db_connection_params>  <conf_file_name>\nPrzykładowe wywołanie: db2conf \"host=localhost dbname=testconf\" dump.conf\n");
	}
}
return 0;

}

#endif /*ifndef WIN32*/
