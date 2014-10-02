#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

/*zwraca linie pliku gdy sukces
	 (dla pustej lini zwraca "\n")
	 NULL, jesli EOF*/
char* bmdconf_wiersz_pliku(FILE *plik)
{
	long rozmiar;
	char *temp;
	char znak[2];
	char *buff=(char*)malloc(2*sizeof(char));
	strcpy(znak," \0");
	strcpy(buff," \0");
	if((znak[0]=(char)fgetc(plik))==EOF)
	{
		free(buff);
		return NULL;
	}
	else
	{
		buff[0]=znak[0];
		if(strcmp(buff,"\n")==0)
		    return buff;
		while((znak[0]=(char)fgetc(plik))!='\n')
		{
			/*jesli zlapie EOF w trakcie odczytu lini*/
			if(znak[0]!=EOF)
			{
				rozmiar=(int)strlen(buff);
				temp=(char*)malloc((rozmiar+1)*sizeof(char));
				strcpy(temp,buff);
				free(buff);
				/*tu zmiana*/
				buff=bmdconf_strmerge(temp,znak);
				free(temp);
			}
			else
				break;
		}
		return buff;
	}
}


void bmdconf_init_dump_config(struct bmdconf_mem_list **dumped)
{
	bmdconf_init_list(dumped);
}

void bmdconf_delete_dump_config(struct bmdconf_mem_list **dumped)
{
	bmdconf_delete_list(dumped);
}

/*zwraca NULL - ok
	 inaczej komunkat błedu- niezgodny*/
