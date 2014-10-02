#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>


/*0 jesli zmienil, -1 jesli nie znalazl tego co mialo byc zmienione*/
long bmdconf_update_option_value( bmd_conf *struktura, char *section, char *option, char *new_value)
{
	if (!(struktura != NULL ))
	{
		PRINT_ERROR("struktura nie może być NULL!(WS)\n");
		abort();
	}

	if (!(section != NULL))
	{
		PRINT_ERROR("section nie może być NULL!(WS)\n");
		abort();
	}

	if (!(option != NULL))
	{
		PRINT_ERROR("option nie może być NULL!(WS)\n");
		abort();
	}

	if (!(new_value != NULL))
	{
		PRINT_ERROR("new_value nie może być NULL!(WS)\n");
		abort();
	}

	if(contain_quotation(new_value)==0)
	{
		return -1;
	}

	return bmdconf_update_list_item( struktura->dump, section, option, new_value);
}

/*przekopiowuje zawartosc pliku o nazwie src do otwartego pliku dest
 0 jesli ok.
 -1  w przypadku niepowodzenia (np. otwarty plik do zapisu bez prawa do pisania, badz otwary tylko do odczytu)*/
long bmdconf_copy_file(	char *src,
				FILE* dest)
{
FILE *file_src	= NULL;
long read_bytes	= 0;
char buf[8];

	if (!(src != NULL))
	{
		PRINT_ERROR("Plik źródłowy nie może być NULL!(WS)\n");
		abort();
	}

	if (!(dest != NULL))
	{
		PRINT_ERROR("Plik docelowy nie może być NULL!(WS)\n");
		abort();
	}

	/*plik nie moze byc otwarty w trybie binarnym (b),
	 bo plik tymczasowy bedzie zawieral krzaki i nie mozna zrobic rename*/
	file_src=fopen(src, "r");
	if(file_src==NULL)
	{
		BMD_FOK(LIBBMDCONFIG_UPDATE_FILE_OPEN_SRC_FILE_ERR);
	}

	while( (read_bytes=fread(buf, sizeof(char), 8, file_src)) != 0)
	{
		if(fwrite(buf, sizeof(char), read_bytes, dest) != read_bytes)
		{
			fclose(file_src);
			BMD_FOK(LIBBMDCONFIG_UPDATE_FILE_WRITE_DEST_FILE_ERR);
		}
	}
	fclose(file_src);

	return BMD_OK;
}

/*
dla podanej lini z assignmentem wywmienia wartosc ujeta w cudzyslow
zwraca 0, gdy wstawianie zakonczone sukcesem
 -1 , gdy niedopasowanie do wzorca i brak przeprowadzenia wstawiania
 result to nowo zaalokowany string - wynik operacji wstawiania
 dla zwroconego -1, result=NULL*/
