#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdts/libbmdts.h>
//#include <conf_sections.h>
#include <stdlib.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

extern long _GLOBAL_debug_level;
int main(int argc, char ** argv)
{
bmd_info_t *bi=NULL;
bmdDatagram_t *dtg=NULL,*tmp_dtg=NULL;
bmdDatagramSet_t *req_dtg_set=NULL;
bmdDatagramSet_t *resp_dtg_set=NULL;
lob_request_info_t *li=NULL;
long int index=0;
long int tmps=0;
char *filename=NULL;
GenBuf_t tmp_gb;

BMD_attr_t **form=NULL;
int form_count;
GenBuf_t *mtd_buf=NULL;
char *oid=NULL,*desc=NULL;

unsigned long int timestamp[]	= {OID_PKI_METADATA_TIMESTAMP};
unsigned long int signature[]	= {OID_PKI_METADATA_SIGNATURE};
unsigned long int dvcs[]	= {OID_PKI_METADATA_DVCS};
unsigned long int tim_sig[]	= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};
unsigned long int tim_dvcs[]	= {OID_PKI_METADATA_TIMESTAMP_FROM_DVCS};


long int i=0;
long int j=0;
long int k=0;
long int id=0;

char *host=NULL;
char *pfx=NULL;
char * pfx_pass=NULL;
char *cer=NULL;
int port=0;


	_GLOBAL_debug_level=0;
	if (argc==15)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-h")==0)
			{
				if (argc>i+1) asprintf(&host,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-pr")==0)
			{
				if (argc>i+1) port=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-pfx")==0)
			{
				if (argc>i+1) asprintf(&pfx,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-pfx_p")==0)
			{
				if (argc>i+1) asprintf(&pfx_pass,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-c")==0)
			{
				if (argc>i+1) asprintf(&cer,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0)
			{
				if (argc>i+1) id=atoi(argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-h adres\thost BMD\n");
		printf("\t-pr liczba\tport hosta BMD\n");
		printf("\t-pfx plik .pfx\tcertyfikat logowania\n");
		printf("\t-pfx_p hasło\thasło do certyfikatu logowania\n");
		printf("\t-c plik .cer\tklucz publiczny serwera\n");
		printf("\t-p liczba\tid pliku ktorego prfy chcemy pobrac\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_login_set(host,port,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE,NULL, &bi));
	FOK(bmd_info_set_credentials_pfx(pfx,pfx_pass,strlen(pfx_pass),&bi));
	FOK(bmd_info_set_bmd_cert(cer,&bi));
	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));
	printf("getting props file nr %li... ",id);
	FOK(bmd_datagram_create(BMD_DATAGRAM_GET_METADATA,&dtg));
	FOK(bmd_datagram_set_id(&id,&dtg));
	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	FOK(bmd_send_request(bi,req_dtg_set, 100, &resp_dtg_set,0));
	printf("done with status = %li \n",resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/********************************/
	/*	wyswietlenie wynikow	*/
	/********************************/
	if (resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus==BMD_OK)
	{
		printf("plik:\n%s\n",resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
		for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
		{
			if (OID_cmp2(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableSize,
					timestamp,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableSize)==0)
			{
				printf("\ttimestamp: %li %li\n",resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId,
								resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->ownerId);
			}
		}

		for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
		{
			if (OID_cmp2(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableSize,
					signature,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableSize)==0)
			{
				printf("\tsignature: %li %li\n",resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId,
								resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->ownerId);

				printf("\n");

				for (j=0; j<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; j++)
				{
					if ((OID_cmp2(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,
							resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableSize,
							tim_sig,
							resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableSize)==0) &&
					   (resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
					{
						printf("\t\ttimestamp: %li %li\n",resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId,
										resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId);

					}

					printf("\n");

					if ((OID_cmp2(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,
							resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableSize,
							dvcs,
							resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableSize)==0) &&
					   (resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
					{
						printf("\t\tdvcs: %li %li\n",resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId,
										resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId);


						printf("\n");

						for (k=0; k<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; k++)
						{
							if ((OID_cmp2(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableBuf,
									resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableSize,
									tim_dvcs,
									resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableSize)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId))
							{
								printf("\t\t\ttimestamp: %li %li\n",resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->myId,
												resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId);

							}
						}
					}
				}
			}
		}
	}


	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_end());
	return 0;
}
