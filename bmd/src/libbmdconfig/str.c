#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

char* bmdconf_substr(char *string,long start,long end)
{
	long i, offset=0;
	char *new_string;
	if(start>=end)
		return NULL;
        new_string=(char*)malloc((end-start+1)*sizeof(char));
	for(i=start;i<end;i++)
	{
		new_string[offset]=string[i];
		offset++;
	}
	new_string[offset]='\0'	;
	return new_string;
}

void bmdconf_substr_free(char *str)
{
	free(str);
}

char* bmdconf_strmerge(char* src1,char *src2)
{
	char * buf = NULL;
	
	if (asprintf(&buf,"%s%s",src1,src2) < 1) {
	    PRINT_DEBUG("asprintf() failed!\n");
	}
	
	return buf;
}

void bmdconf_strmerge_free(char *str)
{
	free(str);
}

/*zwraca stringa z integera
	NULL jesli niepowodzenie*/
char* bmdconf_itostr(long liczba)
{
	char * buf = NULL;
	
	
	long result = asprintf(&buf, "%li", liczba);
	
	if (result < 1) {
		PRINT_DEBUG("asprintf() failed!\n");
	}
	
	return  buf;
}

void bmdconf_itostr_free(char *str)
{
	free(str);
}


/*-1 jesli nie znalazl znaku cudzyslowani w stringu
	0 jesli znalazl w stringu cudzyslow*/
long contain_quotation(char *string)
{
	long iter=0;
	long len=(int)strlen(string);
	for(iter=0; iter<len; iter++)
	{
		if(string[iter] == '\"')
		{
			return 0;
		}
	}
	return -1;
}

