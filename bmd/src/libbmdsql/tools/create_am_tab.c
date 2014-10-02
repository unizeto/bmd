#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdsql/common.h>
#include "create_am_tab.h"
#include "cam_conf_sections.h"


#define CONF_FILE				"create_am_tab.conf"
#define ERR_CONFIG_INIT_FAILED			-100
#define ERR_CONFIG_SECTION2DICT_FAILED		-101
#define ERR_CONFIG_READ_DBCONNINFO		-102
#define ERR_OCI_NO_DATA				-606

extern long _GLOBAL_debug_level;

server_configuration_t *_GLOBAL_bmd_configuration = NULL;

int CreateAdditionalMetadataTable(void *connection_handle, char *db_type);

int main(int argc, char *argv[])
{
int err = 0;
bmd_conf *configuration		= NULL;
char *db_type			= NULL;
char *db_host			= NULL;
char *db_port			= NULL;
char *db_name			= NULL;
char *db_user			= NULL;
char *db_password		= NULL;
void *connection_handle		= NULL;
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
int status=0;
int i=0;
char *bmd_conf_file=NULL;
	_GLOBAL_debug_level = 2;


/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	/*status=parse_command_line_params(argc,argv,&bmd_conf_file);
	if(status!=0)
	{
		PRINT_DEBUG("BMDSERVERERR Error in parsing command line parameters. Error=%i\n",status);
		return -1;
	}*/
	asprintf(&bmd_conf_file,"%s",argv[1]);

	PRINT_VDEBUG("BMDSERVERINF Creating Additional metadata table...\n");
	for (i=0; i<argc; i++)
	{
		PRINT_VDEBUG("BMDSERVERINF %i. %s\n",i,argv[i]);
	}

	if(bmd_conf_file==NULL)
	{
		PRINT_DEBUG("BMDSERVERERR Configuration file not found or not specified\n");
		exit(-1);
	}

	status=bmd_server_configuration_create(&_GLOBAL_bmd_configuration);
	if (status!=BMD_OK)
	{
		PRINT_DEBUG("BMDSERVERERR Error in creating configuration. Error=%i\n",status);
		return status;
	}

	status=bmd_get_server_configuration(bmd_conf_file,_GLOBAL_bmd_configuration, TIME_FIRST);
	if (status!=BMD_OK)
	{
		PRINT_DEBUG("BMDSERVERERR Error in getting configuration. Error=%i\n",status);
		return status;
	}
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

	/*PRINT_VDEBUG("LIBBMDSQLINF Getting configuration from %s...\n", CONF_FILE);
	err = bmdconf_init(&configuration);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in getting configuration err=%i\n", err);
		return ERR_CONFIG_INIT_FAILED;
	}
	err = bmdconf_add_section2dict(configuration, "cam", _GLOBAL_cam_DICT_BMDDB, 1);
	if(err<0){
		PRINT_DEBUG("LIBBMDSQLERR Error in adding configuration section err=%i\n", err);
		return ERR_CONFIG_SECTION2DICT_FAILED;
	}
	err = bmdconf_load_file(CONF_FILE, configuration, NULL);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in loading configuration file err=%i\n", err);
		return ERR_CONFIG_SECTION2DICT_FAILED;
	}
	err=bmdconf_get_value(configuration, "cam", "db_type", &db_type);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in reading db_type err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}
	err=bmdconf_get_value(configuration, "cam", "db_host", &db_host);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Erorr in reading db_host err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}
	err=bmdconf_get_value(configuration, "cam", "db_port", &db_port);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in reading db_port err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}
	err=bmdconf_get_value(configuration, "cam", "db_name", &db_name);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in reading db_name err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}
	err=bmdconf_get_value(configuration, "cam", "db_user", &db_user);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in reading db_user err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}
	err=bmdconf_get_value(configuration, "cam", "db_password", &db_password);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in reading db_password err=%i\n", err);
		return ERR_CONFIG_READ_DBCONNINFO;
	}

	err=bmdconf_destroy(&configuration);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in destroying configuration dic err=%i\n", err);
		return err;
	}
	PRINT_VDEBUG("LIBBMDSQLINF ... done\n", __FILE__, __LINE__, __FUNCTION__);*/


	printf("\n\n\n\n\n%s\n\n\n\n",(char *)_GLOBAL_bmd_configuration->db_conninfo);

	/*status=bmd_db_connect(bmdconf_get_value_static(_GLOBAL_bmd_configuration->conf,"database","db_conninfo"),&db_conn_info);
	if(db_conn_info==NULL)
	{
		PRINT_DEBUG("BMDSERVERERR Database configuration error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}*/

	return;

	err = bmd_db_connect2(db_host, db_port, db_name, db_user, db_password, &connection_handle);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in beginning database transaction err=%i\n", err);
		return err;
	}
	err = bmd_db_start_transaction(connection_handle);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in beginning database transaction err=%i\n", err);
		return err;
	}
	err = CreateAdditionalMetadataTable(connection_handle, db_type);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in creating additional metadata table err=%i\n", err);
		return err;
	}
	err = bmd_db_end_transaction(connection_handle);
	if(err<0) {
		PRINT_DEBUG("ERR Error in begining database transaction. "
			    "Recieved error code = %i\n",
			    err);
		return err;
	}
	bmd_db_disconnect(&connection_handle);

	if(db_type)	{free(db_type); db_type=NULL;}
	if(db_host)	{free(db_host); db_host=NULL;}
	if(db_port)	{free(db_port); db_port=NULL;}
	if(db_name)	{free(db_name); db_name=NULL;}
	if(db_user)	{free(db_user); db_user=NULL;}
	if(db_password)	{free(db_password); db_password=NULL;}
	return err;
}

