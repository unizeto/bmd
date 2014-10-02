/* test operacji update */

#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdsql/proofs.h>
#include<bmd/libbmdsql/dict.h>
#include<bmd/libbmdutils/libbmdutils.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdpkcs12/libbmdpkcs12.h>
#include<bmd/libbmdsql/js.h>
#include<bmd/libbmd/libbmd.h>

extern long _GLOBAL_debug_level;

int main()
{
	int err=0,status=0;
	unsigned long int 	oid_sys_metadata_groups[]={OID_RFC3281_GROUP};
	unsigned long int role_OID[]={OID_RFC3281_ROLE};
	char *oid_additional_metadata_nazwa_str	= "1.2.616.1.113527.4.3.3.6";
	void *db_conn_info			= NULL;
	GenBuf_t tmp_buf;
	bmdDatagram_t *dg1=NULL;
	bmdDatagramSet_t *set1=NULL,*set2=NULL;
	char *server_info_postgres =	"hostaddr='127.0.0.1' dbname='soe_1.3' port='5432' user='js'";
	//char *server_info_oracle = 	"(DESCRIPTION = (ADDRESS_LIST = (ADDRESS = (PROTOCOL = TCP)(HOST = 127.0.0.1)(PORT = 1521)))"
	//			"(CONNECT_DATA = (SID = BMD) (SERVER = DEDICATED)))";
	char *server_info = server_info_postgres;

	long id=94; /* na sztywno wpisane ID pliku */

	/* ustawienie roli */
	memset(&tmp_buf,0,sizeof(tmp_buf));
	tmp_buf.buf="administrator";
	tmp_buf.size=strlen("administrator");

	/* wiekszy DEBUG */
	_GLOBAL_debug_level=0;

	printf("Zrzucam slowniki... ");
	err=dump_all_dictionaries(server_info);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Err=%i\n", __FILE__, __LINE__, __FUNCTION__, err);
		return(err);
	}
	printf("[done]\n");
	bmd_db_connect(server_info, &db_conn_info);

	/* utworzenie datagramu */
	status=bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE,&dg1);
	if(status!=BMD_OK) { PRINT_DEBUG("Error in creating datagram %i\n",status); return status; }

	/* id pliku */
	status=bmd_datagram_set_id(&id,&dg1);
	if(status!=BMD_OK) { PRINT_DEBUG("Error in setting id %i\n",status); return status; }

	/* owner pliku */
	dg1->protocolDataOwner = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg1->protocolDataOwner->buf= (char *) malloc (sizeof(char) * 9);
	memset(dg1->protocolDataOwner->buf, 0, 4);
	strcpy(dg1->protocolDataOwner->buf, "szu");
	dg1->protocolDataOwner->size=4;

	/* metdane systemowe */
	PR2_bmdDatagram_add_metadata(oid_sys_metadata_groups,
								 "1.2.616.1.113527.4.3.2.3.111",1+strlen("1.2.616.1.113527.4.3.2.3.111"),
								 SYS_METADATA,dg1,0,0,0);
	/* metadane dodatkowe */
	PR_bmdDatagram_add_metadata_sequence2(oid_additional_metadata_nazwa_str,
										  "","bo chce by byla pusta",ADDITIONAL_METADATA,dg1,0,0,0);
	/* dodanie roli */
	status=PR2_bmdDatagram_add_metadata(role_OID,tmp_buf.buf,tmp_buf.size,SYS_METADATA,dg1,0,0,0);
	if(status<0)
	{
		printf("nie dodano roli do testowego datagramu. status == %i\n",status);
		return status;
	}

	err=bmd_datagram_add_to_set(dg1,&set1);
	if(err<0){ printf("Error in adding into set\n"); return err; }

	err = JS_main(db_conn_info, set1, &set2, NULL, NULL, NULL,NULL);
	printf("JS_main status %i\n",err);
	if(err<0)
		return err;

	PR2_bmdDatagramSet_free(&set1);PR2_bmdDatagramSet_free(&set2);
	bmd_db_disconnect(&db_conn_info);
	return err;
}

