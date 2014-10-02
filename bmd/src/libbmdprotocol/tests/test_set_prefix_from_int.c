/****************** plik testowy do funkcji ****************************\
int set_prefix_from_int(uint32_t value,GenBuf_t **output)
{
	uint32_t net_value;
	
	unsigned char *tmp=NULL;

	if (!( output != NULL && *output != NULL))
	{
		PRINT_ERROR("output == NULL || *output == NULL (LK)\n");
		abort();
	}
	
	net_value=htonl(value);
	
	tmp=realloc((*output)->buf,(*output)->size+4);
	if(tmp)
	{
		(*output)->buf=tmp;
		memmove((*output)->buf+4,(*output)->buf,(*output)->size);
		( *(uint32_t *)( (*output)->buf) )=net_value;
		(*output)->size=(*output)->size+4;
	}
	else
	{
		free_gen_buf(output);
		return -1;
	}
	
	return 0;
	
} 
\****************** plik testowy do funkcji ****************************/

#include <stdio.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <stdlib.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

int main()
{
	uint32_t value=0;
	int status=0;
	int i=0;
	GenBuf_t *output=NULL;

	output=(GenBuf_t *)malloc(sizeof(GenBuf_t*));
	
	output->buf=(unsigned char *)malloc(strlen("test funkcji"));
	memset(output->buf,0,strlen("test funkcji"));
	memmove(output->buf,"test funkcji",strlen("test funkcji"));

	value=100;

	FOK(set_prefix_from_int(value,&output));
}
