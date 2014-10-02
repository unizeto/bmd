#include "bmd_wsse.h"
#include "wsseapi.h"


/** server side **/

/**
@retval 1 jesli zadanie z uzyciem WS Security
@retval 0 jesli zadanie bezz WS Security
@retval -1 jesli bledny parametr soapStruct
*/
long IsWSSEDetected(struct soap* soapStruct)
{
	struct _wsse__Security* security=NULL;

	if(soapStruct == NULL)
		{ return -1; }

	security=soap_wsse_Security(soapStruct);
	if(security == NULL)
	{
		return 0;
	}
	
	return 0;
}


/**
pobranie informacji z WS Security
@arg userName - aby nie pobierac tej informacji, podac NULL
@arg password - aby nie pobierac tej informacji, podac NULL (jesli != NULL, to passwordType musi byc != NULL)
@arg passwordType - aby nie pobierac tej informacji, podac NULL (jesli != NULL, to password musi byc != NULL)
@arg certificate - aby nie pobierac tej informacji, podac NULL
	pobranie certyfikatu jest mozliwe tylko wtedy, gdy strona kliencka nada mu id= X509Token
@retval 0 jesli sukces
@retval <0 w przypadku bledu
*/
long GetWSSEInformation(struct soap* soapStruct, char** userName, char**password, long *passwordType, char** certificate, long *certificateLen)
{
	X509 *X509UserCert=NULL;
	struct _wsse__Security* security=NULL;
	char* userNamePtr=NULL;

	/* walidacja parametrow */
	if(soapStruct == NULL)
		{ return -1; }

	if(userName != NULL)
	{
		if(*userName != NULL)
			{ return -2; }
	}

	if( (password != NULL && passwordType == NULL) || (password == NULL && passwordType != NULL) )
		{ return -3; }
	if(password != NULL)
	{
		if(*password != NULL)
			{ return -4; }
	}

	if( (certificate != NULL && certificateLen == NULL) || (certificate == NULL && certificateLen != NULL) )
		{ return -5; }
	if(certificate != NULL)
	{
		if(*certificate != NULL)
			{ return -6; }
	}
	
	/* pobieranie nazwy uzytkownika */
	if(userName != NULL)
	{	
		userNamePtr=(char*)soap_wsse_get_Username(soapStruct);
		if(userNamePtr == NULL)
		{
			soap_print_fault(soapStruct, stdout);
			return -11;
		}
		*userName=strdup(userNamePtr);
		userNamePtr=NULL;
		if(*userName == NULL)
		{
			return -12;
		}
	}

	if(certificate != NULL)
	{
		///? aby pominac weryfikacje struct _wsse__BinarySecurityToken *soap_wsse_BinarySecurityToken(struct soap *soap, const char *id); zamiast ponizszego
		X509UserCert=soap_wsse_get_BinarySecurityTokenX509(soapStruct, "X509Token");
		if(X509UserCert == NULL)
		{
			soap_print_fault(soapStruct, stdout);
			if(userName != NULL)
			{
				free(*userName);
				*userName=NULL;
			}
			return -13;
		}

		*certificateLen=i2d_X509(X509UserCert, (unsigned char**)certificate);
		X509_free(X509UserCert); X509UserCert=NULL;
		if(*certificate == NULL)
		{
			if(userName != NULL)
			{
				free(*userName);
				*userName=NULL;
			}
			return -14;
		}
		
	}

	/* pobieranie hasla */
	if(password != NULL)
	{
		security=soap_wsse_Security(soapStruct);
		if(security == NULL)
		{
			soap_print_fault(soapStruct, stdout);
			if(userName != NULL)
			{
				free(*userName);
				*userName=NULL;
			}
			if(certificate != NULL)
			{
				free(*certificate);
				*certificate=NULL;
			}
			return -15;
		}
		if(security->UsernameToken == NULL)
		{
			if(userName != NULL)
			{
				free(*userName);
				*userName=NULL;
			}
			if(certificate != NULL)
			{
				free(*certificate);
				*certificate=NULL;
			}
			return -16;
		}
		if(security->UsernameToken->Password != NULL)
		{
			if(security->UsernameToken->Password->Type == NULL)
			{
				if(userName != NULL)
				{
					free(*userName);
					*userName=NULL;
				}
				if(certificate != NULL)
				{
					free(*certificate);
					*certificate=NULL;
				}
				return -18;
			}
	
			if(strcmp(security->UsernameToken->Password->Type, wsse_PasswordDigestURI) == 0) /*skrot z hasla*/
			{
				*passwordType = BMD_WSSE_PASSWORD_SHA1;
			}
			else /*plain*/
			{
				*passwordType = BMD_WSSE_PASSWORD_PLAIN;
			}
			*password=strdup(security->UsernameToken->Password->__item);
			security=NULL;
			if(*password == NULL)
			{
				if(userName != NULL)
				{
					free(*userName);
					*userName=NULL;
				}
				if(certificate != NULL)
				{
					free(*certificate);
					*certificate=NULL;
				}
				return -19;
			}
		}
		
	}

	return 0;
}


