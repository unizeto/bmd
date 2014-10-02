#define _WINSOCK2API_
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmderr/libbmderr.h>
#ifndef WIN32
#include <pthread.h>
#endif

/* dodaje datagramy do zbioru datagramow */
long PR2_bmdDatagramSet_fill_with_datagrams(long number, bmdDatagramSet_t **dtg_set)
{
bmdDatagram_t *tmpDatagram=NULL;
long i=0;

	PRINT_INFO("LIBBMDPRINF Filling datagramset with %li datagrams\n",number);

	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(number<1)					{	BMD_FOK(BMD_ERR_PARAM1);	}

	/************************************************/
	/*	aokacja pamięci dla nowego datagramsetu	*/
	/************************************************/
	if (dtg_set==NULL)
	{
		(*dtg_set)=(bmdDatagramSet_t *)malloc(sizeof(bmdDatagramSet_t *)+2);
		memset((*dtg_set),0,sizeof(bmdDatagramSet_t *)+1);
	}

	/****************************************************************/
	/*	tworzenie odpowiedniej ilości datagramow odpowiedzi	*/
	/****************************************************************/
	for (i=0; i<number; i++)
	{
		/********************************************************/
		/*	stworzenie i zainicjowanie datagramu odpowiedzi	*/
		/********************************************************/
		BMD_FOK(PR2_bmdDatagram_create(&tmpDatagram));
		BMD_FOK(PR2_bmdDatagramSet_add(tmpDatagram,dtg_set));
	}
	return BMD_OK;
}

long PR2_bmdDatagramSet_add(bmdDatagram_t *dtg,bmdDatagramSet_t **dtg_set)
{
	long status;

	if(dtg==NULL)
		return BMD_ERR_PARAM1;
	if(dtg_set==NULL)
		return BMD_ERR_PARAM2;

	if((*dtg_set)==NULL)
		status=_PR2_create_bmdDatagram_set(dtg_set);

	status=_PR2_add_bmdDatagram_to_set(dtg,*dtg_set);
	return BMD_OK;
}

struct twt_serialise_count_info
{
	bmdDatagramSet_t *tws_datagram_set;
	long tws_start_index;
	long tws_thread_start_index;
	long tws_iterations_count;
	long *tws_serialisation_lens;
	long *tws_metadata_string_lens;

	char* tws_buffer;
	long tws_shift;
	long tws_position;
	long tws_free_datagrams_flag;
};
typedef struct twt_serialise_count_info twt_serialise_count_info_t;

/*PR2_bmdDatagram_serialize_count_bytes_thr*/
void *PR2_bmdDatagram_serialize_count_bytes_thr(void *twf_struct)
{
twt_serialise_count_info_t *twl_serialise_count_info 	= NULL;
long i							= 0;

	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagram_serialize_count_bytes_thr\n");

	if(twf_struct == NULL)			{		BMD_FOK_THREAD(BMD_ERR_PARAM1);		}

	twl_serialise_count_info = (twt_serialise_count_info_t *)twf_struct;
	PRINT_VDEBUG("LIBBMDPRVDEBUG Serializing datagram count bytes. Index: %li tws_thread_start_index: %li.\n",
	twl_serialise_count_info->tws_start_index, twl_serialise_count_info->tws_thread_start_index);


	twl_serialise_count_info->tws_shift = 0;

	for(i = twl_serialise_count_info->tws_thread_start_index; i < twl_serialise_count_info->tws_iterations_count +
	twl_serialise_count_info->tws_thread_start_index; i++)
	{
		PRINT_VDEBUG("LIBBMDPRVDEBUG Index: %li.\n", i);

		BMD_FOK_THREAD(PR2_bmdDatagram_serialize_count_bytes_TW(
		twl_serialise_count_info->tws_datagram_set->bmdDatagramSetTable[i +
		twl_serialise_count_info->tws_start_index],

		&(twl_serialise_count_info->tws_serialisation_lens[i]),
		&(twl_serialise_count_info->tws_metadata_string_lens[i])) );

		PRINT_VDEBUG("LIBBMDPRVDEBUG tws_serialisation_len %li tws_metadata_string_len %li.\n",
		twl_serialise_count_info->tws_serialisation_lens[i], twl_serialise_count_info->tws_metadata_string_lens[i]);

		twl_serialise_count_info->tws_shift = twl_serialise_count_info->tws_shift +
		twl_serialise_count_info->tws_serialisation_lens[i];
	}

	return NULL;
}

