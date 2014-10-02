#include<bmd/libbmdlob/libbmdlob.h>
#include<bmd/libbmderr/libbmderr.h>
#ifdef WIN32
#pragma warning(disable:4100)
#endif

long set_lob_size(struct lob_transport **lob, u_int64_t lob_size)
{
	PRINT_VDEBUG("LIBBMDLOBINF Setting lob size\n");
	if(lob==NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid first parameter. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*lob)==NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid first parameter. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	(*lob)->fs = lob_size;
	(*lob)->tfs = lob_size;

	return BMD_OK;
}

long get_lob_max_size(struct lob_transport **lob, u_int64_t * lob_size)
{
	return BMD_OK;
}

long get_lob_current_size(struct lob_transport **lob,
			 u_int64_t *lob_size)
{
	return BMD_OK;
}
long get_lob_in_size(struct lob_transport **lob,
			 u_int64_t *lob_size)
{
	return BMD_OK;
}
long get_lob_out_size(struct lob_transport **lob,
			 u_int64_t *lob_size)
{
    (*lob_size)=(*lob)->fs;
    return BMD_OK;
}
