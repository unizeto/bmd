#include <bmd/libbmdaa2/wrappers/AC_RoleSyntax.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_RoleSyntax_create(GeneralNames_t *roleAuthority,
		GeneralName_t *roleName,
		RoleSyntax_t *RoleSyntax)
{
int err = 0;
	if(roleAuthority == NULL)
		return BMD_ERR_PARAM1;
	if(roleName == NULL)
		return BMD_ERR_PARAM2;
	if(RoleSyntax == NULL)
		return BMD_ERR_PARAM3;
	
	memset(RoleSyntax,0,sizeof(RoleSyntax_t));
	/* podpinam wskaznik z RoleAuthority */
	RoleSyntax->roleAuthority  = roleAuthority;
	err = AC_GeneralName_clone_pointers(roleName, &(RoleSyntax->roleName));
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in cloning GeneralName pointers. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

int AC_RoleSyntax_create_alloc(GeneralNames_t *roleAuthority,
		GeneralName_t *roleName,
		RoleSyntax_t **RoleSyntax)
{
int err = 0;
	if(roleAuthority == NULL)
		return BMD_ERR_PARAM1;
	if(roleName == NULL)
		return BMD_ERR_PARAM2;
	if(RoleSyntax == NULL || *RoleSyntax != NULL)
		return BMD_ERR_PARAM3;
	
	*RoleSyntax = (RoleSyntax_t *) malloc (sizeof(RoleSyntax_t));
	if(*RoleSyntax == NULL) return NO_MEMORY;
	
	err = AC_RoleSyntax_create(roleAuthority, roleName, *RoleSyntax);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating RoleSyntax pointers. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}
