#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>

/** SMIME P7S API
Autor: Włodzimierz Szczygieł (Dział Programowania, Unizeto Technologies SA)
*/

long bmd_smime_create(bmd_smime_msg ** msg)
{
	long ret_val=0;
	bmd_smime_msg *new_msg=NULL;

	if(msg == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(*msg != NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }

	PRINT_INFO("LIBBMDMIME creating smime structure\n");

	new_msg=(bmd_smime_msg*)calloc(1, sizeof(bmd_smime_msg));
	if(new_msg == NULL)
		{ return BMD_ERR_MEMORY; }

	ret_val=init_attach_list( &(new_msg->attachments) );
	if(ret_val != BMD_OK)
	{
		free(new_msg);
		return BMD_ERR_OP_FAILED;
	}

	*msg=new_msg;
	return BMD_OK;
}


long bmd_smime_add_attachment(	bmd_smime_msg * msg,
					char *file_name,
					long hidden_flag)
{
	bmd_mime_attachment *new_attachment=NULL;
	long ret_val=0;

	if(msg == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(file_name == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	ret_val=bmd_create_attachment(file_name, hidden_flag, &new_attachment);
	if(ret_val != BMD_OK )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}
	ret_val=add_to_attch_list( &(msg->attachments), new_attachment);
	if(ret_val != BMD_OK)
	{
		bmd_destroy_attachment(&new_attachment);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return BMD_OK;
}


long bmd_smime_remove_attachment(bmd_smime_msg * msg, char *file_name)
{
	long ret_val=0;

	if(msg == NULL)
		{ return -1; }
	if(file_name == NULL)
		{ return -2; }

	ret_val=remove_from_attach_list( &(msg->attachments), file_name);
	return ret_val;
}


long bmd_smime_add_attachment_from_buf(bmd_smime_msg * msg, char *file_buf, long file_len, char *file_name, long hidden_flag)
{
	bmd_mime_attachment *new_attachment=NULL;
	long ret_val=0;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(file_buf == NULL)
		{ return BMD_ERR_PARAM2; }
	//zalacznik moze byc pusty
	if(file_len < 0)
		{ return BMD_ERR_PARAM3; }
	if(file_name == NULL)
		{ return BMD_ERR_PARAM4; }

	ret_val=bmd_create_attachment_from_buf(file_buf, file_len, file_name, hidden_flag, &new_attachment);
	if(ret_val != BMD_OK )
		{ return BMD_ERR_OP_FAILED; }
	ret_val=add_to_attch_list( &(msg->attachments), new_attachment);
	if(ret_val != BMD_OK)
	{
		bmd_destroy_attachment(&new_attachment);
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}


long bmd_smime_has_attachment(bmd_smime_msg * msg, char *file_name)
{
	long retVal=0;

	if(msg == NULL)
		{ return -1; }
	if(file_name == NULL)
		{ return -2; }
	if(strlen(file_name) <= 0)
		{ return -3; }

	retVal=is_attachment_in_attach_list(msg->attachments, file_name);
	
	if(retVal > 0)
	{
		return 1;
	}
	else if(retVal == 0)
	{
		return 0;
	}
	else
	{
		return -4;
	}
}


long bmd_smime_set_body(bmd_smime_msg * msg, GenBuf_t *body, long body_type, long body_charset)
{
	GenBuf_t *new_body=NULL;

	if(msg == NULL)
		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(body == NULL)
		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(body->buf == NULL)
		{	BMD_FOK(BMD_ERR_PARAM2);	}

	free_gen_buf( &(msg->body) ); //przy zmianie tresci zwolnic poprzednia
	BMD_FOK(set_gen_buf2(body->buf, body->size, &new_body));

	msg->body=new_body;
	msg->body_type=body_type;
	msg->body_charset=body_charset;

	return BMD_OK;
}


long bmd_smime_set_header(bmd_smime_msg * msg, char *from, char *to, char *subject)
{
	if(msg == NULL)		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(to == NULL)		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(from == NULL)		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(subject == NULL)	{ BMD_FOK(BMD_ERR_PARAM4); }

	PRINT_INFO("LIBBMDMIME setting header (v.1)\n");

	free(msg->from); //przy zmianie nadawcy zwalniac porzedni
	msg->from=strdup(from);
	if(msg->from == NULL)	{ return BMD_ERR_OP_FAILED; }

	free(msg->to); //przy zmianie adresata zwalniac porzedni
	msg->to=strdup(to);
	if(msg->to == NULL)
	{
		free(msg->from); msg->from=NULL;
		return BMD_ERR_OP_FAILED;
	}
	free(msg->subject); //przy zmianie tematu zwalniac porzedni

	//jesli ogonki w temacie , to odpowiedni zapis
	if( (check_ma_ogonki(subject)) == 1 )
	{
		if( (mail_attachment_name(subject, &(msg->subject))) != 0)
		{
			free(msg->from); msg->from=NULL;
			free(msg->to); msg->to=NULL;
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
		msg->subject=strdup(subject);
	}
	if(msg->subject == NULL)
	{
		free(msg->from); msg->from=NULL;
		free(msg->to); msg->to=NULL;
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

long bmd_smime_set_header2(bmd_smime_msg * msg, char *from, char *to, char *subject, char *messageId)
{
	if(msg == NULL)		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(to == NULL)		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(from == NULL)		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(subject == NULL)	{ BMD_FOK(BMD_ERR_PARAM4); }

	PRINT_INFO("LIBBMDMIME setting header (v.2), messageID = %p\n", messageId);

	free(msg->from); //przy zmianie nadawcy zwalniac porzedni
	msg->from=strdup(from);
	if(msg->from == NULL)	{ return BMD_ERR_OP_FAILED; }

	free(msg->to); //przy zmianie adresata zwalniac porzedni
	msg->to=strdup(to);
	if(msg->to == NULL)
	{
		free(msg->from); msg->from=NULL;
		return BMD_ERR_OP_FAILED;
	}
	free(msg->subject); //przy zmianie tematu zwalniac porzedni

	//jesli ogonki w temacie , to odpowiedni zapis
	if( (check_ma_ogonki(subject)) == 1 )
	{
		if( (mail_attachment_name(subject, &(msg->subject))) != 0)
		{
			free(msg->from); msg->from=NULL;
			free(msg->to); msg->to=NULL;
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
		msg->subject=strdup(subject);
	}
	if(msg->subject == NULL)
	{
		free(msg->from); msg->from=NULL;
		free(msg->to); msg->to=NULL;
		return BMD_ERR_OP_FAILED;
	}


	msg->messageId=strdup(messageId);
	if(msg->messageId == NULL)
	{
		free(msg->from); msg->from=NULL;
		free(msg->to); msg->to=NULL;
		free(msg->subject); msg->subject=NULL;
		return BMD_ERR_OP_FAILED;
	}


	return BMD_OK;
}

long bmd_smime_noSign(bmd_smime_msg *msg)
{
	long ret_val=0;

	if(msg == NULL)
		{ BMD_FOK(-1); }

	PRINT_INFO("LIBBMDMIME smime without signature\n");

	free_gen_buf(&(msg->mime_entity)); //jesli bylo juz podpisywane/tworzone
	ret_val=_bmd_make_MIMEentity_to_sign_advanced(msg);
	if(ret_val != 0)
		{ return BMD_SMIME_ERR_CREATE_ENTITY; }
	free_gen_buf(&(msg->smime)); //jesli bylo juz podpisywane/towrzone
	ret_val=_bmd_createMimeMessage(msg);
	if(ret_val != 0)
		{ return BMD_SMIME_ERR_CREATE_SMIME; }

	return 0;
}

long bmd_smime_sign(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx)
{
	GenBuf_t *signature=NULL;

	/*atrybuty podpisane*/
	bmd_signature_params_t *sig_params=NULL;
	SignedAttributes_t *sig_attrs=NULL;
	Attribute_t *attr_SigningTime=NULL;

	long ret_val=0;

	if(msg == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ctx == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	PRINT_INFO("LIBBMDMIME smime with signature\n");

	free_gen_buf(&(msg->mime_entity)); //jesli bylo juz podpisywane
	ret_val=_bmd_make_MIMEentity_to_sign_advanced(msg);
	if(ret_val != BMD_OK)
	{	
		BMD_FOK(BMD_SMIME_ERR_CREATE_ENTITY);
	}


	/**atrybut podpisany SigningTime*/

	if( ( SignedAtributes_init(&sig_attrs) ) != BMD_OK )
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	if( ( SigAttr_Create_SigningTime(&attr_SigningTime) ) != BMD_OK )
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	if( ( SignedAtributes_add(attr_SigningTime, sig_attrs) ) != BMD_OK)
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr_SigningTime, 1);
		free(attr_SigningTime); attr_SigningTime=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	if( ( SigningParams_Create(sig_attrs, NULL, &sig_params) ) != BMD_OK )
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	sig_params->signature_flags=BMD_CMS_SIG_EXTERNAL;

	ret_val=bmd_sign_data(msg->mime_entity, ctx , &signature, (void*)sig_params);
	SigningParams_Destroy(&sig_params);
	if(ret_val != BMD_OK)
	{
		BMD_FOK(BMD_SMIME_ERR_SIGN_ENTITY);
	}

	free_gen_buf(&(msg->signature)); //jesli bylo juz podpisywane
	msg->signature=signature;

	free_gen_buf(&(msg->smime)); //jesli bylo juz podpisywane
	ret_val=_bmd_createSMIMEmessage(msg);
	if(ret_val != BMD_OK)
	{
		BMD_FOK(BMD_SMIME_ERR_CREATE_SMIME);
	}

	return BMD_OK;
}

/*na potrzeby play - zrezygnowano z wykorzystania bmd_smime_sign_jpeg_inline*/
/*
long bmd_smime_sign_jpeg_inline(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx)
{
	GenBuf_t *signature=NULL;
	bmd_signature_params_t *sig_params=NULL;
	long ret_val=0;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(ctx == NULL)
		{ return BMD_ERR_PARAM2; }

	ret_val=_bmd_make_MIMEentity_to_sign_jpeg_inline(msg);
	if(ret_val != BMD_OK)
		{ return BMD_SMIME_ERR_CREATE_ENTITY; }

	sig_params=(bmd_signature_params_t*)calloc(1, sizeof(bmd_signature_params_t));
	if(sig_params == NULL)
		{ return BMD_ERR_MEMORY; }
	sig_params->signature_flags=BMD_CMS_SIG_EXTERNAL;

	ret_val=bmd_sign_data(msg->mime_entity, ctx , &signature, (void*)sig_params);
	if(ret_val != BMD_OK)
	{
		free(sig_params);
		return BMD_SMIME_ERR_SIGN_ENTITY;
	}

	free(sig_params);
	msg->signature=signature;

	ret_val=_bmd_createSMIMEmessage(msg);
	if(ret_val != BMD_OK)
		{ return BMD_SMIME_ERR_CREATE_SMIME; }

	return BMD_OK;
}
*/

/*na potrzeby play- zrezygnowano -teraz ujednolicona funkcja bmd_smime_sign()
long bmd_smime_sign_play(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx)
{
	GenBuf_t *signature=NULL;

	//atrybuty podpisane
	bmd_signature_params_t *sig_params=NULL;
	SignedAttributes_t *sig_attrs=NULL;
	Attribute_t *attr_SigningTime=NULL;

	long ret_val=0;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(ctx == NULL)
		{ return BMD_ERR_PARAM2; }

	free_gen_buf(&(msg->mime_entity)); //jesli bylo juz podpisywane
	//ret_val=_bmd_make_MIMEentity_to_sign_play(msg);
	ret_val=_bmd_make_MIMEentity_to_sign_advanced(msg);
	if(ret_val != BMD_OK)
		{ return BMD_SMIME_ERR_CREATE_ENTITY; }


	//atrybut podpisany SigningTime

	if( ( SignedAtributes_init(&sig_attrs) ) != BMD_OK )
	{
		return BMD_ERR_OP_FAILED;
	}
	if( ( SigAttr_Create_SigningTime(&attr_SigningTime) ) != BMD_OK )
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		return BMD_ERR_OP_FAILED;
	}
	if( ( SignedAtributes_add(attr_SigningTime, sig_attrs) ) != BMD_OK)
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr_SigningTime, 1);
		free(attr_SigningTime); attr_SigningTime=NULL;
		return BMD_ERR_OP_FAILED;
	}
	if( ( SigningParams_Create(sig_attrs, NULL, &sig_params) ) != BMD_OK )
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, sig_attrs, 1);
		free(sig_attrs); sig_attrs=NULL;
		return BMD_ERR_OP_FAILED;
	}
	sig_params->signature_flags=BMD_CMS_SIG_EXTERNAL;

	ret_val=bmd_sign_data(msg->mime_entity, ctx , &signature, (void*)sig_params);
	SigningParams_Destroy(&sig_params);
	if(ret_val != BMD_OK)
	{
		return BMD_SMIME_ERR_SIGN_ENTITY;
	}

	free_gen_buf(&(msg->signature)); //jesli bylo juz podpisywane
	msg->signature=signature;

	free_gen_buf(&(msg->smime)); //jesli bylo juz podpisywane
	ret_val=_bmd_createSMIMEmessage(msg);
	if(ret_val != BMD_OK)
		{ return BMD_SMIME_ERR_CREATE_SMIME; }

	return BMD_OK;
}
*/

//na wejsciu mailer musi miec wypelnione pola: adresu serwera smtp, adresata, potrzebnych danych autoryzacyjnych
long bmd_smime_send(bmd_smime_msg * msg, mail_send_info_t * mailer)
{
	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }
	if(msg->smime == NULL)
		{ return BMD_ERR_PARAM1; }
	if(mailer == NULL)
		{ return BMD_ERR_PARAM2; }

	mailer->mail_body=(char*)msg->smime->buf;
	if( bmd_send_mail(mailer) !=  BMD_OK )
		{ return BMD_ERR_OP_FAILED; }
	return BMD_OK;
}

/* funkcja na podstawie msg wypelnia pola mail_body oraz recipient_to mailera*/
long bmd_smime_fill_mailer(bmd_smime_msg *msg, mail_send_info_t * mailer)
{
char *body_str		= NULL;
char *to_str		= NULL;


	if(msg == NULL)					{	return -1;	}
	if(mailer == NULL)				{	return -2;	}
	if(msg->smime == NULL && msg->to == NULL)	{	return -3;	}

	body_str=strdup( (char*)(msg->smime->buf) );
	if(body_str == NULL)				{	return -4;	}
	to_str=strdup( (msg->to) );
	if(to_str == NULL)
	{
		free(body_str);
		return -5;
	}

	mailer->recipient_to=to_str;
	mailer->mail_body=body_str;

	return 0;
}


long bmd_smime_toGenBuf(bmd_smime_msg * msg, GenBuf_t ** dest)
{
long ret_val			= 0;
GenBuf_t *nowy		= NULL;

	if(msg == NULL)		{	return BMD_ERR_PARAM1;	}
	if(msg->smime == NULL)	{	return BMD_ERR_PARAM1;	}
	if(dest == NULL)		{	return BMD_ERR_PARAM2;	}
	if(*dest != NULL)		{	return BMD_ERR_PARAM2;	}

	ret_val=set_gen_buf2(msg->smime->buf, msg->smime->size, &nowy);
	if(ret_val != BMD_OK)	{	return BMD_ERR_OP_FAILED;	}

	*dest=nowy;
	return BMD_OK;
}

long bmd_smime_destroy(bmd_smime_msg ** msg)
{
	free_gen_buf( &((*msg)->body) );
	free( (*msg)->from );
	free( (*msg)->to );
	free( (*msg)->subject );
	free( (*msg)->messageId );
	destroy_attach_list( &((*msg)->attachments) );
	free_gen_buf( &((*msg)->mime_entity) );
	free_gen_buf( &((*msg)->signature) );
	free_gen_buf( &((*msg)->smime) );
	free(*msg);
	*msg=NULL;

	return BMD_OK;
}
