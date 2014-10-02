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
#include <bmd/common/bmd_definitions.h>
#include <sys/types.h>
#include <bmd/libbmderr/libbmderr.h>

#ifndef WIN32
	#include <dirent.h>
#endif //#ifndef WIN32

#ifdef WIN32
	#include <sys/locking.h>  ///blokowanie pliku pod Windows
	#pragma warning(disable:4055)
/*
#define DEBUG_LOB_LOG(fmt,...) {\
								FILE *file_p; \
								time_t t; \
								char sec_ctime[256];\
								struct _timeb tb;\
								char *klient_lob_log=NULL; \
								memset(sec_ctime,0,256); \
								asprintf(&klient_lob_log,"c:\\klient_log_%li.txt",GetCurrentThreadId());\
								file_p=fopen(klient_lob_log,"a+");\
								_ftime(&tb); \
								t=time(NULL); \
								ctime_s(sec_ctime,256,&t); \
								sec_ctime[strlen(sec_ctime)-1]='\0'; \
								fprintf(file_p,"[%li.%li %s %li][%s:%i:%s] : ", \
											   (long)tb.time,(long)tb.millitm,\
											   sec_ctime, \
											   (long)GetCurrentThreadId(), \
											   __FILE__,__LINE__,__FUNCTION__); \
								fprintf(file_p,fmt,__VA_ARGS__);\
								fclose(file_p);\
								}
*/
#endif