long bmdconf_change_value(char *assign, char* val, char **result)
{
	long i=0, temp=0;
	const char *error_pointer=NULL;
	int error_offset;
	int subpatterns[9];
	char *wzorzec="(\").*?(\")";
	long len_assign=strlen(assign);
	long len_val=strlen(val);
	pcre *reg=pcre_compile(wzorzec,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	if(pcre_exec(reg,NULL,assign,len_assign,0,0,subpatterns,9) > 0)
	{
		*result=(char*)malloc((len_assign+len_val+1)*sizeof(char));
		for(i=0;i<subpatterns[3];i++)
		{
			(*result)[i]=assign[i];
		}
		temp=i;
		for(i=0;i<len_val;i++)
		{
			(*result)[temp+i]=val[i];
		}
		for(i=0;i<len_assign-subpatterns[4];i++)
		{
			(*result)[temp+len_val+i]=assign[subpatterns[4]+i];
		}
		(*result)[temp+len_val+i]='\0';
		pcre_free(reg);
		return 0;
	}
	else
	{
		pcre_free(reg);
		*result=NULL;
		return -1;
	}

}

/*tworzy kopie bezpieczenstwa pliku konfiguracyjnego, aby w przypadku jakiegos bledu i wymazania
	nie stracic jego zawartosci
file_src to otwarty plik zrodlowy
@RETURN zaalokowany string z nazwa pliku backup'a
	NULL jesli backup nie moze zostac stworzony
*/
/*
char* bmdconf_make_backup(char *file_path, FILE *file_src)
{
	long num=0;
	char *new_name=NULL;
	struct stat buf_stat;
	FILE *backup=NULL;
	long read_bytes=0;
	char buf[8];


	//gdyby 1000 razy sie nie udalo dobrac odpowiedniej nazwy, to niech odpusci...nie moze tkwic w petli nieskonczonej
	for(num=0; num<1000; num++)
	{
		asprintf(&new_name, "%s_bu%i", file_path, num );
		//jesli nie ma takiego pliku
		if( stat(new_name, &buf_stat) != 0 )
		{
			//plik backupu tworzony i otwierany do zapisu
			backup=fopen(new_name, "wb");
			//jesli otwarty
			if( backup != NULL )
			{
				//ustawienie na poczatku pliku zrodlowego
				rewind(file_src);
				//kopiowanie zawartosci do backupu
				while( (read_bytes=fread(buf, sizeof(char), 8, file_src)) != 0)
				{
					//w razie problemow z zapisem rezygnacja z robienia backupu
					if(fwrite(buf, sizeof(char), read_bytes, backup) != read_bytes)
					{
						fclose(backup);
						remove(new_name);
						free(new_name);
						return NULL;
					}
				}
			}
			//jesli nie udalo sie utworzyc pliku do zapisu, to nic z tego nie bedzie
			else
			{
				free(new_name);
				return NULL;
			}

			fclose(backup);
			return new_name;
		}
		free(new_name);
		new_name=NULL;
	}
	free(new_name);
	return NULL;
}
*/

/*zapisywanie do pliku konfiguracyjnego
docelowy plik konfiguracyjny jest parsowany (w tym przypadku pomija sie sprawdzanie powtorzen opcji w sekcji oraz powtorzen sekcji);
jesli wykazany zostanie blad, to nie bedzie mozna naniesc zmian*/
long bmdconf_save_configuration( bmd_conf *struktura, char **error )
{
	const char *error_pointer=NULL;
	int error_offset=0;
	int subpatterns[20];
	pcre *sekcja_regex=NULL, *assign_regex=NULL, *komentarz_regex=NULL;
	char *wzorzec_sekcja="^\\[([_a-zA-Z][_0-9a-zA-Z]*?)\\]\\s*(#.*)*$";
	char *wzorzec_assign="^([_a-zA-Z][_0-9a-zA-Z]*)\\s*=\\s*\"([^\"]*?)\"\\s*(#.*)*$";
	char *wzorzec_komentarz="^\\s*(#.*)*$";

	FILE *original=NULL;
	FILE *config_copy=NULL;
	char *name=NULL;
	char *wiersz=NULL;
	char *aktualna_sekcja=NULL;
	char *aktualna_opcja=NULL;
	char *aktualna_wartosc=NULL;
	char *wartosc_cmp=NULL;
	char *zmieniona_linia=NULL;
	char *tmp=NULL;

	if (!(struktura != NULL))
	{
		PRINT_ERROR("Struktura nie może być NULL!(WS)\n");
		abort();
	}
	if (!(error != NULL))
	{
		PRINT_ERROR("error nie może być NULL!(WS)\n");
		abort();
	}
	/*jesli omylkowo ktos podalby strukture bmd_conf wypelniona funkcja bmdconf_load_mem, to asercja*/
	if (!(struktura->file_name != NULL))
	{
		PRINT_ERROR("Struktura nie może być wcześniej wypełniona!(WS)\n");
		abort();
	}

	*error=NULL;

	sekcja_regex=pcre_compile(wzorzec_sekcja,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	assign_regex=pcre_compile(wzorzec_assign,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	komentarz_regex=pcre_compile(wzorzec_komentarz,PCRE_UTF8,&error_pointer,&error_offset,NULL);

	/*tworzenie pliku tymczasowego, do ktorego zrzucona bedzie zawartosc pliku konfiguracyjnego*/
	name=tmpnam(NULL);
	/*jesli nie udalo sie wygenerowac unikatowej nazwy, to blad*/
	if(name==NULL)
	{
		*error=strdup("ERROR: Nie udalo sie wygenerowac pliku tymczasowego.\n");
		pcre_free(sekcja_regex);
		pcre_free(assign_regex);
		pcre_free(komentarz_regex);
		return -1;
	}

	config_copy=fopen(name, "w");
	if(config_copy==NULL)
	{
		*error=strdup("ERROR: Nie mozna utworzyc pliku tymczasowego.\n");
		pcre_free(sekcja_regex);
		pcre_free(assign_regex);
		pcre_free(komentarz_regex);
		return -1;
	}
	/*kopiowanie zawartosci pliku konfiguracyjnego do pliku tymczasowego*/
	if( bmdconf_copy_file(struktura->file_name, config_copy) != 0)
	{
		*error=strdup("ERROR: Nie mozna skopiowac pliku konfiguracyjnego.\n");
		fclose(config_copy);
		remove(name);
		pcre_free(sekcja_regex);
		pcre_free(assign_regex);
		pcre_free(komentarz_regex);
		return -1;
	}

	/*czyszczenie zawartosci pliku konfiguracyjnego (otwarty do zapisu)*/
	original=fopen(struktura->file_name, "w");
	/*jesli nie udalo sie "wyzerowac" pliku*/
	if(original==NULL)
	{
		*error=strdup("ERROR: Nie mozna zmodyfikowac pliku konfiguracyjnego.\n");
		fclose(config_copy);
		remove(name);
		pcre_free(sekcja_regex);
		pcre_free(assign_regex);
		pcre_free(komentarz_regex);
		return -1;
	}

	/*zapamietanie zawartosci pliku konfiguracyjnego i ustawienie na poczatek pliku (tymczasowego)*/
	/*FIXED po wpisywaniu do pliku, do momentu, kiedy fclose nie zostanie wywolany, plik jest trakotwany jak pusty
		dlatego aby odczytywac jego zawartosc, nalezy najpierw wywolac fclose i jeszcze raz fopen do odczytu */
	fclose(config_copy);
	config_copy=fopen(name, "r");

	/*kopiowanie zawartosci pliku tymczasowego do pliku konfiguracyjnego z uwzglednieniem modyfikacji
	 wprowadzonych na strukturze bmd_conf*/
	while( (wiersz=bmdconf_wiersz_pliku(config_copy)) != NULL)
	{
		/*dla sekcji*/
		if( pcre_exec(sekcja_regex,NULL,wiersz,strlen(wiersz),0,0,subpatterns,20) > 0 )
		{
			if(aktualna_sekcja!=NULL)
			{
				free(aktualna_sekcja);
			}
			aktualna_sekcja=bmdconf_substr(wiersz, subpatterns[2], subpatterns[3]);
		}
		else
		{
			/* dla assigna*/
			if( pcre_exec(assign_regex,NULL,wiersz,strlen(wiersz),0,0,subpatterns,20) > 0 )
			{
				/*proste zabezpieczenie na wypadek, gdyby ktos namotal miedzy wczytaniem pliku
				 konfiguracyjnego a zapisem nowej konfiguracji
				 (definiowanie wartosci pol bez wczesniejszej deklaracji sekcji)*/
				if(aktualna_sekcja == NULL)
				{
					free(wiersz);
					*error=strdup("ERROR: Nieprawidlowa struktura docelowego pliku konfiguracyjnego.\n");
					/*usuniecie oryginalu*/
					fclose(original);
					remove(struktura->file_name);
					/*tymczasowy zastepuje oryginal (rename)*/
					fclose(config_copy);
					rename(name, struktura->file_name);
					pcre_free(sekcja_regex);
					pcre_free(assign_regex);
					pcre_free(komentarz_regex);
					return -1;
				}
				aktualna_opcja=bmdconf_substr(wiersz, subpatterns[2], subpatterns[3]);
				aktualna_wartosc=bmdconf_substr(wiersz, subpatterns[4], subpatterns[5]);
				/*wartosc aktualnej opcji dla aktualnej sekcji, ale ta w strukturze bmd_conf
					wartosc ta mogla byc zmodyfikowana
				 wartosc_cmp jest tylko bindowana, nie zas dynamicznie alokowana*/
				wartosc_cmp=bmdconf_find_list_item( &(struktura->dump),aktualna_sekcja, aktualna_opcja);
				/*jesli znaleziona taka opcja w danej sekcji*/
				if(wartosc_cmp != NULL)
				{
					/*jesli wartosci w pliku i dumpie roznia sie, to zmiana w tej lini*/
					if( strcmp(aktualna_wartosc, wartosc_cmp) != 0)
					{
						bmdconf_change_value(wiersz, wartosc_cmp, &zmieniona_linia);
						free(wiersz);
						wiersz=zmieniona_linia;
					}
				}
				free(aktualna_opcja);
				free(aktualna_wartosc);
			}
			/*komentarze i linie puste nie sa poddawane dodatkowym operacjom
			 jesli cos innego wystapi, to ktos gmeral przy pliku i trzeba zglosic blad */
			else
			{
				if( !(pcre_exec(komentarz_regex,NULL,wiersz,strlen(wiersz),0,0,subpatterns,20)>0
					|| strcmp(wiersz, "\n")==0) )
				{
					free(aktualna_sekcja);
					free(wiersz);
					*error=strdup("ERROR: Nieprawidlowy docelowy plik konfiguracyjny.\n");
					/*usuniecie oryginalu*/
					fclose(original);
					remove(struktura->file_name);
					/*tymczasowy zastepuje oryginal (rename)*/
					fclose(config_copy);
					rename(name, struktura->file_name);
					pcre_free(sekcja_regex);
					pcre_free(assign_regex);
					pcre_free(komentarz_regex);
					return -1;
				}
			}
		}

		if( strcmp(wiersz, "\n") != 0 )
		{
			tmp=wiersz;
			wiersz=bmdconf_strmerge(tmp, "\n");
			free(tmp);
		}
		/*jesli nie mozna zapisac lini pliku*/
		if( (fputs(wiersz, original)) == EOF )
		{
			free(aktualna_sekcja);
			*error=strdup("ERROR: Nie mozna wprowadzic modyfikacji do pliku konfiguracyjnego.\n");
			free(wiersz);
			pcre_free(sekcja_regex);
			pcre_free(assign_regex);
			pcre_free(komentarz_regex);
			/*usuwam oryginal*/
			fclose(original);
			remove(struktura->file_name);
			/*tymczasowy zastepuje oryginal (rename)*/
			fclose(config_copy);
			rename(name, struktura->file_name);
			return -1;
		}
		free(wiersz);
	}

	free(aktualna_sekcja);
	pcre_free(sekcja_regex);
	pcre_free(assign_regex);
	pcre_free(komentarz_regex);

	fclose(original);
	/*usuwanie tymczasowego*/
	fclose(config_copy);
	remove(name);
	return 0;
}
