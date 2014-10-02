#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/**
 * Funkcja konwertuje OID z postaci w jakiej był przesłany od BMD, na postać tekstową.
 * Funkcja alokuje pamięc pod wyjściowy łańcuch tekstowy zawierający OID.
 *
 * \param[in] oid Wartość OIDu do przekonwertowania.
 *
 * \param[out] oid_as_string OID w postaci łańcucha tekstowego.
 *
 * \retval BMD_OK Funkcja zakończyła się bez błędów.
 * \retval -1 Konwersja nie powiodła się.
 * */
// long bmd_oid_to_string(OID_attr_t *oid,char **oid_as_string)
// {
// 	/******************************/
// 	/*	walidacja parametrow	*/
// 	/******************************/
// 	if (oid==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (oid->oid_table==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (oid->oid_table_size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
//
// 	BMD_FOK(ulong_to_str_of_OID2(oid->oid_table,oid->oid_table_size,oid_as_string));
//
// 	return BMD_OK;
// }
//

long genbuf_to_char(	GenBuf_t *gbuf,
				char **output)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (gbuf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}


	(*output)=(char *)malloc(sizeof(unsigned char)*(gbuf->size+1));
	if ((*output)==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}
	memset((*output), 0, sizeof(unsigned char)*(gbuf->size+1));
	memcpy((*output), gbuf->buf, sizeof(unsigned char)*gbuf->size);


	return BMD_OK;
}


