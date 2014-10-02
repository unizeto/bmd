#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>

#define EINVOICE_RECP_REGEXP "^([A-Za-z0-9_.-]+)@.*$"

/** Funkcja pobiera pierwszy czlon adresu email - tytulowy adresat w tresci maila
@param email - adres email z którego pobrany zostanie człon adresu przed znakiem '@'
@param out_recipient - nazwa konta email pobrana z adresu - wartość zwracana przez funkcję
@retval 0 - ok
@retval -1 - bład wywołania funkcji pcre_compile
@retval -2 - bład wywołania funkcji pcre_exec
*/
long get_einvoice_recipient(char *email,char **out_recipient)
{
	pcre *rc			= NULL;
	long re 			= 0;
	const char *error		= NULL;
	int erroroffset			= 0;
	int ovector[6];
	long len			= 0;
	
	memset(ovector,0,6);
	rc=pcre_compile(EINVOICE_RECP_REGEXP,0,&error, &erroroffset,NULL);

	if(rc==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Error. Error=%i\n",-1);
		return -1;
	}
	
	re = pcre_exec(rc,NULL,email,(int)strlen(email),0,0,ovector,6);
	if(re<0)
	{
		PRINT_DEBUG("LIBBMDMAILERR Error. Error=%i\n",-2);
		return -2;
	}
	
	len=pcre_get_substring(email,ovector,re,1,(const char **)out_recipient);
	
	pcre_free(rc);
	rc=NULL;
	return 0;
}
