#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>

long bmd2_datagram_del_symkey(	bmdDatagram_t *dtg)
{
long i					= 0;
long mtd_idx				= -1;
long no_of_metadata		= 0;
MetaDataBuf_t **metadata	= NULL;
char symkey_oid[]= {OID_SYS_METADATA_DTG_SYMKEY};

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(dtg==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}

	metadata=dtg->sysMetaData;
	no_of_metadata=dtg->no_of_sysMetaData;
	if(metadata)
	{
		for(i=0;i<no_of_metadata;i++)
		{
			if(strcmp(symkey_oid,metadata[i]->OIDTableBuf)==0)
			{
				mtd_idx=i;
				break; /* znaleziono metadana systemowa bedaca kluczem symetrycznym */
			}
		}
	}
	else
	{
		BMD_FOK(BMD_ERR_NODATA);
	}
	if(mtd_idx==-1)
	{
		BMD_FOK(BMD_ERR_NODATA);
	}

	/* skasuj klucz symetryczny z listy metadanych */
	free(dtg->sysMetaData[mtd_idx]);dtg->sysMetaData[mtd_idx]=NULL;
	for(i=mtd_idx;i<dtg->no_of_sysMetaData-1;i++)
	{
		dtg->sysMetaData[i]=dtg->sysMetaData[i+1];
	}
	dtg->sysMetaData[i]=NULL;
	dtg->no_of_sysMetaData--;

	return BMD_OK;
}
