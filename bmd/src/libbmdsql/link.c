#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdsql/user.h>
#include <bmd/libbmdsql/group.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdlog/bmd_logs.h>


/**
* @author WSZ
* Funkcja GenerateINConditionList() na podstawie listy wartosci (identyfikatorow) generuje zawartows slq'owego warunku IN( wygenerowana_lista )
* @param[in] idsArray to lista identyfikatorow, z ktorych ma zostac zlozona lista sql'owego warunku IN
* @param[out] condition to zaalokowany lancuch znakowy z wygenerowana lista
* @return 0 w przypadku sukcesu, lub wartosc ujemna w przypadku bledu.
*/
long GenerateINConditionList(bmdStringArray_t* idsArray, char** condition)
{
long retVal		= 0;
long itemsCount		= 0;
long iter		= 0;
char* idPtr		= NULL;
char* tempCondition	= NULL;
char *temp		= NULL;


	if(idsArray == NULL)
		{ return -1; }
	if(condition == NULL)
		{ return -2; }
	if(*condition != NULL)
		{ return -3; }

	retVal=bmdStringArray_GetItemsCount(idsArray, &itemsCount);
	if(retVal < 0)
	{
		return -4;
	}
	if(itemsCount == 0)
	{
		return -5;
	}

	for(iter=0; iter<itemsCount ; iter++)
	{
		retVal=bmdStringArray_GetItemPtr(idsArray, iter, &idPtr);
		if(retVal < 0)
		{
			free(tempCondition);
			return -6;
		}
		if(iter == 0)
		{
			tempCondition=strdup(idPtr);
			if(tempCondition == NULL)
			{
				return -7;
			}
		}
		else
		{
			asprintf(&temp, "%s, %s", tempCondition, idPtr);
			free(tempCondition);
			if(temp == NULL)
			{
				return -8;
			}
			tempCondition=temp;
			temp=NULL;
		}
		idPtr=NULL;
	}

	*condition=tempCondition;
	tempCondition=NULL;
	return 0;
}


/**
* @author WSZ
* Funkcja IgnoreDuplicateLinks() pozwala na pominiecie przy tworzeniu linkow tych plikow,
* dla ktorych istnieja juz linki o pozadanych parametrach (tworca/wlasciciel/grupa).
*
* @param[in] hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param[in] locationId to identyfikator lokalizacyjny archiwum
* @param[in] creatorId to identyfikator tozsamosci, ktora okresla tworce poszukiwanych linkow
* @param[in] ownerId to identyfikator tozsamosci, ktora okresla wlasciciela poszukiwanych linkow
* @param[in] ownerGroupId to identyfikator grupy, dla ktorej sa wyszukiwane linki
* @param[in|out] idsArray to tablica identyfikatorow plikow, dla ktorych maja byc utworzone linki.
*	Tablica moze byc wygenerowana za pomoca funkcji GetCorrespondingIds().
*	W wyniku wykonywania funkcji, z tablicy usuniete zostana te identyfikatory, dla ktorych istnieja juz linki
*	o parametrach creatorId, ownerId i ownerGroupId.
*
* @return Funkcja zwraca 0 w przypadku prawidlowego zakonczenia, badz wartosc ujemna w przypadku bledu.
*/

long IgnoreDuplicateLinks(void* hDB, char* locationId, char* creatorId, char* ownerId, char* ownerGroupId, bmdStringArray_t** idsArray)
{
char *sqlTemplate="SELECT pointing_id FROM crypto_objects WHERE \n\
pointing_id IN (%s) \n \
AND file_type = 'link' \n \
AND location_id = %s \n \
AND creator = %s \n \
AND fk_owner = %s \n \
AND fk_group = %s ;";

// jesli link juz istnieje w niezamknietej transakcji, to tez jest traktowany jako duplikat
/* AND ( transaction_id = '' OR transaction_id IS NULL ) \n \ */
// podobnie w przypadku widocznosci i obecnosci (wartosci nie sa istotne)
/* AND present = 1 */


long retVal								= 0;
long iter								= 0;
long jupiter							= 0;
char* conditionInList					= NULL;
long elementsCount						= 0;
bmdStringArray_t* tempStringArray		= NULL;
const char* idPtr						= NULL;
long ignoreThisElement					= 0;

char* sqlQuery							= NULL;
void* queryResult						= NULL;
long rowsCount							= 0;
long fetchedCount						= 0;
db_row_strings_t *resultRow				= NULL;
char** duplicatesArray					= NULL;

	PRINT_INFO("LIBBMDSQLINFO Looking for duplicated links...\n");

	if(hDB == NULL)
		{ return -1; }
	if(locationId == NULL)
		{ return -2; }
	if(creatorId == NULL)
		{ return -3; }
	if(ownerId == NULL)
		{ return -4; }
	if(ownerGroupId == NULL)
		{ return -5; }
	if(idsArray == NULL)
		{ return -6; }
	if(*idsArray == NULL)
		{ return -7; }


	retVal=bmdStringArray_GetItemsCount(*idsArray, &elementsCount);
	if(retVal < 0)
		{ return -11; }
	
	if(elementsCount == 0)
		{ return 0; } // jesli pusta lista, to na pewno brak duplikatu

	retVal=GenerateINConditionList(*idsArray, &conditionInList);
	if(retVal < 0)
		{ return -12; }
	
	asprintf(&sqlQuery, sqlTemplate, conditionInList, locationId, creatorId, ownerId, ownerGroupId);
	free(conditionInList); conditionInList=NULL;
	if(sqlQuery == NULL)
		{ return -13; }
	
	retVal=bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal == BMD_ERR_NODATA)
		{ return 0; } // nie wykryto duplikatu
	else if(retVal < BMD_OK)
		{ return -14; }
	
	if(rowsCount == 0) // na wszelki wypadek
	{
		bmd_db_result_free(&queryResult);
		return 0; // nie wykryto duplikatu
	}
	
	// tworzenie wygodniejszej tablicy wynikow zapytania
	duplicatesArray=(char**)calloc(rowsCount, sizeof(char*));
	if(duplicatesArray == NULL)
	{
		bmd_db_result_free(&queryResult);
		return -15;
	}
	
	for(iter=0; iter<rowsCount; iter++)
	{
		retVal=bmd_db_result_get_row(hDB, queryResult, iter, 0, &resultRow, FETCH_ABSOLUTE, &fetchedCount);
		if(retVal < BMD_OK)
		{
			DestroyTableOfStrings(&duplicatesArray, rowsCount);
			bmd_db_result_free(&queryResult);
			return -16;
		}
		
		if(resultRow->colvals[0] == NULL)
		{
			DestroyTableOfStrings(&duplicatesArray, rowsCount);
			bmd_db_row_struct_free(&resultRow);
			bmd_db_result_free(&queryResult);
			return -17;
		}
		
		duplicatesArray[iter] = strdup(resultRow->colvals[0]);
		bmd_db_row_struct_free(&resultRow);
		if(duplicatesArray[iter] == NULL)
		{
			DestroyTableOfStrings(&duplicatesArray, rowsCount);
			bmd_db_result_free(&queryResult);
			return -18;
		}
	}
	bmd_db_result_free(&queryResult);
	
	
	retVal=bmdStringArray_Create(&tempStringArray);
	if(retVal < 0)
	{
		DestroyTableOfStrings(&duplicatesArray, rowsCount);
		return -19;
	}
	
	// kazdy znaleziony wpis trzeba usunac z listy do tworzenia linkow
	for(iter=0; iter<elementsCount; iter++)
	{
		retVal=bmdStringArray_GetItemPtr(*idsArray, iter, (char**)(&idPtr));
		if(retVal < 0)
		{
			DestroyTableOfStrings(&duplicatesArray, rowsCount);
			return -20;
		}
		
		ignoreThisElement=0;
		for(jupiter=0; jupiter<rowsCount; jupiter++)
		{
			if(strcmp(idPtr, duplicatesArray[jupiter]) == 0)
			{
				ignoreThisElement=1;
				break;
			}
		}
		
		if(ignoreThisElement == 0)
		{
			retVal=bmdStringArray_AddItem(tempStringArray, (char*)idPtr);
			if(retVal < 0)
			{
				DestroyTableOfStrings(&duplicatesArray, rowsCount);
				return -21;
			}
		}
		idPtr=NULL;
	}
	DestroyTableOfStrings(&duplicatesArray, rowsCount);
	
	bmdStringArray_DestroyList(idsArray);
	*idsArray = tempStringArray;
	tempStringArray = NULL;
	
	return 0;
}



