#include <bmd/libbmdzlib/libbmdzlib.h>

/******************************************************************************************/
/*	Kompresja listy plikow do pliku w formacie ZIP (parametry do sciezki do plikow)	*/
/******************************************************************************************/
long zipCompressFile(	char **in,
				long in_count,
				char *out)
{
long status					= 0;
long i						= 0;
long size_read				= 0;
void* buf					= NULL;
FILE *in_ptr				= NULL;
long opt_compress_level			= Z_DEFAULT_COMPRESSION;
zipFile zf;
zip_fileinfo zi;

	PRINT_INFO("LIBBMDZLIBINF Compressing file(s) to %s\n",out);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (in==NULL)
	{
		return ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION;
	}
	for (i=0; i<in_count; i++)
	{
		if (in[i]==NULL)
		{
			return ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION;
		}
	}
	if (out==NULL)
	{
		return ZLIB_DECOMP_NO_OUTPUT_FILE;
	}

	/******************************************/
	/*	ustawienie poziomu konmpresji (1-9)	*/
	/******************************************/
	opt_compress_level = 9;

	/************************************************/
	/*	sprawdzenie, czy pliki wejsciowe istnieja	*/
	/************************************************/
	for (i=0; i<in_count; i++)
	{
		in_ptr=fopen(in[i],"rb");
		if (in_ptr==NULL)
		{
			return ZLIB_COMP_SRC_FILE_OPEN_ERROR;
		}
		else
		{
			fclose(in_ptr);
		}
	}

	buf=(void*)malloc(WRITEBUFFERSIZE);

	/************************************************/
	/*	otwarcie pliku zip do zapisu archiwum	*/
	/************************************************/
	zf=zipOpen(out,0);
	if (zf == NULL)
	{
		return ZLIB_COMP_DST_FILE_OPEN_ERROR;
	}

	/************************************************************/
	/*	dolaczenie wszystkich plikow z listy do archiwum	*/
	/************************************************************/
	for (i=0; i<in_count; i++)
	{
		/******************************************/
		/*	pobranie informacji o czasie pliku	*/
		/******************************************/
		zi.tmz_date.tm_sec = 0;
		zi.tmz_date.tm_min = 0;
		zi.tmz_date.tm_hour = 0;
		zi.tmz_date.tm_mday = 0;
		zi.tmz_date.tm_mon = 0;
		zi.tmz_date.tm_year = 0;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;
		filetime(in[i], &zi.tmz_date, &zi.dosDate);

		/******************************************/
		/*	stworzenie nowego pliku w archiwum	*/
		/******************************************/
		status=zipOpenNewFileInZip(	zf,
							in[i],
							&zi,
                                 		NULL,
							0,
							NULL,
							0,
							NULL,
                                 		(opt_compress_level != 0) ? Z_DEFLATED : 0,
                                 		opt_compress_level);
		if (status!=ZIP_OK)
		{
			continue;
		}

		/************************************************************/
		/*	otwarcie pliku zrodlowego dodawanego do archiwum	*/
		/************************************************************/
		in_ptr=fopen(in[i],"rb");
		if (in_ptr==NULL)
		{
			continue;
		}

		do
		{
			status=ZIP_OK;
			/************************************/
			/*	odczyt z pliku zrodlowego	*/
			/************************************/
			size_read=(int)fread(buf,1,WRITEBUFFERSIZE,in_ptr);
			if (size_read < WRITEBUFFERSIZE)
			{
				if (feof(in_ptr)==0)
				{
					PRINT_ERROR("LIBBMDZLIBERR Error in reading file %s.\n\tError: .\n\tError=%i\n",in[i],-1);
					status=-1;
				}
			}
			/************************************/
			/*	zapis do pliku w archiwum zip	*/
			/************************************/
			if (size_read>0)
			{
				status=zipWriteInFileInZip(zf,buf,size_read);
				if (status<0)
				{
					PRINT_ERROR("LIBBMDZLIBERR Error in writing file %s to archive %s.\n\tError: .\n\tError=%i\n",in[i],out,-1);
				}
			}
		} while ((status==ZIP_OK) && (size_read>0));

		/************************************************/
		/*	zamkniecie deskryptora pliku zrodlowego	*/
		/************************************************/
		fclose(in_ptr);

		if (status<0)
		{
			status=ZIP_ERRNO;
		}
		else
		{
			/******************************************/
			/*	zamkniecie pliku w archiwum zip	*/
			/******************************************/
			status=zipCloseFileInZip(zf);
			if (status!=ZIP_OK)
			{
				PRINT_ERROR("LIBBMDZLIBERR Error in closing file %s in archive.\n\tError: .\n\tError=%i\n",in[i],-1);
			}
		}
	}

	/************************************************************/
	/*	zamkniecie pliku zip calego spakowanego archiwum	*/
	/************************************************************/
	zipClose(zf,NULL);

	/******************/
	/*	porzadki	*/
	/******************/


	free(buf);
	return BMD_OK;
}