long __bmd_send_lobs(	lob_request_info_t *info,
				bmdnet_handler_ptr session_handle)
{
long i					= 0;
long status				= 0;
long fd					= 0;
long nread				= 0;
struct lob_transport *lob		= NULL;
u_int64_t filesize			= 0;
struct stat file_info;
GenBuf_t *portion			= NULL;
long count				= 0;
long bufferOffset		= 0;

#ifdef WIN32
wchar_t *wideFilename=NULL;
#endif

	if(info == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(info->no_of_buffers_to_send > 0 && info->no_of_buffers_to_send != info->no_of_files_to_send)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	PRINT_INFO("LIBBMDINF Sending Lobs\n");
	for(i=0;i<info->no_of_files_to_send;i++)
	{
		status=new_lob_transport(&lob, &session_handle,
					 info->lob_contexts[i],	   // szyfrowanie
					NULL,			   // brak deszyfrowania
					NULL,			   // brak liczenia skrotu
					NULL,			   // brak znakowania czasem
					0);			   // bez specjalnych opcji
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}

		// odczytaj rozmiar pliku,otworz deskryptor i ustaw rozmiar LOB'a
		if(info->no_of_buffers_to_send == 0) // wysylka z plikow
		{
#ifdef WIN32
			UTF8StringToWindowsString(info->files_to_send[i], &wideFilename, NULL);
			fd=_wopen(wideFilename, O_RDONLY|_O_BINARY);
			free(wideFilename); wideFilename=NULL;
			if( fd == -1)
			{
				PRINT_ERROR("LIBBMDERR Disk error. Error=%i\n",BMD_ERR_DISK);
				PRINT_ERROR("LIBBMDERR Disk error. Error opening file %s\n",info->files_to_send[i]);
				return BMD_ERR_DISK;
			}
#else
			PRINT_INFO("filename_name %s\n",info->files_to_send[i]);
			if( (fd=open(info->files_to_send[i],O_RDONLY)) == -1)
			{
				PRINT_ERROR("LIBBMDERR Disk error. Error=%i\n",BMD_ERR_DISK);
				return BMD_ERR_DISK;
			}
#endif

			status=fstat(fd,&file_info);
			if(status!=0)
			{
				PRINT_ERROR("LIBBMDERR Disk error. Error=%i\n",BMD_ERR_DISK);
				PRINT_ERROR("LIBBMDERR Disk error. Error getting fstat for file %s\n",info->files_to_send[i]);
				close(fd);
				return BMD_ERR_DISK;
			}

			filesize=file_info.st_size;
		}
		else // wysylka z pamieci
		{
			filesize = info->buffers_to_send[i].size;
		}

		if(info->no_of_buffers_to_send == 0) // wysylka z plikow
		{
			//by WSZ - sprawdzenie, czy plik nie zostal miedzyczasie podmieniony na pusty badz za duzy plik
			if(filesize <= 0)
			{
				status=__bmd_send_lobs_cancel(info, session_handle);
				close(fd);
				PRINT_ERROR("LIBBMDERR Send error. File size <= 0 %s\n",info->files_to_send[i]);
				return BMD_ERR_NODATA;
			}
			else if(filesize >= MAX_SIZE)
			{
				status=__bmd_send_lobs_cancel(info,session_handle);
				close(fd);
				PRINT_ERROR("LIBBMDERR Send error. File too big %s\n",info->files_to_send[i]);
				return BMD_ERR_TOO_BIG;
			}
#ifdef WIN32
			//blokowanie pliku, ktorego zawartosc ma byc odczytana i wyslana lobem
			_locking(fd,_LK_LOCK,(long)filesize);
#endif
		}
		else // wysylka z pamieci
		{
			if(filesize <= 0)
			{
				status=__bmd_send_lobs_cancel(info, session_handle);
				return BMD_ERR_NODATA;
			}
			else if(filesize >= MAX_SIZE)
			{
				status=__bmd_send_lobs_cancel(info, session_handle);
				return BMD_ERR_TOO_BIG;
			}
		}

		status=set_lob_size(&lob, filesize);
		if(status!=0)
		{
			PRINT_ERROR("LIBBMDERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			if(info->no_of_buffers_to_send == 0) // wysylka z plikow
			{
#ifdef WIN32
				_locking(fd, _LK_UNLCK, (long)filesize);
#endif
				close(fd);
			}
			return BMD_ERR_OP_FAILED;
		}

		portion=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
		if(portion==NULL)
		{
			PRINT_ERROR("LIBBMDERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			if(info->no_of_buffers_to_send == 0) //wysylka z plikow
			{
#ifdef WIN32
				_locking(fd, _LK_UNLCK, (long)filesize);
#endif
				close(fd);
			}
			return BMD_ERR_MEMORY;
		}
		
		info->send_prog_data.prog_text=(void *)info->files_to_send[i];
		info->send_prog_data.current_object_count++;

		if(info->no_of_buffers_to_send == 0) // wysylka z plikow
		{
			count = 0;
			portion->buf=(char*)calloc(LARGE_FILE_DB_CHUNK_SIZE+2, sizeof(char));
			// odczytuje kolejne czesci pliku i wysyla
			while((nread=read(fd,portion->buf,LARGE_FILE_DB_CHUNK_SIZE))>0)
			{
				portion->size=nread;
				count=count+nread; // ile w sumie bedzie wyslane
				if(count == filesize) // ostatnia porcja loba
				{
					status=send_lob_chunk( &lob, &portion,1);
				}
				else // NIEostatnia porcja loba
				{
					status=send_lob_chunk( &lob, &portion,0);
				}

				if (status<0)
				{
					free_gen_buf(&portion);
#ifdef WIN32
					_locking(fd, _LK_UNLCK, (long)filesize);
#endif
					close(fd);
					BMD_FOK(BMD_ERR_OP_FAILED);
				}

				if(info->show_send_progress==1)
				{
					info->send_prog_data.total_cur_size=info->send_prog_data.total_cur_size+nread;
					status=((progress_function)info->send_prog_data.func)(info->send_prog_data.prog_data,
										  (double)filesize,count,
										  info->send_prog_data.total_size,info->send_prog_data.total_cur_size,
										  info->send_prog_data.prog_text);
					if(status!=BMD_OK)
					{
						PRINT_ERROR("LIBBMDERR Error. Error=%i\n",BMD_ERR_INTERRUPTED);
						// tutaj bedzie wyslac loba z prefixem 000, zeby serwer umial wykryc anulowanie wysylki - RO
#ifdef WIN32
						_locking(fd, _LK_UNLCK, (long)filesize);
#endif
						close(fd);
						free_gen_buf(&portion);
						__bmd_send_lobs_cancel(info,session_handle);
						status=free_lob_transport( &lob );
						return BMD_ERR_INTERRUPTED;
					}
				}
				memset(portion->buf,0,LARGE_FILE_DB_CHUNK_SIZE);
			}
			if( nread < 0 )
			{
#ifdef WIN32
				_locking(fd, _LK_UNLCK, (long)filesize);
#endif
				close(fd);
				free_gen_buf(&portion);
				PRINT_DEBUG("Failed to read from descriptor of %s. Lob number %li\n",info->files_to_send[i],(long)i);
				BMD_FOK(BMD_ERR_DISK);
			}
		}
		else // wysylka z pamieci
		{
			for(count=0, bufferOffset = 0; count < info->buffers_to_send[i].size ; bufferOffset += LARGE_FILE_DB_CHUNK_SIZE)
			{
				// ile do odczytania w tej iteracji
				if(info->buffers_to_send[i].size - bufferOffset > LARGE_FILE_DB_CHUNK_SIZE)
					{ nread=LARGE_FILE_DB_CHUNK_SIZE; }
				else
					{ nread=info->buffers_to_send[i].size - bufferOffset; }
	
				portion->size = nread;
				portion->buf = info->buffers_to_send[i].buf + bufferOffset * sizeof(char);
				count += nread;

				if(count < info->buffers_to_send[i].size)
					{ status=send_lob_chunk( &lob, &portion, 0); }	
				else
					{ status=send_lob_chunk( &lob, &portion, 1); }

				portion->buf = NULL;
				portion->size = 0;

				if(status < 0)
				{
					free_gen_buf(&portion);
					BMD_FOK(BMD_ERR_OP_FAILED);
				}

				if(info->show_send_progress==1)
				{
					info->send_prog_data.total_cur_size=info->send_prog_data.total_cur_size+nread;
					status=((progress_function)info->send_prog_data.func)(info->send_prog_data.prog_data,
										  (double)filesize,count,
										  info->send_prog_data.total_size,info->send_prog_data.total_cur_size,
										  info->send_prog_data.prog_text);
					if(status!=BMD_OK)
					{
						PRINT_ERROR("LIBBMDERR Error. Error=%i\n",BMD_ERR_INTERRUPTED);
						// tutaj bedzie wyslac loba z prefixem 000, zeby serwer umial wykryc anulowanie wysylki - RO
						free_gen_buf(&portion);
						__bmd_send_lobs_cancel(info,session_handle);
						status=free_lob_transport( &lob );
						return BMD_ERR_INTERRUPTED;
					}
				}
			}
		}
		
		status=free_lob_transport( &lob );
		free_gen_buf(&portion);

		// zdjecie blokady i zamkniecie wyslanego pliku
		if(info->no_of_buffers_to_send == 0) // wysylka z plikow
		{
#ifdef WIN32
			_locking(fd, _LK_UNLCK, (long)filesize);
#endif
			close(fd);
		}
		if(status!=0)
		{
			PRINT_ERROR("LIBBMDERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}
	
	if(info->show_send_progress==1)
	{
		//ustawienie informacji w progressdialogu, zanim odebrana zostanie odpowiedz od serwera
		wchar_t *wyslano_text=L"Data sent ... waiting for server response";
		((progress_function)(info->send_prog_data.func_setTitle))(info->send_prog_data.prog_data, 0.0, 0.0, 0.0, 0.0, wyslano_text);
	}

	return BMD_OK;
}


#ifndef WIN32
long __bmd_send_lobs_in_parts(	lob_request_info_t *info,
					bmdnet_handler_ptr session_handle,
					long total_file_size,
					long lob_session_hash,
					long stream_timeout)
{
long j					= 0;
long status				= 0;
long fd				= 0;
long nread				= 0;
long file				= 0;
u_int64_t filesize		= 0;
long iter_num			= 0;
long current_send_size	= 0;
long is_last				= 0;
char *file_chunk_name		= NULL;
char *file_chunk_name_last	= NULL;
struct lob_transport *lob	= NULL;
GenBuf_t *portion			= NULL;
char *tmp_lock			= NULL;
char *file_to_send		= NULL;
long file_counter			= 1;
struct stat file_info;


	if (info->no_of_files_to_send<=0)	{	return BMD_OK;	}

	PRINT_INFO("LIBBMDINF Sending Lobs in parts\n");
	/************************************************/
	/*	inicjalizacja struktury do wysylki lobw	*/
	/************************************************/
	BMD_FOK(new_lob_transport(	&lob, &session_handle,
					 	info->lob_contexts[0],	/* szyfrowanie*/
						NULL,				/* brak deszyfrowania */
						NULL,				/* brak liczenia skrotu */
						NULL,				/* brak znakowania czasem */
						0));				/* bez specjalnych opcji */

	/************************************************/
	/*	ustawienie wielkosci konkretnego loba	*/
	/************************************************/
	BMD_FOK(set_lob_size(&lob, total_file_size));


	asprintf(&file_chunk_name_last, "%li%li_final", (long)getpid(),lob_session_hash);
	if(file_chunk_name_last == NULL)		{	BMD_FOK(NO_MEMORY);	}
	/******************************************************************************************/
	/*	przegladanie katalogu tymczasowego w celu odnajdowania kolejnych cześci wysylki	*/
	/******************************************************************************************/
	while(1)
	{
		asprintf(&file_chunk_name, "%li%li_%li", (long)getpid(), lob_session_hash, file_counter);
		if(file_chunk_name == NULL)		{	BMD_FOK(NO_MEMORY);	}
		/*************************************************************/
		/* sprawdzenie, czy spodziewany plik znajduje sie w katalogu */
		/*************************************************************/
		free(file_to_send); file_to_send=NULL;
		if ((file=open(file_chunk_name, O_RDONLY))<0)
		{
			if ((file=open(file_chunk_name_last, O_RDONLY))<0)
			{
				free(file_to_send); file_to_send=NULL;
			}
			else
			{
				close(file);
				asprintf(&file_to_send, "%s", file_chunk_name_last);
				if(file_to_send == NULL)	{	BMD_FOK(NO_MEMORY);	}
				is_last=1;
			}
		}
		else
		{
			close(file);
			asprintf(&file_to_send, "%s", file_chunk_name);
			if(file_to_send == NULL)		{	BMD_FOK(NO_MEMORY);	}
			is_last=0;
		}

		/************************************************************************/
		/*	jesli nie znaleziono pliku do wysylki, kontynuujemy poszukiwania	*/
		/************************************************************************/
		if (file_to_send==NULL)
		{
			usleep(100000);
			if ((iter_num++)>=stream_timeout*10)
			{
				PRINT_ERROR("LIBBMDERR Still no file to send\n");
				__bmd_send_lobs_cancel(info,session_handle);
				BMD_FOK(BMD_ERR_TIMEOUT);

				break;
			}
			continue;
		}

		/******************************************/
		/*	jesli znaleziono plik do wysylki	*/
		/******************************************/
		else
		{
			/******************************************************/
			/*	sprawdzamy czy nie ma na niego locka odczytu	*/
			/******************************************************/
			asprintf(&tmp_lock, "%s.lock", file_to_send);
			j=0;
			while(1)
			{
				if ((file=open(tmp_lock, O_RDONLY))<0)
				{
					break;
				}
				close(file);

				if (j>=stream_timeout)
				{
					PRINT_ERROR("LIBBMDERR lock file %s still exists.\n", tmp_lock);
					__bmd_send_lobs_cancel(info,session_handle);
					BMD_FOK(BMD_ERR_TIMEOUT);
				}
				j++;
				sleep(1);
			}

			/******************************************/
			/*	otwarcie pliku z kawalkiem wysylki	*/
			/******************************************/
			if( (fd=open(file_to_send,O_RDONLY)) == -1)
			{
				break;
			}

			/************************************/
			/*	odczytanie wielkosci pliku	*/
			/************************************/
			status=fstat(fd,&file_info);
			if(status!=0)
			{
				close(fd);
				break;
			}
			filesize=file_info.st_size;
			portion=(GenBuf_t *)malloc(sizeof(GenBuf_t));
			if(portion==NULL)
			{
				close(fd);
				BMD_FOK(NO_MEMORY);
			}
			memset(portion,0,sizeof(GenBuf_t));
			portion->buf=(char *)malloc(sizeof(char)*LARGE_FILE_DB_CHUNK_SIZE+2);
			if(portion->buf == NULL)			{	BMD_FOK(NO_MEMORY);	}

			/************************************************/
			/*	wysylanie danych odczytanych z pliku	*/
			/************************************************/
			current_send_size=0;
			while( (nread=read(fd,portion->buf,LARGE_FILE_DB_CHUNK_SIZE))>0 )
			{
				portion->size=nread;
				current_send_size+=nread;
				if (current_send_size>=filesize)
				{
					status=send_lob_chunk( &lob, &portion,is_last);
				}
				else
				{
					status=send_lob_chunk( &lob, &portion,0);
				}
				if (status<0)
				{
					close(fd);
					break;
				}
				memset(portion->buf,0,LARGE_FILE_DB_CHUNK_SIZE);
			}
			close(fd);
			if(status!=BMD_OK)
			{
				break;
			}

			/************************************/
			/*	zwiekszenie licznika plikow	*/
			/************************************/
			file_counter++;

			/************************************************/
			/*	porzadki i usuniecie wyslanego pliku	*/
			/************************************************/
			free_gen_buf(&portion);
			unlink(file_to_send);
			iter_num=0;
		}
		if (is_last==1)
		{
			break;
		}
	}
	free_lob_transport( &lob );

	if (is_last==0)
	{
		__bmd_send_lobs_cancel(info,session_handle);
		close(fd);
		BMD_FOK(BMDSOAP_SERVER_LOBS_NOT_FINISHED);
	}

	/******************/
	/*	porzadki	*/
	/******************/
	free(file_chunk_name); file_chunk_name=NULL;
	free(file_chunk_name_last); file_chunk_name_last=NULL;
	free(tmp_lock); tmp_lock=NULL;
	free(file_to_send); file_to_send=NULL;

	return BMD_OK;
}
#endif //#ifndef WIN32



/** Funkcja bmd_send_request_lo2 z ostatnim paremetrem NULL
funkcja dla wstecznej kompatybilnosci

*/
long bmd_send_request_lo(	bmd_info_t *bmd_info,
				bmdDatagramSet_t *request_datagram_set,
				long twf_max_datagram_in_set_transmission,
				bmdDatagramSet_t **response_datagram_set,
				lob_request_info_t *info,
				long deserialisation_max_memory)
{
	BMD_FOK(bmd_send_request_lo2(	bmd_info,
					request_datagram_set,
					twf_max_datagram_in_set_transmission,
					response_datagram_set,
					info,
					NULL,
					deserialisation_max_memory));

	return BMD_OK;
}

/** Funkcja bmd_send_request_lo z dodanym paremetrem funkcji odwierzajcej GUI podczas czekania na gniedzie
@param refreshcallback - funkcja odswierzajaca GUI wywoywana w
*/
long bmd_send_request_lo2(	bmd_info_t *bmd_info,
				bmdDatagramSet_t *request_datagram_set,
				long twf_max_datagram_in_set_transmission,
				bmdDatagramSet_t **response_datagram_set,
				lob_request_info_t *info,
				bmdnet_progress_callback_t refreshcallback,
				long deserialisation_max_memory)
{
long i						= 0;
long status					= 0;
long anuluj					= 0;
long bmdDatagramsTransmissionCount		= 0;
long bmdDatagramsAllCount			= 0;
long bmdDatagramsLeft                           = 0;
long iteration_count				= 0;
long twl_no_of_buffers				= 0;
long twl_memory_guard				= 0;
GenBuf_t **request_to_send			= NULL;
GenBuf_t *received_data				= NULL;
GenBuf_t *tmp_ki_mod_n_client			= NULL;
GenBuf_t *new_ki_mod_n				= NULL;
struct __bmdnet_handler_struct *session_handle	= NULL;
bmdDatagramSet_t *temp_dtg_set			= NULL;

	PRINT_INFO("LIBBMDINF Sending lo request to bmd\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if (bmd_info==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (request_datagram_set==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_max_datagram_in_set_transmission <= 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (response_datagram_set==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*response_datagram_set)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (info==NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (deserialisation_max_memory<0)		{	BMD_FOK(BMD_ERR_PARAM7);	}

	/******************************************************/
	/*	weryfikacja informacji o wysylanych lobach	*/
	/******************************************************/
	BMD_FOK(__bmd_verify_lob_input_data(request_datagram_set,info));

	/************************************************************/
	/*	przygotowanie zadania dotyczacego transmisji lobowej	*/
	/************************************************************/
	info->lob_contexts=(bmd_crypt_ctx_t **)malloc(sizeof(bmd_crypt_ctx_t *) *
	(request_datagram_set->bmdDatagramSetSize+2));
	for(i=0;i<(long)request_datagram_set->bmdDatagramSetSize+1;i++)
	{
		info->lob_contexts[i]=NULL;
	}

	for(i=0;i<(long)request_datagram_set->bmdDatagramSetSize;i++)
	{
		BMD_FOK(__bmd_prepare_request(bmd_info,request_datagram_set->bmdDatagramSetTable[i],
		&(info->lob_contexts[i])));
	}


	iteration_count = request_datagram_set->bmdDatagramSetSize / twf_max_datagram_in_set_transmission;
	if( (request_datagram_set->bmdDatagramSetSize % twf_max_datagram_in_set_transmission) != 0)
	{
		iteration_count++;
	}
	PRINT_VDEBUG("LIBBMDVDEBUG request_datagram_set->bmdDatagramSetSize: %lu\n", request_datagram_set->bmdDatagramSetSize);
	PRINT_VDEBUG("LIBBMDVDEBUG iteration_count: %lu\n", iteration_count);

	/******************************************/
	/*	wyslanie zadania datagramowego	*/
	/******************************************/
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
							0));

		BMD_FOK(__bmd_request_send( &session_handle, request_to_send, twl_no_of_buffers, bmd_info, !i));
		/*free_gen_buf(&request_to_send);*/
		free(request_to_send); request_to_send = NULL;
	}

	if ( (refreshcallback) && (session_handle) )
	{
		session_handle->pcallback = *refreshcallback;
	}

	/************************************/
	/*	wysylka lobow do serwera	*/
	/************************************/
	status=__bmd_send_lobs(info,session_handle);
	if (status!=BMD_OK)
	{

		if (status==BMD_ERR_INTERRUPTED)
		{

			anuluj=BMD_ERR_INTERRUPTED;
		}
		else
		{

			if(status==BMD_ERR_NODATA || status==BMD_ERR_TOO_BIG)
			{
				anuluj=status;
			}
			else
			{
				return status;
			}
		}

	}

	BMD_FOK(__bmd_response_recv(&session_handle,bmd_info,&received_data));
	PRINT_VDEBUG("LIBBMDVDEBUG received_data size: %li\n", received_data->size);

	BMD_FOK(__bmd_decode_response_buffer(	&received_data,
						bmd_info,
						tmp_ki_mod_n_client,
						&temp_dtg_set,
						&new_ki_mod_n,
						&bmdDatagramsTransmissionCount,
						&bmdDatagramsAllCount,
						&twl_memory_guard,
						deserialisation_max_memory));
	bmd_info->server_ki_mod_n = new_ki_mod_n;
	new_ki_mod_n = NULL;

	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramsTransmissionCount: %lu, bmdDatagramsAllCount %lu\n",
	bmdDatagramsTransmissionCount, bmdDatagramsAllCount);
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
		for(i = 0; i < temp_dtg_set->bmdDatagramSetSize; i++)
		{
			BMD_FOK(PR2_bmdDatagramSet_add(temp_dtg_set->bmdDatagramSetTable[i], response_datagram_set));
		}

		free(temp_dtg_set);
		temp_dtg_set = NULL;
	}

	free_gen_buf(&tmp_ki_mod_n_client);
	/*free_gen_buf(&request_to_send);*/
	free_gen_buf(&received_data);
	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramSetSize after: %lu\n", (*response_datagram_set)->bmdDatagramSetSize);
	/************************************/
	/*	odbieranie lobow z serwera	*/
	/************************************/
	status=__bmd_prepare_response(bmd_info,response_datagram_set,info,session_handle);
	if (status<BMD_OK)
	{
		if (anuluj)
		{
			bmdnet_close(&session_handle);
			return anuluj;
		}
		BMD_FOK(status);
	}

	bmdnet_close(&session_handle);
	if (anuluj)
	{
		return anuluj;
	}

	return BMD_OK;
}


/**
*funkcja bmd_send_request_lo2_to_buf() pozwala na pobranie LOB'a do bufora
*@param lobBuffer, to wskaznik na alokowany wewnatrz funkcji buffor z pobranym lobem
*@param maxLobSize to maksymalny rozmiar pobieranego loba (dla wartosci <= 0 bufor domyslnie przyjmuje ograniczenie 1024*1024 bajtow)
*@param request_datagram_set moze miec ustawiony tylko jeden datagram (mozliwe jest pobranie tylko jednego LOB'a)
*pozostale parametry maja znaczenie jak w przypadku funkcji bmd_send_request_lo2()
*/
long bmd_send_request_lo2_to_buf(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *info,
					bmdnet_progress_callback_t refreshcallback,
					GenBuf_t **lobBuffer,
					long maxLobSize,
					long deserialisation_max_memory)
{
long i						= 0;
long status					= 0;
long anuluj					= 0;
long bmdDatagramsTransmissionCount		= 0;
long bmdDatagramsAllCount			= 0;
long bmdDatagramsLeft                           = 0;
long iteration_count				= 0;
long twl_no_of_buffers				= 0;
long twl_memory_guard				= 0;
GenBuf_t **request_to_send			= NULL;
GenBuf_t *received_data				= NULL;
GenBuf_t *tmp_ki_mod_n_client			= NULL;
GenBuf_t *new_ki_mod_n				= NULL;
struct __bmdnet_handler_struct *session_handle	= NULL;
bmdDatagramSet_t *temp_dtg_set			= NULL;

	PRINT_INFO("LIBBMDINF Sending lo request to bmd\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if (bmd_info==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (request_datagram_set==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (request_datagram_set->bmdDatagramSetSize != 1) 	{ 	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_max_datagram_in_set_transmission <= 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (response_datagram_set==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*response_datagram_set)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (info==NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (lobBuffer == NULL)					{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (deserialisation_max_memory<0)			{	BMD_FOK(BMD_ERR_PARAM9);	}

	/******************************************************/
	/*	weryfikacja informacji o wysylanych lobach	*/
	/******************************************************/
	BMD_FOK(__bmd_verify_lob_input_data(request_datagram_set,info));

	/************************************************************/
	/*	przygotowanie zadania dotyczacego transmisji lobowej	*/
	/************************************************************/
	info->lob_contexts=(bmd_crypt_ctx_t **)malloc(sizeof(bmd_crypt_ctx_t *) *
	(request_datagram_set->bmdDatagramSetSize+2));
	for(i=0;i<(long)request_datagram_set->bmdDatagramSetSize+1;i++)
	{
		info->lob_contexts[i]=NULL;
	}

	for(i=0;i<(long)request_datagram_set->bmdDatagramSetSize;i++)
	{
		BMD_FOK(__bmd_prepare_request(bmd_info,request_datagram_set->bmdDatagramSetTable[i],
		&(info->lob_contexts[i])));
	}


	iteration_count = request_datagram_set->bmdDatagramSetSize / twf_max_datagram_in_set_transmission;
	if( (request_datagram_set->bmdDatagramSetSize % twf_max_datagram_in_set_transmission) != 0)
	{
		iteration_count++;
	}
	PRINT_VDEBUG("LIBBMDVDEBUG request_datagram_set->bmdDatagramSetSize: %lu\n", request_datagram_set->bmdDatagramSetSize);
	PRINT_VDEBUG("LIBBMDVDEBUG iteration_count: %lu\n", iteration_count);

	/******************************************/
	/*	wyslanie zadania datagramowego	*/
	/******************************************/
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
							0));

		BMD_FOK(__bmd_request_send( &session_handle, request_to_send, twl_no_of_buffers, bmd_info, !i));
		/*free_gen_buf(&request_to_send);*/
		free(request_to_send); request_to_send = NULL;
	}

	if ( (refreshcallback) && (session_handle) )
	{
		session_handle->pcallback = *refreshcallback;
	}

	/************************************/
	/*	wysylka lobow do serwera	*/
	/************************************/
	status=__bmd_send_lobs(info,session_handle);
	if (status!=BMD_OK)
	{

		if (status==BMD_ERR_INTERRUPTED)
		{

			anuluj=BMD_ERR_INTERRUPTED;
		}
		else
		{

			if(status==BMD_ERR_NODATA || status==BMD_ERR_TOO_BIG)
			{
				anuluj=status;
			}
			else
			{
				return status;
			}
		}

	}

	BMD_FOK(__bmd_response_recv(&session_handle,bmd_info,&received_data));
	PRINT_VDEBUG("LIBBMDVDEBUG received_data size: %li\n", received_data->size);

	BMD_FOK(__bmd_decode_response_buffer(	&received_data,
						bmd_info,
						tmp_ki_mod_n_client,
						&temp_dtg_set,
						&new_ki_mod_n,
						&bmdDatagramsTransmissionCount,
						&bmdDatagramsAllCount,
						&twl_memory_guard,
						deserialisation_max_memory));
	bmd_info->server_ki_mod_n = new_ki_mod_n;
	new_ki_mod_n = NULL;

	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramsTransmissionCount: %lu, bmdDatagramsAllCount %lu\n",
	bmdDatagramsTransmissionCount, bmdDatagramsAllCount);
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
		for(i = 0; i < temp_dtg_set->bmdDatagramSetSize; i++)
		{
			BMD_FOK(PR2_bmdDatagramSet_add(temp_dtg_set->bmdDatagramSetTable[i], response_datagram_set));
		}

		free(temp_dtg_set);
		temp_dtg_set = NULL;
	}

	free_gen_buf(&tmp_ki_mod_n_client);
	/*free_gen_buf(&request_to_send);*/
	free_gen_buf(&received_data);
	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramSetSize after: %lu\n", (*response_datagram_set)->bmdDatagramSetSize);
	/************************************/
	/*	odbieranie lobow z serwera	*/
	/************************************/
	status=__bmd_prepare_response_to_buf(bmd_info,response_datagram_set,info,session_handle, lobBuffer, maxLobSize);
	if (status<BMD_OK)
	{
		if (anuluj)
		{
			bmdnet_close(&session_handle);
			return anuluj;
		}
		BMD_FOK(status);
	}

	bmdnet_close(&session_handle);
	if (anuluj)
	{
		return anuluj;
	}

	return BMD_OK;
}



