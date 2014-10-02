/****************** plik testowy do funkcji ****************************\
long get_asn1_data_block_size(unsigned char *buffer,int length)
{
	long dlugosc_paczki=0;
	
	int i;
	char *tmp_buf=NULL,octet[3];
	
	if(buffer==NULL)
		return -1;
	
	
	
	PRINT_VDEBUG("%c %i\n",buffer[1],buffer[1]);
	if(buffer[1]&0x80)	
	{
		tmp_buf=(char *)malloc((buffer[1]&0x7F)*2+2);	
		memset(tmp_buf,0,(buffer[1]&0x7F)*2+1);
		for(i=0;i<(buffer[1]&0x7F);i++)			
		{
			if(buffer[2+i]<16)
				sprintf(octet,"0%X",buffer[2+i]);
			else
				sprintf(octet,"%X",buffer[2+i]);
			strcat(tmp_buf,octet);
			memset(octet,0,3);
		}
		dlugosc_paczki=strtol(tmp_buf,(char **)NULL,16);	
		dlugosc_paczki+=2+(buffer[1]&0x7F);
		if(tmp_buf)
		{
			free(tmp_buf);tmp_buf=NULL;
		}
	}
	else
		dlugosc_paczki=buffer[1]+2;	
	return dlugosc_paczki;
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
	int length;
	unsigned char *buffer=NULL; 
	int i=0;
	
	buffer=(unsigned char *)malloc(1000);

	for (i=0; i<900; i++)
	{
		buffer[i]=0x48;
		if (i==600)
		{
			buffer[i]=0x80;
		}
	}
	FOK(length=get_asn1_data_block_size(buffer,length));
}
