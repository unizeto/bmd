#include<bmd/libbmderr/libbmderr.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdlob/libbmdlob.h>

/* modified by WSZ*/
long send_lob_chunk(	struct lob_transport **lob,
				GenBuf_t ** lob_data,
				long is_last)
{
GenBuf_t *tmp_dec			= NULL;
GenBuf_t *tmp_enc			= NULL;
GenBuf_t send_buf;
u_int32_t *tmp_send		= NULL;
u_int32_t *send_buf_offset	= NULL;
long status				= 0;
long zdeszyfrowane_dlugosc	= 0; /*ta zmienna okresla, ile oryginalnych danych zostaje wyslanych
								 po zaszyfrowanie przewaznie jest ich wiecej ze wzgledu na padding*/

	PRINT_INFO("LIBBMDLOBINF Sending lob chunk\n");
	memset(&send_buf,0,sizeof(GenBuf_t));
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(lob==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*lob)==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( (*lob)->enc_ctx == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( (*lob_data)->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(lob_data == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*lob_data == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (is_last!=0) && (is_last!=1) )	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/* zdeszyfruj jesli ustawiono kontekst - w przypadku serwera nastepuje przeszyfrowanie
		czyli najpierw deszyfracja symetryczna a nastepnie szyfrowanie symetryczne*/
	if((*lob)->dec_ctx)
	{
		/*PRINT_INFO("LIBBMDLOBINF kontekst deszyfrujący obecny!!!!!!!!!!!!\nislast=%i\n",is_last);*/
		BMD_FOK(bmdpki_symmetric_decrypt((*lob)->dec_ctx, *lob_data, (long)is_last, &tmp_dec));
	}
	else /* w innym przypadku dane z wejscia zostaja wstawione do bufora, ktory podlega szyfrowaniu*/
	{
		/*PRINT_INFO("kontekst deszyfrujący NIE obecny!!!!!!!!!!!!\n");*/
		set_gen_buf2((*lob_data)->buf,(*lob_data)->size,&tmp_dec);
	}
	/* zaszyfruj */

	zdeszyfrowane_dlugosc=tmp_dec->size;
	if((*lob)->enc_ctx)
	{

		/*PRINT_INFO("kontekst szyfrujący obecny!!!!!!!!!!!!\n");*/
		/*jesli wynik deszyfrowania byl pusty, to pomin szyfrowanie*/
		if(tmp_dec->size>0)
		{

			/*dla ostatniej paczki zdeszyfrowane dane nigdy nie sa puste wiec ok*/
			status=bmdpki_symmetric_encrypt( (*lob)->enc_ctx, tmp_dec, (long)is_last, &tmp_enc);
			if (status!=BMD_OK)
			{
					free_gen_buf(&tmp_dec);
					BMD_FOK(status);
			}
		}
	}
	/* ustaw prefix i wyslij */
	/*jesli mamy byfor z szyfrowania*/

	if(tmp_enc)
	{

		/*jesli bufor nie jest pusty */
		if(tmp_enc->size>0)
		{

			if(is_last==0)
			{
				(*lob)->fl=tmp_enc->size;

			}
			else
			{
				(*lob)->fl=0;
			}

			send_buf.buf=(char *)calloc(tmp_enc->size+3*sizeof(u_int32_t), 1);
			if(send_buf.buf == NULL)
			{
				free_gen_buf(&tmp_dec);
				free_gen_buf(&tmp_enc);
				BMD_FOK(BMD_ERR_MEMORY);
			}
			/* ustawienie fl*/

			tmp_send=(u_int32_t *)(send_buf.buf);
			*tmp_send=htonl((u_int32_t)(*lob)->fl);

			/* ustawienie fs*/
			send_buf_offset=(u_int32_t *)(send_buf.buf+sizeof(u_int32_t));
			*send_buf_offset=htonl(tmp_enc->size);

			/* ustawienie tfs*/
			send_buf_offset=(u_int32_t *)(send_buf.buf+2*sizeof(u_int32_t));
			*send_buf_offset=htonl((u_int32_t)(*lob)->tfs);

			/*wlasciwe dane*/
			memcpy(send_buf.buf+3*sizeof(u_int32_t),tmp_enc->buf,tmp_enc->size);

			send_buf.size=3*sizeof(u_int32_t)+tmp_enc->size;

			status=bmdnet_send((*lob)->handler,send_buf.buf,send_buf.size);

			/*PRINT_INFO("LIBBMDLOBINF send_buf.size=%li\n",(long)send_buf.size);
			PRINT_INFO("LIBBMDLOBINF fl=%li\n",(long)(*lob)->fl);
			PRINT_INFO("LIBBMDLOBINF fs(aktualnie wysylane)=%li\n",(long)tmp_enc->size);
			PRINT_INFO("LIBBMDLOBINF tfs=%li\n",(long)(*lob)->tfs);*/

			if(status != send_buf.size)
			{

				free_gen_buf(&tmp_dec);
				free_gen_buf(&tmp_enc);
				free(send_buf.buf);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			free(send_buf.buf);
		}
		else /* bufor jest pusty */
		{

			/*nic nie zostaje wyslane*/
		}
	}
	else /* nie ma bufora po etapie szyfrowania*/
	{

		/*nic nie zostaje wyslane*/
	}

	free_gen_buf(&tmp_dec);
	free_gen_buf(&tmp_enc);

	return BMD_OK;
}

long send_lob_cancel(struct lob_transport **lob) //, GenBuf_t ** lob_data, long is_last)
{
	GenBuf_t send_buf;
	u_int32_t *tmp_send=NULL;
	u_int32_t *send_buf_offset=NULL;
	int	status=0;

	PRINT_INFO("LIBBMDLOBINF Sending lob cancel\n");
	memset(&send_buf,0,sizeof(GenBuf_t));
	if(lob==NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*lob)==NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	/* ustaw prefix i wyslij */

	(*lob)->fl=0;

	send_buf.buf=(char *)calloc(3*sizeof(u_int32_t), 1);
	if(send_buf.buf == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	/* ustawienie fl*/
	tmp_send=(u_int32_t *)(send_buf.buf);
	*tmp_send=htonl(0);
	/* ustawienie fs*/
	send_buf_offset=(u_int32_t *)(send_buf.buf+sizeof(u_int32_t));
	*send_buf_offset=htonl(0);
	/* ustawienie tfs*/
	send_buf_offset=(u_int32_t *)(send_buf.buf+2*sizeof(u_int32_t));
	*send_buf_offset=htonl(0);
	/*wysylka*/
	send_buf.size=3*sizeof(u_int32_t);
	status=bmdnet_send((*lob)->handler,send_buf.buf,send_buf.size);

	PRINT_INFO("LIBBMDLOBINF send_buf.size=%li\n",(long)send_buf.size);
	PRINT_INFO("LIBBMDLOBINF fl=%li\n",(long)(*lob)->fl);
	PRINT_INFO("LIBBMDLOBINF fs(aktualnie wysylane)=%i\n",0);
	PRINT_INFO("LIBBMDLOBINF tfs=%li\n",(long)(*lob)->tfs);

	if(status != send_buf.size)
	{
		free(send_buf.buf);
		PRINT_ERROR("LIBBMDLOBERR Error in sending lob cancel. Error=%i\n",status);
		return BMD_ERR_OP_FAILED;
	}
	free(send_buf.buf);

	return BMD_OK;
}
