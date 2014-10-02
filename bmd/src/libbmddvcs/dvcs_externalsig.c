
/*Autor: Włodzimierz Szczygieł ( Dział Programowania, Unizeto Technologies SA )*/

#include <bmd/libbmddvcs/libbmddvcs.h>
#include <bmd/libbmderr/libbmderr.h>

long _create_HTTPbody_externalSig(GenBuf_t* DVCSrequest, GenBuf_t *document, GenBuf_t** tsAttachments, long tsAttachmentsCount, char *boundary, GenBuf_t **output)
{
	long ret_val=0;
	long iter = 0;
	GenBuf_t static_gb;
	GenBuf_t *contents=NULL;
	GenBuf_t *tmp=NULL;
	GenBuf_t *tmp1=NULL;

	if(DVCSrequest == NULL)
		{ return -1; }
	if(DVCSrequest->buf == NULL && DVCSrequest->size > 0)
		{ return -2; }
	if(document == NULL)
		{ return -3; }
	if(document->buf == NULL && document->size > 0)
		{ return -4; }
	if(boundary == NULL)
		{ return -5; }
	if( (strlen(boundary)) < 1)
		{ return -6; }
	if(output == NULL)
		{ return -7; }
	if(*output != NULL)
		{ return -8; }
	if(tsAttachmentsCount < 0)
		{ return -9; }
	if(tsAttachments == NULL && tsAttachmentsCount > 0)
		{ return -10; }

	ret_val = asprintf((char**)&(static_gb.buf), "--%s\r\nContent-Disposition: form-data; name=\"request\"; filename=\"\"\r\nContent-Type: application/dvcs; charset=\"utf-8\"\r\nContent-Transfer-Encoding: binary\r\n\r\n", boundary);
	if(ret_val < 0)
		{ return -21; }
	static_gb.size=(long)strlen((char*)(static_gb.buf));
	
	ret_val=bmd_merge_genbufs(&static_gb, DVCSrequest, &tmp);
	free(static_gb.buf); static_gb.buf=NULL; static_gb.size=0;
	if(ret_val != 0)
		{ return -22; }
	
	ret_val = asprintf((char**)&(static_gb.buf), "\r\n--%s\r\nContent-Disposition: form-data; name=\"document\"; filename=\"\"\r\nContent-Type: application/octet-stream; charset=\"utf-8\"\r\nContent-Tranfer-Encoding: binary\r\n\r\n", boundary);
	if(ret_val < 0)
	{
		free_gen_buf(&tmp);
		return -23;
	}
	static_gb.size=(long)strlen((char*)(static_gb.buf));
	
	ret_val=bmd_merge_genbufs(tmp, &static_gb, &tmp1);
	free(static_gb.buf); static_gb.buf=NULL; static_gb.size=0;
	free_gen_buf(&tmp);
	if(ret_val != 0)
		{ return -24; }

	ret_val=bmd_merge_genbufs(tmp1, document, &tmp);
	free_gen_buf(&tmp1);
	if(ret_val != 0)
		{ return -25; }
	
	// zalaczanie znacznikow czasu
	//Content-Type: application/timestamp-token; charset="utf-8"
	//Content-Tranfer-Encoding: binary
	for(iter=0; iter<tsAttachmentsCount; iter++)
	{
		ret_val = asprintf((char**)&(static_gb.buf), "\r\n--%s\r\nContent-Type: application/timestamp-token; charset=\"utf-8\"\r\nContent-Transfer-Encoding: binary\r\n\r\n", boundary);
		if(ret_val < 0)
		{
			free_gen_buf(&tmp);
			return -26;
		}
		static_gb.size=(long)strlen((char*)(static_gb.buf));
		
		ret_val=bmd_merge_genbufs(tmp, &static_gb, &tmp1);
		free(static_gb.buf); static_gb.buf=NULL; static_gb.size=0;
		free_gen_buf(&tmp);
		if(ret_val != 0)
			{ return -27; }
		
		ret_val=bmd_merge_genbufs(tmp1, tsAttachments[iter], &tmp);
		free_gen_buf(&tmp1);
		if(ret_val != 0)
			{ return -28; }
	}
	
	
	ret_val = asprintf((char**)&(static_gb.buf), "\r\n--%s--", boundary );
	if(ret_val < 0)
	{
		free_gen_buf(&tmp);
		return -29;
	}
	static_gb.size=(long)strlen((char*)(static_gb.buf));
	ret_val=bmd_merge_genbufs(tmp, &static_gb, &contents);
	free(static_gb.buf); static_gb.buf=NULL; static_gb.size=0;
	free_gen_buf(&tmp);
	if(ret_val != 0)
		{ return -30; }
	
	*output=contents;
	return 0;
	
}

