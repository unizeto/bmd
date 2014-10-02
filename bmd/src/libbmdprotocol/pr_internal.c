#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>

#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/**
*Funkcja dopisuje prefix powstały z przekształcenia wartości z postaci uint32 na postać sieciową na początek bufora
*@param value wartość w postaci uint23
*@param output bufor do który zawiera dane, które mają zostać poprzedzone prefiksem
*@retval 0 poprawne wykonanie funkcji
*@retval -1 bład podczas wykonywania funkcji
*/

long set_prefix_from_int(uint32_t value, long twf_size, GenBuf_t **output)
{
	uint32_t net_value	= 0;
	uint32_t size		= 0;
	char *tmp		= NULL;
	long temp_value		= 0;

	PRINT_INFO("LIBBMDPROTOCOLINF Setting prefix from int.\n");

	if(twf_size <= 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if( output == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( *output == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (*output)->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (*output)->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	tmp=realloc((*output)->buf, (*output)->size + 2 * sizeof(uint32_t));
	if(tmp == NULL)			{	BMD_FOK(NO_MEMORY);	}

	(*output)->buf=tmp;
	memmove((*output)->buf + 2 * sizeof(uint32_t), (*output)->buf, (*output)->size);

	/*Identyfikator zadania*/
	temp_value = value;
	PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG Session request value: %li\n", temp_value);
	net_value=htonl(value);
	memcpy((*output)->buf + sizeof(uint32_t), &net_value, sizeof(uint32_t));

	/*wielkosc paczki*/
	/*size = (*output)->size + sizeof(uint32_t);*/
	size = twf_size + sizeof(uint32_t);
	temp_value = size;
	PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG Packet size value: %li\n", temp_value);
	net_value=htonl(size);
	memcpy((*output)->buf, &net_value, sizeof(uint32_t));

	(*output)->size=(*output)->size + 2 * sizeof(uint32_t) ;

	return BMD_OK;

}

/**
*Funkcja wydziela prefix z danych zapisanych w buforze
*@param input bufor zawierający dane poprzedzone prefixem
*@param id wynik działania funkcji - wydzielony prefix
*@retval 0 poprawne wykonanie funkcji
*/
long get_prefix_from_gb(GenBuf_t *input, uint32_t *id)
{
uint32_t *tmp           = NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Getting prefix from gen_buf.\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(input == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size <= sizeof(uint32_t))	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(id == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	tmp = (uint32_t *)input->buf;
	*id = ntohl(*tmp);

	PRINT_VDEBUG("LIBBMDPROTOCOLVDEBUG Prefix %u\n", *id);

	return BMD_OK;
}

/**
*Funkcja zwraca długość bloku danych z paczki typu asn1
*@param buffer bufor zawierający paczkę asn1
*@retval >0 długość bloku danych
*@retval -1 niepoprawne wykonanie funkcji
*@bug length parametr nie używany w ciele funkcji
*/
long get_asn1_data_block_size(char *buffer,long length)
{
	long dlugosc_paczki=0;

	long i;
	char *tmp_buf=NULL,octet[3];

	if(buffer==NULL)
		return -1;

	if(buffer[1]&0x80)	/* jesli najstarszy bit pierwszego bajtu to jedynka... */
	{
		tmp_buf=(char *)malloc((buffer[1]&0x7F)*2+2);	/* to nastepne 7 bitow mowi na ilu bajtach kodowana jest dlugosc paczki */
		memset(tmp_buf,0,(buffer[1]&0x7F)*2+1);
		for(i=0;i<(buffer[1]&0x7F);i++)			/* poszczegolne bajty jako lanuchy HEX do char * */
		{
			if(buffer[2+i]<16)
				sprintf(octet,"0%X",buffer[2+i]);
			else
				sprintf(octet,"%X",buffer[2+i]);
			strcat(tmp_buf,octet);
			memset(octet,0,3);
		}
		dlugosc_paczki=strtol(tmp_buf,(char **)NULL,16);	/* konwersja lancucha HEX na liczbe long */
		dlugosc_paczki+=2+(buffer[1]&0x7F);
		if(tmp_buf)
		{
			free(tmp_buf);tmp_buf=NULL;
		}
	}
	else
		dlugosc_paczki=buffer[1]+2;	/* dlugosc kodowana na jednym bajcie */
	return dlugosc_paczki;
}

/**
*Funkcja pobiera dane asn1 z gniazda
*@param handler uchwyt wskazujący gniazdo połaczenia
*@param buf bufor do którego zostaną zapisane otrzymane dane
*@retval >0 długość otrzymanych danych
*@retval BMD_ERR_NET_READ bład odczytania danych z gniazda
*@retval BMD_ERR_TOO_BIG bład przekroczenia maksymalnego rozmiaru odczytywanych danych
*/

long obsolote_asn1_get_from_socket(bmdnet_handler_ptr handler,char **buf)
{
	long block_size=0;
	long received_count=0;
	char buffer[ASN1_DATAGRAM_HDR_SIZE+1];
	/*
	UWAGA!!!
	ASN1_DATAGRAM_HDR_SIZE zdefiniowany na 9 w bmd-const.h
	W przypadku duzych ilosci danych, ich wielkosc zapisana jest w formie dlugiej tj.
	wartosc lenght postaci DER zapisana jest na wielu bajtach. Do tej pory na wartosc lenght
	przewidywalismy 3 bajty (nie liczac tego informujacego, na ilu bajtach zapisana jest sama dlugosc),
	jednak dla duzych ilosci danych potrzebny byl jeszcze jeden bajt. Jesli go brakowalo,
	dlugosc byla blednie obliczana. 3 bajty daja wartosci od 0 do niespelna 17 milionow,
	co przy wyliczaniu rozmiaru danych w bajtach moze byc niewystarczajace.
	Teraz pobieranych jest 10 bajtow, a nie 9 jak to bylo poprzednio.
	bajty #0-#3 to identyfikator żądania
	bajt #4 to poczatek DER: identyfikator asn.1 (OCTET STRING)
	bajt #5 to bajt okreslajacy, na ilu bajtach zapisana jest sama dlugosc danych
	bajty #6-#9 to bajty, na ktorych moze byc zapisana dlugosc danych (4 bajty daja pare miliardow)
	*/
	ssize_t tmp;
	char *data_write_ptr;
	long bytes_to_get;

	received_count=bmdnet_recv_min(handler,buffer,sizeof(buffer),sizeof(buffer));
	/*PRINT_VDEBUG("asn1_get_from_socket: received_count %i\n",received_count);*/

	if(received_count!=sizeof(buffer))
	{
		PRINT_ERROR("Recived count = %li\n", received_count );
		return BMD_ERR_NET_READ;
	}

	block_size=get_asn1_data_block_size(buffer+sizeof(uint32_t),received_count-sizeof(uint32_t));

	if(block_size<sizeof(buffer))
	{
		PRINT_ERROR("block_size = %li\n", block_size );
		return BMD_ERR_NET_READ;
	}

	if(block_size>MAX_FILE_SIZE)
	{
		return BMD_ERR_TOO_BIG;
	}

	(*buf)=(char *)malloc(block_size+sizeof(uint32_t)+16); /* Duze aplikacje musza uzywac ogromnych ilosci pamieci
							   Stad dodajemy 16 bajtow do rozmiaru bloku. Poza tym
							   jak cos pojdzie nie tak, to bedzie po czym jezdzic.
							   Podpisano Af1n */
	/*memset(*buf,0,block_size+received_count+1);*/

	data_write_ptr=(char *)(*buf);
	memcpy(*buf,buffer,received_count);
	data_write_ptr+=received_count;

	bytes_to_get=block_size+sizeof(uint32_t)-received_count;
	while(bytes_to_get>0)
	{

		/*memset(buffer,0,32768);*/
		tmp=bmdnet_recv(handler,data_write_ptr,bytes_to_get);
		if(tmp<0)
		{
			PRINT_ERROR("bmdnet_recv failed rc=%d\n", (int)tmp );
			return BMD_ERR_NET_READ;
		}

		data_write_ptr+=tmp;
		bytes_to_get-=(long)tmp;
		received_count+=(long)tmp;
	}

	//PRINT_VDEBUG("bmdnet_recv received_count(rc) %i and tmp %i\n",received_count,tmp);
	return received_count;
}

/**
*Funkcja pobiera typ żądania z otrzymanych danych z gniazda i przekazuje je na bufor wyjściowy
*@param input_buf bufor zawierający dane odebrane z gniazda
*@param id zmienna, do której zostanie zapisany prefix poprzedzający dane w buforze wejściowym
*@param stripped_buf bufor, do którego zapisany typ żądania w przypadku gdy jest NULL, to nie ma alokowania nowego bufora na
		obciety bufor.
*@retval 0 prawidłowa wykonanie funkcji
*/
long strip_prefix(GenBuf_t **input_buf,uint32_t *id,GenBuf_t **stripped_buf)
{
	long tmp_size		= 0;

	if(input_buf == NULL)		{       BMD_FOK(BMD_ERR_PARAM1);	}
	if(*input_buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(id == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(stripped_buf != NULL)
	{
		if(*stripped_buf != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	}


	BMD_FOK(get_prefix_from_gb(*input_buf, id));

	if(stripped_buf != NULL)
	{
		tmp_size = (*input_buf)->size - sizeof(uint32_t);
		BMD_FOK(set_gen_buf((*input_buf)->buf + sizeof(uint32_t), &tmp_size, stripped_buf));
		free_gen_buf(input_buf);
		*input_buf = NULL;
	}

	return BMD_OK;
}


/** pobiera dane z gniazda do bufora odbiorczego.**/
long bmd_get_from_socket(bmdnet_handler_ptr handler, GenBuf_t **output)
{
long bytes_to_get               = 0;
long received_count             = 0;
long final_received_count       = 0;
uint32_t net_value              = 0;
char *buffer                    = NULL;

        /*req->receive_buffer=(GenBuf_t *)malloc(sizeof(GenBuf_t));
        req->receive_buffer->buf=NULL;*/
	PRINT_INFO("BMDSERVERINF Switching data from socket to receiving buffor\n");

	if(handler == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*output != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	buffer =  malloc(sizeof(uint32_t) + 1);
	if(buffer == NULL)              {       BMD_FOK(NO_MEMORY);     }
	memset(buffer, 0, sizeof(uint32_t) + 1);

	/*Pobieram najpierw 4 pierwsze bajty zeby ustalic wielkosc pakietu.*/
	received_count=bmdnet_recv_min(handler, buffer, sizeof(uint32_t), sizeof(uint32_t));
	PRINT_VDEBUG("BMDSERVERVDEBUG Recived count: %li\n", received_count);
	if(received_count != sizeof(uint32_t))
	{
		BMD_FOK(BMD_ERR_NET_READ);
	}
	memcpy(&net_value, buffer, sizeof(uint32_t));
	free(buffer);
	buffer = NULL;


        /*Ustalam ile bajtow ma wlasciwa informacja*/
	bytes_to_get = ntohl(net_value);
	PRINT_VDEBUG("BMDSERVERVDEBUG Bytes to get: %li\n", bytes_to_get);

        /*Alokuje bufor na wynik*/
        buffer = (char *)malloc(bytes_to_get + 1);
        if(buffer == NULL)		{	BMD_FOK(NO_MEMORY);	};
        memset(buffer, 0, bytes_to_get + 1);

	/*Alokuje strukture bufora wyjsciowego*/
        *output=(GenBuf_t *)malloc(sizeof(GenBuf_t));
        if(*output == NULL)
	{
		free(buffer);
		buffer = NULL;
		BMD_FOK(NO_MEMORY);
	};
	memset(*output, 0, sizeof(GenBuf_t));

	(*output)->buf = (char*)buffer;

	final_received_count = 0;

	/*Petla pobierajaca wlasciwy wynik.*/
	//while(bytes_to_get>0)
	//{
		received_count = bmdnet_recv_min(handler, buffer, bytes_to_get,bytes_to_get);
		if(received_count < 0)
		{
			PRINT_ERROR("bmdnet_recv failed received count: %li\n", received_count );
			free_gen_buf(output);
			return BMD_ERR_NET_READ;
		}

		//buffer = buffer + received_count;
		//bytes_to_get = bytes_to_get - received_count;
		//final_received_count = final_received_count + received_count;
	//}
	//(*output)->size = final_received_count;
	(*output)->size=bytes_to_get;
	PRINT_VDEBUG("BMDSERVERVDEBUG Bytes received: %li\n", final_received_count);

	//(*output)->buf=(char *)malloc(bytes_to_get);
	//memmove((*output)->buf,buffer,bytes_to_get);
	//free(buffer);
	buffer = NULL; /*Nie zwalniac!*/
	return BMD_OK;
}

long bmd2_datagram_add_metadata_char(	const char *oid,
					char *value,
					bmdDatagram_t **datagram)
{
long type	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (oid == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (value == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (datagram == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	type=determine_oid_type_str((char *)oid);
	BMD_FOK(type);

	BMD_FOK(PR2_bmdDatagram_add_metadata((char *)oid, value, (long)strlen(value)+1, type, *datagram, 0, 0, 0, 0));

	return BMD_OK;
}

