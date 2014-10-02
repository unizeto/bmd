#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmderr/libbmderr.h>

#define ADRESAT "tomasz.klimek@unizeto.pl"
#define NADAWCA "tomasz.klimek@unizeto.pl"

extern long _GLOBAL_debug_level;

int sprzataj(bmd_crypt_ctx_t **ctx, bmd_smime_msg **message);

int main(int argc, char **argv)
/* ./program od_kogo do_kogo temat certyfikat  */
{
	bmd_smime_msg *message = NULL; /*wiadomość do wysłania*/
	int status = 0; /*kontrola błędów*/
	char *nadawca = NULL;/*mail od kogo wiadomosc*/
	char *adresat = NULL;/*mail gdzie slac wiadomosc*/
	char *temat = NULL;/*temat wysylanej wiadomosci*/
	char *pfx_file = NULL;/*ścieżka do pliku z certyfikatem*/
	char *pfx_pin = NULL; /* pin do podpisu*/
	GenBuf_t *tresc = NULL;/*genbuf z trescia wysylanej wiadomosci*/
	char *tresc_txt = NULL;
	bmd_crypt_ctx_t *ctx = NULL;/*kontekst do podpisania wiadomości*/
	mail_send_info_t *mail_info = NULL;/*strukturka do libbmdmail*/
	char *login;
	char *password;

	_GLOBAL_debug_level = 15;
	bmd_init();

	status = bmd_smime_create(&message);
	if (status != 0)
	{
		printf("Nie można utworzyć wiadomości. Brak pamięci?\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	nadawca = strdup(NADAWCA);
	adresat = strdup(ADRESAT);
	temat = strdup("tusiotest");


	status = bmd_smime_set_header(message, nadawca, adresat, temat);
	if (status != 0)
	{
		printf("Nie można dodać nagłówka.\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	tresc_txt = strdup("<html><body><h1>Ala ma kota</h1><p>Przykładowy paragraf. A tutaj <a href=\"www.wp.pl\">hiperłącze</a>.</p></body></html>");
	status = set_gen_buf2((unsigned char*) tresc_txt, strlen(tresc_txt), &tresc);
	if (status != 0)
	{
		printf("Nie można wygenerować treści wiadomości.\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	status = bmd_smime_set_body(message, tresc, BODY_TYPE_TEXT_HTML, BODY_CHARSET_UTF8);
	if (status != 0)
	{
		printf("Nie można dodać treści wiadomości.\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	#ifdef WIN32
		status = bmd_smime_add_attachment(message, "..\\..\\tests\\zalacznik.txt", BMD_SMIME_HIDDEN_ATTACHMENT);
	#else
		status = bmd_smime_add_attachment(message, "zalacznik.txt", BMD_SMIME_HIDDEN_ATTACHMENT);
	#endif

	if (status != 0)
	{
		printf("Nie można dodać załącznika.\n");
		sprzataj(&ctx, &message);
		return 1;
	}
/*
	#ifdef WIN32
		pfx_file = strdup("..\\..\\tests\\marua.pfx");
	#else
		pfx_file = strdup("marua.pfx");
	#endif
	pfx_pin = strdup("12345678");

	status = bmd_set_ctx_file(pfx_file, pfx_pin, strlen(pfx_pin), &ctx);
	if (status != 0)
	{
		printf("Nie można ustawić kontekstu do podpisania wiadomości..\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	status = bmd_smime_sign(message, ctx);
	if (status != 0)
	{
		printf("Nie można podpisać wiadomości.\n");
		sprzataj(&ctx, &message);
		return 1;
	}
*/

	status = bmd_smime_noSign(message);
	if (status != 0)
	{
		printf("Nie można podpisać wiadomości.\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	mail_info = (mail_send_info_t*) calloc (1, sizeof(mail_send_info_t));
	if (mail_info == NULL)
	{
		printf("Za mało pamięci!\n");
		sprzataj(&ctx, &message);
		return 1;
	}

	mail_info->recipient_to=strdup(ADRESAT);
	mail_info->reverse_path = strdup(NADAWCA);
	//mail_info->server_port=strdup("195.205.248.66:25");
	mail_info->server_port=strdup("127.0.0.1:25");
	mail_info->with_auth=1;

	/*login = strdup("test_esp");*/
	login = strdup(NADAWCA);
//	password = strdup("12345678");

	/*
	printf("\npodaj login do skrzynki unet: ");
	fgets(login, 256, stdin);
	printf("podaj haslo do skrzynki unet: ");
	fgets(password, 256, stdin);*/

	mail_info->auth_data.username=strdup(login);
	mail_info->auth_data.password=strdup(password);


	status = bmd_smime_send(message, mail_info);
	if (status != 0)
	{
		printf("Nie można wysłać wiadomości, wynik: %i\n", status);
		sprzataj(&ctx, &message);
		return 1;
	}

	sprzataj(&ctx, &message);
	return 0;
}

int sprzataj(bmd_crypt_ctx_t **ctx, bmd_smime_msg **message)
{
	if (ctx != NULL)
		bmd_ctx_destroy(ctx);
	if (message != NULL)
		bmd_smime_destroy(message);
	bmd_end();
	return 0;
}
