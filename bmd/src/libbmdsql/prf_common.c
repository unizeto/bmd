#define _WINSOCK2API_
#include<bmd/libbmdsql/proofs.h>
#include<bmd/common/bmd-oids.h>
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdsql/common.h>
#include<bmd/libbmdasn1_common/CommonUtils.h>
#include<bmd/libbmderr/libbmderr.h>
#include<bmd/libbmdglobals/libbmdglobals.h>
#ifdef WIN32
#pragma warning(disable:4127)
#endif

/**
*Pobieranie wszystkich wartosci dowodowych dotyczacych danego pliku
*/
long prfGetAllProofs(	void *hDB,
			char *crypto_object_id,
			char *location_id,
			bmdDatagram_t *bmdDatagram)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(crypto_object_id < 0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdDatagram == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/********************************/
	/* pobranie wartości dowodowych */
	/********************************/
	BMD_FOK(prfGetAllTimestamps(hDB, crypto_object_id, location_id, bmdDatagram));
	BMD_FOK(prfGetAllSignatures(hDB, crypto_object_id, location_id, bmdDatagram));
	BMD_FOK(prfGetAllDVCS(hDB, crypto_object_id, location_id, bmdDatagram));

	return BMD_OK;
}

long prfGetAllTimestamps(	void *hDB,
				char *crypto_object_id,
				char *location_id,
				bmdDatagram_t *bmdDatagram)
{
char *SQLQuery				= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(crypto_object_id == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdDatagram == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*************************************************************************************/
	/* pobierz wszystkie rastry dla znacznikow skojarzonych z danym crypto obiektem (PL) */
	/*************************************************************************************/
	asprintf(&SQLQuery, "SELECT id, timestamp, fk_objects FROM prf_timestamps WHERE fk_objects = %s AND timestamp_type = \'%s\'%s%s ORDER BY gentime ASC;",
				crypto_object_id,
				OID_PKI_METADATA_TIMESTAMP,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);

	if(SQLQuery == NULL)	{	BMD_FOK(NO_MEMORY)	};

	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB, SQLQuery, PKI_METADATA, OID_PKI_METADATA_TIMESTAMP, bmdDatagram, NULL, location_id));
	free0(SQLQuery);

	/***************************************************************************************/
	/* pobierz wszystkie rastry dla znacznikow skojarzonych z podpisem danego obiektu (TW) */
	/***************************************************************************************/
	asprintf(&SQLQuery, "SELECT id, timestamp, fk_objects FROM prf_timestamps WHERE fk_crypto_objects = %s AND timestamp_type = \'%s\'%s%s ORDER BY gentime ASC;",
				crypto_object_id,
				OID_PKI_METADATA_TIMESTAMP_FROM_SIG,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);
	if(SQLQuery == NULL)    {       BMD_FOK(NO_MEMORY);     }

	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB, SQLQuery, PKI_METADATA, OID_PKI_METADATA_TIMESTAMP_FROM_SIG, bmdDatagram, NULL, location_id));
	free0(SQLQuery);

	/***************************************************************************************/
	/* pobierz wszystkie rastry dla znacznikow skojarzonych z poswiadczeniem podpisow (TW) */
	/***************************************************************************************/
	asprintf(&SQLQuery, "SELECT id, timestamp, fk_objects FROM prf_timestamps WHERE fk_crypto_objects = %s AND timestamp_type = \'%s\'%s%s ORDER BY gentime ASC;",
				crypto_object_id,
				OID_PKI_METADATA_TIMESTAMP_FROM_DVCS,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id);
	if(SQLQuery == NULL)    {       BMD_FOK(NO_MEMORY);     }

	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB, SQLQuery, PKI_METADATA, OID_PKI_METADATA_TIMESTAMP_FROM_DVCS, bmdDatagram, NULL, location_id));
	free0(SQLQuery);

	return BMD_OK;
}


long prfGetAllDVCS(	void *hDB,
			char *crypto_object_id,
			char *location_id,
			bmdDatagram_t *bmdDatagram)
{
char *SQLQuery					= NULL;
char oid_pki_metadata_dvcs[] = {OID_PKI_METADATA_DVCS};

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(crypto_object_id == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdDatagram == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	/********************************************************************************************************/
	/*	pobierz wszystkie rastry dla poswiadczen podpisow skojarzonych z danym crypto obiektem (PL)	*/
	/********************************************************************************************************/
	asprintf(&SQLQuery, "SELECT id, dvcs, fk_prf_signatures FROM prf_dvcs WHERE fk_prf_signatures IN (SELECT id FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s) AND dvcs_type = '%s' AND location_id=%s;", crypto_object_id, location_id, OID_PKI_METADATA_DVCS, location_id);
	if(SQLQuery == NULL)    {       BMD_FOK(NO_MEMORY);     }

	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB,
							SQLQuery,
							PKI_METADATA,
							oid_pki_metadata_dvcs,
							bmdDatagram,
							crypto_object_id,
							location_id));

	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}


