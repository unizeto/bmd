#include <stdio.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdarchpack/config.h>
#include <bmd/libbmdarchpack/gen_metadata.h>
#include <bmd/libbmdarchpack/libbmdarchpack.h>
#include <bmd/libbmdzlib/libbmdzlib.h>


/**
*/

// "/etc/bmd/made_1.7.2/conf/archpack.conf"

long makeArchPack( const char* const confPath, \
		   bmdDatagram_t* const bmdDatagram_tParam, \
                   GenBuf_t* const bufor, \
                   transmission_t transmission, \
                   char** const archPackDir, \
                   char** const archPackFile )

{

    /* ---------------------------------------------- */

       long                  longRet          =    0;
       static int            fileout          =    0;
       static metadata_t*    metadane         = NULL;
       char*                 fileName         = NULL;
       char*                 archPath         = NULL;
       char*		     archPackPath     = NULL;
       char*                 fileMetadata     = NULL;
       char*                 stringTmp        = NULL;
       char*                 fileCases        = NULL;
       char**                zip_list         = NULL;
       long                  zip_list_size    =    0;
       static Config_t                archPackParams;
       long                  i                =    0;
       char                  curr_dir[256]    =  {0};

    /* ---------------------------------------------- */


    if ( confPath == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if (  bmdDatagram_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    if ( archPackDir == NULL )
     {
         PRINT_ERROR("Nieprawidłowy czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
         return BMD_ERR_PARAM4;
     }

    if ( archPackFile == NULL )
     {
         PRINT_ERROR("Nieprawidłowy piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
         return BMD_ERR_PARAM5;
     }


    switch ( transmission )
    {

      case startblk :
      {

       longRet = getArchPackConfiguration( confPath, &archPackParams );

       if ( longRet != BMD_OK )
        {
           PRINT_ERROR("Błąd odczytu konfiguracji paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

       longRet = composeMetadataStruct( &metadane, &archPackParams, bmdDatagram_tParam );

       if ( longRet != BMD_OK )
        {
           PRINT_ERROR("Błąd przy generowaniu pliku metadanych. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

       longRet = openArchPackFile( &archPackParams, (char*)bmdDatagram_tParam->protocolDataFilename->buf, &fileout);

       if ( longRet != BMD_OK )
        {
           PRINT_ERROR("Błąd tworzenia struktury paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

       asprintf( &fileName, "%s/%s_%d/metadane/%s", archPackParams.repository, \
                 archPackParams.tmp_dir, (int)getpid(), (char*)bmdDatagram_tParam->protocolDataFilename->buf );

       longRet = composeXMLDocument( metadane, fileName );

       if ( longRet != BMD_OK )
        {
           PRINT_ERROR("Błąd błąd zapisu metadanych paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

       clearMetadata( &metadane );

       longRet = addData2ArchpackFile(bufor, fileout);

       if ( longRet != BMD_OK )
        {
           PRINT_ERROR("Błąd zapisu zawartości paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

        break;
      }

      case continueblk :
      {
         longRet = addData2ArchpackFile(bufor, fileout);

         if ( longRet != BMD_OK )
         {
            PRINT_ERROR("Błąd zapisu zawartości paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
            return BMD_ERR_OP_FAILED;
         }

        break;
      }

      case stopblk :
      {

         longRet = addData2ArchpackFile(bufor, fileout);

         if ( longRet != BMD_OK )
          {
              PRINT_ERROR("Błąd zapisu zawartości paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
              return BMD_ERR_OP_FAILED;
          }

         closeArchpackFile( &fileout );

         asprintf( &archPath, "%s/%s_%d", archPackParams.repository, archPackParams.tmp_dir, (int)getpid());


        if (getcwd(curr_dir,255)==NULL)
         {
             PRINT_ERROR("Błąd odczytu bieżącego katalogu. Error = %d\n", BMD_ERR_OP_FAILED);
             return BMD_ERR_OP_FAILED;
         }

        if ( chdir(archPath) != 0 )
         {
             PRINT_ERROR("Błąd przy zmianie katalogu na tymczasowy. Error = %d\n", BMD_ERR_OP_FAILED);
             return BMD_ERR_OP_FAILED;
         }

         asprintf( &fileName, "dokumenty/%s",(char*)bmdDatagram_tParam->protocolDataFilename->buf );
         asprintf( &fileMetadata, "metadane/%s.xml",(char*)bmdDatagram_tParam->protocolDataFilename->buf );
         asprintf( &fileCases, "sprawy/" );

         /* ---------------- pobrany plik ------------------ */
         zip_list_size++;
         zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);
         zip_list[zip_list_size-1]=(char*)malloc(strlen(fileName)+2);
         memset(zip_list[zip_list_size-1], 0, strlen(fileName)+1);
         memcpy(zip_list[zip_list_size-1], fileName, strlen(fileName));

         /* ---------------- metadane ------------------ */
         zip_list_size++;
         zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);
         zip_list[zip_list_size-1]=(char*)malloc(strlen(fileMetadata)+2);
         memset(zip_list[zip_list_size-1], 0, strlen(fileMetadata)+1);
         memcpy(zip_list[zip_list_size-1], fileMetadata, strlen(fileMetadata));

         zip_list_size++;
	 zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);
	 zip_list[zip_list_size-1]=(char*)malloc(strlen(fileCases)+2);
	 memset(zip_list[zip_list_size-1], 0, strlen(fileCases)+1);
	 memcpy(zip_list[zip_list_size-1], fileCases, strlen(fileCases));

         asprintf(&archPackPath, "%s/%s.zip", curr_dir, (char*)bmdDatagram_tParam->protocolDataFilename->buf);
         asprintf(archPackDir, "%s", curr_dir);
         asprintf(archPackFile, "%s.zip", (char*)bmdDatagram_tParam->protocolDataFilename->buf );

         longRet = zipPackFile(zip_list, zip_list_size, archPackPath);

         /* porzadki */
         unlink(fileName);
         free0(fileName);

         /* porzadki */
         unlink(fileMetadata);
         free0(fileMetadata);

         /* porzadki */
         asprintf( &stringTmp, "%s/dokumenty", archPath);
         rmdir(stringTmp);
         free0(stringTmp);

         /* porzadki */
         asprintf(&stringTmp, "%s/metadane", archPath);
         rmdir(stringTmp);
         free0(stringTmp);

	 /* porzadki */
         asprintf(&stringTmp, "%s/sprawy", archPath);
	 rmdir(stringTmp);
	 free0(stringTmp);

         /* porzadki */
         rmdir(archPath);

         for (i=0; i<zip_list_size; i++)
          {
             free0(zip_list[i]);
          }

         free0(zip_list);

         if ( chdir(curr_dir) != 0 )
         {
             PRINT_ERROR("Błąd przy zmianie katalogu na tymczasowy. Error = %d\n", BMD_ERR_OP_FAILED);
             return BMD_ERR_OP_FAILED;
         }
      }
    }

   return BMD_OK;

}
