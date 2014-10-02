#define _WINSOCK2API_
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-openssl.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmd/libbmd.h>
#include <curl/curl.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
//HANDLE *lock_cs;

void win32_locking_callback(int mode, int type, char *file, int line)
{
	if (mode & CRYPTO_LOCK)
	{
		WaitForSingleObject(_GLOBAL_lock_cs[type],INFINITE);
	}
	else
	{
		ReleaseMutex(_GLOBAL_lock_cs[type]);
	}
}

void thread_setup(void)
{
	int i;
	PRINT_DEBUG("MSILEWICZ_SSL_THR CRYPTO_num_locks() %li\n",CRYPTO_num_locks());
	_GLOBAL_lock_cs=OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i=0; i<CRYPTO_num_locks(); i++)
	{
		_GLOBAL_lock_cs[i]=CreateMutex(NULL,FALSE,NULL);
	}

	CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))win32_locking_callback);
}

void thread_cleanup(void)
{
	int i;

	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++)
		CloseHandle(_GLOBAL_lock_cs[i]);
	OPENSSL_free(_GLOBAL_lock_cs);
}
#endif

/**Funkcja służy do inicjalizacji biblioteki libbmd. Każdy program korzystający z biblioteki
 * libbmd musi na początku wywołać tę funkcję, inicjalizuje ona zależne biblioteki i umożliwia
 * dalsze działanie z biblioteką.
 * \return Kod błędu lub wartość \b BMD_OK
 * */
long bmd_init(void)
{
	SSLeay_add_all_algorithms();
	bmdnet_init();
	curl_global_init(CURL_GLOBAL_ALL);
#ifdef WIN32
	thread_setup();
#endif
	return BMD_OK;
}
/**Funkcja służy do finalizacji działania biblioteki libbmd. Każdy program korzystający z
 * biblioteki libbmd musi na swoim końcu wywołać tę funkcję, zapewnia ona poprawne zakończenie
 * zależnych bibliotek oraz zapewnia, że żadne wrażliwe dane (takie jak klucze prywatne czy
 * hasła) nie pozostaną w pamięci operacyjnej.
 * \return Kod błędu lub wartość \b BMD_OK
 * */
long bmd_end(void)
{
	curl_global_cleanup();		// choć mimo wszystko, curl powoduje wyciek 865 B - dodane przez -RO
	bmdnet_destroy();
	EVP_cleanup();
	return BMD_OK;
}
