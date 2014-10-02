#include <bmd/libbmddvcs/libbmddvcs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmderr/libbmderr.h> /*potrzebne asprintf - pod Windows*/

#define CHECK_UNKNOWN		0
#define CHECK_CERTIFICATE	1
#define CHECK_INTERNAL_SIG	2
#define CHECK_EXTERNAL_SIG	3
#define CHECK_POSSESSION	4
#define CHECK_EXISTENCE		5


	
void showHelp(void);
long parseCommandLine(char **argv, long argc,
			char **dvcsAddress, long *connectionTimeout,
			char **pfxFile, char **pfxPass,
			long *requestType,
			char **requestDumpFile, char **responseDumpFile,
			char **certFile, char **sigFile, char **dataFile, char **dataSHA1File,
			char **tsrTime, char **tsrToken,
			char **tsrAttachment1, char **tsrAttachment2);
long validateParameters(char *dvcsAddress, long *connectionTimeout,
			char *pfxFile, char *pfxPass,
			long requestType,
			char *certFile, char *sigFile, char *dataFile, char *dataSHA1File,
			char *tsrTime, char* tsrToken,
			char *tsrAttachment1, char *tsrAttachment2);


void showHelp(void)
{
printf("\n====================================================\n");
printf("\nDVCS TESTER (libbmdDVCS library as client API)\n");
printf("\n====================================================\n");
printf("\nGENERAL OPTIONS:\n");
printf("\nMANDATORY:\n");
printf("\t-serv\t\t-\tDVCS server address\n");
printf("\t-pfx\t\t-\t.pfx file for signing dvcs request\n");
printf("\t-pass\t\t-\t.pfx password\n");
printf("\t-type\t\t-\trequest type\n");
printf("OPTIONAL:\n");
printf("\t-timeout\t-\tconnection timeout\n");
///printf("\t-i\t-\tfile for saving dvcs request\n");
printf("\t-o\t\t-\tfile for saving dvcs certificate\n");
printf("\nRequest types:\n");
printf("\tCHECK_CERTIFICATE\n");
printf("\tCHECK_INTERNAL_SIG\n");
printf("\tCHECK_EXTERNAL_SIG\n");
printf("\tCHECK_POSSESSION\n");
printf("\tCHECK_EXISTENCE\n");
printf("\nOPTIONS FOR PARTICULAR REQUEST TYPES:\n");
printf("\nCHECK_CERTIFICATE:\n");
printf("\t-cert\t\t-\tfile with certificate (mandatory)\n");
printf("CHECK_INTERNAL_SIG:\n");
printf("\t-sig\t\t-\tfile with internal signature (mandatory)\n");
printf("\t-tsrTime\t-\ttimestamp file (for setting proof time in request) [OPTIONAL]\n");
printf("\t-tsrToken\t-\ttimestamp file (for setting proof token in request) [OPTIONAL]\n");
printf("CHECK_EXTERNAL_SIG:\n");
printf("\t-sig\t\t-\tfile with external signature (mandatory)\n");
printf("\t-data\t\t-\tfile with signed data (mandatory if -dataSHA1 option not specified)\n");
printf("\t-dataSHA1\t-\tfile with SHA-1 digest of signed data (mandatory if -data option not specified)\n");
printf("\t-tsrTime\t-\ttimestamp file (for setting proof time in request) [OPTIONAL]\n");
printf("\t-tsrToken\t-\ttimestamp file (for setting proof token in request) [OPTIONAL]\n");
printf("\t-tsrAttachment1\t-\ttimestamp file (for adding timestamp in request) [OPTIONAL]\n");
printf("\t-tsrAttachment2\t-\ttimestamp file (for adding timestamp in request) [OPTIONAL]\n");
printf("CHECK_POSSESSION:\n");
printf("\t-data\t\t-\tdata file (mandatory)\n");
printf("CHECK_EXISTENCE:\n");
printf("\t-data\t\t-\tdata file (mandatory)\n");
printf("\n====================================================\n");
}

