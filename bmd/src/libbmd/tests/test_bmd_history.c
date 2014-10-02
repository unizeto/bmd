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
	char *sort_str		 =NULL;

	long int i=0;
	long int id[10];
	int k			= 0;

	_GLOBAL_debug_level=0;

	FOK(bmd_init());

	FOK(bmd_info_create(&bi));

	/*FOK(bmd_info_login_set("192.168.129.145",6783,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));
	FOK(bmd_info_set_credentials_pfx("log_soe_1_crl.p12","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("serwer_soe_1.der",&bi));*/

	FOK(bmd_info_login_set("127.0.0.1",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE,NULL, &bi));
	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));


	FOK(bmd_info_set_selected_role(&bi));

	/*wyszukiwanie listy plików wg kryteriów*/
	id[0]=3306;
	id[1]=3307;
	id[2]=3308;
	id[3]=3309;
	for (k=0; k<4; k++)
	{
		FOK(bmd_datagram_create(BMD_DATAGRAM_GET_METADATA_WITH_TIME,&dtg));
		FOK(bmd_datagram_set_id(&(id[k]),&dtg));
		FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	}

		FOK(bmd_send_request(bi,req_dtg_set, 100, &resp_dtg_set,0));

		FOK(bmd_info_get_form(BMD_FORM_SEARCH,bi,&form,&form_count));

// 		printf("\n\n%i\n\n",resp_dtg_set->bmdDatagramSetSize);
		for (index=0; index<resp_dtg_set->bmdDatagramSetSize; index++)
		{
			printf("\n\n%li. %li\n",index , resp_dtg_set->bmdDatagramSetTable[index]->datagramStatus);
			if (resp_dtg_set->bmdDatagramSetTable[index]->datagramStatus==BMD_OK)
			{
				printf("fileid: %s\n",resp_dtg_set->bmdDatagramSetTable[index]->protocolDataFileId->buf);
				FOK(bmd_datagramset_get_datagram(resp_dtg_set,index,&tmp_dtg));
// 				FOK(bmd_datagram_get_filename(tmp_dtg,&filename));
// 				FOK(bmd_datagram_get_id(tmp_dtg,&tmps));
// 				printf("%i\t%s\t%li\n",index+1,filename,tmps);
				int i=0;
				for (i=0; i<form_count; i++)
				{
					FOK(bmd_attr_get_oid(i,form,&oid));
					FOK(bmd_attr_get_description(i,form,&desc));
					bmd_datagram_get_metadata_2(oid,tmp_dtg,&mtd_buf);
					if (mtd_buf)
					{
						printf("\t\t%s\t\t\t%s\t\t\t%s\n",oid,desc,mtd_buf->buf);
					}
					else
					{
						printf("\t\t%s\t\t\t%s\n",oid,desc);
					}
					bmd_genbuf_free(&mtd_buf);
					free(oid); oid=NULL;
					free(desc); desc=NULL;
				}

				free(filename);filename=NULL;
				free(tmp_dtg); tmp_dtg=NULL;
			}
			else
			{
				printf("\n\nBlad: %li\n\n",resp_dtg_set->bmdDatagramSetTable[index]->datagramStatus);
			}
 		}

		printf("\n\n");
		/*FOK(bmd_datagramset_free(&req_dtg_set));
		FOK(bmd_datagramset_free(&resp_dtg_set));*/

	FOK(bmd_logout(&bi));

	FOK(bmd_end());

	return 0;
}