long GetFileNamesForIds(void *hDB, bmdStringArray_t** Ids, bmdStringArray_t** fileNames)
{
char* queryTemplate="SELECT id, name FROM crypto_objects WHERE \
location_id=%s \
AND id IN( %s );";
char* sqlQuery				= NULL;
long retVal				= 0;
long iter				= 0;
bmdStringArray_t* tempArrayFileNames	= NULL;
bmdStringArray_t* tempArrayIds		= NULL;
char* tempStringId			= NULL;
char* tempStringFileName		= NULL;
void* queryResult			= NULL;
char* conditionInList			= NULL;
long rowsCount				= 0;

long fetchedCount			= 0;
long idsCount				= 0;


	if(hDB == NULL)
		{ return -1; }
	if(Ids == NULL)
		{ return -2; }
	if(*Ids == NULL)
		{ return -3; }
	if(fileNames == NULL)
		{ return -4; }
	if(*fileNames != NULL)
		{ return -5; }
	if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->location_id == NULL)
		{ return -6; }

	retVal=bmdStringArray_Create(&tempArrayFileNames);
	if(retVal < 0)
		{ return -11; }
	retVal=bmdStringArray_Create(&tempArrayIds);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -12;
	}

	retVal=GenerateINConditionList(*Ids, &conditionInList);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&tempArrayIds);
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -13;
	}

	asprintf(&sqlQuery, queryTemplate, _GLOBAL_bmd_configuration->location_id, conditionInList);
	free(conditionInList); conditionInList=NULL;
	if(sqlQuery == NULL)
	{
		bmdStringArray_DestroyList(&tempArrayIds);
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -14;
	}

	retVal=bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK)
	{
		bmdStringArray_DestroyList(&tempArrayIds);
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -15;
	}

	retVal=bmdStringArray_GetItemsCount(*Ids, &idsCount);
	if(retVal < BMD_OK)
	{
		bmd_db_result_free(&queryResult);
		bmdStringArray_DestroyList(&tempArrayIds);
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -21;
	}

	if(rowsCount != idsCount)
	{
		bmd_db_result_free(&queryResult);
		bmdStringArray_DestroyList(&tempArrayIds);
		bmdStringArray_DestroyList(&tempArrayFileNames);
		return -22;
	}

	for(iter=0; iter<rowsCount; iter++)
	{
		retVal=bmd_db_result_get_value(hDB, queryResult, iter, 0, &tempStringId, FETCH_ABSOLUTE, &fetchedCount);
		if(retVal < BMD_OK)
		{
			bmdStringArray_DestroyList(&tempArrayIds);
			bmdStringArray_DestroyList(&tempArrayFileNames);
			return -16;
		}
		retVal=bmd_db_result_get_value(hDB, queryResult, iter, 1, &tempStringFileName, FETCH_ABSOLUTE, &fetchedCount);
		if(retVal < BMD_OK)
		{
			free(tempStringId);
			bmdStringArray_DestroyList(&tempArrayIds);
			bmdStringArray_DestroyList(&tempArrayFileNames);
			return -17;
		}

		retVal=bmdStringArray_AddItem(tempArrayIds, tempStringId);
		free(tempStringId); tempStringId=NULL;
		if(retVal < 0)
		{
			free(tempStringFileName);
			bmdStringArray_DestroyList(&tempArrayIds);
			bmdStringArray_DestroyList(&tempArrayFileNames);
			return -18;
		}

		retVal=bmdStringArray_AddItem(tempArrayFileNames, tempStringFileName);
		free(tempStringFileName); tempStringFileName=NULL;
		if(retVal < 0)
		{
			bmdStringArray_DestroyList(&tempArrayIds);
			bmdStringArray_DestroyList(&tempArrayFileNames);
			return -19;
		}
	}

	bmd_db_result_free(&queryResult);
	
	bmdStringArray_DestroyList(Ids);
	*Ids=tempArrayIds;
	tempArrayIds=NULL;

	*fileNames=tempArrayFileNames;
	return 0;
}

/**
* @author WSZ
* Funkcja GetLinksForIds() pozwala na odszukanie linkow, ktore wskazuja na pliki o identyfikatorach zapisanych w tablicy fileIds,
* oraz ktore zostaly stworzone przez uzytkownika identyfikowanego przez requesterIdentityId dla uzytkownika identyfikowanego przez
* linksOwnerIdentityId. Parametr visibility dodatkowo pozwala okreslic, czy maja byc wyszukiwane linki widoczne/niewidoczne.
* Poniewaz funkcja jest zaprojektowana na potrzeby kasdadowego wycofywania udostepnien, na wyjsciu otrzymuje sie
* tablice linksIds z identyfikatorami odnalezionych linkow, oraz tablice symkeys oraz lobs identyfikatorami bazodanowymi potrzebnymi
* do usuniecia odpowiednio loba klucza symetrycznego oraz zaszyfrowany opis linka. Kolejnosc elementow w tablicach symkeys oraz lobs
* odpowiada kolejnosci linkow w linksIds.
*
* @param[in] hDB to uchwyt do nawiazanego polaczenia bazodanowego
* @param[in] locationId to indentyfikator lokalizacji archiwum
* @param[in] fileIds to lista identyfikatorow plikow, dla ktorych maja zostac odnalezione linki. Oczekuje sie, ze beda to identyfikatory
* "prawdziwych" plikow a nie linki. Lista ta moze byc przygotowana funcja GetCorrespondingIds().
* @param[in] requesterIdentityId to identyfikator tozsamosci uzytkownika tworcy szukanych linkow
* @param[in] linksOwnerIdentityId to identyfikator tozsamosci uzytkownika, dla ktorego zostaly stworzone wyszukiwane linki
* @param[in] linksOwnerGroupId to identyfikator grupy, dla ktorej zostaly stworzone wyszukiwane linki. Parametr jest opcjonalny
* i mozna podac NULL, jesli wyszukiwanie nie ma byc zawezone do linkow dla konkretnej grupy uzytkownikow
* @param[in] visibility pozwala okreslic, czy maja byc wyszukiwane linki widoczne, niewidoczne, wszystkie bez wzgledu na widocznosc
* @param[out] linksIds to alokowana lista identyfikatorow wyszukanych linkow. Jesli nie odnaleziono linkow spelniajacych kryteria
* wyszukiwania, to bedzie to lista pusta (bez elementow) 
* @param[out] symkeys to alokowana lista identyfikatorow bazodanowych dla kluczy symetrycznych skojarzonych z odpowiednimi linkami.
* Jesli nie odnaleziono linkow spelniajacych kryteria wyszukiwania, to bedzie to lista pusta (bez elementow) 
* @param[out] lobs to alokowana lista identyfikatorow bazodanowych dla zaszyfrowanych opisow skojarzonych z odpowiednimi linkami.
* Jesli nie odnaleziono linkow spelniajacych kryteria wyszukiwania, to bedzie to lista pusta (bez elementow).
* @return BMD_OK jesli sukces operacji (w tym brak elementow), w przeciwnym razie wartosc ujemna oznaczajaca blad
*/
long GetLinksForIds(void* hDB, char* locationId, bmdStringArray_t* fileIds, char* requesterIdentityId, char* linksOwnerIdentityId, char* linksOwnerGroupId, bmdVisibility_e visibility, bmdStringArray_t** linksIds, bmdStringArray_t** symkeys, bmdStringArray_t** lobs)
{
long retVal			= 0;
long iter			= 0;

bmdStringArray_t *tempIdsArray		= NULL;
bmdStringArray_t *tempSymkeysArray	= NULL;
bmdStringArray_t *tempLobsArray		= NULL;
char* conditionInList			= NULL;
char* visibilityCondition		= NULL;
char* ownerGroupCondition		= NULL;
long fileIdsCount			= 0;

char* queryTemplate="SELECT id, symkey, raster FROM crypto_objects WHERE \n\
location_id=%s \n\
AND file_type='link' \n\
AND pointing_id IN (%s) \n\
AND creator=%s \n\
AND fk_owner=%s \n\
%s /*opcjonalny warunek na grupe*/ \n\
%s /*warunek na visible*/ \n\
AND present=1 \n\
AND (transaction_id='' OR transaction_id IS NULL);";

char* sqlQuery			= NULL;
long rowsCount			= 0;
void* queryResult		= NULL;
db_row_strings_t *resultRow	= NULL;
long fetchedCounter		= 0;

	if(hDB == NULL)
		{ return -1; }
	if(locationId == NULL)
		{ return -2; }
	if(fileIds == NULL)
		{ return -3; }
	if(requesterIdentityId == NULL)
		{ return -3; }
	if(strlen(requesterIdentityId) <= 0)
		{ return -4; }
	if(linksOwnerIdentityId == NULL)
		{ return -5; }
	if(strlen(linksOwnerIdentityId) <= 0)
		{ return -6; }
	if(linksIds == NULL)
		{ return -7; }
	if(*linksIds != NULL)
		{ return -8; }
	if(symkeys == NULL)
		{ return -9; }
	if(*symkeys != NULL)
		{ return -10; }
	if(lobs == NULL)
		{ return -11; }
	if(*lobs != NULL)
		{ return -12; }

	retVal=bmdStringArray_GetItemsCount(fileIds, &fileIdsCount);
	if(retVal < 0)
		{ return -21; }
	if(fileIdsCount <= 0)
		{ return -22; }

	if(visibility == ALL)
		{ visibilityCondition=strdup(""); }
	else if(visibility == INVISIBLE)
		{ visibilityCondition=strdup("AND visible='0'"); }
	else
		{ visibilityCondition=strdup("AND visible='1'"); }
	
	if(visibilityCondition == NULL)
		{ return -23; }
	
	if(linksOwnerGroupId != NULL)
		{ asprintf(&ownerGroupCondition, "AND fk_group=%s", linksOwnerGroupId); }
	else
		{ ownerGroupCondition=strdup(""); }
	
	if(ownerGroupCondition == NULL)
	{
		free(visibilityCondition);
		return -24;
	}

	retVal=GenerateINConditionList(fileIds, &conditionInList);
	if(retVal < 0)
	{
		free(ownerGroupCondition);
		free(visibilityCondition);
		return -25;
	}

	asprintf(&sqlQuery, queryTemplate, locationId, conditionInList, requesterIdentityId, linksOwnerIdentityId, ownerGroupCondition, visibilityCondition);
	free(ownerGroupCondition); ownerGroupCondition=NULL;
	free(visibilityCondition); visibilityCondition=NULL;
	free(conditionInList); conditionInList=NULL;
	if(sqlQuery == NULL)
		{ return -26; }


	retVal=bmdStringArray_Create(&tempIdsArray);
	if(retVal < BMD_OK)
	{
		free(sqlQuery);
		return -27;
	}
	
	retVal=bmdStringArray_Create(&tempSymkeysArray);
	if(retVal < BMD_OK)
	{
		bmdStringArray_DestroyList(&tempIdsArray);
		free(sqlQuery);
		return -28;
	}

	retVal=bmdStringArray_Create(&tempLobsArray);
	if(retVal < BMD_OK)
	{
		bmdStringArray_DestroyList(&tempSymkeysArray);
		bmdStringArray_DestroyList(&tempIdsArray);
		free(sqlQuery);
		return -29;
	}

	retVal=bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal < BMD_OK && retVal != BMD_ERR_NODATA)
	{
		bmdStringArray_DestroyList(&tempLobsArray);
		bmdStringArray_DestroyList(&tempSymkeysArray);
		bmdStringArray_DestroyList(&tempIdsArray);
		return -30;
	}
	else if(retVal == BMD_ERR_NODATA) //pusta lista, jesli nie odnaleziono linkow spelniajacych okreslone kryteria
	{
		*linksIds=tempIdsArray; tempIdsArray=NULL;
		*symkeys=tempSymkeysArray; tempSymkeysArray=NULL;
		*lobs=tempLobsArray;  tempLobsArray=NULL;
		return BMD_OK;
	}

	for(iter=0; iter<rowsCount; iter++)
	{
		retVal=bmd_db_result_get_row(hDB, queryResult, iter, 0, &resultRow, FETCH_ABSOLUTE, &fetchedCounter);
		if(retVal < BMD_OK)
		{
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempLobsArray);
			bmdStringArray_DestroyList(&tempSymkeysArray);
			bmdStringArray_DestroyList(&tempIdsArray);
			return -31;
		}

		retVal=bmdStringArray_AddItem(tempIdsArray, resultRow->colvals[0]);
		if(retVal < BMD_OK)
		{
			bmd_db_row_struct_free(&resultRow);
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempLobsArray);
			bmdStringArray_DestroyList(&tempSymkeysArray);
			bmdStringArray_DestroyList(&tempIdsArray);
			return -32;
		}

		retVal=bmdStringArray_AddItem(tempSymkeysArray, resultRow->colvals[1]);
		if(retVal < BMD_OK)
		{
			bmd_db_row_struct_free(&resultRow);
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempLobsArray);
			bmdStringArray_DestroyList(&tempSymkeysArray);
			bmdStringArray_DestroyList(&tempIdsArray);
			return -33;
		}

		retVal=bmdStringArray_AddItem(tempLobsArray, resultRow->colvals[2]);
		bmd_db_row_struct_free(&resultRow);
		if(retVal < BMD_OK)
		{
			bmd_db_result_free(&queryResult);
			bmdStringArray_DestroyList(&tempLobsArray);
			bmdStringArray_DestroyList(&tempSymkeysArray);
			bmdStringArray_DestroyList(&tempIdsArray);
			return -34;
		}
	}
	bmd_db_result_free(&queryResult);

	*linksIds=tempIdsArray;
	tempIdsArray=NULL;

	*symkeys=tempSymkeysArray;
	tempSymkeysArray=NULL;

	*lobs=tempLobsArray;
	tempLobsArray=NULL;

	return 0;
}

