#define _WINSOCK2API_
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagramSet.h>
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include<bmd/libbmderr/libbmderr.h>

/* Tworzymy zbior datagramow */
long _PR2_create_bmdDatagram_set(bmdDatagramSet_t **bmdDatagramSet)
{
long err			= 0;
	*bmdDatagramSet = (bmdDatagramSet_t *) malloc (sizeof(bmdDatagramSet_t));
	if(*bmdDatagramSet == NULL){
		return NO_MEMORY;
	}
	err = PR_bmdDatagramSet_init(*bmdDatagramSet);
	if(err < 0){
		return err;
	}
	return err;
}

/* Dodajemy datagram do zbioru.
   UWAGA!!! Kopiowany jest tylko wskaznik wiec nie nalezy niszczyc oryginalu
	    bmdDatagram gdyz staje sie on czescia bmdDatagramSet
*/
long _PR2_add_bmdDatagram_to_set(bmdDatagram_t *bmdDatagram, bmdDatagramSet_t *bmdDatagramSet)
{
long err			= 0;
bmdDatagram_t **tmp	= NULL;

	if(bmdDatagram == NULL || bmdDatagramSet == NULL)
	{
		return NO_ARGUMENTS;
	}

	tmp = (bmdDatagram_t **) realloc ( bmdDatagramSet->bmdDatagramSetTable, (1+bmdDatagramSet->bmdDatagramSetSize) * sizeof(bmdDatagram_t *));
	if(tmp == NULL){
		/* TODO wyslij bmdDatagramSet->bmdDatagramSetTable */
		BMD_FOK(NO_MEMORY);
	}
	else
	{
		bmdDatagramSet->bmdDatagramSetTable = tmp;
	}

	/******************************/
	/* !!!podpinam tylko bufor!!! */
	/******************************/
	bmdDatagramSet->bmdDatagramSetTable[bmdDatagramSet->bmdDatagramSetSize] = bmdDatagram;
	(bmdDatagramSet->bmdDatagramSetSize)++;
	return err;
}

/* Zwalniacz struktury bmdDatagramSet */
long _PR_bmdDatagramSet_free(bmdDatagramSet_t *bmdDatagramSet, long with_pointer)
{
long err = 0;
long i	= 0;

	if(bmdDatagramSet == NULL)
	{
		//PRINT_ERROR("LIBBMDPRERR Invalid pointer to be freed. Error=%i\n",NO_ARGUMENTS);
		return NO_ARGUMENTS;
	}

	/* Zwolnij elementy tablicy */

	for(i=0; i<(long)bmdDatagramSet->bmdDatagramSetSize; i++)
	{
		/*Dodane bo w czasie kodowania zwalniam datagramy w trakcie dla oszczedzenia pamieci*/
		if( bmdDatagramSet->bmdDatagramSetTable[i] != NULL)
		{
			err = PR2_bmdDatagram_free(&(bmdDatagramSet->bmdDatagramSetTable[i]));
			if(err < 0)
			{
				return err;
			}
		}
	}

	/* Zwolnij tablice */
	if(bmdDatagramSet->bmdDatagramSetTable)
	{

		free(bmdDatagramSet->bmdDatagramSetTable);
		bmdDatagramSet->bmdDatagramSetTable = NULL;
	}
	/* Zwolnij opcjonalnie strukture */

	if(with_pointer)
	{

		if(bmdDatagramSet) {free(bmdDatagramSet); bmdDatagramSet=NULL;}
	}
	return err;
}

long _PR2_bmdDatagramSet_der_encode(BMDDatagramSet_t *set, GenBuf_t *kn, GenBuf_t **output)
{
	long status;

	if(set==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(kn==NULL)		{       BMD_FOK(BMD_ERR_PARAM2);        }
	if(output==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	status=asn1_encode(&asn_DEF_BMDDatagramSet, set, kn, output);
	if( status != BMD_OK )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	return BMD_OK;
}

long _PR2_bmdDatagramSet_der_decode(	GenBuf_t **input,
					long twf_prefix,
					BMDDatagramSet_t **set,
					long twf_input_free)
{
	asn_dec_rval_t rc_code;

	if(input==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)->size <= twf_prefix)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)->buf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(twf_prefix < 0)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(set==NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*set)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(twf_input_free < 0 || twf_input_free > 1)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	rc_code=ber_decode(	0,
				&asn_DEF_BMDDatagramSet,
				(void **)set,
				(*input)->buf + twf_prefix,
				(*input)->size - twf_prefix);
    	if(rc_code.code!=RC_OK)				{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	if(twf_input_free == 1)
	{
		free_gen_buf(input);
	}

	return BMD_OK;
}
