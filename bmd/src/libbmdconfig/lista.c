#include <bmd/libbmdconfig/libbmdconfig.h>

void bmdconf_init_lista(struct bmdconf_lista **nazwa_listy)
{
	*nazwa_listy=NULL;
}

void bmdconf_dodaj_element(struct bmdconf_lista **nazwa_listy,char *str)
{
	struct bmdconf_lista *nowy=NULL, *temp=NULL;
	
	if(*nazwa_listy==NULL)
	{
		nowy=(struct bmdconf_lista*)malloc(sizeof(struct bmdconf_lista));
		*nazwa_listy=nowy;
		nowy->element=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->element,str);
		nowy->next=NULL;
	}
	else
	{
		nowy=(struct bmdconf_lista*)malloc(sizeof(struct bmdconf_lista));
		nowy->element=(char*)malloc((strlen(str)+1)*sizeof(char));
		strcpy(nowy->element,str);
		nowy->next=NULL;
		temp=*nazwa_listy;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=nowy;
	}
}

void bmdconf_niszcz_liste(struct bmdconf_lista **nazwa_listy)
{
	struct bmdconf_lista *temp=NULL,*t=NULL;
	t=*nazwa_listy;
	if(t!=NULL)
	{
		while(t->next!=NULL)
		{
			temp=t->next;
			free(t->element);
			free(t);
			t=temp;
		}
		free(t->element);
		free(t);
		*nazwa_listy=NULL;
	}
}

/*zwraca 1, gdy znaleziony
	 0 przy braku elementu w liscie*/
long bmdconf_znajdz_element(struct bmdconf_lista **nazwa_listy,char *str)
{
	if( (*nazwa_listy)!=NULL)
	{
		struct bmdconf_lista *temp=*nazwa_listy;
		while(temp->next!=NULL)
		{
			if( strcmp(str,temp->element)==0)
				return 1;
			else
				temp=temp->next;
		}
		if( strcmp(str,temp->element)==0)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