/*PR2_bmdDatagram_serialize_TW_thr*/
void *PR2_bmdDatagram_serialize_TW_thr(void *twf_struct)
{
twt_serialise_count_info_t *twl_serialise_count_info	= NULL;
long i							= 0;

	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagram_serialize_TW_thr\n");

	if(twf_struct == NULL)			{		BMD_FOK_THREAD(BMD_ERR_PARAM1);		}

	twl_serialise_count_info = (twt_serialise_count_info_t *)twf_struct;
	PRINT_VDEBUG("LIBBMDPRVDEBUG Serializing datagram. Index: %li tws_thread_start_index: %li.\n",
	twl_serialise_count_info->tws_start_index, twl_serialise_count_info->tws_thread_start_index);


	for(i = twl_serialise_count_info->tws_thread_start_index; i < twl_serialise_count_info->tws_iterations_count + twl_serialise_count_info->tws_thread_start_index; i++)
	{
		PRINT_VDEBUG("LIBBMDPRVDEBUG Index: %li.\n", i);

		BMD_FOK_THREAD(PR2_bmdDatagram_serialize_TW(
						twl_serialise_count_info->tws_datagram_set->bmdDatagramSetTable[i + twl_serialise_count_info->tws_start_index],
						twl_serialise_count_info->tws_buffer +twl_serialise_count_info->tws_position,

						twl_serialise_count_info->tws_serialisation_lens[i],
						twl_serialise_count_info->tws_metadata_string_lens[i]));

		if(twl_serialise_count_info->tws_free_datagrams_flag == 1)
		{
			BMD_FOK_THREAD(PR2_bmdDatagram_free(
			&(twl_serialise_count_info->tws_datagram_set->bmdDatagramSetTable[i +
			twl_serialise_count_info->tws_start_index]) ));
		}
		twl_serialise_count_info->tws_position = twl_serialise_count_info->tws_position +
		twl_serialise_count_info->tws_serialisation_lens[i];
	}

	return NULL;
}


/*PR2_bmdDatagram_serialize_full_thr*/
void *PR2_bmdDatagram_serialize_full_thr(void *twf_struct)
{
twt_serialise_count_info_t *twl_serialise_count_info    = NULL;
 bmd_thread_return_t *twl_bmd_thread_return		= NULL;

	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagram_serialize_full_thr\n");

	if(twf_struct == NULL)			{		BMD_FOK_THREAD(BMD_ERR_PARAM1);		}

	twl_serialise_count_info = (twt_serialise_count_info_t *)twf_struct;
	PRINT_VDEBUG("LIBBMDPRVDEBUG Serializing datagram. Index: %li tws_thread_start_index: %li.\n",
	twl_serialise_count_info->tws_start_index, twl_serialise_count_info->tws_thread_start_index);

	twl_bmd_thread_return = (bmd_thread_return_t *)(PR2_bmdDatagram_serialize_count_bytes_thr(twf_struct));
	if(twl_bmd_thread_return != NULL)
	{
		BMD_FOK_THREAD(twl_bmd_thread_return->tws_status);
	}


	/*Alokacja pamieci na bufor wynikowy*/
	twl_serialise_count_info->tws_buffer = malloc(twl_serialise_count_info->tws_shift +
	twl_serialise_count_info->tws_position);
	PRINT_VDEBUG("LIBBMDPRVDEBUG Serializing datagram tws_shift: %li, "
	"tws_position %li\n", twl_serialise_count_info->tws_shift, twl_serialise_count_info->tws_position);

	if(twl_serialise_count_info->tws_buffer == NULL)	{	return NULL;	}
	memset(twl_serialise_count_info->tws_buffer, 0, twl_serialise_count_info->tws_shift +
	twl_serialise_count_info->tws_position);

	twl_bmd_thread_return = (bmd_thread_return_t *)(PR2_bmdDatagram_serialize_TW_thr(twf_struct));
	if(twl_bmd_thread_return != NULL)
	{
		BMD_FOK_THREAD(twl_bmd_thread_return->tws_status);
	}

	return NULL;
}

