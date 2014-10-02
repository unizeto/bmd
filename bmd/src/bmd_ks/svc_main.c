#include <bmd/bmd_ks/bmd_ks.h>

#ifdef WIN32
extern SERVICE_STATUS_HANDLE   svc_sh; 
extern HANDLE                  svc_se;
extern svc_conf_t              *svc_conf;
extern svc_p11_t               *_GLOBAL_svc_p11;
extern HANDLE *hThreads;
extern ks_thread_data_t **thread_data;

#pragma warning(disable:4100)

void WINAPI svc_ctrlhandler( DWORD dwCtrl )
{
     DWORD dwState = SERVICE_RUNNING;
     
     switch(dwCtrl) {  
        case SERVICE_CONTROL_STOP: 
              svc_reportstatus(SERVICE_STOP_PENDING, NO_ERROR, 0,0);
              SetEvent(svc_se);
              return;
        case SERVICE_CONTROL_INTERROGATE: 
             break; 
        default: 
             break;
   } 
   svc_reportstatus(dwState, NO_ERROR, 0,0);
}

void WINAPI svc_main(DWORD dwArgc, char **lpszArgv)
{
    int status;
	SOCKET s;
	char *error=NULL;

    svc_sh = RegisterServiceCtrlHandlerA(SVCNAME,svc_ctrlhandler);
    if( !svc_sh )
    {
		svc_log(MSG_SVC_ERROR_FAILED,SVC_LOG_ERROR,
			    "Unable to register main service program",
				"RegisterServiceCtrlHandler",
				NULL,0,
			    GetLastError());
        return; 
    } 
    
    svc_reportstatus( SERVICE_START_PENDING, NO_ERROR, 3000,0);
    
    /* utworzenie stop-eventu */
    svc_se = CreateEvent(NULL,TRUE,FALSE,NULL);
    if ( svc_se == NULL)
    {
        svc_reportstatus( SERVICE_STOPPED, NO_ERROR, 0,0);
        return;
    }

    /* wczytanie konfiguracji */
    status=svc_load_conf("c:\\bmd_ks\\bmd_ks.conf",&svc_conf);
    if( status != SVC_OK )
    {
		char Buffer[80];
		
		memset(Buffer,0,sizeof(char)*80);
		StringCchPrintfA(Buffer,80,"Original code %i",status);
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,Buffer,"svc_load_conf",NULL,0,0);
		svc_reportstatus(SERVICE_STOPPED,1,0,0);
		return;
    }

    /* patrz dokumentacja - na obecna chwile nie mam dostepu do modulu HSM */
    _putenv("CKNFAST_LOADSHARING=1");
	_putenv("CKNFAST_OVERRIDE_SECURITY_ASSURANCES=all");
    _putenv("CKNFAST_FAKE_ACCELERATOR_LOGIN=1");
    
    status=svc_pkcs11_init(svc_conf->pkcs11_lib,&_GLOBAL_svc_p11,&error);
    if( status != SVC_OK )
    {
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_pkcs11_init",NULL,0,0);
        svc_reportstatus(SERVICE_STOPPED,1,0,0);
        return;
    }
    free0(error);
    status=svc_pkcs11_prepare(svc_conf->sig_key_id,svc_conf->dec_key_id,svc_conf->pin,&_GLOBAL_svc_p11,&error);
    if( status != SVC_OK )
    {
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_pkcs11_prepare",NULL,0,0);
        svc_reportstatus(SERVICE_STOPPED,1,0,0);
        return;
    }
    free0(error);

    /* inicjalizacja sieci i utworzenie gniazda nasluchujacego */
    status=svc_net_initialize(&error);
	if( status != SVC_OK )
	{
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_net_initialize",NULL,0,GetLastError());
		return;
	}
    free0(error);

    status=svc_net_create_listen_socket(&s,svc_conf->adr,svc_conf->port,svc_conf->max_threads,&error);
	if( status != SVC_OK )
	{
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_net_create_listen_socket",NULL,0,0);
		return;
	}

	status=svc_create_threads(s);
	if( status != SVC_OK )
	{
		svc_gen_err_desc(&error,"Error code %i",status);
		svc_log(svc_map_err_code(status),SVC_LOG_ERROR,error,"svc_create_threads",NULL,0,GetLastError());
		return;
	}
    
	svc_log(MSG_SVC_OK,SVC_LOG_INFO,"BMD Keyservice",NULL,NULL,0,0);
    svc_reportstatus(SERVICE_RUNNING,0,0,0);
        
    /* czekanie na stop-event */
    while(WaitForSingleObject(svc_se, 1000) != WAIT_OBJECT_0)
    {
    
    }
        
    svc_reportstatus(SERVICE_STOPPED, NO_ERROR, 0, 0);
   
    return;
}
#endif

