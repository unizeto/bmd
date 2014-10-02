/* AC_OBJECT_IDENTIFIER.c author: Marcin Szulga ver. 2.0.1 */
#include <bmd/libbmdaa2/wrappers/AC_OBJECT_IDENTIFIER.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_OBJECT_IDENTIFIER_clone_pointers(OBJECT_IDENTIFIER_t *src, OBJECT_IDENTIFIER_t *dest)
{
int err = 0;
	if(src == NULL)
		return BMD_ERR_PARAM1;
	if(dest == NULL)
		return BMD_ERR_PARAM2;
	
	dest->buf = src->buf;
	dest->size = src->size;
	return err;
}
