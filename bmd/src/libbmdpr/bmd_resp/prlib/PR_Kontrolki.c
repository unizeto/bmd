#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>
long bmd_kontrolki_init(Kontrolki_t **kontrolki)
{
	if(kontrolki==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*kontrolki)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*kontrolki)=(Kontrolki_t *)malloc(sizeof(Kontrolki_t));
	memset(*kontrolki,0,sizeof(Kontrolki_t));

	return BMD_OK;
}

long bmd_kontrolki_create(Kontrolka_t *kontrolka,Kontrolki_t **kontrolki)
{
	if(kontrolka==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(kontrolki==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	if((*kontrolki)==NULL)
	{
		BMD_FOK(bmd_kontrolki_init(kontrolki));
	}

	asn_set_add(&((*kontrolki)->list), kontrolka);

	return BMD_OK;
}

long bmd_kontrolki_der_encode(	Kontrolki_t *kontrolki,
				GenBuf_t **output)
{
	if(kontrolki==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(asn1_encode(&asn_DEF_Kontrolki,kontrolki, NULL, output));
	return BMD_OK;
}

long bmd_kontrolki_der_decode(	GenBuf_t *input,
						Kontrolki_t **kontrolki)
{
asn_dec_rval_t rc_code;

	rc_code=ber_decode(0,&asn_DEF_Kontrolki,(void **)kontrolki,input->buf,input->size);
	if(rc_code.code!=RC_OK)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return BMD_OK;
}

long bmd_kontrolki_free(Kontrolki_t **kontrolki)
{
    asn_DEF_Kontrolki.free_struct(&asn_DEF_Kontrolki,*kontrolki,0);*kontrolki=NULL;
    return BMD_OK;
}
