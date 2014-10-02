#include <bmd/bmd_ks/bmd_ks.h>
/*#include <bmd/common/bmd-os_specific.h>*/

#ifdef WIN32
SERVICE_STATUS_HANDLE   svc_sh; 
HANDLE                  svc_se = NULL;
HANDLE *hThreads=NULL;
#else
pthread_t *hThreads=NULL;
#endif

svc_conf_t              *svc_conf=NULL;
svc_p11_t               *_GLOBAL_svc_p11=NULL;
ks_thread_data_t **thread_data=NULL;
int            _GLOBAL_debug=0;

int main(int argc,char **argv) 
{
#ifdef WIN32    
    SERVICE_TABLE_ENTRYA DispatchTable[] = 
    { 
        { SVCNAME, (LPSERVICE_MAIN_FUNCTIONA) svc_main }, 
        { NULL, NULL } 
    };

    if( argc >= 2 )
    {
        if( strcmp(argv[1],"test") == 0 )
        {
            int status;    
			char *error=NULL;
            _GLOBAL_debug=1;
            status=svc_load_conf("c:\\bmd_ks\\bmd_ks.conf",&svc_conf);
            print_debug("svc_load_conf status %i\n",status);
            if( svc_conf != NULL )
            {
                print_debug("svc_conf->adr |%s|\n",svc_conf->adr);
                print_debug("|%i|\n",svc_conf->port);
                print_debug("|%s|\n",svc_conf->sig_key_id);
                print_debug("|%s|\n",svc_conf->dec_key_id);
                print_debug("|%s|\n",svc_conf->pkcs11_lib);
                print_debug("|%i|\n",svc_conf->max_threads);
                print_debug("|%s|\n",svc_conf->pin);
            }
    
            status=svc_pkcs11_init(svc_conf->pkcs11_lib,&_GLOBAL_svc_p11,&error);
            print_debug("svc_pkcs11_init %i\n",status);
            status=svc_pkcs11_prepare(svc_conf->sig_key_id,svc_conf->dec_key_id,svc_conf->pin,&_GLOBAL_svc_p11,&error);
            print_debug("svc_pkcs11_prepare %i\n",status);       
        }
        if( strcmp(argv[1],"install") == 0 )
        {
            svc_install();
            return;
        }

        if( strcmp(argv[1],"delete") == 0 )
        {
            svc_delete();
            return;
        }
        if( strcmp(argv[1],"p11_list") == 0 )
        {
			_GLOBAL_debug=1;
            svc_p11_list();
            return;
        }
    }
    
    if( StartServiceCtrlDispatcherA(DispatchTable) == 0 )
    {
		svc_log(MSG_SVC_ERROR_FAILED,SVC_LOG_ERROR,
			    "Unable to start Service Dispatcher Program",
				"StartServiceCtrlDispatcher",
				NULL,0,
				GetLastError());
		return SVC_ERROR_FAILED;
    }
#else
    long status,i;
    char *error=NULL;
    SOCKET s;
    if( argc != 2 )
    {
	printf("Poprawne wywolanie %s <conf_file>\n",argv[0]);
	return -1;
    }
	
    if( strcmp(argv[1],"p11_list") == 0 )
    {
	svc_p11_list();
	return 0;
    }
	
    status=svc_load_conf(argv[1],&svc_conf);
    if( status != SVC_OK )
    {
		printf("Nie wczytano konfiguracji serwisu kluczy\n");
		return SVC_ERROR_FAILED;
	}
	if( svc_conf->log_level == SVC_LOG_LEVEL_DEBUG )
		_GLOBAL_debug=1;
	setenv("CKNFAST_LOADSHARING","1",1);
	setenv("CKNFAST_OVERRIDE_SECURITY_ASSURANCES","all",1);
	setenv("CKNFAST_FAKE_ACCELERATOR_LOGIN","1",1);
	//status=svc_pkcs11_init(svc_conf->pkcs11_lib,&svc_p11,&error);
	if( status != SVC_OK )
	{
		printf("Nie zainicjalizowano podsystemu PKCS#11\n");
		exit(-1);
	}
	//status=svc_pkcs11_prepare(svc_conf->sig_key_id,svc_conf->dec_key_id,svc_conf->pin,&svc_p11,&error);
	if( status != SVC_OK )
	{
		printf("Nie przygotowano danych PKCS#11\n");
		//svc_p11->pFunctionList->C_Finalize(NULL);
		//dlclose(svc_p11->lHandle);
		exit(-1);
	}
	status=svc_net_create_listen_socket(&s,svc_conf->adr,svc_conf->port,svc_conf->max_threads,&error);
	if( status != SVC_OK )
	{
		printf("Nie przygotowano gniazda nasluchujacego\n");
		//svc_p11->pFunctionList->C_Finalize(NULL);
		//dlclose(svc_p11->lHandle);
		exit(-1);
	}
	close(0);close(1);close(2);
	status=svc_create_threads(s);
	if( status != SVC_OK )
	{
		printf("Nie przygotowano watkow nasluchujacych\n");
		//svc_p11->pFunctionList->C_Finalize(NULL);
		//dlclose(svc_p11->lHandle);
		exit(-1);
	}
	for(i=0;i<svc_conf->max_threads;i++)
		pthread_join(hThreads[i],NULL);			
#endif    
    return 0;

}
