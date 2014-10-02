#include "gen_ac.h"

#define AA_CONFIG_FILE		"./conf/uniac.cnf"
#define AA_SERIAL_FILE		"./conf/aa.serial"
#define CERTIFICATE_FILENAME	"bbanan1024.cer"
#define OUT_AC_FILENAME		"bbanan1024.acer"

extern long _GLOBAL_debug_level;
/*SQL_validator_data_t *_GLOBAL_validator_data=NULL;*/

int main(int argc, char *argv[])
{
int err = 0;
ConfigData_t *ConfigData = NULL;
GenBuf_t *X509CertGenBuf = NULL;
GenBuf_t *ACGenBuf	 = NULL;

	_GLOBAL_debug_level = 2;
	AA_PrintServerBanner();

	/* Wczytaj certyfikat w formacie DER i pobierz jego serial oraz DN wystawcy */
	printf("Parsing X509 certificate file \'%s\'...", CERTIFICATE_FILENAME);
	err = AA_db_read_GenBuf_from_file(CERTIFICATE_FILENAME, &X509CertGenBuf);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in reading DER certificate file \'%s\'. "
			"Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, CERTIFICATE_FILENAME, err);
		return err;
	}
	/* Wczytaj opcje konfiguracyjne serwera */
	printf("Parsing config file \'%s\'...", AA_CONFIG_FILE);
	err = ConfigData_read(AA_CONFIG_FILE, &ConfigData);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in parsing config file. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("[DONE]\n");
	err = ConfigData_valacinfoData);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in validating config file!!!. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	ConfigData_print(ConfigData);
	/* Wygeneruj certyfikat atrybutów dla wskazanego w X509CertGenBuf certyfikatu PKI	*
	 * Certyfikat zostanie umieszczonu w ACGenBuf. ConfigData zawiera opcje konfiguracyjne 	*
	 * serwera zaś AA_SERIAL_FILE nazwe z plikiem zawierajacym numer seryjny ostatnio	*
	 * wydanego certyfikatu.								*/

	err = AA_GenAttributeCert(X509CertGenBuf, &ACGenBuf, ConfigData);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in generating attribute certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	/* Zapisz certyfikat do pliku */
	err = AA_WriteGenBufToFile(ACGenBuf, OUT_AC_FILENAME);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in saving Attribute Certificate to file \'%s\'. "
		"Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, OUT_AC_FILENAME, err);
		return err;
	}
	err = ConfigData_free(&ConfigData);
	if(err < 0){
		printf("AASRVERR[%s:%i:%s] Error in freeing config data. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	free_gen_buf(&X509CertGenBuf);
	free_gen_buf(&ACGenBuf);
	return err;
}

