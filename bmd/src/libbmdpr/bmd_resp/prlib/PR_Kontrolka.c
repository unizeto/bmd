#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolka.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>
int bmd_kontrolka_init(	Kontrolka_t **kontrolka)
{
	if(kontrolka==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*kontrolka)=(Kontrolka_t *)malloc(sizeof(Kontrolka_t));
	memset(*kontrolka,0,sizeof(Kontrolka_t));

	return BMD_OK;
}

int bmd_kontrolka_create(	BMD_attr_t *attr,
					Kontrolka_t **kontrolka)
{
int i					= 0;
OCTET_STRING_t *tmp_octet	= NULL;
INTEGER_t *tmp_integer		= NULL;

	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if(attr==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(kontrolka==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_kontrolka_init(kontrolka));

	/******************************/
    	/*	stworzenie etykiety	*/
	/******************************/
	BMD_FOK(OCTET_STRING_fromBuf(&((*kontrolka)->label),(const char *)attr->label->buf,attr->label->size));

	/************************************/
	/*	ustawienie typu kontrolki	*/
	/************************************/
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->wxType),attr->wxType));

	/******************************/
	/*	ustawienie widocznosci	*/
	/******************************/
    	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->visibility),attr->visibility));

	/******************************************/
    	/*	ustawienie docelowego formularza	*/
	/******************************************/
    	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->targetForm),attr->targetForm));

	/************************************/
	/*	ustawienie numeru kolejnego	*/
	/************************************/
    	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->seqNumber),attr->seqNumber));

	/************************************/
	/*	ustawienie listy wartosci	*/
	/************************************/
	if(attr->valuesList!=NULL)
	{

		while(attr->valuesList[i]!=NULL)
		{
			tmp_octet=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
			memset(tmp_octet,0,sizeof(OCTET_STRING_t));
			BMD_FOK(OCTET_STRING_fromBuf(tmp_octet,(char *)attr->valuesList[i]->buf,attr->valuesList[i]->size));
			BMD_FOK(asn_set_add(&((*kontrolka)->valuesList.list),tmp_octet));

			/******************************************************************************/
			/*	nie usuwamy tymczasowego OCTET_STRING'a bo jest podpinany jako wskaznik	*/
			/******************************************************************************/

	    		i++;

		}
	}

	i=0;
	/************************************/
	/*	ustawienie listy priorytetow	*/
	/************************************/
    	if(attr->priorityList)
    	{

		while(attr->priorityList[i])
		{

			tmp_integer=(INTEGER_t *)malloc(sizeof(INTEGER_t));
			memset(tmp_integer,0,sizeof(INTEGER_t));
			BMD_FOK(asn_long2INTEGER(tmp_integer,attr->priorityList[i]));
			/* nie usuwamy tymczasowego INTEGERA bo jest podpinany jako wskaznik */
			i++;
		}

	}

    	/* ustawienie rozmiaru sizeX */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->sizeX),attr->sizeX));

	/* ustawienie rozmiaru sizeY */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->sizeY),attr->sizeY));

	/* ustawienie OIDU */
	BMD_FOK(OCTET_STRING_fromBuf(&((*kontrolka)->oid),(const char *)attr->oid->buf,attr->oid->size));
// pkl    	BMD_FOK(OBJECT_IDENTIFIER_set_arcs(&((*kontrolka)->oid),attr->oid.oid_table,
// 				      sizeof(attr->oid.oid_table[0]),
// 				      attr->oid.oid_table_size/sizeof(attr->oid.oid_table[0])));


	/* ustawienie typu zawartosci */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->contentType),attr->contentType));

	/* ustawienie regexpa */
	BMD_FOK(OCTET_STRING_fromBuf(&((*kontrolka)->regexp),(const char *)attr->regexp->buf,attr->regexp->size));

	/* ustawienie kontrolki nadrzednej */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->superiorControl),attr->superiorControl));

	/* ustawienie kontrolki podrzednej */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->inferiorControl),attr->inferiorControl));

	/* ustawienie typu polaczenia pomiedzy kontrolkami */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->connectType),attr->connectType));

	/* ustawienie zrodla wartosci */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->valueSource),attr->valueSource));

	/* ustawienie tooltipa */
	if(attr->tooltip)
	{
		(*kontrolka)->tooltip=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*kontrolka)->tooltip,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf((*kontrolka)->tooltip,(const char *)attr->tooltip->buf,attr->tooltip->size));
	}

	/* ustawienie listy wartosci podrzednych */
	if(attr->depValueList)
	{
		(*kontrolka)->depValueList=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset((*kontrolka)->depValueList,0,sizeof(OCTET_STRING_t));
		BMD_FOK(OCTET_STRING_fromBuf((*kontrolka)->depValueList,(const char *)attr->depValueList->buf,attr->depValueList->size));
	}
	else
	{
		(*kontrolka)->depValueList=NULL;
	}

	/* ustawienie przechodniosci */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->inheritancePresent),attr->inheritancePresent));

	/* ustawienie nazwy kontrolki */
	BMD_FOK(OCTET_STRING_fromBuf(&((*kontrolka)->controlName),(const char *)attr->controlName->buf,attr->controlName->size));

	/* ustawienie splitToWords */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->splitToWords),attr->splitToWords));

	/* ustawienie rangeEnabled */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->rangeEnabled),attr->rangeEnabled));

	/* ustawienie mandatory */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->mandatory),attr->mandatory));

	/* ustawienie isUpdateEnabled */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->isUpdateEnabled),attr->isUpdateEnabled));

	/* ustawienie isHistoryEnabled */
	BMD_FOK(asn_long2INTEGER(&((*kontrolka)->isHistoryEnabled),attr->isHistoryEnabled));

	return BMD_OK;
}

int bmd_kontrolka_der_encode(	Kontrolka_t *kontrolka,
					GenBuf_t **output)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(kontrolka==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(asn1_encode(&asn_DEF_Kontrolka,kontrolka, NULL, output));

	return BMD_OK;
}

int bmd_kontrolka_free(	Kontrolka_t **kontrolka)
{
    asn_DEF_Kontrolka.free_struct(&asn_DEF_Kontrolka,*kontrolka,0);*kontrolka=NULL;

    return BMD_OK;
}
