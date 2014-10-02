#define _WINSOCK2API_
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include<bmd/libbmderr/libbmderr.h>

long PR2_bmdDatagram_serialize_count_bytes_TW(	bmdDatagram_t *dtg, long *twf_size_of_serialized_datagram,
						long *twf_metadata_string_len)
{
const long twl_cons_number_of_dtg_len		= 1;
const long twl_cons_number_of_long		= 15;

long twl_iteration_0				= 0;
long twl_size_of_serialized_datagram		= 0;
long twl_metadata_string_len			= 0;
long twl_size_of_serialized_genbufs		= 0;

	if(dtg==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_size_of_serialized_datagram==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_metadata_string_len==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}


	/*Oblicza dlugosc calego datagramu po serializacji*/
	/*ilosc bajtow na dlugosc datagramu i NULL po niej*/
	twl_size_of_serialized_datagram = twl_cons_number_of_dtg_len * sizeof(uint32_t) + twl_cons_number_of_dtg_len;

	/*Ilosc bajtow na wartosci typu long i NULL po kazdej z nich*/
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram + twl_cons_number_of_long * sizeof(uint32_t) +
	twl_cons_number_of_long;

	/*Ilosc bajtow na bufory zawarte w genbufach*/
	if(dtg->protocolData != NULL)
	{
		twl_size_of_serialized_genbufs = dtg->protocolData->size + 1;
	}
	else
	{
		twl_size_of_serialized_genbufs = 1;
	}

	if(dtg->protocolDataFilename != NULL)
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + dtg->protocolDataFilename->size + 1;
	}
	else
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + 1;
	}

	if(dtg->protocolDataFileLocationId != NULL)
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + dtg->protocolDataFileLocationId->size + 1;
	}
	else
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + 1;
	}

	if(dtg->protocolDataFileId != NULL)
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + dtg->protocolDataFileId->size + 1;
	}
	else
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + 1;
	}

	if(dtg->protocolDataOwner != NULL)
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + dtg->protocolDataOwner->size + 1;
	}
	else
	{
		twl_size_of_serialized_genbufs = twl_size_of_serialized_genbufs + 1;
	}


	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram + twl_size_of_serialized_genbufs;

	/*ilosc bajtow na dlugosci metadanych action, oida, myId, ownerType, ownerId i NULL po nich*/
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
        dtg->no_of_actionMetaData * 5 * (sizeof(uint32_t) + 1);

	/*ilosc bajtow na dlugosci metadanych systemowych, oida, myId, ownerType, ownerId i NULL po nich*/
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
	dtg->no_of_sysMetaData * 5 * (sizeof(uint32_t) + 1);

	/*ilosc bajtow na dlugosci metadanych systemowych, oid, myId, ownerType, ownerId i NULL po nich*/
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
	dtg->no_of_pkiMetaData * 5 * (sizeof(uint32_t) + 1);

	/*ilosc bajtow na dlugosci metadanych dodatkowych, myId, ownerType, ownerId i NULL po nich*/
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
	dtg->no_of_additionalMetaData * 5 * (sizeof(uint32_t) + 1);

	/*Ilosc bajtow na wartosci metadanych action*/
        for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_actionMetaData; twl_iteration_0++)
        {
		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		dtg->actionMetaData[twl_iteration_0]->AnySize + 1;

		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		(long)strlen(dtg->actionMetaData[twl_iteration_0]->OIDTableBuf) + 1;
        }

	/*Ilosc bajtow na wartosci metadanych systemowych*/
        for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_sysMetaData; twl_iteration_0++)
        {
		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		dtg->sysMetaData[twl_iteration_0]->AnySize + 1;

		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		(long)strlen(dtg->sysMetaData[twl_iteration_0]->OIDTableBuf) + 1;
        }


	/*Ilosc bajtow na wartosci metadanych PKI*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		/*Zbieram do osobnej zmiennej bo potrzebuje potem*/
		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		dtg->pkiMetaData[twl_iteration_0]->AnySize + 1;

		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		(long)strlen(dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf) + 1;
	}


	/*Ilosc bajtow na wartosci metadanych dodatkowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_additionalMetaData; twl_iteration_0++)
	{
		/*Zbieram do osobnej zmiennej bo potrzebuje potem*/
		twl_metadata_string_len = twl_metadata_string_len +
		dtg->additionalMetaData[twl_iteration_0]->AnySize + 1;

		twl_size_of_serialized_datagram = twl_size_of_serialized_datagram +
		(long)strlen(dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf) + 1;
	}
	twl_size_of_serialized_datagram = twl_size_of_serialized_datagram + twl_metadata_string_len;

	*twf_size_of_serialized_datagram = twl_size_of_serialized_datagram;
	*twf_metadata_string_len = twl_metadata_string_len;

	return BMD_OK;
}

