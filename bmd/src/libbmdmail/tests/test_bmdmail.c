#include <bmd/libbmdmail/libbmdmail.h>
#include "../../bmd_server/plugins/session/StdSessionSOE/StdSessionSOE.h"

//#include <stdlib.h>
//#include <stdio.h>
#include<string.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

int sprawdz_bmd_send_mail_multi( void);

extern long _GLOBAL_debug_level;

int main(int argc, char ** argv)
{
int i=0;
int n=0;
char *recipient=NULL;
char *sender=NULL;
mail_send_info_t *mi;
char **message_body=NULL;
bmd_crypt_ctx_t *smime_ctx=NULL;
char *response=NULL;
char **email_body=NULL;
int status=0;

	system("clear");
	_GLOBAL_debug_level=0;
	if (argc==9)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-n")==0)
			{
				if (argc>i+1) n=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-r")==0)
			{
				if (argc>i+1) asprintf(&recipient,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-a")==0)
			{
				if (argc>i+1) asprintf(&sender,"%s",argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-n liczba\tilość wiadomości\n");
		printf("\t-r e-mail\todbiorca\n");
		printf("\t-a e-mail\tnadawca\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	/********************************/
	/*	alokacja pamięci	*/
	/********************************/
	mi=(mail_send_info_t *)malloc(sizeof(mail_send_info_t)*n+2);
	memset(mi,0,sizeof(mail_send_info_t)*n+1);

	message_body=(char **)malloc(sizeof(char*)*n+2);
	memset(message_body,0,sizeof(char*)*n+1);

	/********************************/
	/*	przygotoanie maili	*/
	/********************************/
	for (i=0; i<n; i++)
	{
		asprintf(&message_body[i],"To jest wiadomości testowa wysyłana w celu sprawdzenia działania wysyłki wielu wiadomosci w jednej sesji. To jest %i. mail",i);
		/*bmd_plugin_prepare_default_p7m_email(
				message_body[i],
				recipient,
				sender,
				"Probny mail z pluginu mail-send",
				smime_ctx,
				&email_body[i]);*/

		asprintf(&(mi[i].server_port),"127.0.0.1:25");
		mi[i].recipient_to=recipient;
		mi[i].with_auth=0;
		mi[i].auth_data.username = "root";
		mi[i].auth_data.password = "12345678";
		mi[i].mail_body=message_body[i];
	}


	/************************/
	/*	wysylanie maili */
	/************************/
	status=bmd_send_envoice_avizo(mi,n,&response);
	if (status<BMD_OK)
	{
		PRINT_ERROR("STDSESSIONERR Error in sending e-mails. Error=%i\n",status);
	}
	else
	{
		PRINT_INFO("%s\n",response);
	}

	/************************/
	/*	porzadki	*/
	/************************/
	free(mi); mi=NULL;

	return 0;
}