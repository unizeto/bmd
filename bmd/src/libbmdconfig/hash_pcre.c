#include <bmd/libbmdconfig/libbmdconfig.h>




void bmdconf_init_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy)
{
	*nazwa_listy=NULL;
}

void bmdconf_dodaj_element_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy,pcre *compiled,long czy_zgodny)
{
	struct bmdconf_wzorzec *nowy, *temp;
	if((*nazwa_listy)==NULL)
	{
		nowy=(struct bmdconf_wzorzec*)malloc(sizeof(struct bmdconf_wzorzec));
		*nazwa_listy=nowy;
		nowy->wzor=compiled;
		nowy->zgodnosc=czy_zgodny;
		nowy->next=NULL;
	}
	else
	{
		nowy=(struct bmdconf_wzorzec*)malloc(sizeof(struct bmdconf_wzorzec));
		nowy->wzor=compiled;
		nowy->zgodnosc=czy_zgodny;
		nowy->next=NULL;
		temp=*nazwa_listy;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=nowy;
	}
}

void bmdconf_niszcz_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy)
{
	struct bmdconf_wzorzec *temp,*t;
	t=*nazwa_listy;
	if(t!=NULL)
	{
		while(t->next!=NULL)
		{
			temp=t->next;
			pcre_free(t->wzor);
			free(t);
			t=temp;
		}
		pcre_free(t->wzor);
		free(t);
		*nazwa_listy=NULL;
	}
}

/*--------------------------------------------------------------------------------------------------------------*/
/*nazwa_listy to wskaznik na pierwszy elemet listy; iterator to adres wskaznika przesuwanego po kolejnych polach listy wzorcow (przez funkcje get_item)*/
void bmdconf_init_iterator_wzorzec(struct bmdconf_wzorzec **nazwa_listy,struct bmdconf_wzorzec **iterator)
{
	*iterator=*nazwa_listy;
}

/*iterator to adres wskaznika_iteraora
zwraca wzorzec i ustawia iterator na kolejne pole
w przypadku konca listy wzorcow zwraca NULL*/
struct bmdconf_wzorzec* bmdconf_get_nextitem_wzorzec(struct bmdconf_wzorzec **iterator)
{
	struct bmdconf_wzorzec *temp=*iterator;
	if(temp!=NULL)
	{
		*iterator=temp->next;
		return temp;
	}
	else
		return NULL;
}
/*---------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/
/*struktura opcja i jej "interfejs" */


void bmdconf_init_polelist(struct bmdconf_pole **nazwa_listy)
{
	*nazwa_listy=NULL;
}

/**/
struct bmdconf_pole* bmdconf_dodaj_element_polelist(struct bmdconf_pole **nazwa_listy,char *str,long czy_wymagane,long czy_plik)
{
	struct bmdconf_pole *nowy, *temp;
	if( (*nazwa_listy)==NULL)
	{
		nowy=(struct bmdconf_pole*)malloc(sizeof(struct bmdconf_pole));
		*nazwa_listy=nowy;
		nowy->nazwa_pola=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->nazwa_pola,str);
		nowy->pole_wymagane=czy_wymagane;
		nowy->flaga_plik=czy_plik;
		nowy->next=NULL;
		bmdconf_init_wzorzeclist(&(nowy->wz));
		return nowy;
	}
	else
	{
		nowy=(struct bmdconf_pole*)malloc(sizeof(struct bmdconf_pole));
		nowy->nazwa_pola=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->nazwa_pola,str);
		nowy->pole_wymagane=czy_wymagane;
		nowy->flaga_plik=czy_plik;
		nowy->next=NULL;
		bmdconf_init_wzorzeclist(&(nowy->wz));
		temp=*nazwa_listy;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=nowy;
		return nowy;
	}
}


void bmdconf_niszcz_polelist(struct bmdconf_pole **nazwa_listy)
{
	struct bmdconf_pole *t,*temp;
	t=*nazwa_listy;
	if(t!=NULL)
	{
		while(t->next!=NULL)
		{
			temp=t->next;
			free(t->nazwa_pola);
			bmdconf_niszcz_wzorzeclist(&(t->wz));
			free(t);
			t=temp;
		}
		free(t->nazwa_pola);
		bmdconf_niszcz_wzorzeclist(&(t->wz));
		free(t);
		*nazwa_listy=NULL;
	}
}

/*zwraca wskaznik pola, w ktorym znaleziono opcje str
	 NULL przy braku opcji w liscie*/
struct bmdconf_pole* bmdconf_znajdz_pole(struct bmdconf_pole **nazwa_listy,char *str)
{
	struct bmdconf_pole *temp;
	temp=*nazwa_listy;
	if(temp!=NULL)
	{
		while(temp->next!=NULL)
		{
			if( strcmp(str,temp->nazwa_pola)==0)
				return temp;
			else
				temp=temp->next;
		}
		if( strcmp(str,temp->nazwa_pola)==0)
			return temp;
		else
			return NULL;
	}
	else
		return NULL;
}

/*--------------------------------*/

void bmdconf_init_iterator_pole(struct bmdconf_pole **nazwa_listy,struct bmdconf_pole **iterator)
{
	*iterator=*nazwa_listy;
}

