/*****************************************
 *  Ustawienia konfiguracyjne            *
 *                                       *
 * Data : 23-04-2009                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/

#ifndef __CONFIG_ARCH_T__
#define __CONFIG_ARCH_T__

//#include <bmd/libbmdarchpack/list.h>

typedef struct
 {
    /* ------------------------- */

       long*     	  table;
       long                size;

    /* ------------------------- */

 }oid_t;



typedef struct
{

   /* --------------------------------- */

      /*    metadane obowiazkowe     */

      oid_t              identyfikator;  /* OID */
      oid_t                     tworca;  /* OID */
      oid_t                      tytul;  /* OID */
      oid_t                       data;  /* OID */
      oid_t                        typ;  /* OID */
      oid_t                     format;  /* OID */
      oid_t                     dostep;  /* OID */

      /*     metadane dodatkowe      */

      oid_t                   odbiorca;  /* OID */
      oid_t                    relacja;  /* OID */
      oid_t                 grupowanie;  /* OID */
      oid_t               kwalifikacja;  /* OID */
      oid_t                      jezyk;  /* OID */
      oid_t                       opis;  /* OID */
      oid_t                uprawnienia;  /* OID */
      list_t                     other;  /* other metadata */

      char*                 repository;  /* katalog docelowy na paczki */
      char*                    tmp_dir;  /* prefix katalogu tymczasowego */

   /* --------------------------------- */

}Config_t;


   /* ----------------------------------------------------------------------------------- */
   /* API */

      long getArchPackConfiguration( const char* const, Config_t* const );

   /* ----------------------------------------------------------------------------------- */

#endif

