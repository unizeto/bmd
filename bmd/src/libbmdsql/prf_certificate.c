// #define _WINSOCK2API_
// #include<bmd/libbmdsql/proofs.h>
// #include<bmd/common/bmd-oids.h>
// #include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
// #include<bmd/libbmdX509/libbmdX509.h>
// #include<bmd/libbmderr/libbmderr.h>
// #include<bmd/libbmdglobals/libbmdglobals.h>
// 
// #ifdef WIN32
// #pragma warning(disable:4100)
// #endif
// 
// long prfParseAndInsertCert(	void *hDB,
// 				char *crypto_objects_id,
// 				const WhoInsertIt_t cert_user_or_system,
// 				GenBuf_t *cert_genbuf)
// {
// long cert_version		= 0;
// char *cert_serialNumber		= NULL;
// char *cert_subjectDN		= NULL;
// char *cert_signature_alg	= NULL;
// char *cert_issuerDN		= NULL;
// char *cert_validitynotBefore	= NULL;
// char *cert_validitynotAfter	= NULL;
// 
// 	/************************/
// 	/* walidacja parametrow */
// 	/************************/
// 	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (crypto_objects_id==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (cert_user_or_system < 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (cert_genbuf == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_genbuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_genbuf->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}
// 
// 	BMD_FOK(prfParseCertVersion(cert_genbuf, &cert_version));
// 	BMD_FOK(prfParseCertSerialNumber(cert_genbuf, &cert_serialNumber));
// 	BMD_FOK(prfParseCertSubjectDN(cert_genbuf, &cert_subjectDN));
// 	BMD_FOK(prfParseCertSignatureAlg(cert_genbuf, &cert_signature_alg));
// 	BMD_FOK(prfParseCertIssuerDN(cert_genbuf, &cert_issuerDN));
// 	BMD_FOK(prfParseCertValidityNotBefore(cert_genbuf, &cert_validitynotBefore));
// 	BMD_FOK(prfParseCertValidityNotAfter(cert_genbuf, &cert_validitynotAfter));
// 	BMD_FOK(prfInsertCoPrfCerts (	hDB,
// 					crypto_objects_id,
// 					cert_version,
// 					cert_serialNumber,
// 					cert_subjectDN,
// 					cert_signature_alg,
// 					cert_issuerDN,
// 					cert_validitynotBefore,
// 					cert_validitynotAfter,
// 					cert_genbuf));
// 
// 	free0(cert_serialNumber);
// 	free0(cert_signature_alg);
// 	free0(cert_subjectDN);
// 	free0(cert_issuerDN);
// 	free0(cert_validitynotBefore);
// 	free0(cert_validitynotAfter);
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertVersion(	GenBuf_t *cert_genbuf,
// 				long *cert_version)
// {
// char *AttributeString	= NULL;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_VERSION, &AttributeString, cert_version));
// 
// 	return BMD_OK;
// }
// 
// 
// long prfParseCertSerialNumber(	GenBuf_t *cert_genbuf,
// 					char **cert_serialNumber)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_SN, cert_serialNumber, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertSubjectDN(	GenBuf_t *cert_genbuf,
// 					char **cert_subjectDN)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_SUBJECT_DN, cert_subjectDN, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertSignatureAlg(	GenBuf_t *cert_genbuf,
// 					char **cert_signature_alg)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_SIGALG_OID, cert_signature_alg, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertIssuerDN(	GenBuf_t *cert_genbuf,
// 					char **cert_issuerDN)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_ISSUER_DN, cert_issuerDN, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertValidityNotBefore(	GenBuf_t *cert_genbuf,
// 						char **cert_validitynotBefore)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_VALIDITY_NB, cert_validitynotBefore, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// long prfParseCertValidityNotAfter(GenBuf_t *cert_genbuf, char **cert_validitynotAfter)
// {
// long AttributeLong	= 0;
// 
// 	BMD_FOK(X509CertGetAttr(cert_genbuf, X509_ATTR_VALIDITY_NA, cert_validitynotAfter, &AttributeLong));
// 
// 	return BMD_OK;
// }
// 
// 
// /* Funkcja wstawia zawartość certyfikat do prf_crls i dane do co_prf_certs.	*/
// /* Nie powinno sie w niej tworzyc transakcji z powodu ochrony sekwenserow.	*/
// long prfInsertCoPrfCerts (	void *hDB,
// 				char *crypto_objects_id,
// 				const long cert_version,
// 				const char *cert_serialNumber,
// 				const char *cert_subjectDN,
// 				const char *cert_signature_alg,
// 				const char *cert_issuerDN,
// 				const char *cert_validitynotBefore,
// 				const char *cert_validitynotAfter,
// 				GenBuf_t *cert_genbuf)
// {
// char *SQLQuery				= NULL;
// char *CryptoObjectsSequenceCurrVal	= NULL;
// char *prfCertsSequenceCurrVal		= NULL;
// void *query_result			= NULL;
// char *cert_id				= NULL;
// 
// 	/************************/
// 	/* walidacja parametrow */
// 	/************************/
// 	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (cert_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (cert_serialNumber == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (cert_subjectDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
// 	if (cert_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
// 	if (cert_validitynotBefore == NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
// 	if (cert_validitynotAfter == NULL)	{	BMD_FOK(BMD_ERR_PARAM8);	}
// 	if (cert_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 	if (cert_genbuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 	if (cert_genbuf->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 
// 	/************************************************************/
// 	/*	jesli wstawiany certyfikat nie istnieje to go dodaj	*/
// 	/************************************************************/
// 	BMD_FOK(prfCheckIfCertAlreadyExists(	hDB,
// 						cert_version,
// 						cert_serialNumber,
// 						cert_subjectDN,
// 						cert_signature_alg,
// 						cert_issuerDN,
// 						cert_validitynotBefore,
// 						cert_validitynotAfter,
// 						&cert_id));
// 
// 	/******************************************************/
// 	/*	certyfikat nie istnieje w bazie wiec go wstaw	*/
// 	/******************************************************/
// 	if (cert_id == NULL)
// 	{
// 		BMD_FOK(prfInsertCert(	hDB,
// 					cert_version,
// 					cert_serialNumber,
// 					cert_subjectDN,
// 					cert_signature_alg,
// 					cert_issuerDN,
// 					cert_validitynotBefore,
// 					cert_validitynotAfter,
// 					cert_genbuf));
// 
// 		prfCertsSequenceCurrVal=bmd_db_get_sequence_currval(hDB, "prf_certs_id_seq");
// 		if (prfCertsSequenceCurrVal==NULL) 	{	BMD_FOK(NO_MEMORY);	}
// 
// 		asprintf(&SQLQuery, "	INSERT INTO co_prf_certs (id, fk_crypto_objects, fk_prf_certs%s)VALUES (nextval(\'co_prf_certs_id_seq\'), %s, %s%s%s);",
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
// 						crypto_objects_id,
// 						prfCertsSequenceCurrVal,
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
// 	}
// 	else
// 	{
// 		asprintf(&SQLQuery, "	INSERT INTO co_prf_certs (id, fk_crypto_objects, fk_prf_certs%s)VALUES (nextval(\'co_prf_certs_id_seq\'), %s, %s%s%s);",
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
// 						crypto_objects_id,
// 						cert_id,
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
// 						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
// 	}
// 
// 	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery, NULL,NULL, &query_result));
// 
// 	bmd_db_result_free(&query_result);
// 	free(SQLQuery); SQLQuery=NULL;
// 	free(CryptoObjectsSequenceCurrVal); CryptoObjectsSequenceCurrVal = NULL;
// 	free(prfCertsSequenceCurrVal); prfCertsSequenceCurrVal = NULL;
// 
// 	return BMD_OK;
// }
// 
// 
// /* Funkcja sprawdza czy certyfikat aktualnie znajduje sie w bazie. Jesli tak to w cert_id jest jego id */
// long prfCheckIfCertAlreadyExists(	void *hDB,
// 					const long cert_version,
// 					const char *cert_serialNumber,
// 					const char *cert_subjectDN,
// 					const char *cert_signature_alg,
// 					const char *cert_issuerDN,
// 					const char *cert_validitynotBefore,
// 					const char *cert_validitynotAfter,
// 					char **cert_id)
// {
// void *database_result			= NULL;
// char *SQLQuery				= NULL;
// char *dbcomp_cert_validitynotBefore	= NULL;
// char *dbcomp_cert_validitynotAfter	= NULL;
// long err				= 0;
// long fetched_counter1			= 0;
// 
// 	/******************************/
// 	/*	waidacja parametrow	*/
// 	/******************************/
// 	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (cert_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (cert_serialNumber == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (cert_subjectDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
// 	if (cert_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
// 	if (cert_validitynotBefore == NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
// 	if (cert_validitynotAfter == NULL)	{	BMD_FOK(BMD_ERR_PARAM8);	}
// 	if (cert_id==NULL || *cert_id != NULL)	{	BMD_FOK(BMD_ERR_PARAM9);	}
// 
// 
// 	BMD_FOK(bmd_db_date_to_timestamp(cert_validitynotBefore, &dbcomp_cert_validitynotBefore));
// 	BMD_FOK(bmd_db_date_to_timestamp(cert_validitynotAfter, &dbcomp_cert_validitynotAfter));
// 
// 	asprintf(&SQLQuery, "SELECT id FROM prf_certs WHERE version=%li AND serialNumber=\'%s\' AND subjectDN=\'%s\' "
// 			"AND signatureAlgOID=\'%s\' AND issuerDN=\'%s\' AND validitynotBefore=%s "
// 			"AND validitynotAfter=%s;",
// 			cert_version, cert_serialNumber, cert_subjectDN, cert_signature_alg,
// 			cert_issuerDN, dbcomp_cert_validitynotBefore, dbcomp_cert_validitynotAfter);
// 
// 	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery, NULL,NULL, &database_result));
// 
// 	/* pobierz id pierwszego certyfikatu spelniajacego kryteria */
// 	fetched_counter1 = 0;
// 	err=bmd_db_result_get_value(hDB, database_result, 0, 0, cert_id, FETCH_NEXT, &fetched_counter1);
// 	if (err<0 && err !=BMD_ERR_NODATA && err!=BMD_ERR_NODATA)
// 	{
// 		BMD_FOK(err);
// 	}
// 
// 	/******************/
// 	/*	porzadki	*/
// 	/******************/
// 	free(SQLQuery); SQLQuery=NULL;
// 	free(dbcomp_cert_validitynotBefore); dbcomp_cert_validitynotBefore=NULL;
// 	free(dbcomp_cert_validitynotAfter); dbcomp_cert_validitynotAfter=NULL;
// 	bmd_db_result_free(&database_result);
// 
// 	return BMD_OK;
// }
// 
// 
// /* Funkcja wstawia zawartość certyfikatu do BLOBa a pozostale metadane do tabeli prf_certs */
// long prfInsertCert(	void *hDB,
// 			const long cert_version,
// 			const char *cert_serialNumber,
// 			const char *cert_subjectDN,
// 			const char *cert_signature_alg,
// 			const char *cert_issuerDN,
// 			const char *cert_validitynotBefore,
// 			const char *cert_validitynotAfter,
// 			GenBuf_t *cert_genbuf)
// {
// char *cert_lobjId		= NULL;
// 
// 	/******************************/
// 	/*	walidacja parametrow	*/
// 	/******************************/
// 	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (cert_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (cert_serialNumber == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (cert_subjectDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
// 	if (cert_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
// 	if (cert_validitynotBefore == NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
// 	if (cert_validitynotAfter == NULL)	{	BMD_FOK(BMD_ERR_PARAM8);	}
// 	if (cert_genbuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 	if (cert_genbuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 	if (cert_genbuf->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM9);	}
// 
// 	BMD_FOK(bmd_db_export_blob(hDB, cert_genbuf, &cert_lobjId));
// 
// 	BMD_FOK(prfInsertIntoCertTable(	hDB,
// 					cert_version,
// 					cert_serialNumber,
// 					cert_subjectDN,
// 					cert_signature_alg,
// 					cert_issuerDN,
// 					cert_validitynotBefore,
// 					cert_validitynotAfter,
// 					cert_lobjId));
// 
// 	return BMD_OK;
// }
// 
// long prfInsertIntoCertTable(	void *hDB,
// 				const long cert_version,
// 				const char *cert_serialNumber,
// 				const char *cert_subjectDN,
// 				const char *cert_signature_alg,
// 				const char *cert_issuerDN,
// 				const char *cert_validitynotBefore,
// 				const char *cert_validitynotAfter,
// 				char *cert_lobjId)
// {
// char *SQLQuery				= NULL;
// char *SequenceNextVal			= NULL;
// void *query_result			= NULL;
// char *dbcomp_cert_validitynotBefore	= NULL;
// char *dbcomp_cert_validitynotAfter	= NULL;
// /*long rows					= 0;
// long cols					= 0;*/
// const char *proofs_certs_table_name	= "prf_certs";
// 
// 	/******************************/
// 	/*	walidacja parametrow	*/
// 	/******************************/
// 	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (cert_version < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if (cert_serialNumber == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if (cert_subjectDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if (cert_signature_alg == NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
// 	if (cert_issuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
// 	if (cert_validitynotBefore == NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
// 	if (cert_validitynotAfter == NULL)	{	BMD_FOK(BMD_ERR_PARAM8);	}
// 
// 	SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "prf_certs_id_seq");
// 	if (SequenceNextVal==NULL) return NO_MEMORY;
// 
// 	BMD_FOK(bmd_db_date_to_timestamp(cert_validitynotBefore, &dbcomp_cert_validitynotBefore));
// 	BMD_FOK(bmd_db_date_to_timestamp(cert_validitynotAfter, &dbcomp_cert_validitynotAfter));
// 
// 	asprintf(&SQLQuery, "INSERT INTO %s (id, version, serialNumber, subjectdn, signatureAlgOID, "
// 			"issuerdn, validitynotbefore, validitynotafter, cert%s) "
// 			"VALUES ( %s, %li, \'%s\', \'%s\', \'%s\', \'%s\', %s, %s,  %s%s%s);",
// 			proofs_certs_table_name,
// 			_GLOBAL_bmd_configuration->location_id==NULL?"":", location_id",
// 			SequenceNextVal,
// 			cert_version,
// 			cert_serialNumber,
// 			cert_subjectDN,
// 			cert_signature_alg,
// 			cert_issuerDN,
// 			dbcomp_cert_validitynotBefore,
// 			dbcomp_cert_validitynotAfter,
// 			cert_lobjId,
// 			_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
// 			_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
// 
// 
// 
// 	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &query_result));
// 
// 	/******************/
// 	/*	porzadki	*/
// 	/******************/
// 	free(dbcomp_cert_validitynotBefore); dbcomp_cert_validitynotBefore=NULL;
// 	free(dbcomp_cert_validitynotAfter); dbcomp_cert_validitynotAfter=NULL;
// 	free(SequenceNextVal); SequenceNextVal=NULL;
// 	bmd_db_result_free(&query_result);
// 	free(SQLQuery); SQLQuery=NULL;
// 
// 	return BMD_OK;
// }
