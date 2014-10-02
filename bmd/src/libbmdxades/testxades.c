#include <bmd/libbmdxades/libbmdxades.h>
#include <bmd/libbmdglobals/libbmdglogals.h>
#include <stdio.h>

#include <bmd/libbmderr/libbmderr.h>
#define t(x) { if(x != 0) printf(__FILE__":%i failed\n",__LINE__ ); }

int main(int argc, char ** argv)
{
	FILE * f ;
	char buffer[1024*1024] = {0};
	_GLOBAL_debug_level = 2;

	printf("test xades\n");
	t(xades_init());

	f = fopen("formularz.xml","r");
	if(f==NULL)
	{
		PRINT_ERROR("Blad: Nie mozna otworzyc pliku data.xml\n");
	}
	fread(buffer,1,1024*1024,f);
	fclose(f);

#ifndef WIN32
	if (xades_verify (buffer, strlen(buffer)) < 0)
	{
		PRINT_ERROR("Verification result: negative\n");
	}else
	{
		PRINT_DEBUG("Verification result: positive\n");
	}
#endif
	t(xades_destroy());

	return 0;
}

