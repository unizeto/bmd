#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

long oracle_bmd_db_connect(const char *conn_info,void **hDB)
{
	char **str_N=NULL;
	long N,status;
	OCI_bmd_db_conn_t *db_conn=NULL;
	long err=0;

	status=bmd_strsep((char *)conn_info,'|',&str_N,&N);
	if( status != BMD_OK )
		return BMD_ERR_OP_FAILED;

	db_conn=(OCI_bmd_db_conn_t *)malloc(sizeof(OCI_bmd_db_conn_t));
	if( db_conn == NULL )
		return BMD_ERR_MEMORY;

	memset(db_conn,0,sizeof(OCI_bmd_db_conn_t));
	db_conn->db_type=BMD_DB_ORACLE;
	db_conn->magic=BMD_DBCONN_MAGIC;
	asprintf(&(db_conn->conn_string),"%s",str_N[0]);

	db_conn->bmdHandles=(OCIBMDHandles_t *)malloc(sizeof(OCIBMDHandles_t));
	if( db_conn->bmdHandles == NULL )
		return BMD_ERR_MEMORY;
	memset(db_conn->bmdHandles,0,sizeof(OCIBMDHandles_t));

	/****************************************
	* Alokacja handlerow			*
	****************************************/
	err = OCIEnvCreate((OCIEnv **)&(db_conn->bmdHandles->EnviromentHandle),
					   (ub4)OCI_THREADED,
					   (dvoid *) 0,
					   (dvoid * (*)(dvoid *, size_t)) 0,	
					   (dvoid * (*)(dvoid *, dvoid *, size_t)) 0,
					   (void (*)(dvoid *, dvoid *)) 0,	
					   (size_t) 0,	
					   (dvoid **) 0 );
	if(err) 
	{
		PRINT_ERROR("LIBBMDOCIERR OCIEnvCreate. ErrorCode=%li.\n",err);
		return BMD_ERR_OP_FAILED;
	}
	err = OCIHandleAlloc((dvoid *) db_conn->bmdHandles->EnviromentHandle,
					     (dvoid **) &(db_conn->bmdHandles->ErrorHandle),
						 OCI_HTYPE_ERROR,
						 (size_t) 0,
						 (dvoid **) 0);
	if(err) 
	{
		PRINT_ERROR("LIBBMDOCIERR OCIHandleAlloc. ErrorCode=%li.\n", err);
		freeOCIBMDHandles(db_conn->bmdHandles);
		return BMD_ERR_OP_FAILED;
	}
	
	err=checkerr(db_conn->bmdHandles->ErrorHandle,
			     OCILogon2(db_conn->bmdHandles->EnviromentHandle,
				 		   db_conn->bmdHandles->ErrorHandle,
						   &(db_conn->bmdHandles->ServiceContextHandle),
						   (const OraText *)str_N[1],
						   (ub4)strlen(str_N[1]),
		                   (const OraText *)str_N[2],
		                   (ub4)strlen(str_N[2]),
						   (const OraText *)str_N[0],
						   (ub4)strlen(str_N[0]),
						   OCI_DEFAULT));
	if( err<0)
	{
		PRINT_ERROR("LIBBMDOCIERR OCILogon2 failed. Error Code %li\n",err);
		freeOCIBMDHandles(db_conn->bmdHandles);
		return BMD_ERR_OP_FAILED;
	}
	free0(str_N[1]);
	free0(str_N[2]);
	free0(str_N);
	
	(*hDB)=db_conn;

	return BMD_OK;
}

