#define _WINSOCK2API_
#include<bmd/libbmdsql/proofs.h>
#include<bmd/common/bmd-oids.h>
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdcrl/libbmdcrl.h>
#include<bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

long prfParseAndInsertCrl(	void *hDB,
				GenBuf_t *crl_genbuf,
				CrlTypeEnum_t crl_type)
{
char *crl_signature_alg		= NULL;
char *crl_issuerDN		= NULL;
char *crl_thisUpdate		= NULL;
char *crl_nextUpdate		= NULL;
char *crl_number		= NULL;
char *crl_CompleteOrDelta 	= NULL;
long crl_noOfRevokedEntries 	= 0;
long crl_version		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (crl_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_type < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(prfParseCrlVersion(crl_genbuf, &crl_version));
	BMD_FOK(prfParseCrlNumber(crl_genbuf, &crl_number));
	BMD_FOK(prfParseCrlIsDeltaCrl(crl_genbuf, &crl_CompleteOrDelta));
	BMD_FOK(prfParseCrlSignatureAlg(crl_genbuf, &crl_signature_alg));
	BMD_FOK(prfParseCrlIssuerDN(crl_genbuf, &crl_issuerDN));
	BMD_FOK(prfParseCrlThisUpdate(crl_genbuf, &crl_thisUpdate));
	BMD_FOK(prfParseCrlNextUpdate(crl_genbuf, &crl_nextUpdate));
	BMD_FOK(prfParseCrlNoOfRevokedEntries(crl_genbuf, &crl_noOfRevokedEntries));
	BMD_FOK(prfInsertCrl(hDB, crl_version, crl_number, crl_CompleteOrDelta,crl_signature_alg,
		crl_issuerDN, crl_thisUpdate,crl_nextUpdate, crl_noOfRevokedEntries, crl_genbuf, crl_type));

	/************/
	/* porzadki */
	/************/
	free0(crl_signature_alg);
	free0(crl_issuerDN);
	free0(crl_thisUpdate);
	free0(crl_nextUpdate);
	free0(crl_number);
	free0(crl_CompleteOrDelta);

	return BMD_OK;
}

long prfParseCrlVersion(	GenBuf_t *crl_genbuf,
				long *crl_version)
{
long err			= 0;
char *AttributeString 	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crl_version == NULL || *crl_version != 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	err = CRLGetAttr(crl_genbuf, CRL_ATTR_VERSION, &AttributeString, crl_version);
	/* Jesli nie podano wersji CRLa (bo jest ona opcjonalna) to jako wersje wstaw nr '0' */
	if (err == ERR_CRL_VERSION_NOT_PRESENT) {*crl_version=0; err=0;}

	BMD_FOK(err);

	return BMD_OK;
}

long prfParseCrlNumber(	GenBuf_t *crl_genbuf,
				char **crl_number)
{
long err				= 0;
long AttributeLong		= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crl_number == NULL || *crl_number != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* Jesli nie podano numeru CRLa (bo jest ona opcjonalna) to jako numer wstaw "0" */
	err = CRLGetAttr(crl_genbuf, CRL_ATTR_SN, crl_number, &AttributeLong);
	if (err == ERR_CRL_EXTENSIONS_NOT_PRESENT)
	{
		asprintf(crl_number, "-1");
		err=0;
	}

	BMD_FOK(err);

	return BMD_OK;
}

long prfParseCrlIsDeltaCrl(	GenBuf_t *crl_genbuf,
					char **crl_CompleteOrDelta)
{
long err			= 0;
long AttributeLong	= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (	crl_CompleteOrDelta == NULL ||
		*crl_CompleteOrDelta != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* Jesli nie podano wskaznika delty CRLa (bo jest on opcjonalny) to jako numer wstaw "0" */
	err = CRLGetAttr(crl_genbuf, CRL_ATTR_DELTA, crl_CompleteOrDelta, &AttributeLong);
	if (err == ERR_CRL_EXTENSIONS_NOT_PRESENT)
	{
		asprintf(crl_CompleteOrDelta, "-1");
		err=0;
	}

	BMD_FOK(err);

	return BMD_OK;
}

long prfParseCrlSignatureAlg(	GenBuf_t *crl_genbuf,
					char **crl_signature_alg)
{
long AttributeLong	= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (	crl_signature_alg == NULL ||
		*crl_signature_alg != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(CRLGetAttr(crl_genbuf, CRL_ATTR_SIGALG_OID, crl_signature_alg, &AttributeLong));

	return BMD_OK;
}

long prfParseCrlIssuerDN(	GenBuf_t *crl_genbuf,
					char **crl_issuerDN)
{
long AttributeLong	= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (	crl_issuerDN == NULL ||
		*crl_issuerDN != NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(CRLGetAttr(crl_genbuf, CRL_ATTR_ISSUER_DN, crl_issuerDN, &AttributeLong));

	return BMD_OK;
}

long prfParseCrlThisUpdate(	GenBuf_t *crl_genbuf,
					char **crl_thisUpdate)
{
long AttributeLong	= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}

	if (	crl_thisUpdate == NULL ||
		*crl_thisUpdate != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	/*	return BMD_ERR_PARAM2;*/

	BMD_FOK(CRLGetAttr(crl_genbuf, CRL_ATTR_THIS_UPDATE, crl_thisUpdate, &AttributeLong));

	return BMD_OK;
}

long prfParseCrlNextUpdate(	GenBuf_t *crl_genbuf,
					char **crl_nextUpdate)
{
long AttributeLong = 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}

	if (	crl_nextUpdate == NULL ||
		*crl_nextUpdate != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(CRLGetAttr(crl_genbuf, CRL_ATTR_NEXT_UPDATE, crl_nextUpdate, &AttributeLong));

	return BMD_OK;
}

long prfParseCrlNoOfRevokedEntries(	GenBuf_t *crl_genbuf,
						long *crl_noOfRevokedEntries)
{
char *AttributeString = NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (	crl_genbuf == NULL ||
		crl_genbuf->buf == NULL ||
		crl_genbuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}

	if (	crl_noOfRevokedEntries == NULL ||
		*crl_noOfRevokedEntries < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(CRLGetAttr(crl_genbuf, CRL_ATTR_NUMBER_OF_REVOKED_CERTS, &AttributeString, crl_noOfRevokedEntries));

	return BMD_OK;
}


long prfInsertCrl(	void *hDB,
			const long crl_version,
			const char *crl_number,
			const char *crl_CompleteOrDelta,
			const char *crl_signature_alg,
			const char *crl_issuerDN,
			const char *crl_thisUpdate,
			const char *crl_nextUpdate,
			const long crl_noOfRevokedEntries,
			GenBuf_t *crl_genbuf,
			CrlTypeEnum_t crl_type)
{
long err		= 0;
char *crl_lobjId	= NULL;
char *crl_id		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crl_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_number == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (crl_CompleteOrDelta == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (crl_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (crl_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (crl_thisUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (crl_nextUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (crl_noOfRevokedEntries < 0)		{	BMD_FOK(BMD_ERR_PARAM9);	}
	if (crl_genbuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (crl_genbuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (crl_genbuf->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (crl_type < 0)			{	BMD_FOK(BMD_ERR_PARAM11);	}

	err = prfCheckIfCrlAlreadyExists(	hDB,
						crl_version, crl_number, crl_CompleteOrDelta, crl_signature_alg,
						crl_issuerDN, crl_thisUpdate, crl_nextUpdate, crl_noOfRevokedEntries,
						crl_type, &crl_id);
	if (err < 0 || crl_id < 0)
	{
		BMD_FOK(err);
	}
	if (crl_id == 0)
	{ /* CRL/ARL nie istnieje w bazie wiec go wstaw */

		BMD_FOK(bmd_db_export_blob(hDB, crl_genbuf, &crl_lobjId));
		BMD_FOK(prfInsertIntoCrlsTable(	hDB,
								crl_version, crl_number, crl_CompleteOrDelta, crl_signature_alg,
								crl_issuerDN, crl_thisUpdate, crl_nextUpdate,
								crl_noOfRevokedEntries, crl_lobjId, crl_type));
	}

	/************/
	/* porzadki */
	/************/
	free(crl_lobjId); crl_lobjId=NULL;
	free(crl_id); crl_id=NULL;

	return BMD_OK;
}


long prfCheckIfCrlAlreadyExists(	void *hDB,
					const long crl_version,
					const char *crl_number,
					const char *crl_CompleteOrDelta,
					const char *crl_signature_alg,
					const char *crl_issuerDN,
					const char *crl_thisUpdate,
					const char *crl_nextUpdate,
					const long crl_noOfRevokedEntries,
					CrlTypeEnum_t crl_type,
					char **crl_id)
{
void *database_result			= NULL;
char *SQLQuery				= NULL;

long err				= 0;
long fetched_counter1			= 0;
const char *proofs_crls_table_name	= "prf_crls";
const char *proofs_arls_table_name	= "prf_arls";

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crl_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_number == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (crl_CompleteOrDelta == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (crl_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (crl_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (crl_thisUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (crl_nextUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (crl_noOfRevokedEntries < 0)		{	BMD_FOK(BMD_ERR_PARAM9);	}
	if (crl_type < 0)			{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (crl_id==NULL)			{	BMD_FOK(BMD_ERR_PARAM11);	}
	if (*crl_id != NULL)			{	BMD_FOK(BMD_ERR_PARAM11);	}

	switch(crl_type)
	{
		case TYPE_CRL:
			asprintf(&SQLQuery, "SELECT id FROM %s WHERE "
				"version=%li AND crlserialnumber=\'%s\' AND deltacrl=\'%s\' "
				"AND signatureAlgOID=\'%s\' AND issuerdn=\'%s\' "
				"AND thisupdate=\'%s\' AND nextupdate=\'%s\' AND numberOfRevokedEntries=%li ;",
				proofs_crls_table_name, crl_version, crl_number, crl_CompleteOrDelta,
				crl_signature_alg, crl_issuerDN,
				crl_thisUpdate, crl_nextUpdate, crl_noOfRevokedEntries);
			break;
		case TYPE_ARL:
			asprintf(&SQLQuery, "SELECT id FROM %s WHERE "
				"version=%li AND arlserialnumber=\'%s\' AND deltaarl=\'%s\' "
				"AND signatureAlgOID=\'%s\' AND issuerdn=\'%s\' "
				"AND thisupdate=\'%s\' AND nextupdate=\'%s\' AND numberOfRevokedEntries=%li ;",
				proofs_arls_table_name, crl_version, crl_number, crl_CompleteOrDelta,
				crl_signature_alg, crl_issuerDN,
				crl_thisUpdate, crl_nextUpdate, crl_noOfRevokedEntries);
			break;
	}

	err = bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
	if (err<0 )		{	BMD_FOK(err);	}

	/******************************************************************/
	/*	pobierz id pierwszego certyfikatu spelniajacego kryteria	*/
	/******************************************************************/
	fetched_counter1 = 0;
	err = bmd_db_result_get_value(hDB, database_result, 0, 0, crl_id, FETCH_NEXT, &fetched_counter1);
	if (err<0 && err !=BMD_ERR_NODATA && err!=BMD_ERR_NODATA)
	{
		BMD_FOK(err);
	}
	bmd_db_result_free(&database_result);
	free(SQLQuery); SQLQuery=NULL;

	if (err==BMD_ERR_NODATA)
	{
		err=0;
		(*crl_id)=NULL;
	}

	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

long prfInsertIntoCrlsTable(	void *hDB,
					const long crl_version,
					const char *crl_number,
					const char *crl_CompleteOrDelta,
					const char *crl_signature_alg,
					const char *crl_issuerDN,
					const char *crl_thisUpdate,
					const char *crl_nextUpdate,
					const long crl_noOfRevokedEntries,
					char *crl_lobjId,
					CrlTypeEnum_t crl_type)
{
char *SQLQuery				= NULL;
char *SequenceNextVal			= NULL;
void *query_result			= NULL;
char *dbcomp_crl_thisUpdate		= NULL;
char *dbcomp_crl_nextUpdate		= NULL;
const char *proofs_crls_table_name 	= "prf_crls";
const char *proofs_arls_table_name	= "prf_arls";

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (crl_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (crl_number == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (crl_CompleteOrDelta == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (crl_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (crl_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (crl_thisUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (crl_nextUpdate == NULL)		{	BMD_FOK(BMD_ERR_PARAM8);	}
	if (crl_noOfRevokedEntries < 0)		{	BMD_FOK(BMD_ERR_PARAM9);	}
	if (crl_lobjId ==NULL)			{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (strtoul(crl_lobjId, NULL, 10) < 1)		{	BMD_FOK(BMD_ERR_PARAM10);	}
	if (crl_type < 0)			{	BMD_FOK(BMD_ERR_PARAM11);	}

	BMD_FOK(bmd_db_date_to_timestamp(crl_thisUpdate, &dbcomp_crl_thisUpdate));
	BMD_FOK(bmd_db_date_to_timestamp(crl_nextUpdate, &dbcomp_crl_nextUpdate));

	switch(crl_type)
	{
		case TYPE_CRL:
			SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "prf_crls_id_seq");
			if (SequenceNextVal==NULL) return NO_MEMORY;

			asprintf(&SQLQuery, "INSERT INTO %s (id, version, crlserialnumber, deltacrl, signatureAlgOID, issuerdn, "
				"thisupdate, nextupdate, numberOfRevokedEntries, crl)"
				"VALUES ( %s, %li, \'%s\', \'%s\', \'%s\', \'%s\', %s, %s, %li, %s);",
				proofs_crls_table_name, SequenceNextVal, crl_version, crl_number, crl_CompleteOrDelta,
				crl_signature_alg, crl_issuerDN, dbcomp_crl_thisUpdate, dbcomp_crl_nextUpdate,
				crl_noOfRevokedEntries, crl_lobjId);
			break;
		case TYPE_ARL:
			SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "prf_arls_id_seq");
			if (SequenceNextVal==NULL) return NO_MEMORY;

			asprintf(&SQLQuery, "INSERT INTO %s (id, version, arlserialnumber, deltaarl, signatureAlgOID, issuerdn, "
				"thisupdate, nextupdate, numberOfRevokedEntries, arl)"
				"VALUES ( %s, %li, \'%s\', \'%s\', \'%s\', \'%s\', %s, %s, %li, %s);",
				proofs_arls_table_name, SequenceNextVal, crl_version, crl_number, crl_CompleteOrDelta,
				crl_signature_alg, crl_issuerDN, dbcomp_crl_thisUpdate, dbcomp_crl_nextUpdate,
				crl_noOfRevokedEntries, crl_lobjId);
		break;
	}

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);

	/************/
	/* porzadki */
	/************/
	free0(SequenceNextVal);
	free0(dbcomp_crl_thisUpdate);
	free0(dbcomp_crl_nextUpdate);
	free0(SQLQuery);

	return BMD_OK;
}
