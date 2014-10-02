#ifndef WIN32
#include <bmd/libbmdzlib/libbmdzlib.h>

long filetime(	char *filename,
			tm_zip *tmzip,
			uLong *dt)
{
long status			= 0;
time_t tm_t			= 0;
struct tm* filedate	= NULL;
struct stat s;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (filename==NULL)
	{
		return -1;
	}

	/************************************************/
	/*	pobranie informacji czasowych o pliku	*/
	/************************************************/
	status=stat(filename,&s);
	if (status!=0)
	{
		PRINT_ERROR("LIBBMDZLIBERR Error in getting file's time stst.\n\tError: .\n\tError=%li\n",status);
		return status;
	}
	tm_t = s.st_mtime;

	filedate = localtime(&tm_t);

	tmzip->tm_sec  = filedate->tm_sec;
	tmzip->tm_min  = filedate->tm_min;
	tmzip->tm_hour = filedate->tm_hour;
	tmzip->tm_mday = filedate->tm_mday;
	tmzip->tm_mon  = filedate->tm_mon ;
	tmzip->tm_year = filedate->tm_year;

	return 0;
}


#endif /*ifndef WIN32*/
