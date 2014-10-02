/* AC_IA5String.c author: Marcin Szulga ver. 2.0.2 */
#include <bmd/libbmdaa2/wrappers/AC_IA5String.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_IA5String_clone_pointers(IA5String_t *src, IA5String_t *dest)
{
int err = 0;
	if(src == NULL)
		return BMD_ERR_PARAM1;
	if(dest == NULL)
		return BMD_ERR_PARAM2;
	
	memset(dest,0,sizeof(IA5String_t));
	dest->buf = src->buf;
	dest->size = src->size;
	dest->_asn_ctx.phase = src->_asn_ctx.phase;
	dest->_asn_ctx.step  = src->_asn_ctx.step;
	dest->_asn_ctx.left  = src->_asn_ctx.left;
	dest->_asn_ctx.ptr   = src->_asn_ctx.ptr;
	return err;
}
