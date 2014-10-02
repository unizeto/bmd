#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/* przygotowuje naglowek wiadomosci P7M */
long _bmd_p7m_prepare_signed_message_prefix(p7m_info_t *p7m,char **prefix)
{
    return BMD_OK;
}

/* przygotowuje naglowek wewnetrzny wiadomosci P7M */
long _bmd_p7m_prepare_internal_prefix(p7m_info_t *p7m,char **prefix)
{
    return BMD_OK;
}

/* przygotowuje mime content do podpisania */
long _bmd_p7m_prepare_content_to_sign(p7m_info_t **p7m)
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
	if((*p7m)->internal_prefix==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*p7m)->body==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	asprintf(&((*p7m)->content_to_sign),"%s\r\n%s",(*p7m)->internal_prefix,(*p7m)->body);
	
	return BMD_OK;
}

/* podpisuje wiadomosc zawarta w "input" w formacie P7M - wynik umieszcza w "output" */
long bmd_p7m_sign_content(p7m_info_t **p7m)
{
GenBuf_t tmp_input;
GenBuf_t *tmp_output=NULL;
	
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
	
	if((*p7m)->content_to_sign==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	tmp_input.buf=(char *)(*p7m)->content_to_sign;
	tmp_input.size=(long)strlen((*p7m)->content_to_sign);
	
	BMD_FOK(bmd_sign_data(&tmp_input,(*p7m)->sig_ctx,&tmp_output,NULL));
	
	/* zakodowanie w BASE64 podpisu */
	(*p7m)->signed_content=(char *) spc_base64_encode((unsigned char *)tmp_output->buf,tmp_output->size,1);
	
	free_gen_buf(&tmp_output);
	
	return BMD_OK;
}

long bmd_p7m_prepare_email_body(p7m_info_t *p7m,char **output)
{
	if(p7m==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p7m->headers==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p7m->signed_message_prefix==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p7m->signed_content==NULL)
	{
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	asprintf(output,"%s%s\r\n%s\r\n",p7m->headers,p7m->signed_message_prefix,p7m->signed_content);
	
	return BMD_OK;
}
