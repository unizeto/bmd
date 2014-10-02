#ifndef _NTLM_INCLUDED_
#define _NTLM_INCLUDED_
#ifdef WIN32
#pragma warning(disable:4005)
#include <Security.h>
#include <sspi.h>

#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmdutils/libbmdutils.h>

typedef struct ntlm_auth {
	bmdnet_handler_ptr *handler;		/* handler sieciowy do komunikacji z wybranym hostem */
	char *host;
	long port;
	char *base_http_request;		/* request, ktory idzie do serwera, a musi podlegac autoryzacji */
	char *header_auth;				/* naglowek ktory okresla autoryzacje */
	
	char *ntlm_internal_buf;		/* bufor do wewnetrznych operacji przy autoryzacji NTLM */
	HMODULE hSecDll;				/* uchwyt do biblioteki */
	PSecurityFunctionTableA pSecFn;	/* tablica funkcji z powyzszej biblioteki */

	CredHandle initial_cred_handle;	/* uchwyt do inicjujacych danych poufnych */
	CredHandle next_cred_handle;	/* uchwyt do danych otrzymanych w procesie autoryzacji NTLM */

	TimeStamp tsDummy;

	/* bufory do kolejnych etapow autoryzacji NTLM */
	SecBuffer ntlm_1;
	SecBufferDesc ntlm_desc_1;
	SecBuffer ntlm_2;
	SecBufferDesc ntlm_desc_2;
	SecBuffer ntlm_3;
	SecBufferDesc ntlm_desc_3;
	
	long ntlm_auth_result;				/* wynik autoryzacji NTLM */
	long ntlm_current_stage;				/* aktualny stadium autoryzacji NTLM */

	char **internal_buffers;			/* wewnetrzne bufory do autoryzacji NTLM */
	char *temp_buffer;					/* tymczasowy bufor */
	LONG attrs;
	char *auth_header_resp;
	char *connection_accepted_string;	/* lancuch wskazujacy na poprawna odpowiedz serwera */
} ntlm_auth_t;

long __write_to_file(char *name,char *buf,long length);

/* inicjalizuje autoryzacje NTLM */
long ntlm_auth_init(ntlm_auth_t *auth);

/* wykonuje proces autoryzacji NTLM */
long ntlm_auth_execute(ntlm_auth_t *auth);

/* czysci po autoryzacji NTLM */
long ntlm_auth_cleanup(ntlm_auth_t *auth);

/* ustawia dane potrzebne do autoryzacji */
long ntlm_auth_set(char *host,long port,char *base_http_request,char *header_auth,bmdnet_handler_ptr *handler,char *auth_header_resp,
				  char *connection_accepted_string,
				  ntlm_auth_t *auth);

long bmd_sspi_init(ntlm_auth_t *auth);

long bmd_sspi_acquire_method(ntlm_auth_t *auth,char *method);

#endif
#endif

