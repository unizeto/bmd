#ifndef WIN32

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <sys/sem.h>

#define KEY_GENERATION_ATTEMPTS_MAX_COUNT 1000



// jawna deklaracja na potrzeby semctl()
union semun { 			
  	int val;					// wartosc dla SETVAL
  	struct semid_ds *buf; 		// bufor dla IPC_STAT i IPC_SET
  	unsigned short int *array;	// tablica dla GETALL i SETALL
  	struct seminfo *__buf;		// bufor dla IPC_INFO
};


/**
* @author WSz
* Stworzenie pojedynczego semafora binarnego (i ustawienie go jako odblokowany tj. po utworzeniu jego wartosc ustawiona jest na 1)
* @param semaphoreKey[out]
* @param semId[out]
*/
long bmdCreateSemaphore(key_t *semaphoreKey, int *semId)
{
long iter				= 0;
key_t semaphoreKeyTemp	= 1; // zeby nigdy nie bylo 0 (bo rand() % 0 moze dac 0)
int semIdTemp			= 0;
union semun semCtl;

	if(semaphoreKey == NULL)
		{ return -1; }
	if(semId == NULL) 
		{ return -2; }

	semCtl.val = 1;
		
	for(iter=0; iter < KEY_GENERATION_ATTEMPTS_MAX_COUNT; iter++)
	{
		semaphoreKeyTemp = semaphoreKeyTemp + (rand() % 5);
		
		semIdTemp = semget(semaphoreKeyTemp, 1, IPC_CREAT|IPC_EXCL|0660);
		if(semIdTemp == -1)
			{ continue; }
		
		break;
	}

	if(semIdTemp == -1)
		{ return -11; }
	
	if(semctl(semIdTemp, 0, SETVAL, semCtl)  == -1)
	{
		bmdRemoveSemaphore(&semIdTemp);
		return -12;
	}
	
	*semaphoreKey = semaphoreKeyTemp;
	semaphoreKeyTemp = 0;
	*semId = semIdTemp;
	semIdTemp = 0;
	
	return 0;
}


/**
* @author WSz
* Usuniecie pojedynczego semafora
* @param semaphoreId[in|out]
*/
long bmdRemoveSemaphore(int *semaphoreId)
{
long retVal		= 0;
union semun semCtl;

	if(semaphoreId == NULL)
		{ return -1; }
	if(*semaphoreId <= 0)
		{ return -2; }
	
	retVal = semctl(*semaphoreId, 0, IPC_RMID, semCtl);
	if(retVal == -1)
	{
		// PRINT_ERROR("Error: Unable to remove semaphore\n");
		return -11;
	}
	*semaphoreId = 0;
	
	return 0;
}


/**
* @author WSz
* Pobranie istniejacy pojedynczy semafor o kluczu semaphoreKey
* @param semaphoreKey[in]
* @param semId[out]
*/
long bmdGetExistingSemaphore(const key_t semaphoreKey, int* semId)
{
int semIdTemp		= 0;

	if(semaphoreKey <= 0)
		{ return -1; }
	if(semId == NULL)
		{ return -2; }

	semIdTemp = semget(semaphoreKey, 1, 0);
	if(semIdTemp == -1)
	{
		return -11;
	}
	
	*semId = semIdTemp;
	semIdTemp = 0;
	return 0;
}


/**
* @author WSz
* Funkcja blokuje pojedynczy semafor binarny (dekrementuje jego wartosc z 1 na 0)
* @param semId[in]
*/
long bmdLockSemaphore(const int semId)
{
struct sembuf operations[] = { {0, -1, 0} };

	if(semId <= 0)
		{ return -1; }

	if(semop(semId, operations, 1) == -1)
	{
		return -11;
	}
	
	return 0;
}


/**
* @author WSz
* Funkcja odblokowuje pojedynczy semafor binarny (inkrementuje jego wartosc z 0 na 1)
* @param semId[in]
*/
long bmdUnlockSemaphore(const int semId)
{
struct sembuf operations[] = { {0, 1, 0} };

	if(semId <= 0)
		{ return -1; }

	if(semop(semId, operations, 1) == -1)
	{
		return -11;
	}
	
	return 0;
}


/**
* @author WSz
* Funkcja WEWNETRZNA do usuniecia segmentu pamieci dzielonej (segment musi byc odlaczony)
* @param shmemId[in|out]
*/
long bmdRemoveShmem(int *shmemId)
{
	if(shmemId == NULL)
		{ return -1; }
	if(*shmemId <= 0)
		{ return -2; }
	
	if(shmctl(*shmemId, IPC_RMID, NULL) == -1)
		{ return -11; }
	
	*shmemId = 0;
	return 0;
}