/*Serialzuje sam datagram*/
long PR2_bmdDatagram_serialize_TW(	bmdDatagram_t *dtg,
					char *twf_serialized_datagram_ptr,
					long twf_size_of_serialized_datagram,
					long twf_metadata_string_len)
{
long twl_iteration_0                            = 0;
long twl_position                               = 0;
long twl_position_with_gen_buf                  = 0;
long twl_metadata_string_len                    = 0;
long twl_size_of_serialized_datagram            = 0;
long twl_no_of_metadatas                        = 0;

uint32_t twl_uint32				= 0;

char *twl_serialized_datagram_ptr		= NULL;
/*GenBuf_t *twl_gen_buf				= NULL;*/

	if(dtg==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_serialized_datagram_ptr==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_size_of_serialized_datagram <= 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(twf_metadata_string_len < 0)			{	BMD_FOK(BMD_ERR_PARAM4);	}

	twl_serialized_datagram_ptr = twf_serialized_datagram_ptr;

	twl_size_of_serialized_datagram = twf_size_of_serialized_datagram;
	twl_metadata_string_len = twf_metadata_string_len;

	/*################################*/
	/*Alokuje pamiec na caly datagram*/
	/*twl_size_of_serialized_datagram = twl_size_of_serialized_datagram * sizeof(char);

	twl_serialized_datagram_ptr = malloc(twl_size_of_serialized_datagram);
	if(twl_serialized_datagram_ptr == NULL)
	{
		BMD_FOK(NO_MEMORY);
	}
	memset(twl_serialized_datagram_ptr, 0, twl_size_of_serialized_datagram);*/
	/*###############################*/


	/*Ustawiam dlugosc bufora*/
	twl_position = 0;
	twl_uint32 = htonl(twl_size_of_serialized_datagram);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG twl_size_of_serialized_datagram: %li\n", twl_size_of_serialized_datagram);

	twl_position++;
	/*Wstawiam po kolei wszystkie wartosci long*/
	twl_uint32 = htonl(dtg->protocolVersion);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)),
	&twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolVersion: %li\n", dtg->protocolVersion);

	twl_position++;
	twl_uint32 = htonl(dtg->datagramType);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised datagramType: %li\n", dtg->datagramType);

	twl_position++;
	twl_uint32 = htonl(dtg->no_of_actionMetaData);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised no_of_actionMetaData: %li\n", dtg->no_of_actionMetaData);

	twl_position++;
	twl_uint32 = htonl(dtg->no_of_sysMetaData);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised no_of_sysMetaData: %li\n", dtg->no_of_sysMetaData);

	twl_position++;
	twl_uint32 = htonl(dtg->no_of_pkiMetaData);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised no_of_pkiMetaData: %li\n", dtg->no_of_pkiMetaData);

	twl_position++;
	twl_uint32 = htonl(dtg->no_of_additionalMetaData);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised no_of_additionalMetaData: %li\n", dtg->no_of_additionalMetaData);


	twl_position++;
	twl_uint32 = htonl(dtg->randId);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised randId: %li\n", dtg->randId);

	twl_position++;
	twl_uint32 = htonl(dtg->filesRemaining);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised filesRemaining: %li\n", dtg->filesRemaining);

	twl_position++;
	twl_uint32 = htonl(dtg->datagramStatus);
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised datagramStatus: %li\n", dtg->datagramStatus);

	/*Wielkosci genbufow*/
	twl_position++;
	if(dtg->protocolData != NULL)
	{
		twl_uint32 = htonl(dtg->protocolData->size);
	}
	else
	{
		twl_uint32 = htonl(0);
	}
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolData: %li\n", (long)ntohl(twl_uint32));


	twl_position++;
	if(dtg->protocolDataFilename != NULL)
	{
		twl_uint32 = htonl(dtg->protocolDataFilename->size);
	}
	else
	{
		twl_uint32 = htonl(0);
	}
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolDataFilename: %li\n", (long)ntohl(twl_uint32));

	twl_position++;
	if(dtg->protocolDataFileLocationId != NULL)
	{
		twl_uint32 = htonl(dtg->protocolDataFileLocationId->size);
	}
	else
	{
		twl_uint32 = htonl(0);
	}
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolDataFileLocationId: %li\n", (long)ntohl(twl_uint32));

	twl_position++;
	if(dtg->protocolDataFileId != NULL)
	{
		twl_uint32 = htonl(dtg->protocolDataFileId->size);
	}
	else
	{
		twl_uint32 = htonl(0);
	}
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolDataFileId: %li\n", (long)ntohl(twl_uint32));

	twl_position++;
	if(dtg->protocolDataOwner != NULL)
	{
		twl_uint32 = htonl(dtg->protocolDataOwner->size);
	}
	else
	{
		twl_uint32 = htonl(0);
	}
	memcpy(twl_serialized_datagram_ptr + (twl_position * (sizeof(uint32_t) + 1)), &twl_uint32, sizeof(uint32_t));
	PRINT_VDEBUG("LIBBMDVDEBUG serialised protocolDataOwner: %li\n", (long)ntohl(twl_uint32));

	twl_position++;

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1));

	/*Wstawianie buforow z genbufow*/
	if(dtg->protocolData != NULL)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf,
		dtg->protocolData->buf,	dtg->protocolData->size);
		twl_position_with_gen_buf = dtg->protocolData->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = 1;
	}

	if(dtg->protocolDataFilename != NULL)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf,
		dtg->protocolDataFilename->buf, dtg->protocolDataFilename->size);
        	twl_position_with_gen_buf = twl_position_with_gen_buf + dtg->protocolDataFilename->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	if(dtg->protocolDataFileLocationId != NULL)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf,
		dtg->protocolDataFileLocationId->buf, dtg->protocolDataFileLocationId->size);
		twl_position_with_gen_buf = twl_position_with_gen_buf + dtg->protocolDataFileLocationId->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	if(dtg->protocolDataFileId != NULL)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf,
		dtg->protocolDataFileId->buf, dtg->protocolDataFileId->size);
		twl_position_with_gen_buf = twl_position_with_gen_buf + dtg->protocolDataFileId->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	if(dtg->protocolDataOwner != NULL)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf,
		dtg->protocolDataOwner->buf, dtg->protocolDataOwner->size);
		twl_position_with_gen_buf = twl_position_with_gen_buf + dtg->protocolDataOwner->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	twl_no_of_metadatas = dtg->no_of_actionMetaData + dtg->no_of_sysMetaData + dtg->no_of_pkiMetaData +
	dtg->no_of_additionalMetaData;
	PRINT_VDEBUG("LIBBMDVDEBUG serialised twl_no_of_metadatas: %li\n", twl_no_of_metadatas);

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	/*Teraz serializuje poszczegolne metadane*/
	/*Serializuje pola metadanych action*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_actionMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		twl_uint32 = htonl(dtg->actionMetaData[twl_iteration_0]->myId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
                twl_uint32 = htonl(dtg->actionMetaData[twl_iteration_0]->ownerType);
                memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
                &twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->actionMetaData[twl_iteration_0]->ownerId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
                twl_uint32 = htonl(dtg->actionMetaData[twl_iteration_0]->AnySize);
                memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
                &twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl( (long)strlen(dtg->actionMetaData[twl_iteration_0]->OIDTableBuf) );
		//PRINT_VDEBUG("LIBBMDVDEBUG serialised OIDTableBuf_len: %li\n",
		//(long)strlen(dtg->actionMetaData[twl_iteration_0]->OIDTableBuf));
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));
		twl_position++;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	/*Serializuje pola metadanych systemowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_sysMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		twl_uint32 = htonl(dtg->sysMetaData[twl_iteration_0]->myId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
                twl_uint32 = htonl(dtg->sysMetaData[twl_iteration_0]->ownerType);
                memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
                &twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->sysMetaData[twl_iteration_0]->ownerId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
                twl_uint32 = htonl(dtg->sysMetaData[twl_iteration_0]->AnySize);
                memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
                &twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl( (long)strlen(dtg->sysMetaData[twl_iteration_0]->OIDTableBuf) );
		//PRINT_VDEBUG("LIBBMDVDEBUG serialised OIDTableBuf_len: %li\n",
		//(long)strlen(dtg->sysMetaData[twl_iteration_0]->OIDTableBuf));
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));
		twl_position++;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	/*Serializuje pola metadanych PKI*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		twl_uint32 = htonl(dtg->pkiMetaData[twl_iteration_0]->myId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->pkiMetaData[twl_iteration_0]->ownerType);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->pkiMetaData[twl_iteration_0]->ownerId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->pkiMetaData[twl_iteration_0]->AnySize);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl( (long)strlen(dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf) );
		//PRINT_VDEBUG("LIBBMDVDEBUG serialised OIDTableBuf_len: %li\n",
                //(long)strlen(dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf));
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));
		twl_position++;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	/*Serializuje pola metadanych dodatkowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_additionalMetaData; twl_iteration_0++)
	{
		twl_uint32 = htonl(dtg->additionalMetaData[twl_iteration_0]->myId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->additionalMetaData[twl_iteration_0]->ownerType);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->additionalMetaData[twl_iteration_0]->ownerId);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl(dtg->additionalMetaData[twl_iteration_0]->AnySize);
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));

		twl_position++;
		twl_uint32 = htonl( (long)strlen(dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf) );
                //PRINT_VDEBUG("LIBBMDVDEBUG serialised OIDTableBuf_len: %li\n",
                //(long)strlen(dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf));
		memcpy(twl_serialized_datagram_ptr + ( twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf),
		&twl_uint32, sizeof(uint32_t));
		twl_position++;
	}


	/*Teraz serializuje stringi*/
	twl_position = twl_position  * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf;

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position );

	/*Serializuje wartosci oidow metadanych action*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_actionMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->actionMetaData[twl_iteration_0]->OIDTableBuf,
		strlen(dtg->actionMetaData[twl_iteration_0]->OIDTableBuf));
		twl_position = twl_position + (long)strlen(dtg->actionMetaData[twl_iteration_0]->OIDTableBuf) + 1;
	}

	/*Serializuje wartosci metadanych action*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_actionMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->actionMetaData[twl_iteration_0]->AnyBuf,
		dtg->actionMetaData[twl_iteration_0]->AnySize);
		twl_position = twl_position + dtg->actionMetaData[twl_iteration_0]->AnySize + 1;
	}

	/*Serializuje wartosci oidow metadanych systemowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_sysMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->sysMetaData[twl_iteration_0]->OIDTableBuf,
		strlen(dtg->sysMetaData[twl_iteration_0]->OIDTableBuf));
		twl_position = twl_position + (long)strlen(dtg->sysMetaData[twl_iteration_0]->OIDTableBuf) + 1;
	}

	/*Serializuje wartosci metadanych systemowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_sysMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->sysMetaData[twl_iteration_0]->AnyBuf,
		dtg->sysMetaData[twl_iteration_0]->AnySize);
		twl_position = twl_position + dtg->sysMetaData[twl_iteration_0]->AnySize + 1;
	}

	/*Serializuje wartosci oidow metadanych PKI*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf,
		strlen(dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf));
		twl_position = twl_position + (long)strlen(dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf) + 1;
        }

	/*Serializuje wartosci metadanych PKI*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->pkiMetaData[twl_iteration_0]->AnyBuf,
		dtg->pkiMetaData[twl_iteration_0]->AnySize);
		twl_position = twl_position + dtg->pkiMetaData[twl_iteration_0]->AnySize + 1;
	}



	/*Serializuje wartosci oidow metadanych systemowych*/
	for(twl_iteration_0 = 0; twl_iteration_0 < dtg->no_of_additionalMetaData; twl_iteration_0++)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf,
		strlen(dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf));
		twl_position = twl_position + (long)strlen(dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf) + 1;
	}

	/*Serializuje wartosci metadanych dodatkowych*/
	if(dtg->additionalMetaData_string_count > 0)
	{
		twl_serialized_datagram_ptr = memcpy(twl_serialized_datagram_ptr + twl_position * sizeof(char),
		dtg->additionalMetaData_string, twl_metadata_string_len);
		twl_position = twl_metadata_string_len;
	}

	for(twl_iteration_0 = dtg->additionalMetaData_string_count; twl_iteration_0 < dtg->no_of_additionalMetaData;
	twl_iteration_0++)
	{
		memcpy(twl_serialized_datagram_ptr + twl_position, dtg->additionalMetaData[twl_iteration_0]->AnyBuf,
		dtg->additionalMetaData[twl_iteration_0]->AnySize);
		twl_position = twl_position + dtg->additionalMetaData[twl_iteration_0]->AnySize + 1;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG serialisation position: %li\n", twl_position );

	/*twl_gen_buf = malloc(sizeof(GenBuf_t));
	if(twl_gen_buf == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset(twl_gen_buf, 0, sizeof(GenBuf_t));

	twl_gen_buf->size = twl_size_of_serialized_datagram;
	twl_gen_buf->buf = twl_serialized_datagram_ptr;

	*output = twl_gen_buf;*/
	//BMD_FOK(bmd_save_buf(twl_gen_buf, "/tmp/twl_save_serial.dtg"));
	/*twl_gen_buf = NULL;*/

	return BMD_OK;
}



