#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#endif /* XMLSEC_NO_XSLT */

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>
#include <xmlsec/io.h>

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdxades/libbmdxades.h>
#include <bmd/libbmdxades/xmlio.h>

/*dodane do sprawdzania zgodnosci poszczegolnych pol formularza*/
#include <bmd/libbmdxml/libbmdxml.h>
#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdts/libbmdts.h>
#include <time.h>

/*duuuuże numerki*/
#include <openssl/bn.h>

/*digesty*/
#include <openssl/evp.h>

/*CASA_XADES oznacza, że bierzemy pod uwagę naszego xadesa, generowanego przez
Łukaszka w aplecie, a nie tego, co mamy od Koszalina*/
#define CASA_XADES 1

/**
 * Funkcja dokonuje weryfikacji poprawności złożonego podpisu pod formularzem.
 * \param[in] mngr Zainicjalizowany menedżer kluczy.
 * \param[in] buffer Formularz do zweryfikowania.
 * \param[in] buffer_len Długość weryfikowanego formularza.
 *
 * \retval 0 Podpis poprawny.
 * \retval 1 Podpis niepoprawny.
 * \retval -1 Nie można przeparsować dokumentu.
 * \retval -2 Nie znaleziono startowego węzła.
 * \retval -3 Nie dało się utworzyć kontekstu podpisu.
 * \retval -4 Nie dało rady zweryfikować podpisu.
 * */
static long verify_memory(xmlSecKeysMngrPtr mngr, const char* buffer, const long buffer_len) {
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlSecDSigCtxPtr dsigCtx = NULL;
	long ret = -99;

	assert(mngr);

	/* load file */
	doc = xmlParseMemory(buffer,buffer_len);
	if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)){
		PRINT_DEBUG("UNABLE TO PARSE DOCUMENT\n");
		ret = -1;
		goto done;
	}

	/* find start node */
	node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
	if(node == NULL) {
		PRINT_DEBUG("Start node %s not found\n",xmlSecNodeSignature);
		ret = -2;
		goto done;
	}

	/* create signature context */
	dsigCtx = xmlSecDSigCtxCreate(mngr);
	if(dsigCtx == NULL) {
		PRINT_DEBUG("Failed to create signature context\n");
		ret = -3;
		goto done;
	}

	/* Verify signature */
	if(xmlSecDSigCtxVerify(dsigCtx, node) < 0) {
		PRINT_DEBUG("Error: signature verify failed\n");
		ret = -4;
		goto done;
	}


	/* print verification result to stdout */
	if(dsigCtx->status == xmlSecDSigStatusSucceeded) {
		ret = 0;
		PRINT_DEBUG("XAdES: Signature is OK\n");
	} else {
		ret = 1;
		PRINT_DEBUG("XAdES: Signature is INVALID\n");
	}


done:
			/* cleanup */
			if(dsigCtx != NULL) {
	xmlSecDSigCtxDestroy(dsigCtx);
			}

			if(doc != NULL) {
				xmlFreeDoc(doc);
			}
			return(ret);
}


/**Funkcja weryfikuje poprawność podpisu xades. Weryfikacja obejmuje obliczenia matematyczne
 * oraz weryfikację poszczególnych pól formualarza i certyfikatu.
 * \retval -109 Certyfikat poza terminem ważności.
 * */
