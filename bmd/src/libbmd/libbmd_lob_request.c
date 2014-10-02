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
#include <wchar.h>

/* tworzy strukture lob_request_info_t */
long bmd_lob_request_info_create(lob_request_info_t **lob_info)
{
	if(lob_info==NULL)
		return BMD_ERR_PARAM1;
	if((*lob_info)!=NULL)
		return BMD_ERR_PARAM1;

	(*lob_info)=(lob_request_info_t *)malloc(sizeof(lob_request_info_t));
	memset(*lob_info,0,sizeof(lob_request_info_t));

	return BMD_OK;
}

/* niszczy strukture */
long bmd_lob_request_info_destroy(lob_request_info_t **lob_info)
{
	long i=0;

	if(lob_info==NULL)
		return BMD_ERR_PARAM1;
	if((*lob_info)==NULL)
		return BMD_ERR_PARAM1;

	if((*lob_info)->no_of_files_to_send)
	{
		for(i=0;i<(*lob_info)->no_of_files_to_send;i++)
		{
			if ((*lob_info)->files_to_send != NULL)
			{
				free((*lob_info)->files_to_send[i]);
				(*lob_info)->files_to_send[i]=NULL;
			}
			if ( (*lob_info)->lob_contexts != NULL )
			{	bmd_ctx_destroy(&((*lob_info)->lob_contexts[i]));
			}
		}
	}
	
	if((*lob_info)->no_of_buffers_to_send > 0)
	{
		for(i=0; i<(*lob_info)->no_of_buffers_to_send; i++)
		{
			if((*lob_info)->buffers_to_send != NULL)
				{ free((*lob_info)->buffers_to_send[i].buf); }
		}
		free((*lob_info)->buffers_to_send);
		(*lob_info)->buffers_to_send=NULL;
		(*lob_info)->no_of_buffers_to_send=0;
	}
	
	if((*lob_info)->no_of_files_to_recv)
	{
		for(i=0;i<(*lob_info)->no_of_files_to_recv;i++)
		{
			if ((*lob_info)->files_to_recv)
			{	free((*lob_info)->files_to_recv[i]);
				(*lob_info)->files_to_recv[i]=NULL;
			}
		}
		if ((*lob_info)->lob_contexts!=NULL)
		{
			for(i=0;i<(*lob_info)->no_of_files_to_recv;i++)
			{
				bmd_ctx_destroy(&((*lob_info)->lob_contexts[i]));
			}
		}
	}

	free((*lob_info)->files_to_send);
	(*lob_info)->files_to_send=NULL;
	free((*lob_info)->files_to_recv);
	(*lob_info)->files_to_recv=NULL;
	free((*lob_info)->recv_files_sizes);
	(*lob_info)->recv_files_sizes=NULL;
	free((*lob_info)->lob_contexts);(*lob_info)->lob_contexts=NULL;
	free((*lob_info));
	(*lob_info)=NULL;

	return BMD_OK;
}

