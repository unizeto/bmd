/* Funkcje DLLMain - tylko pod WIN32 - do zmiany gdyby niezbedna okazala sie inicjalizacja na etapie przylaczanie sie do procesu */
#ifdef WIN32
#pragma warning(disable:4100)
#include <windows.h>
BOOL __stdcall DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
#endif