long xades_verify(const char * buffer, const long buffer_len, GenBuf_t *timestamp)
{
	xmlSecKeysMngrPtr mngr = NULL;
	long ret = 0;
	long status = 0;

	if (buffer == NULL)
	{
		PRINT_DEBUG("Wrong argument 1!\n");
		return ERR_arg+1;
	}
	if (timestamp == NULL)
	{
		PRINT_DEBUG("Wrong argument 3!\n");
		return ERR_arg+3;
	}

	/* Init libxml and libxslt libraries */
	xades_init();

	/* create keys manager and load trusted certificates */
	mngr = xmlSecKeysMngrCreate();
	if(mngr == NULL) {
		PRINT_DEBUG( "Error: failed to create keys manager.\n");
		ret = -1;
	}

	if (mngr != NULL) {
		status = xmlSecCryptoAppDefaultKeysMngrInit(mngr);

		if(status < 0) {
			PRINT_DEBUG( "Error: failed to initialize keys manager: %li\n",status);
			ret = -2;
		} else {
			status = verify_memory(mngr,buffer, buffer_len);
			if (status != 0) {
				PRINT_ERROR("XAdES memory verification result: %li\n",status);
				ret = -4;
			}
			status = verify_fields(buffer, buffer_len, timestamp);
			if (status < 0)
			{
				PRINT_ERROR("XAdES tags verification result: %li\n", status);
				ret = -100+status;
			}
		}
	}
	xmlSecKeysMngrDestroy(mngr);
	xades_destroy();
	return ret;
}



/**
 * Funkcja porównuje zawartość pól X509IssuerName, X509SerialNumber i CertDigest
 * w certyfikacie i w formularzu.
 * \retval -1 Nie można przeparsować dokumentu.
 * \retval -2 Nie można pobrać info certyfikatu z formularza.
 * \retval -3 Nie można pobrać info z formularza.
 * \retval -4 Niezgodne numery seryjne.
 * \retval -5 Niezgodne nazwy wystawców.
 * \retval -6 Nie można utworzyć skrótu z certyfikatu.
 * \retval -7 Nie można pobrać skrótu certyfikatu z formularza.
 * \retval -8 Niezgodne skróty z certyfikatów.
 * \retval -9 Certyfikat przeterminowany.
 * */
