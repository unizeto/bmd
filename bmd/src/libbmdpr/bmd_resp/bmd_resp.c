#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>

#include <bmd/libbmdutils/libbmdutils.h>
#include <string.h>
#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolka.h>
#include <bmd/libbmdpr/bmd_resp/prlib/PR_Kontrolki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>
long bmd_kontrolki_serialize(	BMD_attr_t **attrs,
				GenBuf_t **output)
{
long i				= 0;
Kontrolka_t *kontrolka	= NULL;
Kontrolki_t *kontrolki	= NULL;

	PRINT_INFO("LIBBMDPRINF Serializing kontrolki\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(attrs==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	i=0;
	while(attrs[i])
	{
		BMD_FOK(bmd_kontrolka_create(attrs[i],&kontrolka));
		BMD_FOK(bmd_kontrolki_create(kontrolka,&kontrolki));
		i++;
	}

	BMD_FOK(bmd_kontrolki_der_encode(kontrolki,output));
	bmd_kontrolki_free(&kontrolki);

	return BMD_OK;
}

long bmd_kontrolki_deserialize(	GenBuf_t *input,
				BMD_attr_t ***attrs)
{
long i			= 0;
long j			= 0;
Kontrolki_t *kontrolki	= NULL;
Kontrolka_t *kontrolka	= NULL;
OCTET_STRING_t *octet	= NULL;
INTEGER_t *integer	= NULL;

	PRINT_INFO("LIBBMDPRINF Deerializing kontrolki\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (attrs==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*attrs)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_kontrolki_der_decode(input,&kontrolki));

	(*attrs)=(BMD_attr_t **)malloc(sizeof(BMD_attr_t *)*(kontrolki->list.count+2));
	if ((*attrs)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}
	memset(*attrs,0,sizeof(BMD_attr_t *)*(kontrolki->list.count+1));

	for(i=0;i<kontrolki->list.count;i++)
	{

		kontrolka=kontrolki->list.array[i];
		(*attrs)[i]=(BMD_attr_t *)malloc(sizeof(BMD_attr_t));
		memset((*attrs)[i],0,sizeof(BMD_attr_t));

		/* ustawienie etykiety */
		BMD_FOK(set_gen_buf2((char*)(kontrolka->label.buf),kontrolka->label.size,&((*attrs)[i]->label)));

		/* ustawienie typy kontrolki */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->wxType),&((*attrs)[i]->wxType)));

		/* ustawienie widocznosci */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->visibility),&((*attrs)[i]->visibility)));

		/* ustawienie docelowego formularza */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->targetForm),&((*attrs)[i]->targetForm)));

		/* ustawienie numeru kolejnego */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->seqNumber),&((*attrs)[i]->seqNumber)));

		/* ustawienie listy wartosci */
		(*attrs)[i]->valuesList=(BMD_attr_value **)malloc(sizeof(BMD_attr_value *)*(kontrolka->valuesList.list.count+2));
		memset((*attrs)[i]->valuesList,0,sizeof(BMD_attr_value *)*(kontrolka->valuesList.list.count+1));
		for(j=0;j<kontrolka->valuesList.list.count;j++)
		{
			octet=kontrolka->valuesList.list.array[j];
			BMD_FOK(set_gen_buf2((char*)(octet->buf), octet->size,&((*attrs)[i]->valuesList[j])));
		}

		/* ustawienie listy priorytetow */
		(*attrs)[i]->priorityList=(long *)malloc(sizeof(long)*(kontrolka->priorityList.list.count+2));
		memset((*attrs)[i]->priorityList,0,sizeof(long)*(kontrolka->priorityList.list.count+1));
		for(j=0;j<kontrolka->priorityList.list.count;j++)
		{
			integer=kontrolka->priorityList.list.array[j];
			BMD_FOK(asn_INTEGER2long(integer,&((*attrs)[i]->priorityList[j])));
		}

		/* ustawienie rozmiaru sizeX */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->sizeX),&((*attrs)[i]->sizeX)));

		/* ustawienie rozmiaru sizeY */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->sizeY),&((*attrs)[i]->sizeY)));

		BMD_FOK(set_gen_buf2((char *)(kontrolka->oid.buf), kontrolka->oid.size, &((*attrs)[i]->oid)));

		/* ustawienie typu zawartosci */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->contentType),&((*attrs)[i]->contentType)));

		/* ustawienie regexp */
		BMD_FOK(set_gen_buf2((char *)(kontrolka->regexp.buf), kontrolka->regexp.size, &((*attrs)[i]->regexp)));

		/* ustawienie kontrolki nadrzednej */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->superiorControl),&((*attrs)[i]->superiorControl)));

		/* ustawienie kontrolki podrzednej */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->inferiorControl),&((*attrs)[i]->inferiorControl)));

		/* ustawienie kontrolki nadrzednej */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->superiorControl),&((*attrs)[i]->superiorControl)));

		/* ustawienie zrodla wartosci*/
		BMD_FOK(asn_INTEGER2long(&(kontrolka->valueSource),&((*attrs)[i]->valueSource)));

		/* ustawienie podpowiedzi */
		if(kontrolka->tooltip)
		{
			BMD_FOK(set_gen_buf2((char *)(kontrolka->tooltip->buf), kontrolka->tooltip->size, \
			&((*attrs)[i]->tooltip)));
		}

		/* ustawienie wartosci zaleznych */
		if(kontrolka->depValueList)
		{
			BMD_FOK(set_gen_buf2((char *)(kontrolka->depValueList->buf), kontrolka->depValueList->size, \
			&((*attrs)[i]->depValueList)));
		}

		/* ustawienie przechodniosci */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->inheritancePresent),&((*attrs)[i]->inheritancePresent)));

		/* ustawienie nazwy kontrolki */
		BMD_FOK(set_gen_buf2((char *)(kontrolka->controlName.buf), kontrolka->controlName.size, \
		&((*attrs)[i]->controlName)));

		/* ustawienie splitToWords */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->splitToWords),&((*attrs)[i]->splitToWords)));

		/* ustawienie rangeEnabled */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->rangeEnabled),&((*attrs)[i]->rangeEnabled)));

		/* ustawienie mandatory */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->mandatory),&((*attrs)[i]->mandatory)));

		/* ustawienie flagi czy kontrolka jest updatowalna */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->isUpdateEnabled),&((*attrs)[i]->isUpdateEnabled)));

		/* ustawienie flagi czy kontrolka jest historyzowalna */
		BMD_FOK(asn_INTEGER2long(&(kontrolka->isHistoryEnabled),&((*attrs)[i]->isHistoryEnabled)));
	}

	bmd_kontrolki_free(&kontrolki);

	return BMD_OK;
}