/**
* @author WSZ
* Funkcja GetCorrespondingIds() wyszukuje pliki (nie bedace linkami) powiazane z plikiem o identyfikatorze rootFileId. W wyszukiwaniu
* brane sa pod uwage pliki oraz udostepnienia, ktorych wlascicielem jest uzytkownik o tozsamosci requesterIdentityId, ale w zwracanej
* liscie znajduja sie tylko identyfikatory "prawdziwych" plikow (w przypadku linka wyciagany jest id pliku wskazywanego przez ten link).
* Wynikowa lista identyfikatorow uwzglednia tylko te pliki, ktore moga byc dalej udostepnianie przez wlasciciela requesterIdentityId
* (wartosc for_grant=1 dla pliku/udostepnienia do pliku). Wzwracanej liscie, na pierwszej pozycji bedzie identyfikator rootFileId
* (lub id pliku wskazywanego przez rootFileId), o ile requesterIdentityId ma mozliwosc dalszego jego udostepniania.
* @param[in] hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param[in] locationId to identyfikator lokalizacji archiwum
* @param[in] requesterIdentityId to identyfikator tozsamosci wlasciciela wyszukiwanych plikow
* @param[in] rootFileId to identyfikator pliku, ktory jest korzeniem wyszukiwanego drzewa powiazanych plikow
* @param[in] rootFileForGrant to wartosc okreslajaca, czy rootFileId jest przeznaczony do dalszego udostepniania ("0"-nie, "1"-tak)
* @param[in] treeLevel, to ograniczenie wyszukiwania do wskazanego poziomu drzewa plikow powiazanych
* wartosci <= 0 oznaczaja brak ograniczenia, 1 oznacza pierwszy poziom (tylko rootFileId), 2 oznacza rootFileId i pliki bezposrednio do niego dowiazane, itd.
* @param[in] visibility pozwala okreslic, czy maja byc wyszukiwane pliki widoczne, niewidoczne, wszystkie bez wzgledu na widocznosc
* @param[out] correspondingIds to alokowana lista identyfikatorow wyszukanych plikow. Jesli nie odnaleziono plikow spelniajacych kryteria
* wyszukiwania, to bedzie to lista pusta (bez elementow) .
* @return Funkcja zwraca 0 w przypadku poprawnego wykonania funkcji, lub wartosc ujemna w przypadku bledu.
*/
long GetCorrespondingIds(void *hDB, char *locationId, char* requesterIdentityId, char *rootFileId, char* rootFileForGrant, long treeLevel, bmdVisibility_e visibility, bmdStringArray_t** correspondingIds)
{
bmdStringArray_t *idsFromLevel		= NULL;
bmdStringArray_t *idsFromNextLevel	= NULL;
bmdStringArray_t *tempIdsArray		= NULL;
bmdStringArray_t *grantsFromLevel	= NULL;
bmdStringArray_t *grantsFromNextLevel	= NULL;
long retVal				= 0;
long iter 				= 0;
long levelNumber			= 0;
long itemsCount				= 0;
char* idPtr				= 0;
char* conditionInList			= NULL;
char* visibilityCondition		= NULL;
char* forGrantPtr			= NULL;

//wyszukiwanie dzieci dopoki osiagne odpowiedniego poziomu, badz poziom da pusty zbior identyfikatorow
char *queryTemplate="SELECT DISTINCT ON (pointing_id) pointing_id, for_grant FROM crypto_objects WHERE \
fk_owner='%s' \
AND corresponding_id IN (%s) \
AND (transaction_id IS NULL OR transaction_id='') \
AND location_id='%s' \
AND file_type <> 'dir' \
%s /*condition for visible*/ \
AND present='1' \
ORDER BY pointing_id, for_grant DESC;";
//wyniki musza byc sortowane malejaca, aby distinc on zwracal obiekty przeznaczone do udostepniania (for_grant = 1)
char *sqlQuery				= NULL;
void* queryResult			= NULL;
long rowsCount				= 0;
db_row_strings_t *resultRow		= NULL;
long fetchedCounter			= 0;

	if(hDB == NULL)
		{ return -1; }
	if(locationId == NULL)
		{ return -2; }
	if(requesterIdentityId == NULL)
		{ return -3; }
	if(strlen(requesterIdentityId) <= 0)
		{ return -4; }
	if(rootFileId == NULL)
		{ return -5; }
	if(strlen(rootFileId) <=0)
		{ return -6; }
	if(rootFileForGrant == NULL)
		{ return -7; }
	if(strlen(rootFileForGrant) <= 0)
		{ return -8; }
	if(correspondingIds == NULL)
		{ return -10; }
	if(*correspondingIds != NULL)
		{ return -11; }

	if(visibility == ALL)
		{ visibilityCondition=strdup(""); }
	else if(visibility == INVISIBLE)
		{ visibilityCondition=strdup("AND visible='0'"); }
	else
		{ visibilityCondition=strdup("AND visible='1'"); }
	
	if(visibilityCondition == NULL)
		{ return -21; }


	retVal=bmdStringArray_Create(&tempIdsArray);
	if(retVal < 0)
	{
		free(visibilityCondition);
		return -22;
	}

	if(treeLevel <= 0) //pelne drzewo
		{ treeLevel=LONG_MAX; } //maximum value for long
		
	retVal=bmdStringArray_Create(&idsFromLevel);
	if(retVal < 0)
	{
		free(visibilityCondition);
		bmdStringArray_DestroyList(&tempIdsArray);
		return -23;
	}
	retVal=bmdStringArray_AddItem(idsFromLevel, rootFileId);
	if(retVal < 0)
	{
		free(visibilityCondition);
		bmdStringArray_DestroyList(&tempIdsArray);
		bmdStringArray_DestroyList(&idsFromLevel);
		return -24;
	}

	retVal=bmdStringArray_Create(&grantsFromLevel);
	if(retVal < 0)
	{
		free(visibilityCondition);
		bmdStringArray_DestroyList(&tempIdsArray);
		bmdStringArray_DestroyList(&idsFromLevel);
		return -25;
	}

	retVal=bmdStringArray_AddItem(grantsFromLevel, rootFileForGrant);
	if(retVal < 0)
	{
		free(visibilityCondition);
		bmdStringArray_DestroyList(&tempIdsArray);
		bmdStringArray_DestroyList(&idsFromLevel);
		bmdStringArray_DestroyList(&grantsFromLevel);
		return -26;
	}

	for(levelNumber=0; levelNumber<treeLevel; levelNumber++)
	{
		retVal=bmdStringArray_GetItemsCount(idsFromLevel, &itemsCount);
		if(retVal < 0)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			return -27;
		}
		if(itemsCount == 0) //jesli nie ma dokumentow powiazanych na tym poziomie, to znaczy ze koniec drzewa dokumentow
			{ break; }

		retVal=GenerateINConditionList(idsFromLevel, &conditionInList);
		if(retVal < 0)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			return -28;
		}

		asprintf(&sqlQuery, queryTemplate, requesterIdentityId, conditionInList, locationId, visibilityCondition);
		free(conditionInList); conditionInList=NULL;

		if(sqlQuery == NULL)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			return -29;
		}

		retVal=bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
		free(sqlQuery); sqlQuery=NULL;
		
		if(retVal == BMD_ERR_NODATA) //nie ma dokumentow powiazanych na tym poziomie
		{
			//wstawienie identyfikatorow z przetwarzanego poziomu do tablicy wynikow
			retVal=bmdStringArray_GetItemsCount(idsFromLevel, &itemsCount);
			if(retVal < 0)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				return -30;
			}
	
			for(iter=0; iter<itemsCount; iter++)
			{
				retVal=bmdStringArray_GetItemPtr(idsFromLevel, iter, &idPtr);
				if(retVal < 0)
				{
					free(visibilityCondition);
					bmdStringArray_DestroyList(&tempIdsArray);
					bmdStringArray_DestroyList(&idsFromLevel);
					bmdStringArray_DestroyList(&grantsFromLevel);
					return -31;
				}

				retVal=bmdStringArray_GetItemPtr(grantsFromLevel, iter, &forGrantPtr);
				if(retVal < 0)
				{
					free(visibilityCondition);
					bmdStringArray_DestroyList(&tempIdsArray);
					bmdStringArray_DestroyList(&idsFromLevel);
					bmdStringArray_DestroyList(&grantsFromLevel);
					return -32;
				}
				
				if(strcmp(forGrantPtr, "1") == 0)
				{
					retVal=bmdStringArray_AddItem(tempIdsArray, idPtr);
					if(retVal < 0)
					{
						free(visibilityCondition);
						bmdStringArray_DestroyList(&tempIdsArray);
						bmdStringArray_DestroyList(&idsFromLevel);
						bmdStringArray_DestroyList(&grantsFromLevel);
						return -33;
					}
				}

				idPtr=NULL;
				forGrantPtr=NULL;
			}
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);

			break;
		}
		else if(retVal != BMD_OK)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			return -34;
		}

		//idziemy dalej, jesli sa jakies dokumenty powiazane
		retVal=bmdStringArray_Create(&idsFromNextLevel);
		if(retVal < 0)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			return -35;
		}

		retVal=bmdStringArray_Create(&grantsFromNextLevel);
		if(retVal < 0)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			bmdStringArray_DestroyList(&idsFromNextLevel);
			return -36;
		}

		for(iter=0; iter<rowsCount; iter++)
		{
			retVal=bmd_db_result_get_row(hDB, queryResult, iter, 0, &resultRow, FETCH_ABSOLUTE, &fetchedCounter);
			if(retVal < BMD_OK)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				bmdStringArray_DestroyList(&idsFromNextLevel);
				bmdStringArray_DestroyList(&grantsFromNextLevel);
				bmd_db_result_free(&queryResult);
				return -37;
			}

			retVal=bmdStringArray_AddItem(idsFromNextLevel, resultRow->colvals[0]);
			if(retVal < 0)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				bmdStringArray_DestroyList(&idsFromNextLevel);
				bmdStringArray_DestroyList(&grantsFromNextLevel);
				bmd_db_row_struct_free(&resultRow);
				bmd_db_result_free(&queryResult);
				return -38;
			}
			
			retVal=bmdStringArray_AddItem(grantsFromNextLevel, resultRow->colvals[1]);
			bmd_db_row_struct_free(&resultRow);
			if(retVal < 0)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				bmdStringArray_DestroyList(&idsFromNextLevel);
				bmdStringArray_DestroyList(&grantsFromNextLevel);
				bmd_db_result_free(&queryResult);
				return -39;
			}
		}
		bmd_db_result_free(&queryResult);

		//wstawienie identyfikatorow z przetwarzanego poziomu do tablicy wynikow
		retVal=bmdStringArray_GetItemsCount(idsFromLevel, &itemsCount);
		if(retVal < 0)
		{
			free(visibilityCondition);
			bmdStringArray_DestroyList(&tempIdsArray);
			bmdStringArray_DestroyList(&idsFromLevel);
			bmdStringArray_DestroyList(&grantsFromLevel);
			bmdStringArray_DestroyList(&idsFromNextLevel);
			bmdStringArray_DestroyList(&grantsFromNextLevel);
			return -40;
		}

		for(iter=0; iter<itemsCount; iter++)
		{
			retVal=bmdStringArray_GetItemPtr(idsFromLevel, iter, &idPtr);
			if(retVal < 0)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				bmdStringArray_DestroyList(&idsFromNextLevel);
				bmdStringArray_DestroyList(&grantsFromNextLevel);
				return -41;
			}

			retVal=bmdStringArray_GetItemPtr(grantsFromLevel, iter, &forGrantPtr);
			if(retVal < 0)
			{
				free(visibilityCondition);
				bmdStringArray_DestroyList(&tempIdsArray);
				bmdStringArray_DestroyList(&idsFromLevel);
				bmdStringArray_DestroyList(&grantsFromLevel);
				bmdStringArray_DestroyList(&idsFromNextLevel);
				bmdStringArray_DestroyList(&grantsFromNextLevel);
				return -42;
			}

			if(strcmp(forGrantPtr, "1") == 0)
			{
				retVal=bmdStringArray_AddItem(tempIdsArray, idPtr);
				if(retVal < 0)
				{
					free(visibilityCondition);
					bmdStringArray_DestroyList(&tempIdsArray);
					bmdStringArray_DestroyList(&idsFromLevel);
					bmdStringArray_DestroyList(&grantsFromLevel);
					bmdStringArray_DestroyList(&idsFromNextLevel);
					bmdStringArray_DestroyList(&grantsFromNextLevel);
					return -43;
				}
			}
			idPtr=NULL;
			forGrantPtr=NULL;
		}
		bmdStringArray_DestroyList(&idsFromLevel);
		bmdStringArray_DestroyList(&grantsFromLevel);

		//ustawienie identyfikatorow z kolejnego poziomu do przetwarzania 
		idsFromLevel=idsFromNextLevel;
		idsFromNextLevel=NULL;
		//ustawienie do przetwarzania praw udostepniania z kolejnego poziomu (odpowiadajacych identyfikatorom)
		grantsFromLevel=grantsFromNextLevel;
		grantsFromNextLevel=NULL;
	}

	free(visibilityCondition); visibilityCondition=NULL;
	bmdStringArray_DestroyList(&idsFromLevel);
	bmdStringArray_DestroyList(&grantsFromLevel);

	*correspondingIds=tempIdsArray;
	tempIdsArray=NULL;
	return 0;
}


