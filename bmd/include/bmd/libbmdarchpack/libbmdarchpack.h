/*******************************
 *                             *
 * Węzeł z konfiguracją dla    *
 * paczki archiwalnej          *
 *                             *
 * Data : 04-12-2007           *
 * Autor : Tomasz Klimek       *
 *                             *
 *******************************/

#ifndef _ARCH_PACK_
#define _ARCH_PACK_

#include <bmd/libbmdarchpack/gen_metadata.h>
#include <bmd/libbmdarchpack/config.h>

typedef enum { startblk, continueblk, stopblk } transmission_t;

   /* --------------------------------------------------------------------------------- */
   /* API */

      long openArchPackFile(Config_t*, const char* const, int* );
      long addData2ArchpackFile(GenBuf_t* const, int );
      long closeArchpackFile( int* );

      long makeArchPack( const char* const, bmdDatagram_t* const, \
                         GenBuf_t* const, transmission_t, char** const, char** const );

   /* ---------------------------------------------------------------------------------- */

#endif

