#include <bmd/libbmdconfig/libbmdconfig.h>

/*zwrocony string jest alokowany, wiec po wykorzystaniu musi byc zwolniony*/
char *bmdconf_komunikat_bledu_general(char *nazwa_sekcji,long index)
{
	char *temp1,*temp2,*temp3;

	temp3=bmdconf_itostr(index);
	temp1=bmdconf_strmerge("Blad w slowniku \"",nazwa_sekcji);
	temp2=bmdconf_strmerge(temp1,"\" - index blednego wpisu: ");
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,temp3);
	bmdconf_itostr_free(temp3);
	bmdconf_strmerge_free(temp2);
	temp2=bmdconf_strmerge(temp1,"; \n");
	bmdconf_strmerge_free(temp1);
	return temp2;
}

/*zwrocony string jest alokowany, wiec po wykorzystaniu musi byc zwolniony*/
char *bmdconf_komunikat_bledu_wzorca(char *nazwa_sekcji,long index,long nr_wzorca)
{
	char *temp1, *temp2, *temp3;
	temp1=bmdconf_strmerge("Blad w slowniku \"",nazwa_sekcji);
	temp2=bmdconf_strmerge(temp1,"\" - index blednego wpisu: ");
	bmdconf_strmerge_free(temp1);
	/*index blednego wpisu*/
	temp3=bmdconf_itostr(index);
	temp1=bmdconf_strmerge(temp2,temp3);
	bmdconf_itostr_free(temp3);
	bmdconf_strmerge_free(temp2);
	/**/
	temp2=bmdconf_strmerge(temp1,"; niepoprawny wzorzec nr: ");
	bmdconf_strmerge_free(temp1);
	/*nr niepoprawnego wzorca*/
	temp3=bmdconf_itostr(nr_wzorca);
	temp1=bmdconf_strmerge(temp2,temp3);
	bmdconf_itostr_free(temp3);
	bmdconf_strmerge_free(temp2);
	/**/
	temp2=bmdconf_strmerge(temp1,"; \n");
	bmdconf_strmerge_free(temp1);
	return temp2;
}

/*zwrocony string jest alokowany, wiec po wykorzystaniu musi byc zwolniony
funckja uzywana w przypadku powtorzonego pola w slowniku*/
char *bmdconf_komunikat_bledu_pola(char *nazwa_sekcji,long index,char *nazwa_pola)
{
	char *temp1, *temp2, *temp3;
	temp1=bmdconf_strmerge("Blad w slowniku \"",nazwa_sekcji);
	temp2=bmdconf_strmerge(temp1,"\" - index blednego wpisu: ");
	bmdconf_strmerge_free(temp1);
	/*index blednego wpisu*/
	temp3=bmdconf_itostr(index);
	temp1=bmdconf_strmerge(temp2,temp3);
	bmdconf_itostr_free(temp3);
	bmdconf_strmerge_free(temp2);
	/**/
	temp2=bmdconf_strmerge(temp1,"; powtorzone pole \"");
	bmdconf_strmerge_free(temp1);
	temp1=bmdconf_strmerge(temp2,nazwa_pola);
	bmdconf_strmerge_free(temp2);
	bmdconf_strmerge_free(nazwa_pola); /*nazwa_pola jest przkazywana jako string zaalokowany i trzeba go zwolnic*/
	temp2=bmdconf_strmerge(temp1,"\" \n");
	bmdconf_strmerge_free(temp1);
	return temp2;
}

void bmdconf_init_dictionary_config(struct bmdconf_sekcja **dictionary)
{
	bmdconf_init_sekcjalist(dictionary);
}

void bmdconf_delete_dictionary_config(struct bmdconf_sekcja **dictionary)
{
	bmdconf_niszcz_sekcjalist(dictionary);
}