long GetPointingIdForFileId(void *hDB, char *fileId, char** pointingId)
{
char* queryTemplate = "SELECT pointing_id FROM crypto_objects WHERE id='%s' AND location_id='%s';";
char* sqlQuery			= NULL;
char** answers			= NULL;
long answersCount		= 0;
long retVal			= 0;
long iter			= 0;


	if(hDB == NULL)
		{ return -1; }
	if(fileId == NULL)
		{ return -2; }
	if(strlen(fileId) <= 0)
		{ return -3; }
	if(pointingId == NULL)
		{ return -4; }
	if(*pointingId != NULL)
		{ return -5; }
	if(_GLOBAL_bmd_configuration == NULL)
		{ return -6; }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ return -7; }

	asprintf(&sqlQuery, queryTemplate, fileId, _GLOBAL_bmd_configuration->location_id);
	retVal=ExecuteSQLQueryWithAnswersKnownDBConnection(hDB, sqlQuery, &answersCount, &answers);
	free(sqlQuery); sqlQuery=NULL;
	if(retVal != BMD_OK)
		{ return -11; }
	if(answersCount != 1 || answers == NULL || answers[0] == NULL)
	{
		for(iter=0; iter<answersCount ; iter++)
			{ free(answers[iter]); }
		free(answers); answers=NULL;
		answersCount=0;
		return -12;
	}

	*pointingId=strdup(answers[0]);
	free(answers[0]);
	free(answers); answers=NULL;
	answersCount=0;

	if(*pointingId == NULL)
		{ return -13; }

	return 0;
}


