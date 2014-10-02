#ifndef WIN32
#ifdef WITH_POSTGRES
#include <libpq-fe.h>
#include <pcre.h>
#include <dlfcn.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

#define LIBPQ_PATH "libpq.so"

#define CAST_FUNC(handler) *(void**)(&handler)

/*standardowa wersja strdup nie lubi NULL na wejsciu i konczy sie SEGV*/
char* bmdconf_strdup(char* str)
{
	if(str==NULL)
		{ return NULL; }
	else
		{ return strdup(str); }
}

/*param_err jest wskaznikiem !!!*/
#define CHECK_DL_ERROR(param_lib, param_err) \
{ \
	param_err=bmdconf_strdup(dlerror()); \
	if(param_err != NULL) \
	{ \
		asprintf(&param_err, "ERROR: %s\n", param_err); \
		dlclose(param_lib); \
		return param_err; \
	} \
}

/*param_err jest wskaznikiem na wskaznik !!!*/
#define CHECK_DL_ERROR_INT(param_lib, param_err, param_tmp) \
{ \
	param_tmp=bmdconf_strdup(dlerror()); \
	if(param_tmp != NULL) \
	{ \
		asprintf(param_err, "ERROR: %s\n", param_tmp); \
		free(param_tmp);\
		dlclose(param_lib); \
		return -1; \
	} \
}


char* bmdconf_read_and_parse_config_db(char *conn_params, bmdconf_dump **dumped)
{
	
	/*do sprawdzania poprawnosci formatu*/
	const char *error_pointer=NULL;
	int error_offset=0;
	pcre *regex=NULL;
	char *wzorzec="^[_a-zA-Z][_0-9a-zA-Z]*$"; /*ten sam dla nazwy sekcji jak i opcji*/
	/*wartosci opcji nie trzeba sprawdzac*/
	int subpatterns[4];

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;
	/**/
	
	PGconn *connection=NULL;
	PGresult *result_sections=NULL, *result_options=NULL;
	long ile_sekcji=0, ile_opcji=0;
	long iter=0, iter_opt=0;
	char *sekcja=NULL, *opcja=NULL, *wartosc=NULL;
	char *tmp=NULL;
	char *sekcja_id=NULL, *opcja_id=NULL;
	char *query=NULL;
	struct bmdconf_lista *opcje_list=NULL;
	char *dl_err=NULL;

	
	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(&dl_err, "ERROR: %s\n", dlerror());
		return dl_err;
	}

	/*zczytywanie symboli*/
	/*PQconnectdb=(PGconn* (*)(char*))dlsym(lib, "PQconnectdb");*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR(lib, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");
	CHECK_DL_ERROR(lib, dl_err)


	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		PQfinish(connection);
		bmdconf_delete_list(dumped);
		dlclose(lib);
		return strdup("ERROR: Nie mozna polaczyc z baza.\n");
	}
	

	/*kompilacja wzorca*/
	regex=pcre_compile(wzorzec, PCRE_UTF8, &error_pointer, &error_offset, NULL);

	result_sections=PQexec(connection, "SELECT * FROM sections");
	if(PQresultStatus(result_sections) != PGRES_TUPLES_OK)
	{
		bmdconf_delete_list(dumped);
		/*printf("STATUS sekcja %s\n",PQresStatus( PQresultStatus(result_sections) ));*/
		pcre_free(regex);
		PQclear(result_sections);
		PQfinish(connection);
		dlclose(lib);
		return strdup("ERROR: Brak oczekiwanych danych w odpowiedzi.\n");
	}

	ile_sekcji=PQntuples(result_sections);
	if(ile_sekcji==0)
	{
		bmdconf_delete_list(dumped);
		pcre_free(regex);
		PQclear(result_sections);
		PQfinish(connection);
		dlclose(lib);
		return strdup("ERROR: Brak definicji sekcji.\n");
	}

	for(iter=0; iter<ile_sekcji; iter++)
	{
		/*nie sprawdzam PQgetisnull bo baza wszystkie pola wymaga jako NOT NULL*/
		sekcja=PQgetvalue(result_sections, iter, 1);
		sekcja_id=PQgetvalue(result_sections, iter, 0);
		if(pcre_exec(regex,NULL,sekcja,strlen(sekcja),0,0,subpatterns,4) <= 0)
		{
			asprintf(&tmp,"ERROR: Tabela 'sections', wpis id_sec = '%s'; Nazwa sekcji '%s' niezgodna z wymaganym formatem.\n", sekcja_id, sekcja);
			bmdconf_delete_list(dumped);
			pcre_free(regex);
			PQclear(result_sections);
			PQfinish(connection);
			dlclose(lib);
			return tmp;
		}
	
		/*mergowanie  stringow*/
		query=bmdconf_strmerge("SELECT * FROM options WHERE id_sec=",sekcja_id);
		result_options=PQexec(connection, query);
		free(query); query=NULL;
		if(PQresultStatus(result_options) != PGRES_TUPLES_OK)
		{
			bmdconf_delete_list(dumped);
			/*printf("STATUS sekcja %s\n",PQresStatus( PQresultStatus(result_options) ));*/
			pcre_free(regex);
			PQclear(result_sections);
			PQfinish(connection);
			dlclose(lib);
			return strdup("ERROR: Brak oczekiwanych danych w odpowiedzi.\n");
		}

		ile_opcji=PQntuples(result_options);
		/*
		if(ile_opcji==0)
		{
			printf("WARNING: Brak opcji dla zdefiniowanej sekcji.\n");
		}
		*/

		/*przygotowuje liste opcji danej sekcji*/
		bmdconf_init_lista(&opcje_list);
		for(iter_opt=0; iter_opt<ile_opcji; iter_opt++)
		{
			opcja=PQgetvalue(result_options, iter_opt, 2);
			opcja_id=PQgetvalue(result_options, iter_opt, 0);
			/*jesli opcja zostala juz wczesnie napotkana dla tej sekcji*/
			if(bmdconf_znajdz_element(&opcje_list,opcja)==1)
			{
				asprintf(&tmp,"ERROR: Tabela 'options', wpis id = '%s'; Powtorzona opcja '%s' w sekcji '%s'.\n", opcja_id, opcja, sekcja);
				bmdconf_delete_list(dumped);
				bmdconf_niszcz_liste(&opcje_list);
				pcre_free(regex);
				PQclear(result_options);/*!!!!*/
				PQclear(result_sections);
				PQfinish(connection);
				dlclose(lib);
				return tmp;
			}
			bmdconf_dodaj_element(&opcje_list,opcja);
			wartosc=PQgetvalue(result_options, iter_opt, 3);

			/*jesli wartosc zawiera znak cudzyslowia*/
			if( contain_quotation(wartosc) == 0)
			{
				asprintf(&tmp,"ERROR: Tabela 'options', wpis id = '%s'; Wartosc opcji '%s' w sekcji '%s' zawiera niedozwolony znak cudzyslowia ( \" ).\n", opcja_id, opcja, sekcja);
				bmdconf_delete_list(dumped);
				bmdconf_niszcz_liste(&opcje_list);
				pcre_free(regex);
				PQclear(result_options);/*!!!!*/
				PQclear(result_sections);
				PQfinish(connection);
				dlclose(lib);
				return tmp;
			}
			
			if(pcre_exec(regex,NULL,opcja,strlen(opcja),0,0,subpatterns,4) <= 0)
			{
				asprintf(&tmp,"ERROR: Tabela 'options', wpis id = '%s'; Nazwa opcji '%s' niezgodna z wymaganym formatem.\n", opcja_id, opcja);
				bmdconf_delete_list(dumped);
				bmdconf_niszcz_liste(&opcje_list);
				pcre_free(regex);
				PQclear(result_options);/*!!!!*/
				PQclear(result_sections);
				PQfinish(connection);
				dlclose(lib);
				return tmp;
			}
			bmdconf_add_list_item(dumped, atoi(opcja_id), sekcja, opcja, wartosc);
			/*printf("sekcja %s: <%s>=<%s>\n", sekcja, opcja, wartosc);*/
		}
		bmdconf_niszcz_liste(&opcje_list);
		PQclear(result_options);
	}

	pcre_free(regex);
	PQclear(result_sections);
	PQfinish(connection);
	dlclose(lib);
	return NULL;
}




