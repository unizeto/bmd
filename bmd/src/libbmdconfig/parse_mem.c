#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

/*	pobieranie wiersza pliku konfiguracyjnego (plik zrzuczony w pamieci)
	funkcja zwraca:
	 kolejna linie pliku gdy sukces
	 (dla pustej lini zwraca "\n")
	 NULL, jesli EOF */
char* bmdconf_wiersz_mem(char *buf,long length,long *read_index)
{
	long rozmiar;
	char *temp;
	char znak[2];
	char *buff=(char*)malloc(2*sizeof(char));
	strcpy(znak," \0");
	strcpy(buff," \0");
	/*jesli na dzien dobry koniec pliku (w pamieci)*/
	if( (*read_index)>=length)
	{
		free(buff);
		return NULL;
	}

	znak[0]=buf[(*read_index)++];
	buff[0]=znak[0];
	/*jesli pusta linia*/
	if(strcmp(buff,"\n")==0)
		return buff;
	
	/*jesli koniec pliku*/
	if( (*read_index)>=length )
	{
		return buff;
	}
	while((znak[0]=buf[(*read_index)++])!='\n')
	{
		rozmiar=(int)strlen(buff);
		temp=(char*)malloc((rozmiar+1)*sizeof(char));
		strcpy(temp,buff);
		free(buff);
		/*tu zmiana*/
		buff=bmdconf_strmerge(temp,znak);
		free(temp);
		/*jesli zlapie EOF w trakcie odczytu lini*/
		if( (*read_index)>=length )
		{
			break;
		}
	}
	return buff;
}

/*zwraca NULL - ok
	 inaczej komunkat błedu- niezgodny*/
char* bmdconf_read_and_parse_config_mem(char *buf,long length,struct bmdconf_mem_list **dumped )
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
	long read_index=0;
	bmdconf_init_list(dumped);
	/*wozrzec_sekcja - do sprawdzania poprawnosci dekaracji sekcji*/
	sekcja_regex=pcre_compile(wzorzec_sekcja,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	/*wzorzec_assign - do sprawdzenia poprawnosci przpisania wartosci do pola*/
	assign_regex=pcre_compile(wzorzec_assign,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	/*wzorzez_komentarz - do sprawdzenia poprawnośći pustej lini, komentarza*/
	komentarz_regex=pcre_compile(wzorzec_komentarz,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	bmdconf_init_lista(&sekcje);
	while( (wiersz=bmdconf_wiersz_mem(buf,length,&read_index))!=NULL )
	{
		/*printf("wiersz : |%s| and read_index %i\n",wiersz,read_index);*/
		/*jesli linia pusta "\n"*/
		if(strcmp(wiersz,"\n")==0)
		{
		    /*printf("tutaj\n");*/
		    nr_wiersza++;
		    free(wiersz);
		    continue;
		}
		
		/*jesli deklaracja sekcji*/
		if( pcre_exec(sekcja_regex,NULL,wiersz,(int)strlen(wiersz),0,0,subpatterns,20) > 0 )
		{
			/*printf("sekcja\n");*/
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
				pcre_free(sekcja_regex);sekcja_regex=NULL;
				pcre_free(assign_regex);assign_regex=NULL;
				pcre_free(komentarz_regex);komentarz_regex=NULL;
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
					pcre_free(sekcja_regex);sekcja_regex=NULL;
					pcre_free(assign_regex);assign_regex=NULL;
					pcre_free(komentarz_regex);komentarz_regex=NULL;
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
					pcre_free(sekcja_regex);sekcja_regex=NULL;
					pcre_free(assign_regex);assign_regex=NULL;
					pcre_free(komentarz_regex);komentarz_regex=NULL;
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
					pcre_free(sekcja_regex);sekcja_regex=NULL;
					pcre_free(assign_regex);assign_regex=NULL;
					pcre_free(komentarz_regex);komentarz_regex=NULL;
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
	pcre_free(sekcja_regex);sekcja_regex=NULL;
	pcre_free(assign_regex);assign_regex=NULL;
	pcre_free(komentarz_regex);komentarz_regex=NULL;
	return NULL;
}

/* WYSOKOPOZIOMOWA FUNKCJA laduje blok pamieci do sparsowania jako plik konfiguracyjny */
long bmdconf_load_memory(char *buf,long length,bmd_conf *struktura,char **error_string)
{
char *ret = NULL;

	if (!(buf != NULL))
	{
		PRINT_ERROR("buf nie może być NULL!(WS)\n");
		abort();
	}
	
	if (!(struktura != NULL))
	{
		PRINT_ERROR("struktura nie może być NULL!(WS)\n");
		abort();
	}
	if (error_string != NULL)
		*error_string=NULL;
	ret=bmdconf_read_and_parse_config_mem(buf,length,&(struktura->dump));
	if (ret!=NULL) 
	{
		if(error_string != NULL)
		{ 
			*error_string = ret;
		}
		else
		{
			PRINT_ERROR("Blad: %s\n", ret);
			free(ret);
		}
		return -2; /*blad przy parsingu pliku konfiguracyjnego*/
	}

	ret=bmdconf_validate_config(struktura);
	if(ret!=NULL)
	{
		if(error_string != NULL)
		{
			*error_string = ret;
		}
		else
		{
			PRINT_ERROR("Blad: %s\n", ret);
			free(ret);
		}
		bmdconf_delete_list(&(struktura->dump)); /*niszczenie dumpa pliku konfiguracyjnego , gdy niepowodzenie walidacji*/
		return -3; /*blad przy alidacji pliku konfigracyjnego*/
	}
	/*sukces*/
	return 0;
}
