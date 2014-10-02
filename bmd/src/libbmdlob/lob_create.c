#include<bmd/libbmderr/libbmderr.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdlob/libbmdlob.h>

/* funkcja tworzy nowego jednostke transportowa LOB'A */
long new_lob_transport(	struct lob_transport **lob,
				bmdnet_handler_ptr * net_handler,
				bmd_crypt_ctx_t * enc_ctx,
				bmd_crypt_ctx_t * dec_ctx,
				bmd_crypt_ctx_t * hash_ctx,
				ts_info_t * ts_info,
				u_int64_t options)
{
	PRINT_INFO("LIBBMDLOBINF Setting nem lob transport\n");
	if (lob==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*lob)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (net_handler==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*net_handler)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*lob)=(lob_transport_t *)malloc(sizeof(lob_transport_t));
	if ((*lob)==NULL)		{	BMD_FOK(NO_MEMORY);	}

	memset((*lob), 0, sizeof(lob_transport_t));

	(*lob)->handler = (*net_handler);
	(*lob)->options=(int)options;


	(*lob)->enc_ctx=enc_ctx;    /* szyfrowanie */
	(*lob)->dec_ctx=dec_ctx;    /* deszyfrowanie */
	(*lob)->hash_ctx=hash_ctx;  /* liczenie skrotu */

	/* ustawienie ze liczenie skrotu jest wielorundowe */
	if (hash_ctx)
	{
		(*lob)->hash_ctx->hash->hash_params=BMD_HASH_MULTI_ROUND;
	}

	/* opcje ustawiamy zawsze - czasem bowiem jest potrzeba liczyc sam skrot bez liczenia podpisu */
	(*lob)->options|=options;
	(*lob)->ts_info = ts_info;

	return BMD_OK;
}

long free_lob_transport(struct lob_transport **lob)
{
	if (lob==NULL)
	    return BMD_ERR_PARAM1;
	if ((*lob)==NULL)
	    return BMD_ERR_PARAM1;

	/* nie niszcze kontekstow - to ma byc robione poza ta funkcje - bo przekazuje przez jedna "*" */
	/*
	bmd_ctx_destroy(&((*lob)->enc_ctx));
	bmd_ctx_destroy(&((*lob)->dec_ctx));
	bmd_ctx_destroy(&((*lob)->hash_ctx));
	*/

	free(*lob);(*lob)=NULL;

	return BMD_OK;
}