char* bmdconf_read_and_parse_config(FILE *fileconf,struct bmdconf_mem_list **dumped )
{
char *wiersz,*sub1,*sub2;
char *aktualna_sekcja=NULL;
long nr_wiersza=1;
const char *error_pointer;
int error_offset;
int subpatterns[20];
pcre *sekcja_regex, *assign_regex,*komentarz_regex;
char *wzorzec_sekcja="^\\[([_a-zA-Z][_0-9a-zA-Z]*?)\\]\\s*(#.*)*$";
char *wzorzec_assign="^([_a-zA-Z][_0-9a-zA-Z]*)\\s*=\\s*\"([^\"]*?)\"\\s*(#.*)*$";
char *wzorzec_komentarz="^\\s*(#.*)*$";
struct bmdconf_lista *sekcje=NULL;
struct bmdconf_lista *opcje=NULL;

	bmdconf_init_list(dumped);
	/*wozrzec_sekcja - do sprawdzania poprawnosci dekaracji sekcji*/
	sekcja_regex=pcre_compile(wzorzec_sekcja,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	/*wzorzec_assign - do sprawdzenia poprawnosci przpisania wartosci do pola*/
	assign_regex=pcre_compile(wzorzec_assign,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	/*wzorzez_komentarz - do sprawdzenia poprawnośći pustej lini, komentarza*/
	komentarz_regex=pcre_compile(wzorzec_komentarz,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	bmdconf_init_lista(&sekcje);
	while( (wiersz=bmdconf_wiersz_pliku(fileconf))!=NULL )
	{
		/*jesli linia pusta "\n"*/
		if(strcmp(wiersz,"\n")==0)
		{
		    nr_wiersza++;
		    free(wiersz);
		    continue;
		}

		/*jesli deklaracja sekcji*/
		if( pcre_exec(sekcja_regex,NULL,wiersz,(int)strlen(wiersz),0,0,subpatterns,20) > 0 )
		{
			/*tworzenie nowej listy do sprawdzania wielokrotnosci wystepowania tej samej opcji w sekcji */
			bmdconf_niszcz_liste(&opcje);
			bmdconf_init_lista(&opcje);
			if(aktualna_sekcja!=NULL)
			    free(aktualna_sekcja);
			aktualna_sekcja=bmdconf_substr(wiersz,subpatterns[2],subpatterns[3]);
			/*jesli nie ma w liscie sekcji, a wiec nowa deklaracja*/
			if(bmdconf_znajdz_element(&sekcje,aktualna_sekcja)==0)
			    bmdconf_dodaj_element(&sekcje,aktualna_sekcja);
			/*znalazl w liscie sekcji wiec podwojna deklaracja*/
			else
			{
				free(aktualna_sekcja);
				bmdconf_niszcz_liste(&sekcje);
				bmdconf_niszcz_liste(&opcje);
				bmdconf_delete_list(dumped);
				sub1=bmdconf_itostr(nr_wiersza);
				sub2=bmdconf_strmerge("Blad w lini nr:",sub1);
				bmdconf_strmerge_free(sub1);
				sub1=bmdconf_strmerge(sub2," -> wielokrotna deklaracja sekcji");
				bmdconf_strmerge_free(sub2);
				pcre_free(sekcja_regex);
				pcre_free(assign_regex);
				pcre_free(komentarz_regex);
				free(wiersz);
				return sub1;
			}
			nr_wiersza++;
		}
		else
		{
			/*jesli przypisanie wartosci do pola*/
			if( pcre_exec(assign_regex,NULL,wiersz,(int)strlen(wiersz),0,0,subpatterns,20) > 0 )
			{
				/*brak deklaracji sekcji na poczatku pliku*/
				if(aktualna_sekcja==NULL)
				{
					bmdconf_delete_list(dumped);
					bmdconf_niszcz_liste(&sekcje);
					bmdconf_niszcz_liste(&opcje);
					sub1=bmdconf_itostr(nr_wiersza);
					sub2=bmdconf_strmerge("Blad: brak deklaracji sekcji -> linia:",sub1);
					bmdconf_strmerge_free(sub1);
					pcre_free(sekcja_regex);
					pcre_free(assign_regex);
					pcre_free(komentarz_regex);
					free(wiersz);
					return sub2;
				}
				/**/
				sub1=bmdconf_substr(wiersz, subpatterns[2],subpatterns[3]);
				if(subpatterns[4]!=subpatterns[5])
				{
				    sub2=bmdconf_substr(wiersz, subpatterns[4],subpatterns[5]);
				}
				else
				{
				    sub2=(char*)malloc(sizeof(char));
				    sub2[0]='\0';
				}
				/*nie ma w liscie opcji tych, ktore pojawily sie w sekcji*/
				if(bmdconf_znajdz_element(&opcje,sub1)==0)
					bmdconf_dodaj_element(&opcje,sub1);
				/*powtorzyla sie opcja w sekcji*/
				else
				{
					free(aktualna_sekcja);/*dodane po wyciekach*/
					bmdconf_delete_list(dumped);
					bmdconf_niszcz_liste(&sekcje);
					bmdconf_niszcz_liste(&opcje);
					bmdconf_substr_free(sub1);
					bmdconf_substr_free(sub2);
					sub1=bmdconf_itostr(nr_wiersza);
					sub2=bmdconf_strmerge("Blad w lini nr:",sub1);
					bmdconf_strmerge_free(sub1);
					sub1=bmdconf_strmerge(sub2," -> powtorne wystapienie opcji w sekcji");
					bmdconf_strmerge_free(sub2);
					pcre_free(sekcja_regex);
					pcre_free(assign_regex);
					pcre_free(komentarz_regex);
					free(wiersz);
					return sub1;
				}

				bmdconf_add_list_item(dumped,nr_wiersza,aktualna_sekcja,sub1,sub2);
				bmdconf_substr_free(sub1);
				bmdconf_substr_free(sub2);
				nr_wiersza++;
			}
			else
			{
				/*jesli linia niezgodna z formatem*/
				if( pcre_exec(komentarz_regex,NULL,wiersz,(int)strlen(wiersz),0,0,subpatterns,20) <= 0 )
				{
					bmdconf_delete_list(dumped);
					bmdconf_niszcz_liste(&sekcje);
					bmdconf_niszcz_liste(&opcje);
					if(aktualna_sekcja!=NULL)
						free(aktualna_sekcja);
					sub1=bmdconf_itostr(nr_wiersza);
					sub2=bmdconf_strmerge("Blad skladni w lini nr:",sub1);
					bmdconf_strmerge_free(sub1);
					pcre_free(sekcja_regex);
					pcre_free(assign_regex);
					pcre_free(komentarz_regex);
					free(wiersz);
					return sub2;
				}
				/*komentarz badz spajce/tab*/
				else
					nr_wiersza++;
			}
		}
		free(wiersz);
	}

	if(aktualna_sekcja!=NULL)
		free(aktualna_sekcja);
	bmdconf_niszcz_liste(&sekcje); /*dodane po wyciekach*/
	bmdconf_niszcz_liste(&opcje); /**/
	pcre_free(sekcja_regex);
	pcre_free(assign_regex);
	pcre_free(komentarz_regex);
	return NULL;
}

/*wynik trzeba zwalniac, bo jest dynamicznie alokowany*/
char* bmdconf_get_value_config(struct bmdconf_mem_list **list_name,char *section_name,char *option_name)
{
char *temp1,*temp2;

	PRINT_VDEBUG("BMDCONFIGINF Getting configuration value %s in section %s\n",option_name, section_name);

	temp1=bmdconf_find_list_item(list_name,section_name,option_name);

	if(temp1!=NULL)
	{
		temp2=(char*)malloc( (strlen(temp1)+1)*sizeof(char) );
		strcpy(temp2,temp1);
		return temp2;
	}

	return NULL;
}

/*wyniku nie trzeba zwalniac - wskaznik bezposrenio do stringa w strukturze bmdconf_dump*/
const char* bmdconf_get_value_static_config(struct bmdconf_mem_list **list_name,char *section_name,char *option_name)
{
	const char *temp;
	/*NULL jesli nie znaleziono, wskaznik do stringa jesli ok.*/
	temp=bmdconf_find_list_item(list_name,section_name,option_name);
	return temp;
}

char* bmdconf_blad_niedozwolona_wartosc(long nr_linii,char *nazwa_pola)
{
	char *temp1, *temp2;

	temp1=bmdconf_itostr(nr_linii);
	temp2=bmdconf_strmerge("Blad w lini nr: ",temp1);
	bmdconf_itostr_free(temp1);
	temp1=bmdconf_strmerge(temp2,"; Niedozwolona wartosc pola \"");
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,nazwa_pola);
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,"\"\n");
	bmdconf_strmerge_free(temp2);
	return temp1;
}

char* bmdconf_blad_nieznane_pole(long nr_linii,char *nazwa_pola)
{
	char *temp1,*temp2;

	temp1=bmdconf_itostr(nr_linii);
	temp2=bmdconf_strmerge("Blad w lini nr: ",temp1);
	bmdconf_itostr_free(temp1);
	temp1=bmdconf_strmerge(temp2,"; Nieznane pole \"");
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,nazwa_pola);
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,"\"\n");
	bmdconf_strmerge_free(temp2);
	return temp1;
}

