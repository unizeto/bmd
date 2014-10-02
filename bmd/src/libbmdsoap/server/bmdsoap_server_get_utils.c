#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/******************************************************************************/
/*	pobranie z bazy danych pliku i zapisanie go we wskazanej lokalizacji	*/
/******************************************************************************/
long getFileFromBMD(	bmd_info_t *bi,
				bmdDatagramSet_t *req_dtg_set,
				bmdDatagramSet_t **resp_dtg_set,
				char *dest_local)
{
long status				= 0;
lob_request_info_t *li			= NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

	/**************************************/
	/* pobranie ustawień konfiguracyjnych */
	/**************************************/
	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	/******************************************************/
	/*	stworzenie zadania pobrania pliku glownego	*/
	/******************************************************/
	status=bmd_lob_request_info_create(&li);
	if(status<BMD_OK)
	{
		return status;
	}

	/************************************/
	/*	okreslenie pliku docelowego	*/
	/************************************/
	status=bmd_lob_request_info_add_filename(dest_local, &li, BMD_RECV_LOB,1000000000);
	if(status<BMD_OK)
	{
		return status;
	}

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	status=bmd_send_request_lo(bi,req_dtg_set, max_datagram_in_set_transmission, resp_dtg_set, li, deserialisation_max_memory);
	if (status==BMD_OK)
	{
		status=(*resp_dtg_set)->bmdDatagramSetTable[0]->datagramStatus;
	}
	if(status<BMD_OK)
	{
		return status;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_lob_request_info_destroy(&li);

	return BMD_OK;
}
