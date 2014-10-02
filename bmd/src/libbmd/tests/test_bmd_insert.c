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
	PRINT_PANIC("START\n");
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL,*tmp_dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	lob_request_info_t *li=NULL;
	long int index=0;
	long int tmps=0;
	char *filename=NULL;
	GenBuf_t tmp_gb,tmp_gb1,*cert_pem;
	GenBuf_t *tmp_gb2=NULL;
	GenBuf_t *plik=NULL;
	GenBuf_t *timestamp_hash_buf= NULL;


	int status=0;
	BMD_attr_t **form=NULL;
	int form_count;
	GenBuf_t *mtd_buf=NULL;
	char *oid=NULL,*desc=NULL;
	bmd_crypt_ctx_t *timestamp_hash_ctx= NULL;


	long int i=0;
	long int id=0;
	unsigned long int dvcs_oid[]			= {OID_PKI_METADATA_DVCS};

	_GLOBAL_debug_level=3;

	FOK(bmd_init());

	FOK(bmd_info_create(&bi));

	FOK(bmd_info_login_set("127.0.0.1",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi));
	//FOK(bmd_info_login_set("192.168.156.34",443,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi));

	/*FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));*/
	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));


	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));


	/*wprowadzanie danych*/

	//for (i=0; i<=3; i++)
	//{
	//	int j=0;
	//	printf("pliki \t%li - %li... ",i*500,(i+1)*500);
	//	for (j=0; j<=500; j++)
	//	{
			int u=0;
			int p=0;

			FOK(bmd_lob_request_info_create(&li));

			/*status=bmd_load_binary_content("/home/Lesioo/Lesioo/trunk/src/console_client/fak.pdf",&plik);
			if (status<BMD_OK)
			{
				printf("****************************** %i\n",__LINE__);
				return status;
			}*/
			for (u=0; u<1; u++)
			{


				FOK(bmd_lob_request_info_add_filename(L"/home/Lesioo/Lesioo/trunk/src/libbmd/tests/cos.doc",&li, BMD_SEND_LOB,0));



					FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg));
					//FOK(bmd_load_binary_content("dump.pkl",&tmp_gb2));
					//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);
					/*printf("\n\n");
					for (i=0; i<tmp_gb2->size; i++)
						printf("%x",tmp_gb2->buf[i]);
					printf("\n\n");*/
					//tmp_gb.buf=(unsigned char*)input->file->__ptr;
					//tmp_gb.size=input->file->__size;

					//FOK(bmd_datagram_add_data(tmp_gb2,"homie.pkl",&dtg));
					//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);

					FOK(bmd_datagram_set_filename(L"cos.doc",&dtg));



					for (i=0; i<1; i++)
					{
						switch (i)
						{
							case 0:
								tmp_gb.buf="1.2.616.1.113527.4.3.3.8";
								tmp_gb.size=strlen(tmp_gb.buf);
								if (u>=0)
								{
									tmp_gb1.buf="pawel.lesniak@unizeto.pl;radoslaw.olejnik@unizeto.pl";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								else
								{
									tmp_gb1.buf="2007-10-09 - dupa";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								break;
							case 1:
								tmp_gb.buf="1.2.616.1.113527.4.3.3.5";
								tmp_gb.size=strlen(tmp_gb.buf);
								if (u==1)
								{
									tmp_gb1.buf="Lesioo";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								else
								{
									tmp_gb1.buf="Pawel";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								break;
							case 2:
								tmp_gb.buf="1.2.616.1.113527.4.3.3.8";
								tmp_gb.size=strlen(tmp_gb.buf);
								if (u==1)
								{
									tmp_gb1.buf="plesniak1@unet.pl";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								else
								{
									tmp_gb1.buf="plesniak2@unet.pl";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								break;
							case 3:
								tmp_gb.buf="1.2.616.1.113527.4.3.3.6";
								tmp_gb.size=strlen(tmp_gb.buf);
								if (u<1)
								{
									tmp_gb1.buf="2007-10-09";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								else
								{
									tmp_gb1.buf="2007-10-09 - dupa";
									tmp_gb1.size=strlen(tmp_gb1.buf);
								}
								break;
						}

						FOK(bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg));


						/*

						{
							bmd_logout(&bi);
							bmd_end();
							PRINT_DEBUG("TESTBMDERR[%s:%i:%s] blad dodania metadanej do datagramu\n",__FILE__,__LINE__,__FUNCTION__);
							return soap_receiver_fault(soap,"Wewnetrzny blad","Wystapil wewnetrzny blad nr 3");
						}*/
					}

					//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);
					FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));


			}
			//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);
	status=bmd_send_request_lo(bi,req_dtg_set, 100, &resp_dtg_set, li);

	printf("\n\n\n\nresponse\n------------------- %i\n",status);
	printf("%i\n",resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
	for (i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{
		printf("%i. %i %s\n",i,resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus,resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf);
	}


			//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);
			FOK(bmd_datagramset_free(&req_dtg_set));
			//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);
			bmd_datagramset_free(&resp_dtg_set);
			//printf("%s:%i:%s PKL\n",__FILE__,__LINE__,__FUNCTION__);

		//}
			printf(" wprowadzone.\n");

	//}
	/**************************************************/

	FOK(bmd_logout(&bi));
	FOK(bmd_end());

	PRINT_PANIC("STOP\n");
	return 0;
}