long parseCommandLine(char **argv, long argc,
			char **dvcsAddress, long *connectionTimeout,
			char **pfxFile, char **pfxPass,
			long *requestType,
			char **requestDumpFile, char **responseDumpFile,
			char **certFile, char **sigFile, char **dataFile, char **dataSHA1File,
			char **tsrTime, char **tsrToken,
			char **tsrAttachment1, char **tsrAttachment2)
{
	long iter=0;
	long errorOccured=0	;

	char *tempdvcsAddress=NULL;
	long tempconnectionTimeout=0;
	char *temppfxFile=NULL;
	char *temppfxPass=NULL;
	long temprequestType=0;
	char *temprequestDumpFile=NULL;
	char *tempresponseDumpFile=NULL;
	char *tempcertFile=NULL;
	char *tempsigFile=NULL;
	char *tempdataFile=NULL;
	char *tempdataSHA1File=NULL;
	char *temptsrTime=NULL;
	char *temptsrToken=NULL;
	char *temptsrAttachment1=NULL;
	char *temptsrAttachment2=NULL;


	if(argv == NULL)
		{ return -1; }
	if(argc <= 1)
		{ return -2; }
	if(dvcsAddress == NULL)
		{ return -3; }
	if(pfxFile == NULL)
		{ return -4; }
	if(pfxPass == NULL)
		{ return -5; }
	if(requestType == NULL)
		{ return -6; }
	if(requestDumpFile == NULL)
		{ return -7; }
	if(responseDumpFile == NULL)
		{ return -8; }
	if(certFile == NULL)
		{ return -9; }
	if(sigFile == NULL)
		{ return -10; }
	if(dataFile == NULL)
		{ return -11; }
	if(connectionTimeout == NULL)
		{ return -12; }
	if(dataSHA1File == NULL)
		{ return -13; }
	if(tsrTime == NULL)
		{ return -14; }
	if(tsrToken == NULL)
		{ return -15; }
	if(tsrAttachment1 == NULL)
		{ return -16; }
	if(tsrAttachment2 == NULL)
		{ return -17; }



	for(iter=1; iter<argc; iter++)
	{
		if( strcmp(argv[iter], "-serv") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempdvcsAddress, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-timeout") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			tempconnectionTimeout=atol(argv[iter]);
		}
		else if( strcmp(argv[iter], "-pfx") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temppfxFile, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-pass") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temppfxPass, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-type") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}

			if(strcmp(argv[iter], "CHECK_INTERNAL_SIG") == 0 )
			{
				temprequestType=CHECK_INTERNAL_SIG;
			}
			else if(strcmp(argv[iter], "CHECK_EXTERNAL_SIG") == 0)
			{
				temprequestType=CHECK_EXTERNAL_SIG;
			}
			else if(strcmp(argv[iter], "CHECK_CERTIFICATE") == 0)
			{
				temprequestType=CHECK_CERTIFICATE;
			}
			else if(strcmp(argv[iter], "CHECK_POSSESSION") == 0)
			{
				temprequestType=CHECK_POSSESSION;
			}
			else if(strcmp(argv[iter], "CHECK_EXISTENCE") == 0)
			{
				temprequestType=CHECK_EXISTENCE;
			}
			else
			{
				temprequestType=CHECK_UNKNOWN;
			}
		}
///		else if( strcmp(argv[iter], "-i") == 0)
///		{
///			iter++;
///			if(iter >= argc)
///			{
///				errorOccured=1;
///				break;
///			}
///			asprintf(&temprequestDumpFile, "%s", argv[iter]);
///		}
		else if( strcmp(argv[iter], "-o") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempresponseDumpFile, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-cert") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempcertFile, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-sig") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempsigFile, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-data") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempdataFile, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-dataSHA1") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&tempdataSHA1File, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-tsrTime") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temptsrTime, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-tsrToken") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temptsrToken, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-tsrAttachment1") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temptsrAttachment1, "%s", argv[iter]);
		}
		else if( strcmp(argv[iter], "-tsrAttachment2") == 0)
		{
			iter++;
			if(iter >= argc)
			{
				errorOccured=1;
				break;
			}
			asprintf(&temptsrAttachment2, "%s", argv[iter]);
		}
		else
		{
			printf("\nERROR:\tUnrecognized option \"%s\"\n", argv[iter]);
			errorOccured=1;
			break;
		}
	}

	if(errorOccured != 0)
	{
		free(tempdvcsAddress);
		free(temppfxFile);
		free(temppfxPass);
		free(temprequestDumpFile);
		free(tempresponseDumpFile);
		free(tempcertFile);
		free(tempsigFile);
		free(tempdataFile);
		free(tempdataSHA1File);
		free(temptsrTime);
		free(temptsrToken);
		free(temptsrAttachment1);
		free(temptsrAttachment2);
		return -20;
	}
	else
	{
		*dvcsAddress=tempdvcsAddress;
		*connectionTimeout=tempconnectionTimeout;
		*pfxFile=temppfxFile;
		*pfxPass=temppfxPass;
		*requestType=temprequestType;
		*requestDumpFile=temprequestDumpFile;
		*responseDumpFile=tempresponseDumpFile;
		*certFile=tempcertFile;
		*sigFile=tempsigFile;
		*dataFile=tempdataFile;
		*dataSHA1File=tempdataSHA1File;
		*tsrTime=temptsrTime;
		*tsrToken=temptsrToken;
		*tsrAttachment1=temptsrAttachment1;
		*tsrAttachment2=temptsrAttachment2;
		return 0;
	}
}

