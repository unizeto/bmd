/************************************************
 *			                 	*
 * Implementacja obszaru pamięci dzielonej na   *
 * zakolejkowane maile i opcje konfiguracyjne   *
 *			                 	*
 * Data : 07-10-2009                     	*
 * Autor : Tomasz Klimek                 	*
 *			                 	*
 ************************************************/

#ifndef __SHMEM_REGION__
#define __SHMEM_REGION__

#include <semaphore.h>
#include <signal.h>

#define MAX_MSG_SIZE 1048576
#define MAX_MSG_ID_SIZE 512
#define MAX_RANGE 1024

  typedef struct {
 
     /* --------------------------------- */

	char       message[MAX_MSG_SIZE];   /* bufor pamięci dzielonej */
	char  messageID[MAX_MSG_ID_SIZE];
	long                     msgSize;   /* rozmiar komunikatu */

     /* --------------------------------- */

  } message_t;


  typedef struct {

     /* -------------------------------- */

	char	    smtpAddr[MAX_RANGE];	/* adres serwera pocztowego */
	char	    smtpPort[MAX_RANGE];	/* port serwera pocztowego */
	char	    smtpUser[MAX_RANGE];	/* nazwa użytkownika */
	char	    smtpPswd[MAX_RANGE];	/* haslo */
	char          sender[MAX_RANGE];	/* obecny nadawca wiadomosci */
	char	 sms_api_url[MAX_RANGE];	/* adres URL API uslugi wysylki SMS */
	char   sms_api_error[MAX_RANGE];	/* substring zwarty w informacji o bledzie wysylki smsa */
	int		   maxImageSize;	/* maksymalny rozmiar pliku załącznika */
	int		    awizoAction;	/* 1-tylko wysyłka, 2-zapis do eml, 4-zapis do html */
	int		      mailqueue;        /* czy kolejkować e-Awiza (1) - tak, (0) - nie, wysyłka od razu */
	int		   maxcachesize;	/* maksymalna wielkość pamięci cache (ilość slotów) */
	int		   maxmailqueue;	/* maksymalna ilość zakolejkowanych maili */ 
	int		    sendingtime;	/* czas po upływie którego należy uruchomić proces wysyłki eAwiz */
	int	       allowImageAbsent;	/* czy zezwolić na wysyłkę awiza bez załącznika */
	int		    stopSending;  	/* czy kontynuować proces wysyłki eAwiza */

     /* -------------------------------- */

} shConfig_t;

  typedef struct{

     /* ---------------------------------- */

	sem_t                 clientMutex;   /* semafor opuszczany dla klienta do obsługi procesów związanych bezpośrednio z wysyłką. */
	sem_t            awizoActionMutex;   /* semafor opuszczany dla w momencie zmiany stanu aplikacji eAwiza */
	message_t                     buf;   /* bufor pamięci dzielonej */
	shConfig_t	           config;   /* konfiguracja aplikacji e-Awiza */
	volatile sig_atomic_t awizoAction;   /* wartość określająca stan w jakim znajduje się aplikacja eAwiza */

     /* ---------------------------------- */

  } shRegion_t;


/* ------------------------------------------------------------------------------------ */
/* API */

/* ********************************* */
/*  zainicjowanie pamięci dzielonej  */
/* ********************************* */
long createShRegion(char const * const, shRegion_t const * const, shRegion_t** const );

/* ********************************* */
/*    zwolnienie pamięci dzielonej   */
/* ********************************* */
long destroyShRegion( shRegion_t const * const, shRegion_t* const );

/* ------------------------------------------------------------------------------------- */

#endif /* __SHMEM_REGION__ */

