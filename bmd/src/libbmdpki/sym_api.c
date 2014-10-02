#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
//#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifdef WIN32
#pragma warning(disable:4100)
#endif

long bmd_symmetric_encrypt(GenBuf_t *input,bmd_crypt_ctx_t **ctx,GenBuf_t **output,void *opt)
{
long status		= 0;
long flag		=-1;
int temp		= 0;

	if(ctx==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	BMD_FOK(bmd_check_ctx(*ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_SYM));

	if((*ctx)->sym->sym_params==BMD_SYM_SINGLE_ROUND) /* jesli jest jeden przebieg to bufor nie moze byc NULL */
	{
		if(input==NULL)
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
		if(input->buf==NULL)
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
		if(input->size<=0)
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
			return BMD_ERR_PARAM1;
		}
        }
	else
	{/* jesli wiele przebiegow to wtedy podanie NULL swiadczy o tym ze byc DigestFinal */
		if(input==NULL)
		{
			if( (*ctx)->sym->sym_stage!=BMD_SYM_STAGE_COMPLETE )
			{
				(*ctx)->sym->sym_stage=BMD_SYM_STAGE_LAST;
			}
		}
	}

        if(output==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
        if((*output)!=NULL)
        {
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	/* jednoprzebiegowe lub wieloprzebiegowe liczenie skrotu */
	if((*ctx)->sym->sym_params==BMD_SYM_SINGLE_ROUND)
	{
		flag=BMD_SYM_SINGLE_ROUND;
	}
	else
	{
		flag=BMD_SYM_MULTI_ROUND;
	}

        if((*ctx)->sym->algo_type!=BMD_CRYPT_ALGO_DES3)
	{
		PRINT_ERROR("LIBBMDPKIERR Method unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}

	/* przy pierwszym wywolaniu */
	if( (flag==BMD_SYM_SINGLE_ROUND) || ( ((*ctx)->sym->sym_stage==BMD_SYM_STAGE_FIRST) && ((*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
		status=EVP_EncryptInit(&( (*ctx)->sym->sym_ctx ), EVP_des_ede3_cbc(), NULL, NULL);
		if(status!=1)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		EVP_CIPHER_CTX_set_key_length(&( (*ctx)->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
		status=EVP_EncryptInit(&( (*ctx)->sym->sym_ctx ), NULL, (unsigned char*)((*ctx)->sym->key->buf), \
		(unsigned char*)((*ctx)->sym->IV->buf) );
		if(status!=1)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		(*ctx)->sym->sym_stage=BMD_SYM_STAGE_NORMAL;
	}

	/* normalne wywolanie */
	if( (flag==BMD_SYM_SINGLE_ROUND) || ( ((*ctx)->sym->sym_stage==BMD_SYM_STAGE_NORMAL) && \
	((*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
		(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		if((*output)==NULL)
		{
			PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY;
		}
		(*output)->buf=(char *)malloc(input->size+EVP_MAX_KEY_LENGTH);
		if((*output)->buf==NULL)
                {
			PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY;
		}
		temp = (*output)->size;
		status=EVP_EncryptUpdate(&( (*ctx)->sym->sym_ctx ), (unsigned char*)((*output)->buf), &temp, \
		(unsigned char *)(input->buf), input->size);
		(*output)->size = temp;
		if(status!=1)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}

		/* ostatnie wywolanie */
	if( (flag==BMD_SYM_SINGLE_ROUND) ||
		( ( ( (*ctx)->sym->sym_stage==BMD_SYM_STAGE_LAST ) ||
		( (*ctx)->sym->sym_stage==BMD_SYM_STAGE_COMPLETE) )
		&& ((*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
		if((*ctx)->sym->sym_stage!=BMD_SYM_STAGE_COMPLETE)
		{
			temp = (*ctx)->sym->last_block_length;
			status=EVP_EncryptFinal(&( (*ctx)->sym->sym_ctx ), (unsigned char*)((*ctx)->sym->last_block), &temp);
			(*ctx)->sym->last_block_length = temp;
			if(status!=1)
			{
				PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
		}
		if((*ctx)->sym->last_block_length)
		{
			if(flag==BMD_SYM_SINGLE_ROUND)
			{
				memmove((*output)->buf+(*output)->size,(*ctx)->sym->last_block,(*ctx)->sym->last_block_length);
				(*output)->size+=(*ctx)->sym->last_block_length;
			}
			else
			{
				set_gen_buf2((*ctx)->sym->last_block,(*ctx)->sym->last_block_length,output);
			}
			(*ctx)->sym->padding_present=BMD_SYM_PADDING_PRESENT;
		}
		else
		{
			if(flag==BMD_SYM_MULTI_ROUND)
			{
				output=NULL;
			}
			(*ctx)->sym->padding_present=BMD_SYM_PADDING_ABSENT;
		}
		EVP_CIPHER_CTX_cleanup(&( (*ctx)->sym->sym_ctx ));

		if(flag==BMD_SYM_MULTI_ROUND)
		{
			(*ctx)->sym->sym_stage=BMD_SYM_STAGE_COMPLETE;
		}
	}
	return BMD_OK;
}

long bmd_symmetric_decrypt(	GenBuf_t *input,
				long twf_offset,
				bmd_crypt_ctx_t **ctx,
				GenBuf_t **output,
				void *opt)
{
long status			= 1;
long flag			=-1;
int temp			= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(ctx==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_check_ctx(*ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_SYM));

	if((*ctx)->sym->sym_params==BMD_SYM_SINGLE_ROUND) /* jesli jest jeden przebieg to bufor nie moze byc NULL */
	{
		if(input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
		if(input->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
		if(input->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	}
	else    /* jesli wiele przebiegow to wtedy podanie NULL swiadczy o tym ze byc DigestFinal */
	{
		if(input==NULL)
		{
			if((*ctx)->sym->sym_stage!=BMD_SYM_STAGE_COMPLETE)
			{
				(*ctx)->sym->sym_stage=BMD_SYM_STAGE_LAST;
			}
		}
	}
	if(twf_offset < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_offset >= input->size)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(output==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}

	/* jednoprzebiegowe lub wieloprzebiegowe liczenie skrotu */
	if((*ctx)->sym->sym_params==BMD_SYM_SINGLE_ROUND)
	{
		flag=BMD_SYM_SINGLE_ROUND;
	}
	else
	{
		flag=BMD_SYM_MULTI_ROUND;
	}

	if((*ctx)->sym->algo_type!=BMD_CRYPT_ALGO_DES3)
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	/* przy pierwszym wywolaniu */
	if( (flag==BMD_SYM_SINGLE_ROUND) || ( ((*ctx)->sym->sym_stage==BMD_SYM_STAGE_FIRST) && ((*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
	    /*PRINT_ERROR("Pierwsze wywolanie MULTIROUND lub INIT DLA SYM_SINGLE 1\n");*/
		status=EVP_DecryptInit(&( (*ctx)->sym->sym_ctx ), EVP_des_ede3_cbc(), NULL, NULL);
		if(status!=1)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		/*PRINT_ERROR("Pierwsze wywolanie MULTIROUND lub INIT DLA SYM_SINGLE 2\n");*/
		EVP_CIPHER_CTX_set_key_length(&( (*ctx)->sym->sym_ctx ), EVP_CIPHER_key_length(EVP_des_ede3_cbc()));
		status=EVP_DecryptInit(	&( (*ctx)->sym->sym_ctx ), NULL, (unsigned char*)((*ctx)->sym->key->buf), \
					(unsigned char*)((*ctx)->sym->IV->buf));
		if(status!=1)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		/*PRINT_ERROR("Pierwsze wywolanie MULTIROUND lub INIT DLA SYM_SINGLE 3\n");*/
		(*ctx)->sym->sym_stage=BMD_SYM_STAGE_NORMAL;
	}

	/* normalne wywolanie */
	if( (flag==BMD_SYM_SINGLE_ROUND) || ( ((*ctx)->sym->sym_stage==BMD_SYM_STAGE_NORMAL) && ((*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
		(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		(*output)->buf=(char *)malloc(input->size - twf_offset + EVP_MAX_KEY_LENGTH);
		temp = (*output)->size;
		status=EVP_DecryptUpdate(&( (*ctx)->sym->sym_ctx ), (unsigned char*)((*output)->buf), &temp, \
		(unsigned char*)(input->buf + twf_offset), input->size - twf_offset);
		(*output)->size = temp;
		if(status!=1)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}
	}

		/* ostatnie wywolanie */
	if( 	(flag==BMD_SYM_SINGLE_ROUND) ||
		(	( 	( 	(*ctx)->sym->sym_stage==BMD_SYM_STAGE_LAST ) ||
				( 	(*ctx)->sym->sym_stage==BMD_SYM_STAGE_COMPLETE) ) &&
				(	(*ctx)->sym->sym_params==BMD_SYM_MULTI_ROUND) ) )
	{
		if((*ctx)->sym->sym_stage!=BMD_SYM_STAGE_COMPLETE)
		{
			temp = (*ctx)->sym->last_block_length;
			status=EVP_DecryptFinal(&( (*ctx)->sym->sym_ctx ), (unsigned char*)((*ctx)->sym->last_block), &temp);
			(*ctx)->sym->last_block_length = temp;
			if(status!=1)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}
		}

		if((*ctx)->sym->last_block_length)
		{
			if(flag==BMD_SYM_SINGLE_ROUND)
			{
				memmove((*output)->buf+(*output)->size,(*ctx)->sym->last_block,(*ctx)->sym->last_block_length);
				(*output)->size+=(*ctx)->sym->last_block_length;
			}
			else
			{
				BMD_FOK(set_gen_buf2((*ctx)->sym->last_block,(*ctx)->sym->last_block_length,output));
			}
			(*ctx)->sym->padding_present=BMD_SYM_PADDING_PRESENT;
		}
		else
		{
			if(flag==BMD_SYM_MULTI_ROUND)
			{
				output=NULL;
			}
			(*ctx)->sym->padding_present=BMD_SYM_PADDING_ABSENT;
		}
		EVP_CIPHER_CTX_cleanup(&( (*ctx)->sym->sym_ctx ));

		if(flag==BMD_SYM_MULTI_ROUND)
		{
			(*ctx)->sym->sym_stage=BMD_SYM_STAGE_COMPLETE;
		}
	}

	return BMD_OK;
}
