#include<bmd/libbmderr/libbmderr.h>
#include<bmd/libbmdpki/libbmdpki.h>
#include<bmd/libbmdlob/libbmdlob.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4100)
BOOL __stdcall DllMain(
		HANDLE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved
		)
{

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
		return TRUE;
}

#endif

#ifndef WIN32
	#include <bmd/common/global_types.h>
#endif //ifndef WIN32

long get_lob_hash(struct lob_transport **lob,GenBuf_t **hash_buf)
{
long status=0;

	PRINT_VDEBUG("LIBBMDLOBINF Getting lob's hash\n");
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
	if(hash_buf==NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid second parameter. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*hash_buf)!=NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid second parameter. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* finalna iteracja do policzenia wartosci skrotu */
	status=bmd_hash_data(NULL,&((*lob)->hash_ctx),hash_buf,NULL);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("LIBBMDLOBERR Error in hashing data. Error=%li\n",status);
		return status;
	}
    
	return BMD_OK;
}

long get_lob_timestamp(struct lob_transport **lob,GenBuf_t **ts_buf) 
{
long status=0;
GenBuf_t *hash=NULL;
 
	PRINT_VDEBUG("LIBBMDLOBINF Getting lob's timestamp\n");
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
	if(ts_buf==NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid second parameter. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*ts_buf)!=NULL)
	{
		PRINT_DEBUG("LIBBMDLOBERR Invalid second parameter. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
  
	if((*lob)->ts_info!=NULL)
	{
		status=get_lob_hash(lob,&hash);
		if (status!=BMD_OK)
		{
			PRINT_DEBUG("LIBBMDLOBERR Error in getting lob's hash. Error=%li\n",status);
			free_gen_buf(&hash);
			return status;
		}
		V_TRY("Timestamping data...\n");
		status=bmd_timestamp_data(hash,BMD_HASH_ALGO_SHA1,(*lob)->ts_info->server_name,(*lob)->ts_info->port,-1,NULL,-1,NULL,NULL,3, 0, ts_buf);
		if (status!=BMD_OK)
		{
			V_DENY("Timestamping data failed\n");
			PRINT_DEBUG("LIBBMDLOBERR Error in setting lob's timestamp. Error=%li\n",status);
			free_gen_buf(&hash);
			return status;
		}
		V_DONE("Data timestamped successfully\n");
		free_gen_buf(&hash);
	}

	return BMD_OK;
 
}