long PR2_bmdDatagram_deserialize_TW(	char *twf_input,
					bmdDatagram_t **dtg,
					long *twf_deserialised_len,
					long *twf_memory_guard,
 					long twf_max_memory)
{
const long twlc_numbers_count			= 15;
long twl_iteration_0				= 0;
long twl_position				= 0;
long twl_size_of_deserialized_datagram		= 0;
long twl_no_of_metadatas			= 0;
long twl_metadata_values_len                    = 0;
long twl_temp_value				= 0;
long twl_position_with_gen_buf			= 0;
long *twl_array_of_oids_lens			= NULL;
long *twl_array_of_values_lens			= NULL;
long twl_memory_guard				= 0;
long twl_max_memory				= twf_max_memory;
uint32_t twl_uint32				= 0;

bmdDatagram_t *twl_dtg				= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (twf_input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dtg == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*dtg != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_deserialised_len == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (twf_memory_guard == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*twf_memory_guard < 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	twl_memory_guard = *twf_memory_guard;


	if(*twf_deserialised_len < twlc_numbers_count * (long) (sizeof(uint32_t) + 1))
	{
		BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
	}

	BMD_MEMORY_GUARD(sizeof(bmdDatagram_t));
	twl_dtg = malloc(sizeof(bmdDatagram_t));
	if(twl_dtg == NULL)			{	BMD_FOK(NO_MEMORY);		}
	memset(twl_dtg, 0, sizeof(bmdDatagram_t));

	twl_position = 0;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_size_of_deserialized_datagram = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised twl_size_of_deserialized_datagram: %li\n",
	twl_size_of_deserialized_datagram);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->protocolVersion = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolVersion: %li\n", twl_dtg->protocolVersion);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->datagramType = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised datagramType: %li\n", twl_dtg->datagramType);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->no_of_actionMetaData = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised no_of_actionMetaData: %li\n", twl_dtg->no_of_actionMetaData);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->no_of_sysMetaData = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised no_of_sysMetaData: %li\n", twl_dtg->no_of_sysMetaData);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->no_of_pkiMetaData = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised no_of_pkiMetaData: %li\n", twl_dtg->no_of_pkiMetaData);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->no_of_additionalMetaData = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised no_of_additionalMetaData: %li\n", twl_dtg->no_of_additionalMetaData);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->randId = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised randId: %li\n", twl_dtg->randId);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->filesRemaining = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised filesRemaining: %li\n", twl_dtg->filesRemaining);

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_dtg->datagramStatus = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised datagramStatus: %li\n", twl_dtg->datagramStatus);


	/*Wielkosci genbufow*/
	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolData->size %li\n", twl_temp_value);
	if(twl_temp_value > 0)
	{
		BMD_MEMORY_GUARD(sizeof(GenBuf_t));
		twl_dtg->protocolData = malloc(sizeof(GenBuf_t));
		if(twl_dtg->protocolData == NULL)	{	BMD_FOK(NO_MEMORY);	}
		twl_dtg->protocolData->size = twl_temp_value;
		twl_dtg->protocolData->buf = NULL;
	}
	else
	{
		twl_dtg->protocolData = NULL;
	}

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolDataFilename->size %li\n", twl_temp_value);
	if(twl_temp_value > 0)
	{
		BMD_MEMORY_GUARD(sizeof(GenBuf_t));
		twl_dtg->protocolDataFilename = malloc(sizeof(GenBuf_t));
		if(twl_dtg->protocolDataFilename == NULL)	{	BMD_FOK(NO_MEMORY);	}
		twl_dtg->protocolDataFilename->size = twl_temp_value;
		twl_dtg->protocolDataFilename->buf = NULL;
	}
	else
	{
		twl_dtg->protocolDataFilename = NULL;
	}

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolDataFileLocationId->size %li\n", twl_temp_value);
	if(twl_temp_value > 0)
	{
		BMD_MEMORY_GUARD(sizeof(GenBuf_t));
		twl_dtg->protocolDataFileLocationId = malloc(sizeof(GenBuf_t));
		if(twl_dtg->protocolDataFileLocationId == NULL)         {       BMD_FOK(NO_MEMORY);     }
		twl_dtg->protocolDataFileLocationId->size = twl_temp_value;
		twl_dtg->protocolDataFileLocationId->buf = NULL;
	}
	else
	{
		twl_dtg->protocolDataFileLocationId = NULL;
	}

	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolDataFileId->size %li\n", twl_temp_value);
	if(twl_temp_value > 0)
	{
		BMD_MEMORY_GUARD(sizeof(GenBuf_t));
		twl_dtg->protocolDataFileId = malloc(sizeof(GenBuf_t));
		if(twl_dtg->protocolDataFileId == NULL)		{	BMD_FOK(NO_MEMORY);	}
		twl_dtg->protocolDataFileId->size = twl_temp_value;
		twl_dtg->protocolDataFileId->buf = NULL;
	}
	else
	{
		twl_dtg->protocolDataFileId = NULL;
	}


	twl_position++;
	memcpy(&twl_uint32, twf_input + twl_position * (sizeof(uint32_t) + 1), sizeof(uint32_t));
	twl_temp_value = ntohl(twl_uint32);
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised protocolDataOwner->size %li\n", twl_temp_value);
	if(twl_temp_value > 0)
	{
		BMD_MEMORY_GUARD(sizeof(GenBuf_t));
		twl_dtg->protocolDataOwner = malloc(sizeof(GenBuf_t));
		if(twl_dtg->protocolDataOwner == NULL)	{	BMD_FOK(NO_MEMORY);	}
		twl_dtg->protocolDataOwner->size = twl_temp_value;
		twl_dtg->protocolDataOwner->buf = NULL;
	}
	else
	{
		twl_dtg->protocolDataOwner = NULL;
	}

	twl_position++;

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1));

	/*Deserializuje zawartosc genbufow*/
	if(twl_dtg->protocolData != NULL)
	{
		if(*twf_deserialised_len < twlc_numbers_count * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf +
		twl_dtg->protocolData->size + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->protocolData->size + 1);
		twl_dtg->protocolData->buf = malloc(twl_dtg->protocolData->size + 1);
		if(twl_dtg->protocolData->buf == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memcpy(twl_dtg->protocolData->buf, twf_input + twl_position * (sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf, twl_dtg->protocolData->size);
		twl_dtg->protocolData->buf[twl_dtg->protocolData->size] = '\0';
		twl_position_with_gen_buf = twl_dtg->protocolData->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = 1;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	if(twl_dtg->protocolDataFilename != NULL)
	{
		if(*twf_deserialised_len < twlc_numbers_count * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf +
		twl_dtg->protocolDataFilename->size + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->protocolDataFilename->size + 1);
		twl_dtg->protocolDataFilename->buf = malloc(twl_dtg->protocolDataFilename->size + 1);
		if(twl_dtg->protocolDataFilename->buf == NULL)  {	BMD_FOK(NO_MEMORY);	}
		memcpy(twl_dtg->protocolDataFilename->buf, twf_input + twl_position * (sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf, twl_dtg->protocolDataFilename->size);
		twl_dtg->protocolDataFilename->buf[twl_dtg->protocolDataFilename->size] = '\0';
		twl_position_with_gen_buf = twl_position_with_gen_buf + twl_dtg->protocolDataFilename->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	if(twl_dtg->protocolDataFileLocationId != NULL)
	{
		if(*twf_deserialised_len < twlc_numbers_count * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf +
		twl_dtg->protocolDataFileLocationId->size + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->protocolDataFileLocationId->size + 1);
		twl_dtg->protocolDataFileLocationId->buf = malloc(twl_dtg->protocolDataFileLocationId->size + 1);
		if(twl_dtg->protocolDataFileLocationId->buf == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memcpy(twl_dtg->protocolDataFileLocationId->buf, twf_input + twl_position * (sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf, twl_dtg->protocolDataFileLocationId->size);
		twl_dtg->protocolDataFileLocationId->buf[twl_dtg->protocolDataFileLocationId->size] = '\0';
		twl_position_with_gen_buf = twl_position_with_gen_buf + twl_dtg->protocolDataFileLocationId->size + 1;
        }
        else
        {
                twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
        }

	if(twl_dtg->protocolDataFileId != NULL)
	{
		if(*twf_deserialised_len < twlc_numbers_count * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf +
		twl_dtg->protocolDataFileId->size + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->protocolDataFileId->size + 1);
		twl_dtg->protocolDataFileId->buf = malloc(twl_dtg->protocolDataFileId->size + 1);
		if(twl_dtg->protocolDataFileId->buf == NULL)  {       BMD_FOK(NO_MEMORY);     }
		memcpy(twl_dtg->protocolDataFileId->buf, twf_input + twl_position * (sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf, twl_dtg->protocolDataFileId->size);
		twl_dtg->protocolDataFileId->buf[twl_dtg->protocolDataFileId->size] = '\0';
		twl_position_with_gen_buf = twl_position_with_gen_buf + twl_dtg->protocolDataFileId->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	if(twl_dtg->protocolDataOwner != NULL)
	{
		if(*twf_deserialised_len < twlc_numbers_count * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf +
		twl_dtg->protocolDataOwner->size + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->protocolDataOwner->size + 1);
		twl_dtg->protocolDataOwner->buf = malloc(twl_dtg->protocolDataOwner->size + 1);
		if(twl_dtg->protocolDataOwner->buf == NULL)  {		BMD_FOK(NO_MEMORY);	}
		memcpy(twl_dtg->protocolDataOwner->buf, twf_input + twl_position * (sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf, twl_dtg->protocolDataOwner->size);
		twl_dtg->protocolDataOwner->buf[twl_dtg->protocolDataOwner->size] = '\0';
		twl_position_with_gen_buf = twl_position_with_gen_buf + twl_dtg->protocolDataOwner->size + 1;
	}
	else
	{
		twl_position_with_gen_buf = twl_position_with_gen_buf + 1;
	}



	twl_dtg->additionalMetaData_string_count = twl_dtg->no_of_additionalMetaData;

	/*Alokuje miejsce na dlugosci wartosci metadanych dodatkowych w tablicy additionalMetaData_string*/
	if(twl_dtg->no_of_additionalMetaData > 0)
	{
		BMD_MEMORY_GUARD(twl_dtg->no_of_additionalMetaData * sizeof(long));
		twl_dtg->additionalMetaData_string_len = malloc(twl_dtg->no_of_additionalMetaData * sizeof(long));
		if(twl_dtg->additionalMetaData_string_len == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->additionalMetaData_string_len, 0, twl_dtg->no_of_additionalMetaData * sizeof(long));
	}

	if(twl_dtg->no_of_actionMetaData > 0)
	{
		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_actionMetaData);
		twl_dtg->actionMetaData = malloc(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_actionMetaData);
		if(twl_dtg->actionMetaData == NULL)                        {       BMD_FOK(NO_MEMORY);     }
		memset(twl_dtg->actionMetaData, 0, sizeof(MetaDataBuf_t *) * twl_dtg->no_of_actionMetaData);
	}

	if(twl_dtg->no_of_sysMetaData > 0)
	{
		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_sysMetaData);
		twl_dtg->sysMetaData = malloc(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_sysMetaData);
		if(twl_dtg->sysMetaData == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->sysMetaData, 0, sizeof(MetaDataBuf_t *) * twl_dtg->no_of_sysMetaData);
	}

	if(twl_dtg->no_of_pkiMetaData > 0)
	{
		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_pkiMetaData);
		twl_dtg->pkiMetaData = malloc(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_pkiMetaData);
		if(twl_dtg->pkiMetaData == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->pkiMetaData, 0, sizeof(MetaDataBuf_t *) * twl_dtg->no_of_pkiMetaData);
	}

	if(twl_dtg->no_of_additionalMetaData > 0)
	{
		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_additionalMetaData);
		twl_dtg->additionalMetaData = malloc(sizeof(MetaDataBuf_t *) * twl_dtg->no_of_additionalMetaData);
		if(twl_dtg->additionalMetaData == NULL)                        {       BMD_FOK(NO_MEMORY);     }
		memset(twl_dtg->additionalMetaData, 0, sizeof(MetaDataBuf_t *) * twl_dtg->no_of_additionalMetaData);
	}


	twl_no_of_metadatas = twl_dtg->no_of_actionMetaData + twl_dtg->no_of_sysMetaData + twl_dtg->no_of_pkiMetaData
	+ twl_dtg->no_of_additionalMetaData;
	PRINT_VDEBUG("LIBBMDVDEBUG deserialised twl_no_of_metadatas: %li\n", twl_no_of_metadatas);

	if(twl_no_of_metadatas > 0)
	{
		/*Alokuje tablice pomocnicza na dlugosci oidow*/
		BMD_MEMORY_GUARD(twl_no_of_metadatas * sizeof(long));
		twl_array_of_oids_lens = malloc( twl_no_of_metadatas * sizeof(long));
		if(twl_array_of_oids_lens == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_array_of_oids_lens, 0, twl_no_of_metadatas * sizeof(long));

		/*Alokuje tablice pomocnicza na dlugosci wartosci metadanych*/
		BMD_MEMORY_GUARD(twl_no_of_metadatas * sizeof(long));
		twl_array_of_values_lens = malloc( twl_no_of_metadatas * sizeof(long));
		if(twl_array_of_values_lens == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(twl_array_of_values_lens, 0, twl_no_of_metadatas * sizeof(long));
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_actionMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if(*twf_deserialised_len < (twlc_numbers_count + 5 + twl_position) * (long)(sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t));
		twl_dtg->actionMetaData[twl_iteration_0] = malloc(sizeof(MetaDataBuf_t));
		if(twl_dtg->actionMetaData[twl_iteration_0] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->actionMetaData[twl_iteration_0], 0, sizeof(MetaDataBuf_t));

		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->actionMetaData[twl_iteration_0]->myId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->actionMetaData[twl_iteration_0]->ownerType = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->actionMetaData[twl_iteration_0]->ownerId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->actionMetaData[twl_iteration_0]->AnySize = ntohl(twl_uint32);
		twl_array_of_values_lens[twl_iteration_0] = twl_dtg->actionMetaData[twl_iteration_0]->AnySize;

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));

		//PRINT_VDEBUG("LIBBMDVDEBUG deserialised OIDTableBuf_len: %li\n", (long)ntohl(twl_uint32));
		BMD_MEMORY_GUARD((ntohl(twl_uint32) + 1) * sizeof(char));
		twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf = malloc( (ntohl(twl_uint32) + 1) * sizeof(char));
		if(twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf == NULL)	{	BMD_FOK(NO_MEMORY);	}
		/*memset(twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf, 0, (ntohl(twl_uint32) + 1) *sizeof(char));*/
		twl_position++;

		twl_array_of_oids_lens[twl_iteration_0] = ntohl(twl_uint32);
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_sysMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if(*twf_deserialised_len < (twlc_numbers_count + 5 + twl_position) * (long)(sizeof(uint32_t) + 1) + twl_position_with_gen_buf)
		{
// 			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
			/* prawdopodobnie sprawdzenie warunku jest niepoprawne dla mniejszych ilosci metadanych systemowych */
		}

		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t));
		twl_dtg->sysMetaData[twl_iteration_0] = malloc(sizeof(MetaDataBuf_t));
		if(twl_dtg->sysMetaData[twl_iteration_0] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->sysMetaData[twl_iteration_0], 0, sizeof(MetaDataBuf_t));


		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->sysMetaData[twl_iteration_0]->myId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->sysMetaData[twl_iteration_0]->ownerType = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->sysMetaData[twl_iteration_0]->ownerId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->sysMetaData[twl_iteration_0]->AnySize = ntohl(twl_uint32);
		twl_array_of_values_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData] =
		twl_dtg->sysMetaData[twl_iteration_0]->AnySize;

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));

		//PRINT_VDEBUG("LIBBMDVDEBUG deserialised OIDTableBuf_len: %li\n", (long)ntohl(twl_uint32));
		BMD_MEMORY_GUARD((ntohl(twl_uint32) + 1) * sizeof(char));
		twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf = malloc( (ntohl(twl_uint32) + 1) * sizeof(char));
		if(twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf == NULL)	{	BMD_FOK(NO_MEMORY);	}
		/*memset(twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf, 0, (ntohl(twl_uint32) + 1) * sizeof(char));*/
		twl_position++;

		twl_array_of_oids_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData] = ntohl(twl_uint32);
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);


	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if(*twf_deserialised_len < (twlc_numbers_count + 5 + twl_position) * (long)(sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t));
		twl_dtg->pkiMetaData[twl_iteration_0] = malloc(sizeof(MetaDataBuf_t));
		if(twl_dtg->pkiMetaData[twl_iteration_0] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->pkiMetaData[twl_iteration_0], 0, sizeof(MetaDataBuf_t));


		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->pkiMetaData[twl_iteration_0]->myId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->pkiMetaData[twl_iteration_0]->ownerType = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->pkiMetaData[twl_iteration_0]->ownerId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->pkiMetaData[twl_iteration_0]->AnySize = ntohl(twl_uint32);
		twl_array_of_values_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData + twl_dtg->no_of_sysMetaData] =
		twl_dtg->pkiMetaData[twl_iteration_0]->AnySize;

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));

		//PRINT_VDEBUG("LIBBMDVDEBUG deserialised OIDTableBuf_len: %li\n", (long)ntohl(twl_uint32));
		BMD_MEMORY_GUARD((ntohl(twl_uint32) + 1) * sizeof(char));
		twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf = malloc((ntohl(twl_uint32) + 1) * sizeof(char));
		if(twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf == NULL)  {	BMD_FOK(NO_MEMORY);	}
		/*memset(twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf, 0, (ntohl(twl_uint32) + 1) * sizeof(char));*/
		twl_position++;

		twl_array_of_oids_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData + twl_dtg->no_of_sysMetaData] =
		ntohl(twl_uint32);
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);

	PRINT_TEST("LIBBMDVDEBUG deserialisation position: %li\n", twl_position * (sizeof(uint32_t) + 1) +
	twl_position_with_gen_buf);


	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_additionalMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if(*twf_deserialised_len < (twlc_numbers_count + 5 + twl_position) * (long)(sizeof(uint32_t) + 1) +
		twl_position_with_gen_buf)
		{
// 			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(sizeof(MetaDataBuf_t));
		twl_dtg->additionalMetaData[twl_iteration_0] = malloc(sizeof(MetaDataBuf_t));
		if(twl_dtg->additionalMetaData[twl_iteration_0] == NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->additionalMetaData[twl_iteration_0], 0, sizeof(MetaDataBuf_t));


		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->additionalMetaData[twl_iteration_0]->myId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->additionalMetaData[twl_iteration_0]->ownerType = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->additionalMetaData[twl_iteration_0]->ownerId = ntohl(twl_uint32);

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));
		twl_dtg->additionalMetaData[twl_iteration_0]->AnySize = ntohl(twl_uint32);
		twl_array_of_values_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData + twl_dtg->no_of_sysMetaData +
		twl_dtg->no_of_pkiMetaData] = twl_dtg->additionalMetaData[twl_iteration_0]->AnySize;

		twl_metadata_values_len = twl_metadata_values_len +
		twl_dtg->additionalMetaData[twl_iteration_0]->AnySize + 1;
		twl_dtg->additionalMetaData_string_len[twl_iteration_0] =
		twl_dtg->additionalMetaData[twl_iteration_0]->AnySize;

		twl_position++;
		memcpy(&twl_uint32, twf_input + twl_position_with_gen_buf + twl_position * (sizeof(uint32_t) + 1),
		sizeof(uint32_t));

		//PRINT_VDEBUG("LIBBMDVDEBUG deserialised OIDTableBuf_len: %li\n", (long)ntohl(twl_uint32));
		BMD_MEMORY_GUARD((ntohl(twl_uint32) + 1) * sizeof(char));
		twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf = malloc((ntohl(twl_uint32) + 1) * sizeof(char));
		if(twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf == NULL)  {	BMD_FOK(NO_MEMORY);	}
		/*memset(twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf,0,(ntohl(twl_uint32) + 1)*sizeof(char));*/
		twl_position++;

		twl_array_of_oids_lens[twl_iteration_0 + twl_dtg->no_of_actionMetaData + twl_dtg->no_of_sysMetaData +
		twl_dtg->no_of_pkiMetaData] = ntohl(twl_uint32);
	}

	/*Deserializuje wartosci metadanych i oidy*/
	twl_position = twl_position * (sizeof(uint32_t) + 1) + twl_position_with_gen_buf;

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation position: %li\n", twl_position);

	twl_no_of_metadatas = 0;
	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_actionMetaData; twl_iteration_0++, twl_no_of_metadatas++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		memcpy(twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf, twf_input + twl_position,
		twl_array_of_oids_lens[twl_no_of_metadatas]);
		twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf[twl_array_of_oids_lens[twl_no_of_metadatas]] = '\0';
		twl_position = twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1;
		PRINT_VDEBUG("LIBBMDVDEBUG OID: %s\n", twl_dtg->actionMetaData[twl_iteration_0]->OIDTableBuf);
	}

	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_actionMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_dtg->actionMetaData[twl_iteration_0]->AnySize + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->actionMetaData[twl_iteration_0]->AnySize + 1);
		twl_dtg->actionMetaData[twl_iteration_0]->AnyBuf = malloc(twl_dtg->actionMetaData[twl_iteration_0]->
		AnySize + 1);
		if(twl_dtg->actionMetaData[twl_iteration_0]->AnyBuf == NULL)	{	BMD_FOK(NO_MEMORY)	}
		/*memset(twl_dtg->actionMetaData[twl_iteration_0]->AnyBuf, 0, twl_dtg->actionMetaData[twl_iteration_0]->
		AnySize + 1);*/

		memcpy(twl_dtg->actionMetaData[twl_iteration_0]->AnyBuf, twf_input + twl_position,
		twl_dtg->actionMetaData[twl_iteration_0]->AnySize);
		twl_dtg->actionMetaData[twl_iteration_0]->AnyBuf[twl_dtg->actionMetaData[twl_iteration_0]->AnySize] = '\0';
		twl_position = twl_position + twl_dtg->actionMetaData[twl_iteration_0]->AnySize + 1;
	}


	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_sysMetaData; twl_iteration_0++, twl_no_of_metadatas++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		memcpy(twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf, twf_input + twl_position,
		twl_array_of_oids_lens[twl_no_of_metadatas]);
		twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf[twl_array_of_oids_lens[twl_no_of_metadatas]] = '\0';
		twl_position = twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1;
		PRINT_VDEBUG("LIBBMDVDEBUG OID: %s\n", twl_dtg->sysMetaData[twl_iteration_0]->OIDTableBuf);
	}

	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_sysMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_dtg->sysMetaData[twl_iteration_0]->AnySize + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->sysMetaData[twl_iteration_0]->AnySize + 1);
		twl_dtg->sysMetaData[twl_iteration_0]->AnyBuf = malloc(twl_dtg->sysMetaData[twl_iteration_0]->AnySize + 1);
		if(twl_dtg->sysMetaData[twl_iteration_0]->AnyBuf == NULL)	{	BMD_FOK(NO_MEMORY)	}
		/*memset(twl_dtg->sysMetaData[twl_iteration_0]->AnyBuf, 0, twl_dtg->sysMetaData[twl_iteration_0]->AnySize);*/

		memcpy(twl_dtg->sysMetaData[twl_iteration_0]->AnyBuf, twf_input + twl_position,
		twl_dtg->sysMetaData[twl_iteration_0]->AnySize);
		twl_dtg->sysMetaData[twl_iteration_0]->AnyBuf[twl_dtg->sysMetaData[twl_iteration_0]->AnySize] = '\0';
		twl_position = twl_position + twl_dtg->sysMetaData[twl_iteration_0]->AnySize + 1;
	}


	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_pkiMetaData; twl_iteration_0++, twl_no_of_metadatas++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		memcpy(twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf, twf_input + twl_position,
		twl_array_of_oids_lens[twl_no_of_metadatas]);
		twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf[twl_array_of_oids_lens[twl_no_of_metadatas]] = '\0';
		twl_position = twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1;
		//PRINT_VDEBUG("LIBBMDVDEBUG OID: %s\n", twl_dtg->pkiMetaData[twl_iteration_0]->OIDTableBuf);
	}

	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_pkiMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_dtg->pkiMetaData[twl_iteration_0]->AnySize + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		BMD_MEMORY_GUARD(twl_dtg->pkiMetaData[twl_iteration_0]->AnySize + 1);
		twl_dtg->pkiMetaData[twl_iteration_0]->AnyBuf = malloc(twl_dtg->pkiMetaData[twl_iteration_0]->AnySize + 1);
		if(twl_dtg->pkiMetaData[twl_iteration_0]->AnyBuf == NULL)       {       BMD_FOK(NO_MEMORY)      }
		/*memset(twl_dtg->pkiMetaData[twl_iteration_0]->AnyBuf,0, twl_dtg->pkiMetaData[twl_iteration_0]->AnySize);*/
		memcpy(twl_dtg->pkiMetaData[twl_iteration_0]->AnyBuf, twf_input + twl_position,
		twl_dtg->pkiMetaData[twl_iteration_0]->AnySize);
		twl_dtg->pkiMetaData[twl_iteration_0]->AnyBuf[twl_dtg->pkiMetaData[twl_iteration_0]->AnySize] = '\0';
		twl_position = twl_position + twl_dtg->pkiMetaData[twl_iteration_0]->AnySize + 1;
	}


	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_additionalMetaData;
	twl_iteration_0++, twl_no_of_metadatas++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);

		if( *twf_deserialised_len < twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1)
		{
			BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
		}

		memcpy(twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf, twf_input + twl_position,
		twl_array_of_oids_lens[twl_no_of_metadatas]);
		twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf[twl_array_of_oids_lens[twl_no_of_metadatas]] ='\0';
		twl_position = twl_position + twl_array_of_oids_lens[twl_no_of_metadatas] + 1;
		//PRINT_VDEBUG("LIBBMDVDEBUG OID: %s\n", twl_dtg->additionalMetaData[twl_iteration_0]->OIDTableBuf);
	}

	if( *twf_deserialised_len < twl_position + twl_metadata_values_len * (long)sizeof(char))
	{
		BMD_FOK(LIBBMDPR_DATAGRAMSET_TO_SMALL);
	}

	if(twl_metadata_values_len > 0)
	{
		BMD_MEMORY_GUARD(twl_metadata_values_len * sizeof(char));
		twl_dtg->additionalMetaData_string = malloc(twl_metadata_values_len * sizeof(char));
		if(twl_dtg->additionalMetaData_string == NULL)		{	BMD_FOK(NO_MEMORY);	}
		memset(twl_dtg->additionalMetaData_string, 0, twl_metadata_values_len * sizeof(char));
		memcpy(twl_dtg->additionalMetaData_string, twf_input + twl_position, twl_metadata_values_len *sizeof(char));
	}
	twl_position = twl_position + twl_metadata_values_len;

	twl_metadata_values_len = 0;
	/*Teraz wystarczy podpiac tylko wskazniki*/
	for(twl_iteration_0 = 0; twl_iteration_0 < twl_dtg->no_of_additionalMetaData; twl_iteration_0++)
	{
		//PRINT_VDEBUG("LIBBMDVDEBUG twl_iteration_0: %li\n", twl_iteration_0);
		twl_dtg->additionalMetaData[twl_iteration_0]->AnyBuf = twl_dtg->additionalMetaData_string +
		twl_metadata_values_len;
		twl_metadata_values_len = twl_metadata_values_len + 1 +
		twl_dtg->additionalMetaData_string_len[twl_iteration_0];
	}

	PRINT_VDEBUG("LIBBMDVDEBUG deserialisation len: %li\n", twl_position);
	*twf_deserialised_len = twl_size_of_deserialized_datagram;

	*dtg = twl_dtg;
	twl_dtg = NULL;

	PRINT_VDEBUG("LIBBMDVDEBUG Memory guard after datagram deserialisation: %li bytes\n", twl_memory_guard);
	*twf_memory_guard = twl_memory_guard;

	free(twl_array_of_oids_lens); twl_array_of_oids_lens = NULL;
	free(twl_array_of_values_lens); twl_array_of_values_lens = NULL;

	return BMD_OK;
}
/*######################################################################*/

