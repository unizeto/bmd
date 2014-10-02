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