char* bmdconf_blad_niedozwolona_wartosc_db(long id_opcji,char *sekcja,char *pole, char *wartosc)
{
	char *result=NULL;
	
	asprintf(&result, "ERROR: Niedozwolona wartosc '%s' pola '%s' w sekcji '%s'; Indeks blednego wpisu w tabeli options: '%li'\n", wartosc, pole, sekcja, id_opcji);
	return result;
}

char* bmdconf_blad_nieznane_pole_db(long id_opcji, char *sekcja, char *pole)
{
	char *result=NULL;

	asprintf(&result, "ERROR: Nierozpoznane pole/sekcja; sekcja: '%s', pole: '%s'; Indeks wpisu w tabeli options: '%li'\n", sekcja, pole, id_opcji);
	return result;
}

char* bmdconf_blad_braku_wymaganego_pola_db(char *sekcja, char *pole)
{
	char *result=NULL;
	asprintf(&result, "ERROR: Brak wymaganego pola '%s' w sekcji '%s'.\n", pole, sekcja);

	return result;
}

char* bmdconf_blad_niepoprawnej_sciezki_db(long id_opcji, char *path)
{
	char *result=NULL;
	asprintf(&result, "ERROR: Brak pliku o podanej sciezce '%s'; Indeks wpisu w tabeli options: %li\n", path, id_opcji);

	return result;
}


char* bmdconf_validate_config_restrict_db(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary)
{
struct bmdconf_sekcja *slownik=*dictionary;
struct bmdconf_mem_list *konfiguracja=*dumped;
struct bmdconf_pole *wsk_pole;
struct bmdconf_wzorzec *iterator, *wzor_dict;
int subpatterns[30];
long wynik_porownania;
long ostatnia_linia_nr;
char *temp1;
char *temp2;
char *aktualna_sekcja=NULL;
struct bmdconf_lista *pola_req;
struct bmdconf_sekcja *temp_sekcja;
struct bmdconf_pole *iter_pole, *temp_pole;

/*@added*/struct bmdconf_sekcja *temp_dict=*dictionary;
/*@added*/struct bmdconf_lista *napotkane_sekcje=NULL; /*lista sekcji napotkanych w pliku*/
/*@added*/bmdconf_init_lista(&napotkane_sekcje);

	/**/
	if(konfiguracja==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("ERROR: Brak wpisow konfiguracyjnych.","\n");
		return temp1;
	}
	/*jesli pusty slownik */
	if(slownik==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("ERROR: Nie zaladowano zadnego slownika badz ladowany slownik byl pusty.","\n");
		return temp1;
	}

	while(konfiguracja != NULL)
	{
		ostatnia_linia_nr=konfiguracja->line_number;
		/*jesli kolejny wpis badany*/
		if(aktualna_sekcja!=NULL)
		{
			/*jesli nowo napotkana sekcja*/
			if( strcmp(konfiguracja->section,aktualna_sekcja) != 0)
			{
				/*----------*/
				temp_sekcja=bmdconf_znajdz_sekcje(&slownik,aktualna_sekcja);
				if(temp_sekcja!=NULL)
				{
					bmdconf_init_iterator_pole(&(temp_sekcja->field),&iter_pole);
					temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
					while(temp_pole!=NULL)
					{
						/*gdy pole wymagane i niezawarte w sekcji w pliku konfiguracyjnym*/
						if( temp_pole->pole_wymagane==1 && (bmdconf_znajdz_element(&pola_req,temp_pole->nazwa_pola))==0 )
						{
							/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
							bmdconf_niszcz_liste(&pola_req);
							return bmdconf_blad_braku_wymaganego_pola_db(aktualna_sekcja, temp_pole->nazwa_pola);
						}
						
						temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
					}
				}
				/*jesli w slowniku sekcji nie ma zdych wpisow, to nic sie nie dzeje*/

				/*---------*/
				bmdconf_niszcz_liste(&pola_req);
				free(aktualna_sekcja);
				aktualna_sekcja=(char*)malloc((strlen(konfiguracja->section)+1)*sizeof(char));
				strcpy(aktualna_sekcja,konfiguracja->section);
				/*@added*/bmdconf_dodaj_element(&napotkane_sekcje, aktualna_sekcja);
				bmdconf_init_lista(&pola_req);
				bmdconf_dodaj_element(&pola_req,konfiguracja->option);
			}
			/*jesli wpisy bedace kontynuacja sekcji*/
			else
				bmdconf_dodaj_element(&pola_req,konfiguracja->option);
		}
		/*jesli pierwszy wpis badany*/
		else
		{
			aktualna_sekcja=(char*)malloc((strlen(konfiguracja->section)+1)*sizeof(char));
			strcpy(aktualna_sekcja,konfiguracja->section);
			bmdconf_init_lista(&pola_req);
			bmdconf_dodaj_element(&pola_req,konfiguracja->option);
			/*@added*/bmdconf_dodaj_element(&napotkane_sekcje, aktualna_sekcja);
		}



		wsk_pole=bmdconf_znajdz_pole_sekcji_hash(&slownik,konfiguracja->section,konfiguracja->option);
		/*brak definicji pola w slowniku*/
		if(wsk_pole==NULL)
		{
			/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
			bmdconf_niszcz_liste(&pola_req);
			free(aktualna_sekcja);
			return bmdconf_blad_nieznane_pole_db(konfiguracja->line_number, konfiguracja->section, konfiguracja->option);
		}
		else
		{
			bmdconf_init_iterator_wzorzec(&(wsk_pole->wz),&iterator);
			wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
			while(wzor_dict!=NULL)
			{
				wynik_porownania=pcre_exec(wzor_dict->wzor,NULL,konfiguracja->value,strlen(konfiguracja->value),0,0,subpatterns,30);
				if(wzor_dict->zgodnosc==1)
				{
					/*w przypadku niedopoasowania, gdy wymagane dopasowanie, zwraca blad*/
					if(wynik_porownania<=0)
					{
						/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
						bmdconf_niszcz_liste(&pola_req);
						free(aktualna_sekcja);
						return bmdconf_blad_niedozwolona_wartosc_db(konfiguracja->line_number, konfiguracja->section, konfiguracja->option, konfiguracja->value);
					}
				}
				else
				{
					/*w przypadku dopasowania, gdy wymagane jest niedopasowanie, zwraca blad*/
					if(wynik_porownania>0)
					{
						/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
						bmdconf_niszcz_liste(&pola_req);
						free(aktualna_sekcja);
						return bmdconf_blad_niedozwolona_wartosc_db(konfiguracja->line_number, konfiguracja->section, konfiguracja->option, konfiguracja->value);
					}
				}
				wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
			}
			/*sprawdzenie,czy wyrazenie sciezkowe prowadzi do instniejacego pliku*/
			if(wsk_pole->flaga_plik==1)
			{
				/*jesli plik nie istnieje*/
				if(bmdconf_file_exists(konfiguracja->value)==0)
				{
					/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
					bmdconf_niszcz_liste(&pola_req);
					free(aktualna_sekcja);
					return bmdconf_blad_niepoprawnej_sciezki_db(konfiguracja->line_number,konfiguracja->value);
				}
			}
			
		}
		konfiguracja=konfiguracja->next;
		
	}

	/*dla konca pliku konfiguracyjnego sprawdzanie poprawnosci ostatniej sekcji*/
	temp_sekcja=bmdconf_znajdz_sekcje(&slownik,aktualna_sekcja);
	if(temp_sekcja!=NULL)
	{
		bmdconf_init_iterator_pole(&(temp_sekcja->field),&iter_pole);
		temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
		while(temp_pole!=NULL)
		{
			/*gdy pole wymagane i niezawarte w sekcji w pliku konfiguracyjnym*/
			if( temp_pole->pole_wymagane==1 && (bmdconf_znajdz_element(&pola_req,temp_pole->nazwa_pola))==0 )
			{
				/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
				bmdconf_niszcz_liste(&pola_req);
				return bmdconf_blad_braku_wymaganego_pola_db(aktualna_sekcja,temp_pole->nazwa_pola);
			}
			temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
		}
	}
	/*jesli w slowniku sekcji nie ma zadnych wpisow, to nic sie nie dzeje*/
	bmdconf_niszcz_liste(&pola_req);
	free(aktualna_sekcja);
	
	/*<@added>*/
	while(temp_dict != NULL)
	{
		/*jesli sekcja ze slownika NIE znaleziona w liscie napotkanych sekcji*/
		if( (bmdconf_znajdz_element(&napotkane_sekcje, temp_dict->nazwa_sekcji)) != 1 )
		{
			/*jesli brak sekcji w sprawdzanym piku .conf ORAZ sekcja w slowniku oznaczona jako wymagana*/
			if( temp_dict->wymagana == 1 )
			{
				bmdconf_niszcz_liste(&napotkane_sekcje);
				temp1=bmdconf_strmerge("ERROR: Brak wymaganej sekcji '",temp_dict->nazwa_sekcji);
				temp2=bmdconf_strmerge(temp1,"'.\n");
				free(temp1);
				return temp2;
			}
			/*else : nic sie nie dzieje i kolejna iteracja bo sekcja nie jest wymagana*/
		}
		/*else : nic sie nie dzieje i kolejna iteracja*/
		temp_dict=temp_dict->next;
	}
	bmdconf_niszcz_liste(&napotkane_sekcje);
	/*</@added>*/

	
	return NULL;
}