long PR2_bmdDatagram_serialize(bmdDatagram_t *dtg,GenBuf_t **output)
{
BMDDatagram_t *asn1_dtg		= NULL;

	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* konwersja */
	BMD_FOK(_PR2_bmdDatagram2BMDDatagram(dtg,&asn1_dtg));

	/* zakodowanie do DER datagramu */
	BMD_FOK(_PR2_BMDDatagram_der_encode(asn1_dtg,output));

	asn_DEF_BMDDatagram.free_struct(&asn_DEF_BMDDatagram,asn1_dtg,0);asn1_dtg=NULL;

	return BMD_OK;
}

long PR2_bmdDatagram_deserialize(GenBuf_t *input,bmdDatagram_t **dtg)
{
BMDDatagram_t *asn1_dtg = NULL;
long status			= 0;

	if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*dtg)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(_PR2_BMDDatagram_der_decode(input,&asn1_dtg),
			BMD_ERR_OP_FAILED);

	(*dtg)=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	if((*dtg) == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*dtg,0,sizeof(bmdDatagram_t));

	/* wartosci INTEGER */
	status=asn_INTEGER2long(&(asn1_dtg->protocolVersion), &((*dtg)->protocolVersion));
	status=asn_INTEGER2long(&(asn1_dtg->datagramType), &((*dtg)->datagramType));
	status=asn_INTEGER2long(&(asn1_dtg->randId), &((*dtg)->randId));
	status=asn_INTEGER2long(&(asn1_dtg->filesRemaining), &((*dtg)->filesRemaining));
	status=asn_INTEGER2long(&(asn1_dtg->datagramStatus), &((*dtg)->datagramStatus));

	/* wartosci OCTET_STRING */
	if(asn1_dtg->protocolData)
	{
		status=set_gen_buf2((char *)(asn1_dtg->protocolData->buf),
				asn1_dtg->protocolData->size,
				&((*dtg)->protocolData));
	}
	if(asn1_dtg->protocolDataFilename)
	{

		status=set_gen_buf2((char *)(asn1_dtg->protocolDataFilename->buf),
				asn1_dtg->protocolDataFilename->size,
				&((*dtg)->protocolDataFilename));
	}
	if(asn1_dtg->protocolDataFileLocationId)
	{

		status=set_gen_buf2((char *)(asn1_dtg->protocolDataFileLocationId->buf),
				asn1_dtg->protocolDataFileLocationId->size,
				&((*dtg)->protocolDataFileLocationId));
	}

	if(asn1_dtg->protocolDataFileId)
	{

		status=set_gen_buf2((char *)(asn1_dtg->protocolDataFileId->buf),
				asn1_dtg->protocolDataFileId->size,
				&((*dtg)->protocolDataFileId));
	}
	if(asn1_dtg->protocolDataOwner)
	{

		status=set_gen_buf2((char *)(asn1_dtg->protocolDataOwner->buf),
				    asn1_dtg->protocolDataOwner->size,
				    &((*dtg)->protocolDataOwner));
	}

	/* listy metadanych */
	status=_PR_MetaData2MetaDataBuf((struct MetaDatas *)asn1_dtg->actionMetaDatas,
					&((*dtg)->actionMetaData),
					&((*dtg)->no_of_actionMetaData), NULL);

	status=_PR_MetaData2MetaDataBuf((struct MetaDatas *)asn1_dtg->sysMetaDatas,
					&((*dtg)->sysMetaData),
					&((*dtg)->no_of_sysMetaData), NULL);

	status=_PR_MetaData2MetaDataBuf((struct MetaDatas *)asn1_dtg->additionalMetaDatas,
					&((*dtg)->additionalMetaData),
					&((*dtg)->no_of_additionalMetaData), *dtg);

	status=_PR_MetaData2MetaDataBuf((struct MetaDatas *)asn1_dtg->pkiMetaDatas,
					&((*dtg)->pkiMetaData),
					&((*dtg)->no_of_pkiMetaData), NULL);


	asn_DEF_BMDDatagram.free_struct(&asn_DEF_BMDDatagram,asn1_dtg,0);

	asn1_dtg=NULL;

	return BMD_OK;
}

