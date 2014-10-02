#include <bmd/libbmd/libbmd.h>
#include <stdlib.h>
#include <string.h>
#include <bmd/common/bmd-sds.h>
#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif


#include <bmd/libbmdpkcs12/libbmdpkcs12.h>


int sprawdz_wynik_info_login_set( const char *bmd_address,const int bmd_port,const char *proxy_url,
			    const int proxy_port,const char *proxy_username,const char *proxy_password,const int proxy_type,
			    bmd_info_t **bi);

int sprawdz_file_to_P12 (void);

extern long _GLOBAL_debug_level;


int main(int argc, char ** argv)
{
	bmd_info_t * bi = NULL;

	_GLOBAL_debug_level=2;
	FOK(bmd_init());

// 	sprawdz_file_to_P12 ();

	FOK(bmd_info_create(&bi));
	FOK(bi==NULL);
	FOK(bmd_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));
	FOK(sprawdz_wynik_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));

	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));
	FOK(bmd_logout(&bi));
 	FOK(bmd_info_destroy(&bi));

	FOK(bmd_end());

	return 0;
}

/** Funkcja sprawdza poprawność działania funkcji info_login_set().
Funkcję należy wywołać po wcześniejeszym wywołaniu funkcji info_login_set() podając te same parametry
*/
int sprawdz_wynik_info_login_set( const char *bmd_address,const int bmd_port,const char *proxy_url,
			    const int proxy_port,const char *proxy_username,const char *proxy_password,const int proxy_type,
			    bmd_info_t **bi)
{
	int blad =0;
	int por = 0;
	if (*bi== NULL) exit(-1);
	if ( por=strncmp(((*bi)->bmd_address),bmd_address,50) )
	{	blad = 1;
		printf("\nbmd_address powinno być: %s\n a jest: %s\nwynik porownania: %i\n",bmd_address,(*bi)->bmd_address,por);
	}
	if ( (*bi)->bmd_port != bmd_port )
	{	blad += 2;
	}
	if (proxy_url)
	{	if ( por=strncmp(((*bi)->proxy_info.proxy_url),proxy_url,50) )
		{	blad += 4;
			printf("\nproxy_url powinno być: %s\n a jest: %s\nwynik porownania: %i\n",proxy_url,(*bi)->proxy_info.proxy_url,por);
		}
	}
	if (proxy_port)
	{	if ( (*bi)->proxy_info.proxy_port != proxy_port )
		{	blad += 8;
		}
	}
	if (proxy_username)
	{	if ( por=strncmp(((*bi)->proxy_info.proxy_username),proxy_username,50) )
		{	blad += 16;
			printf("\nproxy_username powinno być: %s\n a jest: %s\nwynik porownania: %i\n",proxy_username,(*bi)->proxy_info.proxy_username,por);
		}
	}
	if (proxy_password)
	{	if ( por=strncmp(((*bi)->proxy_info.proxy_password),proxy_password,50) )
		{	blad += 32;
		printf("\nproxy_password powi#include <bmd/libbmdpkcs12/libbmdpkcs12.h>nno być: %s\n a jest: %s\nwynik porownania: %i\n",proxy_password,(*bi)->proxy_info.proxy_password,por);
		}
	}
	if (proxy_port)
	{	if ( (*bi)->proxy_info.proxy_type != proxy_type )
		{	blad += 64;
		}
	}




	return blad;
}


int sprawdz_file_to_P12 (void)
{
	PKCS12 *p12=NULL;
	FOK( __file_to_P12("marua.pfx",&p12) );
	bmd_free(p12);

	return 0;
}