char* bmdconf_validate_config_tolerant_db(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary)
{
	struct bmdconf_sekcja *slownik=*dictionary;
	struct bmdconf_mem_list *konfiguracja=*dumped;
	struct bmdconf_pole *wsk_pole=NULL;
	struct bmdconf_wzorzec *iterator=NULL, *wzor_dict=NULL;
	int subpatterns[30];
	long wynik_porownania=0;
	long ostatnia_linia_nr=0;
	char *temp1=NULL;
	char *temp2=NULL;
	char *aktualna_sekcja=NULL;
	struct bmdconf_lista *pola_req=NULL;
	struct bmdconf_sekcja *temp_sekcja=NULL;
	struct bmdconf_pole *iter_pole=NULL, *temp_pole=NULL;
	struct bmdconf_mem_list* actual_copy=NULL;

	/*@added*/struct bmdconf_sekcja *temp_dict=*dictionary;
	/*@added*/struct bmdconf_lista *napotkane_sekcje; /*lista sekcji napotkanych w pliku*/
	/*@added*/bmdconf_init_lista(&napotkane_sekcje);

	/**/
	if(konfiguracja==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("ERROR: Brak wpisow konfiguracyjnych.","\n");
		return temp1;
	}
	/*jesli pusty slownik */
	if(slownik==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("ERROR: Nie zaladowano zadnego slownika badz ladowany slownik byl pusty.","\n");
		return temp1;
	}

	while(konfiguracja != NULL)
	{
		ostatnia_linia_nr=konfiguracja->line_number;
		/*jesli kolejny wpis badany*/
		if(aktualna_sekcja!=NULL)
		{
			/*jesli nowo napotkana sekcja*/
			/*w tym miejscu aktualna_sekcja wlasciwie jest juz nieaktualna :)*/
			if( strcmp(konfiguracja->section,aktualna_sekcja) != 0)
			{
				/*----------*/
				temp_sekcja=bmdconf_znajdz_sekcje(&slownik,aktualna_sekcja);
				/*jesli zaladowany jest slownik dla tej sekcji*/
				if(temp_sekcja!=NULL)
				{
					bmdconf_init_iterator_pole(&(temp_sekcja->field),&iter_pole);
					temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
					/*sprawdzanie dla wszystkich pol sekcji w slowniku */
					while(temp_pole!=NULL)
					{
						/*gdy pole wymagane i niezawarte w sekcji w pliku konfiguracyjnym*/
						if( temp_pole->pole_wymagane==1 && (bmdconf_znajdz_element(&pola_req,temp_pole->nazwa_pola))==0 )
						{
							/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
							bmdconf_niszcz_liste(&pola_req);
							return bmdconf_blad_braku_wymaganego_pola_db(aktualna_sekcja,temp_pole->nazwa_pola);
						}
						
						temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
					}
				}
				/*jesli w slowniku sekcji nie ma takiego wpisu, to nic sie nie dzeje*/

				/*---------*/
				bmdconf_niszcz_liste(&pola_req);
				free(aktualna_sekcja);
				aktualna_sekcja=(char*)malloc((strlen(konfiguracja->section)+1)*sizeof(char));
				strcpy(aktualna_sekcja,konfiguracja->section);
				/*@added*/bmdconf_dodaj_element(&napotkane_sekcje, aktualna_sekcja);
				bmdconf_init_lista(&pola_req);
				bmdconf_dodaj_element(&pola_req,konfiguracja->option);
			}
			/*jesli wpisy bedace kontynuacja sekcji*/
			else
			{
				bmdconf_dodaj_element(&pola_req,konfiguracja->option);
			}
		}
		/*jesli pierwszy wpis badany*/
		else
		{
			aktualna_sekcja=(char*)malloc((strlen(konfiguracja->section)+1)*sizeof(char));
			strcpy(aktualna_sekcja,konfiguracja->section);
			bmdconf_init_lista(&pola_req);
			bmdconf_dodaj_element(&pola_req,konfiguracja->option);
			/*@added*/bmdconf_dodaj_element(&napotkane_sekcje, aktualna_sekcja);
		}


/*------------------------------------------------------------------------------------------*/
		/*sprawdzanie aktualnie pobranego wpisu*/
		
		/*dla aktualnie pobranego wpisu sprawdzanie , czy slownik sekcji jest zaladowany*/
		temp_sekcja=bmdconf_znajdz_sekcje(&slownik,aktualna_sekcja);
		/*jesli slownik sekcji nie jest zaladowany, to wpis ignorowany(dla trybu tolerant to nie jest blad) */
		if(temp_sekcja==NULL )
		{
			actual_copy=konfiguracja;
			konfiguracja=konfiguracja->next;
			/*wyrzucanie z dumpa konfiguracji wpisu sekcji, dla ktorej nie zaladowano slownika
				dzieki temu nie bedzie mozna dobrac sie do wartosci, ktorych nikt sie nie spodziewa*/
			bmdconf_remove_list_item(dumped, actual_copy->section, actual_copy->option);
		}
		/*jesli slownik jest zaladowany, to sprawdzenie musi byc wykonane*/
		else
		{
			wsk_pole=bmdconf_znajdz_pole_sekcji_hash(&slownik,konfiguracja->section,konfiguracja->option);
			/*brak definicji pola w slowniku*/
			if(wsk_pole==NULL)
			{
				/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
				bmdconf_niszcz_liste(&pola_req);
				free(aktualna_sekcja);
				return bmdconf_blad_nieznane_pole_db(konfiguracja->line_number, konfiguracja->section, konfiguracja->option);
			}
			else
			{
				bmdconf_init_iterator_wzorzec(&(wsk_pole->wz),&iterator);
				wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
				while(wzor_dict!=NULL)
				{
					wynik_porownania=pcre_exec(wzor_dict->wzor,NULL,konfiguracja->value,strlen(konfiguracja->value),0,0,subpatterns,30);
					if(wzor_dict->zgodnosc==1)
					{
						/*w przypadku niedopoasowania, gdy wymagane dopasowanie, zwraca blad*/
						if(wynik_porownania<=0)
						{
							/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
							bmdconf_niszcz_liste(&pola_req);
							free(aktualna_sekcja);
							return bmdconf_blad_niedozwolona_wartosc_db(konfiguracja->line_number, konfiguracja->section, konfiguracja->option, konfiguracja->value);
						}
					}
					else
					{
						/*w przypadku dopasowania, gdy wymagane jest niedopasowanie, zwraca blad*/
						if(wynik_porownania>0)
						{
							/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
							bmdconf_niszcz_liste(&pola_req);
							free(aktualna_sekcja);
							return bmdconf_blad_niedozwolona_wartosc_db(konfiguracja->line_number,konfiguracja->section, konfiguracja->option, konfiguracja->value);
						}
					}
					wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
				}
				/*sprawdzenie,czy wyrazenie sciezkowe prowadzi do instniejacego pliku*/
				if(wsk_pole->flaga_plik==1)
				{
					/*jesli plik nie istnieje*/
					if(bmdconf_file_exists(konfiguracja->value)==0)
					{
						/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
						bmdconf_niszcz_liste(&pola_req);
						free(aktualna_sekcja);
						return bmdconf_blad_niepoprawnej_sciezki_db(konfiguracja->line_number,konfiguracja->value);
					}
				}
			
			}
			konfiguracja=konfiguracja->next;
		}
		
	}

	/*dla konca pliku konfiguracyjnego sprawdzanie poprawnosci ostatniej sekcji*/
	temp_sekcja=bmdconf_znajdz_sekcje(&slownik,aktualna_sekcja);
	/*jesli slownik dla tej sekcji jest zaladowany*/
	if(temp_sekcja!=NULL)
	{
		bmdconf_init_iterator_pole(&(temp_sekcja->field),&iter_pole);
		temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
		/*sprawdzanie dla wszystkich pol sekcji w slowniku */
		while(temp_pole!=NULL)
		{
			/*gdy pole wymagane i niezawarte w sekcji w pliku konfiguracyjnym*/
			if( temp_pole->pole_wymagane==1 && (bmdconf_znajdz_element(&pola_req,temp_pole->nazwa_pola))==0 )
			{
				/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
				bmdconf_niszcz_liste(&pola_req);
				return bmdconf_blad_braku_wymaganego_pola_db(aktualna_sekcja,temp_pole->nazwa_pola);
			}
			temp_pole=bmdconf_get_nextitem_pole(&iter_pole);
		}
	}
	/*jesli w slowniku sekcji nie ma zadnych wpisow, to nic sie nie dzeje*/
	bmdconf_niszcz_liste(&pola_req);
	free(aktualna_sekcja);
	
	/*<@added>*/
	while(temp_dict != NULL)
	{
		/*jesli sekcja ze slownika NIE znaleziona w liscie napotkanych sekcji*/
		if( (bmdconf_znajdz_element(&napotkane_sekcje, temp_dict->nazwa_sekcji)) != 1 )
		{
			/*jesli brak sekcji w sprawdzanym piku .conf ORAZ sekcja w slowniku oznaczona jako wymagana*/
			if( temp_dict->wymagana == 1 )
			{
				bmdconf_niszcz_liste(&napotkane_sekcje);
				temp1=bmdconf_strmerge("Brak wymaganej sekcji '",temp_dict->nazwa_sekcji);
				temp2=bmdconf_strmerge(temp1,"'.\n");
				free(temp1);
				return temp2;
			}
			/*else : nic sie nie dzieje i kolejna iteracja bo sekcja nie jest wymagana*/
		}
		/*else : nic sie nie dzieje i kolejna iteracja*/
		temp_dict=temp_dict->next;
	}
	bmdconf_niszcz_liste(&napotkane_sekcje);
	/*</@added>*/

	
	return NULL;
}

