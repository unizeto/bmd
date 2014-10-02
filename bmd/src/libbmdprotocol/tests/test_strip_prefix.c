/****************** plik testowy do funkcji ****************************\
int strip_prefix(GenBuf_t **input_buf,uint32_t *id,GenBuf_t **stripped_buf)
{
	int tmp_size;
	int is_ok;
	
	get_prefix_from_gb(*input_buf,id);
	tmp_size=(*input_buf)->size-sizeof(uint32_t);
	set_gen_buf((*input_buf)->buf+sizeof(uint32_t),&tmp_size,stripped_buf,&is_ok);
	free_gen_buf(input_buf);
	*input_buf = NULL;

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
	GenBuf_t *output2=NULL;

	output=(GenBuf_t *)malloc(sizeof(GenBuf_t*));
	
	output->buf=(unsigned char *)malloc(strlen("test funkcji"));
	memset(output->buf,0,strlen("test funkcji"));
	memmove(output->buf,"test funkcji",strlen("test funkcji"));

	value=100;

	FOK(set_prefix_from_int(value,&output));
	FOK(strip_prefix(&output,&value,&output2));
}
