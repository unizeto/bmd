#ifndef _BMD_WSSE_H_INCLUDED_
#define _BMD_WSSE_H_INCLUDED_

#include "stdsoap2.h"

#define BMD_WSSE_PASSWORD_PLAIN		0
#define BMD_WSSE_PASSWORD_SHA1		1

/** server side **/

long IsWSSEDetected(struct soap* soapStruct);
long GetWSSEInformation(struct soap* soapStruct, char** userName, char**password, long *passwordType, char** certificate, long *certificateLen);

/** client side **/

typedef struct bmdWSSecurityClient
{
	EVP_PKEY *rsaPrivateKey;
} bmdWSSecurityClient_t;

long WSSecurity_client_init(struct soap *soapStruct, char* privateKeyPemFile, char* privateKeyPassword, char *certificatePemFile, char *user, char *userPassword, bmdWSSecurityClient_t* WSSEclientStruct);
long WSSecurity_client_clear(bmdWSSecurityClient_t* WSSEclientStruct);

#endif /* ifndef _BMD_WSSE_H_INCLUDED_ */
