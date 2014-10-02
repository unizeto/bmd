#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/common/bmd-os_specific.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdutils/libbmdutils.h>

#include <bmd/libbmd/libbmd_internal.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

long bmd_datagramset_free(bmdDatagramSet_t **dtg_set)
{
long status;

	PRINT_VDEBUG("LIBBMDINF Freeing datagram set\n");
	if(dtg_set==NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid first parameter value\n");
		return BMD_ERR_PARAM1;
	}
	if((*dtg_set)==NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid first parameter value\n");
		return BMD_ERR_PARAM1;
	}

	status=PR2_bmdDatagramSet_free(dtg_set);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("LIBBMDERR Error in freeing datagramset\n");
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

long bmd_datagramset_get_datagram(bmdDatagramSet_t *dtg_set,long nr,bmdDatagram_t **dtg)
{

	PRINT_VDEBUG("LIBBMDINF Gettin datagram from datagramset\n");
	if(dtg_set==NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid first parameter value\n");
		return BMD_ERR_PARAM1;
	}

	if(dtg_set->bmdDatagramSetTable==NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid first parameter value\n");
		return BMD_ERR_PARAM1;
	}

	if(dtg_set->bmdDatagramSetSize<=0)
	{
		PRINT_DEBUG("LIBBMDERR Invalid first parameter value\n");
		return BMD_ERR_PARAM1;
	}

	if(nr<0)
	{
		PRINT_DEBUG("LIBBMDERR Invalid second parameter value\n");
		return BMD_ERR_PARAM2;
	}

	if(dtg==NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid third parameter value\n");
		return BMD_ERR_PARAM3;
	}

	if((*dtg)!=NULL)
	{
		PRINT_DEBUG("LIBBMDERR Invalid third parameter value\n");
		return BMD_ERR_PARAM3;
	}

	if(nr>(int)dtg_set->bmdDatagramSetSize)
	{
		PRINT_DEBUG("LIBBMDERR Datagram number out of datagramset range\n");
		return BMD_ERR_OVERFLOW;
	}

	*dtg=dtg_set->bmdDatagramSetTable[nr];

	return BMD_OK;

}
