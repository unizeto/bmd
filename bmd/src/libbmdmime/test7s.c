#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdmail/libbmdmail.h>


int main()
{
	GenBuf_t *body=NULL;
	GenBuf_t *toBuf=NULL;
	bmd_smime_msg *msg=NULL;
	bmd_crypt_ctx_t *ctx=NULL;
	int ret_val=0;
	char login[20];
	char password[20];
	
	mail_send_info_t mail_info;
	memset(&mail_info, 0, sizeof(mail_send_info_t));
		
	bmd_init();

	bmd_smime_create(&msg);
	//set_gen_buf2("To jest próbny tekst1To jest próbny tekst2To jest próbny tekst3żółć", strlen("To jest próbny tekst1To jest próbny tekst2To jest próbny tekst3"), &body);
	bmd_load_binary_content("Play_base64_template.html", &body);
	bmd_smime_set_body(msg, body, BODY_TYPE_TEXT_HTML, BODY_CHARSET_UTF8);
	free_gen_buf(&body);
	bmd_smime_set_header(msg, "wlodzimierz.szczygiel@wp.pl", "wszczygiel@unet.pl", "wiadomość SMIME: awizo PLAY z treścią html utf-8");
	bmd_smime_add_attachment(msg, "awizo1.jpg");
	//bmd_smime_add_attachment(msg, "awizo2.jpg");
	bmd_smime_add_attachment(msg, "awizo3.jpg");
	bmd_smime_add_attachment(msg, "playLogo.jpg");
	
		
	//bmd_save_buf(msg->mime_entity, "mail.eml");
	bmd_set_ctx_file("wlodzimierz.pfx","certyfikat:1", strlen("certyfikat:1"), &ctx);
	
	//ret_val=bmd_smime_sign(msg, ctx);
	ret_val=bmd_smime_sign_play(msg, ctx);
	if(ret_val != BMD_OK)
	{
		printf("nie podpisal: <%i>\n", ret_val);
	}
		//! WRAPPER na tworzenie i niszczenie struktury mail_send_info_t
	//przygotowanie wysyłki i wysyłanie
	mail_info.recipient_to=strdup("wszczygiel@unet.pl");
	mail_info.server_port=strdup("smtp.unet.pl:25");
	mail_info.with_auth=1;
	printf("\npodaj login do skrzynki unet: ");
	gets(login);
	printf("podaj haslo do skrzynki unet: ");
	gets(password);

	mail_info.auth_data.username=strdup(login);
	mail_info.auth_data.password=strdup(password);
	
	ret_val=bmd_smime_send(msg, &mail_info);
	if(ret_val != BMD_OK)
	{
		printf("nie udalo sie wysłac %i\n", ret_val);
	}
	else
		{ printf("poszło na maila\n"); }
	
	bmd_smime_toGenBuf(msg, &toBuf);
	bmd_save_buf(toBuf, "sig_awizo.eml");
	free_gen_buf(&toBuf);

	bmd_smime_destroy(&msg);

	free(mail_info.recipient_to);
	free(mail_info.server_port);
	free(mail_info.auth_data.username);
	free(mail_info.auth_data.password);

	bmd_ctx_destroy(&ctx);
	bmd_end();

	return 0;
}