/*
correspondingId - w przypadku tworzenia linka na wyjsciu otrzymamy correspoondingId skopiowane z dokumentu wskazywanego;
w innych przypadkach wartosc jest nieistotna (najlepiej wtedy przekazac NULL)
*/
long InsertLOBandTimestampIntoDatabase_cascadeLink(	void *hDB,
					bmdDatagram_t **bmdJSRequest,
					char **lobjId,
					char **correspondingId,
					u_int64_t *lob_size_for_db,
					server_request_data_t *req)
{
long retVal			= 0;
long i				= 0;

char* tempCorrespondingId	= NULL;
char *pointing_id		= NULL;

long addMetadataCount		= 0;
char **addMetadata		= NULL;
char **addMetadataOid		= NULL;


	PRINT_INFO("LIBBMDSQLINF Creating cascade link\n");

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if((*bmdJSRequest)->datagramType != BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS )
		{ return BMD_ERR_PARAM2; }
	if(correspondingId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(lob_size_for_db == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }


	BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, "link", bmdJSRequest));

	if ((*bmdJSRequest)->datagramStatus==BMD_OK ||
		(*bmdJSRequest)->datagramStatus==BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS)
	{
		BMD_FOK_CHG(bmd_db_export_blob(hDB, (*bmdJSRequest)->protocolData, lobjId),
				LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR);
	}
	

	for(i=0; i<(*bmdJSRequest)->no_of_sysMetaData; i++)
	{

		if (strcmp((*bmdJSRequest)->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)
		{
			(*bmdJSRequest)->sysMetaData[i]->AnyBuf[(*bmdJSRequest)->sysMetaData[i]->AnySize]='\0';
			asprintf(&pointing_id, "%s", (*bmdJSRequest)->sysMetaData[i]->AnyBuf);
			if (pointing_id==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if(pointing_id==NULL)
		{ BMD_FOK(LIBBMDSQL_INSERT_NO_POINTING_ID_IN_LINK_CREATION); }


	// sprawdzenie, czy plik wskazywany przez link istnieje i pobranie wartosci metadanych dodatkowych
	// dlatego bez autoryzacji, bo pointing_id jest juz wynikiem odpowiedniej autoryzacji
	retVal=VerifyPermissionsForCryptoObject(hDB, req, pointing_id,
				SKIP_AUTHORIZATION,
				NULL, NULL, NULL, &tempCorrespondingId, NULL,
				&addMetadataCount, &addMetadata);
	free(pointing_id); pointing_id=NULL;
	BMD_FOK(retVal);


	/***********************************************/
	/* pobranie listy oidow metadanych dodatkowych */
	/***********************************************/
	//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
	retVal=getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataCount, &addMetadataOid);
	if(retVal < BMD_OK)
	{
		free(tempCorrespondingId);
		for(i=0; i<addMetadataCount; i++)
			{ free(addMetadata[i]); }
		free(addMetadata);
		BMD_FOK(retVal);
	}

	/**********************************************/
	/* usuniecie metadanych dodatkowych z zadania */
	/**********************************************/
	for (i=0; i<(*bmdJSRequest)->no_of_additionalMetaData; i++)
	{
		if((*bmdJSRequest)->additionalMetaData[i])
		{
			if ((*bmdJSRequest)->additionalMetaData[i]->OIDTableBuf)
			{
				free0((*bmdJSRequest)->additionalMetaData[i]->OIDTableBuf);
			}
			if ((*bmdJSRequest)->additionalMetaData[i]->AnyBuf)
			{
				(*bmdJSRequest)->additionalMetaData[i]->AnyBuf=NULL;
				(*bmdJSRequest)->additionalMetaData[i]->AnySize=0;
			}
			free0((*bmdJSRequest)->additionalMetaData[i]);
		}
	}
	free0((*bmdJSRequest)->additionalMetaData);
	(*bmdJSRequest)->no_of_additionalMetaData=0;

	
	// przepisanie metadanych pliku wskazywanego do metadanych linka
	for (i=0; i<addMetadataCount; i++)
	{
		if (strlen(addMetadata[i])>0)
		{
			retVal=bmd_datagram_add_metadata_char(	addMetadataOid[i], addMetadata[i], bmdJSRequest);
			if(retVal < BMD_OK)
			{
				free(tempCorrespondingId);
				for(i=0; i<addMetadataCount; i++)
					{ free(addMetadata[i]); }
				free(addMetadata);
				BMD_FOK(retVal);
			}
		}
	}

	*correspondingId=tempCorrespondingId;
	tempCorrespondingId=NULL;

	*lob_size_for_db=(*bmdJSRequest)->protocolData->size;

	return (*bmdJSRequest)->datagramStatus;
}


long JS_bmdDatagram_to_CreateCascadeLinks(void *hDB,
				bmdDatagram_t* bmdJSRequest,
				bmdDatagramSet_t** bmdJSResponse,
				server_request_data_t* req)
{
long status			= 0;
char *lobjId			= NULL;
char *MaxId			= NULL;
char *correspondingId		= NULL; //na potrzeby tworzenia linka (jedyna metadata systemowa, ktora jest kopiowana)
u_int64_t lob_size		= 0;

long iter				= 0;
long jupiter				= 0;
bmdStringArray_t* correspondingIds	= NULL;
bmdStringArray_t* fileNames		= NULL;
char* rootFileId			= NULL;
long linksCount				= 0; //ile linkow do stworzenia
char* tempStringPtr				= NULL;
bmdDatagram_t* tempRespDtgPtr		= NULL;
char* visibility			= NULL;
bmdVisibility_e visibilityFlag		= 0;
GenBuf_t tempGenBuf;
char* rootFilePointingId		= NULL;
char* rootFileForGrant			= NULL;
char* maxCorrespondingFileLevelString	= NULL;
long maxCorrespondingFileLevel		= 0;
char* ownerIdentityId			= NULL;
char* ownerGroupId				= NULL;


	memset(&tempGenBuf, 0, sizeof(GenBuf_t));


	PRINT_NOTICE("LIBBMDSQLINF Begining database transaction\n");

	if(bmdJSRequest == NULL)
		{ return BMD_ERR_PARAM2; }

	if(req == NULL || req->ua == NULL || req->ua->identityId == NULL || req->ua->identityId->buf == NULL)
		{ return BMD_ERR_PARAM5; }
	
	//wydobycie id crypto_obiektu , dla ktorego i jego wszystkich dokumentow powiazanych maja zostac stworzone linki
	for(iter=0; iter<bmdJSRequest->no_of_sysMetaData; iter++)
	{
		if(bmdJSRequest->sysMetaData[iter]->OIDTableBuf != NULL)
		{
			if (strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID) == 0)
			{
				if(bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
				{
					bmdJSRequest->sysMetaData[iter]->AnyBuf[bmdJSRequest->sysMetaData[iter]->AnySize]='\0';
					asprintf(&rootFileId, "%s", bmdJSRequest->sysMetaData[iter]->AnyBuf);
					if(rootFileId == NULL)
						{ BMD_FOK(NO_MEMORY); }
				}
			}
			else if (strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE) == 0)
			{
				if(bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
				{
					bmdJSRequest->sysMetaData[iter]->AnyBuf[bmdJSRequest->sysMetaData[iter]->AnySize]='\0';
					asprintf(&visibility, "%s", bmdJSRequest->sysMetaData[iter]->AnyBuf);
					if(visibility == NULL)
						{ BMD_FOK(NO_MEMORY); }
				}
			}
			else if (strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CORESPONDING_FILE_LEVEL) == 0)
			{
				if(bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
				{
					bmdJSRequest->sysMetaData[iter]->AnyBuf[bmdJSRequest->sysMetaData[iter]->AnySize]='\0';
					asprintf(&maxCorrespondingFileLevelString, "%s", bmdJSRequest->sysMetaData[iter]->AnyBuf);
					if(maxCorrespondingFileLevelString == NULL)
						{ BMD_FOK(NO_MEMORY); }
				}
			}
		}
	}

	if(rootFileId == NULL)
	{
		free(visibility);
		free(maxCorrespondingFileLevelString);
		return BMD_ERR_PARAM2;
	}
	if(visibility == NULL)
		{ visibilityFlag = ALL; }
	else if(strcmp(visibility, "0") == 0)
		{ visibilityFlag = INVISIBLE; }
	else
		{ visibilityFlag = VISIBLE; }
	free(visibility); visibility=NULL;

	if(maxCorrespondingFileLevelString != NULL)
	{
		maxCorrespondingFileLevel=strtol(maxCorrespondingFileLevelString, NULL, 10);
	}
	else
		{ maxCorrespondingFileLevel=-1; }
	
	free(maxCorrespondingFileLevelString); maxCorrespondingFileLevelString=NULL;
	PRINT_INFO("Maximum corresponding files level is set to %li", maxCorrespondingFileLevel);


	/* -- sprawdzenie, czy uzytkownik ma uprawnienie (widzi) obiekt dostepowy -- */
	status=VerifyPermissionsForCryptoObject(hDB, req, rootFileId, CONSIDER_ONLY_OWNER,
				NULL, NULL, &rootFilePointingId, NULL, &rootFileForGrant, NULL, NULL);
	//status=GetPointingIdForFileId(hDB, rootFileId, &rootFilePointingId);
	free(rootFileId); rootFileId=NULL;
	if(status < 0)
		{ BMD_FOK(status); }
	
	status=GetCorrespondingIds(hDB, _GLOBAL_bmd_configuration->location_id, req->ua->identityId->buf, rootFilePointingId, rootFileForGrant, maxCorrespondingFileLevel, visibilityFlag, &correspondingIds);
	free(rootFileForGrant); rootFileForGrant=NULL;
	if(status < 0)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in getting identifiers of corresponding documents (%li)\n", status);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	status=bmdStringArray_GetItemsCount(correspondingIds, &linksCount);
	if(status < 0)
	{
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if(linksCount == 0)
	{
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(LIBBMDSQL_NO_CASCADE_LINK_CREATED);
	}

	status=GetOwnerFromRequestDatagram(hDB, bmdJSRequest, req->ua->identityId->buf, &ownerIdentityId, &ownerGroupId);
	if(status < BMD_OK)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in getting information from request (%li)\n", status);
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	status=IgnoreDuplicateLinks(hDB, _GLOBAL_bmd_configuration->location_id, req->ua->identityId->buf, ownerIdentityId, ownerGroupId, &correspondingIds);
	free(ownerIdentityId); ownerIdentityId=NULL;
	free(ownerGroupId); ownerGroupId=NULL;
	if(status < 0)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in ignoring duplicated links (%li)\n", status);
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	status=bmdStringArray_GetItemsCount(correspondingIds, &linksCount);
	if(status < 0)
	{
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if(linksCount == 0)
	{
		PRINT_NOTICE("LIBBMDSQL No links left to create after ignoring duplicates\n");
		bmdStringArray_DestroyList(&correspondingIds);
		BMD_FOK(LIBBMDSQL_NO_CASCADE_LINK_CREATED);
	}
	
	
	status=GetFileNamesForIds(hDB, &correspondingIds, &fileNames);
	if(status < 0)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in getting filenames for documents' identifiers (%li)\n", status);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	

	/************************************************/
	/*	przygotowanie datagramu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(linksCount, bmdJSResponse));


	for(iter=0; iter<linksCount; iter++)
	{
		status=bmdStringArray_GetItemPtr(fileNames, iter, &tempStringPtr);
		if(status < 0)
			{ BMD_FOK(BMD_ERR_OP_FAILED); }

		free_gen_buf(&(bmdJSRequest->protocolDataFilename));
		set_gen_buf2(tempStringPtr, (long)strlen(tempStringPtr), &(bmdJSRequest->protocolDataFilename));
		tempStringPtr=NULL;

		tempRespDtgPtr=(*bmdJSResponse)->bmdDatagramSetTable[iter];

		status=bmdStringArray_GetItem(correspondingIds, iter, &(tempGenBuf.buf));
		if(status < 0)
		{
			bmdStringArray_DestroyList(&correspondingIds);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		tempGenBuf.size=(long)strlen(tempGenBuf.buf);

		//podmienienie pointing_id
		for(jupiter=0; jupiter<bmdJSRequest->no_of_sysMetaData; jupiter++)
		{
			if(bmdJSRequest->sysMetaData[jupiter]->OIDTableBuf != NULL)
			{
				if (strcmp(bmdJSRequest->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID) == 0)
				{
					free(bmdJSRequest->sysMetaData[jupiter]->AnyBuf);
					bmdJSRequest->sysMetaData[jupiter]->AnyBuf=strdup(tempGenBuf.buf);
					if(bmdJSRequest->sysMetaData[jupiter] == NULL)
					{
						bmdStringArray_DestroyList(&correspondingIds);
						BMD_FOK(BMD_ERR_MEMORY);
					}
				}
			}
		}
		


		/****************************************************************************************
		* 1. Na poczatek wstawiamy large object z pola protocol data. W zamian otrzymujemy 	*
		* Oid large objecta - lobjId, ktory posluzy do dalszej pracy na tabelach		*
		****************************************************************************************/
		status=InsertLOBandTimestampIntoDatabase_cascadeLink(	hDB,
								&bmdJSRequest,
								&lobjId,
								&correspondingId,
								&lob_size,
								req);
		/*********************************************************************/
		/* jesli wczesniej nie wykryto bledu w datagramie zadania	     */
		/* dopiero tutaj, poniewaz konieczne jest odebranie lobow od klienta,*/
		/* zeby mozna bylo mu odeslac blad w datagramie odpowiedzi - w tym   */
		/* wypadku nie zapisujemy odebranych danych w bazie danych	     */
		/*********************************************************************/
		if ( status<BMD_OK )	{	BMD_FOK(status);	}
	
	
		/********************************************************************************************
		* 2. pobierz id nowododanego elementu. Jest to ID obiektu do ktorego dalej dodawac bedziemy *
		* kolejne metadane 									    *
		********************************************************************************************/
		//BMD_FOK(bmd_db_get_max_entry(hDB, "id", "crypto_objects", &MaxId));
		void *queryResult = NULL;
		long fetched_counter=0;
		BMD_FOK(bmd_db_execute_sql(hDB, "SELECT nextval('crypto_objects_id_seq')", NULL, NULL, &queryResult));
		BMD_FOK(bmd_db_result_get_value(hDB, queryResult, 0, 0, &MaxId, FETCH_ABSOLUTE, &fetched_counter));
		bmd_db_result_free(&queryResult);

		/****************************************************************************************
		* 3. wstawienie wiersza crypto_objects wraz z metadanymi systemowymi
		****************************************************************************************/
		BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_CryptoObjects(hDB, bmdJSRequest, MaxId, correspondingId, lobjId, lob_size, NULL/*hash_as_hex_string*/, req));
		free(correspondingId); correspondingId=NULL;

		/********************************************************************************************
		* 5. Wstaw metadane dodatkowe								    *
		********************************************************************************************/
		status = JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(hDB, bmdJSRequest, MaxId,req);
		if(status<BMD_OK && status != ERR_JS_NO_ADDITIONAL_METADATA_PRESENT)
		{
			BMD_FOK(status);
		}


	
		/******************************************************************************************/
		/*	ustawienie wlasciciela datagramu odpowiedzi na wlasciciela datagramu wejsciowego	*/
		/******************************************************************************************/
		if (bmdJSRequest->protocolDataOwner!=NULL)
		{
			if (bmdJSRequest->protocolDataOwner->buf!=NULL)
			{
				if (tempRespDtgPtr->protocolDataOwner==NULL)
				{
					tempRespDtgPtr->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
					tempRespDtgPtr->protocolDataOwner->buf=(char*)malloc(bmdJSRequest->protocolDataOwner->size+2);
					memset(tempRespDtgPtr->protocolDataOwner->buf, 0, bmdJSRequest->protocolDataOwner->size+1);
					memcpy(tempRespDtgPtr->protocolDataOwner->buf, bmdJSRequest->protocolDataOwner->buf,bmdJSRequest->protocolDataOwner->size);
					tempRespDtgPtr->protocolDataOwner->size=bmdJSRequest->protocolDataOwner->size;
				}
			}
		}
	
		/***  ustawienie id stworzonego dowiazania  ***/
		free_gen_buf(&(tempRespDtgPtr->protocolDataFileId));
		set_gen_buf2(MaxId, (long)strlen(MaxId), &(tempRespDtgPtr->protocolDataFileId));
		
		/*** ustawienie identyfikatora pliku, do ktorego zostalo stworzone dowiazanie ***/
		status=bmd_datagram_add_metadata_2(OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID,&tempGenBuf, &tempRespDtgPtr, 0);
		free(tempGenBuf.buf); tempGenBuf.buf=NULL; tempGenBuf.size=0;

		free0(lobjId);
		free0(MaxId);
	}

	bmdStringArray_DestroyList(&correspondingIds);
	bmdStringArray_DestroyList(&fileNames);

	return BMD_OK;
}


/************************************************/
/*		Creating cascade links		*/
/************************************************/
long JS_bmdDatagramSet_to_CreateCascadeLinks(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long iter		= 0;
long status		= 0;
long error_occured	= 0;


	if(bmdJSRequestSet == NULL || bmdJSRequestSet->bmdDatagramSetSize != 1)
		{ BMD_FOK(BMD_ERR_PARAM1); } 


	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));


	status=JS_bmdDatagram_to_CreateCascadeLinks(	hDB,
							bmdJSRequestSet->bmdDatagramSetTable[0],
							bmdJSResponseSet,
							req);


	/*** przygotowanie odpowiedzi ***/
	if(status != BMD_OK)
	{
		if(*bmdJSResponseSet == NULL)
			{ PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet); }

		if( (*bmdJSResponseSet)->bmdDatagramSetSize > 1 )
		{
			for(iter=(*bmdJSResponseSet)->bmdDatagramSetSize-1; iter>0; iter--)
			{
				bmd_datagram_free(&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]));
			}
			(*bmdJSResponseSet)->bmdDatagramSetSize=1;
		}
		(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus= status;
		(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType=BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS_RESPONSE;
	}

	/*** zarejestrowanie akcji w bazie danych ***/
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		for(iter=0; iter<(*bmdJSResponseSet)->bmdDatagramSetSize; iter++)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus=status;
				break;
			}
		}
	}

	for(iter=0; iter<(*bmdJSResponseSet)->bmdDatagramSetSize; iter++)
	{
		if ((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus<BMD_OK)
		{
			status=(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus;
			error_occured=1;
			break;
		}
	}
	
	if(error_occured == 1)
	{
		if( (*bmdJSResponseSet)->bmdDatagramSetSize > 1 )
		{
			for(iter=(*bmdJSResponseSet)->bmdDatagramSetSize-1; iter>0; iter--)
			{
				bmd_datagram_free(&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]));
			}
			(*bmdJSResponseSet)->bmdDatagramSetSize=1;
		}
		(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus= status;
		(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType=BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS_RESPONSE;

		bmd_db_rollback_transaction(hDB,NULL);
	}
	else
	{
		status=bmd_db_end_transaction(hDB);
		if( status < BMD_OK)
		{
			if( (*bmdJSResponseSet)->bmdDatagramSetSize > 1 )
			{
				for(iter=(*bmdJSResponseSet)->bmdDatagramSetSize-1; iter>0; iter--)
				{
					bmd_datagram_free(&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]));
				}
				(*bmdJSResponseSet)->bmdDatagramSetSize=1;
			}
			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus= status;
			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType=BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS_RESPONSE;
		}
		else
		{
			for(iter=0; iter<(*bmdJSResponseSet)->bmdDatagramSetSize; iter++)
			{
				Q_DONE("Created link with id=%s\n", (*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataFileId->buf);
			}
		}
	}

	return BMD_OK;
}



