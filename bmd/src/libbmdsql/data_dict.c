#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdsql/user.h>



long dumpAllDataDictionaries(	char *db_conn_info)
{
void *hDB		= NULL;
char *SQLQuery		= NULL;
long retVal		= 0;

	BMD_FOK(bmd_db_connect(db_conn_info,&hDB));

	if (_GLOBAL_bmd_configuration->location_id!=NULL)
	{

		asprintf(&SQLQuery, "SELECT users.id, name, identity, cert_serial, cert_dn, cert_dn::varchar||cert_serial::varchar,accepted, identities.description FROM users LEFT JOIN identities ON (users.identity=identities.id AND users.location_id=identities.location_id) WHERE users.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
        	if (SQLQuery==NULL)     {       BMD_FOK(NO_MEMORY);     }
        	BMD_FOK(initSingleDataDictionary( &_GLOBAL_UsersDictionary, SQLQuery));
        	free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, 	"SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups "
					"LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id AND users_and_groups.location_id=groups.location_id) WHERE groups.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)     {       BMD_FOK(NO_MEMORY);     }
		BMD_FOK(initSingleDataDictionary(       &_GLOBAL_UserGroupsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, 	"SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes "
					"LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id AND users_and_classes.location_id=classes.location_id) WHERE classes.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)     {       BMD_FOK(NO_MEMORY);     }
		BMD_FOK(initSingleDataDictionary(       &_GLOBAL_UserClassesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT  id, name FROM groups WHERE groups.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)     {       BMD_FOK(NO_MEMORY);     }
		BMD_FOK(initSingleDataDictionary(       &_GLOBAL_GroupsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT  id, name FROM classes WHERE classes.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)     {       BMD_FOK(NO_MEMORY);     }
		BMD_FOK(initSingleDataDictionary(       &_GLOBAL_ClassesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT src, dest FROM groups_graph WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_GroupsGraphDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT id, name FROM roles WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RolesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		retVal = asprintf(&SQLQuery, "SELECT id, name, registration, notification FROM actions WHERE location_id=%s ", _GLOBAL_bmd_configuration->location_id);
		if(retVal == -1)
			{ BMD_FOK(NO_MEMORY); }
		BMD_FOK(initSingleDataDictionary(&_GLOBAL_ActionsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery = NULL;

		asprintf(&SQLQuery, 	"SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles "
					"LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id AND users_and_roles.location_id=roles.location_id) WHERE roles.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_UserRolesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, 	"SELECT roles.id, roles.name, actions.id, actions.name, actions.registration, actions.notification FROM roles "
					"LEFT JOIN roles_and_actions ON (roles_and_actions.fk_roles_id=roles.id AND roles_and_actions.location_id=roles.location_id) "
					"LEFT JOIN actions ON (roles_and_actions.fk_actions_id=actions.id AND roles_and_actions.location_id=actions.location_id) WHERE actions.name IS NOT NULL AND roles.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RolesAndActionsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, 	"SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories "
					"LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) "
					"LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id AND users_and_security.location_id=sec_levels.location_id) WHERE (sec_levels.location_id=%s OR sec_categories.location_id=%s);",_GLOBAL_bmd_configuration->location_id,_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_AddMetadataTypesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
		{
			BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RoleRightsStructureDictionary, "SELECT a.attname as \"Column\", pg_catalog.format_type(a.atttypid, a.atttypmod) as \"Datatype\" FROM pg_catalog.pg_attribute a WHERE a.attnum > 0 AND NOT a.attisdropped AND a.attrelid = (SELECT c.oid FROM pg_catalog.pg_class c LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace WHERE c.relname ~ '^(role_rights)$' AND pg_catalog.pg_table_is_visible(c.oid));"));
		}

		asprintf(&SQLQuery, 	"SELECT roles.name,* FROM role_rights "
					"LEFT JOIN roles ON (roles.id=role_rights.fk_roles AND roles.location_id=role_rights.location_id) WHERE roles.location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RoleRightsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT id, type, name FROM sec_categories WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityCategoriesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT id, type, name, priority FROM sec_levels WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityLevelsDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT src ,dest FROM categories_graph WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityCategoriesGraphDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;

		asprintf(&SQLQuery, "SELECT id, location_id, type, value FROM metadata_values WHERE location_id=%s;",_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_MetadataValuesDictionary, SQLQuery));
		free(SQLQuery); SQLQuery=NULL;
	}
	else
	{
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_UsersDictionary, "SELECT users.id, name, identity, cert_serial, cert_dn, cert_dn::varchar||cert_serial::varchar,accepted, identities.description FROM users LEFT JOIN identities ON (users.identity=identities.id AND users.location_id=identities.location_id);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_UserGroupsDictionary, "SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_UserClassesDictionary, "SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_GroupsDictionary, "SELECT id, name FROM groups;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_ClassesDictionary, "SELECT id, name FROM classes;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_GroupsGraphDictionary, "SELECT src, dest FROM groups_graph;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RolesDictionary, "SELECT id, name FROM roles;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_ActionsDictionary, "SELECT id, name, registration, notification FROM actions"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_UserRolesDictionary, "SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RolesAndActionsDictionary, "SELECT roles.id, roles.name, actions.id, actions.name, actions.registration, actions.notification FROM roles_and_actions LEFT JOIN roles ON (roles_and_actions.fk_roles_id=roles.id) LEFT JOIN actions ON (roles_and_actions.fk_actions_id=actions.id) WHERE actions.name IS NOT NULL;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityDictionary, "SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_AddMetadataTypesDictionary, "SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;"));

		if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
		{
			BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RoleRightsStructureDictionary, "SELECT a.attname as \"Column\", pg_catalog.format_type(a.atttypid, a.atttypmod) as \"Datatype\" FROM pg_catalog.pg_attribute a WHERE a.attnum > 0 AND NOT a.attisdropped AND a.attrelid = (SELECT c.oid FROM pg_catalog.pg_class c LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace WHERE c.relname ~ '^(role_rights)$' AND pg_catalog.pg_table_is_visible(c.oid));"));
		}
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_RoleRightsDictionary, "SELECT roles.name,* FROM role_rights LEFT JOIN roles ON (roles.id=role_rights.fk_roles);"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityCategoriesDictionary, "SELECT id, type, name FROM sec_categories;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityCategoriesGraphDictionary, "SELECT  src ,dest FROM categories_graph;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_SecurityLevelsDictionary, "SELECT id, type, name, priority FROM sec_levels;"));
		BMD_FOK(initSingleDataDictionary(	&_GLOBAL_MetadataValuesDictionary, "SELECT id, location_id, type, value FROM metadata_values;"));
	}

	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_UsersDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_UserGroupsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_UserClassesDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_GroupsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_ClassesDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_GroupsGraphDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_RolesDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_ActionsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_UserRolesDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_RolesAndActionsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_SecurityDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_AddMetadataTypesDictionary));

	if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
	{
		BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_RoleRightsStructureDictionary));
	}

	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_RoleRightsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_SecurityCategoriesDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_SecurityCategoriesGraphDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_SecurityLevelsDictionary));
	BMD_FOK(dumpSingleDataDictionary( hDB, &_GLOBAL_MetadataValuesDictionary));

	BMD_FOK(IsPossibleIdentitityRegistration(hDB)); // blad jesli tozsamosci wiecej, niz pozwala licencja

	BMD_FOK(bmd_db_disconnect(&hDB));

	/*******************************************************************************************/
	/* sprawdzenie czy wielkosci odpowiednich slownikow nie przekraczaja ograniczen z licencji */
	/*******************************************************************************************/
	if ((_GLOBAL_bmd_configuration->max_registered_users>0) && (_GLOBAL_UsersDictionary->rows>_GLOBAL_bmd_configuration->max_registered_users))	{	BMD_FOK(LIBBMDSQL_USER_TOO_MANY_REGISTERED_USERS);	}
	if ((_GLOBAL_bmd_configuration->max_registered_roles>0) && (_GLOBAL_RolesDictionary->rows>_GLOBAL_bmd_configuration->max_registered_roles))	{	BMD_FOK(LIBBMDSQL_ROLE_TOO_MANY_REGISTERED_ROLES);	}
	if ((_GLOBAL_bmd_configuration->max_registered_groups>0) && (_GLOBAL_GroupsDictionary->rows>_GLOBAL_bmd_configuration->max_registered_groups))	{	BMD_FOK(LIBBMDSQL_GROUP_TOO_MANY_REGISTERED_GROUPS);	}
	if ((_GLOBAL_bmd_configuration->max_registered_classes>0) &&(_GLOBAL_ClassesDictionary->rows>_GLOBAL_bmd_configuration->max_registered_classes))	{	BMD_FOK(LIBBMDSQL_CLASS_TOO_MANY_REGISTERED_CLASSES);	}


	return BMD_OK;
}

