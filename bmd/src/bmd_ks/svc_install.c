#include <bmd/bmd_ks/bmd_ks.h>

void svc_install(void)
{
	#ifdef WIN32
    SC_HANDLE schSCManager,schService;
    char szPath[MAX_PATH];
	int status;

	SERVICE_DESCRIPTION serv_desc;
	serv_desc.lpDescription=L"PKCS#11 Key service for BMD systems";

    if( !GetModuleFileNameA( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE); 
    if( schSCManager == NULL ) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = CreateServiceA(schSCManager,SVCNAME,DISP_SVCNAME,
							   SERVICE_ALL_ACCESS|SERVICE_CHANGE_CONFIG,SERVICE_WIN32_OWN_PROCESS,
                               SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,szPath,NULL,NULL,NULL,NULL,NULL);
    if (schService == NULL) 
    {
		printf("Nie mozna zainstalowac uslugi. Kod WIN32 == %d\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }
    else 
		printf("Zainstalowano usluge\n"); 

	/* ustaw opis */
	status=ChangeServiceConfig2(schService,SERVICE_CONFIG_DESCRIPTION,(LPVOID)&serv_desc);

    CloseServiceHandle(schService);CloseServiceHandle(schSCManager);

	svc_registry_add("c:\\bmd_ks\\exe\\svcmsg.dll",1);
	#else
	return;
	#endif
}