/*UWAGA - funkcja zwalnia strig nazwa_sekcji, wiec nazwa_sekcji musi byc alokowany */
char* bmdconf_blad_braku_wymaganego_pola(long nr_linii,char *nazwa_sekcji, char *nazwa_pola)
{
	char *temp1,*temp2;

	temp1=bmdconf_itostr(nr_linii);
	temp2=bmdconf_strmerge("Blad w lini nr: ",temp1);
	bmdconf_itostr_free(temp1);
	temp1=bmdconf_strmerge(temp2,"; Brak wymaganego pola \"");
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,nazwa_pola);
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,"\" w sekcji \"");
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,nazwa_sekcji);
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,"\"; \n");
	bmdconf_strmerge_free(temp2);
	free(nazwa_sekcji);
	return temp1;
}

char* bmdconf_blad_niepoprawnej_sciezki(long nr_linii, char *path)
{
	char *temp1,*temp2;

	temp1=bmdconf_itostr(nr_linii);
	temp2=bmdconf_strmerge("Blad w lini nr: ",temp1);
	bmdconf_itostr_free(temp1);
	temp1=bmdconf_strmerge(temp2,"; Plik \"");
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,path);
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,"\" nie istnieje\n");
	bmdconf_strmerge_free(temp2);
	return temp1;
}


/*zwraca 1 , gdy jest plik,
	 0, gdy plik nie istnieje*/
long bmdconf_file_exists(char *path)
{
	FILE *plik=fopen(path,"r");
	/*plik nieistnieje*/
	/*if(errno==ENOENT)*/
	/** @bug cryptlib ustawia zmienna errno i przez to powyzsze porownanie nie daje pozadanych rezultatow */
	if(plik==NULL)
	{
		return 0;
	}
	else
	{
		if(plik!=NULL)
			fclose(plik);
		return 1;
	}
}


char* bmdconf_validate_config(bmd_conf* struktura)
{
	if(struktura->tryb==BMDCONF_RESTRICT)
	{
		return bmdconf_validate_config_restrict( &(struktura->dump), &(struktura->dict) );
	}
	else
	{
		return bmdconf_validate_config_tolerant( &(struktura->dump), &(struktura->dict) );
	}
}


/*zwraca NULL w przypadku powodzenia
	string z komunikatem bledu w przypadku niepowodzenia - zwrocony string trzeba przechwycic i zwolnic
							       bo jest dynamicznie alokowany*/
