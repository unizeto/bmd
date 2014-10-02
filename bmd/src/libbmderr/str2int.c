#ifndef WIN32
#include <stdio.h>
#include <bmd/libbmderr/libbmderr.h>

int main(int argc, char ** argv)
{
	if (argc!=2)
	{
		printf ("Usage: %s <string>\n",argv[0]);
		return -1;
	}
	printf ("%i\n",bmd_str2int(argv[1]));

	return 0; 
}
#else
#pragma warning(disable:4206) /* nie ostrzegaj o pustych plikach */
#endif