/**
* @author WSz
* Funkcja WEWNETRZNA do tworzenia i nadawania uprawnien dla segmentu pamieci dzielonej
* @param shmemId[out]
* @param sizeInBytes[in]
* @todo jak ustawic innego wlasciciela segmentu pamieci dzielonej (moze przy ustwaianiu uprawnienia zamazuje sie wlasciel i robi sie root?)
*/
long bmdCreateShmem(int *shmemId, const long sizeInBytes)
{
key_t shmemKey		= 1; // zeby nigdy nie bylo 0 (bo rand() % 0 moze dac 0)
int shmemIdTemp		= 0;
long iter			= 0;
struct shmid_ds shmidStruct;
/*
struct shmid_ds statShmidStruct;
*/

	if(shmemId == NULL)
		{ return -1; }
	if(sizeInBytes <= 0)
		{ return -2; }
		
	for(iter=0; iter < KEY_GENERATION_ATTEMPTS_MAX_COUNT; iter++)
	{
		shmemKey = shmemKey + (rand() % 5);
	
		shmemIdTemp = shmget(shmemKey, sizeInBytes, IPC_CREAT|IPC_EXCL);
		if(shmemIdTemp == -1)
			{ continue; }
		
		break;
	}
	
	if(shmemIdTemp == -1)
		{ return -11; }
	
	/*
	memset(&statShmidStruct, 0, sizeof(struct shmid_ds));
	shmidStruct.shm_perm.mode = 0440;
	if(shmctl(*shmemId, IPC_STAT, &statShmidStruct) == -1)
	{
		shmctl(*shmemId, IPC_RMID, NULL); // usuniecie segmentu
		return -3;
	}
	*/
	
	// ustawienie uprawnien do segmentu pamieci dzielonej
	memset(&shmidStruct, 0, sizeof(struct shmid_ds));
	shmidStruct.shm_perm.mode = 0660;
	/*
	shmidStruct.shm_perm.uid = statShmidStruct.shm_perm.uid;
	shmidStruct.shm_perm.gid = statShmidStruct.shm_perm.gid;
	*/
	if(shmctl(shmemIdTemp, IPC_SET, &shmidStruct) == -1)
	{
		bmdRemoveShmem(&shmemIdTemp);
		return -12;
	}
	
	*shmemId = shmemIdTemp;
	shmemIdTemp = 0;
		
	return 0;
}


/**
* @author WSz
* Funkcja tworzy i przylacza segment pamieci dzielonej
* @param sizeInBytes[in]
* @param shmemPtr[out]
* @param shmemId[out]
*/
long bmdAllocShmem(const long sizeInBytes, void** shmemPtr, int* shmemId)
{
long retVal			= 0;
int shmemIdTemp		= 0;
void* shmemPtrTemp	= NULL;
	
	if(sizeInBytes <= 0)
		{ return -1; }
	if(shmemPtr == NULL)
		{ return -2; }
	if(shmemId == NULL)
		{ return -3; }
	
	retVal = bmdCreateShmem(&shmemIdTemp, sizeInBytes);
	if(retVal < 0)
	{
		PRINT_ERROR("Error: bmdCreateShmem() function (status = %li) \n", retVal);
		return -11;
	}
	
	shmemPtrTemp = shmat(shmemIdTemp, NULL, 0);
	if((int)((long)shmemPtrTemp) == -1)
	{
		bmdRemoveShmem(&shmemIdTemp);
		return -12;
	}
	memset(shmemPtrTemp, 0, sizeInBytes);
	
	*shmemPtr = shmemPtrTemp;
	shmemPtrTemp = NULL;
	*shmemId = shmemIdTemp;
	shmemIdTemp = 0;
	
	return 0;
}


/**
* @author WSz
* Funkcja odlacza i usuwa segment pamieci dzielonej
* Odlaczenie i usuniecie nie sa powiazane (patrz warunki w kodzie).
* @param shmemPtr[in|out]
* @param shmemId[in|out]
*/
long bmdFreeShmem(void** shmemPtr, int *shmemId)
{
long retVal		= 0;

	if(shmemPtr == NULL)
		{ return -1; }
	if(shmemId == NULL)
		{ return -3; }
	
	if(*shmemPtr != NULL)
	{
		retVal = shmdt(*shmemPtr);
		if(retVal == -1)
		{
			//PRINT_ERROR("Error: Unable to detach shared memory segment \n");
			return -11;
		}
		*shmemPtr = NULL;
	}
	
	if(*shmemId > 0)
	{
		retVal = shmctl(*shmemId, IPC_RMID, NULL);
		if(retVal == -1)
		{
			//PRINT_ERROR("Error: Unable to remove shared memory segment \n");
			return -12;
		}
		*shmemId = 0;
	}
	
	return 0;
}



#endif
