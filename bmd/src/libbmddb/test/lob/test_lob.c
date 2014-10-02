
#include "test_lob.h"
#define LOB_FILENAME_IN "/tmp/aaa"
#define LOB_FILENAME_OUT "/tmp/aaa.1"
int read_GenBuf_from_file(char *filename, GenBuf_t **gbuf);
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
long int lobjId= 0;
int fd = 0;
_GLOBAL_debug_level = 2;

	err = read_GenBuf_from_file(LOB_FILENAME_IN, &gbuf_in);
	if(err<0) {
		PRINT_DEBUG("[%s:%i:%s] Error in reading file to GenBuf. "
			"RecievedErrorCode=%i.\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
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
	err =  exportFileIntoLO(hDB, gbuf_in, &lobjId, WITHOUT_TRANSACTION);
	if(err < 0) {
		PRINT_DEBUG("[%s:%i:%s] Error in exporting LOB to database. "
			"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	PRINT_DEBUG("[%s:%i:%s] LOB succesfully exported to database. LOB OID = %li. "
		"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, lobjId, err);


	/* Sprawdzenie */
	err = importFileFromLO(hDB, lobjId, &gbuf_out, WITHOUT_TRANSACTION);
	if(err < 0) {
		PRINT_DEBUG("[%s:%i:%s] Error in importing LOB from database. "
			"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_DEBUG("[%s:%i:%s] LOB succesfully imported from database. LOB OID = %li. "
		"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, lobjId, err);
	fd = open(LOB_FILENAME_OUT, O_WRONLY | O_CREAT, 0666);
	if(fd==-1) {
		PRINT_DEBUG("[%s:%i:%s] Error in opening output filename. "
			"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	err = write(fd, gbuf_out->buf, gbuf_out->size);
	if(err==-1){
		PRINT_DEBUG("[%s:%i:%s] Error writing LOB to file. "
			"Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	close(fd);


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





