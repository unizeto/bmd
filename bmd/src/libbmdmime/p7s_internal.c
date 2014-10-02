#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdmime/mime_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/** funkcje wewnętrzne na uzytek SMIME P7S API - tworzenie jednostki mime do podpisu oraz tworzenie pełnej
wiadomosci SMIME
Autor: Włodzimierz Szczygieł (Dział Programowania, Unizeto Technologies SA)
*/

/*jesli nie ma zalacznikow, to wowczas umieszcza sie tylko treść maila (juz nie w multipart/mixed)*/
long _bmd_simple_entity_to_sign(bmd_smime_msg * msg)
{
	char *text1=NULL;
	char *text2=NULL;
	char* encoded64=NULL;
	char *cut_enc64=NULL;
	GenBuf_t *new_entity=NULL;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }

	switch(msg->body_type)
	{
		case BODY_TYPE_TEXT_PLAIN:
		{
			asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: text/plain; ");
			break;
		}
		case BODY_TYPE_TEXT_HTML:
		{
			asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: text/html; ");
			break;
		}
		default:
		{ // kazda inna na domysle BODY_TYPE_TEXT_PLAIN
			asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: text/plain; ");
		}
	}
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_charset)
	{
		case BODY_CHARSET_UTF8:
		{
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_2:
		{
			asprintf(&text2, "%scharset=\"iso-8859-2\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_1:
		{
			asprintf(&text2, "%scharset=\"iso-8859-1\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_CP1250:
		{
			asprintf(&text2, "%scharset=\"windows-1250\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		default:
		{ //na domysle utf-8
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
	}

	encoded64=(char *)spc_base64_encode( (unsigned char *)msg->body->buf, msg->body->size, 1);
	if(encoded64 == NULL)
	{
		free(text2);
		return BMD_ERR_OP_FAILED;
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n

	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2);
		free(encoded64);
		return BMD_ERR_OP_FAILED;
	}
	free(encoded64); encoded64=NULL;

	asprintf(&text1, "%sContent-Transfer-Encoding: base64\r\n\r\n%s", text2, cut_enc64);
	free(cut_enc64); cut_enc64=NULL;
	free(text2); text2=NULL;

	new_entity=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_entity == NULL)
	{
		free(text1);
		return BMD_ERR_MEMORY;
	}
	new_entity->buf=(char *)text1;
	new_entity->size=(long)strlen(text1);

	msg->mime_entity=new_entity;

	return BMD_OK;
}

/* zrezygnowano na rzecz _bmd_make_MIMEentity_to_sign_advanced()
long _bmd_make_MIMEentity_to_sign(bmd_smime_msg * msg)
{
	char* encoded64=NULL;
	bmd_attach_list* iter=NULL;
	bmd_mime_attachment *attachment=NULL;
	char *text1=NULL;
	char *text2=NULL;
	char *boundary="unizetoboundarymainentity";
	long ret_val=0;
	GenBuf_t *new_entity=NULL;
	char *cut_enc64=NULL;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }

	asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"%s\"\r\n\r\n", boundary);

	//!jesli nie podano tresci, to wrzuca sie jedną spację
	if( msg->body == NULL )
	{
		set_gen_buf2((char *)" ", 1, &(msg->body));
	}


	//!jesli nie ma zalacznikow, to nie generuje sie multipart/mixed tylko zwykla jedna czesc tekstowa
	if(msg->attachments == NULL)
	{
		return _bmd_simple_entity_to_sign(msg);
	}

	//!!! tutaj poczatek wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)
	asprintf(&text2, "%s--%s\r\n", text1, boundary); //--boundary
	free(text1); text1=NULL;
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_type)
	{
		case BODY_TYPE_TEXT_PLAIN:
		{
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		case BODY_TYPE_TEXT_HTML:
		{
			asprintf(&text1, "%sContent-Type: text/html; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		default:
		{ // kazda inna na domysle BODY_TYPE_TEXT_PLAIN
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
		}
	}
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_charset)
	{
		case BODY_CHARSET_UTF8:
		{
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_2:
		{
			asprintf(&text2, "%scharset=\"iso-8859-2\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_1:
		{
			asprintf(&text2, "%scharset=\"iso-8859-1\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_CP1250:
		{
			asprintf(&text2, "%scharset=\"windows-1250\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		default:
		{ //na domysle utf-8
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
	}

	encoded64=spc_base64_encode( (unsigned char *)msg->body->buf, msg->body->size, 1);
	if(encoded64 == NULL)
	{
		free(text2);
		return BMD_ERR_OP_FAILED;
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n

	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2);
		free(encoded64);
		return BMD_ERR_OP_FAILED;
	}
	free(encoded64); encoded64=NULL;

	asprintf(&text1, "%sContent-Transfer-Encoding: base64\r\n\r\n%s", text2, cut_enc64);
	free(cut_enc64); cut_enc64=NULL;
	free(text2); text2=NULL;

	//!!! tutaj koniec wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)

	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	if(ret_val != BMD_END_OF_LIST)
	{
		while(attachment != NULL)
		{
			asprintf(&text2, "%s--unizetoboundarymainentity\r\nContent-Type: application/octet-stream;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment\r\n\r\n", text1, attachment->file_name);
			free(text1); text1=NULL;

			encoded64=spc_base64_encode((unsigned char *)attachment->content->buf, attachment->content->size, 1);
			if(encoded64 == NULL)
			{
				free(text2);
				return BMD_ERR_OP_FAILED;
			}

			if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
			{
				free(text2);
				free(encoded64);
				return BMD_ERR_OP_FAILED;
			}
			free(encoded64); encoded64=NULL;

			asprintf(&text1, "%s%s\r\n", text2, cut_enc64);
			free(text2); text2=NULL;
			free(cut_enc64); cut_enc64=NULL;
			attachment=NULL; //nie wolno zwalniac - bindowanie wskaznikow
			get_next_item_attach_list(&iter, &attachment);
		}
	}

	asprintf(&text2, "%s--unizetoboundarymainentity--\r\n", text1);
	free(text1); text1=NULL;

	new_entity=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_entity == NULL)
	{
		free(text2);
		return BMD_ERR_MEMORY;
	}
	new_entity->buf=(uint8_t*)text2;
	new_entity->size=strlen(text2);

	msg->mime_entity=new_entity;
	return BMD_OK;
}
*/



/*
funkcja sluzy do skladania wiadomosci MIME (bez podpisu)
*/
long _bmd_createMimeMessage(bmd_smime_msg *msg)
{
	char *text1		= NULL;
	GenBuf_t *genbuf	= NULL;
	long retVal = 0;

	if(msg == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->mime_entity == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->mime_entity->buf == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if( !(msg->from) || !(msg->to) || !(msg->subject) )
		{ BMD_FOK(BMD_ERR_PARAM1); }

	//pole smime struktury bmd_smime_msg przechowuje takze gotowa wiadomosc bez podpisu
	if(msg->messageId != NULL)
	{
		retVal = asprintf(&text1, "From: %s\r\nTo: %s\r\nSubject: %s\r\nMessage-Id: <%s>\r\n%s", msg->from, msg->to, msg->subject, msg->messageId, msg->mime_entity->buf );
		PRINT_DEBUG("LIBBMDMIME setting messageID\n");
	}
	else
	{
		retVal = asprintf(&text1, "From: %s\r\nTo: %s\r\nSubject: %s\r\n%s", msg->from, msg->to, msg->subject, msg->mime_entity->buf );
		PRINT_DEBUG("LIBBMDMIME setting without messageID\n");
	}
	
	if(retVal == -1)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	else
		{ PRINT_DEBUG("LIBBMDMIME message =\n%s\n", text1); }

	genbuf=(GenBuf_t*)calloc(1,sizeof(GenBuf_t));
	if(genbuf == NULL)
	{
		free(text1);
		return BMD_ERR_MEMORY;
	}
	genbuf->buf=(char *)text1;
	genbuf->size=(long)strlen(text1);

	msg->smime=genbuf;
	return 0;
}


/*
funkcja sluzy do skladania podpisywanej zawartosci i podpisu w jednostke SMIME typu multipart/signed
*/
long _bmd_createSMIMEmessage(bmd_smime_msg *msg)
{
	char* encoded64=NULL;
	char *text1=NULL;
	char *text2=NULL;
	char *boundary="unizetosmime";
	char *cut_enc64=NULL;
	long retVal = 0;

	if(msg == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->mime_entity == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->mime_entity->buf == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->signature == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(msg->signature->buf == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if( !(msg->from) || !(msg->to) || !(msg->subject) )
		{ BMD_FOK(BMD_ERR_PARAM1); }

	//!! uwaga dostawiony charset
	if(msg->messageId != NULL)
	{
		retVal = asprintf(&text1, "From: %s\r\nTo: %s\r\nSubject: %s\r\nMessage-Id: <%s> \r\nMIME-Version: 1.0\r\nContent-Type: multipart/signed; charset=\"utf-8\";\r\n protocol=\"application/pkcs7-signature\";\r\n micalg=sha1; boundary=%s\r\n\r\n--%s\r\n", msg->from, msg->to, msg->subject, msg->messageId, boundary, boundary);
		PRINT_DEBUG("LIBBMDMIME setting messageID\n");
	}
	else
	{
		retVal = asprintf(&text1, "From: %s\r\nTo: %s\r\nSubject: %s\r\nMIME-Version: 1.0\r\nContent-Type: multipart/signed; charset=\"utf-8\";\r\n protocol=\"application/pkcs7-signature\";\r\n micalg=sha1; boundary=%s\r\n\r\n--%s\r\n", msg->from, msg->to, msg->subject, boundary, boundary);
		PRINT_DEBUG("LIBBMDMIME setting without messageID\n");
	}
	if(retVal == -1)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	else
		{ PRINT_DEBUG("LIBBMDMIME message =\n%s\n", text1); }
	
	retVal = asprintf(&text2, "%s%s\r\n--%s\r\nContent-Type: application/pkcs7-signature; name=smime.p7s\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=smime.p7s\r\n\r\n", text1, msg->mime_entity->buf, boundary);
	free(text1); text1=NULL;
	if(retVal == -1)
		{ return BMD_ERR_MEMORY; }
	
	encoded64=(char *)spc_base64_encode( (unsigned char *)msg->signature->buf, msg->signature->size, 1);
	if(encoded64 == NULL)
	{
		free(text2); text2 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n
	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2); text2 = NULL;
		free(encoded64); encoded64 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	free(encoded64); encoded64=NULL;

	retVal = asprintf(&text1, "%s%s\r\n--%s--\r\n", text2, cut_enc64, boundary); //tutaj bylo \r\n\r\n przed --%s--
	free(text2); text2=NULL;
	free(cut_enc64); cut_enc64=NULL;
	if(retVal == -1)
		{ BMD_FOK(BMD_ERR_MEMORY); }

	msg->smime=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(msg->smime == NULL)
	{
		free(text1); text1 = NULL;
		BMD_FOK(NO_MEMORY);
	}
	msg->smime->buf=(char *)text1;
	msg->smime->size=(long)strlen(text1);
	return BMD_OK;
}

/* zrezygnowano z wykorzystania bmd_smime_sign_jpeg_inline() i tym samym z _bmd_make_MIMEentity_to_sign_jpeg_inline()*/
/*long _bmd_make_MIMEentity_to_sign_jpeg_inline(bmd_smime_msg * msg)
{
	char* encoded64=NULL;
	bmd_attach_list* iter=NULL;
	bmd_mime_attachment *attachment=NULL;
	char *text1=NULL;
	char *text2=NULL;
	char *boundary="unizetoboundarymainentity";
	long ret_val=0;
	GenBuf_t *new_entity=NULL;
	char *cut_enc64=NULL;

	if(msg == NULL)
		{ return BMD_ERR_PARAM1; }

	asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"%s\"\r\n\r\n", boundary);

	//!jesli nie podano tresci, to wrzuca sie jedną spację
	if( msg->body == NULL )
	{
		set_gen_buf2((char *)" ", 1, &(msg->body));
	}


	//!jesli nie ma zalacznikow, to nie generuje sie multipart/mixed tylko zwykla jedna czesc tekstowa
	if(msg->attachments == NULL)
	{
		return _bmd_simple_entity_to_sign(msg);
	}

	//!!! tutaj poczatek wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)
	asprintf(&text2, "%s--%s\r\n", text1, boundary); //--boundary
	free(text1); text1=NULL;
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_type)
	{
		case BODY_TYPE_TEXT_PLAIN:
		{
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		case BODY_TYPE_TEXT_HTML:
		{
			asprintf(&text1, "%sContent-Type: text/html; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		default:
		{ // kazda inna na domysle BODY_TYPE_TEXT_PLAIN
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
		}
	}
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_charset)
	{
		case BODY_CHARSET_UTF8:
		{
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_2:
		{
			asprintf(&text2, "%scharset=\"iso-8859-2\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_1:
		{
			asprintf(&text2, "%scharset=\"iso-8859-1\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_CP1250:
		{
			asprintf(&text2, "%scharset=\"windows-1250\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		default:
		{ //na domysle utf-8
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
	}

	encoded64=spc_base64_encode( (unsigned char *)msg->body->buf, msg->body->size, 1);
	if(encoded64 == NULL)
	{
		free(text2);
		return BMD_ERR_OP_FAILED;
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n

	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2);
		free(encoded64);
		return BMD_ERR_OP_FAILED;
	}
	free(encoded64); encoded64=NULL;

	asprintf(&text1, "%sContent-Transfer-Encoding: base64\r\n\r\n%s", text2, cut_enc64);
	free(cut_enc64); cut_enc64=NULL;
	free(text2); text2=NULL;

	//!!! tutaj koniec wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)

	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	if(ret_val != BMD_END_OF_LIST)
	{
		while(attachment != NULL)
		{
			asprintf(&text2, "%s--unizetoboundarymainentity\r\nContent-Type: image/jpeg;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-ID: <%s>\r\nContent-Disposition: inline\r\n\r\n", text1, attachment->file_name, attachment->file_name);
			free(text1); text1=NULL;

			encoded64=spc_base64_encode((unsigned char *)attachment->content->buf, attachment->content->size, 1);
			if(encoded64 == NULL)
			{
				free(text2);
				return BMD_ERR_OP_FAILED;
			}

			if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
			{
				free(text2);
				free(encoded64);
				return BMD_ERR_OP_FAILED;
			}
			free(encoded64); encoded64=NULL;

			asprintf(&text1, "%s%s\r\n", text2, cut_enc64);
			free(text2); text2=NULL;
			free(cut_enc64); cut_enc64=NULL;
			attachment=NULL; //nie wolno zwalniac - bindowanie wskaznikow
			get_next_item_attach_list(&iter, &attachment);
		}
	}

	asprintf(&text2, "%s--unizetoboundarymainentity--\r\n", text1);
	free(text1); text1=NULL;

	new_entity=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_entity == NULL)
	{
		free(text2);
		return BMD_ERR_MEMORY;
	}
	new_entity->buf=(uint8_t*)text2;
	new_entity->size=strlen(text2);

	msg->mime_entity=new_entity;
	return BMD_OK;
}
*/

/*
funkcja bmd_compare_extension() sprawdza,czy plik o nazwie file_name jest roszerzenia extension
Wielkosc znakow dla file_name jak i extension nie ma znaczenia (funkcja NIE jest case sensitive)

@arg file_name to nazwa pliku (moze byc rowniez cala sciezka do pliku) - w nazwie jest rowniez rozszerzenie
@arg extension to wymagane rozszerzenie
@note
np. dla file_name "test.pdf" i extenion ".pdf" sprawdza, czy nazwa pliku wskazuje na plik o rozszerzeniu .pdf.
extension musi byc podane z kropka (kropka  + rozszerzenie), aby miec pewnosc w przypadku poprawnego dopasowania,
ze to rozszerzenie, a nie np. koncowka nazwy pliku pozbawionego roszerzenia

@retval 0 jesli plik file_name jest rozszerzenia extension
@retval -1 jesli za file_name przekazano NULL
@retval -2 jesli za extension przekazano NULL
@retval -3 jesli file_name to pusty lanuch
@retval -4 jesli extensio to pusty lancuch
@retval -5 jesli lancuch extension jest dluzszy od nazwy pliku file_name
@retval -6,-7 w przypadku problemu z alokacja pamieci
@retval -100 jesli podany plik file_name nie jest rozszerzenia extension

!!!! w tej chwili funkcja nie jest wykorzystywana !!!!

*/
/*
long bmd_compare_extension(char *file_name, char *extension)
{
	long len_name=0;
	long len_ext=0;
	long len_withoutExt=0;
	char *cut_extension=NULL;
	char *copy_extension=NULL;
	long iter=0;

	if(file_name == NULL)
		{ return -1; }
	if(extension == NULL)
		{ return -2;}
	if( (len_name=strlen(file_name)) <= 0 )
		{ return -3; }
	if( (len_ext=strlen(extension)) <= 0 )
		{ return -4; }
	if(len_ext > len_name)
		{ return -5; }

	len_withoutExt=len_name-len_ext;

	copy_extension=(char*)calloc(len_ext+1, sizeof(char));
	if(copy_extension == NULL)
		{ return -6; }
	for(iter=0; iter < len_ext; iter++)
	{
		copy_extension[iter]=tolower( extension[iter] );
	}

	cut_extension=(char*)calloc(len_ext+1, sizeof(char));
	if(cut_extension == NULL)
	{
		free(copy_extension);
		return -7;
	}
	for(iter=len_withoutExt; iter < len_name; iter++)
	{
		cut_extension[iter - len_withoutExt]= tolower(file_name[iter]);
	}

	if( strcmp(copy_extension, cut_extension) == 0 )
	{
		free(copy_extension);
		free(cut_extension);
		return 0;
	}
	else
	{
		free(copy_extension);
		free(cut_extension);
		return -100;
	}
}
*/

/*funkcja tworzy:
1) strukture multipart/mixed, jesli brakuje zalacznikow, badz wystepuja tylko zalaczniki jawne
2) strukture multipart/related, jesli wystepuja tylko zalaczniki ukryte
*/
long __bmd_make_MIMEentity_to_sign_basic(bmd_smime_msg * msg)
{
	char* encoded64=NULL;
	bmd_attach_list* iter=NULL;
	bmd_mime_attachment *attachment=NULL;
	char *text1=NULL;
	char *text2=NULL;
	char *boundary="unizetoboundarymainentity";
	long ret_val=0;
	GenBuf_t *new_entity=NULL;
	char *cut_enc64=NULL;
	long have_hidden_attachment=0;
	char *mime_content_type=NULL;
	char *filename_in_mail=NULL;

	if(msg == NULL)		{ return BMD_ERR_PARAM1; }

	//zliczanie, ile zalacznikow ukrytych, bo tylko wtedy multipart/related
	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	while(ret_val != BMD_END_OF_LIST)
	{
		if(attachment->hidden != BMD_SMIME_SHOW_ATTACHMENT)
		{
			have_hidden_attachment=1;
			iter=NULL;
			attachment=NULL;
			break;
		}
		attachment=NULL;
		ret_val=get_next_item_attach_list(&iter, &attachment);
	}
	//w tym miejscu iter na pewno ma wartosc NULL (get_next_item_attach_list() tak ustawia dla konca listy)

	if(have_hidden_attachment != 0)
	{
		asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: multipart/related;\r\n boundary=\"%s\"\r\n\r\n", boundary);
	}
	else
	{
		asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"%s\"\r\n\r\n", boundary);
	}

	//!jesli nie podano tresci, to wrzuca sie jedną spację
	if( msg->body == NULL )
	{
		set_gen_buf2((char *)" ", 1, &(msg->body));
	}

	//!jesli nie ma zalacznikow, to nie generuje sie multipart/mixed tylko zwykla jedna czesc tekstowa
	if(msg->attachments == NULL)
	{
		return _bmd_simple_entity_to_sign(msg);
	}

	//!!! tutaj poczatek wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)
	asprintf(&text2, "%s--%s\r\n", text1, boundary); //--boundary
	free(text1); text1=NULL;
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_type)
	{
		case BODY_TYPE_TEXT_PLAIN:
		{
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		case BODY_TYPE_TEXT_HTML:
		{
			asprintf(&text1, "%sContent-Type: text/html; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		default:
		{ // kazda inna na domysle BODY_TYPE_TEXT_PLAIN
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
		}
	}
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_charset)
	{
		case BODY_CHARSET_UTF8:
		{
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_2:
		{
			asprintf(&text2, "%scharset=\"iso-8859-2\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_1:
		{
			asprintf(&text2, "%scharset=\"iso-8859-1\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_CP1250:
		{
			asprintf(&text2, "%scharset=\"windows-1250\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		default:
		{ //na domysle utf-8
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
	}

	encoded64=(char *)spc_base64_encode( (unsigned char *)msg->body->buf, msg->body->size, 1);
	if(encoded64 == NULL)
	{
		free(text2); text2 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n

	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2); text2 = NULL;
		free(encoded64); encoded64 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	free(encoded64); encoded64=NULL;

	asprintf(&text1, "%sContent-Transfer-Encoding: base64\r\n\r\n%s", text2, cut_enc64);
	free(cut_enc64); cut_enc64=NULL;
	free(text2); text2=NULL;

	//!!! tutaj koniec wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)

	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	while(ret_val != BMD_END_OF_LIST)
	{
		_find_mime_content_type(attachment->file_name, &mime_content_type);
		if(mime_content_type == NULL) //default
			{ mime_content_type = strdup("text/plain"); }
		if( (check_ma_ogonki(attachment->file_name)) == 1)
		{
			mail_attachment_name(attachment->file_name, &filename_in_mail);
		}
		else
		{
			filename_in_mail = strdup(attachment->file_name);
		}
		if (attachment->hidden == BMD_SMIME_HIDDEN_ATTACHMENT)
		{
			asprintf(&text2, "%s--%s\r\nContent-Type: %s;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"%s\"\r\nContent-ID: <%s>\r\n\r\n", text1, boundary, mime_content_type, filename_in_mail, filename_in_mail, filename_in_mail);
		}
		else
		{
			asprintf(&text2, "%s--%s\r\nContent-Type: %s;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-ID: <%s>\r\n\r\n", text1, boundary, mime_content_type, filename_in_mail, filename_in_mail);
		}
		free(mime_content_type); mime_content_type=NULL;
		free(filename_in_mail); filename_in_mail=NULL;

		free(text1); text1=NULL;

		encoded64=(char *)spc_base64_encode((unsigned char *)attachment->content->buf, attachment->content->size, 1);
		if(encoded64 == NULL)
		{
			free(text2); text2 = NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
		{
			free(text2); text2 = NULL;
			free(encoded64); encoded64 = NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		free(encoded64); encoded64=NULL;

		asprintf(&text1, "%s%s\r\n", text2, cut_enc64);
		free(text2); text2=NULL;
		free(cut_enc64); cut_enc64=NULL;
		attachment=NULL; //nie wolno zwalniac - bindowanie wskaznikow
		ret_val=get_next_item_attach_list(&iter, &attachment);
	}

	asprintf(&text2, "%s--%s--\r\n", text1, boundary);
	free(text1); text1=NULL;

	new_entity=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_entity == NULL)
	{
		free(text2); text2 = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}
	//przypisujemy bufor text2 dlatego nie wolno go zwalaniac
	new_entity->buf=(char *)text2;
	new_entity->size=(long)strlen(text2);

	msg->mime_entity=new_entity;
	return BMD_OK;
}

/*
funkcja tworzy zagniezdzona strukture multipart/related wewnatrz multipart/mixed
jesli wystepuja zarowno ukryte jak i jawne zalaczniki
*/
long _bmd_make_MIMEentity_to_sign_advanced(bmd_smime_msg * msg)
{
	long hidden_attachment_count=0;
	long shown_attachment_count=0;
	char* encoded64=NULL;
	bmd_attach_list* iter=NULL;
	bmd_mime_attachment *attachment=NULL;
	char *text1=NULL;
	char *text2=NULL;
	char *boundary_mixed="unizetoboundarymainentity";
	char *boundary_related="unizetoboundaryrelated";
	long ret_val=0;
	GenBuf_t *new_entity=NULL;
	char *cut_enc64=NULL;
	char *mime_content_type=NULL;
	char *filename_in_mail=NULL;

	if(msg == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//zliczanie, ile zalacznikow ukrytych a ile do pokazania na liscie zalacznikow
	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	while(ret_val != BMD_END_OF_LIST)
	{
		if(attachment->hidden != BMD_SMIME_SHOW_ATTACHMENT)
		{
			hidden_attachment_count++;
		}
		else
		{
			shown_attachment_count++;
		}
		attachment=NULL;
		ret_val=get_next_item_attach_list(&iter, &attachment);
	}
	//w tym miejscu iter na pewno ma wartosc NULL (get_next_item_attach_list() tak ustawia dla konca listy)

	//jesli brak zalacznikow, badz wystepuja tylko ukryte zalaczniki, albo tylko pokazywane zalaczniki
	if( !((hidden_attachment_count != 0) && (shown_attachment_count != 0)) )
	{
		return __bmd_make_MIMEentity_to_sign_basic(msg);
	}

	// else
	// jesli wystepuja jednoczesnie ukryte i pokazywane zalaczniki, wowczas trzeba zagniezdzic multipart/related
	//	 wewnatrz multipart/mixed - nowa kompozycja mime do podpisania

	//!jesli nie podano tresci, to wrzuca sie jedną spację
	if( msg->body == NULL )
	{
		set_gen_buf2((char *)" ", 1, &(msg->body));
	}

	asprintf(&text1, "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n boundary=\"%s\"\r\n\r\n", boundary_mixed);

	asprintf(&text2, "%s--%s\r\n", text1, boundary_mixed); //--boundary_mixed
	free(text1); text1=NULL;
	//zagniezdzam multipart/related wewnatrz multipart/mixed
	asprintf(&text1, "%sContent-Type: multipart/related;\r\n boundary=\"%s\"\r\n\r\n", text2, boundary_related);
	free(text2); text2=NULL;
	asprintf(&text2, "%s--%s\r\n", text1, boundary_related); //--boundary_related
	free(text1); text1=NULL;
	//!!! tutaj poczatek wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_type)
	{
		case BODY_TYPE_TEXT_PLAIN:
		{
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		case BODY_TYPE_TEXT_HTML:
		{
			asprintf(&text1, "%sContent-Type: text/html; ", text2 );
			free(text2); text2=NULL;
			break;
		}
		default:
		{ // kazda inna na domysle BODY_TYPE_TEXT_PLAIN
			asprintf(&text1, "%sContent-Type: text/plain; ", text2 );
			free(text2); text2=NULL;
		}
	}
	//zaimplementowane pod katem latwego dopisywania
	switch(msg->body_charset)
	{
		case BODY_CHARSET_UTF8:
		{
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_2:
		{
			asprintf(&text2, "%scharset=\"iso-8859-2\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_ISO8859_1:
		{
			asprintf(&text2, "%scharset=\"iso-8859-1\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		case BODY_CHARSET_CP1250:
		{
			asprintf(&text2, "%scharset=\"windows-1250\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
		default:
		{ //na domysle utf-8
			asprintf(&text2, "%scharset=\"utf-8\";\r\n", text1);
			free(text1); text1=NULL;
			break;
		}
	}

	encoded64=(char *)spc_base64_encode((unsigned char *)msg->body->buf, msg->body->size, 1);
	if(encoded64 == NULL)
	{
		free(text2); text2 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//w base64 z zawijaniem nalezy zamienic wszystkie \n zamienic na \r\n

	if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
	{
		free(text2); text2 = NULL;
		free(encoded64); encoded64 = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	free(encoded64); encoded64=NULL;

	asprintf(&text1, "%sContent-Transfer-Encoding: base64\r\n\r\n%s", text2, cut_enc64);
	free(cut_enc64); cut_enc64=NULL;
	free(text2); text2=NULL;
	//wyprodukowany kod base64 ma na koncu \r\n
	//!!! tutaj koniec wrzucania tresci wlasciwej maila (przed wrzucaniem zalacznikow)


	///jawnie pokazywane zalaczniki maja byc wrzucone w zagniezdzeniu mime/relative
	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	while(ret_val != BMD_END_OF_LIST)
	{

		//jesli w zalaczniku np. pdf, to ma byc na liscie zalacznikow, dlatego brak cid
		if( attachment->hidden == BMD_SMIME_HIDDEN_ATTACHMENT )
		{
			_find_mime_content_type(attachment->file_name, &mime_content_type);
			if(mime_content_type == NULL) //default
				{ mime_content_type = strdup("text/plain"); }
			asprintf(&text2, "%s--%s\r\nContent-Type: %s;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-ID: <%s>\r\n\r\n", text1, boundary_related, mime_content_type, attachment->file_name, attachment->file_name);
			free(mime_content_type); mime_content_type=NULL;

			free(text1); text1=NULL;

			encoded64=(char *)spc_base64_encode((unsigned char *)attachment->content->buf,
			attachment->content->size, 1);
			if(encoded64 == NULL)
			{
				free(text2); text2 = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
			{
				free(text2); text2 = NULL;
				free(encoded64); encoded64 = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			free(encoded64); encoded64=NULL;

			asprintf(&text1, "%s%s", text2, cut_enc64); //\r\n
			free(text2); text2=NULL;
			free(cut_enc64); cut_enc64=NULL;
		}
		attachment=NULL; //nie wolno zwalniac - bindowanie wskaznikow
		ret_val=get_next_item_attach_list(&iter, &attachment);
	}

	asprintf(&text2, "%s--%s--\r\n", text1, boundary_related);
	free(text1); text1=NULL;
	//!! koniec czesci mutipart/related

	///ukryte zalaczniki maja byc w czesci multipart/mixed
	init_iter_attach_list(&iter, msg->attachments);
	ret_val=get_next_item_attach_list(&iter, &attachment);
	while(ret_val != BMD_END_OF_LIST)
	{

		//jesli jawny zalacznik, to brak cid
		if( attachment->hidden == BMD_SMIME_SHOW_ATTACHMENT )
		{
			_find_mime_content_type(attachment->file_name, &mime_content_type);
			if(mime_content_type == NULL) //default
				{ mime_content_type = strdup("text/plain"); }

			if( (check_ma_ogonki(attachment->file_name)) == 1)
			{
				mail_attachment_name(attachment->file_name, &filename_in_mail);
				asprintf(&text1, "%s--%s\r\nContent-Type: %s;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n filename=\"%s\"\r\n\r\n", text2, boundary_mixed, mime_content_type, filename_in_mail, filename_in_mail);

				free(filename_in_mail); filename_in_mail=NULL;
			}
			else
			{
				asprintf(&text1, "%s--%s\r\nContent-Type: %s;\r\n name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n filename=\"%s\"\r\n\r\n", text2, boundary_mixed, mime_content_type, attachment->file_name, attachment->file_name);
			}

			free(mime_content_type); mime_content_type=NULL;
			free(text2); text2=NULL;

			encoded64=(char *)spc_base64_encode((unsigned char *)attachment->content->buf,
			attachment->content->size, 1);
			if(encoded64 == NULL)
			{
				free(text1); text1 = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			if( bmd_change_linebreak( (char*)encoded64, &cut_enc64 ) != BMD_OK)
			{
				free(text1); text1 = NULL;
				free(encoded64); encoded64 = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			free(encoded64); encoded64=NULL;

			asprintf(&text2, "%s%s", text1, cut_enc64); // \r\n
			free(text1); text1=NULL;
			free(cut_enc64); cut_enc64=NULL;
		}
		attachment=NULL; //nie wolno zwalniac - bindowanie wskaznikow
		ret_val=get_next_item_attach_list(&iter, &attachment);
	}

	asprintf(&text1, "%s--%s--\r\n", text2, boundary_mixed);
	free(text2); text2=NULL;

	new_entity=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(new_entity == NULL)
	{
		free(text1); text1 = NULL;
		BMD_FOK(NO_MEMORY);
	}

	//przypisujemy bufor text1 dlatego nie wolno go zwalaniac
	new_entity->buf=(char *)text1;
	new_entity->size=(long)strlen(text1);

	msg->mime_entity=new_entity;
	return BMD_OK;

}

/**
funkcja na podstawie nazwy pliku, wydobywa jego rozszerzenie i
znajduje w tablicy przypisany typ MIME (funkcja operuje na statycznej tablicy MIME_types_table)

@arg file_name to nazwa pliku (moze byc sciezka do pliku)
@arg mime_content_type to adres wskaznika na tworzony lanuch z odnalezionym typem MIME

@retval 0 jesli odnaleziono typ MIME dla podanego pliku
@retval -1 jesli za file_name podano NULL
@retval -2 jesli za mime_content_type podano NULL
@retval -3 jesli wartosc wyluskana z mime_content_type nie jest wyNULLowana
@retval -4 jesli nie znaleziono typu MIME skojarzonego z rozszerzeniem pliku

Jesli w tablicy typow MIME znajdzie sie wpis niezgodny z oczekiwanym fomratem, to zostanie on zignorowany,
a wyszukiwanie bedzie dalej przeprowadzane.

*/
long _find_mime_content_type(char *file_name, char **mime_content_type)
{
	char **substrings=NULL;
	char *extension=NULL;
	long subs_num=0;
	long iter=0;
	char *found_type=NULL;

	if(file_name == NULL)
		{ return -1; }
	if(mime_content_type == NULL)
		{ return -2; }
	if(*mime_content_type != NULL)
		{ return -3; }

	//wydzielenie rozszerzenia bez kropki
	bmd_strsep(file_name, '.', &substrings, &subs_num);
	if(subs_num > 1)
	{
		extension=strdup( substrings[subs_num-1] );
		for(iter=0; iter<(long)strlen(extension); iter++)
		{
			extension[iter]=(char)tolower(extension[iter]);
		}
	}
	else //caly file_name traktowany jako rozszerzenie
	{
		extension=strdup( file_name );
		for(iter=0; iter<(long)strlen(file_name); iter++)
		{
			extension[iter]=(char)tolower(extension[iter]);
		}
	}
	bmd_strsep_destroy(&substrings, subs_num);
	subs_num=0;
	substrings=NULL;

	for(iter=0; (strcmp(MIME_types_table[iter],"\0")) !=0  ; iter++ )
	{
		bmd_strsep(MIME_types_table[iter], '=', &substrings, &subs_num);
		if(subs_num == 2) //ok
		{
			//jesli wpis w tabeli odpowiada poszukiwanemu rozszerzeniu pliku
			if( (strcmp(substrings[0], extension)) == 0 )
			{
				found_type=strdup(substrings[1]);
				*mime_content_type=found_type;
				bmd_strsep_destroy(&substrings, subs_num);
				free(extension);
				return 0;
			}
			else
			{
				bmd_strsep_destroy(&substrings, subs_num);
				substrings=NULL;
				subs_num=0;
			}
		}
		else //jesli wpis w tabeli niezgodny z formatem, to ignorujemy go i badamy nastepny
		{
			bmd_strsep_destroy(&substrings, subs_num);
			subs_num=0;
			substrings=NULL;
		}
	}

	free(extension);
	return -4;
}