/*
dictionary to adres wskaznika do listy sekwencji (w rzeczywistosci sekwencje-pola-wzorce)
section_name to nazwa sekcji
registers to tabica stringow - wpisow slownikowych
reg_number to ilosc stringow w tablicy registers
*/
/*w przypadk bledu niszczy cala zawartosc hasha slownika lacznie z wczesniej zaladowanymi*/
/*@added long czy_wymagana*/
char* bmdconf_load_dictionary_config(struct bmdconf_sekcja **dictionary,char *section_name,char **registers, long czy_wymagana)
{
	long i, index, wynik_porownania, liczba_wzorcow;
	char *ile_wzorcow;
	pcre *wzorzec_compiled;
	const char *error_pointer;
	int error_offset;
	int *subpatterns_pelny; /*subpatterns_begi dla dopasowania  3 substringow- REQ,pole,ilosc wzorcow*/
	int subpatterns_begin[15];
	char *wzorzec_begin="^\\s*(\\%?)(#?)([_a-zA-Z][_0-9a-zA-Z]*)\\s+(\\d+)";
	char *wzorzec_pelny,*wzorzec_temp;
	char *nazwa_pola, *wzorzec_slownikowy;
	struct bmdconf_pole *aktualne_pole;

	index=0;
	while(registers[index]!=NULL && registers[index][0]!='\0')
	{
		/*wstepny parsing lini slownika nazwa pola i ilosc wzorcow*/
		wzorzec_compiled=pcre_compile(wzorzec_begin,PCRE_UTF8,&error_pointer,&error_offset,NULL);
		wynik_porownania=pcre_exec(wzorzec_compiled,NULL,registers[index],(int)strlen(registers[index]),0,0,subpatterns_begin,15);
		pcre_free(wzorzec_compiled);wzorzec_compiled=NULL;
		wzorzec_pelny=(char*)malloc((strlen(wzorzec_begin)+1)*sizeof(char));
		strcpy(wzorzec_pelny,wzorzec_begin);
		if(wynik_porownania>0)
		{
			
			ile_wzorcow=bmdconf_substr(registers[index],subpatterns_begin[8],subpatterns_begin[9]);
			liczba_wzorcow=atoi(ile_wzorcow);
			bmdconf_substr_free(ile_wzorcow);
			/*printf("liczba wzorcow %i\n",liczba_wzorcow);*/
			/*budowanie pelnego wzorca*/
			for(i=0;i<liczba_wzorcow;i++)
			{
				wzorzec_temp=bmdconf_strmerge(wzorzec_pelny,"\\s+(#?)\"([^\"]+?)\"");
				free(wzorzec_pelny);
				wzorzec_pelny=(char*)malloc((strlen(wzorzec_temp)+1)*sizeof(char));
				strcpy(wzorzec_pelny,wzorzec_temp);
				bmdconf_strmerge_free(wzorzec_temp);
			}
			wzorzec_temp=bmdconf_strmerge(wzorzec_pelny,"\\s*"); /*! wyrzucone z konca '$' */
			free(wzorzec_pelny);
			wzorzec_pelny=(char*)malloc((strlen(wzorzec_temp)+1)*sizeof(char));
			strcpy(wzorzec_pelny,wzorzec_temp);
			bmdconf_strmerge_free(wzorzec_temp);
			subpatterns_pelny=(int*)malloc((6*liczba_wzorcow+15)*sizeof(int));
			
			wzorzec_compiled=pcre_compile(wzorzec_pelny,PCRE_UTF8,&error_pointer,&error_offset,NULL);
			wynik_porownania=pcre_exec(wzorzec_compiled,NULL,registers[index],(int)strlen(registers[index]),0,0,subpatterns_pelny,6*liczba_wzorcow+15);
			pcre_free(wzorzec_compiled);wzorzec_compiled=NULL;
			/*pelny parsing wpisu slownikowego*/
			if(wynik_porownania>0)
			{
				nazwa_pola=bmdconf_substr(registers[index],subpatterns_pelny[6],subpatterns_pelny[7]);
				/*pole wymagane*/
				if(subpatterns_pelny[4]==subpatterns_pelny[5])
				{
					if(subpatterns_pelny[2]==subpatterns_pelny[3])
						aktualne_pole=bmdconf_dodaj_sekcje_pole_hash(dictionary,section_name,nazwa_pola,1,0, czy_wymagana);
					else
						aktualne_pole=bmdconf_dodaj_sekcje_pole_hash(dictionary,section_name,nazwa_pola,1,1, czy_wymagana);
				}
				/*pole opcjonalne*/
				else
				{
					if(subpatterns_pelny[2]==subpatterns_pelny[3])
						aktualne_pole=bmdconf_dodaj_sekcje_pole_hash(dictionary,section_name,nazwa_pola,0,0, czy_wymagana);
					else
						aktualne_pole=bmdconf_dodaj_sekcje_pole_hash(dictionary,section_name,nazwa_pola,0,1, czy_wymagana);
				}
				if(aktualne_pole==NULL)
				{
					free(subpatterns_pelny);
					free(wzorzec_pelny);
					bmdconf_niszcz_sekcjalist(dictionary);
					return bmdconf_komunikat_bledu_pola(section_name,index,nazwa_pola);
				}
				for(i=10;i<(10+liczba_wzorcow*4);i+=4)
				{
					wzorzec_slownikowy=bmdconf_substr(registers[index],subpatterns_pelny[i+2],subpatterns_pelny[i+3]);
					wzorzec_compiled=pcre_compile(wzorzec_slownikowy,PCRE_UTF8,&error_pointer,&error_offset,NULL);
					/*jesli wzorzec jest poprawny*/
					if(wzorzec_compiled!=NULL)
					{
						/*wymagane dopoasowanie do wzorca*/
						if(subpatterns_pelny[i]==subpatterns_pelny[i+1])
							bmdconf_dodaj_wzorzec_pola_hash(aktualne_pole,wzorzec_compiled,1);
						/*wymagany brak dopasowana do wzorca*/
						else
							bmdconf_dodaj_wzorzec_pola_hash(aktualne_pole,wzorzec_compiled,0);
					}
					else
					{
						bmdconf_substr_free(wzorzec_slownikowy);
						free(subpatterns_pelny);
						free(wzorzec_pelny);
						bmdconf_substr_free(nazwa_pola);
						bmdconf_niszcz_sekcjalist(dictionary);
						return bmdconf_komunikat_bledu_wzorca(section_name,index,(int)( (i-10)/4 ));
					}
					bmdconf_substr_free(wzorzec_slownikowy);
				}
				bmdconf_substr_free(nazwa_pola);
				free(subpatterns_pelny);
				free(wzorzec_pelny);
			}
			else
			{
				free(subpatterns_pelny);
				free(wzorzec_pelny);
				bmdconf_niszcz_sekcjalist(dictionary);
				return bmdconf_komunikat_bledu_general(section_name,index);
			}
			
		}
		else
		{
			free(wzorzec_pelny);
			bmdconf_niszcz_sekcjalist(dictionary);
			return bmdconf_komunikat_bledu_general(section_name,index);
		}
	index++;
	}
	return NULL;
}

