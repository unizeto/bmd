#define _WINSOCK2API_
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include"../../klient/vs2005/klient/VersionNo.h"
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

long PR_bmdDatagram_PrepareResponse(bmdDatagram_t **bmdDatagram, long dtg_type, long status)
{

	PRINT_INFO("LIBBMDPRINF Preparing datagram response\n");
	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if (bmdDatagram==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*bmdDatagram==NULL)				{       BMD_FOK(BMD_ERR_PARAM1);        }

	(*bmdDatagram)->datagramStatus=(long )status;
	(*bmdDatagram)->datagramType=dtg_type;
	(*bmdDatagram)->protocolData=NULL;
	(*bmdDatagram)->protocolDataFilename=NULL;
	(*bmdDatagram)->protocolDataFileLocationId=NULL;
	(*bmdDatagram)->protocolDataFileId=NULL;
	(*bmdDatagram)->protocolDataOwner=NULL;

	return BMD_OK;
}

long PR_bmdDatagram_init(bmdDatagram_t *bmdDatagram)
{
	if (bmdDatagram==NULL)                          {       BMD_FOK(BMD_ERR_PARAM1);        }

	memset(bmdDatagram, 0, sizeof(bmdDatagram_t));
	bmdDatagram->protocolVersion = BMD_PROTOCOL_VERSION;	/* NOWA WERSJA PROTOKOLU 1.03.00.00 bmd-const.h*/
	return BMD_OK;
}

/* to sa funkcje szu, ktore najlepiej jak szu ewentualnie pozmienia */
long PR_bmdDatagram_print(bmdDatagram_t *bmdDatagram, WithTime_t WithTime, FILE *fd)
{
	if (bmdDatagram==NULL)                          {       BMD_FOK(BMD_ERR_PARAM1);        }

	fprintf(fd,"\n");
	fprintf(fd,"\tProtocol Version: \t%li\n", bmdDatagram->protocolVersion);
	fprintf(fd,"\tDatagram Type: \t\t%li\n\n", bmdDatagram->datagramType);

	fprintf(fd,"\tAction metadata list: \n");
	fprintf(fd,"\t*******************\n");
	if(bmdDatagram->no_of_actionMetaData == 0)
	{
		fprintf(fd,"\t\t<none>\n\n");
	}
	else
	{

		PR_bmdDatagram_metadata_print(bmdDatagram->actionMetaData, bmdDatagram->no_of_actionMetaData, WithTime, fd);

		fprintf(fd,"\n");
	}

	fprintf(fd,"\tSystem metadata list:\n");
	fprintf(fd,"\t*********************\n");
	if(bmdDatagram->no_of_sysMetaData == 0)
	{
		fprintf(fd,"\t\t<none>\n\n");
	}
	else
	{

		PR_bmdDatagram_metadata_print(bmdDatagram->sysMetaData, bmdDatagram->no_of_sysMetaData, WithTime, fd);

		fprintf(fd,"\n");
	}

	fprintf(fd,"\tPKI metadata list:\n");
	fprintf(fd,"\t******************\n");
	if(bmdDatagram->no_of_pkiMetaData == 0)
	{
		fprintf(fd,"\t\t<none>\n\n");
	}
	else
	{

		PR_bmdDatagram_metadata_print(bmdDatagram->pkiMetaData, bmdDatagram->no_of_pkiMetaData, WithTime, fd);
		fprintf(fd,"\n");
	}

	fprintf(fd,"\tAdditional metadata list:\n");
	fprintf(fd,"\t*************************\n");
	if(bmdDatagram->no_of_additionalMetaData == 0)
	{
		fprintf(fd,"\t\t<none>\n\n");
	}
	else
	{

		PR_bmdDatagram_metadata_print(bmdDatagram->additionalMetaData, bmdDatagram->no_of_additionalMetaData, WithTime, fd);

		fprintf(fd,"\n");
	}


	fprintf(fd,"\tProtocol Data:");
	if(bmdDatagram->protocolData == NULL)
	{

		fprintf(fd,"\t<none>\n");
	}
	else
	{

		fprintf(fd, "\t%s\n", bmdDatagram->protocolData->buf);
	}

	fprintf(fd,"\tProtocol Data Filename:");
	if(bmdDatagram->protocolDataFilename == NULL)
	{

		fprintf(fd,"\t<none>\n");
	}
	else
	{

		fprintf(fd, "\t%s\n", bmdDatagram->protocolDataFilename->buf);
	}

	fprintf(fd,"\tProtocol Data FileLocationId:");
	if(bmdDatagram->protocolDataFileLocationId == NULL)
	{

		fprintf(fd,"\t<none>\n");
	}
	else
	{

		fprintf(fd, "\t%s\n",bmdDatagram->protocolDataFileLocationId->buf);
	}

	fprintf(fd,"\tProtocol Data FileId:");
	if(bmdDatagram->protocolDataFileId == NULL)
	{

		fprintf(fd,"\t<none>\n");
	}
	else
	{

		fprintf(fd, "\t%s\n",bmdDatagram->protocolDataFileId->buf);
	}

	fprintf(fd,"\tProtocol Data Owner:");
	if(bmdDatagram->protocolDataOwner == NULL)
	{

		fprintf(fd,"\t<none>\n");
	}
	else
	{

		fprintf(fd, "\t%s\n",bmdDatagram->protocolDataOwner->buf);
	}

	fprintf(fd,"\tRandom ID: \t\t%li\n", bmdDatagram->randId);

	fprintf(fd,"\tFiles Remaining: \t%li\n", bmdDatagram->filesRemaining);

	fprintf(fd,"\tDatagram Status: \t%li\n", bmdDatagram->datagramStatus);

	fprintf(fd,"**********************************************************************************************************************************\n\n\n");

	return BMD_OK;
}


long PR_bmdDatagram_metadata_print(	MetaDataBuf_t **MetaDataArray,
						long no_of_metadata,
						WithTime_t WithTime,
						FILE *fd)
{
long i				= 0;
char *value_temp		= NULL;

	if (MetaDataArray==NULL)                          {       BMD_FOK(BMD_ERR_PARAM1);        }

	for (i=0; i<no_of_metadata; i++)
	{
		value_temp = malloc( (MetaDataArray[i]->AnySize + 1) * sizeof(char));
		if(value_temp == NULL)			{	BMD_FOK(NO_MEMORY);	}
		memset(value_temp, 0, (MetaDataArray[i]->AnySize + 1) * sizeof(char));

		memcpy(value_temp, MetaDataArray[i]->AnyBuf, MetaDataArray[i]->AnySize);

		fprintf(fd,"%s", MetaDataArray[i]->OIDTableBuf);

		if ( MetaDataArray[i]->AnySize > 0 && MetaDataArray[i] != NULL )
		{

			fprintf(fd,"\t\t%30s\t%li\t%li\t%li\n", value_temp,  MetaDataArray[i]->myId, MetaDataArray[i]->ownerType,  MetaDataArray[i]->ownerId);

		}
		else
		{

			fprintf(fd,"\n");
		}

		free(value_temp); value_temp = NULL;
	}

	return 0;
}
