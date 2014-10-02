#include <bmd/libbmdaa2/wrappers/AC_GeneralizedTime.h>
#include <bmd/libbmderr/libbmderr.h>
/* TODO implementation via OCTET_STRING */

int AC_GeneralizedTime_create_from_tm(struct tm *time, GeneralizedTime_t *GeneralizedTime)
{
int err = 0;
	if(time == NULL)
		return BMD_ERR_PARAM1;
	if(GeneralizedTime == NULL)
		return BMD_ERR_PARAM2;

	memset(GeneralizedTime,0,sizeof(GeneralizedTime_t));
	asn_time2GT(GeneralizedTime, time, 0); /*w GeneralizedTime.c w asn1clib*/
	if (GeneralizedTime < 0) return ERR_AA_TIMEERR; 
	return err;
}

int AC_GeneralizedTime_create_from_tm_alloc(struct tm *time1, GeneralizedTime_t **GeneralizedTime)
{
int err = 0;
	if(time1 == NULL)
		return BMD_ERR_PARAM1;
	if(GeneralizedTime == NULL || *GeneralizedTime != NULL)
		return BMD_ERR_PARAM2;

	*GeneralizedTime = (GeneralizedTime_t *) malloc (sizeof(GeneralizedTime_t));
	if(*GeneralizedTime == NULL) return NO_MEMORY;
		
	err = AC_GeneralizedTime_create_from_tm(time1, *GeneralizedTime);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating GeneralizedTime_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

int AC_GeneralizedTime_create_from_time(time_t *time, GeneralizedTime_t *GeneralizedTime)
{
int err = 0;
struct tm *slocal = NULL;

	if(time == NULL)
		return BMD_ERR_PARAM1;
	if(GeneralizedTime == NULL)
		return BMD_ERR_PARAM2;

	memset(GeneralizedTime,0,sizeof(GeneralizedTime_t));
	slocal = (struct tm *) malloc (sizeof (struct tm));
	if(slocal == NULL) return NO_MEMORY;
		
	slocal = localtime (time);
	asn_time2GT(GeneralizedTime, slocal, 1); /*w GeneralizedTime.c w asn1clib*/
	if (GeneralizedTime < 0) return ERR_AA_TIMEERR; 
	return err;
}

int AC_GeneralizedTime_create_from_time_alloc(time_t *time1, GeneralizedTime_t **GeneralizedTime)
{
int err = 0;
	if(time1 == NULL)
		return BMD_ERR_PARAM1;
	if(GeneralizedTime == NULL || *GeneralizedTime != NULL)
		return BMD_ERR_PARAM2;

	*GeneralizedTime = (GeneralizedTime_t *) malloc (sizeof(GeneralizedTime_t));
	if(*GeneralizedTime == NULL) return NO_MEMORY;

	err = AC_GeneralizedTime_create_from_time(time1, *GeneralizedTime);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating GeneralizedTime_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}
