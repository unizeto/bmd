#include <bmd/libbmdsoap/libbmdsoap.h>

/************************/
/*	skopiowanie pliku	*/
/************************/
long fileCopy(	char *source,
			char *destination)
{
GenBuf_t *gb	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (source==NULL)
	{
		return -1;
	}
	if (destination==NULL)
	{
		return -2;
	}

	bmd_load_binary_content(source,&gb);
	bmd_save_buf(gb, destination);

	/******************/
	/*	porzadki	*/
	/******************/
	free_gen_buf(&gb);

	return BMD_OK;
}

/******************************/
/*	przesuniecie pliku	*/
/******************************/
long fileMove(	char *source,
			char *destination)
{
GenBuf_t *gb	= NULL;
int status		= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (source==NULL)
	{
		return -1;
	}
	if (destination==NULL)
	{
		return -2;
	}

		status=bmd_load_binary_content(source,&gb);
	if (status<0)
	{
		return status;
	}
	status=bmd_save_buf(gb, destination);
	if (status<0)
	{
		return status;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	free_gen_buf(&gb);
	unlink(source);

	return BMD_OK;
}


/******************************/
/*	Główna funkcja serwera	*/
/******************************/
int main(int argc, char **argv)
{
int status			= 0;
char dir_in[]		= {"/mnt/press/in/P4EFFAKTURA000001.xml"};
char dir_out[]		= {"/mnt/press/out/a%abcde01%1787637%2008\$05\$03.pdf"};
char file_in[]		= {"/home/Lesioo/source2.xml"};
char *source		= NULL;
char *destination		= NULL;
char *com			= NULL;
int timeout			= 10000;
int i				= 0;

	/************************************************/
	/*	skopiowanie pliku do katalogu wejściowego	*/
	/************************************************/
	printf("copying input file... ");
	status=fileCopy(file_in,dir_in);
	if (status<0)
	{
		printf("\tFAILED (error in copying %s to %s)\n",file_in,dir_in);
		return -1;
	}
	printf("\tOK\n");

	printf("copying output file... ");
	while(1)
	{
		i++;
		status=fileMove(dir_out,"/home/Lesioo/a%abcde01%1787637%2008\$05\$03.pdf");
		if (status==0)
		{
			break;
		}

		if (i>=timeout)
		{
			printf("\tFAILED (timeout)\n");
		}
		sleep(1);
	}
	if (status==0)
	{
		printf("\tOK (time %i)\n",i);
	}

	return 0;
}