long updateAllDataDictionary()
{

	freeAllDataDictionary();
	dumpAllDataDictionaries(_GLOBAL_bmd_configuration->db_conninfo);

	return BMD_OK;
}

long dumpSingleDataDictionary(	void *hDB,
				dictionary_data_t **dictionary)
{
long i				= 0;
long j				= 0;
long select_rows		= 0;
long select_cols		= 0;
long fetched_counter		= 0;
void *SelectResult		= NULL;
db_row_strings_t *rowstruct	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dictionary==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/**************************************************/
	/* wykonanie polecenie na bazie, pobranie wynikow */
	/**************************************************/
	BMD_FOK(bmd_db_execute_sql(hDB, (*dictionary)->SQLQuery, &select_rows, &select_cols, &SelectResult));

	(*dictionary)->cols=select_cols;
	(*dictionary)->rows=select_rows;

	(*dictionary)->data=(char ***)malloc(sizeof(char**)*((*dictionary)->rows+1));
	memset((*dictionary)->data, 0, sizeof(char)*((*dictionary)->rows+1));
	for (i=0; i<(*dictionary)->rows; i++)
	{
		(*dictionary)->data[i]=(char **)malloc(sizeof(char*)*((*dictionary)->cols+1));
		memset((*dictionary)->data[i], 0, sizeof(char*)*((*dictionary)->cols+1));
		BMD_FOK(bmd_db_result_get_row(hDB, SelectResult, i, 0, &rowstruct, FETCH_NEXT, &fetched_counter));

		for (j=0; j<(*dictionary)->cols; j++)
		{
			asprintf(&((*dictionary)->data[i][j]), "%s", rowstruct->colvals[j]);

			if ((*dictionary)->data[i][j]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		bmd_db_row_struct_free(&rowstruct);
	}
	bmd_db_result_free(&SelectResult);

	return BMD_OK;
}