long prfGetAllSignatures(	void *hDB,
				char *crypto_object_id,
				char *location_id,
				bmdDatagram_t *bmdDatagram)
{
char *SQLQuery						= NULL;
long retVal						= 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(crypto_object_id ==NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }


	// pobieranie podpisow CAdES
	retVal = asprintf(&SQLQuery, "SELECT id, signature, fk_crypto_objects FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s AND standard='%i' ;", crypto_object_id, location_id, STANDARD_CMS);
	if(retVal == -1)
		{ BMD_FOK(NO_MEMORY); }
	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB,
							SQLQuery,
							PKI_METADATA,
							OID_PKI_METADATA_SIGNATURE,
							bmdDatagram,
							crypto_object_id,
							location_id));
	free(SQLQuery); SQLQuery=NULL;

	// pobieranie podpisow XAdES
	retVal = asprintf(&SQLQuery, "SELECT id, signature, fk_crypto_objects FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s AND standard='%i' ;", crypto_object_id, location_id, STANDARD_XADES);
	if(retVal == -1)
		{ BMD_FOK(NO_MEMORY); }
	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB,
							SQLQuery,
							PKI_METADATA,
							OID_PKI_METADATA_SIGNATURE_XADES,
							bmdDatagram,
							crypto_object_id,
							location_id));
	free(SQLQuery); SQLQuery=NULL;

	// pobieranie podpisow PAdES
	retVal = asprintf(&SQLQuery, "SELECT id, signature, fk_crypto_objects FROM prf_signatures WHERE fk_crypto_objects=%s AND location_id=%s AND standard='%i' ;", crypto_object_id, location_id, STANDARD_PADES);
	if(retVal == -1)
		{ BMD_FOK(NO_MEMORY); }
	BMD_FOK(prfGetAllBlobsAndInsertIntoMetadata(	hDB,
							SQLQuery,
							PKI_METADATA,
							OID_PKI_METADATA_SIGNATURE_PADES,
							bmdDatagram,
							crypto_object_id,
							location_id));
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}


