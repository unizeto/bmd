#include <bmd/bmd_ks/bmd_ks.h>

#ifdef WIN32
extern SERVICE_STATUS_HANDLE   svc_sh; 
extern HANDLE                  svc_se;
#endif

void svc_reportstatus(DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint,DWORD dwCheckPoint)
{
	#ifdef WIN32
	SERVICE_STATUS svc_status;
     
    svc_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS; 
    svc_status.dwServiceSpecificExitCode=0;
    svc_status.dwCurrentState=dwCurrentState;
    svc_status.dwControlsAccepted=0;
    svc_status.dwWin32ExitCode = dwWin32ExitCode; 
    svc_status.dwServiceSpecificExitCode = 0; 
    svc_status.dwCheckPoint = dwCheckPoint; 
    svc_status.dwWaitHint = dwWaitHint; 
    
	if( dwCurrentState == SERVICE_START_PENDING )
        svc_status.dwControlsAccepted = 0;
    else
        svc_status.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
    
    SetServiceStatus(svc_sh, &svc_status);
	#else
	return;
	#endif
}