char* bmdconf_validate_config_db(bmd_conf* struktura)
{
	if(struktura->tryb==BMDCONF_RESTRICT)
	{
		return bmdconf_validate_config_restrict_db( &(struktura->dump), &(struktura->dict) );
	}
	else
	{
		return bmdconf_validate_config_tolerant_db( &(struktura->dump), &(struktura->dict) );
	}
}

long bmdconf_load_db(char *db_params, bmd_conf *struktura, char **error_string)
{

	char *ret=NULL;

	if (!(db_params != NULL))
	{
		PRINT_ERROR("db_params nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(struktura != NULL))
	{
		PRINT_ERROR("Pusta struktura typu bmd_conf! (WS)\n");
		abort();
	}

	ret=bmdconf_read_and_parse_config_db(db_params, &(struktura->dump));
	if(ret!=NULL)
	{
		if(error_string!=NULL)
		{
			*error_string=ret;
		}
		else
		{
			PRINT_DEBUG("%s", ret);
			free(ret); ret=NULL;
		}
		return -1; /*blad przy parsingu lub polaczeniu z baza*/
	}
	
	ret=bmdconf_validate_config_db(struktura);
	if(ret!=NULL)
	{
		if(error_string != NULL)
			*error_string = ret;
		else
		{
			PRINT_DEBUG("%s", ret);
			free(ret); ret=NULL;
		}
		/*niszczenie dumpa pliku konfiguracyjnego , gdy niepowodzenie walidacji*/
		bmdconf_delete_list(&(struktura->dump)); 
		return -2; /*blad przy walidacji pliku konfigracyjnego*/
	}

	return 0;
}


/*! PUBLICZNE FUNKCJE "ADMINISTRATORSKIE" */
/*!!! MODIFIED na wejsciu connection - uprzednio nawiazane polaczenie z baza (np. za pomoca PQconnectdb() )*/

long bmdconf_insert_option_db(char* conn_params, char *section, char *option, char *value, char **error)
{
	char *id_section=NULL, *query=NULL;
	PGresult *result_option=NULL;
	PGresult *result_section=NULL;
	char *dl_err=NULL;
	PGconn *connection=NULL;
	
	const char *error_pointer=NULL;
	int error_offset=0;
	pcre *regex=NULL;
	char *wzorzec="^[_a-zA-Z][_0-9a-zA-Z]*$"; /*ten sam dla nazwy sekcji jak i opcji*/
	int subpatterns[4];

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;
	char* (*PQhost)(PGconn*)=NULL;
	char* (*PQdb)(PGconn*)=NULL;
	char* (*PQuser)(PGconn*)=NULL;
	/**/

	if (!(section != NULL))
	{
		PRINT_ERROR("section nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(option != NULL))
	{
		PRINT_ERROR("option nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(value != NULL))
	{
		PRINT_ERROR("value nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL! (WS)\n");
		abort();
	}

	if (!( error!=NULL && *error==NULL))
	{
		PRINT_ERROR("Wystąpił błąd!(WS)\n");
		abort();
	}


	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQhost)=dlsym(lib, "PQhost");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQdb)=dlsym(lib, "PQdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQuser)=dlsym(lib, "PQuser");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	/**/

	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		asprintf(error,"ERROR: Wystapil problem z polaczeniem do bazy.\nCONNECTION INFO: host='%s', database='%s'; user='%s';\n", PQhost(connection), PQdb(connection), PQuser(connection));
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}

	regex=pcre_compile(wzorzec, PCRE_UTF8, &error_pointer, &error_offset, NULL);
	if( pcre_exec(regex,NULL,section,strlen(section),0,0,subpatterns,4) <= 0 )
	{
		pcre_free(regex);
		asprintf(error, "ERROR: Nazwa sekcji '%s' niezgodna z wymaganym formatem.\n", section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	if(pcre_exec(regex,NULL,option,strlen(option),0,0,subpatterns,4) <= 0)
	{
		pcre_free(regex);
		asprintf(error, "ERROR: Nazwa opcji '%s' niezgodna z wymaganym formatem.\n", option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	pcre_free(regex);
	if(contain_quotation(value)==0)
	{
		asprintf(error, "ERROR: Podana wartosc zawiera niedozwolony znak ( \" ).\n");
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}


	/*sprawdzanie, czy istnieje juz w bazie podana sekcja*/
	asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	/*jesli nie ma jeszcze sekcji*/
	if(PQntuples(result_section) == 0)
	{
		PQclear(result_section);
		asprintf(&query, "INSERT INTO sections (section) VALUES ('%s')", section);
		result_section=PQexec(connection, query);
		free(query);
		if(PQresultStatus(result_section) != PGRES_COMMAND_OK)
		{
			asprintf(error, "ERROR: Wstawianie sekcji '%s' nie powiodlo sie.\n", section);
			PQclear(result_section);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
		/**/
		PQclear(result_section);

		asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
		result_section=PQexec(connection, query);
		free(query);
		if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
		{
			*error=strdup("ERROR #2: Brak oczekiwanych danych w odpowiedzi.\n");
			PQclear(result_section);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
	}
		
	id_section=PQgetvalue(result_section, 0, 0);
	asprintf(&query, "SELECT id FROM options WHERE options.id_sec=%s AND option='%s'", id_section, option);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) == PGRES_TUPLES_OK)
	{
		/*jesli juz jest wpis dla takiej opcji, to blad*/
		if(PQntuples(result_option) != 0)
		{
			asprintf(error,"ERROR: Wpis dla opcji '%s' w sekcji '%s' juz istnieje w bazie.\n", option, section);
			PQclear(result_section);
			PQclear(result_option);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
	}
	else
	{
		*error=strdup("ERROR #3: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_option);
	
	asprintf(&query, "INSERT INTO options (id_sec, option, value) VALUES (%s, '%s', '%s')", id_section, option, value);
	PQclear(result_section);
	result_option=PQexec(connection, query);
	free(query); query=NULL;
	if(PQresultStatus(result_option) != PGRES_COMMAND_OK)
	{
		asprintf(error, "ERROR: Wstawianie opcji '%s' w sekcji '%s' nie powiodlo sie.\n", option, section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_option);
	PQfinish(connection);
	dlclose(lib);
	return 0;
}

long bmdconf_set_value_db(char* conn_params, char *section, char *option, char *value, char **error)
{
	char *id_section=NULL, *query=NULL;
	PGresult *result_option=NULL;
	PGresult *result_section=NULL;
	char *dl_err=NULL;
	PGconn *connection=NULL;

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;
	char* (*PQhost)(PGconn*)=NULL;
	char* (*PQdb)(PGconn*)=NULL;
	char* (*PQuser)(PGconn*)=NULL;
	/**/


	if (!(section != NULL))
	{
		PRINT_ERROR("Section nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(option != NULL))
	{
		PRINT_ERROR("option nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(value != NULL))
	{
		PRINT_ERROR("value nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL! (WS)\n");
		abort();
	}

	if (!( error!=NULL && *error==NULL))
	{
		PRINT_ERROR("Wystąpił błąd! (WS)\n");
		abort();
	}

	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQhost)=dlsym(lib, "PQhost");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQdb)=dlsym(lib, "PQdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQuser)=dlsym(lib, "PQuser");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	/**/

	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		asprintf(error,"ERROR: Wystapil problem z polaczeniem do bazy.\nCONNECTION INFO: host='%s', database='%s'; user='%s';\n", PQhost(connection), PQdb(connection), PQuser(connection));
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	if(contain_quotation(value)==0)
	{
		asprintf(error, "ERROR: Nowa wartosc zawiera niedozwolony znak ( \" ).\n");
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}

	/*sprawdzanie, czy istnieje juz w bazie podana sekcja*/
	asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	/*jesli w bazie nie ma podanej sekcji*/
	if( PQntuples(result_section) == 0 )
	{
		asprintf(error, "ERROR: Brak w bazie sekcji '%s'.\n", section);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	id_section=PQgetvalue(result_section, 0, 0);
	asprintf(&query, "SELECT id FROM options WHERE options.id_sec=%s AND option='%s'", id_section, option);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) == PGRES_TUPLES_OK)
	{
		/*jesli brak w bazie podanej opcji dla podanej sekcji*/
		if(PQntuples(result_option) == 0)
		{
			asprintf(error,"ERROR: Brak w bazie opcji '%s' w sekcji '%s'.\n", option, section);
			PQclear(result_section);
			PQclear(result_option);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
	}
	else
	{
		*error=strdup("ERROR #2: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	asprintf(&query, "UPDATE options SET value='%s' WHERE id=%s", value, PQgetvalue(result_option, 0, 0) );
	PQclear(result_option);
	result_option=PQexec(connection, query);
	free(query); query=NULL;
	if(PQresultStatus(result_option) != PGRES_COMMAND_OK)
	{
		asprintf(error, "ERROR: Modyfikacja wartosci opcji '%s' w sekcji '%s' nie powiodla sie.\n", option, section);
		PQclear(result_option);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_section);
	PQclear(result_option);
	PQfinish(connection);
	dlclose(lib);
	return 0;
}

long bmdconf_delete_option_db(char *conn_params, char *section, char *option, char **error)
{
	char *id_section=NULL, *query=NULL;
	PGresult *result_option=NULL;
	PGresult *result_section=NULL;
	char* dl_err=NULL;
	PGconn *connection=NULL;

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;
	char* (*PQhost)(PGconn*)=NULL;
	char* (*PQdb)(PGconn*)=NULL;
	char* (*PQuser)(PGconn*)=NULL;
	/**/


	if (!(section != NULL))
	{
		PRINT_ERROR("Section nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(option != NULL))
	{
		PRINT_ERROR("option nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL! (WS)\n");
		abort();
	}

	if (!( error!=NULL && *error==NULL))
	{
		PRINT_ERROR("Wystąpił błąd! (WS)\n");
		abort();
	}

	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQhost)=dlsym(lib, "PQhost");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQdb)=dlsym(lib, "PQdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQuser)=dlsym(lib, "PQuser");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	/**/

	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		asprintf(error,"ERROR: Wystapil problem z polaczeniem do bazy.\nCONNECTION INFO: host='%s', database='%s'; user='%s';\n", PQhost(connection), PQdb(connection), PQuser(connection));
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	/*sprawdzanie, czy istnieje juz w bazie podana sekcja*/
	asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	/*jesli w bazie nie ma podanej sekcji*/
	if( PQntuples(result_section) == 0 )
	{
		asprintf(error, "ERROR: Brak w bazie sekcji '%s'.\n", section);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}

	id_section=PQgetvalue(result_section, 0, 0);
	asprintf(&query, "SELECT id FROM options WHERE options.id_sec=%s AND option='%s'", id_section, option);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) == PGRES_TUPLES_OK)
	{
		/*jesli brak w bazie podanej opcji dla podanej sekcji*/
		if(PQntuples(result_option) == 0)
		{
			asprintf(error,"ERROR: Brak w bazie opcji '%s' w sekcji '%s'.\n", option, section);
			PQclear(result_section);
			PQclear(result_option);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
	}
	else
	{
		*error=strdup("ERROR #2: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}

	asprintf(&query, "DELETE FROM options WHERE id=%s", PQgetvalue(result_option, 0, 0));
	PQclear(result_option);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) != PGRES_COMMAND_OK)
	{
		*error=strdup("ERROR: Usuwanie wpisu konfiguracyjnego zakonczone niepowodzeniem.\n");
		PQclear(result_section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_option);

	/*zapytanie sprawdzajace czy sa jeszcze jakies opcje dla danej sekcji*/
	asprintf(&query, "SELECT * FROM options WHERE id_sec=%s", id_section);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #3: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	/*jesli nie ma opcji dla danej sekcji, to mozna wykasowac sama sekcje*/
	if(PQntuples(result_option) == 0)
	{
		PQclear(result_option);
		asprintf(&query, "DELETE FROM sections WHERE id_sec=%s",id_section);
		result_option=PQexec(connection, query);
		free(query);
		/*mimo ze nie mozna wykasowac pustej sekcji, usuwanie opcji powiodlo sie - return 0 !! */
		if(PQresultStatus(result_option) != PGRES_COMMAND_OK)
		{
			PQclear(result_option);
			PQclear(result_section);
			PQfinish(connection);
			dlclose(lib);
			return 0;
		}
	}
	PQclear(result_option);
	PQclear(result_section);
	PQfinish(connection);
	dlclose(lib);
	return 0;
}

/*usuwa cala sekcje tj. usuwa wszystkie wpisy sekcji i sama sekcje
 usuwanie sekcji - iteracyjne usuwanie opcji sekcji zastopione funkcja bmdconf_delete_section_db  
LIBBMDCONFIG_SCOPE long bmdconf_delete_section_iter_db(PGconn *connection, char *section, char **error);
*/
/*
long bmdconf_delete_section_iter_db(char *conn_params, char *section, char **error)
{
	char *id_section=NULL, *query=NULL;
	PGresult *result_option=NULL;
	PGresult *result_section=NULL;
	PGresult *result_deletion=NULL;
	long iter=0, num_opt=0;
	char *dl_err=NULL;
	PGconn *connection=NULL;	

	//wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;
	char* (*PQhost)(PGconn*)=NULL;
	char* (*PQdb)(PGconn*)=NULL;
	char* (*PQuser)(PGconn*)=NULL;
	//

	bmd_assert(section != NULL, "WSZ");
	bmd_assert(conn_params != NULL, "WSZ");
	bmd_assert( error!=NULL && *error==NULL, "WSZ");

	//otwieranie biblioteki dzielonej i zczytywanie symboli
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(&dl_err, "ERROR: %s\n", dlerror());
		return dl_err;
	}

	//zczytywanie symboli
	CAST_FUNC(PQconnectdb)=(PGconn* (*)(char*))dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQhost)=dlsym(lib, "PQhost");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQdb)=dlsym(lib, "PQdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQuser)=dlsym(lib, "PQuser");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	//


	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		asprintf(error,"ERROR: Wystapil problem z polaczeniem do bazy.\nCONNECTION INFO: host='%s', database='%s'; user='%s';\n", PQhost(connection), PQdb(connection), PQuser(connection));
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	//sprawdzanie, czy istnieje juz w bazie podana sekcja
	asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	//jesli w bazie nie ma podanej sekcji
	if( PQntuples(result_section) == 0 )
	{
		asprintf(error, "ERROR: Brak w bazie sekcji '%s'.\n", section);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}

	id_section=PQgetvalue(result_section, 0, 0);
	asprintf(&query, "SELECT id FROM options WHERE id_sec=%s", id_section);
	PQclear(result_section);
	result_option=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_option) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #2: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_option);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	num_opt=PQntuples(result_option);
		
	for(iter=0; iter<num_opt; iter++)
	{
		asprintf(&query, "DELETE FROM options WHERE id=%s", PQgetvalue(result_option, iter, 0) );
		result_deletion=PQexec(connection, query);
		free(query);
		if(PQresultStatus(result_deletion) != PGRES_COMMAND_OK)
		{
			*error=strdup("ERROR: Usuwanie opcji podanej sekcji zakonczone niepowodzeniem.\n");
			PQclear(result_option);
			PQclear(result_deletion);
			PQfinish(connection);
			dlclose(lib);
			return -1;
		}
		PQclear(result_deletion);
	}
	PQclear(result_option);

	asprintf(&query, "DELETE FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query); query=NULL;
	if(PQresultStatus(result_section) != PGRES_COMMAND_OK)
	{
		*error=strdup("ERROR: Usuwanie sekcji zakonczone niepowodzeniem.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_section);
	PQfinish(connection);
	dlclose(lib);
	return 0;
}
*/

/*usuwa cala sekcje  tj. usuwa wszystkie wpisy sekcji i sama sekcje
 usuwanie calej sekcji - kaskadowe usuwanie opcji sekcji po stronie postgres (ON DELETE CASCADE)*/
long bmdconf_delete_section_db(char* conn_params, char *section, char **error)
{
	char *query=NULL;
	PGresult *result_section=NULL;
	char* dl_err=NULL;
	PGconn *connection=NULL;

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQhost)(PGconn*)=NULL;
	char* (*PQdb)(PGconn*)=NULL;
	char* (*PQuser)(PGconn*)=NULL;
	/**/
	
	if (!(section != NULL))
	{
		PRINT_ERROR("Section nie może być NULL! (WS)\n");
		abort();
	}
	
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL! (WS)\n");
		abort();
	}

	if (!( error!=NULL && *error==NULL))
	{
		PRINT_ERROR("Wystąpił błąd! (WS)\n");
		abort();
	}
	
	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQhost)=dlsym(lib, "PQhost");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQdb)=dlsym(lib, "PQdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQuser)=dlsym(lib, "PQuser");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	/**/

	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		asprintf(error,"ERROR: Wystapil problem z polaczeniem do bazy.\nCONNECTION INFO: host='%s', database='%s'; user='%s';\n", PQhost(connection), PQdb(connection), PQuser(connection));
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	
	/*sprawdzanie, czy istnieje juz w bazie podana sekcja*/
	asprintf(&query, "SELECT id_sec FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_TUPLES_OK)
	{
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	/*jesli w bazie nie ma podanej sekcji*/
	if( PQntuples(result_section) == 0 )
	{
		asprintf(error, "ERROR: Brak w bazie sekcji '%s'.\n", section);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_section);

	asprintf(&query, "DELETE FROM sections WHERE section='%s'", section);
	result_section=PQexec(connection, query);
	free(query);
	if(PQresultStatus(result_section) != PGRES_COMMAND_OK)
	{
		asprintf(error, "ERROR: Usuwanie sekcji '%s' zakonczone niepowodzeniem.\n", section);
		PQclear(result_section);
		PQfinish(connection);
		dlclose(lib);
		return -1;
	}
	PQclear(result_section);
	PQfinish(connection);
	dlclose(lib);
	return 0;
}


/*czysci baze z wpisami konfiguracyjnymi - strukture rzecz jasna pozostawia nietknieta*/
long bmdconf_clean_db(char* conn_params, char **error)
{
	PGconn *connection=NULL;
	PGresult *result_sections=NULL;
	long iter_sek=0, ile_sekcji=0;
	long ret_status=0;
	char *sekcja=NULL;
	char *del_error=NULL;
	char* dl_err=NULL;

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;

	
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL (WS)\n");
		abort();
	}
	
	if (!(error != NULL))
	{
		PRINT_ERROR("Wystąpił błąd!(WS)\n");
		abort();
	}
	
	if (!(*error == NULL))
	{
		PRINT_ERROR("Wystąpił błąd!(WS)\n");
		abort();
	}

	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");


	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		dlclose(lib);
		*error=strdup("ERROR: Nie mozna polaczyc z baza.\n");
		return -1;
	}
	
	result_sections=PQexec(connection, "SELECT section FROM sections");
	if(PQresultStatus(result_sections) != PGRES_TUPLES_OK)
	{
		dlclose(lib);
		PQfinish(connection);
		*error=strdup("ERROR : Brak oczekiwanych danych w odpowiedzi.\n");
		return -1;
	}
	
	PQfinish(connection);
	
	ile_sekcji=PQntuples(result_sections);
	for(iter_sek=0; iter_sek<ile_sekcji; iter_sek++)
	{
		sekcja=PQgetvalue(result_sections, iter_sek, 0);
		ret_status=bmdconf_delete_section_db(conn_params, sekcja, &del_error);
		if(ret_status != 0)
		{
			dlclose(lib);
			free(del_error);
			PQclear(result_sections);
			*error=strdup("ERROR: Nie mozna usunac z bazy poprzednich wpisow konfiguracyjnych.\n");
			return -1;
		}
	}
	PQclear(result_sections);
	dlclose(lib);
	return 0;
}


/*najpierw czyści bazę, potem wstawia konfigurację*/
long bmdconf_file2db(char *file_name, char* conn_params, char** error)
{
	bmdconf_dump *dump=NULL;
	bmdconf_dump *temp=NULL;
	char *read_error=NULL;
	char* del_error=NULL, *insert_error=NULL;
	long ret_status=0;
	FILE *plik=NULL;
	

	if (!(file_name != NULL))
	{
		PRINT_ERROR("file_name nie może być NULL!(WS)\n");
		abort();
	}
	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL!(WS)\n");
		abort();
	}
	if (!(error != NULL))
	{
		PRINT_ERROR("error nie może być NULL!(WS)\n");
		abort();
	}
	if(!(*error == NULL))
	{
		PRINT_ERROR("Wystąpił błąd!(WS)\n");
		abort();
	}

	plik=fopen(file_name, "r");
	if(plik==NULL)
	{
		*error=strdup("ERROR: Nie mozna otworzyc podany plik konfiguracyjny.\n");
		return -1;
	}
	read_error=bmdconf_read_and_parse_config(plik, &dump);
	if(read_error != NULL)
	{
		fclose(plik);
		free(read_error);
		*error=strdup("ERROR: Niepoprawny plik konfiguracyjny.\n");
		return -1;
	}
	fclose(plik); plik=NULL;

	/*czyszczenie bazy*/
	ret_status=bmdconf_clean_db(conn_params, &del_error);
	if(ret_status != 0)
	{
		*error=strdup("ERROR: Nie mozna usunac z bazy poprzednich wpisow konfiguracyjnych.\n");
		bmdconf_delete_list(&dump);
		return -1;
	}
	
	temp=dump;
	while(temp!=NULL)
	{
		ret_status=bmdconf_insert_option_db(conn_params, temp->section, temp->option, temp->value,
							 &insert_error);
		if(ret_status != 0)
		{
			bmdconf_delete_list(&dump);
			/*usuwanie wpisow, ktore udalo sie wstawic przed wystapieniem bledu
				tutaj juz nie sprawdzam statusu usuwania, bo nie ma to wiekszego sensu */
			bmdconf_clean_db(conn_params, &del_error);
			free(del_error);
			free(insert_error);
			*error=strdup("ERROR: Wystapil problem ze wstawieniem wpisow konfiguracyjnych do bazy.\n");
			return -1;
		}
		temp=temp->next;
	}
	bmdconf_delete_list(&dump);
	return 0;
}


long bmdconf_db2file(char *conn_params, char* file_name, char** error)
{
	PGconn *connection=NULL;
	PGresult *result_sections=NULL, *result_options=NULL;
	FILE *new_conf_file=NULL;
	long ile_sekcji=0;
	long ile_opcji=0;
	long iter_sec=0, iter_opt=0;
	char *section=NULL, *section_id=NULL;
	char *option=NULL, *value=NULL;
	char *query=NULL;
	char* dl_err=NULL;

	/*wskazniki funkcji, ktore beda powiazane z symbolami z libpq.so*/
	void *lib=NULL;
	PGconn* (*PQconnectdb)(char*)=NULL;
	ConnStatusType* (*PQstatus)(PGconn*)=NULL;
	PGresult* (*PQexec)(PGconn*, const char*)=NULL;
	ExecStatusType (*PQresultStatus)(const PGresult*)=NULL;
	void (*PQfinish)(PGconn*)=NULL;
	void (*PQclear)(PGresult*)=NULL;
	long (*PQntuples)(const PGresult*)=NULL;
	char* (*PQgetvalue)(const PGresult*, int, int)=NULL;


	if (!(conn_params != NULL))
	{
		PRINT_ERROR("conn_params nie może być NULL!(WS)\n");
		abort();
	}

	if (!(file_name != NULL))
	{
		PRINT_ERROR("file_name nie może być NULL!(WS)\n");
		abort();
	}
	if (!(error != NULL))
	{
		PRINT_ERROR("error nie może być NULL!(WS)\n");
		abort();
	}
	if (!(*error == NULL))
	{
		PRINT_ERROR("Wystąpił błąd!(WS)\n");
		abort();
	}


	/*otwieranie biblioteki dzielonej i zczytywanie symboli*/
	lib=dlopen(LIBPQ_PATH, RTLD_NOW);
	if(lib==NULL)
	{
		asprintf(error, "ERROR: %s\n", dlerror());
		return -1;
	}

	/*zczytywanie symboli*/
	CAST_FUNC(PQconnectdb)=dlsym(lib, "PQconnectdb");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQstatus)=dlsym(lib, "PQstatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQexec)=dlsym(lib, "PQexec");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQresultStatus)=dlsym(lib, "PQresultStatus");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQfinish)=dlsym(lib, "PQfinish");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQclear)=dlsym(lib, "PQclear");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQntuples)=dlsym(lib, "PQntuples");
	CHECK_DL_ERROR_INT(lib, error, dl_err)
	CAST_FUNC(PQgetvalue)=dlsym(lib, "PQgetvalue");


	connection=PQconnectdb(conn_params);
	if(PQstatus(connection) != CONNECTION_OK)
	{
		dlclose(lib);
		*error=bmdconf_strmerge("ERROR: Nie mozna polaczyc z baza.\n","");
		return -1;
	}
	
	new_conf_file=fopen(file_name,"w");
	if(new_conf_file == NULL)
	{
		dlclose(lib);
		PQfinish(connection);
		*error=strdup("ERROR: Nie można utworzyć pliku.\n");
		return -1;
	}

	result_sections=PQexec(connection, "SELECT * FROM sections");
	if(PQresultStatus(result_sections) != PGRES_TUPLES_OK)
	{
		dlclose(lib);
		*error=strdup("ERROR #1: Brak oczekiwanych danych w odpowiedzi.\n");
		PQclear(result_sections);
		PQfinish(connection);
		fclose(new_conf_file);
		remove(file_name);
		return -1;
	}
	
	ile_sekcji=PQntuples(result_sections);
	if(ile_sekcji == 0)
	{
		dlclose(lib);
		fprintf(new_conf_file, "\n# Wskazana baza danych nie zawiera wpisów konfiguracyjnych\n\n");
		fclose(new_conf_file);
		PQclear(result_sections);
		PQfinish(connection);
		return 0;
	}
	
	for(iter_sec=0; iter_sec<ile_sekcji; iter_sec++)
	{
		section_id=PQgetvalue(result_sections, iter_sec, 0);
		section=PQgetvalue(result_sections, iter_sec, 1);
		fprintf(new_conf_file, "\n\n[%s]\n\n", section);
		
		query=bmdconf_strmerge("SELECT option, value FROM options WHERE id_sec=", section_id);
		result_options=PQexec(connection, query);
		free(query);
		if(PQresultStatus(result_options) != PGRES_TUPLES_OK)
		{
			dlclose(lib);
			*error=strdup("ERROR #2: Brak oczekiwanych danych w odpowiedzi.\n");
			PQclear(result_options);
			PQclear(result_sections);
			PQfinish(connection);
			fclose(new_conf_file);
			remove(file_name);
			return -1;
		}

		ile_opcji=PQntuples(result_options);
		for(iter_opt=0; iter_opt<ile_opcji; iter_opt++)
		{
			option=PQgetvalue(result_options, iter_opt, 0);
			value=PQgetvalue(result_options, iter_opt, 1);
			fprintf(new_conf_file, "%s = \"%s\"\n", option, value);
		}
		PQclear(result_options);
	}
	
	PQclear(result_sections);
	PQfinish(connection);
	
	fprintf(new_conf_file, "\n");
	fclose(new_conf_file);
	dlclose(lib);
	return 0;
}
#endif
#else /*ifndef WIN32*/
#pragma warning(disable:4206)
#endif
