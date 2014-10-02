/* Test bmdDatagram insert */

#include <libxml/parser.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>

#include <bmd/libbmdpkcs15/libbmdpkcs15.h>
#include <bmd/libbmdutils/libbmdutils.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd//libbmdpkcs12/libbmdpkcs12.h>

#include <pthread.h>

#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdaa/wrappers/AC_OBJECT_IDENTIFIER.h>

#include <bmd/libbmdaa/wrappers/AC_Certificate.h>


#include <bmd/libbmdxml/libbmdxml.h>

#define OID_CMS_ID_DATA			1,2,840,113549,1,7,1


int main()
{
int err=0;
unsigned long int 	oid_additional_metadata_regon[] 	= {1,2,616,1,113527,4,3,3,1};
unsigned long int 	oid_additional_metadata_nip[] 		= {1,2,616,1,113527,4,3,3,2};
unsigned long int 	oid_additional_metadata_undefined[]	= {1,2,616,1,113527,4,3,3,0};
unsigned long int 	oid_user_metadata_keyword[]		= {OID_USR_METADATA_KEYWORDS};
unsigned long int 	oid_user_metadata_data_description[]	= {OID_USR_METADATA_DATA_DESCRIPTION};
unsigned long int 	oid_user_metadata_software_types[]	= {OID_USR_METADATA_SOFTWARE};
unsigned long int 	oid_user_metadata_doc_types[]		= {OID_USR_METADATA_DOCUMENT_TYPE};
unsigned long int 	oid_sys_metadata_groups[]            	= {OID_RFC3281_GROUP};
unsigned long int 	oid_sys_metadata_sec_categories[]    	= {OID_SYS_METADATA_SEC_CAT};
unsigned long int 	oid_sys_metadata_sec_levels[]        	= {OID_SYS_METADATA_BMD_SEC_LEVELS};
unsigned long int 	oid_pki_metadata_timestamp[]        	= {1,2,616,1,113527,4,4,2};
xmlNodePtr xml;

xmlDocPtr doc;
bmdDatagram_t dg;
bmdDatagramSet_t dgs;
xmlChar *xmlbuff;
int buffersize;
int i;



	PR_bmdDatagram_init(&dg);
	dg.datagramType = BMD_DATAGRAM_TYPE_INSERT;

	/* zawartosc pliku */
	dg.protocolData = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg.protocolData->buf= (char *) malloc (sizeof(char) * 5);
	memset(dg.protocolData->buf, 0, 5);
	strcpy(dg.protocolData->buf, "dund");
	dg.protocolData->size=5;

	/* nazwa pliku */
	dg.protocolDataFilename = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg.protocolDataFilename->buf= (char *) malloc (sizeof(char) * 9);
	memset(dg.protocolDataFilename->buf, 0, 9);
	strcpy(dg.protocolDataFilename->buf, "dund.txt");
	dg.protocolDataFilename->size=9;

	/* owner pliku */
	dg.protocolDataOwner = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	dg.protocolDataOwner->buf= (char *) malloc (sizeof(char) * 9);
	memset(dg.protocolDataOwner->buf, 0, 4);
	strcpy(dg.protocolDataOwner->buf, "szu");
	dg.protocolDataOwner->size=4;



#ifndef aaa


	/* jakas dziwny oid */
	PR_bmdDatagram_add_metadata(oid_additional_metadata_regon,
				sizeof(oid_additional_metadata_regon),
				"regon2",
				1+strlen("regon1"),
				ADDITIONAL_METADATA,
				&dg);
	PR_bmdDatagram_add_metadata(oid_additional_metadata_nip,
				sizeof(oid_additional_metadata_nip),
				"nip2",
				1+strlen("nip1"),
				ADDITIONAL_METADATA,
				&dg);
	PR_bmdDatagram_add_metadata(oid_additional_metadata_undefined,
				sizeof(oid_additional_metadata_undefined),
				"undefined2",
				1+strlen("undefined1"),
				ADDITIONAL_METADATA,
				&dg);
	/* keyword 1*/
	PR_bmdDatagram_add_metadata(oid_user_metadata_keyword,
				sizeof(oid_user_metadata_keyword),
				"slowo kluczowe 1",
				1+strlen("slowo kluczowe 1"),
				USER_METADATA,
				&dg);
	/* description 1*/
	PR_bmdDatagram_add_metadata(oid_user_metadata_data_description,
				sizeof(oid_user_metadata_data_description),
				"opis 1",
				1+strlen("opis 1"),
				USER_METADATA,
				&dg);
	/* software type 1*/
	PR_bmdDatagram_add_metadata(oid_user_metadata_software_types,
				sizeof(oid_user_metadata_software_types),
				"1.2.616.1.113527.4.3.1.5.3",
				1+strlen("1.2.616.1.113527.4.3.1.5.3"),
				USER_METADATA,
				&dg);
	/* doc type 1*/
	PR_bmdDatagram_add_metadata(oid_user_metadata_doc_types,
				sizeof(oid_user_metadata_doc_types),
				"1.2.616.1.113527.4.3.1.4.1",
				1+strlen("1.2.616.1.113527.4.3.1.4.1"),
				USER_METADATA,
				&dg);
	/* group 1*/
	PR_bmdDatagram_add_metadata(oid_sys_metadata_groups,
				sizeof(oid_sys_metadata_groups),
				"1.2.616.1.113527.4.3.2.3.1",
				1+strlen("1.2.616.1.113527.4.3.2.3.1"),
				SYS_METADATA,
				&dg);
	PR_bmdDatagram_add_metadata(oid_sys_metadata_sec_categories,
				sizeof(oid_sys_metadata_sec_categories),
				"1.2.616.1.113527.4.3.2.1.1",
				1+strlen("1.2.616.1.113527.4.3.2.1.1"),
				SYS_METADATA,
				&dg);
	PR_bmdDatagram_add_metadata(oid_sys_metadata_sec_levels,
				sizeof(oid_sys_metadata_sec_levels),
				"1.2.616.1.113527.4.3.2.4.2",
				1+strlen("1.2.616.1.113527.4.3.2.4.2"),
				SYS_METADATA,
				&dg);
	PR_bmdDatagram_add_metadata(oid_pki_metadata_timestamp,
				sizeof(oid_pki_metadata_timestamp),
				"timestamp 1",
				1+strlen("timestamp 1"),
				PKI_METADATA,
				&dg);

	/*PR_bmdDatagram_print(&dg);*/





	doc = xmlNewDoc("1.0");
	bmdxml_init("oid.txt");
	dgs.bmdDatagramSetSize=100000;
	dgs.bmdDatagramSetTable = (bmdDatagram_t **) bmd_new( sizeof(bmdDatagram_t *)*dgs.bmdDatagramSetSize,0);
	for (i=0; i<dgs.bmdDatagramSetSize; i++) {
	    dgs.bmdDatagramSetTable[i] = &dg;
	}

	xml = bmd_datagramset2xml(&dgs, WITHOUT_BASE64);
	bmdxml_destroy();
	xmlDocSetRootElement(doc,xml);
        //xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	xmlFreeDoc(doc);
        //printf((char *) xmlbuff);
        //xmlFree(xmlbuff);
	bmd_free(dgs.bmdDatagramSetTable);


#endif /*aaa*/


	PR_bmdDatagram_free(&dg,0);

	return err;
}

