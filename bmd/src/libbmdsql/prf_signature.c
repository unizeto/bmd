#define _WINSOCK2API_
#include<bmd/libbmdsql/proofs.h>
#include<bmd/libbmdsql/common.h>
#include<bmd/common/bmd-oids.h>
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdxades/libbmdxades.h>

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4189)
#endif

long prfParseAndInsertSignature(	void *hDB,
					char *crypto_objects_id,
					const WhoInsertIt_t signature_userorsystem,
					bmd_crypt_ctx_t *sym_ctx,
					GenBuf_t *signature_genbuf,
					char **signature_id)
{
PKIStandardEnum_t signature_standard	= STANDARD_UNKNOWN;
GenBuf_t *enc_signature			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	/*********************************/
	/* sparsowanie standardu podpisu */
	/*********************************/
	BMD_FOK_CHG(prfParseSignatureStandard(signature_genbuf, &signature_standard),
			LIBBMDSQL_PRF_SIGNATURE_PARSE_SIGNATURE_STANDARD_ERR);

	/******************************************************************/
	/*	tu by sie przydalo szyfrowac wrzucany do bazy danych podpis	*/
	/******************************************************************/
	BMD_FOK(bmdpki_symmetric_encrypt(sym_ctx, signature_genbuf, BMDPKI_SYM_LAST_CHUNK, &enc_signature));

	/********************************************************/
	/*	wreszcie wstaw sam plik koperty z podpisem	*/
	/********************************************************/
	BMD_FOK(prfInsertSignature(	hDB,
					crypto_objects_id,
					signature_standard,
					signature_userorsystem,
					enc_signature,
					signature_id));

	return BMD_OK;
}

