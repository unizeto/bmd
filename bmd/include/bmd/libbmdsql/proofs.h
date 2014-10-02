
#ifndef _NEW_BMD_SERVER_PROOFS_H_INCLUDED_
#define _NEW_BMD_SERVER_PROOFS_H_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include<bmd/libbmd/libbmd.h>
#include<bmd/libbmddb/libbmddb.h>
#include<bmd/libbmdts/libbmdts.h>


typedef enum CrlTypeEnum { TYPE_CRL, TYPE_ARL } CrlTypeEnum_t;
typedef enum CrlCompleteOrDelta  { COMPLETE_CRL, DELTA_CRL} CrlCompleteOrDelta_t;
typedef enum PKIStandardEnum { STANDARD_UNKNOWN=0, STANDARD_CMS=1, STANDARD_PKCS7=2, STANDARD_XADES=3, STANDARD_PADES=4 } PKIStandardEnum_t;
typedef enum WhoInsertIt { INSERT_USER=0, INSERT_BMD=1 } WhoInsertIt_t;

/****************
 * DVCS section *
 ***************/
LIBBMDSQL_SCOPE long prfInsertDVCS(	void *hDB,
					char *cryptoObjectId,
					char *signature_id,
					GenBuf_t *dvcs_genbuf,
					char *verify_status,
					bmd_crypt_ctx_t *sym_ctx,
					char **dvcs_id);

LIBBMDSQL_SCOPE long prfInsertIntoDVCSTable(	void *hDB,
						char *cryptoObjectId,
						char *signature_id,
						char *dvcs_lobjId,
						char *verify_status,
						char **dvcs_id);

/*********************
 * Timestamp section *
 ********************/
LIBBMDSQL_SCOPE long prfMaintainAllTimestamps(void *hDB,long   years_before_expiration,long  months_before_expiration,long    days_before_expiration,long   hours_before_expiration,long minutes_before_expiration,long seconds_before_expiration,TimestampType_t TimestampType,ConservationConfig_t *twf_ConservationConfig,struct GenBufList *list,const char * timestamp_metadata_oid_str);

LIBBMDSQL_SCOPE long prfTimestampClearConservationId(void *db_connection_handle);

LIBBMDSQL_SCOPE long prfSelectTimestampsForConservation(void *hDB,long   years_before_expiration, long  months_before_expiration,long    days_before_expiration, long   hours_before_expiration,long minutes_before_expiration, long seconds_before_expiration,void **query_result,long round_size,char *objects_ids,char * timestamp_metadata_oid_str);

LIBBMDSQL_SCOPE long prfGetTimestampKeyData(	void *hDB,
						long *fetched_counter,
						void *query_result,
						char **lo_oid,
						char **fk_crypto_objects,
						char **fk_objects,
						char **timestamp_id,
						char **timestamp_type);

LIBBMDSQL_SCOPE long prfImportTimestampToGenBuf(	void *hDB,
							char *lo_oid,
							GenBuf_t **plainDataGenBuf);

LIBBMDSQL_SCOPE long prfUpdateConservedTimestampStatus(	void *hDB,
							char *timestamp_id,
							char *new_timestamp_id);

LIBBMDSQL_SCOPE long prfParseTimestampVersion(	GenBuf_t *timestamp_genbuf,
						long *timestamp_version,
						ConservationConfig_t *twf_ConservationConfig);

