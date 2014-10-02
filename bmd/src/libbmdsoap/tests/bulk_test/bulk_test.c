#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAME_SIZE 1000

int main()
{
pid_t pid = 0;
long iter = 0;
long jupiter = 0;
long proc_count = 1600;
char filename[NAME_SIZE];
	
	for(jupiter=0; jupiter < proc_count; jupiter++)
	{
		pid = fork();
		/*
		for(iter = 1 ; iter<= 1000; iter ++)
		{
			memset(filename, 0, NAME_SIZE);
			sprintf(filename, "plik%li.txt", iter);
			fopen(filename, "r");
		}
		*/

		if(pid == 0)
		{
			chdir("/home/wszczygiel/TRUNK/src/libbmdsoap/tests/");
			printf("wykonuję %li\n", jupiter);
			system("/home/wszczygiel/TRUNK/src/libbmdsoap/tests/bmdsoap_test_AdvancedSearchFiles_ssl_auth -d 4");
			break;
		}
		
	}

	if(pid > 0)
	{
		for(jupiter=0; jupiter < proc_count; jupiter++)
		{
			int proc_status;
			waitpid(&proc_status);
		}
	}
	

	
	printf("Zasypiam\n");
	sleep(proc_count/2);
	
	return 0;
}
