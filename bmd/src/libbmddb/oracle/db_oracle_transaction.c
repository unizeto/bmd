#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

/* TODO dopisac walidacje parametru 2 - jakie wartosci dopuszczamy a jakie odrzucamy	*/
long oracle_bmd_db_start_transaction(void *connection_handle,long transacton_isolation_level)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	long err=0;
	OCITrans *tr=NULL;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	err=checkerr(db_conn->bmdHandles->ErrorHandle,
				 OCIHandleAlloc(db_conn->bmdHandles->EnviromentHandle, 
								(dvoid **)&tr, OCI_HTYPE_TRANS, 0, 0));
	err=checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIAttrSet(db_conn->bmdHandles->ServiceContextHandle,
				   OCI_HTYPE_SVCCTX, (dvoid *)tr, 0,
				   OCI_ATTR_TRANS,db_conn->bmdHandles->ErrorHandle));

	/*err=checkerr(db_conn->bmdHandles->ErrorHandle,
				 OCIAttrSet(tr, OCI_HTYPE_TRANS,(dvoid *)"TRANSACTION",11,
			   OCI_ATTR_TRANS_NAME, db_conn->bmdHandles->ErrorHandle));*/

	err = checkerr( db_conn->bmdHandles->ErrorHandle,
		OCITransStart(db_conn->bmdHandles->ServiceContextHandle,
		db_conn->bmdHandles->ErrorHandle,0,OCI_TRANS_SERIALIZABLE));
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in starting database transaction - OCITransStart. "
			"RecievedErrorCode=%li.\n", err);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_ERROR("LIBBMDOCIINF Succesfully started database transaction.\n");
	return err;
}

/* TODO dopisac walidacje parametru 2 - jakie wartosci dopuszczamy a jakie odrzucamy	*/
long oracle_bmd_db_end_transaction(void *connection_handle)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCIBMDHandles_t *OCIBMDHandles = NULL;//(OCIBMDHandles_t *) connection_handle;
	long err=0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	OCIBMDHandles=db_conn->bmdHandles;

	err = checkerr( OCIBMDHandles->ErrorHandle,
		OCITransCommit ( OCIBMDHandles->ServiceContextHandle,
			OCIBMDHandles->ErrorHandle,
			OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in ending database transaction - OCITransCommit. "
			"RecievedErrorCode=%li.\n", err);
		return BMD_ERR_OP_FAILED;
	}
	PRINT_ERROR("LIBBMDOCIINF Succesfully ended database transaction.\n");
	return err;
}

long oracle_bmd_db_rollback_transaction(void *connection_handle,void *params)
{
	long err;
	OCI_bmd_db_conn_t *db_conn=NULL;

	if( connection_handle == NULL )
		return BMD_ERR_PARAM1;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	if( db_conn->bmdHandles == NULL )
		return BMD_ERR_NODATA;

	err=checkerr(db_conn->bmdHandles->ErrorHandle,
				 OCITransRollback(db_conn->bmdHandles->ServiceContextHandle,
								  db_conn->bmdHandles->ErrorHandle,OCI_DEFAULT));
	
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in rolling back database transaction - OCITransRollback. "
			"RecievedErrorCode=%li.\n", err);
		return BMD_ERR_OP_FAILED;
	}
	PRINT_ERROR("LIBBMDOCIINF Succesfully rollbacked database transaction.\n");

	return err;
}
