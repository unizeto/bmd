/* Funkcje inicjalizujace biblioteke, finalizujace ja */
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-errors.h>
long bmdnet_init()
{
	/* docelowo dla WIN32 tutaj bedzie wywolanie WSAStartup */
	return 0;
}

long bmdnet_destroy()
{
#ifdef WIN32
	long err;
	err=WSACleanup();
	if(err!=0)
		return BMD_ERR_OP_FAILED;
#endif
	return 0;
}
