#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>

long bmd_p7m_info_create(p7m_info_t **p7m)
{
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*p7m)!=NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	(*p7m)=(p7m_info_t *)malloc(sizeof(p7m_info_t));
	memset(*p7m,0,sizeof(p7m_info_t));
	
	return BMD_OK;
}

long bmd_p7m_info_destroy(p7m_info_t **p7m)
{
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*p7m)==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	free((*p7m)->body);(*p7m)->body=NULL;
	free((*p7m)->headers);(*p7m)->headers=NULL;
	free((*p7m)->signed_message_prefix);(*p7m)->signed_message_prefix=NULL;
	free((*p7m)->internal_prefix);(*p7m)->internal_prefix=NULL;
	free((*p7m)->content_to_sign);(*p7m)->content_to_sign=NULL;
	free((*p7m)->signed_content);(*p7m)->signed_content=NULL;
	free(*p7m);(*p7m)=NULL;
	
	return BMD_OK;
}

/* ustawia domyslne wartosci dla p7m - UTF-8, BASE-64 , text/html, 7bit */
long bmd_p7m_info_set_default(p7m_info_t **p7m)
{
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*p7m)==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	(*p7m)->message_transfer_encoding=BMD_MAIL_TRANSFER_ENCODING_BASE64;
	(*p7m)->body_content_type=BMD_MAIL_CONTENT_TYPE_HTML;
	(*p7m)->body_charset=BMD_MAIL_CHARSET_UTF8; 
	(*p7m)->body_transfer_encoding=BMD_MAIL_TRANSFER_ENCODING_7BIT;
	
	asprintf(&((*p7m)->signed_message_prefix),"%sContent-Transfer-Encoding: %s\r\n%s%s",
		"Content-Type: application/pkcs7-mime; smime-type=signed-data;name=smime.p7m\r\n",
		"base64",
		"Content-Disposition: attachment; filename=smime.p7m\r\n",
		"Content-Description: S/MIME Cryptographic Signature\r\n");
		
	asprintf(&((*p7m)->internal_prefix),"%s%scharset=%s; format=flowed\r\nContent-Transfer-Encoding: %s\r\n",
		"MIME-Version: 1.0\r\nContent-Type: ",
		"text/html; ",
		"UTF-8",
		"7bit");
	
	return BMD_OK;
}

/* ustawia cialo */
long bmd_p7m_set_body(char *body,p7m_info_t **p7m)
{
	if(body==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*p7m)==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	asprintf(&((*p7m)->body),"%s",body);
	
	return BMD_OK;
}

/* ustawia naglowki */
long bmd_p7m_set_headers(char *to,char *from,char *subject,p7m_info_t **p7m)
{
	if(to==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(from==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(subject==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	if((*p7m)==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	
	asprintf(&((*p7m)->headers),"To: %s\r\nFrom: %s\r\nSubject: %s\r\n",to,from,subject);
	return BMD_OK;
}

/* ustawia kontekst */
long bmd_p7m_set_sig_ctx(bmd_crypt_ctx_t *sig_ctx,p7m_info_t **p7m)
{
	if(sig_ctx==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*p7m)==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	(*p7m)->sig_ctx=sig_ctx;
	
	return BMD_OK;
}
