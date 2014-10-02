#include <stdio.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdarchpack/config.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdutils/libbmdutils.h>



/**
*/

long openArchPackFile(Config_t* config, const char* const fileName, int* inputFile)
 {

     /* ----------------------- */

        char* documents = NULL;
        char* document  = NULL;
        char* metadata  = NULL;
        char* cases     = NULL;

     /* ----------------------- */


   if ( config == NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   if ( fileName == NULL )
    {
       PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

   if ( inputFile == NULL )
    {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
    }

   asprintf(&documents, "%s/%s_%d/", config->repository, config->tmp_dir, (int)getpid());
   mkdir(documents, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   free0(documents);

   asprintf(&documents, "%s/%s_%d/dokumenty", config->repository, config->tmp_dir, (int)getpid());
   mkdir(documents, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

   asprintf(&metadata, "%s/%s_%d/metadane", config->repository, config->tmp_dir, (int)getpid());
   mkdir(metadata, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   free0(metadata);

   asprintf(&cases, "%s/%s_%d/sprawy/", config->repository, config->tmp_dir, (int)getpid());
   mkdir(cases, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   free0(cases);

   asprintf(&document, "%s/%s", documents, fileName);
   free0(documents);

   *inputFile = open( document, O_CREAT|O_WRONLY|O_LARGEFILE, 0666);

   if ( *inputFile == -1 )
    {
      PRINT_ERROR("Błąd tworzenia pliku. Error = %d\n", BMD_ERR_OP_FAILED);
      return BMD_ERR_OP_FAILED;
    }

   return BMD_OK;
 }



 /**
 */

long addData2ArchpackFile(GenBuf_t* const bufor, int inputFile)
 {
     /* ------------------- */

        ssize_t nbytes = 0;

     /* ------------------- */

    if ( bufor == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( inputFile < 0 )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    nbytes = write( inputFile, bufor->buf, bufor->size);

    if ( nbytes < 0 )
     {
         PRINT_ERROR("Błąd zapisu do pliku. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
     }

    bufor->size = 0;


   return BMD_OK;
 }



 /**
 */

long closeArchpackFile( int* inputFile )
 {

    if ( inputFile == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    close(*inputFile);

    *inputFile = 0;

   return BMD_OK;
 }
 
