#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>


long __bmd_genbuf_to_int(	GenBuf_t *gb,
					uint32_t *result)
{
BIGNUM *tmp=NULL;
char *tmp1=NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Converting genbuf to int\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(gb==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(gb->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(result==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*result)=0;

	tmp=BN_bin2bn((unsigned char*)gb->buf,gb->size,NULL);
	if(tmp==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	tmp1=BN_bn2dec(tmp);
	if(tmp1==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	PRINT_ERROR("tmp1 %s\n",tmp1);
	*result=(uint32_t)atoi(tmp1);

	BN_clear_free(tmp);tmp=NULL;
	OPENSSL_free(tmp1);tmp1=NULL;

	return BMD_OK;
}

long obsolote___bmd_wrap_with_octet_string(	GenBuf_t **input,
								GenBuf_t *twf_prefix,
								GenBuf_t **output)
{
OCTET_STRING_t *tmp_octet	= NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Wrapping with octet string\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(input == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	};
	if((*input) == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	};
	if((*input)->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	};
	if((*input)->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	};
	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	};
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	};

	tmp_octet=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
	if( tmp_octet == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset(tmp_octet,0,sizeof(OCTET_STRING_t));
	BMD_FOK(OCTET_STRING_fromBuf(tmp_octet,(char *)(*input)->buf,(int)(*input)->size));
	free_gen_buf(input);

	BMD_FOK(asn1_encode(&asn_DEF_OCTET_STRING, tmp_octet, twf_prefix, output));

	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,tmp_octet,0);
	tmp_octet=NULL;

	return BMD_OK;
}

long obsolote___bmd_unwrap_from_octet_string(	GenBuf_t *input,
								GenBuf_t **output)
{
OCTET_STRING_t *tmp_octet	= NULL;
asn_dec_rval_t rc_code;

	PRINT_INFO("LIBBMDPROTOCOLINF Unwrapping from octet string\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	rc_code=ber_decode(0,&asn_DEF_OCTET_STRING,(void **)&tmp_octet,input->buf,input->size);
	if(rc_code.code!=RC_OK)	{	BMD_FOK(BMD_ERR_FORMAT);	}

	BMD_FOK(set_gen_buf2((char *)(tmp_octet->buf),tmp_octet->size,output));
	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,tmp_octet,0);tmp_octet=NULL;

	return BMD_OK;
}

long bmd2_create_session_request(	GenBuf_t **twf_input_buffers,
					long twf_no_of_buffers,

					bmd_crypt_ctx_t *sym_ctx,
					uint32_t request_id,
					GenBuf_t ***twf_output_buffers,
					long *twf_no_of_output_buffers)
{
GenBuf_t *twl_output			= NULL;
GenBuf_t **twl_output_array		= NULL;

long i				= 0;
long twl_buffers_size		= 0;
long twl_stage			= 0;
long twl_params			= 0;

	PRINT_INFO("LIBBMDPROTOCOLINF Creating session request\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(twf_input_buffers == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(*twf_input_buffers == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(twf_no_of_buffers <= 0)					{	BMD_FOK(BMD_ERR_PARAM2);	}


	if(sym_ctx==NULL)						{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(twf_output_buffers==NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}
	if((*twf_output_buffers)!=NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}

	if(twf_no_of_output_buffers == NULL)				{	BMD_FOK(BMD_ERR_PARAM6);	}

	twl_output_array = malloc((twf_no_of_buffers  + 1) * sizeof(GenBuf_t *));
	if(twl_output_array == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset(twl_output_array, 0, (twf_no_of_buffers + 1) * sizeof(GenBuf_t *));


	twl_params = sym_ctx->sym->sym_params;
        twl_stage = sym_ctx->sym->sym_stage;

	sym_ctx->sym->sym_params = BMD_SYM_MULTI_ROUND;
	sym_ctx->sym->sym_stage = BMD_SYM_STAGE_FIRST;

	for(i = 0; i < twf_no_of_buffers; i++)
	{
		BMD_FOK(bmd_symmetric_encrypt(twf_input_buffers[i],
		&sym_ctx, &twl_output, NULL));
		free_gen_buf(&(twf_input_buffers[i]));

		PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG iteration %li, encrypted size: %li\n", i, twl_output->size);

		twl_buffers_size = twl_buffers_size + twl_output->size;
		twl_output_array[i] = twl_output;
		twl_output = NULL;

		sym_ctx->sym->sym_stage = BMD_SYM_STAGE_NORMAL;
	}
	sym_ctx->sym->sym_stage = BMD_SYM_STAGE_LAST;

	BMD_FOK(bmd_symmetric_encrypt(NULL, &(sym_ctx), &twl_output, NULL));
	twl_buffers_size = twl_buffers_size + twl_output->size;
	twl_output_array[i] = twl_output;
	twl_output = NULL;

	sym_ctx->sym->sym_params = twl_params;
	sym_ctx->sym->sym_stage = twl_stage;

	PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG twl_buffers_size %li\n", twl_buffers_size);
	/*Prefix jest w pierwszym buforze!*/
	BMD_FOK(set_prefix_from_int(request_id, twl_buffers_size, &(twl_output_array[0]) ));
	PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG twl_output_array[0]->size: %li\n", twl_output_array[0]->size);


	*twf_output_buffers = twl_output_array;
	twl_output_array = NULL;

	/*Bo w ostatnim dodatkowym buforze jest pading*/
	*twf_no_of_output_buffers = twf_no_of_buffers  + 1;

	return BMD_OK;
}

long __int_to_gb(	uint32_t value,
			GenBuf_t **gb)
{
char *hex_value	= NULL;
BIGNUM *tmp_bn	= NULL;
long status		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(gb==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*gb)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	asprintf(&hex_value,"%02X",value);
	if(hex_value==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	status=BN_hex2bn(&tmp_bn,hex_value);
	if(status==0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	(*gb)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if((*gb)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	(*gb)->size=BN_num_bytes(tmp_bn);
	(*gb)->buf=(char *)malloc((*gb)->size);
	if((*gb)->buf==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	status=BN_bn2bin(tmp_bn, (unsigned char*)(*gb)->buf);
	if(status<=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	(*gb)->size=status;
	BN_free(tmp_bn);tmp_bn=NULL;
	free(hex_value);hex_value=NULL;

	return BMD_OK;
}

long bmd2_decode_session_request(	GenBuf_t **input,
					GenBuf_t **cert_login_as,
					long twf_offset,
					bmd_crypt_ctx_t *dec_ctx,
					GenBuf_t *cache_ki_mod_n,
					GenBuf_t *i,
					GenBuf_t *n,
					GenBuf_t **new_ki_mod_n,
					bmdDatagramSet_t **dtg,
					long *twf_bmdDatagramsTransmissionCount,
					long *twf_bmdDatagramsAllCount,
					long twf_input_free,
					long *twf_memory_guard,
					long deserialisation_max_memory)
{
long bmdDatagramsTransmissionCount	= 0;
long bmdDatagramsAllCount		= 0;

GenBuf_t *tmp				= NULL;
GenBuf_t *req_ki_gb			= NULL;
GenBuf_t *to_compare_ki_mod_n		= NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Decoding session request\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*(input)==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)->size<= twf_offset)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dec_ctx==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (cache_ki_mod_n==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (cache_ki_mod_n->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (cache_ki_mod_n->size<=0)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (deserialisation_max_memory<0)	{	BMD_FOK(BMD_ERR_PARAM14);	}

	if(new_ki_mod_n != NULL)
	{
		/*Jezeli chcemy otrzymac nowa wartosc to bufor wyjsciowy musi byc pusty*/
		if(*new_ki_mod_n != NULL)
		{
			BMD_FOK(BMD_ERR_PARAM7);
		}
	}

	if(dtg==NULL)							{	BMD_FOK(BMD_ERR_PARAM8);	}
	if((*dtg)!=NULL)							{	BMD_FOK(BMD_ERR_PARAM4);	}

	if(twf_bmdDatagramsTransmissionCount == NULL)		{	BMD_FOK(BMD_ERR_PARAM9);	}
	if(twf_bmdDatagramsAllCount == NULL)			{       BMD_FOK(BMD_ERR_PARAM10);	}

	if(twf_input_free < 0 || twf_input_free > 1)		{	BMD_FOK(BMD_ERR_PARAM11);	}
	if(twf_memory_guard == NULL)					{	BMD_FOK(BMD_ERR_PARAM13);	}
	if(*twf_memory_guard < 0)					{	BMD_FOK(BMD_ERR_PARAM13);	}

	BMD_FOK(bmd_symmetric_decrypt(*input, twf_offset, &dec_ctx, &tmp, NULL));
	if(twf_input_free == 1)
	{
		free_gen_buf(input);
	}

	/*free_gen_buf(&tmp1);*/
	set_gen_buf2(tmp->buf,BMD_BN_BYTE_SIZE,&req_ki_gb);

	BMD_FOK(bmd_compute_ki_mod_n(cache_ki_mod_n, i, n, &to_compare_ki_mod_n));

	/*PRINT_ERROR("req_ki_gb->size  %i cache_ki_mod_n->size  %i\n",req_ki_gb->size,cache_ki_mod_n->size);*/
	BMD_FOK(bmd_compare_bn(req_ki_gb, to_compare_ki_mod_n));

	if(new_ki_mod_n != NULL)
	{
		set_gen_buf2(to_compare_ki_mod_n->buf,to_compare_ki_mod_n->size,new_ki_mod_n);
	}

	BMD_FOK(PR2_bmdDatagramSet_deserialize(	&tmp,
						BMD_BN_BYTE_SIZE,
						dtg,
						&bmdDatagramsTransmissionCount,
						&bmdDatagramsAllCount,
						twf_input_free,
						twf_memory_guard,
						deserialisation_max_memory));


	if ((cert_login_as!=NULL) && (*cert_login_as)==NULL)
	{
		BMD_FOK(bmd2_datagram_get_cert_login_as((*dtg)->bmdDatagramSetTable[0],cert_login_as));
	}

	*twf_bmdDatagramsTransmissionCount = bmdDatagramsTransmissionCount;
	*twf_bmdDatagramsAllCount = bmdDatagramsAllCount;

	free_gen_buf(&req_ki_gb);
	free_gen_buf(&to_compare_ki_mod_n);

	return BMD_OK;
}