long validateParameters(char *dvcsAddress, long *connectionTimeout,
			char *pfxFile, char *pfxPass,
			long requestType,
			char *certFile, char *sigFile, char *dataFile, char *dataSHA1File,
			char *tsrTime, char* tsrToken,
			char *tsrAttachment1, char *tsrAttachment2)
{

	if(dvcsAddress == NULL)
	{
		printf("ERROR:\tDVCS address not specified\n");
		return -1;
	}
	if(pfxFile == NULL)
	{
		printf("ERROR:\t.pfx file not specified\n");
		return -2;
	}
	if(pfxPass == NULL)
	{
		printf("ERROR:\t.pfx password not specified\n");
		return -3;
	}
	if(*connectionTimeout < 15)
	{
		*connectionTimeout=15;
	}

	

	if(requestType == CHECK_CERTIFICATE)
	{
		if(certFile == NULL)
		{
			printf("ERROR:\tcertificate not specified\n");
			return -4;
		}
		if(sigFile != NULL)
		{
			printf("ERROR:\t-sig option is not allowed for CHECK_CERTIFICATE type\n");
			return -5;
		}
		if(dataFile != NULL)
		{
			printf("ERROR:\t-data option is not allowed for CHECK_CERTIFICATE type\n");
			return -6;
		}
		if(dataSHA1File != NULL)
		{
			printf("ERROR:\t-dataSHA1 option is not allowed for CHECK_CERTIFICATE type\n");
			return -20;
		}
		if(tsrTime != NULL)
		{
			printf("ERROR:\t-tsrTime option is not allowed for CHECK_CERTIFICATE type\n");
			return -101;
		}
		if(tsrToken != NULL)
		{
			printf("ERROR:\t-tsrToken option is not allowed for CHECK_CERTIFICATE type\n");
			return -102;
		}
		if(tsrAttachment1 != NULL)
		{
			printf("ERROR:\t-tsrAttachment1 option is not allowed for CHECK_CERTIFICATE type\n");
			return -201;
		}
		if(tsrAttachment2 != NULL)
		{
			printf("ERROR:\t-tsrAttachment2 option is not allowed for CHECK_CERTIFICATE type\n");
			return -202;
		}
	}
	else if(requestType == CHECK_INTERNAL_SIG)
	{
		if(sigFile == NULL)
		{
			printf("ERROR:\tsignature file not specified\n");
			return -7;
		}
		if(certFile != NULL)
		{
			printf("ERROR:\t-cert option is not allowed for CHECK_INTERNAL_SIG type\n");
			return -8;
		}
		if(dataFile != NULL)
		{
			printf("ERROR:\t-data option is not allowed for CHECK_INTERNAL_SIG type\n");
			return -9;
		}
		if(dataSHA1File != NULL)
		{
			printf("ERROR:\t-dataSHA1 option is not allowed for CHECK_INTERNAL_SIG type\n");
			return -21;
		}
		if(tsrTime != NULL && tsrToken != NULL)
		{
			printf("ERROR:\t-tsrTime and -tsrToken options can't be used simultaneously. Choose one of them.\n");
			return -131;
		}
		if(tsrAttachment1 != NULL)
		{
			printf("ERROR:\t-tsrAttachment1 option is not allowed for CHECK_INTERNAL_SIG type\n");
			return -203;
		}
		if(tsrAttachment2 != NULL)
		{
			printf("ERROR:\t-tsrAttachment2 option is not allowed for CHECK_INTERNAL_SIG type\n");
			return -204;
		}
	}
	else if(requestType == CHECK_EXTERNAL_SIG)
	{
		if(sigFile == NULL)
		{
			printf("ERROR:\tsignature file not specified\n");
			return -10;
		}
		if(dataFile == NULL && dataSHA1File == NULL)
		{
			printf("ERROR:\tneither data nor data SHA1 digest file specified\n");
			return -11;
		}
		if(dataFile != NULL && dataSHA1File != NULL)
		{
			printf("ERROR:\tspecify only one from -data or -dataSHA1 option\n");
			return -24;
		}
		if(certFile != NULL)
		{
			printf("ERROR:\t-cert option is not allowed for CHECK_EXTERNAL_SIG type\n");
			return -12;
		}
		if(tsrTime != NULL && tsrToken != NULL)
		{
			printf("ERROR:\t-tsrTime and -tsrToken options can't be used simultaneously. Choose one of them.\n");
			return -141;
		}
	}
	else if(requestType == CHECK_POSSESSION)
	{
		if(dataFile == NULL)
		{
			printf("ERROR:\tdata file not specified\n");
			return -13;
		}
		if(sigFile != NULL)
		{
			printf("ERROR:\t-sig option is not allowed for CHECK_POSSESSION type\n");
			return -14;
		}
		if(certFile != NULL)
		{
			printf("ERROR:\t-cert option is not allowed for CHECK_POSSESSION type\n");
			return -15;
		}
		if(dataSHA1File != NULL)
		{
			printf("ERROR:\t-dataSHA1 option is not allowed for CHECK_POSSESSION type\n");
			return -22;
		}
		if(tsrTime != NULL)
		{
			printf("ERROR:\t-tsrTime option is not allowed for CHECK_POSSESSION type\n");
			return -111;
		}
		if(tsrToken != NULL)
		{
			printf("ERROR:\t-tsrToken option is not allowed for CHECK_POSSESSION type\n");
			return -112;
		}
		if(tsrAttachment1 != NULL)
		{
			printf("ERROR:\t-tsrAttachment1 option is not allowed for CHECK_POSSESSION type\n");
			return -205;
		}
		if(tsrAttachment2 != NULL)
		{
			printf("ERROR:\t-tsrAttachment2 option is not allowed for CHECK_POSSESSION type\n");
			return -206;
		}
	}
	else if(requestType == CHECK_EXISTENCE)
	{
		if(dataFile == NULL)
		{
			printf("ERROR:\tdata file not specified\n");
			return -16;
		}
		if(sigFile != NULL)
		{
			printf("ERROR:\t-sig option is not allowed for CHECK_EXISTENCE type\n");
			return -17;
		}
		if(certFile != NULL)
		{
			printf("ERROR:\t-cert option is not allowed for CHECK_EXISTENCE type\n");
			return -18;
		}
		if(dataSHA1File != NULL)
		{
			printf("ERROR:\t-dataSHA1 option is not allowed for CHECK_EXISTENCE type\n");
			return -23;
		}
		if(tsrTime != NULL)
		{
			printf("ERROR:\t-tsrTime option is not allowed for CHECK_EXISTENCE type\n");
			return -121;
		}
		if(tsrToken != NULL)
		{
			printf("ERROR:\t-tsrToken option is not allowed for CHECK_EXISTENCE type\n");
			return -122;
		}
		if(tsrAttachment1 != NULL)
		{
			printf("ERROR:\t-tsrAttachment1 option is not allowed for CHECK_EXISTENCE type\n");
			return -207;
		}
		if(tsrAttachment2 != NULL)
		{
			printf("ERROR:\t-tsrAttachment2 option is not allowed for CHECK_EXISTENCE type\n");
			return -208;
		}
	}
	else
	{
		printf("ERROR:\tunknown request type\n");
		return -19;
	}
	
	return 0;
}



