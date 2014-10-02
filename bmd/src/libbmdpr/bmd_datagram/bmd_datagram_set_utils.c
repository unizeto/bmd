#define _WINSOCK2API_
#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagramSet.h>

/* Zwalniacz struktury bmdDatagramSet */
int PR_bmdDatagramSet_init(bmdDatagramSet_t *bmdDatagramSet)
{
	int err=0;
	bmdDatagramSet->bmdDatagramSetTable 	= NULL;
	bmdDatagramSet->bmdDatagramSetSize 	= 0;
	return err;
}


/* Wyswietlacz struktury bmdDatagramSet */
long PR_bmdDatagramSet_print(bmdDatagramSet_t *bmdDatagramSet, WithTime_t WithTime, FILE *fd)
{
int err = 0;
int i = 0;

struct tm czas;

	GetActualTime(&czas);
	fprintf(fd,"%02i-%02i-%02i %02i:%02i:%02i\n\n",czas.tm_year+1900, czas.tm_mon+1, czas.tm_mday,czas.tm_hour, czas.tm_min, czas.tm_sec);

	fprintf(fd,"*********************************************\n");

	fprintf(fd,"*            bmdDatagramSet                 *\n");

	fprintf(fd,"*********************************************\n\n\n");

	for(i=0; i<(int)bmdDatagramSet->bmdDatagramSetSize; i++)
	{

		fprintf(fd,"*************************\n");
		fprintf(fd,"*   bmdDatagram nr %i   *\n", i+1);
		fprintf(fd,"*************************\n");
		PR_bmdDatagram_print(bmdDatagramSet->bmdDatagramSetTable[i], WithTime, fd);


	}

	return err;
}