long PR2_bmdDatagramSet_serialize(	bmdDatagramSet_t *dtg_set,
					GenBuf_t *kn,
					GenBuf_t ***twf_output_buffers,
					long *twf_no_of_buffers,
					long twf_free_datagrams_flag,
					long twf_start_index,
					long twf_transmission_count)
// 					long twf_no_of_serialisation_threads)
{
long i 								= 0;
long iteration_count						= 0;
long *twl_metadata_string_lens					= NULL;
long *twl_serialisation_lens					= NULL;
long twl_buffer_len						= 0;
long twl_position						= 0;
long twf_no_of_serialisation_threads				= 1;
char *twl_buffer						= NULL;
GenBuf_t **twl_buffer_array					= NULL;
uint32_t twl_uint32						= 0;

	/*TODO*/
#ifndef WIN32
	pthread_t *thread_id_ptr					= NULL;
#endif
	twt_serialise_count_info_t **twl_serialise_count_info_ptr	= NULL;
	void **twl_thread_status					= NULL;
	bmd_thread_return_t *twl_bmd_thread_return			= NULL;


	PRINT_INFO("LIBBMDPRINF Serializing datagramset.\n");

	if (dtg_set == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (kn == NULL)							{	BMD_FOK(BMD_ERR_PARAM2);	}
	/*if(output==NULL)						{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*output)!=NULL)						{	BMD_FOK(BMD_ERR_PARAM3);	}*/

	if(twf_output_buffers == NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(*twf_output_buffers != NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}

	if(twf_no_of_buffers == NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}

	if(twf_free_datagrams_flag < 0 || twf_free_datagrams_flag > 1)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(dtg_set->bmdDatagramSetSize <= twf_start_index)		{	BMD_FOK(BMD_ERR_PARAM7);	}

	PRINT_VDEBUG("LIBBMDPRVDEBUG twf_start_index: %lu twf_transmission_count: %lu.\n",
	twf_start_index, twf_transmission_count);

	if( twf_start_index + twf_transmission_count > dtg_set->bmdDatagramSetSize)
	{
		iteration_count = dtg_set->bmdDatagramSetSize - twf_start_index;
	}
	else
	{
		iteration_count = twf_transmission_count;
	}
	PRINT_VDEBUG("LIBBMDPRVDEBUG iteration_count: %lu.\n", iteration_count);

		twl_serialisation_lens = malloc(sizeof(long) * iteration_count);
		if(twl_serialisation_lens == NULL)		{	BMD_FOK(NO_MEMORY);	}
		memset(twl_serialisation_lens, 0, sizeof(long) * iteration_count);

		twl_metadata_string_lens = malloc(sizeof(long) * iteration_count);
		if(twl_metadata_string_lens == NULL)		{	BMD_FOK(NO_MEMORY);	}
		memset(twl_metadata_string_lens, 0, sizeof(long) * iteration_count);

		if(iteration_count < 1)
		{
			twf_no_of_serialisation_threads = iteration_count;
		}

#ifdef WIN32
		twf_no_of_serialisation_threads = 1;
#endif

		twl_serialise_count_info_ptr = malloc( twf_no_of_serialisation_threads *
		sizeof(twt_serialise_count_info_t *));
		if(twl_serialise_count_info_ptr == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_serialise_count_info_ptr, 0, twf_no_of_serialisation_threads *
		sizeof(twt_serialise_count_info_t *));

#ifndef WIN32
		thread_id_ptr = malloc(twf_no_of_serialisation_threads * sizeof(pthread_t));
		if(thread_id_ptr == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(thread_id_ptr, 0, twf_no_of_serialisation_threads * sizeof(pthread_t));
#endif

		twl_thread_status = malloc(twf_no_of_serialisation_threads * sizeof(void *));
		if(twl_thread_status == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_thread_status, 0, twf_no_of_serialisation_threads * sizeof(void *));


		for(i=0; i < iteration_count % twf_no_of_serialisation_threads; i++)
		{
			twl_serialise_count_info_ptr[i] = malloc(sizeof(twt_serialise_count_info_t));
			if(twl_serialise_count_info_ptr[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
			memset(twl_serialise_count_info_ptr[i], 0, sizeof(twt_serialise_count_info_t));

			twl_serialise_count_info_ptr[i]->tws_datagram_set = dtg_set;
			twl_serialise_count_info_ptr[i]->tws_start_index = twf_start_index;

			twl_serialise_count_info_ptr[i]->tws_thread_start_index = i * (iteration_count /
			twf_no_of_serialisation_threads +1);
			twl_serialise_count_info_ptr[i]->tws_iterations_count = iteration_count /
			twf_no_of_serialisation_threads + 1;

			twl_serialise_count_info_ptr[i]->tws_serialisation_lens = twl_serialisation_lens;
			twl_serialise_count_info_ptr[i]->tws_metadata_string_lens = twl_metadata_string_lens;
		}

		for(; i < twf_no_of_serialisation_threads; i++)
		{
			twl_serialise_count_info_ptr[i] = malloc(sizeof(twt_serialise_count_info_t));
			if(twl_serialise_count_info_ptr[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
			memset(twl_serialise_count_info_ptr[i], 0, sizeof(twt_serialise_count_info_t));

			twl_serialise_count_info_ptr[i]->tws_datagram_set = dtg_set;
			twl_serialise_count_info_ptr[i]->tws_start_index = twf_start_index;

			twl_serialise_count_info_ptr[i]->tws_thread_start_index = i * (iteration_count /
			twf_no_of_serialisation_threads) + iteration_count % twf_no_of_serialisation_threads;
			twl_serialise_count_info_ptr[i]->tws_iterations_count = iteration_count /
			twf_no_of_serialisation_threads;

			twl_serialise_count_info_ptr[i]->tws_serialisation_lens = twl_serialisation_lens;
			twl_serialise_count_info_ptr[i]->tws_metadata_string_lens = twl_metadata_string_lens;
		}

#ifndef WIN32
		if( twf_no_of_serialisation_threads > 1)
		{
			for(i=0; i < twf_no_of_serialisation_threads; i++)
			{
				PRINT_VDEBUG("LIBBMDPRVDEBUG No of serialisation count bytes thread: %li.\n", i);
				if( pthread_create( &(thread_id_ptr[i]), NULL, PR2_bmdDatagram_serialize_count_bytes_thr,
				(void *)(twl_serialise_count_info_ptr[i])) != 0)
				{		BMD_FOK(BMD_ERR_PTHREAD_CREATE);			}
			}
		}
		else
#endif
		{
			twl_bmd_thread_return = (bmd_thread_return_t *)PR2_bmdDatagram_serialize_count_bytes_thr(
			twl_serialise_count_info_ptr[0]);
			if(twl_bmd_thread_return != NULL)
			{
				BMD_FOK(twl_bmd_thread_return->tws_status);
			}
		}

		twl_serialise_count_info_ptr[0]->tws_position = 0;
		for(i=0; i < twf_no_of_serialisation_threads; i++)
		{
#ifndef WIN32
			if(twf_no_of_serialisation_threads > 1)
			{
				PRINT_VDEBUG("LIBBMDPRVDEBUG Join iteration: %li.\n", i)
				if(pthread_join(thread_id_ptr[i], &(twl_thread_status[i])) != 0)
				{		BMD_FOK(BMD_ERR_PTHREAD_JOIN);		}
				thread_id_ptr[i] = 0;
			}
#endif

			twl_serialise_count_info_ptr[i]->tws_position = twl_buffer_len;
			twl_buffer_len = twl_buffer_len + twl_serialise_count_info_ptr[i]->tws_shift;
		}

		/*Sprawdzam statusy wyjscia z watkow.*/
		if(twf_no_of_serialisation_threads > 1)
		{
			for(i=0; i < twf_no_of_serialisation_threads; i++)
			{
				if(twl_thread_status[i] != NULL)
				{
					BMD_FOK( ((bmd_thread_return_t *)twl_thread_status[i])->tws_status);
				}
			}
		}

		twl_buffer_len = (twl_buffer_len +  3 * sizeof(uint32_t) + 3 + kn->size + 1) * sizeof(char);

		PRINT_VDEBUG("LIBBMDVDEBUG twl_buffer_len %li\n", twl_buffer_len);
		twl_buffer = malloc(twl_buffer_len);
		if(twl_buffer == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_buffer, 0, twl_buffer_len );

		if(kn->size > 0)
		{
			memcpy(twl_buffer, kn->buf, kn->size);
		}

		twl_position = 0;
		twl_uint32 = htonl(twl_buffer_len);
		memcpy(twl_buffer + (twl_position * (sizeof(uint32_t) + 1)) + kn->size, &twl_uint32, sizeof(uint32_t));
		PRINT_VDEBUG("LIBBMDVDEBUG serialised datagramset size %li\n", twl_buffer_len);

		twl_position++;
		twl_uint32 = htonl(iteration_count);
		memcpy(twl_buffer + (twl_position * (sizeof(uint32_t) + 1)) + kn->size, &twl_uint32, sizeof(uint32_t));
		PRINT_VDEBUG("LIBBMDVDEBUG serialised bmdDatagramsTransmissionCount %li\n", iteration_count);

		twl_position++;
		twl_uint32 = htonl(dtg_set->bmdDatagramSetSize);
		memcpy(twl_buffer  + (twl_position * (sizeof(uint32_t) + 1)) + kn->size, &twl_uint32, sizeof(uint32_t));
		PRINT_VDEBUG("LIBBMDVDEBUG serialised bmdDatagramsAllCount %li\n", dtg_set->bmdDatagramSetSize);

		twl_position++;
		twl_position = twl_position * (sizeof(uint32_t) + 1) + kn->size;

#ifndef WIN32
		if(twf_no_of_serialisation_threads > 1)
		{
			for(i=0; i < twf_no_of_serialisation_threads; i++)
			{
				twl_serialise_count_info_ptr[i]->tws_free_datagrams_flag = twf_free_datagrams_flag;
				twl_serialise_count_info_ptr[i]->tws_buffer = twl_buffer;
				twl_serialise_count_info_ptr[i]->tws_position = twl_serialise_count_info_ptr[i]->
				tws_position + twl_position;

				PRINT_VDEBUG("LIBBMDPRVDEBUG No of serialisation thread: %li.\n", i);

				if( pthread_create( &(thread_id_ptr[i]), NULL, PR2_bmdDatagram_serialize_TW_thr,
				(void *)(twl_serialise_count_info_ptr[i])) != 0)
				{		BMD_FOK(BMD_ERR_PTHREAD_CREATE);		}
			}
		}
		else
#endif
		{
			twl_serialise_count_info_ptr[0]->tws_free_datagrams_flag = twf_free_datagrams_flag;
			twl_serialise_count_info_ptr[0]->tws_buffer = twl_buffer;
			twl_serialise_count_info_ptr[0]->tws_position = twl_position;

			twl_bmd_thread_return = (bmd_thread_return_t *)PR2_bmdDatagram_serialize_TW_thr(twl_serialise_count_info_ptr[0]);
			if(twl_bmd_thread_return != NULL)
			{
				BMD_FOK(twl_bmd_thread_return->tws_status);
			}
		}


		for(i=0; i < twf_no_of_serialisation_threads; i++)
		{
#ifndef WIN32
			if(twf_no_of_serialisation_threads > 1)
			{
				PRINT_VDEBUG("LIBBMDPRVDEBUG Join iteration: %li.\n", i)
				if(pthread_join(thread_id_ptr[i], &(twl_thread_status[i])) != 0)
				{		BMD_FOK(BMD_ERR_PTHREAD_JOIN);		}
				thread_id_ptr[i] = 0;
			}
#endif
			free(twl_serialise_count_info_ptr[i]);
			twl_serialise_count_info_ptr[i] = NULL;
		}

		/*Sprawdzam statusy wyjscia z watkow.*/
		if(twf_no_of_serialisation_threads > 1)
		{
			for(i=0; i < twf_no_of_serialisation_threads; i++)
			{
				if(twl_thread_status[i] != NULL)
				{
					BMD_FOK( ((bmd_thread_return_t *)twl_thread_status[i])->tws_status);
				}
			}
		}


		free(twl_serialise_count_info_ptr); twl_serialise_count_info_ptr = NULL;
#ifndef WIN32
		free(thread_id_ptr); thread_id_ptr = NULL;
#endif
		free(twl_thread_status); twl_thread_status = NULL;

		free(twl_metadata_string_lens); twl_metadata_string_lens = NULL;
		free(twl_serialisation_lens); twl_serialisation_lens = NULL;

		twl_buffer_array = malloc(sizeof(GenBuf_t *));
		if(twl_buffer_array == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_buffer_array, 0, sizeof(GenBuf_t *));

		BMD_FOK(set_gen_buf_ptr(&twl_buffer, twl_buffer_len, &(twl_buffer_array[0])));

		*twf_output_buffers = twl_buffer_array;
		twl_buffer_array = NULL;
		*twf_no_of_buffers = 1;

	return BMD_OK;
}

long PR2_bmdDatagramSet_deserialize(	GenBuf_t **input,
					long twf_prefix,
					bmdDatagramSet_t **dtg_set,
					long *twf_bmdDatagramsTransmissionCount,
					long *twf_bmdDatagramsAllCount,
					long twf_input_free,
					long *twf_memory_guard,
					long deserialisation_max_memory)
{
long i						= 0;
long twl_temp_value				= 0;
long twl_position				= 0;
long twl_deserialised_len			= 0;
long twl_memory_guard				= 0;
long twl_max_memory				= 0;
uint32_t twl_uint32				= 0;

	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagramSet_deserialize.\n");

	if (twf_bmdDatagramsTransmissionCount == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (twf_bmdDatagramsAllCount == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (twf_input_free < 0 || twf_input_free > 1)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (twf_memory_guard == NULL)			{	BMD_FOK(BMD_ERR_PARAM7); 	}
	if (*twf_memory_guard < 0)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (deserialisation_max_memory<0)		{	BMD_FOK(BMD_ERR_PARAM8);	}

 	twl_max_memory = deserialisation_max_memory;
	twl_memory_guard = *twf_memory_guard;

	PRINT_VDEBUG("LIBBMDVDEBUG twl_memory_guard %li\n", twl_memory_guard);

	if( (*input)->size < 3 * ( (long)sizeof(uint32_t) + 1 ) + twf_prefix)
	{
		BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
	}

	twl_position = 0;
	memcpy(&twl_uint32, (*input)->buf + twl_position * (sizeof(uint32_t) + 1) + twf_prefix, sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised datagramset size %li\n", twl_temp_value);

	if(twl_temp_value < 3 * (sizeof(twl_uint32) + 1) )
	{
		BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
	}

	BMD_MEMORY_GUARD(sizeof(bmdDatagramSet_t));
	(*dtg_set)=(bmdDatagramSet_t *)malloc(sizeof(bmdDatagramSet_t));
	if( *dtg_set == NULL)					{	 BMD_FOK(NO_MEMORY);	}
	memset(*dtg_set,0,sizeof(bmdDatagramSet_t));

	twl_position++;
	memcpy(&twl_uint32, (*input)->buf + twl_position * (sizeof(uint32_t) + 1) + twf_prefix, sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	*twf_bmdDatagramsTransmissionCount = twl_temp_value;
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised bmdDatagramsTransmissionCount %li\n", twl_temp_value);

	(*dtg_set)->bmdDatagramSetSize = twl_temp_value;

	BMD_MEMORY_GUARD(sizeof(bmdDatagram_t *)*(twl_temp_value + 1));
	(*dtg_set)->bmdDatagramSetTable = (bmdDatagram_t **)malloc(sizeof(bmdDatagram_t *)*(twl_temp_value + 1));
	if((*dtg_set)->bmdDatagramSetTable == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset((*dtg_set)->bmdDatagramSetTable,0,sizeof(bmdDatagram_t *)*(twl_temp_value + 1));

	twl_position++;
	memcpy(&twl_uint32, (*input)->buf + twl_position * (sizeof(uint32_t) + 1) + twf_prefix, sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	*twf_bmdDatagramsAllCount = twl_temp_value;
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised bmdDatagramsAllCount %li\n", twl_temp_value);

	twl_position++;
	twl_position = twl_position * (sizeof(uint32_t) + 1) + twf_prefix;
	for(i = 0; i < (*dtg_set)->bmdDatagramSetSize; i++)
	{
		twl_deserialised_len = (*input)->size - twl_position;
		BMD_FOK(PR2_bmdDatagram_deserialize_TW(	(*input)->buf + twl_position,
							&((*dtg_set)->bmdDatagramSetTable[i]),
							&twl_deserialised_len,
							&twl_memory_guard,
 							twl_max_memory));
		twl_position = twl_position + twl_deserialised_len;
	}

	if(twf_input_free == 1)
	{
		free_gen_buf(input);
	}

	PRINT_VDEBUG("LIBBMDVDEBUG twl_memory_guard %li\n", twl_memory_guard);
	*twf_memory_guard = twl_memory_guard;

	return BMD_OK;
}

long PR2_bmdDatagramSet_free(bmdDatagramSet_t **dtg_set)
{
	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagramSet_free.\n");

	_PR_bmdDatagramSet_free(*dtg_set,1);
	*dtg_set=NULL;

	return BMD_OK;
}


/*Funkcja dodaje wszystkie datagramy z jednego datagram setu do drugiego. Kopiowane sa jedynie wskazniki. W datagramie
bedacym zrodlem datagramow sama tablica datagramow jest zwalniana i ilosc datagramow jest ustawiana na zero. Same
datagramy nie sa zwalniane gdy sa juz powiazane z nowym datagramsetem.*/
long PR2_bmdDatagramSet_merge_pointers(bmdDatagramSet_t *dtg_set_destiny, bmdDatagramSet_t *dtg_set_source)
{
	bmdDatagram_t **bmdDatagramSetTable_temp	= NULL;

	PRINT_INFO("LIBBMDPRINF PR2_bmdDatagramSet_merge_pointers.\n");

        if(dtg_set_destiny==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg_set_source==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(dtg_set_destiny->bmdDatagramSetSize < 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(dtg_set_destiny == dtg_set_source)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(dtg_set_source->bmdDatagramSetSize < 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(dtg_set_source->bmdDatagramSetSize == 0)	{	return BMD_OK;			}
	if(dtg_set_source->bmdDatagramSetTable == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*Tworze nowa tablice datagramow*/
	bmdDatagramSetTable_temp = realloc ( dtg_set_destiny->bmdDatagramSetTable, \
	(dtg_set_destiny->bmdDatagramSetSize + dtg_set_source->bmdDatagramSetSize) * sizeof(bmdDatagram_t *));
	if(bmdDatagramSetTable_temp == NULL)		{	BMD_FOK(NO_MEMORY);	}

	/*Kopiuje wskazniki z drugiego datagramu do nowej tablicy*/
	memcpy(bmdDatagramSetTable_temp + dtg_set_destiny->bmdDatagramSetSize, dtg_set_source->bmdDatagramSetTable, \
	(dtg_set_source->bmdDatagramSetSize) * sizeof(bmdDatagram_t *));

	/*Podpinam nowa tablice w miejsce starej*/
	dtg_set_destiny->bmdDatagramSetTable = bmdDatagramSetTable_temp;
	bmdDatagramSetTable_temp = NULL;
	dtg_set_destiny->bmdDatagramSetSize = dtg_set_destiny->bmdDatagramSetSize + dtg_set_source->bmdDatagramSetSize;

	/*Zwalniam sama tablice datagramow w datagramsecie zrodlowym*/
	dtg_set_source->bmdDatagramSetSize = 0;
	free(dtg_set_source->bmdDatagramSetTable);
	dtg_set_source->bmdDatagramSetTable = NULL;

        return BMD_OK;
}