char* bmdconf_validate_config_restrict(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary)
{
	struct bmdconf_sekcja *slownik=*dictionary;
	struct bmdconf_mem_list *konfiguracja=*dumped;
	struct bmdconf_pole *wsk_pole;
	struct bmdconf_wzorzec *iterator, *wzor_dict;
	int subpatterns[30];
	long wynik_porownania;
	long ostatnia_linia_nr=0;
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
		temp1=bmdconf_strmerge("Plik konfiguracyjny nie okresla zadnych pol.","\n");
		return temp1;
	}
	/*jesli pusty slownik */
	if(slownik==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("Nie zaladowano zadnego slownika badz ladowany slownik byl pusty.","\n");
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
							return bmdconf_blad_braku_wymaganego_pola(konfiguracja->line_number,aktualna_sekcja,temp_pole->nazwa_pola);
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
			return bmdconf_blad_nieznane_pole(konfiguracja->line_number,konfiguracja->option);
		}
		else
		{
			bmdconf_init_iterator_wzorzec(&(wsk_pole->wz),&iterator);
			wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
			while(wzor_dict!=NULL)
			{
				wynik_porownania=pcre_exec(wzor_dict->wzor,NULL,konfiguracja->value,(int)strlen(konfiguracja->value),0,0,subpatterns,30);
				if(wzor_dict->zgodnosc==1)
				{
					/*w przypadku niedopoasowania, gdy wymagane dopasowanie, zwraca blad*/
					if(wynik_porownania<=0)
					{
						/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
						bmdconf_niszcz_liste(&pola_req);
						free(aktualna_sekcja);
						return bmdconf_blad_niedozwolona_wartosc(konfiguracja->line_number,konfiguracja->option);
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
						return bmdconf_blad_niedozwolona_wartosc(konfiguracja->line_number,konfiguracja->option);
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
					return bmdconf_blad_niepoprawnej_sciezki(konfiguracja->line_number,konfiguracja->value);
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
				return bmdconf_blad_braku_wymaganego_pola(ostatnia_linia_nr,aktualna_sekcja,temp_pole->nazwa_pola);
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
				temp1=bmdconf_strmerge("Brak wymaganej sekcji \"",temp_dict->nazwa_sekcji);
				temp2=bmdconf_strmerge(temp1,"\" w sprawdzanym pliku.\n");
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


/*zwraca NULL w przypadku powodzenia
	string z komunikatem bledu w przypadku niepowodzenia - zwrocony string trzeba przechwycic i zwolnic
							       bo jest dynamicznie alokowany*/
char* bmdconf_validate_config_tolerant(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary)
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
		temp1=bmdconf_strmerge("Plik konfiguracyjny nie okresla zadnych pol.","\n");
		return temp1;
	}
	/*jesli pusty slownik */
	if(slownik==NULL)
	{
		/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
		temp1=bmdconf_strmerge("Nie zaladowano zadnego slownika badz ladowany slownik byl pusty.","\n");
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
							return bmdconf_blad_braku_wymaganego_pola(konfiguracja->line_number,aktualna_sekcja,temp_pole->nazwa_pola);
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
				return bmdconf_blad_nieznane_pole(konfiguracja->line_number,konfiguracja->option);
			}
			else
			{
				bmdconf_init_iterator_wzorzec(&(wsk_pole->wz),&iterator);
				wzor_dict=bmdconf_get_nextitem_wzorzec(&iterator);
				while(wzor_dict!=NULL)
				{
					wynik_porownania=pcre_exec(wzor_dict->wzor,NULL,konfiguracja->value,(int)strlen(konfiguracja->value),0,0,subpatterns,30);
					if(wzor_dict->zgodnosc==1)
					{
						/*w przypadku niedopoasowania, gdy wymagane dopasowanie, zwraca blad*/
						if(wynik_porownania<=0)
						{
							/*valus*/bmdconf_niszcz_liste(&napotkane_sekcje);
							bmdconf_niszcz_liste(&pola_req);
							free(aktualna_sekcja);
							return bmdconf_blad_niedozwolona_wartosc(konfiguracja->line_number,konfiguracja->option);
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
							return bmdconf_blad_niedozwolona_wartosc(konfiguracja->line_number,konfiguracja->option);
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
						return bmdconf_blad_niepoprawnej_sciezki(konfiguracja->line_number,konfiguracja->value);
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
				return bmdconf_blad_braku_wymaganego_pola(ostatnia_linia_nr,aktualna_sekcja,temp_pole->nazwa_pola);
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
				temp1=bmdconf_strmerge("Brak wymaganej sekcji \"",temp_dict->nazwa_sekcji);
				temp2=bmdconf_strmerge(temp1,"\" w sprawdzanym pliku.\n");
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

