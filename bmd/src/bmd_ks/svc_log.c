#include <bmd/bmd_ks/bmd_ks.h>

extern int _GLOBAL_debug;
extern svc_conf_t *svc_conf;

void svc_log(DWORD errCode,WORD eventType,char * addData,char * function,LPVOID binData,DWORD binLength,DWORD win32ErrCode)
{
    #ifdef WIN32	
    HANDLE hEventSource;
    char * msgStrings[3];
	WORD nmsg=0;
	int log_level;

	if( svc_conf == NULL )
		log_level=SVC_LOG_LEVEL_DEBUG;
	else
		log_level=svc_conf->log_level;

	if( log_level == SVC_LOG_LEVEL_NONE )
		return;
    hEventSource = RegisterEventSourceA(NULL,SVCNAME);

	msgStrings[0]=NULL;msgStrings[1]=NULL;msgStrings[2]=NULL;

    if( hEventSource != NULL )
    {
		if( addData != NULL )
		{
			msgStrings[nmsg] = addData;
			nmsg++;
		}
		if( function != NULL )
		{
			if( log_level == SVC_LOG_LEVEL_DEBUG )
			{
				msgStrings[nmsg]=function;
				nmsg++;
			}
		}
		if( log_level == SVC_LOG_LEVEL_DEBUG )
		{
			char Buf[80];
			memset(Buf,0,sizeof(char)*80);
			StringCchPrintfA(Buf,80,"%i",win32ErrCode);
			msgStrings[nmsg]=Buf;
			nmsg++;
		}
		if( log_level == SVC_LOG_LEVEL_DEBUG )
		{
			ReportEventA(hEventSource,			/* event log handle */
				        eventType,				/* event type */
					    0,						/* event category */
						errCode,				/* SVC_ERROR event identifier */
						NULL,					/* no security identifier */
						nmsg,					/* size of lpszStrings array */
						binLength,						/* no binary data */
						msgStrings,				/* array of strings */
						binData);					/* no binary data */
		}
		else
		{
			ReportEventA(hEventSource,			/* event log handle */
				        eventType,				/* event type */
					    0,						/* event category */
						errCode,				/* SVC_ERROR event identifier */
						NULL,					/* no security identifier */
						nmsg,					/* size of lpszStrings array */
						0,						/* no binary data */
						msgStrings,				/* array of strings */
						NULL);
		}

        DeregisterEventSource(hEventSource);
    }
#else
	int fd;
	char *real_filename=NULL;
	char *log_buf=NULL;
	time_t t;
	struct tm s_tm;

	t=time(NULL);
	localtime_r(&t,&s_tm);
	asprintf(&real_filename,"%s/bmd_ks_%04i-%02i-%02i_%i.log",
                 svc_conf->log_dir,
		 s_tm.tm_year+1900,
		 s_tm.tm_mon+1,
		 s_tm.tm_mday,
		 (int)pthread_self());
	fd=open(real_filename,O_CREAT|O_APPEND|O_WRONLY,0666);
	if( fd < 0 ) /* dac znac do jakiegos pliku o tym */
		return;
	free0(real_filename);
	asprintf(&log_buf,
                 "errCode == %li eventType == %li addData == %s function == %s\n",
	         errCode,eventType,addData,function);
	write(fd,log_buf,strlen(log_buf));
	free0(log_buf);
	close(fd);
#endif
}

void print_debug(char *fmt,...)
{
	va_list ap;

    if( _GLOBAL_debug == 0 )
		return;     
	va_start(ap, fmt);
	vprintf(fmt,ap);
	va_end(ap); 
};

#ifdef WIN32
/* dodaje niezbedne dane zeby event-log systemowy odczytywal pelne informacje o bledach */
BOOL svc_registry_add(char * pszMsgDLL,DWORD dwNum)
{
   HKEY hk; 
   DWORD dwData, dwDisp,length; 
   char szBuf[MAX_PATH]; 
 

   memset(szBuf,0,sizeof(char)*MAX_PATH);
   StringCchPrintfA(szBuf,STRSAFE_MAX_CCH,
	                "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s",
					SVCNAME); 
 
   if( RegCreateKeyExA(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hk,&dwDisp) != ERROR_SUCCESS ) 
   {
	   printf("Could not create the registry key."); 
	   return FALSE;
   }
 
   length=sizeof(char)*lstrlenA(pszMsgDLL)+1;
   if( RegSetValueExA(hk,"EventMessageFile",0,REG_EXPAND_SZ,(LPBYTE)pszMsgDLL,length) != ERROR_SUCCESS ) 
   {
	   printf("Could not set the event message file."); 
	   RegCloseKey(hk); 
	   return FALSE;
   }
  
   dwData =EVENTLOG_ERROR_TYPE|EVENTLOG_WARNING_TYPE|EVENTLOG_INFORMATION_TYPE|EVENTLOG_SUCCESS;
 
   if( RegSetValueEx(hk,TEXT("TypesSupported"),0,REG_DWORD,(LPBYTE) &dwData,sizeof(DWORD)) != ERROR_SUCCESS ) 
   {
	   printf("Could not set the supported types.");
	   RegCloseKey(hk); 
	   return FALSE;
   }
 
   if( RegSetValueEx(hk,TEXT("CategoryMessageFile"),0,REG_EXPAND_SZ,(LPBYTE) pszMsgDLL,length) != ERROR_SUCCESS ) 
   {
	   printf("Could not set the category message file.");
	   RegCloseKey(hk); 
	   return FALSE;
   }
 
   if( RegSetValueEx(hk,TEXT("CategoryCount"),0,REG_DWORD,(LPBYTE) &dwNum,sizeof(DWORD)) != ERROR_SUCCESS ) 
   {
	   printf("Could not set the category count."); 
	   RegCloseKey(hk); 
	   return FALSE;
   }

   RegCloseKey(hk); 
   return TRUE;
}

BOOL svc_registry_delete(void)
{
	char szBuf[MAX_PATH];
	
	memset(szBuf,0,sizeof(char)*MAX_PATH);
	StringCchPrintfA(szBuf,STRSAFE_MAX_CCH,
		             "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s",
					 SVCNAME);

	if( RegDeleteKeyA(HKEY_LOCAL_MACHINE,szBuf) != ERROR_SUCCESS )
	{
		printf("Could not delete registry key\n");
		return FALSE;
	}

	return TRUE;
}
#endif