long verify_fields(const char *buffer, const long buffer_len, GenBuf_t *timestamp)
{
	xmlDocPtr document 		= NULL;
	char *CertSerialNumber	= NULL;	/*serial sczytany z certyfikatu*/
	char *FormSerialNumber	= NULL;	/*serial sczytany z formularza*/
	char *CertIssuerName		= NULL; 	/*wystawca sczytany z certyfikatu*/
	char *FormIssuerName		= NULL; 	/*wystawca sczytany z formularza*/
	char *CertDigest			= NULL;	/*digest w base64 do porównania*/
	char *FormDigest			= NULL;	/*digest z formularza*/
	GenBuf_t *certyfikat		= NULL;	/*genbuf z certyfikatem*/
	LIBBMDXADES_DIGEST_METHOD_t metoda;
	long status;

	/*kontrola poprawnosci parametrow*/
	if (buffer == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (buffer_len == 0)
	{
		PRINT_DEBUG("Wrong argument 2 (too short!)\n");
		return ERR_arg+2;
	}

	/* load file */
	document = xmlParseMemory(buffer,buffer_len);
	if ((document == NULL) || (xmlDocGetRootElement(document) == NULL))
	{
		PRINT_DEBUG("UNABLE TO PARSE DOCUMENT\n");
		return -1;
	}

	/* pobieramy certyfikat*/
	status = _getCertificateFromXML(&document, &certyfikat);
	if (status < 0)
	{
		PRINT_DEBUG("Error while getting certificate.\n");
		return -2;
	}

	/*pobieramy date waznosci*/
	status = isCertificateValid(&certyfikat, timestamp);
	if (status < 0)
	{
		PRINT_DEBUG("Error - certificate not valid!\n");
		return -9;
	}

	/* get Serial and IssuerName from certificate in the form*/
	status = _getInfoFromCertificate(&certyfikat, &CertSerialNumber, &CertIssuerName);
	if (status < 0)
	{
		PRINT_DEBUG("Error while getting info from X509 Certificate.\n");
		return -2;
	}
	PRINT_VDEBUG("Form signed by certificate issued by %s, serial: %s\n",
					 CertIssuerName, CertSerialNumber);

	/* get Serial and IssuerName from the form*/
	_getInfoFromXML(&document, &FormSerialNumber, &FormIssuerName);
	if (status < 0)
	{
		PRINT_DEBUG("Error while getting info from the form.\n");
		return -3;
	}

	/*porównujemy seriale*/
	status = _compareSerials(&CertSerialNumber, &FormSerialNumber);
	if (status != 0)
	{
		PRINT_DEBUG("Bad serial number.\n");
		return -4;
	}

	/*porównujemy wystawcow*/
	status = _compareIssuerNames(&CertIssuerName, &FormIssuerName);
	if (status != 0)
	{
		PRINT_DEBUG("Bad issuer name.\n");
		return -5;
	}

	/*sprawdzamy digest*/
	status = _getDigestAndMethod(&document, &FormDigest, &metoda);
	if (status < 0)
	{
		PRINT_DEBUG("Cannot get digests from XML!\n");
		return -7;
	}
	_getCertificateDigest(&certyfikat, metoda, &CertDigest);
	if (status < 0)
	{
		PRINT_DEBUG("Error while digesting certificate!\n");
		return -6;
	}
	if (strcmp(FormDigest, CertDigest)!= 0)
	{
		PRINT_DEBUG("Digests in cert and XML vary!\n");
		return -8;
	}

	/*sprzatamy*/
	free(CertIssuerName);
	free(CertSerialNumber);
	free(FormIssuerName);
	free(FormSerialNumber);
	free(CertDigest);
	free(FormDigest);
	free_gen_buf(&certyfikat);
	xmlFreeDoc(document);
	return 0;
}

#if 0
/**funkcja zamienia czas w postaci stringu z timestampa do sekund od poczatku epoki*/
time_t _to_seconds(const char *date)
{
	struct tm storage={0,0,0,0,0,0,0,0,0};
	char *p=NULL;
	time_t retval=0;

	if (date == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}
#ifndef NPORTED
	p=(char *)strptime(date,"%Y-%m-%d %H:%M:%S",&storage);
#endif
	if(p==NULL)
	{
		retval=0;
	}
	else
	{
		retval=mktime(&storage);
	}
	return retval;
}
#endif

/**Funkcja sprawdza, czy certyfikat jest ważny w danym momencie oznaczonym przez timestamp.
 * \param certyfikat Certyfikat do sprawdzenia.
 * \param timestamp Znacznik czasu.
 * */
long isCertificateValid(GenBuf_t **certyfikat, GenBuf_t *timestamp)
{
	long status		= -1;
	char *validNB	= NULL;/*początek okresu wazności*/
	char *validNA	= NULL;/*koniec okresu ważności*/
	char *today		= NULL;/*dzisiejsza data*/
	GenBuf_t *PlainData = NULL;
	time_t tNB, tNA, tToday;

	if (certyfikat == NULL || *certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}
	if (timestamp == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	/*poczatek waznosci*/
	status = GetValidityNBFromX509Certificate_time(*certyfikat, &tNB);
	if (status < 0)
	{
		PRINT_DEBUG("Couldn't get validity not before\n");
		return -1;
	}

	/*koniec waznosci*/
	status = GetValidityNAFromX509Certificate_time(*certyfikat, &tNA);
	if (status < 0)
	{
		PRINT_DEBUG("Couldn't get validity not after\n");
		return -2;
	}

	PRINT_VDEBUG("Getting current date from timestamp...\n");

	status = GetGenerationTimeFromTimestamp_time(timestamp, &tToday);
	if (status < 0)
	{
		PRINT_DEBUG("Couldn't get current date and time\n");
		return -3;
	}

	/*ewentualne wydruki*/
	/*PRINT_VDEBUG("Valid not before: %s\n", validNB);
	PRINT_VDEBUG("Valid not after:  %s\n", validNA);
	PRINT_VDEBUG("Today is:         %s\n", today);*/

	/*konieczne porównania*/
	/*tNB = _to_seconds(validNB);
	tNA = _to_seconds(validNA);
	tToday = _to_seconds(today);*/

	if (tToday < tNB)
	{
		PRINT_DEBUG("Certificate not valid before %s!\n", validNB);
		return -4;
	}
	else if (tToday > tNA)
	{
		PRINT_DEBUG("Certificate not valid after %s!\n", validNA);
		return -5;
	}

	free(validNB);
	free(validNA);
	free(today);
	free_gen_buf(&PlainData);

	return 0;
}

/**
 * Funkcja pobiera numer seryjny i wystawcę certyfikatu z formularza.
 * \param document Wskaźnik na sparsowany dokument xml.
 * \param FormSerialNumber Wskaźnik na numer seryjny. String pod ten numer
 * zostanie zaalokowany, należy go potem zwolnić.
 * \param FormIssuerName Wskaźnik na nazwę wystawcy. String pod tę nazwę
 * zostanie zaalokowany, nalezy go potem zwolnić.
 *
 * \retval 0 Wszystko OK.
 * \retval -1 Nie można pobrać numeru seryjnego z formularza.
 * \retval -2 Nie można pobrać imienia wystawcy z formularza.
 * */
long _getInfoFromXML(const xmlDocPtr *document, char **FormSerialNumber, char **FormIssuerName)
{
	long status;
	char *xpath 			= NULL;	/*ściezka xpath do certyfikatu*/
	char *node_value 		= NULL;	/*zawartość taga ze zbejzowanym certem*/

	if (document == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (FormSerialNumber == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*FormSerialNumber != NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (FormIssuerName == NULL)
	{
		PRINT_DEBUG("Wrong argument 3\n");
		return ERR_arg+3;
	}

	if (*FormIssuerName != NULL)
	{
		PRINT_DEBUG("Wrong argument 3\n");
		return ERR_arg+3;
	}

	/*pobieramy serial*/
#ifdef CASA_XADES
	asprintf(&xpath, "//X509SerialNumber");
#else
	asprintf(&xpath, "//ds:Signature/ds:Object/xades:QualifyingProperties/xades:SignedProperties/xades:SignedSignatureProperties/xades:SigningCertificate/xades:Cert/xades:IssuerSerial/ds:X509SerialNumber");
#endif
	status = bmdxml_get_node_value_by_xpath (*document, xpath, &node_value);
	if (status < 0)
	{
		PRINT_DEBUG("Unable to get serial number from the form!\n");
		return -1;
	}
	asprintf(FormSerialNumber, "%s", node_value);
	free(node_value);
	node_value = NULL;
	free(xpath);
	xpath = NULL;

	/*pobieramy IssuerName*/
#ifdef CASA_XADES
	asprintf(&xpath, "//X509IssuerName");
#else
	asprintf(&xpath, "//ds:Signature/ds:Object/xades:QualifyingProperties/xades:SignedProperties/xades:SignedSignatureProperties/xades:SigningCertificate/xades:Cert/xades:IssuerSerial/ds:X509IssuerName");
#endif
	status = bmdxml_get_node_value_by_xpath (*document, (const char *) xpath, &node_value);
	if (status < 0)
	{
		PRINT_DEBUG("Unable to get issuer name from the form!\n");
		return -2;
	}
	asprintf(FormIssuerName, "%s", node_value);
	free(node_value);
	node_value = NULL;
	free(xpath);
	xpath = NULL;

	return 0;
}

/**
 * Funkcja pobiera numer seryjny i wystawcę certyfikatu z certyfikatu
 * zakodowanego w base64 w formularzu.
 * \param document Wskaźnik na sparsowany dokument xml.
 * \param FormSerialNumber Wskaźnik na numer seryjny. String pod ten numer
 * zostanie zaalokowany, należy go potem zwolnić.
 * \param FormIssuerName Wskaźnik na nazwę wystawcy. String pod tę nazwę
 * zostanie zaalokowany, nalezy go potem zwolnić.
 *
 * \retval 0 Wszystko OK.
 * \retval -1 Nie można pobrać numeru seryjnego z certyfikatu.
 * \retval -2 Nie można pobrać imienia wystawcy z certyfikatu.
 * */
long _getInfoFromCertificate(GenBuf_t ** certyfikat, char **CertSerialNumber, char **CertIssuerName)
{
	long status = 0;

	/*sprawdzamy wywolanie*/
	if (certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (*certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (CertSerialNumber == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*CertSerialNumber != NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (CertIssuerName == NULL)
	{
		PRINT_DEBUG("Wrong argument 3\n");
		return ERR_arg+3;
	}

	if (*CertIssuerName != NULL)
	{
		PRINT_DEBUG("Wrong argument 3\n");
		return ERR_arg+3;
	}

	/*pobieramy informacje z certyfikatu*/
	status = GetSerialNumberFromX509Certificate(*certyfikat, CertSerialNumber);
	if (status < 0)
	{
		PRINT_DEBUG("Cannot get the serial number!\n");
		return -1;
	}
	status = GetIssuerDNFromX509Certificate(*certyfikat, CertIssuerName);
	if (status < 0)
	{
		PRINT_DEBUG("Cannot get the issuer name!\n");
		return -2;
	}
	return 0;
}

/**
 * Funkcja pobiera certyfikat z formularza wczytanego do struktury xmlDocPtr i
 * zapisuje go do bufora generycznego.
 * \param document Sparsowany formularz XML.
 * \param certyfikat Wskaźnik na bufor generyczny. Pod wskazanym adresem zapisany
 * zostanie zdekodowany certyfikat. Potrzebna pamięc zostanie zaalokowana.
 *
 * \retval 0 Wszystko OK.
 * \retval -1 Nie można pobrać z formularza certyfikatu. Może podano zły formularz?
 * \retval -2 Nie można zdekodować certyfikatu.
 * */
long _getCertificateFromXML(xmlDocPtr *document, GenBuf_t ** certyfikat)
{
	long status			= 0;
	int si_temp			= 0;
	char *xpath 			= NULL;	/*ściezka xpath do certyfikatu*/
	char *node_value 		= NULL;	/*zawartość taga ze zbejzowanym certem*/
	size_t dlugosc_bufora=0;		/*długość rozbejzowanego certyfikatu*/

	/*sprawdzamy wywolanie*/
	if (document == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*certyfikat != NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg;
	}

	/*pobieramy certyfikat*/
#ifdef CASA_XADES
	asprintf(&xpath, "//ds:X509Certificate");
#else
	asprintf(&xpath, "//ds:Signature/ds:KeyInfo/ds:X509Data/ds:X509Certificate");
#endif
	status = bmdxml_get_node_value_by_xpath (*document, (const char *) xpath, &node_value);
	if (status < 0)
	{
		PRINT_DEBUG("Unable to get certificate from the form!\n");
		return -1;
	}

	/*dodajemy go do genbufa*/
	(*certyfikat) = (GenBuf_t*) malloc (sizeof(GenBuf_t));
	(*certyfikat)->buf = (char *)spc_base64_decode((unsigned char *)node_value, &dlugosc_bufora, 0, &si_temp);
	if (si_temp != 0)
	{
		PRINT_DEBUG("Error while decoding certificate!\n");
		return -2;
	}
	if (dlugosc_bufora <= 5)
	{
		PRINT_DEBUG("Decoded buffer has %li length!\n", (long)dlugosc_bufora);
		return -2;
	}
	(*certyfikat)->size = (long)dlugosc_bufora;

	/*sprzatamy*/
	free(xpath);
	free(node_value);

	return 0;
}

/**
 * Funkcja porównuje numery seryjne z certyfikatu i z formularza
 * */
long _compareSerials(char **cert, char **form)
{
	BIGNUM *certGigant	= NULL;	/*serial z certyfikatu*/
	BIGNUM *formGigant 	= NULL;	/*serial z formularza*/
	BN_CTX *ctx				= NULL;	/*kontekst dla numerków*/
	char *castrate			= NULL;	/*serial cert. pozbawiony spacji*/
	long equal				= -13;
	long i, j;
	long len		= 0;		/*długość numeru seryjnego*/

	/*wywolanie funkcji*/
	if (cert == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (*cert == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (form == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*form == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	/*kastracja cert*/
	len = (long)strlen(*cert);
	castrate = (char*) malloc (sizeof(char) * len);
	if (castrate == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return -13;
	}
	for (i = 0, j = 0; i < len; i++)
	{
		if ((*cert)[i] != ' ')
		{
			castrate[j] = (*cert)[i];
			j++;
		}
	}
	castrate[j] = '\0';

	/*inicjaliacja numerków*/
	certGigant = BN_new();
	formGigant = BN_new();

	/*ładujemy numerki do BN*/
	BN_hex2bn(&certGigant, castrate);
	BN_dec2bn(&formGigant, *form);

	/*porównujemy seriale*/
	equal = BN_cmp(certGigant, formGigant);

	/*inicjalizujemy kontekst do BN*/
	ctx = BN_CTX_new();

	/*sprzatamy*/
	free(castrate);
	BN_CTX_free(ctx);
	BN_free(certGigant);
	BN_free(formGigant);
	return equal;
}

/**
 * Funkcja porównuje nazwy wystawców z certyfikatu i z formularza.
 * */
long _compareIssuerNames(char **cert, char **form)
{
#define ILESKROTOW 8
	char skroty[8][10] = {"CN=", "SN=", "C=", "L=", "S=", "O=", "OU=", "G="};
	/*skroty poszczegolnych pol certyfikatu*/
	long i;
	char *formwsk = NULL;	/*wskaznik na pole w wystawcy wg formularza*/
	char *certwsk = NULL;	/*wskaznik na pole w wystawcy wg certyfikatu*/
	char *tmpwsk = NULL;		/*do obliczen na wskaznikach*/
	long dlugosc;
	long status;

	/*wywolanie funkcji*/
	if (cert == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (*cert == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (form == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*form == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	/*sprawdzamy czy w formularzu pojawiaja sie poszczegolne skroty*/
	for (i = 0; i < ILESKROTOW; i++)
	{
		formwsk = strstr(*form, skroty[i]);
		if (formwsk != NULL)/*znaleziono dane pole w formularzu*/
		{
			certwsk = strstr(*cert, skroty[i]);
			if (certwsk != NULL) /*znaleziono tez w certyfikacie*/
			{
				/*porownujemy zawartosc*/
				dlugosc = (long)strlen(skroty[i]);
				formwsk += dlugosc;
				certwsk += (dlugosc+1);

				tmpwsk = strchr(certwsk, '>');
				dlugosc = (long)(tmpwsk - certwsk);
				status = strncmp(certwsk, formwsk, dlugosc);
				if (status != 0)
				{
					return -1;
				}
			}
		}
#undef ILESKROTOW
	}
	return 0;
}

/**
 * Funkcja generuje skrót z certyfikatu wykorzystując podaną w drugim parametrze
 * metodę.
 * \param certyfikat Bufor generyczny ze zdekodowanym certyfikatem.
 * \param method Metoda generowania skrótu. W chwili obecnej do wyboru:
 * - LIBBMDXADES_DIGEST_METHOD_SHA1
 * - LIBBMDXADES_DIGEST_METHOD_MD5
 * \param basedHash Skrót zakodowany do base64.
 * \retval 0 Wszystko OK.
 * \retval -1 Nieznana metoda.
 * \retval -2 Brak pamięci.
 * */
long _getCertificateDigest(GenBuf_t **certyfikat, LIBBMDXADES_DIGEST_METHOD_t method, char **basedHash)
{
	EVP_MD_CTX mdctx;	/*kontekst digesta*/
	const EVP_MD *md;	/*metoda skrotu*/
	char md_value[EVP_MAX_MD_SIZE];	/*otrzymany skrot*/
	long md_len = 0;	/*dlugosc skrotu*/
	unsigned int ui_temp = 0;

	if (certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (*certyfikat == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (basedHash == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*basedHash != NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	OpenSSL_add_all_digests();

	switch (method)
	{
		case LIBBMDXADES_DIGEST_METHOD_SHA1:
			md = EVP_get_digestbyname("sha1");
			break;
		case LIBBMDXADES_DIGEST_METHOD_MD5:
			md = EVP_get_digestbyname("md5");
			break;
		default:
			PRINT_DEBUG("UNKNOWN DIGEST METHOD!\n");
			return -1;
	}

	EVP_MD_CTX_init(&mdctx);	/*inicjalizacja kontekstu*/
	EVP_DigestInit_ex(&mdctx, md, NULL);	/*ustawiamy odpowiednia metode liczenia skrotu*/
	EVP_DigestUpdate(&mdctx, (*certyfikat)->buf, (*certyfikat)->size);/*dodajemy tekst*/
	ui_temp = md_len;
	EVP_DigestFinal_ex(&mdctx, (unsigned char*)md_value, &ui_temp);
	md_len = ui_temp;
	EVP_MD_CTX_cleanup(&mdctx);

	(*basedHash) = (char *) spc_base64_encode((unsigned char *)md_value, md_len, 0);
	if (*basedHash == NULL)
	{
		PRINT_ERROR("NO MEMORY!\n");
		return -2;
	}
	return 0;
}

/**
 * Funkcja pobiera z formularza skrót certyfikatu i metodę generowania tego skrótu.
 * \param document Przeparsowany formularz XML.
 * \param hash Tutaj zostanie zaalokowany string ze skrótem pobranym z formularza.
 * \param metoda Tutaj zostanie zapisana metoda generowania skrótu.
 * */
long _getDigestAndMethod(const xmlDocPtr *document, char **hash, LIBBMDXADES_DIGEST_METHOD_t *metoda)
{
	long status;
	char *xpath 			= NULL;	/*ściezka xpath do digesta/metody*/
	char *node_value 		= NULL;	/*zawartość taga z metoda / digestem*/
	char *tmpwsk			= NULL;	/*do wybierania metody*/

	/*argumenty wywolania*/
	if (document == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return ERR_arg+1;
	}

	if (hash == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	if (*hash != NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return ERR_arg+2;
	}

	/*pobieramy metode*/
#ifdef CASA_XADES
	asprintf(&xpath, "//ds:Object//CertDigest[1]/DigestMethod/@Algorithm");
#else
	asprintf(&xpath, "//ds:Signature/ds:Object/xades:QualifyingProperties/xades:SignedProperties/xades:SignedSignatureProperties/xades:SigningCertificate/xades:Cert/xades:CertDigest[1]/ds:DigestMethod/@Algorithm");
#endif
	status = bmdxml_get_node_value_by_xpath (*document, xpath, &node_value);
	if (status < 0)
	{
		PRINT_DEBUG("Unable to get serial number from the form!\n");
		return -1;
	}
	if (strstr(node_value, "http://www.w3.org/2000/09/xmldsig#sha1") != NULL)
	{
		tmpwsk = strstr(node_value, "#sha1");
		if (tmpwsk != NULL && strlen(tmpwsk) == 5)
		{
			*metoda = LIBBMDXADES_DIGEST_METHOD_SHA1;
		}
	} else if(strstr(node_value, "http://www.w3.org/2000/09/xmldsig#md5") != NULL)
	{
		tmpwsk = strstr(node_value, "#md5");
		if (tmpwsk != NULL && strlen(tmpwsk) == 4)
		{
			*metoda = LIBBMDXADES_DIGEST_METHOD_MD5;
		}
	}
	free(node_value);
	node_value = NULL;
	free(xpath);
	xpath = NULL;

	/*pobieramy digest*/
#ifdef CASA_XADES
	asprintf(&xpath, "//ds:Object//CertDigest[1]/DigestValue");
#else
	asprintf(&xpath, "//ds:Signature/ds:Object/xades:QualifyingProperties/xades:SignedProperties/xades:SignedSignatureProperties/xades:SigningCertificate/xades:Cert/xades:CertDigest[1]/ds:DigestValue");
#endif
	status = bmdxml_get_node_value_by_xpath (*document, (const char *) xpath, &node_value);
	if (status < 0)
	{
		PRINT_DEBUG("Unable to get digest value from the form!\n");
		return -1;
	}
	asprintf(hash, "%s", node_value);
	free(node_value);
	node_value = NULL;
	free(xpath);
	xpath = NULL;

	return 0;
}
