

#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmdutils/libbmdutils.h>

main(int argc, char **argv)
{

GenBuf_t *pfx=NULL;
GenBuf_t encoded64;
FILE *conf_base64=NULL;
char *new_filename=0;

if(argc != 2)
{
	printf("Wywolanie funkcji: base_pfx plik.pfx\n");
	return -1;
}

if( (bmd_load_binary_content(argv[1], &pfx)) != 0 )
{
	printf("Nie mozna wczytac pliku %s\n");
	return -1;
}

encoded64.buf=spc_base64_encode( (unsigned char *)pfx->buf, pfx->size, 0);
free_gen_buf(&pfx);
if(encoded64.buf == NULL)
{
	printf("Wystapil problem z zakodowaniem do base64\n");
	return -1;
}
encoded64.size=strlen((char*)(encoded64.buf));

if( (bmd_strmerge(argv[1],".conf", &new_filename)) != 0)
{
	free(encoded64.buf);
	printf("Wystapil problem z alokacja pamieci\n");
	return -1;
}

conf_base64=fopen(new_filename, "w+");
if(conf_base64 == NULL)
{
	free(encoded64.buf);
	free(new_filename);
	printf("Nie mozna utworzyc pliku do zapisu\n");
	return -1;
}
free(new_filename);

if( (fprintf(conf_base64, "update_pfx=\"%s\"", encoded64.buf)) < 0 )
{
	printf("Wystapil problem z zapisem do pliku\n");
	free(encoded64.buf);
	return -1;
}
free(encoded64.buf);
fclose(conf_base64);

printf("\nDONE\n");
return 0;
}