int main(int argc, char **argv)
{
long retVal=0;

char *dvcsAddress=NULL;
long connectionTimeout=0;
char *pfxFile=NULL;
char *pfxPass=NULL;
long requestType=0;
char *requestDumpFile=NULL;
char *responseDumpFile=NULL;
char *certFile=NULL;
char *sigFile=NULL;
char *dataFile=NULL;
char *dataSHA1File=NULL;
char *tsrTime=NULL;
char *tsrToken=NULL;
char *tsrAttachment1=NULL;
char *tsrAttachment2=NULL;

bmd_crypt_ctx_t *context=NULL;
bmdDVCS_t *dvcsStruct=NULL;
GenBuf_t *certBuf=NULL;
GenBuf_t *sigBuf=NULL;
GenBuf_t *dataBuf=NULL;
GenBuf_t *dvcsCert=NULL;
GenBuf_t *tsrBuf=NULL;
char *errorString=NULL;


	retVal=parseCommandLine(argv, argc, &dvcsAddress, &connectionTimeout, &pfxFile, &pfxPass, &requestType,
				&requestDumpFile, &responseDumpFile, &certFile, &sigFile, &dataFile, &dataSHA1File, &tsrTime, &tsrToken, &tsrAttachment1, &tsrAttachment2);
	if(retVal != 0)
	{
		showHelp();
		return -1;
	}

	retVal=validateParameters(dvcsAddress, &connectionTimeout, pfxFile, pfxPass, requestType, certFile, sigFile, dataFile, dataSHA1File, tsrTime, tsrToken, tsrAttachment1, tsrAttachment2);
	if(retVal != 0)
	{
		showHelp();
		free(dvcsAddress);
		free(pfxFile);
		free(pfxPass);
		free(requestDumpFile);
		free(responseDumpFile);
		free(certFile);
		free(sigFile);
		free(dataFile);
		free(dataSHA1File);
		free(tsrTime);
		free(tsrToken);
		free(tsrAttachment1);
		free(tsrAttachment2);
		return -2;
	}


	bmd_init();
	retVal=bmd_set_ctx_file(pfxFile, pfxPass, (long)strlen(pfxPass), &context);
	if(retVal != BMD_OK)
	{
		printf("ERROR:\tUnable to create request signing context. Is \"%s\" correct PKCS#12 file ?\n", pfxFile);
		free(dvcsAddress);
		free(pfxFile);
		free(pfxPass);
		free(requestDumpFile);
		free(responseDumpFile);
		free(certFile);
		free(sigFile);
		free(dataFile);
		free(dataSHA1File);
		free(tsrTime);
		free(tsrToken);
		free(tsrAttachment1);
		free(tsrAttachment2);
		return -3;
	}

	printf("\n====================================================\n");
	switch(requestType)
	{
		case CHECK_CERTIFICATE:
		{
			printf("\nCHECK_CERTIFICATE started...\n");
			retVal=bmd_load_binary_content(certFile, &certBuf);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file\n", certFile);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -10;
			}
			
			retVal=bmdDVCS_load_memCert(certBuf, &dvcsStruct);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file or improper certificate structure\n", certFile);
				free_gen_buf(&certBuf);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -11;
			}
			assert(bmdDVCS_set_server(dvcsStruct, dvcsAddress) == 0);
			if(connectionTimeout > 0)
			{
				bmdDVCS_set_connectionTimeout(dvcsStruct, connectionTimeout);
			}
			
			retVal=bmdDVCS_verify_cert(dvcsStruct, context);
			free_gen_buf(&certBuf);
			bmdDVCS_get_errString(dvcsStruct, &errorString);
			printf("DVCS SERVER MESSAGE: [%s]\n", errorString);
			free(errorString);
			if(retVal < -1)
			{
				printf("ERROR:\tOperation status (bmdDVCS_verify_cert()) : %li\n", retVal);
			}
			else
			{
				printf("SUCCESS:\tOperation status (bmdDVCS_verify_cert()) : %li\n", retVal);
			}
		
			break;
		}
		case CHECK_INTERNAL_SIG:
		{
			printf("\nCHECK_INTERNAL_SIG started...\n");
			retVal=bmd_load_binary_content(sigFile, &sigBuf);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file\n", sigFile);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -12;
			}
			assert(bmdDVCS_load_Sig(sigBuf, &dvcsStruct) == 0);
			assert(bmdDVCS_set_server(dvcsStruct, dvcsAddress) == 0);
			if(connectionTimeout > 0)
			{
				bmdDVCS_set_connectionTimeout(dvcsStruct, connectionTimeout);
			}

			if(tsrTime != NULL)
			{
				retVal=bmd_load_binary_content(tsrTime, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrTime);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -101;
				}
				assert(bmdDVCS_set_proof_Time(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}
			else if(tsrToken != NULL)
			{
				retVal=bmd_load_binary_content(tsrToken, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrToken);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -102;
				}
				assert(bmdDVCS_set_proof_TimestampToken(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}

			retVal=bmdDVCS_validate_signature(dvcsStruct, context);
			free_gen_buf(&sigBuf);
			bmdDVCS_get_errString(dvcsStruct, &errorString);
			printf("DVCS SERVER MESSAGE: [%s]\n", errorString);
			free(errorString);
			if(retVal < -1)
			{
				printf("ERROR:\tOperation status (bmdDVCS_validate_signature()) : %li\n", retVal);
			}
			else
			{
				printf("SUCCESS:\tOperation status (bmdDVCS_validate_signature()) : %li\n", retVal);
			}
		
			break;
		}
		case CHECK_EXTERNAL_SIG:
		{
			printf("\nCHECK_EXTERNAL_SIG started...\n");
			retVal=bmd_load_binary_content(sigFile, &sigBuf);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file\n", sigFile);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -13;
			}
			assert(bmdDVCS_load_Sig(sigBuf, &dvcsStruct) == 0);

			retVal=bmd_load_binary_content( (dataFile != NULL ? dataFile : dataSHA1File), &dataBuf);
			if(retVal != 0)
			{
				free_gen_buf(&sigBuf);
				printf("ERROR:\tUnable to load \"%s\" file\n", (dataFile != NULL ? dataFile : dataSHA1File) );
				bmd_ctx_destroy(&context);
				bmd_end();
				return -14;
			}

			assert(bmdDVCS_set_server(dvcsStruct, dvcsAddress) == 0);
			if(connectionTimeout > 0)
			{
				bmdDVCS_set_connectionTimeout(dvcsStruct, connectionTimeout);
			}

			if(tsrTime != NULL)
			{
				retVal=bmd_load_binary_content(tsrTime, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrTime);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -111;
				}
				assert(bmdDVCS_set_proof_Time(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}
			else if(tsrToken != NULL)
			{
				retVal=bmd_load_binary_content(tsrToken, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrToken);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -112;
				}
				assert(bmdDVCS_set_proof_TimestampToken(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}
			
			if(tsrAttachment1 != NULL)
			{
				retVal=bmd_load_binary_content(tsrAttachment1, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrAttachment1);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -209;
				}
				assert(bmdDVCS_add_timestamp_attachment(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}
			
			if(tsrAttachment2 != NULL)
			{
				retVal=bmd_load_binary_content(tsrAttachment2, &tsrBuf);
				if(retVal != 0)
				{
					printf("ERROR:\tUnable to load \"%s\" file\n", tsrAttachment2);
					bmdDVCS_destroy(&dvcsStruct);
					bmd_ctx_destroy(&context);
					bmd_end();
					return -210;
				}
				assert(bmdDVCS_add_timestamp_attachment(dvcsStruct, tsrBuf) == 0);
				free_gen_buf(&tsrBuf);
			}

			if(dataFile == NULL)
			{
				assert(bmdDVCS_set_hash_algorithm(dvcsStruct, OID_HASH_FUNCTION_SHA1) == 0);
			}
			
			retVal=	bmdDVCS_validate_externalSignature(dvcsStruct, dataBuf,
				(dataFile != NULL ? BMDDVCS_EXTERNAL_DOCUMENT_CONTENT : BMDDVCS_EXTERNAL_DOCUMENT_HASH),
				context);
			free_gen_buf(&sigBuf);
			free_gen_buf(&dataBuf);
			bmdDVCS_get_errString(dvcsStruct, &errorString);
			printf("DVCS SERVER MESSAGE: [%s]\n", errorString);
			free(errorString);
			if(retVal < -1)
			{
				printf("ERROR:\tOperation status (bmdDVCS_validate_externalSignature()) : %li\n", retVal);
			}
			else
			{
				printf("SUCCESS:\tOperation status (bmdDVCS_validate_externalSignature()) : %li\n", retVal);
			}

			break;
		}
		case CHECK_POSSESSION:
		{
			printf("\nCHECK_POSSESSION started...\n");
			retVal=bmd_load_binary_content(dataFile, &dataBuf);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file\n", dataFile);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -15;
			}
			
			assert(bmdDVCS_load_data(dataBuf, &dvcsStruct) == 0);
			assert(bmdDVCS_set_server(dvcsStruct, dvcsAddress) == 0);
			if(connectionTimeout > 0)
			{
				bmdDVCS_set_connectionTimeout(dvcsStruct, connectionTimeout);
			}
			
			retVal=bmdDVCS_certify_possession_of_data(dvcsStruct, context);
			free_gen_buf(&dataBuf);
			bmdDVCS_get_errString(dvcsStruct, &errorString);
			printf("DVCS SERVER MESSAGE: [%s]\n", errorString);
			free(errorString);
			if(retVal < -1)
			{
				printf("ERROR:\tOperation status (bmdDVCS_certify_possession_of_data()) : %li\n", retVal);
			}
			else
			{
				printf("SUCCESS:\tOperation status (bmdDVCS_certify_possession_of_data()) : %li\n", retVal);
			}
		
			break;
		}
		case CHECK_EXISTENCE:
		{
			printf("\nCHECK_EXISTENCE started...\n");
			retVal=bmd_load_binary_content(dataFile, &dataBuf);
			if(retVal != 0)
			{
				printf("ERROR:\tUnable to load \"%s\" file\n", dataFile);
				bmd_ctx_destroy(&context);
				bmd_end();
				return -16;
			}
			
			assert(bmdDVCS_load_data(dataBuf, &dvcsStruct) == 0);
			assert(bmdDVCS_set_server(dvcsStruct, dvcsAddress) == 0);
			if(connectionTimeout > 0)
			{
				bmdDVCS_set_connectionTimeout(dvcsStruct, connectionTimeout);
			}
			
			retVal=bmdDVCS_certify_existence_of_data(dvcsStruct, context, BMDDVCS_COUNT_HASH);
			free_gen_buf(&dataBuf);
			bmdDVCS_get_errString(dvcsStruct, &errorString);
			printf("DVCS SERVER MESSAGE: [%s]\n", errorString);
			free(errorString);
			if(retVal < -1)
			{
				printf("ERROR:\tOperation status (bmdDVCS_certify_existence_of_data()) : %li\n", retVal);
			}
			else
			{
				printf("SUCCESS:\tOperation status (bmdDVCS_certify_existence_of_data()) : %li\n", retVal);
			}
		
			break;
		}
		default:
		{
			printf("ERROR:\tUnrecognized request type\n");
			bmd_ctx_destroy(&context);
			bmd_end();
			return -17;
		}
	}

	//zapis otrzymanego poswiadczenia
	if(responseDumpFile != NULL)
	{
		assert(bmdDVCS_get_dvcsCert(dvcsStruct, &dvcsCert) == 0);
		retVal=bmd_save_buf(dvcsCert, responseDumpFile);
		if(retVal != 0)
		{
			printf("ERROR:\tUnable to save \"%s\" file\n", responseDumpFile);
		}
		else
		{
			printf("SUCCESS:\tDVCS certificate saved in \"%s\" file\n", responseDumpFile);
		}
		free_gen_buf(&dvcsCert);
	}

	bmdDVCS_destroy(&dvcsStruct);
	bmd_ctx_destroy(&context);
	bmd_end();

	free(dvcsAddress);
	free(pfxFile);
	free(pfxPass);
	free(requestDumpFile);
	free(responseDumpFile);
	free(certFile);
	free(sigFile);
	free(dataFile);
	free(dataSHA1File);
	free(tsrTime);
	free(tsrToken);
	free(tsrAttachment1);
	free(tsrAttachment2);

printf("\n====================================================\n");
	return 0;
}
