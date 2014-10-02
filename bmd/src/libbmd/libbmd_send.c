#define _FILE_OFFSET_BITS 64
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdlob/libbmdlob.h>

#include <bmd/libbmd/libbmd_internal.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/** Funkcja bmd_send_request2 z ostatnim paremetrem NULL
funkcja dla wstecznej kompatybilnosci
*/
long bmd_send_request(	bmd_info_t *bmd_info,
			bmdDatagramSet_t *request_datagram_set,
			long twf_max_datagram_in_set_transmission,
			bmdDatagramSet_t **response_datagram_set,
			long twf_free_datagrams_flag,
			long deserialisation_max_memory)
{

	BMD_FOK(bmd_send_request_2(	bmd_info,
					request_datagram_set,
					twf_max_datagram_in_set_transmission,
					response_datagram_set,
					NULL,
					twf_free_datagrams_flag,
					deserialisation_max_memory));

	return BMD_OK;
}

/** Funkcja bmd_send_request z dodanym paremetrem funkcji odwierzajcej GUI podczas czekania na gniedzie
@param refreshcallback - funkcja odswierzajaca GUI wywoywana w
parametr "persistent" aktualnie jest nieuzywany
*/
long bmd_send_request_2(	bmd_info_t *bmd_info,
				bmdDatagramSet_t *request_datagram_set,
				long twf_max_datagram_in_set_transmission,
				bmdDatagramSet_t **response_datagram_set,
				bmdnet_progress_callback_t refreshcallback,
				long twf_free_datagrams_flag,
				long deserialisation_max_memory)
{
long i							= 0;
long bmdDatagramsTransmissionCount			= 0;
long bmdDatagramsAllCount				= 0;
long bmdDatagramsLeft					= 0;
long iteration_count					= 0;
long twl_no_of_buffers					= 0;
long twl_memory_guard					= 0;
bmdnet_handler_ptr session_handle			= NULL;
GenBuf_t *tmp_ki_mod_n_client				= NULL;
GenBuf_t **request_to_send				= NULL;
GenBuf_t *received_data					= NULL;
GenBuf_t *new_ki_mod_n					= NULL;
bmdDatagramSet_t *temp_dtg_set				= NULL;

	PRINT_INFO("LIBBMDINF Sending request to bmd server\n");
	/******************************/
	/*	walidacja parametrów	*/
	/******************************/
	if (bmd_info == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (request_datagram_set==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (request_datagram_set->bmdDatagramSetSize<=0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_max_datagram_in_set_transmission <= 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (response_datagram_set == NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*response_datagram_set != NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (twf_free_datagrams_flag < 0)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (twf_free_datagrams_flag > 1)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (deserialisation_max_memory < 0)			{	BMD_FOK(BMD_ERR_PARAM7);	}

	for(i=0;i<(long)request_datagram_set->bmdDatagramSetSize;i++)
	{
		BMD_FOK(__bmd_prepare_request(bmd_info,request_datagram_set->bmdDatagramSetTable[i],NULL));
	}


	iteration_count = request_datagram_set->bmdDatagramSetSize / twf_max_datagram_in_set_transmission;
	if( (request_datagram_set->bmdDatagramSetSize % twf_max_datagram_in_set_transmission) != 0)
	{
		iteration_count++;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG request_datagram_set->bmdDatagramSetSize: %lu\n", request_datagram_set->bmdDatagramSetSize);
	PRINT_VDEBUG("LIBBMDVDEBUG iteration_count: %lu\n", iteration_count);

	BMD_FOK(bmd_compute_ki_mod_n(bmd_info->cur_ki_mod_n, bmd_info->i, bmd_info->n, &tmp_ki_mod_n_client));

	for(i = 0; i < iteration_count; i++)
	{

		BMD_FOK(__bmd_prepare_request_buffer(	request_datagram_set,
							bmd_info,
							i*twf_max_datagram_in_set_transmission,
							twf_max_datagram_in_set_transmission,
							&request_to_send,
							&twl_no_of_buffers,
							tmp_ki_mod_n_client,
							twf_free_datagrams_flag));

		BMD_FOK(__bmd_request_send(&session_handle, request_to_send, twl_no_of_buffers, bmd_info, !i));

		free0(request_to_send);
		/*free_gen_buf(&(request_to_send[i]));*/

	}
	/*free(request_to_send); request_to_send = NULL;*/


	if ( (refreshcallback) && (session_handle) )
	{
		session_handle->pcallback = *refreshcallback;
	}

	BMD_FOK(__bmd_response_recv(&session_handle,bmd_info,&received_data));
	PRINT_VDEBUG("LIBBMDVDEBUG received_data size: %li\n", received_data->size);
	/*fflush(stdout);*/

	BMD_FOK(__bmd_decode_response_buffer(	&received_data,
						bmd_info,
						tmp_ki_mod_n_client,
						&temp_dtg_set,
						&new_ki_mod_n,
						&bmdDatagramsTransmissionCount,
						&bmdDatagramsAllCount,
						&twl_memory_guard,
						deserialisation_max_memory));

	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramsTransmissionCount: %lu, bmdDatagramsAllCount %lu\n", bmdDatagramsTransmissionCount, bmdDatagramsAllCount);
	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramSetSize: %lu\n", temp_dtg_set->bmdDatagramSetSize);

	bmdDatagramsLeft = bmdDatagramsAllCount - bmdDatagramsTransmissionCount;
	*(response_datagram_set) = temp_dtg_set;
	temp_dtg_set = NULL;
	while(bmdDatagramsLeft > 0)
	{
		BMD_FOK(__bmd_response_recv(&session_handle,bmd_info,&received_data));
		PRINT_VDEBUG("LIBBMDVDEBUG In loop received_data size: %li\n", received_data->size);

		BMD_FOK(__bmd_decode_response_buffer(	&received_data,
							bmd_info,
							tmp_ki_mod_n_client,
							&temp_dtg_set,
							NULL,
							&bmdDatagramsTransmissionCount,
							&bmdDatagramsAllCount,
							&twl_memory_guard,
							deserialisation_max_memory));

		PRINT_VDEBUG("LIBBMDVDEBUG In loop bmdDatagramsTransmissionCount: %lu, bmdDatagramsAllCount %lu\n",
		bmdDatagramsTransmissionCount, bmdDatagramsAllCount);
		bmdDatagramsLeft = bmdDatagramsLeft - bmdDatagramsTransmissionCount;

		PRINT_VDEBUG("LIBBMDVDEBUG In loop bmdDatagramSetSize: %lu\n", temp_dtg_set->bmdDatagramSetSize);
		BMD_FOK(PR2_bmdDatagramSet_merge_pointers(*response_datagram_set, temp_dtg_set));
		/*
		for(i = 0; i < temp_dtg_set->bmdDatagramSetSize; i++)
		{
			BMD_FOK(PR2_bmdDatagramSet_add(temp_dtg_set->bmdDatagramSetTable[i], response_datagram_set));
		}
		free(temp_dtg_set);
		temp_dtg_set = NULL;
		*/

		PR2_bmdDatagramSet_free(&temp_dtg_set);
	}
	/*bmd_info->server_ki_mod_n = new_ki_mod_n;
	new_ki_mod_n = NULL;*/

	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramSetSize after: %lu\n", (*response_datagram_set)->bmdDatagramSetSize);
	/*fflush(stdout);*/

	BMD_FOK(__bmd_prepare_response(bmd_info,response_datagram_set,NULL,NULL));

	/*fflush(stdout);*/
	bmdnet_close(&session_handle);
	/*free_gen_buf(&request_to_send);*/
	free_gen_buf(&received_data);
	free_gen_buf(&tmp_ki_mod_n_client);

	return BMD_OK;
}