long prfGetAllBlobsAndInsertIntoMetadata(	void *hDB,
						char *SQLQuerySelectingBlobs,
						EnumMetaData_t KindOfMetadata,
						char *MetadataOid,
						bmdDatagram_t *bmdDatagram,
						char *crypto_object_id,
						char *location_id)
{
long status				= 0;
long fetched_counter			= 0;
long i					= 0;
long myId				= 0;
long ownerId				= 0;
long ownerType				= 0;
void *database_result			= NULL;
GenBuf_t *blobGenBuf			= NULL;
char *SQLQuery				= NULL;
char *ans				= NULL;
GenBuf_t *symkey			= NULL;
bmd_crypt_ctx_t *plain_symkey		= NULL;
GenBuf_t *raster_decr			= NULL;
char *lo_oid				= NULL;
char *symkey_oid			= NULL;
db_row_strings_t *db_row=NULL;

	/********************************/
	/*	walidacja paramtrow	*/
	/********************************/
	if(hDB == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SQLQuerySelectingBlobs == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(KindOfMetadata < 0)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(MetadataOid == NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(bmdDatagram == NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}

	/************************************************************************************/
	/*	wywolanie zadania na bd wyszukujacego odpowiednie dane dot. metadanch pki	*/
	/************************************************************************************/
	status=bmd_db_execute_sql(hDB, SQLQuerySelectingBlobs,NULL,NULL, &database_result);
	if(status<0 )	{	BMD_FOK(status);	}
	fetched_counter = 0;

	/****************************************/
	/* przegladanie zwraconych wierszy	*/
	/****************************************/
	i=0;
	while(1)
	{
		status=bmd_db_result_get_row(hDB,database_result,i,0,&db_row,FETCH_NEXT,&fetched_counter);
		if( status == BMD_ERR_NODATA )
		{
			break;
		}
		if( db_row->colvals[0] !=  NULL )
		{
			myId=strtol(db_row->colvals[0], (char **)NULL, 10);
		}
		else
		{
			myId=0;
		}
		if( db_row->colvals[1] != NULL )
		{
			asprintf(&lo_oid,"%s",db_row->colvals[1]);
		}
		else
		{
			lo_oid=NULL;
		}
		if( db_row->colvals[2] != NULL )
		{
			ownerId=strtol(db_row->colvals[2], (char **)NULL, 10);
		}
		else
		{
			ownerId=0;
		}

		/******************************************************************/
		/*	jesli pobieramy poswiadczenie DVCS, pobieramy jego status	*/
		/******************************************************************/
		if (strcmp(MetadataOid, OID_PKI_METADATA_DVCS)==0)
		{
			asprintf(&SQLQuery,"SELECT verify_status FROM prf_dvcs WHERE id=%li AND location_id=%s;", myId, location_id);
			status = ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &ans);
			free(SQLQuery); SQLQuery = NULL;
			if( status < BMD_OK && status!=BMD_ERR_NODATA && status!=BMD_ERR_NODATA)
			{
				BMD_FOK(status);
			}

			if (ans!=NULL)
			{
				ownerType=strtol(ans, (char **)NULL, 10);
				free(ans); ans=NULL;
			}
			else
			{
				ownerType=0;
			}
			i++;
		}

		/************************/
		/*	importuj znacznik	*/
		/************************/
		BMD_FOK(bmd_db_import_blob(hDB, lo_oid, &blobGenBuf));
		if (crypto_object_id!=NULL)
		{
			/************************************************/
			/*	importuj klucz symetryczny z bazy danych	*/
			/************************************************/
			asprintf(&SQLQuery,"SELECT symkey FROM crypto_objects WHERE id=%s AND location_id=%s;",crypto_object_id, location_id);
			if(SQLQuery == NULL)    {       BMD_FOK(NO_MEMORY);     }
			status=ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &symkey_oid);
			free(SQLQuery); SQLQuery = NULL;
			if (status<BMD_OK)
			{
				if(status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
				{
					PRINT_TEST("LIBBMDSQLWRN No symkey found for given file's id\n");
					return status;
				}
				else
				{
					BMD_FOK(status);
				}
			}
			if (symkey_oid==NULL)
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			/***********************************************/
			/* pobranie klucza symetrycznego z bazy danych */
			/***********************************************/
			BMD_FOK(bmd_db_import_blob(hDB, symkey_oid, &symkey));

			/*************************************/
			/* zdeszyfrowanie pobranego podpisu  */
			/*************************************/
			BMD_FOK(bmd_decode_bmdKeyIV(symkey, NULL, _GLOBAL_bmd_configuration->crypto_conf->server_ctx, &plain_symkey));
			plain_symkey->sym->sym_params=BMD_SYM_SINGLE_ROUND;

			/*************************************/
			/* deszyfrowanie pliku z bazy danych */
			/*************************************/
			BMD_FOK(bmdpki_symmetric_decrypt(plain_symkey, blobGenBuf, (int)1, &raster_decr));

			/****************************/
			/*  i wrzuc go do datagramu */
			/****************************/

			BMD_FOK(PR2_bmdDatagram_add_metadata(	MetadataOid,
										raster_decr->buf,
										raster_decr->size,
										KindOfMetadata,
										bmdDatagram,
										myId,
										ownerType,
										ownerId,
										1));
			free_gen_buf(&blobGenBuf);
			free_gen_buf(&symkey);
			bmd_ctx_destroy(&plain_symkey);
			raster_decr->buf = NULL; /*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
			raster_decr->size = 0;
			free_gen_buf(&raster_decr);

		}
		else
		{

			/********************************/
			/* 	i wrzuc go do datagramu	*/
			/********************************/

			BMD_FOK(PR2_bmdDatagram_add_metadata(	MetadataOid,
										blobGenBuf->buf,
										blobGenBuf->size,
										KindOfMetadata,
										bmdDatagram,
										myId,
										0,
										ownerId,
										1));
			blobGenBuf->buf = NULL; /*TW 2009-01-21 By nie zwalniac wnetrza bufora!*/
			blobGenBuf->size = 0;
			free_gen_buf(&blobGenBuf);

		}
		/*free(ans); ans=NULL;
		free(lo_oid); lo_oid=NULL;*/
		bmd_db_row_struct_free(&db_row);
		free(symkey_oid); symkey_oid=NULL;

	}

	/************/
	/* porzadki */
	/************/
	free(ans); ans=NULL;
	free_gen_buf(&symkey);
	bmd_ctx_destroy(&plain_symkey);
	free_gen_buf(&raster_decr);
	free_gen_buf(&blobGenBuf);
	free(SQLQuery); SQLQuery=NULL;
	free_gen_buf(&symkey);
	free_gen_buf(&raster_decr);
	free(lo_oid); lo_oid=NULL;
	free(symkey_oid); symkey_oid=NULL;
	bmd_db_result_free(&database_result);

	return BMD_OK;
}