struct bmdconf_pole* bmdconf_get_nextitem_pole(struct bmdconf_pole **iterator)
{
	struct bmdconf_pole *temp=*iterator;
	if(temp!=NULL)
	{
		*iterator=temp->next;
		return temp;
	}
	else
		return NULL;
}

/*--------------------------------*/


void bmdconf_init_sekcjalist(struct bmdconf_sekcja **nazwa_listy)
{
	*nazwa_listy=NULL;
}

/*zwraca wskaznik do dodanego pola*/
/*@added w interfejsie long czy_wymagana*/
struct bmdconf_sekcja* bmdconf_dodaj_element_sekcjalist(struct bmdconf_sekcja **nazwa_listy,char *str, long czy_wymagana)
{
	struct bmdconf_sekcja *nowy, *temp;
	if( (*nazwa_listy)==NULL)
	{
		nowy=(struct bmdconf_sekcja*)malloc(sizeof(struct bmdconf_sekcja));
		*nazwa_listy=nowy;
		nowy->nazwa_sekcji=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->nazwa_sekcji,str);
		bmdconf_init_polelist(&(nowy->field));
		nowy->wymagana=czy_wymagana;
		nowy->next=NULL;
		return nowy;
	}
	else
	{
		nowy=(struct bmdconf_sekcja*)malloc(sizeof(struct bmdconf_sekcja));
		nowy->nazwa_sekcji=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->nazwa_sekcji,str);
		bmdconf_init_polelist(&(nowy->field));
		nowy->wymagana=czy_wymagana;
		nowy->next=NULL;
		temp=*nazwa_listy;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=nowy;
		return nowy;
	}
}

void bmdconf_niszcz_sekcjalist(struct bmdconf_sekcja **nazwa_listy)
{
	struct bmdconf_sekcja *t,*temp;
	t=*nazwa_listy;
	if(t!=NULL)
	{
		while(t->next!=NULL)
		{
			temp=t->next;
			free(t->nazwa_sekcji);
			bmdconf_niszcz_polelist(&(t->field));
			free(t);
			t=temp;
		}
		free(t->nazwa_sekcji);
		bmdconf_niszcz_polelist(&(t->field));
		free(t);
		*nazwa_listy=NULL;
	}
}


/*zwraca wskaznik pola, w ktorym znaleziono sekcje str
	 NULL przy braku sekcij w liscie*/
struct bmdconf_sekcja* bmdconf_znajdz_sekcje(struct bmdconf_sekcja **nazwa_listy,char *str)
{
	struct bmdconf_sekcja *temp=*nazwa_listy;
	if(temp!=NULL)
	{
		while(temp->next!=NULL)
		{
			if( strcmp(str,temp->nazwa_sekcji)==0)
				return temp;
			else
				temp=temp->next;
		}
		if( strcmp(str,temp->nazwa_sekcji)==0)
			return temp;
		else
			return NULL;
	}
	else
		return NULL;
}

/*sek_lista to adres wskaznika do listy sekcji
zwraca NULL, gdy powielone pole w slowniku
gdy ok, zwraca wskaznik do elementu listy zawierajacego pole*/
/*@added long czy_sek_wymagana*/
struct bmdconf_pole* bmdconf_dodaj_sekcje_pole_hash(struct bmdconf_sekcja **sek_lista,char *nazwa_sekcji,char *nazwa_pola,long czy_pole_wymagane,long czy_plik, long czy_sek_wymagana)
{
	struct bmdconf_sekcja *temp_sekcja;
	struct bmdconf_pole *temp_pole;
	/*jesli nie ma jeszcze sekcji w liscie to dodawanie*/
	temp_sekcja=bmdconf_znajdz_sekcje(sek_lista,nazwa_sekcji);
	if( temp_sekcja == NULL)
		temp_sekcja=bmdconf_dodaj_element_sekcjalist(sek_lista,nazwa_sekcji,czy_sek_wymagana);
	
	temp_pole=bmdconf_znajdz_pole(&(temp_sekcja->field),nazwa_pola);
	if( temp_pole == NULL)
	{
		temp_pole=bmdconf_dodaj_element_polelist(&(temp_sekcja->field),nazwa_pola,czy_pole_wymagane,czy_plik);
		return temp_pole;
	}
	else
		return NULL;
}

void bmdconf_dodaj_wzorzec_pola_hash(struct bmdconf_pole *element, pcre *wzor, long czy_wymagana_zgodnosc)
{
	bmdconf_dodaj_element_wzorzeclist(&(element->wz),wzor,czy_wymagana_zgodnosc);
}

/*zwraca NULL gdy nie znajdzie pola w sekcji (w slowiku)
w przypadku sukcesu zwraca wskaznik do pola*/
struct bmdconf_pole *bmdconf_znajdz_pole_sekcji_hash(struct bmdconf_sekcja **sek_lista,char *nazwa_sekcji,char *nazwa_pola)
{
	struct bmdconf_sekcja *temp_sekcja;
	struct bmdconf_pole *temp_pole;

	temp_sekcja=bmdconf_znajdz_sekcje(sek_lista,nazwa_sekcji);
	if(temp_sekcja==NULL)
		return NULL;
	else
	{
		temp_pole=bmdconf_znajdz_pole(&(temp_sekcja->field),nazwa_pola);
		if(temp_pole==NULL)
			return NULL;
		else
			return temp_pole;
	}
}