LIBBMDSQL_SCOPE long prfParseTimestampPolicyOID(	GenBuf_t *timestamp_genbuf,
							char **timestamp_policy_oid,
							ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampSN(	GenBuf_t *timestamp_genbuf,
						char **timestamp_SN,
						ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampTsaSN(	GenBuf_t *timestamp_genbuf,
						char **timestamp_tsa_SN,
						ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampTsaDN(	GenBuf_t *timestamp_genbuf,
						char **timestamp_tsa_DN,
						ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampGenTime(	GenBuf_t *timestamp_genbuf,
						char **timestamp_genTime,
						ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampCartValidTo(	GenBuf_t *timestamp_genbuf,
							char **timestamp_certValidTo,
							ConservationConfig_t *twf_ConservationConfig);
LIBBMDSQL_SCOPE long prfParseTimestampStandard(	GenBuf_t *timestamp_genbuf,
						PKIStandardEnum_t *timestamp_standard);

LIBBMDSQL_SCOPE long prfParseAndInsertTimestamp(	void *hDB,
							char *crypto_objects_id,
							char *objects_id,
							const WhoInsertIt_t timestamp_user_or_system,
							GenBuf_t *timestamp_genbuf,
							const char * timestamp_metadata_oid_str,
							char **new_timestamp_id,
							long twf_conserved,
							ConservationConfig_t *twf_ConservationConfig);

LIBBMDSQL_SCOPE long prfInsertTimestamp(	void *hDB,
						char *crypto_objects_id,
						char *objects_id,
						const long timestamp_version,
						const char *timestamp_policy_oid,
						const char *timestamp_serialNumber,
						const char *timestamp_genTime,
						const char *timestamp_certValidTo,
						const char *timestamp_tsaDN,
						const char *timestamp_tsaSN,
						const PKIStandardEnum_t timestamp_standard,
						const WhoInsertIt_t timestamp_userorsystem,
						GenBuf_t *timestamp_genbuf,
						const char * timestamp_metadata_oid_str,
						char **new_timestamp_id,
						long twf_conserved);

LIBBMDSQL_SCOPE long prfInsertIntoTimestampsTable(	void *hDB,
							char *crypto_objects_id,
							char *objects_id,
							const long timestamp_version,
							const char *timestamp_policy_oid,
							const char *timestamp_serialNumber,
							const char *timestamp_genTime,
							const char *timestamp_certValidTo,
							const char *timestamp_tsaDN,
							const char *timestamp_tsaSN,
							const PKIStandardEnum_t timestamp_standard,
							const WhoInsertIt_t timestamp_userorsystem,
							char *timestamp_lobjId,
							const char * timestamp_metadata_oid_str,
							char **new_timestamp_id,
							long twf_conserved);

/*********************
 * Signature section *
 ********************/
LIBBMDSQL_SCOPE long prfParseSignatureStandard(	GenBuf_t *signature_genbuf,
						PKIStandardEnum_t *signature_standard);

LIBBMDSQL_SCOPE long prfParseAndInsertSignature(	void *hDB,
							char *crypto_objects_id,
							const WhoInsertIt_t signature_user_or_system,
							bmd_crypt_ctx_t *sym_ctx,
							GenBuf_t *signature_genbuf,
							char **signature_id);

LIBBMDSQL_SCOPE long prfParseAndInsertSignatureinXades(	void *hDB,
							char *crypto_objects_id,
							const WhoInsertIt_t signature_user_or_system,
							bmd_crypt_ctx_t *sym_ctx,
							GenBuf_t *signature_genbuf,
							char **signature_id);

LIBBMDSQL_SCOPE long prfParseAndInsertSignatureinPades(	void *hDB,
					char *crypto_objects_id,
					const WhoInsertIt_t signature_userorsystem,
					bmd_crypt_ctx_t *sym_ctx,
					GenBuf_t *signature_genbuf,
					char **signature_id);

LIBBMDSQL_SCOPE long GetPadesHashValueAndAlgorithm(bmdDatagram_t *requestDatagram, char** hexHashValue, char** hashAlgorithmOid);
long InsertPadesHashToDatabase(void* hDB, char* hexHashValue, char* hashAlgorithm, char* signatureId);

LIBBMDSQL_SCOPE long prfInsertIntoSignaturesTable(	void *hDB,
							char *crypto_objects_id,
							const PKIStandardEnum_t signature_standard,
							const WhoInsertIt_t signature_userorsystem,
							char *signature_lobjId,
							char **signature_id);

LIBBMDSQL_SCOPE long prfInsertSignature(	void *hDB,
						char *crypto_objects_id,
						const PKIStandardEnum_t signature_standard,
						const WhoInsertIt_t signature_userorsystem,
						GenBuf_t *signature_genbuf,
						char **signature_id);

/***********************
 * Certificate section *
 **********************/
// LIBBMDSQL_SCOPE long prfInsertCert(	void *hDB,
// 					const long cert_version,
// 					const char *cert_serialNumber,
// 					const char *cert_subjectDN,
// 					const char *cert_signature_alg,
// 					const char *cert_issuerDN,
// 					const char *cert_validitynotBefore,
// 					const char *cert_validitynotAfter,
// 					GenBuf_t *cert_genbuf);
// 
// LIBBMDSQL_SCOPE long prfCheckIfCertAlreadyExists(	void *hDB,
// 							const long cert_version,
// 							const char *cert_serialNumber,
// 							const char *cert_subjectDN,
// 							const char *cert_signature_alg,
// 							const char *cert_issuerDN,
// 							const char *cert_validitynotBefore,
// 							const char *cert_validitynotAfter,
// 							char **cert_id);
// 
// LIBBMDSQL_SCOPE long prfInsertIntoCertTable(	void *hDB,
// 						const long cert_version,
// 						const char *cert_serialNumber,
// 						const char *cert_subjectDN,
// 						const char *cert_signature_alg,
// 						const char *cert_issuerDN,
// 						const char *cert_validitynotBefore,
// 						const char *cert_validitynotAfter,
// 						char *cert_lobjId);
// 
//LIBBMDSQL_SCOPE long prfParseAndInsertCert(	void *hDB,
//						char *crypto_objects_id,
// 						const WhoInsertIt_t cert_user_or_system,
// 						GenBuf_t *cert_genbuf);
// 
// LIBBMDSQL_SCOPE long prfInsertCoPrfCerts (	void *hDB,
// 						char *crypto_objects_id,
// 						const long cert_version,
// 						const char *cert_serialNumber,
// 						const char *cert_subjectDN,
// 						const char *cert_signature_alg,
// 						const char *cert_issuerDN,
// 						const char *cert_validitynotBefore,
// 						const char *cert_validitynotAfter,
// 						GenBuf_t *cert_genbuf);
// 
// LIBBMDSQL_SCOPE long prfParseCertVersion(GenBuf_t *cert_genbuf, long *cert_version);
// LIBBMDSQL_SCOPE long prfParseCertSerialNumber(GenBuf_t *cert_genbuf, char **cert_serialNumber);
// LIBBMDSQL_SCOPE long prfParseCertSubjectDN(GenBuf_t *cert_genbuf, char **cert_subjectDN);
// LIBBMDSQL_SCOPE long prfParseCertSignatureAlg(GenBuf_t *cert_genbuf, char **cert_signature_alg);
// LIBBMDSQL_SCOPE long prfParseCertIssuerDN(GenBuf_t *cert_genbuf, char **cert_issuerDN);
// LIBBMDSQL_SCOPE long prfParseCertValidityNotBefore(GenBuf_t *cert_genbuf, char **cert_validitynotBefore);
// LIBBMDSQL_SCOPE long prfParseCertValidityNotAfter(GenBuf_t *cert_genbuf, char **cert_validitynotAfter);

/***************
 * CRL section *
 **************/
LIBBMDSQL_SCOPE long prfParseAndInsertCrl(	void *hDB,
						GenBuf_t *crl_genbuf,
						CrlTypeEnum_t crl_type);

LIBBMDSQL_SCOPE long prfInsertCrl(	void *hDB,
					const long crl_version,
					const char *crl_number,
					const char *crl_CompleteOrDelta,
					const char *crl_signature_alg,
					const char *crl_issuerDN,
					const char *crl_thisUpdate,
					const char *crl_nextUpdate,
					const long crl_noOfRevokedEntries,
					GenBuf_t *crl_genbuf,
					CrlTypeEnum_t crl_type);

LIBBMDSQL_SCOPE long prfCheckIfCrlAlreadyExists(	void *hDB,
						const long crl_version,
						const char *crl_number,
						const char *crl_CompleteOrDelta,
						const char *crl_signature_alg,
						const char *crl_issuerDN,
						const char *crl_thisUpdate,
						const char *crl_nextUpdate,
						const long crl_noOfRevokedEntries,
						CrlTypeEnum_t crl_type,
						char **crl_id);

LIBBMDSQL_SCOPE long prfInsertIntoCrlsTable(	void *hDB,
						const long crl_version,
						const char *crl_number,
						const char *crl_CompleteOrDelta,
						const char *crl_signature_alg,
						const char *crl_issuerDN,
						const char *crl_thisUpdate,
						const char *crl_nextUpdate,
						const long crl_noOfRevokedEntries,
						char *crl_lobjId,
						CrlTypeEnum_t crl_type);

LIBBMDSQL_SCOPE long prfParseCrlVersion(GenBuf_t *crl_genbuf, long *crl_version);
LIBBMDSQL_SCOPE long prfParseCrlNumber(GenBuf_t *crl_genbuf, char **crl_number);
LIBBMDSQL_SCOPE long prfParseCrlIsDeltaCrl(GenBuf_t *crl_genbuf, char **crl_CompleteOrDelta);
LIBBMDSQL_SCOPE long prfParseCrlSignatureAlg(GenBuf_t *crl_genbuf, char **crl_signature_alg);
LIBBMDSQL_SCOPE long prfParseCrlIssuerDN(GenBuf_t *crl_genbuf, char **crl_issuerDN);
LIBBMDSQL_SCOPE long prfParseCrlThisUpdate(GenBuf_t *crl_genbuf, char **crl_thisUpdate);
LIBBMDSQL_SCOPE long prfParseCrlNextUpdate(GenBuf_t *crl_genbuf, char **crl_nextUpdate);
LIBBMDSQL_SCOPE long prfParseCrlNoOfRevokedEntries(GenBuf_t *crl_genbuf, long *crl_noOfRevokedEntries);

/******************
 * Common section *
 *****************/
LIBBMDSQL_SCOPE long prfGetAllProofs(	void *hDB,
					char *cert_lobjId,
					char *location_id,
					bmdDatagram_t *bmdDatagram);

LIBBMDSQL_SCOPE long prfGetAllTimestamps(	void *hDB,
						char *crypto_object_id,
						char *location_id,
						bmdDatagram_t *bmdDatagram);

LIBBMDSQL_SCOPE long prfGetAllBlobsAndInsertIntoMetadata(	void *hDB,
								char *SQLQuerySelectingBlobs,
								EnumMetaData_t KindOfMetadata,
								char *MetadataOid,
								bmdDatagram_t *bmdDatagram,
								char *crypto_object_id,
								char *location_id);


LIBBMDSQL_SCOPE long prfGetAllSignatures(	void *hDB,
						char *crypto_object_id,
						char *location_id,
						bmdDatagram_t *bmdDatagram);

LIBBMDSQL_SCOPE long prfGetAllDVCS(	void *hDB,
					char *crypto_object_id,
					char *location_id,
					bmdDatagram_t *bmdDatagram);


#endif
