#include <bmd/libbmdlog/libbmdlog.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


#define BUFF 1024


int main(int argc, char* argv[])
{

   /* ------------------------- */

      int             c =    0;
      int             i =    0;
      char    buf[BUFF] =  {0};
      err_t   err_tRet  =    0;
      log_t*  log_tLog  = NULL;
      char*   confFile  = NULL;

   /* ------------------------- */

//_GLOBAL_debug_level = 30;

   if (argc==3)
    {
     for (i=1; i<argc; i++)
      {
        if (strcmp(argv[i],"-c")==0)
         {
           if (argc>i+1) asprintf(&confFile,"%s", argv[i+1]);
         }
      }
    }
   else
    {
      printf("%s\n",argv[0]);
      printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
      printf("-------------------------------------------------------\n");
      printf("\t-c \tplik konfiguracyjny\n");
      printf("-------------------------------------------------------\n");

      return -1;
    }

  err_tRet = LogInit( confFile, &log_tLog);

  if ( BMD_ERR( err_tRet ))
  {
    BMD_BTERR( err_tRet );
    BMD_FREEERR( err_tRet );
    
    exit(-1);
  }

  while( (c = fgetc(stdin)) != EOF )
   {
     for(i=0; (c != '\n' && c != EOF); i++)
      {
        if ( i > BUFF-2 )
         {
           break;
         }
        buf[i] = c;
        c = fgetc(stdin);
      }

     err_tRet = LogCreate(log_tLog);

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }

//PRIM_PARAM_START=1;

     err_tRet = LogSetParam( log_level, "11:4k", log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }



     err_tRet = LogSetParam( operation_type, "Logowanie w trybie developerskim", log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }

     err_tRet = LogSetParam( log_owner, "ADE", log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }

     err_tRet = LogSetParam( service, "ADE", log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }

     err_tRet = LogSetParam( log_string, buf, log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }
      
    err_tRet = LogSetParam( remote_host, "127.0.0.1", log_tLog );

     if ( BMD_ERR( err_tRet ))
      {
	printf("ERROR: LogSetParam\n");
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }

    err_tRet = LogSetParam( remote_port, "2021", log_tLog );


     if ( BMD_ERR( err_tRet ))
      {
	printf("ERROR: LogSetParam\n");
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }


     err_tRet = LogCommit(log_tLog);
 
      if ( BMD_ERR( err_tRet ))
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }
      
     memset(buf, 0, 1024);
  }

     err_tRet = LogEnd(&log_tLog);

     if ( BMD_ERR( err_tRet )) 
      {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
      }


  return 0;
}

