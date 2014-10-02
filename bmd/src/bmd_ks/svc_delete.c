#include <bmd/bmd_ks/bmd_ks.h>

void svc_delete(void)
{
	#ifdef WIN32
    SC_HANDLE schSCManager,schService;
    
    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE); 
    if( schSCManager == NULL ) 
    {
		printf("Nie mozna nawiazac polaczenia z SCM. Kod WIN32 == %d\n", GetLastError());
        return;
    }

    schService=OpenService(schSCManager,TEXT("bmd_keyservice"),SERVICE_ALL_ACCESS);
    if( schService == NULL ) 
    {
		printf("Nie mozna otworzyc uslugi. Kod WIN32 == %d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }
    DeleteService(schService);

    CloseServiceHandle(schService);CloseServiceHandle(schSCManager);

	svc_registry_delete();
	printf("Skasowano usluge\n");
	#else
	return;
	#endif
}

