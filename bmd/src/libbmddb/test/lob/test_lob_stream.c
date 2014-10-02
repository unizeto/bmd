#include "test_lob.h"
#define LOB_FILENAME_IN "/tmp/aaa"
#define LOB_FILENAME_OUT "/tmp/fakt.pdf"

extern long _GLOBAL_debug_level;

/************************/
/*	MAIN		*/
/************************/

int main(int argc, char *argv[])
{
int err = 0;
char *hostaddr = "127.0.0.1";
char *port = "1521";
char *dbname = "bmd";
char *user = "bmd";
char *password = "12345678";
void *hDB = NULL;
GenBuf_t *gbuf_in = NULL;
GenBuf_t *gbuf_out = NULL;
GenBuf_t *gbuf_one_piece = NULL;
long int lobjId= 0;
int fd_in = 0;
int fd_out = 0;
int readed = 0;
int written = 0;
_GLOBAL_debug_level = 2;
void *blob_locator = NULL;
unsigned long int blob_size = 0;
int lob_piece = 0;
int lob_size = 0;
struct stat filestat;
unsigned int lob_chunk_size = 0;
int sum = 0;

	/***********************************************/
	/* GenBuf o rozmiarze LARGE_FILE_DB_CHUNK_SIZE */
	/***********************************************/
	gbuf_in = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(gbuf_in == NULL) return NO_MEMORY;
	gbuf_in->buf = (unsigned char *) malloc (sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE);
	if(gbuf_in->buf == NULL) return NO_MEMORY;
	memset(gbuf_in->buf, 0, sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE);
	gbuf_in->size = sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE;
	lob_chunk_size = sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE;

	err = bmd_db_connect2(hostaddr, port, dbname, user, password, &hDB);
	if(err<0) {
		PRINT_DEBUG("[%s:%i:%s] Error in connecting to database. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = bmd_db_start_transaction(hDB);
	if(err<0) {
		PRINT_DEBUG("[%s:%i:%s] Error in begining database transaction. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	/**************************************/
	/* Zapis strumieniowy z pliku do bazy */
	/**************************************/
	fd_in = open(LOB_FILENAME_IN, O_RDONLY, 0666);
	if(fd_in==-1){
		PRINT_DEBUG("[%s:%i:%s] Error in opening input LOB file %s. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
		return fd_in;
	}
	err = stat(LOB_FILENAME_IN, &filestat);
	if(err == -1){
		PRINT_DEBUG("[%s:%i:%s] Error in stating file %s. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
		return err;
	}
	lob_size = filestat.st_size;

	/* Jesli rozmiar LOB jest mniejszy niz rozmiar bufora to nalezy go zapisac jako ONE_PIECE */
	if(lob_size <= lob_chunk_size)
	{
		PRINT_DEBUG("INFO [%s:%i:%s] Writing LOB in ONE_PIECE mode\n",
			__FILE__, __LINE__, __FUNCTION__);
		err = read_GenBuf_from_file(LOB_FILENAME_IN, &gbuf_one_piece);
		if(err<0) {
			PRINT_DEBUG("[%s:%i:%s] Error in reading file %s to gbuf_one_piece. "
				"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
			return err;
		}
		err = exportFileIntoLO(hDB, gbuf_one_piece, &lobjId, WITHOUT_TRANSACTION);
		if(err<0) {
			PRINT_DEBUG("[%s:%i:%s] Error in exporting file %s to database. "
				"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
			return err;
		}
		free_gen_buf(&gbuf_one_piece);
	}
	else
	{
		PRINT_DEBUG("INFO [%s:%i:%s] Writing LOB in STREAMING_POOLING mode\n",
				__FILE__, __LINE__, __FUNCTION__);
		err = bmd_db_export_blob_begin(hDB, &lobjId, &blob_locator, WITHOUT_TRANSACTION);
		if(err < 0){
			PRINT_DEBUG("[%s:%i:%s] Error in begin export LOB file %s into database. "
				"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
			return fd_out;
		}
		do {
			/********************************/
			/** TU CZYTAMY ZE ŹRÓDŁA DANYCH */
			readed=read(fd_in, gbuf_in->buf, gbuf_in->size);
			if (readed == -1){
				PRINT_DEBUG("[%s:%i:%s] Error in reading LOB file %s. "
					"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
				return readed;
			}
			gbuf_in->size = readed;
			printf("Readed %i BYTES\n", readed);
			/** KONIEC ODCZYTU ZE ŹRÓDŁA DANYCH */
			/************************************/
			if(!lob_piece){
				err = bmd_db_export_blob_continue(hDB, gbuf_in, blob_locator, LOB_FIRST_PIECE); lob_piece = 1;
				if(err < 0){
					PRINT_DEBUG("[%s:%i:%s] Error in iterate export LOB file %s into database - FIRST_PIECE. "
						"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
					return fd_out;
				}
				lob_piece = 1;
			}
			else {
				if(lob_size <= lob_chunk_size){
					err = bmd_db_export_blob_continue(hDB, gbuf_in, blob_locator, LOB_LAST_PIECE);
					if(err < 0){
						PRINT_DEBUG("[%s:%i:%s] Error in iterate export LOB file %s into database - LAST_PIECE. "
							"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
						return fd_out;
					}
					break;
				}
				else{
					err = bmd_db_export_blob_continue(hDB, gbuf_in, blob_locator, LOB_NEXT_PIECE);
					if(err < 0){
						PRINT_DEBUG("[%s:%i:%s] Error in iterate export LOB file %s into database - NEXT_PIECE. "
							"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
						return fd_out;
					}
				}
			}
			printf("Exported chunk %i BYTES into DB\n", readed);
			lob_size -= readed;
		} while(readed > 0);
		close(fd_in);
		err = bmd_db_export_blob_end(hDB, &blob_locator, WITHOUT_TRANSACTION);
		if(err < 0){
			PRINT_DEBUG("[%s:%i:%s] Error in end export LOB file %s into database. "
				"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
			return fd_out;
		}
	} /* Koniec przetwarzania strumieniowego */
	PRINT_DEBUG("[%s:%i:%s] LOB succesfully exported to database. LOB OID = %li. "
		"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, lobjId, err);

	/***************************************/
	/* Odczyt strumieniowy z bazy do pliku */
	/***************************************/
	fd_out = open(LOB_FILENAME_OUT, O_WRONLY | O_CREAT, 0666);
	if(fd_out==-1){
		PRINT_DEBUG("[%s:%i:%s] Error in opening output LOB file %s. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
		return fd_out;
	}
	err = bmd_db_import_blob_begin(hDB, /*lobjId*/513, &blob_size, &blob_locator,
				&gbuf_out, WITHOUT_TRANSACTION);
	if(err < 0){
		PRINT_DEBUG("[%s:%i:%s] Error in begin import LOB file %s into database. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
		return fd_out;
	}
	printf("blob_size = %li\n", blob_size);
	lob_piece = 0;
	sum = 0;
	do {
		if(!lob_piece){
			err = bmd_db_import_blob_continue(hDB, gbuf_out, blob_locator, LOB_FIRST_PIECE);
			if(err < 0){
				PRINT_DEBUG("[%s:%i:%s] Error in iterate import LOB file %s from database. "
					"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__,
					LOB_FILENAME_IN, err);
				return fd_out;
			}
			lob_piece = 1;
		}
		else {
			err = bmd_db_import_blob_continue(hDB, gbuf_out, blob_locator, LOB_NEXT_PIECE);
			if(err < 0){
				PRINT_DEBUG("[%s:%i:%s] Error in iterate import LOB file %s from database. "
					"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
				return fd_out;
			}
		}
		if(err == 0) break;
		printf("Imported chunk %i BYTES from DB\n", err);
		written=write(fd_out, gbuf_out->buf, err);
		if (written == -1){
			PRINT_DEBUG("[%s:%i:%s] Error in writing LOB file %s. "
				"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
			return written;
		}
		printf("written %i BYTES info %s file\n", written, LOB_FILENAME_OUT);
		sum += err;
	} while(blob_size > sum && err > 0);
	close(fd_in);
	err = bmd_db_import_blob_end(hDB, &blob_locator, &gbuf_out, WITHOUT_TRANSACTION);
	if(err < 0){
		PRINT_DEBUG("[%s:%i:%s] Error in end export LOB file %s into database. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, LOB_FILENAME_IN, err);
		return fd_out;
	}
	PRINT_DEBUG("[%s:%i:%s] LOB succesfully exported to database. LOB OID = %li. "
		"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, lobjId, err);
	close(fd_out);

	/************/
	/* Koncowka */
	/************/
	err = bmd_db_end_transaction(hDB);
	if(err<0) {
		PRINT_DEBUG("[%s:%i:%s] Error in ending database transaction. "
			"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = bmd_db_disconnect(&hDB);
	if(err<0) {
		PRINT_DEBUG("[%s:%i:%s] Error in closing database connection. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	free_gen_buf(&gbuf_in);
	free_gen_buf(&gbuf_out);
	return err;
}


int read_GenBuf_from_file(char *filename, GenBuf_t **gbuf)
{
int err=0, fd=0, readed=0;
struct stat filestat;
	*gbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*gbuf==NULL) return -1;

	err = stat(filename, &filestat);
	if (err) return -2;

	fd = open(filename, O_RDONLY, 0666);
	if(fd==-1) return -2;

	(*gbuf)->buf = (unsigned char *) malloc (sizeof(unsigned char) * filestat.st_size + 2);
	memset((*gbuf)->buf, 0 , filestat.st_size + 1);
	if((*gbuf)->buf==NULL) return -1;
	(*gbuf)->size=filestat.st_size;

	if ( (readed=read(fd,(*gbuf)->buf,(*gbuf)->size)) == -1 ){
		return -3;
	}
	close(fd);
	return err;
}