int CreateAdditionalMetadataTable(void *connection_handle, char *db_type)
{
int err = 0;
char *SQLQuery = "SELECT type from add_metadata_types;";
char *SQLQuery2 = NULL;
char tmp2[100];
int rows=0, cols=0;
void *query_result=NULL;
void *query_result2=NULL;
void *query_result3=NULL;
db_row_strings_t *rowstruct = NULL;
db_row_strings_t *rowstruct2 = NULL;
unsigned long int fetched_counter=0;
unsigned long int fetched_counter2=0;
/*
CREATE TABLE add_metadata (
	id				integer		PRIMARY KEY,
	fk_crypto_objects		integer		REFERENCES crypto_objects(id) ON DELETE CASCADE NOT NULL,
	am_value			varchar(256),
	fk_am_types 			integer		REFERENCES add_metadata_types(id) NOT NULL,
	update_date			TIMESTAMP with time zone,
	update_reason			varchar(256),
	fk_updater_dn			integer,
	visible				integer
);
*/
char *CreateSQLPrefix1 = "\nCREATE TABLE add_metadata (\n"
	"id					";
char *CreateSQLPrefix2 = ",\n"
	"fk_crypto_objects			";
char *SQLChunk		= NULL;
char *tmp		= NULL;
char *SQLFullQuery	= NULL;

char *SQLSequenceQuery 	= NULL;
char *SQLCryptoObjectsIndexQuery = "CREATE INDEX am_fk_co_idx ON add_metadata (fk_crypto_objects);";
char **SQLAmValueIndexQuery =	NULL;
char *EscapedOid	=	NULL;

char *colname_am_value =	BMD_ADD_METADATA_COLNAME_AM_VALUE;
char *colname_fk_am_types =	BMD_ADD_METADATA_COLNAME_FK_AM_TYPES;
char *colname_update_date =	BMD_ADD_METADATA_COLNAME_UPDATE_DATE;
char *colname_update_reason =	BMD_ADD_METADATA_COLNAME_UPDATE_REASON;
char *colname_fk_updater_dn =	BMD_ADD_METADATA_COLNAME_FK_UPDATER_DN;
char *colname_visible =		BMD_ADD_METADATA_COLNAME_VISIBLE;

char *integer_postgres = "integer";
char *integer_oracle = "number(9)";
char *integer = NULL;

char *on_delcascade_postgres = "ON DELETE CASCADE NOT NULL,\n";
char *on_delcascade_oracle = "NOT NULL,\n";
char *on_delcascade = NULL;

char *pk_postgres = "PRIMARY KEY";
char *pk_oracle = "";
char *pk = NULL;

char *fk_co_postgres = "REFERENCES crypto_objects(id)";
char *fk_co_oracle = "";
char *fk_co = NULL;

char *fk_am_types_postgres = "REFERENCES add_metadata_types(id)";
char *fk_am_types_oracle = "";
char *fk_am_types = NULL;

char *seq_sufix_postgres = "INCREMENT 1 MINVALUE 1";
char *seq_sufix_oracle = "NOMAXVALUE NOMINVALUE NOCYCLE";
char *seq_sufix = NULL;

int no_of_indexes = 0;
int i = 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;

	if(!strcmp(db_type, "postgres"))
	{
		integer = integer_postgres;
		on_delcascade = on_delcascade_postgres;
		pk = pk_postgres;
		fk_co = fk_co_postgres;
		fk_am_types = fk_am_types_postgres;
		seq_sufix = seq_sufix_postgres;
	}
	else if(!strcmp(db_type, "oracle"))
	{
		integer = integer_oracle;
		on_delcascade = on_delcascade_oracle;
		pk = pk_oracle;
		fk_co = fk_co_oracle;
		fk_am_types = fk_am_types_oracle;
		seq_sufix = seq_sufix_oracle;
	}
	else
	{
		integer = integer_postgres;
		on_delcascade = on_delcascade_postgres;
		pk = pk_postgres;
		fk_co = fk_co_postgres;
		fk_am_types = fk_am_types_postgres;
		seq_sufix = seq_sufix_postgres;
	}


	err = bmd_db_execute_sql(	connection_handle, SQLQuery,
				WITHOUT_CURSOR, WITHOUT_ROWS, WITHOUT_COLS,
				&rows, &cols, &query_result);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDSQLERR SQL query execute error err=%i\n", err);
		if(SQLQuery != NULL) { free(SQLQuery); SQLQuery = NULL; }
		return err;
	}

	fetched_counter = 0;
	while(err>=0)
	{
		/* zerowa kolumna zawiera OID - przekonwertuj string na tablice unsigned long */
		err = bmd_db_result_get_row(connection_handle,query_result,0, 0, &rowstruct, FETCH_NEXT, WITHOUT_COLNAMES, &fetched_counter);
		if(err==ERR_OCI_NO_DATA || err==ERR_PG_NO_DATA) {err=0; bmd_db_row_struct_free(&rowstruct); break;}
		if(err < 0)
		{
			PRINT_DEBUG("LIBBMDSQLERR Getting row from query result error err=%i\n", err);
			return err;
		}
		/* for(k=0; k<rowstruct->size; k++)
			printf("%li:%s = %s\n", rowstruct->colnumbers[k], rowstruct->colnames[k], rowstruct->colvals[k]); */
		/* w kolumnie colvals nr 1 jest OIC a w colvals nr 0 jest ID */

		/*********** pobranie typu danych *************/
		asprintf(&SQLQuery2,"SELECT sql_cast_string FROM add_metadata_types WHERE type='%s';",rowstruct->colvals[0]);
		err = bmd_db_execute_sql(connection_handle, SQLQuery2, WITHOUT_CURSOR, WITHOUT_ROWS, WITHOUT_COLS, &rows, &cols, &query_result3);
		if(err < 0)
		{
			PRINT_DEBUG("LIBBMDSQLERR SQL query execute error err=%i\n", err);
			if(SQLQuery2 != NULL) { free(SQLQuery2); SQLQuery2 = NULL; }
			return err;
		}
		fetched_counter2=0;

		err = bmd_db_result_get_row(connection_handle,query_result3,0, 0, &rowstruct2, FETCH_NEXT, WITHOUT_COLNAMES, &fetched_counter2);
		if(err==ERR_OCI_NO_DATA || err==ERR_PG_NO_DATA) {err=0; bmd_db_row_struct_free(&rowstruct2); break;}
		if(err < 0)
		{
			PRINT_DEBUG("LIBBMDSQLERR Getting row from query result error err=%i\n", err);
			return err;
		}
		if (strcmp(rowstruct2->colvals[0],"undefined")==0)
		{
			strcpy(tmp2,"varchar");
		}
		else
		{
			strcpy(tmp2,rowstruct2->colvals[0]);
		}
		if (strcmp(tmp2,"varchar")==0)
		{
			strcat(tmp2,"(256)");
		}

		if (SQLQuery2!=NULL) { free(SQLQuery2); SQLQuery2 = NULL; }
		if (query_result3!=NULL) { free(query_result3); query_result3 = NULL; }
		bmd_db_row_struct_free(&rowstruct2);
		/************************************************/
		PRINT_VDEBUG("LIBBMDSQLINF oid=%s  cast_string=%s\n",rowstruct->colvals[0],tmp2);
		err = CharReplace(rowstruct->colvals[0], &EscapedOid, '.', '_');
		if(err < 0)
		{
			printf("ERR\n", __FILE__, __LINE__, __FUNCTION__);
			return err;
		}
		asprintf(&SQLChunk,
				"%s_%s	%s,\n"
				"%s_%s	%s		%s DEFAULT %li,\n"
				"%s_%s	TIMESTAMP	with time zone,\n"
				"%s_%s	varchar(256),\n"
				"%s_%s	%s,\n"
				"%s_%s	%s DEFAULT 1,\n",
				colname_am_value, EscapedOid, tmp2,
				colname_fk_am_types, EscapedOid, integer, fk_am_types, fetched_counter,
				colname_update_date, EscapedOid,
				colname_update_reason, EscapedOid,
				colname_fk_updater_dn, EscapedOid, integer,
				colname_visible, EscapedOid, integer
		);
		if(SQLChunk == NULL)
			return NO_MEMORY;

		SQLAmValueIndexQuery = (char **) realloc (SQLAmValueIndexQuery, sizeof(char *) * (no_of_indexes+1));
		if(SQLAmValueIndexQuery == NULL) return NO_MEMORY;
		asprintf(&SQLAmValueIndexQuery[no_of_indexes], "CREATE INDEX amvti_%s ON add_metadata(%s_%s);",
					EscapedOid, colname_am_value, EscapedOid);
		if(SQLAmValueIndexQuery == NULL) return NO_MEMORY;
		no_of_indexes++;


		if(fetched_counter == 1){
			asprintf(&SQLFullQuery, "%s %s %s %s %s %s %s %s", CreateSQLPrefix1, integer, pk, CreateSQLPrefix2,
							integer, fk_co, on_delcascade, SQLChunk);
			if(SQLFullQuery == NULL) return NO_MEMORY;
			if(SQLChunk) {free(SQLChunk); SQLChunk=NULL;}
		}
		else {
			asprintf(&tmp, "%s", SQLFullQuery);
			if(tmp == NULL) return NO_MEMORY;
			if(SQLFullQuery) {free(SQLFullQuery); SQLFullQuery=NULL;}

			asprintf(&SQLFullQuery, "%s%s", tmp, SQLChunk);
			if(SQLChunk) {free(SQLChunk); SQLChunk=NULL;}
			if(tmp) {free(tmp); tmp=NULL;}

		}
		bmd_db_row_struct_free(&rowstruct);
		if(EscapedOid) {free(EscapedOid); EscapedOid=NULL;}

	}
	if(fetched_counter == 0)
	{
		printf("ERR No additional metadata types present in add_metadata_types table so creating add_metadata table is not needed! Exiting now!\n", __FILE__, __LINE__, __FUNCTION__);
		return err;
	}
	asprintf(&tmp, "%s", SQLFullQuery);
	if(tmp == NULL) return NO_MEMORY;
	if(SQLFullQuery) {free(SQLFullQuery); SQLFullQuery=NULL;}

	asprintf(&SQLFullQuery, "%s);", tmp);
	if(tmp) {free(tmp); tmp = NULL;}
	bmd_db_result_free(&query_result);

	/* Usuń ostatni przeciniek ze stringu */
	SQLFullQuery[strlen(SQLFullQuery)-4] = ' ';
	/*printf("SZU1: %s\n", SQLFullQuery);
	printf("SZU2: %s\n", SQLAmValueIndexQuery[i]);*/
	/* Teraz można już wykonać wszystkie zapytania */
	err = bmd_db_execute_sql(	connection_handle,
				SQLFullQuery, WITHOUT_CURSOR,
				WITHOUT_ROWS, WITHOUT_COLS,
				&rows, &cols, &query_result);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in executing %s. Error=%i\n", SQLFullQuery, err);
		if(SQLFullQuery != NULL) { free(SQLFullQuery); SQLFullQuery=NULL; }
		return err;
	}
	bmd_db_result_free(&query_result);

	asprintf(&SQLSequenceQuery, "CREATE SEQUENCE add_metadata_id_seq %s;", seq_sufix);
	err = bmd_db_execute_sql(	connection_handle,
				SQLSequenceQuery, WITHOUT_CURSOR,
				WITHOUT_ROWS, WITHOUT_COLS,
				&rows, &cols, &query_result);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in executing %s. Error=%i\n", SQLSequenceQuery, err);
		return err;
	}
	bmd_db_result_free(&query_result);
	if(SQLSequenceQuery) {free(SQLSequenceQuery); SQLSequenceQuery = NULL;}

	err = bmd_db_execute_sql(	connection_handle,
				SQLCryptoObjectsIndexQuery, WITHOUT_CURSOR,
				WITHOUT_ROWS, WITHOUT_COLS,
				&rows, &cols, &query_result);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDSQLERR Error in executing %s. Error=%i\n", SQLCryptoObjectsIndexQuery, err);
		return err;
	}
	bmd_db_result_free(&query_result);

	for(i=0; i<no_of_indexes; i++)
	{
		err = bmd_db_execute_sql(	connection_handle,
					SQLAmValueIndexQuery[i], WITHOUT_CURSOR,
					WITHOUT_ROWS, WITHOUT_COLS,
					&rows, &cols, &query_result2);
		if(err < 0)
		{
			PRINT_DEBUG("LIBBMDSQLERR Error in executing %s. Error=%i\n", SQLAmValueIndexQuery[i], err);
			if(SQLAmValueIndexQuery != NULL) { free(SQLAmValueIndexQuery); SQLAmValueIndexQuery=NULL; }
			return err;
		}
		bmd_db_result_free(&query_result2);
		if(SQLAmValueIndexQuery[i]) {free(SQLAmValueIndexQuery[i]); SQLAmValueIndexQuery[i]=NULL;}
	}
	if(SQLAmValueIndexQuery) {free(SQLAmValueIndexQuery); SQLAmValueIndexQuery=NULL;}
	if(SQLFullQuery){free(SQLFullQuery);SQLFullQuery=NULL;}
	if(SQLAmValueIndexQuery){free(SQLAmValueIndexQuery);SQLAmValueIndexQuery=NULL;}
	return err;
}
