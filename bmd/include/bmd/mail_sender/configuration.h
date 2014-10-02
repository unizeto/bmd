/*****************************************
 *  Ustawienia konfiguracyjne            *
 *                                       *
 * Data : 17-08-2009                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/


#ifndef __CONFIGURATION_T__
#define __CONFIGURATION_T__

#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/mail_sender/queue.h>


typedef struct {

     /* ------------------------------- */

	char*		    name;	/* nazwa serwera BMD */
	char*		    addr;	/* adres serwera BMD */
	char*		    port;	/* port serwera BMD */
	char*		     der;	/* ścieżka do certyfikatu klucza publicznego klienta */
	char*		     pfx;	/* ścieżka do certyfikatu klucza publicznego wraz z kluczem prywatnym klienta */
	char*		pfx_pass;	/* hasło certyfikatu klienta */
	char*		bmd_cert;	/* ścieżka do certyfikatu serwera */

     /* ------------------------------- */


} bmdserver_t;


typedef struct {

     /* ------------------------------- */



	char*                       dbIP;  /* IP serwera bazy danych */
	char*                     dbPort;  /* Port serwera bazy danych */
	char*                     dbName;  /* Nazwa bazy danych */
	char*                     dbUser;  /* Użytkownik bazy */
	char*                 dbPassword;  /* Hasło */
	char*                  dbLibrary;  /* Rodzaj biblioteki obsługi bazy danych */
	void*              dbase_handler;  /* uchwyt do bazy danych */
        mail_send_info_t*     awizoTable;  /* tabela z przechowywanymi wiadomościami email */
	bmd_crypt_ctx_t*             ctx;  /* kontekst do certyfikatu klucza publiczengo do podpisu awiza */
	queue_t 	      bmdservers;  /* lista serwerów bmd lub serwerów e-Awizo*/

     /* ------------------------------- */

} configuration_t;

/* -------------------------------------------------------------------------------------------------- */
/* API */

   long getAwizoConfiguration( bmd_conf* const, const char* const, configuration_t* const );
   long getAwizoBrokerConfiguration(bmd_conf* const, const char* const, configuration_t* const );
   long CreateBmdServerNode(bmdserver_t** const, char* const, char* const, \
				char* const, char* const,char* const, \
				char* const, char* const);


/* --------------------------------------------------------------------------------------------------- */

#endif /* __CONFIGURATION_T__  */