long __bmd_send_lobs_cancel(	lob_request_info_t *info,
				bmdnet_handler_ptr session_handle)
{
struct lob_transport *lob	= NULL;

	PRINT_INFO("LIBBMDINF Sending Lobs Cancel\n");
	/* wyslanie loba z prefiksem 0,0,0 oznaczajacego dla serwera, ze anulowano wysylke lobow */

	BMD_FOK(new_lob_transport(	&lob, &session_handle,
				 	info->lob_contexts[0],	   /* szyfrowanie*/
					NULL,			   /* brak deszyfrowania */
					NULL,			   /* brak liczenia skrotu */
					NULL,			   /* brak znakowania czasem */
					0));			   /* bez specjalnych opcji */
	BMD_FOK(set_lob_size(&lob, 0));	//lob o rozmiarze 0
	BMD_FOK(send_lob_cancel( &lob));

	/* zwolnij lob_transport */
	BMD_FOK(free_lob_transport( &lob ));

	return BMD_OK;
}

#ifndef WIN32
long bmd_send_request_lo_in_parts(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *info,
					bmdnet_progress_callback_t refreshcallback,
					long total_file_size,
					long lob_session_hash,
					long stream_timeout,
					long deserialisation_max_memory)
{
long i						= 0;
long status					= 0;
long anuluj					= 0;
long bmdDatagramsTransmissionCount		= 0;
long bmdDatagramsAllCount			= 0;
long bmdDatagramsLeft				= 0;
long iteration_count				= 0;
long twl_no_of_buffers				= 0;
long twl_memory_guard				= 0;
GenBuf_t **request_to_send			= NULL;
GenBuf_t *received_data				= NULL;
GenBuf_t *tmp_ki_mod_n_client			= NULL;
GenBuf_t *new_ki_mod_n				= NULL;
bmdnet_handler_ptr session_handle		= NULL;
bmdDatagramSet_t *temp_dtg_set			= NULL;

	PRINT_INFO("LIBBMDINF Sending lo in parts request to bmd\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (bmd_info==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (request_datagram_set==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_max_datagram_in_set_transmission <= 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (response_datagram_set==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*response_datagram_set)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (info==NULL)					{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (deserialisation_max_memory<0)		{	BMD_FOK(BMD_ERR_PARAM10);	}

	iteration_count = request_datagram_set->bmdDatagramSetSize / twf_max_datagram_in_set_transmission;
	if( (request_datagram_set->bmdDatagramSetSize % twf_max_datagram_in_set_transmission) != 0)
	{
		iteration_count++;
	}
	PRINT_VDEBUG("LIBBMDVDEBUG request_datagram_set->bmdDatagramSetSize: %lu\n", \
	request_datagram_set->bmdDatagramSetSize);
	PRINT_VDEBUG("LIBBMDVDEBUG iteration_count: %lu\n", iteration_count);


	/******************************************/
	/*	weryfikacja danych wejsciowych	*/
	/******************************************/
	BMD_FOK(__bmd_verify_lob_input_data(request_datagram_set,info));

	/************************************************************/
	/*	w tym wypadku mozliwe jest tylko wyslanie 1 datagramu	*/
	/************************************************************/
	info->lob_contexts=(bmd_crypt_ctx_t **)malloc(sizeof(bmd_crypt_ctx_t *)*3);
	info->lob_contexts[0]=NULL;

	/************************************************/
	/*	przygotowanie requesta i wyslanie go	*/
	/************************************************/
	BMD_FOK(__bmd_prepare_request(bmd_info,request_datagram_set->bmdDatagramSetTable[0],&(info->lob_contexts[0])));
	BMD_FOK(bmd_compute_ki_mod_n(bmd_info->cur_ki_mod_n, bmd_info->i, bmd_info->n, &tmp_ki_mod_n_client));

	for(i = 0; i < iteration_count; i++)
	{
		BMD_FOK(__bmd_prepare_request_buffer(	request_datagram_set,
							bmd_info,
							0,
							request_datagram_set->bmdDatagramSetSize,
							&request_to_send,
							&twl_no_of_buffers,
							tmp_ki_mod_n_client,
							0));
// 							twf_serialisation_type,
// 							twf_no_of_serialisation_threads));

		BMD_FOK(__bmd_request_send(&session_handle, request_to_send, twl_no_of_buffers, bmd_info,!i));
		/*free_gen_buf(&request_to_send);*/
		free(request_to_send); request_to_send = NULL;
	}

	if ( (refreshcallback) && (session_handle) )
	{
		session_handle->pcallback = *refreshcallback;
	}

	/******************************************************************/
	/*	wysylanie lobow zgodnie z tym, co pojawi sie w katalogu	*/
	/******************************************************************/
	status=__bmd_send_lobs_in_parts(info,session_handle,total_file_size,lob_session_hash,stream_timeout);
	if (status!=BMD_OK)
	{
		if (status==BMD_ERR_INTERRUPTED)
		{
			anuluj=BMD_ERR_INTERRUPTED;
		}
		else
		{
			if(status==BMD_ERR_NODATA || status==BMD_ERR_TOO_BIG)
			{
				anuluj=status;
			}
			else
			{
				BMD_FOK(status);
			}
		}
	}

	/************************************************/
	/*	odebranie odpowiedzi i zdekodowanie jej	*/
	/************************************************/
	BMD_FOK(__bmd_response_recv(&session_handle,bmd_info,&received_data));
	PRINT_VDEBUG("LIBBMDVDEBUG received_data size: %li\n", received_data->size);

	BMD_FOK(__bmd_decode_response_buffer(	&received_data,
							bmd_info,
							tmp_ki_mod_n_client,
							&temp_dtg_set,
							&new_ki_mod_n,
							&bmdDatagramsTransmissionCount,
							&bmdDatagramsAllCount,
							&twl_memory_guard,
							deserialisation_max_memory));
	bmd_info->server_ki_mod_n = new_ki_mod_n;
	new_ki_mod_n = NULL;

	PRINT_VDEBUG("LIBBMDVDEBUG bmdDatagramsTransmissionCount: %lu, bmdDatagramsAllCount %lu\n",
	bmdDatagramsTransmissionCount, bmdDatagramsAllCount);
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
		for(i = 0; i < temp_dtg_set->bmdDatagramSetSize; i++)
		{
			BMD_FOK(PR2_bmdDatagramSet_add(temp_dtg_set->bmdDatagramSetTable[i], response_datagram_set));
		}

		free(temp_dtg_set);
		temp_dtg_set = NULL;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	free_gen_buf(&tmp_ki_mod_n_client);
	/*free_gen_buf(&request_to_send);*/
	free_gen_buf(&received_data);

	/************************************/
	/*	odbieranie lobow z serwera	*/
	/************************************/
	status=__bmd_prepare_response_in_parts(bmd_info,response_datagram_set,info,session_handle,lob_session_hash,
	stream_timeout);
	if (status<BMD_OK)
	{
		if (anuluj)
		{
			bmdnet_close(&session_handle);
			return anuluj; /*Nie wiem jaki byl zamysl. Na wszelki wypadek nie wprowadazm BMD_FOK*/
		}
		BMD_FOK(status);
	}

	bmdnet_close(&session_handle);

	if (anuluj)
	{
		return anuluj; /*Nie wiem jaki byl zamysl. Na wszelki wypadek nie wprowadazm BMD_FOK*/
	}

	return BMD_OK;
}
#endif //#ifndef WIN32
