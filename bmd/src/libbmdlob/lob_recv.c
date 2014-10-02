#include<bmd/libbmderr/libbmderr.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdlob/libbmdlob.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif
/*
funkcja recv_lob_chunk() zwraca:
	1 jesli ostatnia paczka przetwarzana
	0 jesli nieostatnia paczka loba
	<0 w przypadku bledu
*/
long recv_lob_chunk(	struct lob_transport **lob,
				GenBuf_t **lob_data,
				long min_bytes_to_get,
				long max_bytes_to_get )
{
long status			= 0;
u_int32_t net_fl=0, net_bs=0, net_tfs=0;
u_int32_t host_bs=0 ,host_fl=0;
long count_to_recv=0;
GenBuf_t recv_buf;
GenBuf_t *tmp=NULL;
GenBuf_t *tmp_hash=NULL;

	if(lob == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid first parameter error. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(*lob == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid first parameter error. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(lob_data == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid second parameter error. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	/*added by WSZ*/
	if( (*lob)->dec_ctx == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Invalid second parameter error. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	PRINT_INFO("LIBBMDLOBINF Receiving lob chunk\n");
	memset(&recv_buf,0,sizeof(GenBuf_t));

	/* odbierz dane z gniazda - najpierw prefix ile zostalo bajtow */
	status = bmdnet_recv_min((*lob)->handler,&net_fl,sizeof(net_fl),sizeof(net_fl));
	if (status <BMD_OK)
	{
		host_fl=ntohl(net_fl);
		BMD_FOK(status);
	}

	status = bmdnet_recv_min((*lob)->handler,&net_bs,sizeof(net_bs),sizeof(net_bs));
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDLOBERR Error in getting min bs value. Error=%li\n",status);
		host_bs=ntohl(net_bs);
		PRINT_INFO("LIBBMDLOBINF Host bs = %i\n", host_bs);
		return status;
	}

	status = bmdnet_recv_min((*lob)->handler,&net_tfs,sizeof(net_tfs),sizeof(net_tfs));
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDLOBERR Error in getting min tfs value. Error=%li\n",status);
		(*lob)->tfs=ntohl(net_tfs);
		PRINT_INFO("LIBBMDLOBINF Lob tfs = %li\n", (long)(*lob)->tfs);
		return status;
	}


	host_fl	=ntohl(net_fl);
	host_bs	=ntohl(net_bs);
	(*lob)->tfs=ntohl(net_tfs);

	if ((host_fl==0) && (host_bs==0) && ((long)(*lob)->tfs==0))
	{
		free(recv_buf.buf);
		PRINT_INFO("LIBBMDLOBINF Lob transfer cancelled by client\n");
		return LOB_CANCELED_BY_CLIENT;
	}

	/*jesli aktualnie przetwarzana jest ostatnia paczka loba*/
	if(host_fl==0)
	{
		/*jesli przetwarzana paczka nie jest pusta*/
		if(host_bs>0)
		{
			recv_buf.buf=(char *)calloc(host_bs, 1);
			if(recv_buf.buf == NULL)
			{
				PRINT_ERROR("LIBBMDLOBERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
				return BMD_ERR_MEMORY;
			}
			status=bmdnet_recv_min((*lob)->handler,recv_buf.buf,host_bs,host_bs);
			/*status=bmdnet_recv((*lob)->handler, recv_buf.buf, host_bs, 1);*/
			if(status < 0)
			{
				free(recv_buf.buf);
				PRINT_ERROR("LIBBMDLOBERR Net reading error. Error=%i\n",BMD_ERR_MEMORY);
				return BMD_ERR_NET_READ;
			}
			recv_buf.size=status;
			PRINT_DEBUG("MSILEWICZ_LAST_BLK_SIZE %li\n",recv_buf.size);
			status=bmdpki_symmetric_decrypt((*lob)->dec_ctx, &recv_buf, BMDPKI_SYM_LAST_CHUNK, &tmp);
			if (status!=BMD_OK)
			{
				free(recv_buf.buf);
				PRINT_ERROR("LIBBMDLOB Error in symmetric decryption. Error=%li\n", status);
				return status;
			}
			/*PRINT_TEST("\n\n%s\n",tmp->buf);*/
	    		/* WSZ porzadki - niepotrzebne, bo nigdzie nie uzywane
			(*lob)->fs+=host_bs;
			*/

			/*liczenie skrotu do timestampa - na wyjsciu dane odebrane bez deszyfracji*/
			if((*lob)->options & TS_PLAIN_DATA) /*iloczyn binarny*/
			{
				status=bmd_hash_data(tmp,&((*lob)->hash_ctx),&tmp_hash,NULL);
				if (status!=BMD_OK)
				{
					free_gen_buf(&tmp);
					free(recv_buf.buf);
					PRINT_ERROR("LIBBMDLOB Error in hashing data. Error=%li\n", status);
					return status;
				}
				set_gen_buf2(recv_buf.buf,recv_buf.size,lob_data);
			}
			else /* na wyjsciu dane zdeszyfrowane*/
			{
				set_gen_buf2(tmp->buf,tmp->size,lob_data);
			}
			free_gen_buf(&tmp);

			/*poniewaz to nie finalizacja liczenia skrotu (NULL na wejsciu)
			to tmp_hash nie jest alokowany i nie trzeba zwalniac*/
			//free_gen_buf(&tmp_hash);
			free(recv_buf.buf);
		}
		/*jesli otrzymana pusta paczka (choc tak nie powinno sie wydarzyc),
		alokowany GenBuf z zerowym rozmiarem*/
		else
		{

			*lob_data=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		}

		//PRINT_INFO("LIBBMDLOBINF Lob's chunk transmitted. Lob fs = %li\n", (long)(*lob)->fs);
		return 1;
	}

	/* jesli przetwarzana paczka nie jest ostania paczka loba */

	if(host_bs>(u_int32_t)max_bytes_to_get)
	{
		count_to_recv=max_bytes_to_get;
	}
	else
	{
		count_to_recv=(int)host_bs;
	}

	/* WSZ porzadki - niepotrzebne, bo nigdzie nie uzywane
	(*lob)->fs+=count_to_recv;
	*/

	recv_buf.buf=(char *)calloc(count_to_recv, 1);
	if(recv_buf.buf == NULL)
	{
		PRINT_ERROR("LIBBMDLOBERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	status=bmdnet_recv_min((*lob)->handler,recv_buf.buf,count_to_recv,count_to_recv);
	if (status<BMD_OK)
	{
		free(recv_buf.buf);
		PRINT_ERROR("LIBBMDLOBERR Error in reciving min. Error=%li\n",status);
		return status;
	}
	/*PRINT_ERROR("bmdnet_recv status %li\n",status);*/
	recv_buf.size=status;

	status=bmdpki_symmetric_decrypt((*lob)->dec_ctx, &recv_buf, BMDPKI_SYM_NOT_LAST_CHUNK, &tmp);
	if (status!=BMD_OK)
	{
		free(recv_buf.buf);
		PRINT_ERROR("LIBBMDLOBERR Error in symmetrc decryption. Error=%li\n",status);
		return status;
	}
	//PRINT_TEST("\n\n%s\n",tmp->buf);

	/*PRINT_ERROR("tmp_buf.size %li lob_recv.c tmp->size %li\n",tmp_buf.size,tmp->size);*/
	/*jesli liczony skrot do timestampa - na wyjsciu dane z odbioru bez deszyfracji*/
	if((*lob)->options & TS_PLAIN_DATA) /*iloczyn binarny*/
	{
		status=bmd_hash_data(tmp,&((*lob)->hash_ctx),&tmp_hash,NULL);
		if (status!=BMD_OK)
		{
			free(recv_buf.buf);
			free_gen_buf(&tmp);
			PRINT_ERROR("LIBBMDLOBERR Error in hashing data. Error=%li\n",status);
			return status;
		}
		set_gen_buf2(recv_buf.buf,recv_buf.size,lob_data);
	}
	else /*bez liczenia skrotu do timestampa -na wyjsciu dane zdeszyfrowane*/
	{
		set_gen_buf2(tmp->buf,tmp->size,lob_data);
	}

	free(recv_buf.buf);
	/*poniewaz to nie finalizacja liczenia skrotu (brak NULL na wejsciu)
	to tmp_hash nie jest alokowany i nie trzeba zwalniac*/
	//free_gen_buf(&tmp_hash);
	free_gen_buf(&tmp);

	//PRINT_INFO("LIBBMDLOBINF Lob's chunk transmitted. Lob fs = %li\n", (long)(*lob)->fs);
	return BMD_OK;
}