long JS_bmdDatagram_to_DeleteCascadeLinks(void *hDB,
				bmdDatagram_t* requestDatagram,
				bmdDatagram_t* responseDatagram,
				server_request_data_t* req)
{
long retVal				= 0;
long iter				= 0;

bmdStringArray_t* correspondingIds	= NULL;
bmdStringArray_t* linksIds		= NULL;
bmdStringArray_t* symkeysArray		= NULL;
bmdStringArray_t* lobsArray		= NULL;

char* rootFileId			= NULL;
char* rootFileForGrant			= NULL;
char* ownerIdentityId			= NULL;
char* ownerGroupId				= NULL;
long linksCount				= 0;
char* idPtr				= NULL;
char* symkeyPtr				= NULL;
char* lobPtr				= NULL;

char* visibility			= NULL;
bmdVisibility_e visibilityFlag		= 0;

char* rootFilePointingId		= NULL;
char* maxCorrespondingFileLevelString	= NULL;
long maxCorrespondingFileLevel		= 0;

char* sqlDeleteTemplate	= "DELETE FROM crypto_objects WHERE id=%s AND location_id=%s;";
char* sqlDelete		= NULL;
void* deleteResult	= NULL;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram->datagramType != BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(responseDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL || req->ua == NULL || req->ua->identityId == NULL || req->ua->identityId->buf == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	// sprawdzenie, czy podany jest identyfikator pliku (korzen drzewa plikow, dla ktorych maja byc usuniete linki)
	if(requestDatagram->protocolDataFileId == NULL || requestDatagram->protocolDataFileId->size<=0 || requestDatagram->protocolDataFileId->buf == NULL)
		{ BMD_FOK(BMD_ERR_FILE_ID_NOT_SPECIFIED); }

	//wydobycie id crypto_obiektu , dla ktorego i jego wszystkich dokumentow powiazanych maja zostac usuniete linki danego wlasciciela
	for(iter=0; iter<requestDatagram->no_of_sysMetaData; iter++)
	{
		if(requestDatagram->sysMetaData[iter]->OIDTableBuf != NULL)
		{
			if(visibility == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE) == 0)
			{
				if(requestDatagram->sysMetaData[iter]->AnyBuf != NULL)
				{
					requestDatagram->sysMetaData[iter]->AnyBuf[requestDatagram->sysMetaData[iter]->AnySize]='\0';
					asprintf(&visibility, "%s", requestDatagram->sysMetaData[iter]->AnyBuf);
					if(visibility == NULL)
					{
						free(ownerGroupId);
						free(maxCorrespondingFileLevelString);
						free(ownerIdentityId);
						BMD_FOK(NO_MEMORY);
					}
				}
			}
			else if(maxCorrespondingFileLevelString == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CORESPONDING_FILE_LEVEL) == 0)
			{
				if(requestDatagram->sysMetaData[iter]->AnyBuf != NULL)
				{
					requestDatagram->sysMetaData[iter]->AnyBuf[requestDatagram->sysMetaData[iter]->AnySize]='\0';
					asprintf(&maxCorrespondingFileLevelString, "%s", requestDatagram->sysMetaData[iter]->AnyBuf);
					if(maxCorrespondingFileLevelString == NULL)
					{
						free(ownerGroupId);
						free(visibility);
						free(ownerIdentityId);
						BMD_FOK(NO_MEMORY);
					}
				}
			}
			else if(ownerIdentityId == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY)==0 )
			{
				if(requestDatagram->sysMetaData[iter]->AnyBuf != NULL)
				{
					requestDatagram->sysMetaData[iter]->AnyBuf[requestDatagram->sysMetaData[iter]->AnySize]='\0';
					ownerIdentityId=STRNDUP(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
					if(ownerIdentityId == NULL)
					{
						free(ownerGroupId);
						free(visibility);
						free(maxCorrespondingFileLevelString);
						BMD_FOK(NO_MEMORY);
					}
				}
			}
			else if(ownerIdentityId == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ID)==0)
			{
				if(requestDatagram->sysMetaData[iter]->AnyBuf != NULL)
				{
					requestDatagram->sysMetaData[iter]->AnyBuf[requestDatagram->sysMetaData[iter]->AnySize]='\0';
					retVal=id2identify(hDB, (char *)requestDatagram->sysMetaData[iter]->AnyBuf, &ownerIdentityId);
					if(retVal < BMD_OK)
					{
						free(ownerGroupId);
						free(visibility);
						free(maxCorrespondingFileLevelString);
						BMD_FOK(retVal);
					}
				}
			}
			else if(ownerGroupId == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP)==0 )
			{
				if(requestDatagram->sysMetaData[iter]->AnyBuf != NULL)
				{
					requestDatagram->sysMetaData[iter]->AnyBuf[requestDatagram->sysMetaData[iter]->AnySize]='\0';
					ownerGroupId=STRNDUP(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
					if(ownerGroupId == NULL)
					{
						free(visibility);
						free(maxCorrespondingFileLevelString);
						free(ownerIdentityId);
						BMD_FOK(NO_MEMORY);
					}
				}
			}
		}
	}

	//jesli brakuje w zadaniu informacji o tym, czyje linki maja zostac usuniete
	if(ownerIdentityId == NULL)
	{
		free(ownerGroupId);
		free(visibility);
		free(maxCorrespondingFileLevelString);
		BMD_FOK(BMD_ERR_FILE_OWNER_NOT_SPECIFIED);
	}
	
	retVal=CheckIfIdentityExists(hDB, ownerIdentityId);
	if(retVal < BMD_OK)
	{
		free(ownerGroupId);
		free(visibility);
		free(maxCorrespondingFileLevelString);
		free(ownerIdentityId);
		BMD_FOK(retVal);
	}

	// ownerGroupId moze byc NULL (przy przekazywaniu do funkcji GetLinksForIds() ) 
	if(ownerGroupId != NULL)
	{
		retVal=CheckIfGroupExist(ownerGroupId, NULL);
		if(retVal < BMD_OK)
		{
			free(ownerGroupId);
			free(visibility);
			free(maxCorrespondingFileLevelString);
			free(ownerIdentityId);
			BMD_FOK(retVal);
		}
	}
	
	rootFileId = STRNDUP(requestDatagram->protocolDataFileId->buf, requestDatagram->protocolDataFileId->size);
	if(rootFileId == NULL)
	{
		free(ownerGroupId);
		free(visibility);
		free(maxCorrespondingFileLevelString);
		free(ownerIdentityId);
		BMD_FOK(NO_MEMORY);
	}

	if(visibility == NULL)
		{ visibilityFlag = ALL; }
	else if(strcmp(visibility, "0") == 0)
		{ visibilityFlag = INVISIBLE; }
	else
		{ visibilityFlag = VISIBLE; }
	free(visibility); visibility=NULL;

	if(maxCorrespondingFileLevelString != NULL)
	{
		maxCorrespondingFileLevel=strtol(maxCorrespondingFileLevelString, NULL, 10);
		if(maxCorrespondingFileLevel == LONG_MIN || maxCorrespondingFileLevel == LONG_MAX)
			{ maxCorrespondingFileLevel=-1; }
	}
	else
		{ maxCorrespondingFileLevel=-1; }
	
	free(maxCorrespondingFileLevelString); maxCorrespondingFileLevelString=NULL;
	PRINT_INFO("Maximum corresponding files level is set to %li", maxCorrespondingFileLevel);


	// sprawdzenie, czy uzytkownik ma uprawnienie (widzi) obiekt dostepowy
	retVal=VerifyPermissionsForCryptoObject(hDB, req, rootFileId, CONSIDER_ONLY_OWNER,
				NULL, NULL, &rootFilePointingId, NULL, &rootFileForGrant, NULL, NULL);
	free(rootFileId); rootFileId=NULL;
	if(retVal < 0)
	{
		free(ownerGroupId);
		free(ownerIdentityId);
		BMD_FOK(retVal);
	}
	
	retVal=GetCorrespondingIds(hDB, _GLOBAL_bmd_configuration->location_id, req->ua->identityId->buf, rootFilePointingId, rootFileForGrant, maxCorrespondingFileLevel, visibilityFlag, &correspondingIds);
	free(rootFilePointingId); rootFilePointingId=NULL;
	free(rootFileForGrant); rootFileForGrant=NULL;
	if(retVal < 0)
	{
		free(ownerGroupId);
		free(ownerIdentityId);
		PRINT_ERROR("Error in getting identifiers of corresponding documents (%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	retVal=bmdStringArray_GetItemsCount(correspondingIds, &linksCount);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&correspondingIds);
		free(ownerGroupId);
		free(ownerIdentityId);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if(linksCount == 0)
	{
		bmdStringArray_DestroyList(&correspondingIds);
		free(ownerGroupId);
		free(ownerIdentityId);
		return BMD_OK;
	}

	retVal=GetLinksForIds(hDB, _GLOBAL_bmd_configuration->location_id, correspondingIds, req->ua->identityId->buf, ownerIdentityId, ownerGroupId, visibilityFlag, &linksIds, &symkeysArray, &lobsArray);
	bmdStringArray_DestroyList(&correspondingIds);
	free(ownerGroupId); ownerGroupId=NULL;
	free(ownerIdentityId); ownerIdentityId=NULL;
	if(retVal < 0)
	{
		PRINT_ERROR("Error occured during searching for links (%li)\n.", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	retVal=bmdStringArray_GetItemsCount(linksIds, &linksCount);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&linksIds);
		bmdStringArray_DestroyList(&symkeysArray);
		bmdStringArray_DestroyList(&lobsArray);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if(linksCount == 0)
	{
		bmdStringArray_DestroyList(&linksIds);
		bmdStringArray_DestroyList(&symkeysArray);
		bmdStringArray_DestroyList(&lobsArray);
		return BMD_OK; //jesli nie ma linkow spelniajacych nalozone kryteria, to traktowane jako poprawne wykonie
	}


	for(iter=0; iter<linksCount; iter++)
	{
		idPtr=NULL;
		retVal=bmdStringArray_GetItemPtr(linksIds, iter, &idPtr);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get item from link identifiers list (%li).\n", retVal);
			continue;
		}
		
		symkeyPtr=NULL;
		retVal=bmdStringArray_GetItemPtr(symkeysArray, iter, &symkeyPtr);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get item from symkeys list (%li).\n", retVal);
			continue;
		}

		lobPtr=NULL;
		retVal=bmdStringArray_GetItemPtr(lobsArray, iter, &lobPtr);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get item from lobs list (%li).\n", retVal);
			continue;
		}
		
		asprintf(&sqlDelete, sqlDeleteTemplate, idPtr, _GLOBAL_bmd_configuration->location_id);
		if(sqlDelete == NULL)
		{
			PRINT_ERROR("Unable to generate sql command.\n");
			continue;
		}

		retVal=bmd_db_start_transaction(hDB, BMD_TRANSACTION_LEVEL_NONE);
		if(retVal < BMD_OK)
		{
			free(sqlDelete); sqlDelete=NULL;
			PRINT_ERROR("Unable to start database transaction (%li).\n", retVal);
			continue;
		}
		
		retVal=bmd_db_execute_sql(hDB, sqlDelete, NULL, NULL, &deleteResult);
		free(sqlDelete); sqlDelete=NULL;
		bmd_db_result_free(&deleteResult);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("Unable to execute sql query (%li).\n", retVal);
			bmd_db_rollback_transaction(hDB, NULL);
			continue;
		}
		
		retVal=bmd_db_delete_blob(hDB, symkeyPtr);
		symkeyPtr=NULL;
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("Unable to delete symkey (%li).\n", retVal);
			bmd_db_rollback_transaction(hDB, NULL);
			continue;
		}

		retVal=bmd_db_delete_blob(hDB, lobPtr);
		lobPtr=NULL;
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("Unable to delete lob (%li).\n", retVal);
			bmd_db_rollback_transaction(hDB, NULL);
			continue;
		}

		// usuniecie wpisow action_register zwiazanych z usuwanym udostepnieniem (tak jak w przypadku obslugi zadania delete)
		if (_GLOBAL_bmd_configuration->enable_delete_history==0)
		{
			if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_delete_action_register)
			{
				retVal=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_delete_action_register(hDB, idPtr, _GLOBAL_bmd_configuration->location_id);
				if(retVal < BMD_OK)
				{
					PRINT_ERROR("Unable to delete from action register (%li).\n", retVal);
					bmd_db_rollback_transaction(hDB, NULL);
					continue;
				}
			}
		}

		// ustawienie id usunietego linku
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_ID, idPtr, &responseDatagram);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("Unable to set deleted link id in response (%li).\n", retVal);
			bmd_db_rollback_transaction(hDB, NULL);
			continue;
		}

		retVal=bmd_db_end_transaction(hDB);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("Unable to finish database transaction (%li).\n", retVal);
			bmd_db_rollback_transaction(hDB, NULL);
			continue;
		}

		idPtr=NULL;
	}

	bmdStringArray_DestroyList(&linksIds);
	bmdStringArray_DestroyList(&symkeysArray);
	bmdStringArray_DestroyList(&lobsArray);

	return BMD_OK;
}


