#define _WINSOCK2API_
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4189)
#endif

#define _PR2_SYS_MTD	1
#define _PR2_ACT_MTD	2
#define _PR2_ADD_MTD	3
#define _PR2_PKI_MTD	4

/* inicjalizacja struktury BMDDatagram_t */
long _PR2_BMDDatagram_init(BMDDatagram_t **bmdDatagramStruct)
{
	if(bmdDatagramStruct==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(*(bmdDatagramStruct)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*bmdDatagramStruct)=(BMDDatagram_t *)malloc(sizeof(BMDDatagram_t));
	if((*bmdDatagramStruct) == NULL)	{	BMD_FOK(NO_MEMORY);		}
	memset(*bmdDatagramStruct,0,sizeof(BMDDatagram_t));

	return BMD_OK;
}

long _PR2_BMDDatagram_set_metadata(	MetaDataBuf_t **mtds,
						long no,
						long type,
						BMDDatagram_t **asn1_dtg)
{
long i				= 0;

long lenOfOids			= 0;
long lenOfMyIds			= 0;
long lenOfOwnerIds		= 0;
long lenOfOwnerTypes		= 0;
long lenOfDatas			= 0;

char **OID_str_array		= NULL;
char **myId_str_array		= NULL;
char **ownerId_str_array	= NULL;
char **ownerType_str_array	= NULL;

char *OIDs_str			= NULL;
char *myIds_str			= NULL;
char *ownerIds_str		= NULL;
char *ownerTypes_str		= NULL;
char *datas_str			= NULL;

char *pointer_temp		= NULL;
INTEGER_t *INTEGER_temp		= NULL;

struct MetaDatas *tmp_mtd	= NULL;


	if(mtds==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(no<0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	if( (type!=_PR2_SYS_MTD) && (type!=_PR2_ACT_MTD) &&
	    (type!=_PR2_ADD_MTD) && (type!=_PR2_PKI_MTD) )
	{
		BMD_FOK(BMD_ERR_PARAM3);
	}

	if(asn1_dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*asn1_dtg)==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}


	tmp_mtd=(struct MetaDatas *)malloc(sizeof(struct MetaDatas));
	if(tmp_mtd == NULL)		{       BMD_FOK(NO_MEMORY);	}
	memset(tmp_mtd,0,sizeof(struct MetaDatas));


	OID_str_array = (char **)malloc( (no + 1) * sizeof(char *));
	if(OID_str_array == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(OID_str_array, 0, (no + 1) * sizeof(char *));

	myId_str_array = (char **)malloc( (no + 1) * sizeof(char *));
	if(myId_str_array == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(myId_str_array, 0, (no + 1) * sizeof(char *));

	ownerId_str_array = (char **)malloc( (no + 1) * sizeof(char *));
	if(ownerId_str_array == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(ownerId_str_array, 0, (no + 1) * sizeof(char *));

	ownerType_str_array = (char **)malloc( (no + 1) * sizeof(char *));
	if(ownerType_str_array == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(ownerType_str_array, 0, (no + 1) * sizeof(char *));

	/*Konwersja na char* oraz zliczenie wielkosci potrzebnych buforow*/
	for(i=0;i<no;i++)
	{
		OID_str_array[i]=(char*)malloc(sizeof(char)*(strlen(mtds[i]->OIDTableBuf)+1));
		memset(OID_str_array[i], 0, strlen(mtds[i]->OIDTableBuf)+1);
		memcpy(OID_str_array[i], mtds[i]->OIDTableBuf, strlen(mtds[i]->OIDTableBuf));
// 		PRINT_TEST("LIBBMDPRVDEBUG Encoding loop i: %li, OID_str_array[i]: %s\n", i, OID_str_array[i]);
		lenOfOids = lenOfOids + (long)strlen(OID_str_array[i]) + 1;

		asprintf(&(myId_str_array[i]), "%li", mtds[i]->myId);
		if(myId_str_array[i] == NULL)		{	 BMD_FOK(NO_MEMORY);	}
// 		PRINT_TEST("LIBBMDPRVDEBUG Encoding loop i: %li, myId_str_array[i]: %s\n", i, myId_str_array[i]);
		lenOfMyIds = lenOfMyIds + (long)strlen(myId_str_array[i]) + 1;

		asprintf(&(ownerId_str_array[i]), "%li", mtds[i]->ownerId);
		if(ownerId_str_array[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
// 		PRINT_TEST("LIBBMDPRVDEBUG Encoding loop i: %li, ownerId_str_array[i]: %s\n", i, ownerId_str_array[i]);
		lenOfOwnerIds = lenOfOwnerIds + (long)strlen(ownerId_str_array[i]) + 1;

		asprintf(&(ownerType_str_array[i]), "%li", mtds[i]->ownerType);
		if(ownerType_str_array[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
// 		PRINT_TEST("LIBBMDPRVDEBUG Encoding loop i: %li, ownerType_str_array[i]: %s\n", i, ownerType_str_array[i]);
		lenOfOwnerTypes = lenOfOwnerTypes + (long)strlen(ownerType_str_array[i]) + 1;

		lenOfDatas = lenOfDatas + mtds[i]->AnySize + 1;
	}

	/*Po kolei tworze bufory przechowujace odpowiednie lancuchy i kopiuje do nich wczesniej pobrane dane*/
	OIDs_str = malloc(lenOfOids * sizeof(char));
	if(OIDs_str == NULL) {	BMD_FOK(NO_MEMORY);	}
	memset(OIDs_str, 0, lenOfOids * sizeof(char));
	pointer_temp = OIDs_str;
	for(i = 0; i < no; i++)
	{
		memcpy(pointer_temp, OID_str_array[i], strlen(OID_str_array[i]));
// 		PRINT_VDEBUG("LIBBMDPRVDEBUG Encoding loop i: %li, OID_str: %s\n", i, pointer_temp);
		pointer_temp = pointer_temp + strlen( OID_str_array[i] ) + 1;

		INTEGER_temp = (INTEGER_t*)malloc(sizeof(INTEGER_t));
		if(INTEGER_temp == NULL)        {       BMD_FOK(NO_MEMORY);     }
		memset(INTEGER_temp, 0, sizeof(INTEGER_t));
		BMD_FOK(asn_long2INTEGER(INTEGER_temp, (long)strlen( OID_str_array[i] )));
		asn_set_add( &(tmp_mtd->lenOfOids.list), INTEGER_temp);
		INTEGER_temp = NULL;
		free(OID_str_array[i]);
		OID_str_array[i] = NULL;
	}
	free(OID_str_array); OID_str_array = NULL;

	myIds_str = malloc(lenOfMyIds * sizeof(char));
	if(myIds_str == NULL)                            {       BMD_FOK(NO_MEMORY);     }
	memset(myIds_str, 0, lenOfMyIds * sizeof(char));
	pointer_temp = myIds_str;
	for(i = 0; i < no; i++)
	{
		memcpy(pointer_temp, myId_str_array[i], strlen(myId_str_array[i]));
// 		PRINT_VDEBUG("LIBBMDPRVDEBUG Encoding loop i: %li, myId_str: %s\n", i, pointer_temp);
		pointer_temp = pointer_temp + strlen(myId_str_array[i]) + 1;

		INTEGER_temp = (INTEGER_t*)malloc(sizeof(INTEGER_t));
		if(INTEGER_temp == NULL)        {       BMD_FOK(NO_MEMORY);     }
		memset(INTEGER_temp, 0, sizeof(INTEGER_t));
		BMD_FOK(asn_long2INTEGER(INTEGER_temp, (long)strlen(myId_str_array[i])));
		asn_set_add( &(tmp_mtd->lenOfMyIds.list), INTEGER_temp);
		INTEGER_temp = NULL;

		free(myId_str_array[i]);
		myId_str_array[i] = NULL;
	}
	free(myId_str_array); myId_str_array = NULL;


	ownerIds_str = malloc(lenOfOwnerIds * sizeof(char));
	if(ownerIds_str == NULL)			{	BMD_FOK(NO_MEMORY);	}
	memset(ownerIds_str, 0, lenOfOwnerIds * sizeof(char));
	pointer_temp = ownerIds_str;
	for(i = 0; i < no; i++)
	{
		memcpy(pointer_temp, ownerId_str_array[i], strlen(ownerId_str_array[i]));
// 		PRINT_VDEBUG("LIBBMDPRVDEBUG Encoding loop i: %li, ownerIds_str: %s\n", i, pointer_temp);
		pointer_temp = pointer_temp + strlen(ownerId_str_array[i]) + 1;

		INTEGER_temp = (INTEGER_t*)malloc(sizeof(INTEGER_t));
		if(INTEGER_temp == NULL)        {       BMD_FOK(NO_MEMORY);     }
		memset(INTEGER_temp, 0, sizeof(INTEGER_t));
		BMD_FOK(asn_long2INTEGER(INTEGER_temp, (long)strlen(ownerId_str_array[i])));
		asn_set_add( &(tmp_mtd->lenOfOwnerIds.list), INTEGER_temp);
		INTEGER_temp = NULL;

		free(ownerId_str_array[i]);
		ownerId_str_array[i] = NULL;
	}
	free(ownerId_str_array); ownerId_str_array = NULL;


	ownerTypes_str = malloc(lenOfOwnerTypes * sizeof(char));
	if(ownerTypes_str == NULL)			{	BMD_FOK(NO_MEMORY);	}
	memset(ownerTypes_str, 0, lenOfOwnerTypes * sizeof(char));
	pointer_temp = ownerTypes_str;
	for(i = 0; i < no; i++)
	{
		memcpy(pointer_temp, ownerType_str_array[i], strlen(ownerType_str_array[i]));
// 		PRINT_VDEBUG("LIBBMDPRVDEBUG Encoding loop i: %li, ownerType_str: %s\n", i, pointer_temp);
		pointer_temp = pointer_temp + strlen(ownerType_str_array[i]) + 1;

		INTEGER_temp = (INTEGER_t*)malloc(sizeof(INTEGER_t));
		if(INTEGER_temp == NULL)        {       BMD_FOK(NO_MEMORY);     }
		memset(INTEGER_temp, 0, sizeof(INTEGER_t));
		BMD_FOK(asn_long2INTEGER(INTEGER_temp, (long)strlen(ownerType_str_array[i])));
		asn_set_add( &(tmp_mtd->lenOfOwnerTypes.list), INTEGER_temp);
		INTEGER_temp = NULL;

		free(ownerType_str_array[i]);
		ownerType_str_array[i] = NULL;
	}
	free(ownerType_str_array); ownerType_str_array = NULL;


	datas_str = malloc(lenOfDatas * sizeof(char));
	if(datas_str == NULL)				{	BMD_FOK(NO_MEMORY);	}
	memset(datas_str, 0, lenOfDatas * sizeof(char));
	pointer_temp = datas_str;
	for(i = 0; i < no; i++)
	{
		memcpy(pointer_temp, mtds[i]->AnyBuf, mtds[i]->AnySize);
// PRINT_VDEBUG("LIBBMDPRVDEBUG Encoding loop i: %li, data string len: %li\n", i, mtds[i]->AnySize);
		pointer_temp = pointer_temp + mtds[i]->AnySize + 1;

		INTEGER_temp = (INTEGER_t*)malloc(sizeof(INTEGER_t));
		if(INTEGER_temp == NULL)        {       BMD_FOK(NO_MEMORY);     }
		memset(INTEGER_temp, 0, sizeof(INTEGER_t));
		BMD_FOK(asn_long2INTEGER(INTEGER_temp, mtds[i]->AnySize));
		asn_set_add( &(tmp_mtd->lenOfDatas.list), INTEGER_temp);
		INTEGER_temp = NULL;

        }


	BMD_FOK(OCTET_STRING_fromBuf( &(tmp_mtd->datas), datas_str, lenOfDatas));
	free(datas_str); datas_str = NULL;

	BMD_FOK(OCTET_STRING_fromBuf( &(tmp_mtd->oids), OIDs_str, lenOfOids));
	free(OIDs_str); OIDs_str = NULL;

	BMD_FOK(OCTET_STRING_fromBuf( &(tmp_mtd->myIds), myIds_str, lenOfMyIds));
	free(myIds_str); myIds_str = NULL;

	BMD_FOK(OCTET_STRING_fromBuf( &(tmp_mtd->ownerIds), ownerIds_str , lenOfOwnerIds));
	free(ownerIds_str); ownerIds_str = NULL;

	BMD_FOK(OCTET_STRING_fromBuf( &(tmp_mtd->ownerTypes), ownerTypes_str, lenOfOwnerTypes));
	free(ownerTypes_str); ownerTypes_str = NULL;

	switch(type)
	{
		case _PR2_SYS_MTD:
			(*asn1_dtg)->sysMetaDatas = tmp_mtd;
			break;
		case _PR2_ACT_MTD:
			(*asn1_dtg)->actionMetaDatas = tmp_mtd;
			break;
		case _PR2_ADD_MTD:
			(*asn1_dtg)->additionalMetaDatas = tmp_mtd;
			break;
		case _PR2_PKI_MTD:
			(*asn1_dtg)->pkiMetaDatas = tmp_mtd;
			break;
	};
	tmp_mtd = NULL; /*Nie zwalniac*/

	return BMD_OK;
}

long _PR2_BMDDatagram_der_encode(	BMDDatagram_t *asn1_dtg,
						GenBuf_t **output)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if(asn1_dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(asn1_encode(&asn_DEF_BMDDatagram,asn1_dtg, NULL, output));

	return BMD_OK;
}

long _PR2_BMDDatagram_der_decode(	GenBuf_t *input,
						BMDDatagram_t **asn1_dtg)
{
asn_dec_rval_t rc_code;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(asn1_dtg==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*asn1_dtg)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	rc_code=ber_decode(0, &asn_DEF_BMDDatagram, (void **)asn1_dtg, input->buf,input->size);
    	if(rc_code.code!=RC_OK)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return BMD_OK;
}

/*TODO Nie kasowac! Uzywam do testow nowego kodowania*/
/*
long twl_number = 0;
*/

long _PR2_bmdDatagram2BMDDatagram(	bmdDatagram_t *dtg,
					BMDDatagram_t **asn1_dtg)
{
/*TODO Nie kasowac! Uzywam do testow nowego kodowania*/
/*
char *twl_buffer		= NULL;
long twl_buffer_size		= 0;
long twl_metadata_string_len	= 0;
long twl_deserialised_len	= 0;
bmdDatagram_t *twl_dtg		= NULL;
char *twl_file			= NULL;
FILE *fd			= NULL;
*/
	PRINT_INFO("LIBBMDPRINF Adding datagram to der datagram.\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(dtg == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}


	/*TODO Nie kasowac! Uzywam do testow nowego kodowania*/
	/*
	asprintf(&twl_file, "/tmp/twl_output_serialised_%li.%li.dtg", (long)getpid(), twl_number);
	fd = fopen(twl_file, "a+");
	if (fd > 0)
	{
		PR_bmdDatagram_print(dtg, WITH_TIME, fd);
		fclose(fd);
	}

	BMD_FOK(PR2_bmdDatagram_serialize_count_bytes_TW( dtg, &twl_buffer_size, &twl_metadata_string_len));
	twl_buffer = malloc(twl_buffer_size * sizeof(char));
	if(twl_buffer == NULL)		{	BMD_FOK(NO_MEMORY);	}
	memset(twl_buffer, 0, twl_buffer_size * sizeof(char));

	BMD_FOK(PR2_bmdDatagram_serialize_TW(	dtg,
						twl_buffer,
						twl_buffer_size * sizeof(char),
						twl_metadata_string_len));

	BMD_FOK(PR2_bmdDatagram_deserialize_TW(twl_buffer, &twl_dtg, &twl_deserialised_len));
	free(twl_buffer); twl_buffer = NULL;
	asprintf(&twl_file, "/tmp/twl_output_deserialised_%li.%li.dtg", (long)getpid(), twl_number);
	fd = fopen(twl_file, "a+");
	if (fd > 0)
	{
		PR_bmdDatagram_print(twl_dtg, WITH_TIME, fd);
		fclose(fd);
	}
	twl_number++;
	*/


	BMD_FOK(_PR2_BMDDatagram_init(asn1_dtg));

	BMD_FOK(asn_long2INTEGER(&((*asn1_dtg)->protocolVersion),dtg->protocolVersion));

	BMD_FOK(asn_long2INTEGER(&((*asn1_dtg)->datagramType),dtg->datagramType));

	BMD_FOK(asn_long2INTEGER(&((*asn1_dtg)->randId),dtg->randId));

	BMD_FOK(asn_long2INTEGER(&((*asn1_dtg)->filesRemaining),dtg->filesRemaining));

	BMD_FOK(asn_long2INTEGER(&((*asn1_dtg)->datagramStatus),dtg->datagramStatus));


	/* ustawienie protocolData */
	if(dtg->protocolData)
	{

		 (*asn1_dtg)->protocolData=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		 memset((*asn1_dtg)->protocolData,0,sizeof(OCTET_STRING_t));
		 BMD_FOK(OCTET_STRING_fromBuf(	(*asn1_dtg)->protocolData,
					    			(const char *)dtg->protocolData->buf,
					    			dtg->protocolData->size));
	}

	/* ustawienie protocolDataFilename */
	if(dtg->protocolDataFilename)
	{

		(*asn1_dtg)->protocolDataFilename=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*asn1_dtg)->protocolDataFilename,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf(	(*asn1_dtg)->protocolDataFilename,
					    		(const char *)dtg->protocolDataFilename->buf,
					    		dtg->protocolDataFilename->size));
	}

	if(dtg->protocolDataFileLocationId)
	{
		(*asn1_dtg)->protocolDataFileLocationId=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*asn1_dtg)->protocolDataFileLocationId,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf(   (*asn1_dtg)->protocolDataFileLocationId,
							(const char *)dtg->protocolDataFileLocationId->buf,
							dtg->protocolDataFileLocationId->size));
	}

	/* ustawienie protocolDataFileId */
	if(dtg->protocolDataFileId)
	{

		(*asn1_dtg)->protocolDataFileId=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*asn1_dtg)->protocolDataFileId,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf(	(*asn1_dtg)->protocolDataFileId,
					  		(const char *)dtg->protocolDataFileId->buf,
					    		dtg->protocolDataFileId->size));
	}

	/* ustawienie protocolDataOwner */
	if(dtg->protocolDataOwner)
	{

		(*asn1_dtg)->protocolDataOwner=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*asn1_dtg)->protocolDataOwner,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf(	(*asn1_dtg)->protocolDataOwner,
					    		(const char *)dtg->protocolDataOwner->buf,
					    		dtg->protocolDataOwner->size));
	}

	/* ustawienie metadanych systemowych */
	if(dtg->no_of_sysMetaData>0)
	{

		BMD_FOK(_PR2_BMDDatagram_set_metadata(dtg->sysMetaData, dtg->no_of_sysMetaData, _PR2_SYS_MTD,asn1_dtg));
	}

	/* ustawienie metadanych uzytkownika */
	if(dtg->no_of_actionMetaData>0)
	{

		BMD_FOK(_PR2_BMDDatagram_set_metadata(dtg->actionMetaData, dtg->no_of_actionMetaData, _PR2_ACT_MTD,asn1_dtg));
	}

	/* ustawienie metadanych dodatkowych */
	if(dtg->no_of_additionalMetaData>0)
	{

		BMD_FOK(_PR2_BMDDatagram_set_metadata(dtg->additionalMetaData, dtg->no_of_additionalMetaData, _PR2_ADD_MTD,asn1_dtg));
	}

	/* ustawienie metadanych PKI */
	if(dtg->no_of_pkiMetaData>0)
	{

		BMD_FOK(_PR2_BMDDatagram_set_metadata(dtg->pkiMetaData, dtg->no_of_pkiMetaData, _PR2_PKI_MTD,asn1_dtg));
	}

	return BMD_OK;
}

long _PR2_BMDDatagram2bmdDatagram(	BMDDatagram_t *asn1_dtg,
						bmdDatagram_t **dtg)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (asn1_dtg==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dtg==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*dtg)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*dtg)=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	if(*dtg == NULL)	 {	BMD_FOK(NO_MEMORY);	}
	memset(*dtg,0,sizeof(bmdDatagram_t));

	/* wartosci INTEGER */
	BMD_FOK(asn_INTEGER2long(&(asn1_dtg->protocolVersion), &((*dtg)->protocolVersion)));
	BMD_FOK(asn_INTEGER2long(&(asn1_dtg->datagramType), &((*dtg)->datagramType)));
	BMD_FOK(asn_INTEGER2long(&(asn1_dtg->randId), &((*dtg)->randId)));
	BMD_FOK(asn_INTEGER2long(&(asn1_dtg->filesRemaining), &((*dtg)->filesRemaining)));
	BMD_FOK(asn_INTEGER2long(&(asn1_dtg->datagramStatus), &((*dtg)->datagramStatus)));

	/* wartosci OCTET_STRING */
	if(asn1_dtg->protocolData)
	{
		BMD_FOK(set_gen_buf2((char *)(asn1_dtg->protocolData->buf), asn1_dtg->protocolData->size, &((*dtg)->protocolData)));
	}
	if(asn1_dtg->protocolDataFilename)
	{
		BMD_FOK(set_gen_buf2((char *)(asn1_dtg->protocolDataFilename->buf), asn1_dtg->protocolDataFilename->size, &((*dtg)->protocolDataFilename)));
	}
	if(asn1_dtg->protocolDataFileLocationId)
	{
		BMD_FOK(set_gen_buf2((char *)(asn1_dtg->protocolDataFileLocationId->buf), asn1_dtg->protocolDataFileLocationId->size, &((*dtg)->protocolDataFileLocationId)));
	}
	if(asn1_dtg->protocolDataFileId)
	{
		BMD_FOK(set_gen_buf2((char*)(asn1_dtg->protocolDataFileId->buf), asn1_dtg->protocolDataFileId->size, &((*dtg)->protocolDataFileId)));
	}
	if(asn1_dtg->protocolDataOwner)
	{
		BMD_FOK(set_gen_buf2((char*)(asn1_dtg->protocolDataOwner->buf), asn1_dtg->protocolDataOwner->size, &((*dtg)->protocolDataOwner)));
	}


	BMD_FOK(_PR_MetaData2MetaDataBuf(	(struct MetaDatas *)asn1_dtg->sysMetaDatas,
							&((*dtg)->sysMetaData),
							&((*dtg)->no_of_sysMetaData), NULL));
	BMD_FOK(_PR_MetaData2MetaDataBuf(	(struct MetaDatas *)asn1_dtg->actionMetaDatas,
							&((*dtg)->actionMetaData),
							&((*dtg)->no_of_actionMetaData), NULL));
	BMD_FOK(_PR_MetaData2MetaDataBuf(	(struct MetaDatas *)asn1_dtg->pkiMetaDatas,
							&((*dtg)->pkiMetaData),
							&((*dtg)->no_of_pkiMetaData), NULL));
	BMD_FOK(_PR_MetaData2MetaDataBuf(	(struct MetaDatas *)asn1_dtg->additionalMetaDatas,
							&((*dtg)->additionalMetaData),
							&((*dtg)->no_of_additionalMetaData), NULL));

	return BMD_OK;
}

long _PR_MetaData2MetaDataBuf(struct MetaDatas *input,
				     	MetaDataBuf_t ***mtds,
					long *no_of_mtds,
					bmdDatagram_t *twf_bmdDatagram)
{
long i				= 0;
long pos				= 0;
char *OIDs_str                = NULL;
char *myIds_str               = NULL;
char *ownerIds_str            = NULL;
char *ownerTypes_str          = NULL;
char *datas_str               = NULL;
char *str_pos			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(mtds == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*mtds)!= NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(no_of_mtds == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(input==NULL)
	{
		*no_of_mtds=0;
		*mtds=NULL;
		return BMD_OK;
	}

	/**************************************************************/
	/* określenie liczby metadanych i zaalokowanie pamięci na nie */
	/**************************************************************/
	(*no_of_mtds)=input->lenOfDatas.list.count;
	PRINT_VDEBUG("LIBBMDPRVDEBUG Metadatas count: %li\n", *no_of_mtds);

	(*mtds)=(MetaDataBuf_t **)malloc(sizeof(MetaDataBuf_t *)*( (*no_of_mtds)));
	if( (*mtds) == NULL )	{	BMD_FOK(NO_MEMORY);	}
	memset((*mtds),0,sizeof(MetaDataBuf_t *)*( (*no_of_mtds)));

	for (i=0; i<(*no_of_mtds); i++)
	{
		(*mtds)[i]=(MetaDataBuf_t *)malloc(sizeof(MetaDataBuf_t ));
		memset((*mtds)[i], 0, sizeof(MetaDataBuf_t ));
	}

	/***********************************/
	/* deserializacja oidów metadanych */
	/***********************************/
	BMD_FOK(OCTET_STRING2string(&(input->oids), &OIDs_str));
	pos = 0;
	str_pos = OIDs_str;
	for(i=0; i < (*no_of_mtds); i++)
	{
		str_pos = str_pos + pos;
		BMD_FOK(asn_INTEGER2long(input->lenOfOids.list.array[i], &pos));
		((*mtds)[i])->OIDTableBuf=malloc((pos+1)*sizeof(char));
		if (((*mtds)[i])->OIDTableBuf==NULL)	{	BMD_FOK(NO_MEMORY);	}
		memset(((*mtds)[i])->OIDTableBuf, 0, (pos+1)*sizeof(char));
 		memcpy(((*mtds)[i])->OIDTableBuf, str_pos, pos*sizeof(char));
		pos++; /*Na NULL*/
	}
	free(OIDs_str); OIDs_str = NULL;

	/**********************************/
	/* deserializacja myId metadanych */
	/**********************************/
	BMD_FOK(OCTET_STRING2string(&(input->myIds), &myIds_str));
	pos = 0;
	str_pos = myIds_str;
	for(i=0; i < *no_of_mtds; i++)
	{
		str_pos = str_pos + pos;
		((*mtds)[i])->myId = atol(str_pos);
		BMD_FOK(asn_INTEGER2long(input->lenOfMyIds.list.array[i], &pos));
		pos++; /*Na NULL*/
	}
	free(myIds_str); myIds_str = NULL;

	/*************************************/
	/* deserializacja ownerId metadanych */
	/*************************************/
	BMD_FOK(OCTET_STRING2string(&(input->ownerIds), &ownerIds_str));
	pos = 0;
	str_pos = ownerIds_str;
	for(i=0; i < *no_of_mtds; i++)
	{
		str_pos = str_pos + pos;
		((*mtds)[i])->ownerId = atol(str_pos);
		BMD_FOK(asn_INTEGER2long(input->lenOfOwnerIds.list.array[i], &pos));
		pos++; /*Na NULL*/
	}
	free(ownerIds_str); ownerIds_str = NULL;

	/***************************************/
	/* deserializacja ownerType metadanych */
	/***************************************/
	BMD_FOK(OCTET_STRING2string(&(input->ownerTypes), &ownerTypes_str));
	pos = 0;
	str_pos = ownerTypes_str;
	for(i=0; i < *no_of_mtds; i++)
	{
		str_pos = str_pos + pos;
		((*mtds)[i])->ownerType = atol(str_pos);
		BMD_FOK(asn_INTEGER2long(input->lenOfOwnerTypes.list.array[i], &pos));
		pos++; /*Na NULL*/
	}
	free(ownerTypes_str); ownerTypes_str = NULL;

	/**************************************/
	/* deserializacja wartości metadanych */
	/**************************************/
	BMD_FOK(OCTET_STRING2string(&(input->datas), &datas_str));
	pos = 0;
	str_pos = datas_str;

	if(twf_bmdDatagram != NULL)
	{
		str_pos = str_pos + pos;
		BMD_FOK(asn_INTEGER2long(input->lenOfDatas.list.array[i], &pos));
		((*mtds)[i])->AnyBuf = malloc((pos + 1) * sizeof(char));
		if(((*mtds)[i])->AnyBuf == NULL) 	{	BMD_FOK(NO_MEMORY);	}
		memset( ((*mtds)[i])->AnyBuf, 0, (pos + 1) * sizeof(char));
		memcpy( ((*mtds)[i])->AnyBuf, str_pos, pos * sizeof(char));
		((*mtds)[i])->AnySize = pos;
		pos++; /*Na NULL*/
	}
	else
	{
		for(i=0; i < *no_of_mtds; i++)
		{
			str_pos = str_pos + pos;
			BMD_FOK(asn_INTEGER2long(input->lenOfDatas.list.array[i], &pos));
			PRINT_VDEBUG("LIBBMDPRVDEBUG Decoding loop i: %li, data string len: %li\n", i, pos);
			((*mtds)[i])->AnyBuf = malloc((pos + 1) * sizeof(char));
			if(((*mtds)[i])->AnyBuf == NULL) 	{	BMD_FOK(NO_MEMORY);	}
			memset( ((*mtds)[i])->AnyBuf, 0, (pos + 1) * sizeof(char));
			memcpy( ((*mtds)[i])->AnyBuf, str_pos, pos * sizeof(char));
			/*BMD_FOK(asn_INTEGER2long(input->lenOfDatas.list.array[i], &pos));*/
			((*mtds)[i])->AnySize = pos;
			pos++; /*Na NULL*/
		}
		free(datas_str); datas_str = NULL;
	}

	return BMD_OK;
}

long _PR_bmdDatagram_free(	bmdDatagram_t *bmdDatagram,
					long with_pointer)
{
long i		= 0;

	if(bmdDatagram == NULL)
	{

		PRINT_WARNING("LIBBMDPRWARN Nothing to be freed.\n");
		return BMD_OK;
	}


	if(bmdDatagram->actionMetaData != NULL)
	{
		for(i=0; i<bmdDatagram->no_of_actionMetaData; i++)
		{

			PRINT_VDEBUG("LIBBMDPRVDEBUG Free actionMetaData loop iterations: %li\n", i);

			if(bmdDatagram->actionMetaData[i] != NULL)
			{

				free(bmdDatagram->actionMetaData[i]->OIDTableBuf);
				bmdDatagram->actionMetaData[i]->OIDTableBuf = NULL;

				free(bmdDatagram->actionMetaData[i]->AnyBuf);
				bmdDatagram->actionMetaData[i]->AnyBuf = NULL;

				free(bmdDatagram->actionMetaData[i]);
				bmdDatagram->actionMetaData[i] = NULL;
			}
		}

		free(bmdDatagram->actionMetaData);
		bmdDatagram->actionMetaData = NULL;
	}


	if(bmdDatagram->sysMetaData != NULL)
	{

		for(i=0; i<bmdDatagram->no_of_sysMetaData; i++)
		{
			/* MSILEWICZ - wykomentowalem dla lepszej czytelnosci logow */
			//PRINT_VDEBUG("LIBBMDPRVDEBUG Free sysMetaData loop iterations: %li\n", i);
			if(bmdDatagram->sysMetaData[i] != NULL)
			{

				free(bmdDatagram->sysMetaData[i]->OIDTableBuf);
				bmdDatagram->sysMetaData[i]->OIDTableBuf = NULL;

				free(bmdDatagram->sysMetaData[i]->AnyBuf);
				bmdDatagram->sysMetaData[i]->AnyBuf = NULL;

				free(bmdDatagram->sysMetaData[i]);
				bmdDatagram->sysMetaData[i] = NULL;
			}

		}

		free(bmdDatagram->sysMetaData);
		bmdDatagram->sysMetaData = NULL;
	}


	if(bmdDatagram->pkiMetaData != NULL)
	{
		for(i=0; i<bmdDatagram->no_of_pkiMetaData; i++)
		{

			PRINT_VDEBUG("LIBBMDPRVDEBUG Free pkiMetaData loop iterations: %li\n", i);
			if(bmdDatagram->pkiMetaData[i] != NULL)
			{

				free(bmdDatagram->pkiMetaData[i]->OIDTableBuf);
				bmdDatagram->pkiMetaData[i]->OIDTableBuf = NULL;


				free(bmdDatagram->pkiMetaData[i]->AnyBuf);
				bmdDatagram->pkiMetaData[i]->AnyBuf = NULL;

				free(bmdDatagram->pkiMetaData[i]);
				bmdDatagram->pkiMetaData[i] = NULL;
			}

		}

		free(bmdDatagram->pkiMetaData);
		bmdDatagram->pkiMetaData = NULL;
	}


	if(bmdDatagram->additionalMetaData != NULL)
	{
		free(bmdDatagram->additionalMetaData_string);
		bmdDatagram->additionalMetaData_string = NULL;

		free(bmdDatagram->additionalMetaData_string_len);
		bmdDatagram->additionalMetaData_string_len = NULL;

		PRINT_VDEBUG("LIBBMDPRVDEBUG additionalMetaData_string_count %li\n",
		bmdDatagram->additionalMetaData_string_count);

		for(i=0; i<bmdDatagram->additionalMetaData_string_count; i++)
		{
			//PRINT_VDEBUG("LIBBMDPRVDEBUG Free additionalMetaData loop iterations: %li\n", i);
			if(bmdDatagram->additionalMetaData[i] != NULL)
			{
				free(bmdDatagram->additionalMetaData[i]->OIDTableBuf);
				bmdDatagram->additionalMetaData[i]->OIDTableBuf = NULL;

				/*free(bmdDatagram->additionalMetaData[i]->AnyBuf);*/
				bmdDatagram->additionalMetaData[i]->AnyBuf = NULL;

				free(bmdDatagram->additionalMetaData[i]);
				bmdDatagram->additionalMetaData[i] = NULL;
			}
		}

		for(i=bmdDatagram->additionalMetaData_string_count; i<bmdDatagram->no_of_additionalMetaData; i++)
		{
			//PRINT_VDEBUG("LIBBMDPRVDEBUG Free additionalMetaData loop iterations: %li\n", i);
			if(bmdDatagram->additionalMetaData[i] != NULL)
			{
				free(bmdDatagram->additionalMetaData[i]->OIDTableBuf);
				bmdDatagram->additionalMetaData[i]->OIDTableBuf = NULL;

				free(bmdDatagram->additionalMetaData[i]->AnyBuf);
				bmdDatagram->additionalMetaData[i]->AnyBuf = NULL;

				free(bmdDatagram->additionalMetaData[i]);
				bmdDatagram->additionalMetaData[i] = NULL;
			}
		}

		free(bmdDatagram->additionalMetaData);
		bmdDatagram->additionalMetaData = NULL;
	}


	if(bmdDatagram->protocolData != NULL)
	{

		if(bmdDatagram->protocolData->buf != NULL)
		{

			free(bmdDatagram->protocolData->buf);
			bmdDatagram->protocolData->buf = NULL;
		}

		free(bmdDatagram->protocolData);

		bmdDatagram->protocolData = NULL;
	}

	if(bmdDatagram->protocolDataFilename != NULL)
	{
		if (bmdDatagram->protocolDataFilename->buf!=NULL)
		{
			free(bmdDatagram->protocolDataFilename->buf);
			bmdDatagram->protocolDataFilename->buf = NULL;
		}
		free(bmdDatagram->protocolDataFilename);
		bmdDatagram->protocolDataFilename = NULL;
	}

	if(bmdDatagram->protocolDataFileLocationId != NULL)
	{
		if (bmdDatagram->protocolDataFileLocationId->buf!=NULL)
		{
			free(bmdDatagram->protocolDataFileLocationId->buf);
			bmdDatagram->protocolDataFileLocationId->buf = NULL;
		}

		free(bmdDatagram->protocolDataFileLocationId);
		bmdDatagram->protocolDataFileLocationId = NULL;
	}

	if(bmdDatagram->protocolDataFileId != NULL)
	{
		free(bmdDatagram->protocolDataFileId);
		bmdDatagram->protocolDataFileId = NULL;
	}


	if(bmdDatagram->protocolDataOwner != NULL)
	{
		if (bmdDatagram->protocolDataOwner->buf!=NULL)
		{
			free(bmdDatagram->protocolDataOwner->buf);
			bmdDatagram->protocolDataOwner->buf = NULL;
		}

		free(bmdDatagram->protocolDataOwner);
		bmdDatagram->protocolDataOwner = NULL;
	}


	if(with_pointer)
	{

		free(bmdDatagram);
		bmdDatagram = NULL;
	}

	return BMD_OK;
}

/*Jezeli twf_copy_ptr = 1 to z DerEncodedValue jest kopiowany sam wskaznik, a nie jest kopiowana tresc do nowego
bufora. */
long AddElementToMetaDataTable(	MetaDataBuf_t ***MetaDataArray,
						long *no_of_MetaData,
						char *OIDtable,
						GenBuf_t *DerEncodedValue,
						long myId,
						long ownerType,
						long ownerId,
						long twf_copy_ptr)

{
long err = 0;
long ts = *no_of_MetaData;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(MetaDataArray == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(no_of_MetaData == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(OIDtable == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(DerEncodedValue == NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(twf_copy_ptr < 0 || twf_copy_ptr > 1)	{	BMD_FOK(BMD_ERR_PARAM9);	}

	*MetaDataArray = (MetaDataBuf_t **) realloc (*MetaDataArray, (1+ts)*sizeof(MetaDataBuf_t *));
	if(*MetaDataArray == NULL)		{	BMD_FOK(NO_MEMORY);	}

	(*MetaDataArray)[ts] = (MetaDataBuf_t *) malloc (sizeof(MetaDataBuf_t));
	if((*MetaDataArray)[ts] == NULL)	{	BMD_FOK(NO_MEMORY);	}

	memset((*MetaDataArray)[ts], 0, sizeof(MetaDataBuf_t));

	(*MetaDataArray)[ts]->OIDTableBuf = (char *)malloc(sizeof(char)*(strlen(OIDtable)+1));
	if((*MetaDataArray)[ts]->OIDTableBuf == NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset((*MetaDataArray)[ts]->OIDTableBuf, 0, sizeof(char)*(strlen(OIDtable)+1));
	memcpy((*MetaDataArray)[ts]->OIDTableBuf, OIDtable, sizeof(char)*(strlen(OIDtable)));

	if(twf_copy_ptr == 0)
	{
		(*MetaDataArray)[ts]->AnyBuf = (char *) malloc (sizeof(char)*(DerEncodedValue->size+1));
		if((*MetaDataArray)[ts]->AnyBuf == NULL)		{	BMD_FOK(NO_MEMORY);	}
		memset((*MetaDataArray)[ts]->AnyBuf, 0, sizeof(char)*(DerEncodedValue->size+1));
		memmove((*MetaDataArray)[ts]->AnyBuf, DerEncodedValue->buf, DerEncodedValue->size);

		(*MetaDataArray)[ts]->AnySize = DerEncodedValue->size;
	}
	else
	{
		(*MetaDataArray)[ts]->AnyBuf = DerEncodedValue->buf;
		(*MetaDataArray)[ts]->AnySize = DerEncodedValue->size;

		DerEncodedValue->buf = NULL;
		DerEncodedValue->size = 0;
	}

	(*MetaDataArray)[ts]->myId=myId;
	(*MetaDataArray)[ts]->ownerId=ownerId;
	(*MetaDataArray)[ts]->ownerType=ownerType;

	(*no_of_MetaData)++;
	return err;
}