long PR2_bmdDatagram_free(bmdDatagram_t **dtg)
{
	if(dtg==NULL)		{	return BMD_OK;			}
	if (*dtg == NULL)	{	return BMD_OK;			}

	_PR_bmdDatagram_free(*dtg,1);*dtg=NULL;

	return BMD_OK;
}

/*Jezeli twf_copy_ptr = 1 to z DerEncodedValue jest kopiowany sam wskaznik, a nie jest kopiowana tresc do nowego
bufora. */
long PR2_bmdDatagram_add_metadata(	char *OIDtable,
					const char *Buf,
					long BufSize,
					EnumMetaData_t KindOfMetaData,
					bmdDatagram_t *bmdDatagram,
					long myId,
					long ownerType,
					long ownerId,
					long twf_copy_ptr)
{
GenBuf_t gb;
	/* MSILEWICZ - zakomentowalem dla czytelnosci logow */
	//PRINT_INFO("LIBBMDPRINF Adding metedata to datagram\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(OIDtable==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(Buf==NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(bmdDatagram==NULL)				{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(twf_copy_ptr < 0 || twf_copy_ptr > 1)	{	BMD_FOK(BMD_ERR_PARAM10);	}

	memset(&gb,0,sizeof(GenBuf_t));

	gb.buf=(char *)Buf;
	gb.size=BufSize;
	/*GenBuf2Octetstring2Any(&gb,&tmpAny);*/

	switch(KindOfMetaData)
	{
		case ACTION_METADATA:
			AddElementToMetaDataTable(	&(bmdDatagram->actionMetaData),
								&(bmdDatagram->no_of_actionMetaData),
								OIDtable,
								&gb,
								myId,
								ownerType,
								ownerId,
								twf_copy_ptr);
			break;
		case SYS_METADATA:

		AddElementToMetaDataTable(	&(bmdDatagram->sysMetaData),
							&(bmdDatagram->no_of_sysMetaData),
							OIDtable,
							&gb,
							myId,
							ownerType,
							ownerId,
							twf_copy_ptr);
		break;


		case PKI_METADATA:

		AddElementToMetaDataTable(	&(bmdDatagram->pkiMetaData),
							&(bmdDatagram->no_of_pkiMetaData),
							OIDtable,
							&gb,
							myId,
							ownerType,
							ownerId,
							twf_copy_ptr);
		break;


		case UNDEFINED_METADATA:
		case ADDITIONAL_METADATA:

		AddElementToMetaDataTable(	&(bmdDatagram->additionalMetaData),
							&(bmdDatagram->no_of_additionalMetaData),
							OIDtable,
							&gb,
							myId,
							ownerType,
							ownerId,
							twf_copy_ptr);
		break;


		default:
		BMD_FOK(UNKNOWN_METADATA_TYPE);
		break;
	}

	/*free(tmpAny.buf);tmpAny.buf=NULL;*/

	return BMD_OK;
}

long PR2_bmdDatagram_create(bmdDatagram_t **bmdDatagram)
{
	long err;

	if(bmdDatagram==NULL)
		return BMD_ERR_PARAM1;

	*bmdDatagram = (bmdDatagram_t *) malloc (sizeof(bmdDatagram_t));
	if( *bmdDatagram == NULL )
		return BMD_ERR_MEMORY;

	err = PR_bmdDatagram_init(*bmdDatagram);
	if(err < 0)
		return BMD_ERR_OP_FAILED;

	return BMD_OK;
}