long prfParseAndInsertSignatureinPades(	void *hDB,
					char *crypto_objects_id,
					const WhoInsertIt_t signature_userorsystem,
					bmd_crypt_ctx_t *sym_ctx,
					GenBuf_t *signature_genbuf,
					char **signature_id)
{
PKIStandardEnum_t signature_standard	= STANDARD_UNKNOWN;
GenBuf_t *enc_signature			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (signature_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (signature_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	
	BMD_FOK_CHG(prfParseSignatureStandard(signature_genbuf, &signature_standard),
			LIBBMDSQL_PRF_SIGNATURE_PARSE_SIGNATURE_STANDARD_ERR);

	//wyodrebniony podis z PADES to CMS (PKCS#7)
	signature_standard=STANDARD_PADES;
	
	BMD_FOK(bmdpki_symmetric_encrypt(sym_ctx, signature_genbuf, BMDPKI_SYM_LAST_CHUNK, &enc_signature));

	BMD_FOK(prfInsertSignature(	hDB,
					crypto_objects_id,
					signature_standard,
					signature_userorsystem,
					enc_signature,
					signature_id));

	return BMD_OK;
}

/**
* @author WSz
* Funkcja GetPadesHashValueAndAlgorithm() sprawdza, czy w zadaniu zawarty jest komplet metadanych wymaganych przy wstawianiu PAdES
* i wydobywa wartosc funkcji skrotu (z podpisywanej czesci pdf) oraz oid zastosowanej funkcji skrotu
* @param[in] requestDatagram to datagram przetwarzanego zadania
* @param[out] to wskaznik na wydobyty (zaalokowany) skrot (w zapisie hexadecymalnym) z podpisanej czesci pdf
* @param[out] to wskaznik na wydobyty oid funkcji skrotu zastosowanej do wyliczenia skrotu
* @return Gdy wymagane dane sa wydobyte z zadania, funkcja zwraca BMD_OK, w przeciwnym razie odpowiedni kod bledu (wartosc ujemna).
*/
long GetPadesHashValueAndAlgorithm(bmdDatagram_t *requestDatagram, char** hexHashValue, char** hashAlgorithmOid)
{
long iter		= 0;
char* tempHashAlgorithm	= NULL;
char* tempHashValue	= NULL;

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram->no_of_pkiMetaData <= 0)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram->pkiMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(hexHashValue == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*hexHashValue != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(hashAlgorithmOid == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*hashAlgorithmOid != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	

	for(iter=0; iter<requestDatagram->no_of_pkiMetaData; iter++)
	{
		if(requestDatagram->pkiMetaData[iter] == NULL)
		{
			free(tempHashAlgorithm);
			free(tempHashValue);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		if(requestDatagram->pkiMetaData[iter]->OIDTableBuf == NULL)
		{
			free(tempHashAlgorithm);
			free(tempHashValue);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		if(requestDatagram->pkiMetaData[iter]->AnyBuf == NULL)
		{
			free(tempHashAlgorithm);
			free(tempHashValue);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		if(strcmp(requestDatagram->pkiMetaData[iter]->OIDTableBuf, OID_PKI_METADATA_PADES_HASH_ALGORITHM) == 0)
		{
			if( IsOidFormat(requestDatagram->pkiMetaData[iter]->AnyBuf) == 1)
			{
				if(tempHashAlgorithm != NULL)
				{
					free(tempHashAlgorithm);
					free(tempHashValue);
					BMD_FOK(LIBBMDSQL_PADES_TOO_MANY_HASH_ALGORITHM_METADATA);
				}
				
				tempHashAlgorithm=STRNDUP(requestDatagram->pkiMetaData[iter]->AnyBuf, requestDatagram->pkiMetaData[iter]->AnySize);
				if(tempHashAlgorithm == NULL)
				{
					free(tempHashValue);
					BMD_FOK(BMD_ERR_MEMORY);
				}
			}
			else
			{
				free(tempHashAlgorithm);
				free(tempHashValue);
				BMD_FOK(INVALID_PKI_METADATA_HASH_ALGORITHM_OID_FORMAT);
			}
		}
		else if(strcmp(requestDatagram->pkiMetaData[iter]->OIDTableBuf, OID_PKI_METADATA_PADES_HASH_HEX_VALUE) == 0)
		{
			if(tempHashValue != NULL)
			{
				free(tempHashAlgorithm);
				free(tempHashValue);
				BMD_FOK(LIBBMDSQL_PADES_TOO_MANY_HASH_VALUE_METADATA);
			}
			
			// straznik - postac hex skrotu musi miec 40 bajtow
			if(requestDatagram->pkiMetaData[iter]->AnySize != 40)
			{
				free(tempHashAlgorithm);
				PRINT_ERROR("LIBBMDSQL Error incorrect size of hex hash value (%li)\n", requestDatagram->pkiMetaData[iter]->AnySize);
				BMD_FOK(LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE);
			}
			
			tempHashValue=STRNDUP(requestDatagram->pkiMetaData[iter]->AnyBuf, requestDatagram->pkiMetaData[iter]->AnySize);	
			if(tempHashValue == NULL)
			{
				free(tempHashAlgorithm);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			
			// straznik - po skopiowaniu musi miec 40 bajtow
			if(strlen(tempHashValue) != 40)
			{
				//PRINT_ERROR("LIBBMDSQL Error incorrect size of hex hash value (%i)\n", strlen(tempHashValue));
				free(tempHashAlgorithm); tempHashAlgorithm=NULL;
				free(tempHashValue); tempHashValue=NULL;
				BMD_FOK(LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE);
			}
		}
	}
	
	if(tempHashAlgorithm == NULL)
	{
		free(tempHashValue);
		BMD_FOK(LIBBMDSQL_PADES_HASH_ALGORITHM_NOT_SPECIFIED); //brakuje kompletu wymaganych metadanych dla pades
	}
	
	if(tempHashValue == NULL)
	{
		free(tempHashAlgorithm);
		BMD_FOK(LIBBMDSQL_PADES_HASH_VALUE_NOT_SPECIFIED); //brakuje kompletu wymaganych metadanych dla pades
	}
	
	*hexHashValue=tempHashValue;
	hexHashValue=NULL;
	
	*hashAlgorithmOid=tempHashAlgorithm;
	tempHashAlgorithm=NULL;
	
	return BMD_OK;
}

/**
* Funkcja InsertPadesHashToDatabase() dla podpisu PAdES atkualizuje wpis w tabeli prf_signatures
* o wartosc funkcji skrotu dla podpisanej czesci pdf oraz o oid wykorzystanej funkcji skrotu.
* @param[in] hDB uchwyt do nawiazanego wczesniej polaczenia bazodanowego
* @param[in] hexHashValue do wartosc funkcji skrotu w postaci hexadecymalnej
*	(do bazy trafia kod base64 z wartosci binarnej skrotu tj. odtworzonej z postaci hexadecymalnej)
* @param[in] hashAlgorithm to oid zastosowanej funkcji skrotu
* @param[in] signatureId to identyfikator wspisu w tabeli prf_signatures (podpisu), ktorego dane maja zostac zaktualizowane
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, a w przypadku bledu odpowiednia wartosc ujemna.
*/
long InsertPadesHashToDatabase(void* hDB, char* hexHashValue, char* hashAlgorithm, char* signatureId)
{
long retVal			= 0;
char* queryTemplate	= "UPDATE prf_signatures SET hash_oid='%s', hash_value='%s' WHERE id='%s' AND location_id='%s' ;";
char* updateQuery	= NULL;
void* queryResult	= NULL;
GenBuf_t* basedHashValueBuf	= NULL;
char* basedHashValue		= NULL;

char* escapedHashAlgorithm	= NULL;
char* escapedBasedHashValue	= NULL;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(hexHashValue == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	// straznik - postac hex strotu ma 40 bajtow
	if(strlen(hexHashValue) != 40)
		{ BMD_FOK(LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE); }
	if(hashAlgorithm == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(signatureId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	BMD_FOK(bmd_convert_hex2base64(hexHashValue, WITHOUT_SEPARATOR, &basedHashValueBuf));
	
	basedHashValue=STRNDUP(basedHashValueBuf->buf, basedHashValueBuf->size);
	free_gen_buf(&basedHashValueBuf);
	if(basedHashValue == NULL)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	
	retVal = bmd_db_escape_string(hDB, basedHashValue, STANDARD_ESCAPING, &escapedBasedHashValue);
	free(basedHashValue); basedHashValue=NULL;
	BMD_FOK(retVal);
	
	retVal = bmd_db_escape_string(hDB, hashAlgorithm, STANDARD_ESCAPING, &escapedHashAlgorithm);
	if(retVal < BMD_OK)
	{
		free(escapedBasedHashValue); escapedBasedHashValue=NULL;
		BMD_FOK(retVal);
	}
	
	asprintf(&updateQuery, queryTemplate, escapedHashAlgorithm, escapedBasedHashValue, signatureId, _GLOBAL_bmd_configuration->location_id);
	free(escapedBasedHashValue); escapedBasedHashValue=NULL;
	free(escapedHashAlgorithm); escapedHashAlgorithm=NULL;
	if(updateQuery == NULL)
		{ BMD_FOK(BMD_ERR_MEMORY); }
	
	retVal=bmd_db_execute_sql(hDB, updateQuery, NULL, NULL, &queryResult );
	free(updateQuery); updateQuery=NULL;
	bmd_db_result_free(&queryResult);
	BMD_FOK(retVal);
	
	return BMD_OK;
}


long prfParseAndInsertSignatureinXades(	void *hDB,
					char *crypto_objects_id,
					const WhoInsertIt_t signature_userorsystem,
					bmd_crypt_ctx_t *sym_ctx,
					GenBuf_t *signature_genbuf,
					char **signature_id)
{
PKIStandardEnum_t signature_standard	= STANDARD_XADES;
GenBuf_t* enc_signature			= NULL;
xmlDocPtr document 			= NULL;



	/*************************/
	/* walidacja parametrow  */
	/*************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}


	/* load file */
	document = xmlParseMemory(signature_genbuf->buf,signature_genbuf->size);
	if ((document == NULL) || (xmlDocGetRootElement(document) == NULL))
	{
		PRINT_ERROR("Unable to parse XML document. Error = %d\n", BMD_ERR_OP_FAILED);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	xmlFreeDoc(document); document=NULL;

	/***************************************************************/
	/* tu by sie przydalo szyfrowac wrzucany do bazy danych podpis */
	/***************************************************************/
	BMD_FOK(bmdpki_symmetric_encrypt(sym_ctx, signature_genbuf, BMDPKI_SYM_LAST_CHUNK, &enc_signature));

	/**********************************************/
	/* wreszcie wstaw sam plik koperty z podpisem */
	/**********************************************/
	BMD_FOK(prfInsertSignature(	hDB,
					crypto_objects_id,
					signature_standard,
					signature_userorsystem,
					enc_signature,
					signature_id));

	return BMD_OK;
}



long prfParseSignatureStandard(	GenBuf_t *signature_genbuf, PKIStandardEnum_t *signature_standard)
{
	long retVal=0;
	ContentInfo_t *tempContent=NULL;

	retVal=get_CMScontent_from_GenBuf(signature_genbuf, &tempContent);
	if(retVal < BMD_OK)
	{
		return BMD_ERR_FORMAT;
	}
	else
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, tempContent, 1);
		free(tempContent); tempContent=NULL;
		*signature_standard = STANDARD_CMS;
	}

///TODO
// 	*signature_standard = STANDARD_PKCS7;
// 	*signature_standard = STANDARD_XADES;

	return BMD_OK;
}


long prfInsertSignature(	void *hDB,
				char *crypto_objects_id,
				const PKIStandardEnum_t signature_standard,
				const WhoInsertIt_t signature_userorsystem,
				GenBuf_t *signature_genbuf,
				char **signature_id)
{
char *signature_lobjId	= NULL;

	PRINT_INFO("LIBBMDSQLINF Inseting signature to database\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_standard < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_userorsystem < 0)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (signature_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (signature_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (signature_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	BMD_FOK(bmd_db_export_blob(hDB, signature_genbuf, &signature_lobjId));

	BMD_FOK(prfInsertIntoSignaturesTable(	hDB,
						crypto_objects_id,
						signature_standard,
						signature_userorsystem,
						signature_lobjId,
						signature_id));

	free0(signature_lobjId);

	return BMD_OK;
}

long prfInsertIntoSignaturesTable(	void *hDB,
					char *crypto_objects_id,
					const PKIStandardEnum_t signature_standard,
					const WhoInsertIt_t signature_userorsystem,
					char *signature_lobjId,
					char **signature_id)
{
char *SQLQuery		= NULL;
char *SequenceNextVal	= NULL;
void *query_result	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crypto_objects_id==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (signature_standard < 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (signature_userorsystem < 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (signature_lobjId==NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (signature_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if ((*signature_id)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM6);	}


	if ( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
	{
		SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "prf_signatures_id_seq");

		asprintf(&SQLQuery,"SELECT %s FROM prf_signatures_id_seq;", SequenceNextVal);

		BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, signature_id));
	}
	else
		if ( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
		{
			void *hRes=NULL;
			long sf=0;
			bmd_db_execute_sql(hDB,"SELECT prf_signatures_id_seq.nextval FROM DUAL",NULL,NULL,&hRes);
			bmd_db_result_get_value(hDB,hRes,0,0,signature_id,FETCH_NEXT,&sf);
			bmd_db_result_free(&hRes);
		}
	if ( (*signature_id)==NULL)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in executing sql query with known DB connection. Error=%i\n",-1);
		return -1;
	}

	asprintf(&SQLQuery, 	"INSERT INTO prf_signatures (id%s, fk_crypto_objects, standard, userorsystem, signature) VALUES ( %s%s%s, %s, %i, %i, %s);",
					_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
					(*signature_id),
					_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
					crypto_objects_id,
					signature_standard,
					signature_userorsystem,
					signature_lobjId);

	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &query_result));

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&query_result);
	free0(SQLQuery);
	free0(SequenceNextVal);

	return BMD_OK;
}

