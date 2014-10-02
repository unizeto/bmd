/*****************************************
 *  Ustawienia konfiguracyjne            *
 *                                       *
 * Data : 20-05-2008                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdarchpack/config.h>
#include <bmd/libbmdarchpack/config.dic>

/**
* Funkcja pobiera parametry z pliku konfiguracyjnego
*
* @param[in] confFile - ścieżka do pliku konfiguracyjnego
* @param[in,out] conFStruct - uchwyt do struktury
                               przechowywującej parametry
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błędy związane z obsługą słownika
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_OK - zakończenie pomyślne
*
*/

long getArchPackConfiguration( const char* const confFile, Config_t* const conFStruct )
{


   /* ------------------------------------------ */

      bmd_conf*         bmdconfHandle    = NULL;
      char*             stringKomunikat  = NULL;
      char*             value            = NULL;
      long              longRet          =    0;

   /* ------------------------------------------ */



      if ( confFile == NULL )
       {
         PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
       }

      if ( conFStruct == NULL )
       {
         PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
       }

      longRet = bmdconf_init( &bmdconfHandle );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd inicjowania pliku konfiguracyjengo. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = bmdconf_set_mode( bmdconfHandle, BMDCONF_TOLERANT);

      if ( longRet < 0 )
       {
         PRINT_ERROR("Bład zmiany ustawien odczytu słownika. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = bmdconf_add_section2dict( bmdconfHandle, "ARCHPACK", config_dic, BMDCONF_SECTION_REQ );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd przy dodawaniu sekcji do słownika. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = bmdconf_load_file(confFile, bmdconfHandle, &stringKomunikat);

      if ( longRet < 0 )
       {
         PRINT_ERROR("%s. Error = %d\n", stringKomunikat, BMD_ERR_OP_FAILED);
         free0( stringKomunikat );
         return BMD_ERR_OP_FAILED;
       }

      free0( stringKomunikat );

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "REPOSITORY", &(conFStruct->repository) );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu katalogu docelowego na paczkę archiwalną. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "TMP_DIR", &(conFStruct->tmp_dir) );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu nazwy katalogu tymczasowego paczki archiwalnej. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "IDENTYFIKATOR", &value );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu OIDu identyfikatora. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->identyfikator).table), \
                                      &((conFStruct->identyfikator).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "TWORCA", &value );

       if ( longRet < 0 )
        {
           PRINT_ERROR("Błąd odczytu OIDu tworcy. Error = %d\n", BMD_ERR_OP_FAILED);
           return BMD_ERR_OP_FAILED;
        }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->tworca).table), \
                                     &((conFStruct->tworca).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "TYTUL", &value );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu OIDu tytulu. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->tytul).table), \
                                     &((conFStruct->tytul).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "DATA", &value );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu OIDu daty. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }


      longRet = str_of_OID_to_ulong( value, &((conFStruct->data).table), \
                                     &((conFStruct->data).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "TYP", &value );

      if ( longRet < 0 )
       {
         PRINT_ERROR("Błąd odczytu OIDu typu. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
       }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->typ).table), \
                                     &((conFStruct->typ).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "FORMAT", &value );

      if ( longRet < 0 )
       {
          PRINT_ERROR("Błąd odczytu OIDu formatu. Error = %d\n", BMD_ERR_OP_FAILED);
          return BMD_ERR_OP_FAILED;
       }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->format).table), \
                                     &((conFStruct->format).size));

      free0(value);

      longRet = bmdconf_get_value( bmdconfHandle, "ARCHPACK", "DOSTEP", &value );

      if ( longRet < 0 )
       {
          PRINT_ERROR("Błąd odczytu OIDu dostepu. Error = %d\n", BMD_ERR_OP_FAILED);
          return BMD_ERR_OP_FAILED;
       }

      longRet = str_of_OID_to_ulong( value, &((conFStruct->dostep).table), \
                                     &((conFStruct->dostep).size));

      free0(value);

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "ODBIORCA", &value );

      longRet = str_of_OID_to_ulong( value, &((conFStruct->odbiorca).table), \
                                     &((conFStruct->odbiorca).size));
      free0(value);

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "RELACJA", &value );

      longRet = str_of_OID_to_ulong( value, &((conFStruct->relacja).table), \
                                     &((conFStruct->relacja).size));
      free0(value);

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "GRUPOWANIE", &value );

      if ( value != NULL )
       {
          longRet = str_of_OID_to_ulong( value, &((conFStruct->grupowanie).table), \
                                         &((conFStruct->grupowanie).size));
          free0(value);
       }

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "KWALIFIKACJA", &value );

      if ( value != NULL )
       {
         longRet = str_of_OID_to_ulong( value, &((conFStruct->kwalifikacja).table), \
                                        &((conFStruct->kwalifikacja).size));
         free0(value);
       }

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "JEZYK", &value );

      if ( value != NULL )
       {
         longRet = str_of_OID_to_ulong( value, &((conFStruct->jezyk).table), \
                                       &((conFStruct->jezyk).size));
         free0(value);
       }

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "OPIS", &value );

      if ( value != NULL )
       {
         longRet = str_of_OID_to_ulong( value, &((conFStruct->opis).table), \
                                    &((conFStruct->opis).size));
         free0(value);
       }

      bmdconf_get_value( bmdconfHandle, "ARCHPACK", "UPRAWNIENIA", &value );

      if ( value != NULL )
       {
         longRet = str_of_OID_to_ulong( value, &((conFStruct->uprawnienia).table), \
                                         &((conFStruct->uprawnienia).size));
         free0(value);
       }

      bmdconf_destroy( &bmdconfHandle );

   return BMD_OK;

}