/** client side **/


long WSSecurity_client_init(struct soap *soapStruct, char* privateKeyPemFile, char* privateKeyPassword, char *certificatePemFile, char *user, char *userPassword, bmdWSSecurityClient_t* WSSEclientStruct)
{
FILE *fd=NULL;
X509 *cert=NULL;
long status=0;

	if(soapStruct == NULL)
		{ return -1; }
	if(privateKeyPemFile == NULL)
		{ return -2; }
	if(strlen(privateKeyPemFile) <= 0)
		{ return -3; }
	if(certificatePemFile == NULL)
		{ return -4; }
	if(strlen(certificatePemFile) <= 0)
		{ return -5; }
	if(user == NULL)
		{ return -6; }
	if(WSSEclientStruct == NULL)
		{ return -7; }

	WSSEclientStruct->rsaPrivateKey=NULL;
	printf("\n\nWS SECURITY client: start\n");
	
	status=soap_register_plugin(soapStruct, soap_wsse);
	soap_print_fault(soapStruct, stdout);

	fd = fopen(privateKeyPemFile, "r");
	if(fd == NULL)
	{
		printf("WS Security client: no private key PEM file \"%s\"\n", privateKeyPemFile);
		return -11;
	}

	WSSEclientStruct->rsaPrivateKey = PEM_read_PrivateKey(fd, NULL, NULL, privateKeyPassword);
	fclose(fd); fd=NULL;
	
	if(WSSEclientStruct->rsaPrivateKey != NULL)
	{
		printf("WS Security client: private key has been loaded\n");
	}
	else
	{
		printf("WS Security client: can't load private key\n");
		return -12;
	}

	fd = fopen(certificatePemFile, "r");
	if(fd == NULL)
	{
		printf("WS Security client: no certificate PEM file \"%s\"\n", certificatePemFile);
		return -13;
	}
	cert = PEM_read_X509(fd, NULL, NULL, NULL);
	fclose(fd); fd=NULL;
	if(cert != NULL)
	{
		printf("WS Security client: certificate has been loaded\n");
	}
	else
	{
		printf("WS Security client: can't load certificate\n");
		return -14;
	}

	status=soap_wsse_add_UsernameTokenText(soapStruct, "UserToken", user, userPassword);
	if(status)
	{
		X509_free(cert);
		printf("WS Security client: can't add UsernameToken\n");
		soap_print_fault(soapStruct, stdout);
		return -15;
	}
	
	//status=soap_wsse_add_Timestamp(soap, "Time", 10);
	//soap_print_fault(soap, stdout);
	
	status=soap_wsse_add_BinarySecurityTokenX509(soapStruct, "X509Token", cert);
	X509_free(cert); cert=NULL;
	if(status)
	{
		soap_print_fault(soapStruct, stdout);
		printf("WS Security client: can't add binary security token (X509Token)\n");
		return -16;
	}
	status=soap_wsse_add_KeyInfo_SecurityTokenReferenceX509(soapStruct, "#X509Token");
	if(status)
	{
		soap_print_fault(soapStruct, stdout);
		printf("WS Security client: can't add security token reference (#X509Token)\n");
		return -17;
	}
	status=soap_wsse_sign(soapStruct, SOAP_SMD_SIGN_RSA_SHA1, WSSEclientStruct->rsaPrivateKey, 0);
	if(status)
	{
		soap_print_fault(soapStruct, stdout);
		printf("WS Security client: can't sign\n");
		return -18;
	}
	
	printf("WS Security client: will sign...\n");
	
	return 0;
}


long WSSecurity_client_clear(bmdWSSecurityClient_t* WSSEclientStruct)
{
	if(WSSEclientStruct == NULL)
		{ return 0; }

	EVP_PKEY_free(WSSEclientStruct->rsaPrivateKey);
	WSSEclientStruct->rsaPrivateKey=NULL;
	return 0;
}
