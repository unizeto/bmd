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


	long int i=0;
	long int id=0;

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
		FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_SELECT_WITH_OR,&dtg));
		for(i=0;i<4;i++)
		{
			switch(i)
			{
				case 0:
					tmp_gb.buf=(unsigned char*)"ws";
					tmp_gb.size=strlen(tmp_gb.buf);
					FOK(bmd_datagram_add_metadata_2("1.2.616.1.113527.4.3.2.5.2",&tmp_gb,&dtg));
					break;
				case 1:
					tmp_gb.buf=(unsigned char*)"25";
					tmp_gb.size=strlen(tmp_gb.buf);
					FOK(bmd_datagram_add_metadata_2("1.2.616.1.113527.4.3.3.1",&tmp_gb,&dtg));
					break;
				case 2:
					tmp_gb.buf=(unsigned char*)"12354";
					tmp_gb.size=strlen(tmp_gb.buf);
					FOK(bmd_datagram_add_metadata_2("1.2.616.1.113527.4.3.3.2",&tmp_gb,&dtg));
					break;
				case 3:
					tmp_gb.buf=(unsigned char*)"costam";
					tmp_gb.size=strlen(tmp_gb.buf);
					FOK(bmd_datagram_add_metadata_2("1.2.616.1.113527.4.3.3.3",&tmp_gb,&dtg));
					break;
			}
		}

		FOK(bmd_datagram_set_status(BMD_ONLY_GROUP_DOC,BMD_QUERY_ILIKE,&dtg));
		FOK(bmd_datagram_add_limits(0,10,&dtg));
		FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
		FOK(bmd_send_request(bi,req_dtg_set, 100, &resp_dtg_set,0));

		FOK(bmd_info_get_form(BMD_FORM_SEARCH,bi,&form,&form_count));

		printf("\n\n%i\n\n",resp_dtg_set->bmdDatagramSetSize);
		for (index=0; index<resp_dtg_set->bmdDatagramSetSize; index++)
		{
			FOK(bmd_datagramset_get_datagram(resp_dtg_set,index,&tmp_dtg));
			FOK(bmd_datagram_get_filename(tmp_dtg,&filename));
			FOK(bmd_datagram_get_id(tmp_dtg,&tmps));
			printf("%i\t%s\t%li\n",index+1,filename,tmps);
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

		printf("\n\n");
		/*FOK(bmd_datagramset_free(&req_dtg_set));
		FOK(bmd_datagramset_free(&resp_dtg_set));*/

	FOK(bmd_logout(&bi));

	FOK(bmd_end());

	return 0;
}