/****************************************************************************************************************/
/*	Spakowanie listy plikow do nieskompresowanego pliku w formacie ZIP (parametry do sciezki do plikow)	*/
/****************************************************************************************************************/
long zipPackFile(	char **in,
				long in_count,
				char *out)
{
long status					= 0;
long i						= 0;
long size_read				= 0;
void* buf					= NULL;
FILE *in_ptr				= NULL;
long opt_compress_level			= Z_DEFAULT_COMPRESSION;
zipFile zf;
zip_fileinfo zi;

	PRINT_INFO("LIBBMDZLIBINF Compressing file(s) to %s\n",out);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (in==NULL)
	{
		return ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION;
	}
	for (i=0; i<in_count; i++)
	{
		if (in[i]==NULL)
		{
			return ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION;
		}
	}
	if (out==NULL)
	{
		return ZLIB_DECOMP_NO_OUTPUT_FILE;
	}

	/******************************************/
	/*	ustawienie poziomu konmpresji (1-9)	*/
	/******************************************/
	opt_compress_level = 0;

	/************************************************/
	/*	sprawdzenie, czy pliki wejsciowe istnieja	*/
	/************************************************/
	for (i=0; i<in_count; i++)
	{
		in_ptr=fopen(in[i],"rb");
		if (in_ptr==NULL)
		{
			return ZLIB_COMP_SRC_FILE_OPEN_ERROR;
		}
		else
		{
			fclose(in_ptr);
		}
	}

	buf=(void*)malloc(WRITEBUFFERSIZE);

	/************************************************/
	/*	otwarcie pliku zip do zapisu archiwum	*/
	/************************************************/
	zf=zipOpen(out,0);
	if (zf == NULL)
	{
		return ZLIB_COMP_DST_FILE_OPEN_ERROR;
	}

	/************************************************************/
	/*	dolaczenie wszystkich plikow z listy do archiwum	*/
	/************************************************************/
	for (i=0; i<in_count; i++)
	{
		/******************************************/
		/*	pobranie informacji o czasie pliku	*/
		/******************************************/
		zi.tmz_date.tm_sec = 0;
		zi.tmz_date.tm_min = 0;
		zi.tmz_date.tm_hour = 0;
		zi.tmz_date.tm_mday = 0;
		zi.tmz_date.tm_mon = 0;
		zi.tmz_date.tm_year = 0;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;
		filetime(in[i], &zi.tmz_date, &zi.dosDate);

		/******************************************/
		/*	stworzenie nowego pliku w archiwum	*/
		/******************************************/
		status=zipOpenNewFileInZip(	zf,
							in[i],
							&zi,
                                 		NULL,
							0,
							NULL,
							0,
							NULL,
                                 		(opt_compress_level != 0) ? Z_DEFLATED : 0,
                                 		opt_compress_level);
		if (status!=ZIP_OK)
		{
			continue;
		}

		/************************************************************/
		/*	otwarcie pliku zrodlowego dodawanego do archiwum	*/
		/************************************************************/
		in_ptr=fopen(in[i],"rb");
		if (in_ptr==NULL)
		{
			continue;
		}

		do
		{
			status=ZIP_OK;
			/************************************/
			/*	odczyt z pliku zrodlowego	*/
			/************************************/
			size_read=(int)fread(buf,1,WRITEBUFFERSIZE,in_ptr);
			if (size_read < WRITEBUFFERSIZE)
			{
				if (feof(in_ptr)==0)
				{
					PRINT_ERROR("LIBBMDZLIBERR Error in reading file %s.\n\tError: .\n\tError=%i\n",in[i],-1);
					status=-1;
				}
			}
			/************************************/
			/*	zapis do pliku w archiwum zip	*/
			/************************************/
			if (size_read>0)
			{
				status=zipWriteInFileInZip(zf,buf,size_read);
				if (status<0)
				{
					PRINT_ERROR("LIBBMDZLIBERR Error in writing file %s to archive %s.\n\tError: .\n\tError=%i\n",in[i],out,-1);
				}
			}
		} while ((status==ZIP_OK) && (size_read>0));

		/************************************************/
		/*	zamkniecie deskryptora pliku zrodlowego	*/
		/************************************************/
		fclose(in_ptr);

		if (status<0)
		{
			status=ZIP_ERRNO;
		}
		else
		{
			/******************************************/
			/*	zamkniecie pliku w archiwum zip	*/
			/******************************************/
			status=zipCloseFileInZip(zf);
			if (status!=ZIP_OK)
			{
				PRINT_ERROR("LIBBMDZLIBERR Error in closing file %s in archive.\n\tError: .\n\tError=%i\n",in[i],-1);
			}
		}
	}

	/************************************************************/
	/*	zamkniecie pliku zip calego spakowanego archiwum	*/
	/************************************************************/
	zipClose(zf,NULL);

	/******************/
	/*	porzadki	*/
	/******************/


	free(buf);
	return BMD_OK;
}
















