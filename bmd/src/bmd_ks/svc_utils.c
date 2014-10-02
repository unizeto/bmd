#include <bmd/bmd_ks/bmd_ks.h>

extern long _GLOBAL_debug;

DWORD svc_map_err_code(long svc_error_code)
{
	DWORD tmp;
	#ifdef WIN32
	tmp=MSG_SVC_ERROR_FAILED;
	#else
	tmp=SVC_ERROR_FAILED;
	#endif

	switch(svc_error_code) {
		case SVC_ERROR_PARAM:
			#ifdef WIN32	
			tmp=MSG_SVC_ERROR_PARAM;
			#else
			tmp=SVC_ERROR_PARAM;
			#endif 
			break;
		case SVC_ERROR_DISK:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_DISK;
			#else
			tmp=SVC_ERROR_DISK;
			#endif
			break;
		case SVC_ERROR_MEM:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_MEM;
			#else
			tmp=SVC_ERROR_MEM;
			#endif
			break;
		case SVC_ERROR_FORMAT:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_FORMAT;
			#else
			tmp=SVC_ERROR_FORMAT;
			#endif
			break;
		case SVC_ERROR_FAILED:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_FAILED;
			#else
			tmp=SVC_ERROR_FAILED;
			#endif
			break;
		case SVC_ERROR_NODATA:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_NODATA;
			#else
			tmp=SVC_ERROR_NODATA;
			#endif
			break;
		case SVC_ERROR_NET:
			#ifdef WIN32
			tmp=MSG_SVC_ERROR_NET;
			#else
			tmp=SVC_ERROR_NET;
			#endif
			break;
	};
	return tmp;
}

void svc_gen_err_desc(char **dest,char *fmt,...)
{

	va_list ap;

	(*dest)=(char *)malloc(256*sizeof(char));
	memset(*dest,0,256*sizeof(char));

	va_start(ap, fmt);
	vsprintf_s(*dest,256,fmt,ap);
	va_end(ap); 

	if( _GLOBAL_debug > 0 )
		printf("Generated error description |%s|",*dest);

	return;
}