long call_DVCSservice_externalSig(GenBuf_t* document, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result)
{
	struct curl_slist *headers=NULL;
	CURL *curl_handle=NULL;
	CURLcode curl_err_code;

	GenBuf_t *request=NULL;
	GenBuf_t chunk;
	GenBuf_t *body_extern=NULL;
	long ret_val=0;
	char *boundary="unizetoboundary";
	char *tmp=NULL;
	DVCSResponse_t *response=NULL;
	long resp_stat=0;
	long iter=0;
	long num_err_strings=0;
	char *err_string=NULL; //blad podczas przetwarzania zadania DVCS
	char *err_tmp=NULL;
	long httpCode=0;
	

	if(document == NULL)
		{ return -1; }
	if(document->buf == NULL && document->size > 0)
		{ return -2; }
	if(context == NULL)
		{ return -3; }
	if(input == NULL)
		{ return -4; }
	if(result == NULL)
		{ return -5; }
	

	free_gen_buf(&(input->dvcsCert));
	free(input->errorString); input->errorString=NULL;
	chunk.buf=NULL;
	chunk.size=0;

	ret_val=_prepare_DVCSReq_to_send(input, SERVICE_TYPE_VSD, context, &request, 0);
	if(ret_val != 0)
		{ return -6; }

	ret_val=_create_HTTPbody_externalSig(request, document, input->tsAttachments, input->tsAttachmentsCount, boundary, &body_extern);
	free_gen_buf(&request);
	if(ret_val != 0)
		{ return -7; }
	
//assert(bmd_save_buf(body_extern, "body_extern.txt") == 0);

	curl_handle = curl_easy_init();
	if(curl_handle == NULL)
	{
		free_gen_buf(&body_extern);
		return -8;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback);
	if(ret_val != CURLE_OK )
	{
		curl_easy_cleanup(curl_handle);
		free_gen_buf(&body_extern);
		return -9;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	if(ret_val != CURLE_OK )
	{
		curl_easy_cleanup(curl_handle);
		free_gen_buf(&body_extern);
		return -10;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POST, 0);
	if(ret_val != CURLE_OK)
	{
		curl_easy_cleanup(curl_handle);
		free_gen_buf(&body_extern);
		return -11;
	}
	
	asprintf(&tmp ,"Content-Type: multipart/mixed; boundary=%s", boundary);
	headers=curl_slist_append(headers, tmp);
	free(tmp);
	if(headers == NULL)
	{
		curl_easy_cleanup(curl_handle);
		free_gen_buf(&body_extern);
		return -12;
	}
	
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	if(ret_val != CURLE_OK)
	{
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headers);
		free_gen_buf(&body_extern);
		return -13;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body_extern->buf);
	if(ret_val != CURLE_OK )
	{
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headers);
		free_gen_buf(&body_extern);
		return -14;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, body_extern->size);
	if(ret_val != CURLE_OK)
	{
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headers);
		free_gen_buf(&body_extern);
		return -15;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_URL,input->url);
	if(ret_val != CURLE_OK)
	{
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headers);
		free_gen_buf(&body_extern);
		return -16;
	}

	if(input->connectionTimeout > 0)
	{
		ret_val=curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, input->connectionTimeout);
		if(ret_val!=CURLE_OK)
			{ return -17; }
	}

	curl_err_code=curl_easy_perform(curl_handle);
	free_gen_buf(&body_extern);
	if( curl_err_code != 0 )
	{
		//printf("libbmddvcs external signature sending ERROR (CURL): %s\n", curl_easy_strerror(curl_err_code));
		curl_easy_cleanup(curl_handle);
		curl_slist_free_all(headers);
		return -18;
	}

	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &httpCode);
	asprintf( &(input->errorString), "HTTP RESPONSE CODE: %li", httpCode );