/******************************************
*	dodaje plik do struktury lob_info
*
*	@param filename to sciezka do pliku (koniecznie zakodowana w UTF-8)
******************************************/
long bmd_lob_request_info_add_filename(	const char *filename,
							lob_request_info_t **lob_info,
							long send_or_recv,
							long size)
{
long length			= 0;
long index			= 0;
long fd			= 0;
long status			= 0;
struct stat file_info;

#ifdef WIN32
	wchar_t *wideFilename=NULL;
#endif


	PRINT_INFO("LIBBMDINF Adding filename to lob structure\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(filename==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(lob_info==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*lob_info)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}


	/* odczytaj rozmiar pliku,otworz deskryptor i ustaw rozmiar LOB'a */
	#ifdef WIN32
	if(send_or_recv==BMD_SEND_LOB)
	{
		UTF8StringToWindowsString((char*)filename, &wideFilename, NULL);
		fd=_wopen(wideFilename, O_RDONLY|_O_BINARY);
		free(wideFilename); wideFilename=NULL;
		if( fd == -1)
		{
			PRINT_PANIC("blad dysku\n");
			return BMD_ERR_DISK;
		}
	}
	#else
	long y=0;
	if(send_or_recv==BMD_SEND_LOB)
	{
		while (1)
		{
			y++;
			if( (fd=open(filename,O_RDONLY)) == -1)
			{
				perror("error ");
				PRINT_PANIC("blad dysku (nie można otworzyc %s)\n",filename);
// 				return BMD_ERR_DISK;
			}
			else
			{
				break;
			}
			sleep(1);
			if (y>=20)
			{
				perror("error ");
				PRINT_PANIC("blad dysku (nie można otworzyc %s)\n",filename);
				return BMD_ERR_DISK;
			}
		}
	}
	#endif
	if(send_or_recv==BMD_SEND_LOB)
	{
		status=fstat(fd,&file_info);
		close(fd);
		(*lob_info)->total_file_size=(*lob_info)->total_file_size+file_info.st_size;
	}
	length=(long)strlen(filename);
	PRINT_INFO("length == %li\n",length);
	if(send_or_recv==BMD_SEND_LOB)
	{
		if((*lob_info)->files_to_send==NULL)
		{
			(*lob_info)->files_to_send=(char **)malloc(1*sizeof(char*));
			if((*lob_info)->files_to_send==NULL)
				return BMD_ERR_MEMORY;
			(*lob_info)->files_to_send[0]=(char *)malloc(sizeof(char)*(length+2));
			if((*lob_info)->files_to_send[0]==NULL)
				return BMD_ERR_MEMORY;
			memset((*lob_info)->files_to_send[0],0,sizeof(char)*(length+1));
			memmove((*lob_info)->files_to_send[0],filename,length*sizeof(char));
			(*lob_info)->no_of_files_to_send++;
		}
		else
		{

			index=(*lob_info)->no_of_files_to_send;
			(*lob_info)->files_to_send=(char **)realloc((*lob_info)->files_to_send,(index+1)*sizeof(char *));
			if((*lob_info)->files_to_send==NULL)
				return BMD_ERR_MEMORY;
			(*lob_info)->files_to_send[index]=(char *)malloc(sizeof(char)*(length+2));
			if((*lob_info)->files_to_send[index]==NULL)
				return BMD_ERR_MEMORY;

			memset((*lob_info)->files_to_send[index],0,sizeof(char)*(length+1));
			memmove((*lob_info)->files_to_send[index],filename,length*sizeof(char));
			(*lob_info)->no_of_files_to_send++;
		}
	}
	else
	{
		if((*lob_info)->files_to_recv==NULL)
		{

			(*lob_info)->files_to_recv=(char**)malloc(1*sizeof(char*));
			if((*lob_info)->files_to_recv==NULL)
			{
				return BMD_ERR_MEMORY;
			}
			(*lob_info)->files_to_recv[0]=(char*)malloc(sizeof(char)*(length+2));
			if((*lob_info)->files_to_recv[0]==NULL)
			{
				return BMD_ERR_MEMORY;
			}
			memset((*lob_info)->files_to_recv[0],0,sizeof(char)*(length+1));
			memmove((*lob_info)->files_to_recv[0],filename,length*sizeof(char));

			if((*lob_info)->recv_files_sizes==NULL)
			{
				(*lob_info)->recv_files_sizes=(long *)malloc(1*sizeof(long));
				if((*lob_info)->recv_files_sizes==NULL)
					return BMD_ERR_MEMORY;
				(*lob_info)->recv_files_sizes[0]=size;
			}

			(*lob_info)->no_of_files_to_recv++;
		}
		else
		{
			index=(*lob_info)->no_of_files_to_recv;
			(*lob_info)->files_to_recv=(char**)realloc((*lob_info)->files_to_recv,(index+1)*sizeof(char*));
			if((*lob_info)->files_to_recv==NULL)
			{
				return BMD_ERR_MEMORY;
			}
			(*lob_info)->files_to_recv[index]=(char*)malloc(sizeof(char)*(length+2));
			if((*lob_info)->files_to_recv[index]==NULL)
			{
				return BMD_ERR_MEMORY;
			}

			memset((*lob_info)->files_to_recv[index],0,sizeof(char)*(length+1));
			memmove((*lob_info)->files_to_recv[index],filename,length*sizeof(char));

			(*lob_info)->recv_files_sizes=(long *)realloc((*lob_info)->recv_files_sizes,(index+1)*sizeof(long *));
			if((*lob_info)->recv_files_sizes==NULL)
			{
				return BMD_ERR_MEMORY;
			}
			(*lob_info)->recv_files_sizes[index]=size;
			(*lob_info)->no_of_files_to_recv++;
		}
	}

	return BMD_OK;
}

/**
* @author WSz
*
* Funkcja bmd_lob_request_info_add_buffer() przepina zawartosc buffer do nowego elementu tablicy buforow do wyslania w lob_info.
* W przypadku sukcesu operacji zawartosc buffer bedzie wyzerowana (buf=NULL, size=0).
* Przy podpinaniu bufora konieczne jest ustawianie nazwy pliku odpowiadajacemu buforowi tj. wywolanie niniejszej
* funkcji musi isc w parze z wywolaniem funkcji bmd_lob_request_info_add_filename().
*
* @param lob_info to wskaznik do struktury przechowujacej informacje potrzebne do wysylki lobow
* @param buffer to bufor, ktorego zawartosc zostanie podpieta do tablicy buforow.
*
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, lub wartosc ujemna (kod bledu).
*
*/
long bmd_lob_request_info_add_buffer(lob_request_info_t *lob_info, GenBuf_t* buffer)
{
GenBuf_t* tempBuffer = NULL;

	if(lob_info == NULL)
		{ return BMD_ERR_PARAM1; }
	if(buffer == NULL)
		{ return BMD_ERR_PARAM2; }
	if(buffer->size <= 0 || buffer->buf == NULL)
		{ return BMD_ERR_PARAM2; }

	if(lob_info->no_of_buffers_to_send == 0)
	{
		lob_info->buffers_to_send=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		if(lob_info->buffers_to_send == NULL)
			{ return BMD_ERR_MEMORY; }
		lob_info->no_of_buffers_to_send++;

		lob_info->buffers_to_send[0].buf=buffer->buf;
		buffer->buf=NULL;

		lob_info->buffers_to_send[0].size=buffer->size;
		buffer->size=0;
	}
	else
	{
		tempBuffer=(GenBuf_t*)realloc(lob_info->buffers_to_send, (lob_info->no_of_buffers_to_send+1)*sizeof(GenBuf_t));
		if(tempBuffer == NULL)
		{
			return BMD_ERR_MEMORY;
		}
		lob_info->buffers_to_send=tempBuffer;
		tempBuffer=NULL;
		
		lob_info->buffers_to_send[lob_info->no_of_buffers_to_send].buf=buffer->buf;
		buffer->buf=NULL;

		lob_info->buffers_to_send[lob_info->no_of_buffers_to_send].size=buffer->size;
		buffer->size=0;

		lob_info->no_of_buffers_to_send++;
	}

	return BMD_OK;
}


#if 0
/* PLefunkcje puste - nie wykorzystywane */
/* ustawia flage czy pokazywac progress czy nie */
long bmd_lob_request_info_set_progress_flag(long send_or_recv)
{
    return BMD_OK;
}

/* ustawia dana do progresu - np. wxProgressDialog */
long bmd_lob_request_info_set_progress_data(void *send_progress_data,void *recv_progress_data)
{
    return BMD_OK;
}
#endif
