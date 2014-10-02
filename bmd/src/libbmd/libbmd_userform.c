#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

int bmd_userform_free(BMD_attr_t ***user_form,int no_of_user_form)
{
	free_BMD_attr_list(user_form);
	return BMD_OK;
}