//assert(bmd_save_buf(&chunk, "extern.html") == 0);
	
	curl_easy_cleanup(curl_handle);
	curl_slist_free_all(headers);

	// ponizej interpretacja odpowiedzi

	ret_val=decode_DVCSResponse(&chunk, &response);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in decoding DVCS response. Error=%i\n",-19);
		return -19;
	}

	//przypisywanie poswiadczenia
	ret_val=set_gen_buf2(chunk.buf, chunk.size, &(input->dvcsCert));
	if(ret_val != BMD_OK)
		{ return -20; }
	chunk.buf=NULL;
	chunk.size=0;
	
	
	ret_val=_get_respStatus(response, &resp_stat);
	if(ret_val != 0)
	{
		if(response->present == DVCSResponse_PR_dvErrorNote)
		{
			if(response->choice.dvErrorNote.transactionStatus.statusString != NULL)
			{
				num_err_strings=response->choice.dvErrorNote.transactionStatus.statusString->list.count;
				for(iter=0; iter<num_err_strings; iter++)
				{
					if(iter == 0)
					{
						asprintf(&err_tmp, "%s\nDVCS ERROR NOTICE: ", input->errorString); //kod odpowiedzi HTTP
						free(input->errorString);
						input->errorString=NULL;
					}
					asprintf(&err_string, "%s%s\n", err_tmp,  response->choice.dvErrorNote.transactionStatus.statusString->list.array[iter]->buf);
					free(err_tmp);
					err_tmp=err_string;
				}
				err_tmp=NULL;
			}
			if(err_string != NULL)
			{
				free(input->errorString);
				input->errorString=err_string;
			}
		}
		PRINT_ERROR("LIBBMDDVCSERR Error in getting response status. Error=%i\n",-21);
		_destroy_DVCSResponse(&response);
		return -21;
	}
	
	_destroy_DVCSResponse(&response);
	
	if(resp_stat > 1)
	{
		*result=-1;
	}
	else //ok
	{
		*result=resp_stat; //0 lub 1 odpowiednio dla kwalifikowanego lub zwyklego
	}
	return 0;
}


long _destroy_tsAttachments(bmdDVCS_t *input)
{
long iter		= 0;

	if(input == NULL)
		{ return -1; }
	if(input->tsAttachmentsCount > 0 && input->tsAttachments == NULL)
		{ return -2; }
	
	if(input->tsAttachmentsCount > 0)
	{
		for(iter=0; iter<input->tsAttachmentsCount; iter++)
			{ free_gen_buf(&(input->tsAttachments[iter])); }
		free(input->tsAttachments);
		input->tsAttachments = NULL;
		input->tsAttachmentsCount = 0;
	}
	
	return 0;
}

long _add_tsAttachment(bmdDVCS_t *input, GenBuf_t *timestampAttachment)
{
long retVal						= 0;
GenBuf_t *timestampCopy			= NULL;
GenBuf_t **tempAttachments		= NULL;

	if(input == NULL)
		{ return -1; }
	if(input->tsAttachmentsCount < 0)
		{ return -2; }
	if(timestampAttachment == NULL)
		{ return -3; }
	if(timestampAttachment->buf == NULL || timestampAttachment->size <= 0)
		{ return -4; }
	
	retVal = set_gen_buf2(timestampAttachment->buf, timestampAttachment->size, &timestampCopy);
	if(retVal < BMD_OK)
		{ return -11; }
	
	tempAttachments=(GenBuf_t**)realloc(input->tsAttachments, (input->tsAttachmentsCount+1) * sizeof(GenBuf_t*));
	if(tempAttachments == NULL)
	{
		free_gen_buf(&timestampCopy);
		return -12;
	}
	
	tempAttachments[input->tsAttachmentsCount] = timestampCopy;
	timestampCopy = NULL;
	
	input->tsAttachmentsCount++ ;
	input->tsAttachments = tempAttachments;
	tempAttachments = NULL;
		
	return 0;
}