long JS_bmdDatagramSet_to_DeleteCascadeLinks(void *hDB, bmdDatagramSet_t *bmdJSRequestSet, bmdDatagramSet_t **bmdJSResponseSet, server_request_data_t *req)
{
long iter		= 0;
long retVal		= 0;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequestSet->bmdDatagramSetSize <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*bmdJSResponseSet != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }


	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));


	for(iter=0; iter<bmdJSRequestSet->bmdDatagramSetSize; iter++)
	{
		// przekopiowanie id z datagramu zadania do datagramu odpowiedzi
		free_gen_buf(&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataFileId));
		set_gen_buf2(	bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->buf,
				bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->size,
				&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataFileId));

		// przekopiowanie informacji o autorze zadania do odpowiedzi
		if(bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner != NULL)
		{
			if(bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner->buf != NULL)
			{
				if((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner==NULL)
				{
					(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
					if((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner != NULL)
					{
						(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner->buf=(char*)calloc( (bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner->size)+1, sizeof(char));
						if((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner->buf != NULL)
						{
							memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner->buf,
								bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner->buf,
								bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner->size);
							(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataOwner->size=bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataOwner->size;
						}
					}
				}
			}
		}

		retVal=JS_bmdDatagram_to_DeleteCascadeLinks(hDB, bmdJSRequestSet->bmdDatagramSetTable[iter], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus=retVal;
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramType=BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS_RESPONSE;
	}


	return BMD_OK;
}