long oracle_bmd_db_connect2(const char *hostaddr,char *port, const char *dbname,
		const char *user, const char *password, void **hDB)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	char *server_info 		= NULL;
	long err				= 0;

	text *username_oracle = (text *) user;
	text *password_oracle = (text *) password;

	if(hostaddr == NULL)
		return BMD_ERR_PARAM1;
	if(port == NULL)
		return BMD_ERR_PARAM2;
	if(dbname == NULL)
		return BMD_ERR_PARAM3;
	if(user == NULL)
		return BMD_ERR_PARAM4;
	if( hDB == NULL)
		return BMD_ERR_PARAM6;
	if( (*hDB) != NULL )
		return BMD_ERR_PARAM6;

	asprintf(&server_info, "(DESCRIPTION = "
						   " (ADDRESS_LIST = "
						   "		(ADDRESS = "
						   "		(PROTOCOL = TCP)"
						   "		(HOST = %s)"
						   "		(PORT = %s))) "
						   " (CONNECT_DATA = "
						   "		(SID = %s)"
						   "		(SERVER = DEDICATED)))",
						 hostaddr, port, dbname);
	if(server_info == NULL)
		return BMD_ERR_MEMORY;

	db_conn=(OCI_bmd_db_conn_t *)malloc(sizeof(OCI_bmd_db_conn_t));
	if( db_conn == NULL )
		return BMD_ERR_MEMORY;
	memset(db_conn,0,sizeof(OCI_bmd_db_conn_t));
	db_conn->db_type=BMD_DB_ORACLE;
	db_conn->magic=BMD_DBCONN_MAGIC;
	asprintf(&(db_conn->conn_string),"%s",server_info);
	
	db_conn->bmdHandles=(OCIBMDHandles_t *)malloc(sizeof(OCIBMDHandles_t));
	if( db_conn->bmdHandles == NULL )
		return BMD_ERR_MEMORY;
	memset(db_conn->bmdHandles,0,sizeof(OCIBMDHandles_t));

	/****************************************
	* Alokacja handlerow			*
	****************************************/
	err = OCIEnvCreate((OCIEnv **)&(db_conn->bmdHandles->EnviromentHandle),
					   (ub4)OCI_THREADED,
					   (dvoid *) 0,
					   (dvoid * (*)(dvoid *, size_t)) 0,
					   (dvoid * (*)(dvoid *, dvoid *, size_t)) 0,
					   (void (*)(dvoid *, dvoid *)) 0,
					   (size_t) 0,
					   (dvoid **)0);
	if(err) 
	{
		PRINT_ERROR("LIBBMDOCIERR OCIEnvCreate. ErrorCode=%li.\n",err);
		return BMD_ERR_OP_FAILED;
	}
	err = OCIHandleAlloc((dvoid *) db_conn->bmdHandles->EnviromentHandle,
						 (dvoid **) &(db_conn->bmdHandles->ErrorHandle),
						 OCI_HTYPE_ERROR,
						 (size_t) 0,
						 (dvoid **) 0);
	if(err) 
	{
		PRINT_ERROR("LIBBMDOCIERR OCIHandleAlloc. ErrorCode=%li.\n", err);
		freeOCIBMDHandles(db_conn->bmdHandles);
		return BMD_ERR_OP_FAILED;
	}
	
	err=checkerr(db_conn->bmdHandles->ErrorHandle,
			     OCILogon2(db_conn->bmdHandles->EnviromentHandle,
				 		   db_conn->bmdHandles->ErrorHandle,
						   &(db_conn->bmdHandles->ServiceContextHandle),
						   (const OraText *)username_oracle,
						   (ub4)strlen(username_oracle),
		                   (const OraText *)password_oracle,
		                   (ub4)strlen(password_oracle),
						   (const OraText *)server_info,
						   (ub4)strlen(server_info),
						   OCI_DEFAULT));

	(*hDB)=db_conn;
	return err;
}

/***
 * Funkcja pobiera handler polaczenia z baza i zamyka polaczenie.
 */
long oracle_bmd_db_disconnect(void **connection_handle)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	long err = 0;

	if(*connection_handle == NULL || connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;	
	if((*connection_handle) == NULL )
		return BMD_ERR_PARAM1;
	if( (*(long *)(connection_handle)) == BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*(long *)(*connection_handle)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)(*connection_handle);

	err=checkerr(db_conn->bmdHandles->ErrorHandle,
				 OCILogoff(db_conn->bmdHandles->ServiceContextHandle,
				 db_conn->bmdHandles->ErrorHandle));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR OCILogoff. RecievedErrorCode=%li.\n", err);
		freeOCIBMDHandles(db_conn->bmdHandles);
		return BMD_ERR_OP_FAILED;
	}
	
	freeOCIBMDHandles(db_conn->bmdHandles);
	free0(db_conn->conn_string);
	free0(*connection_handle);
	
	return err;
}
