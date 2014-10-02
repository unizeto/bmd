#include <stdlib.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>

int main()
{
int err=0;
void *db_conn_handle		= NULL;

char *server_info_postgres =	"hostaddr='127.0.0.1' dbname='bae' port='5432' user='bae' password='12345678'";
char *server_info_oracle = 	"(DESCRIPTION = (ADDRESS_LIST = (ADDRESS = (PROTOCOL = TCP)(HOST = 127.0.0.1)(PORT = 1521)))"
				"(CONNECT_DATA = (SID = JS2) (SERVER = DEDICATED)))";

int crl_version = 1;
char *crl_signature_alg = "SHAwithRSA";
char *crl_issuerDN = "C=PL, O=Unizeto, OU=Deployment, CN=Level1";
char *crl_issuerSN = "01";
char *crl_thisUpdate = "1999-01-08 04:05:06+02";
char *crl_nextUpdate = "2000-01-08 04:05:06+02";
GenBuf_t *crl_genbuf = NULL;

int cert_version = 1;
char *cert_serialNumber = "01";
char *cert_subjectDN = "C=PL, O=Unizeto, OU=Deployment, CN=Bolek";
char *cert_signature_alg = "SHAwithRSA";
char *cert_issuerDN = "C=PL, O=Unizeto, OU=Deployment, CN=Level1";
char *cert_issuerSN = "01";
char *cert_validitynotBefore = "1999-01-08 04:05:06+02";
char *cert_validitynotAfter = "2000-01-08 04:05:06+02";
GenBuf_t *cert_genbuf = NULL;


unsigned long int timestamp_crypto_objects_id = 1;
int timestamp_version = 1;
int timestamp_policy = 2;
char *timestamp_serialNumber = "02";
char *timestamp_genTime = "1999-01-08 04:05:06+02";
char *timestamp_tsaDN = "C=PL, O=Unizeto, OU=Deployment, CN=Level1";
char *timestamp_tsaSN = "01";
PKIStandardEnum_t timestamp_standard=STANDARD_CMS;
WhoInsertIt_t timestamp_user_or_system=INSERT_USER;
GenBuf_t *timestamp_genbuf = NULL;

unsigned long int signature_crypto_objects_id = 1;
PKIStandardEnum_t signature_standard=STANDARD_CMS;
WhoInsertIt_t signature_user_or_system=INSERT_USER;
GenBuf_t *signature_genbuf = NULL;

bmdDatagram_t *dg1 = NULL; /* datagram do pobrania wartości dowodowych */
unsigned long int which_crypto_object = 1; /* dla ktorego crypto objecta pobrac dane dowodowe */

#define crl_genbuf_size 20
#define cert_genbuf_size 20
#define timestamp_genbuf_size 50
#define signature_genbuf_size 50


	crl_genbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	crl_genbuf->buf = malloc(sizeof(char) * crl_genbuf_size);
	memset(crl_genbuf->buf, 0, crl_genbuf_size);
	crl_genbuf->size = crl_genbuf_size;
	strcpy(crl_genbuf->buf, "zawartosc crl");

	cert_genbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	cert_genbuf->buf = malloc(sizeof(char) * crl_genbuf_size);
	memset(cert_genbuf->buf, 0, cert_genbuf_size);
	cert_genbuf->size = cert_genbuf_size;
	strcpy(cert_genbuf->buf, "zawartosc certyfikatu");

	timestamp_genbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	timestamp_genbuf->buf = malloc(sizeof(char) * timestamp_genbuf_size);
	memset(timestamp_genbuf->buf, 0, timestamp_genbuf_size);
	timestamp_genbuf->size = timestamp_genbuf_size;
	strcpy(timestamp_genbuf->buf, "zawartosc znacznika czasu");

	signature_genbuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	signature_genbuf->buf = malloc(sizeof(char) * signature_genbuf_size);
	memset(signature_genbuf->buf, 0, signature_genbuf_size);
	signature_genbuf->size = signature_genbuf_size;
	strcpy(signature_genbuf->buf, "zawartosc podpisu");

/*	bmd_db_connect(server_info_oracle, &db_conn_handle);*/
	bmd_db_connect(server_info_postgres, &db_conn_handle);

	err = bmd_db_start_transaction(db_conn_handle);
	if(err<0) {
		PRINT_DEBUG("INSERTERR[%s:%i:%s] Error in begining database transaction. Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	/* przykład umiezczania CRLa w bazie */
/*	err = prfInsertCrl(db_conn_handle,
			crl_version,
			crl_signature_alg,
			crl_issuerDN,
			crl_issuerSN,
			crl_thisUpdate,
			crl_nextUpdate,
			crl_genbuf,
			TYPE_CRL,
			options);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	
*/	/* przykład umiezczania ARLa w bazie */
/*	err = prfInsertCrl(db_conn_handle,
			crl_version,
			crl_signature_alg,
			crl_issuerDN,
			crl_issuerSN,
			crl_thisUpdate,
			crl_nextUpdate,
			crl_genbuf,
			TYPE_ARL,
			options);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	
*/	
	/* przykład umiezczania certyfikatu w bazie */
/*	err = prfInsertCert(db_conn_handle,
			cert_version,
			cert_serialNumber,
			cert_subjectDN,
			cert_signature_alg,
			cert_issuerDN,
			cert_issuerSN,
			cert_validitynotBefore,
			cert_validitynotAfter,
			cert_genbuf,
			options);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	
*/

	/* przykład umiezczania znacznika w bazie */
/*	err = prfInsertTimestamp(db_conn_handle,
			timestamp_crypto_objects_id,
			timestamp_version,
			timestamp_policy,
			timestamp_serialNumber,
			timestamp_genTime,
			timestamp_tsaDN,
			timestamp_tsaSN,
			timestamp_standard,
			timestamp_user_or_system,
			timestamp_genbuf,
			options);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	
*/
	/* przyklad umieszczania */
/*	err = prfInsertSignature(db_conn_handle,
			signature_crypto_objects_id,
			signature_standard,
			signature_user_or_system,
			signature_genbuf,
			options);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	

*/
	/* jeszcze poberzmy wszystkie dane dowodowe */
	dg1 = (bmdDatagram_t *) malloc (sizeof(bmdDatagram_t));
	PR_bmdDatagram_init(dg1);
	err = prfGetAllProofs(db_conn_handle, which_crypto_object, dg1);
	if(err<0){
		printf("ERR[%s:%i%s]", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}	

	PR_bmdDatagram_print(dg1);

	err = bmd_db_end_transaction(db_conn_handle);
	if(err<0) {
		PRINT_DEBUG("INSERTERR[%s:%i:%s] Error in ending database transaction. Recieved error code = %i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	bmd_db_disconnect(&db_conn_handle);
	return err;
}

