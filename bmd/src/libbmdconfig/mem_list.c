#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmderr/libbmderr.h>

void bmdconf_init_list(struct bmdconf_mem_list **list_name)
{
	PRINT_INFO("BMDCONFIGINF Initializing configuration list\n");
	*list_name=NULL;
}

void bmdconf_add_list_item(struct bmdconf_mem_list** list_name,long numer_lini, char *sekcja,char *opcja,char *wartosc)
{
struct bmdconf_mem_list *temp,*new_item;

	PRINT_INFO("BMDCONFIGINF Adding item to configuration list (section: %s; option: %s; value: %s\n",sekcja, opcja, wartosc);
	if((*list_name)==NULL)
	{
		new_item=(struct bmdconf_mem_list*)malloc(sizeof(struct bmdconf_mem_list));
		*list_name=new_item;
		new_item->line_number=numer_lini;
		new_item->section=(char*)malloc((strlen(sekcja)+1)*sizeof(char));
		strcpy(new_item->section,sekcja);
		new_item->option=(char*)malloc((strlen(opcja)+1)*sizeof(char));
		strcpy(new_item->option,opcja);
		new_item->value=(char*)malloc((strlen(wartosc)+1)*sizeof(char));
		strcpy(new_item->value,wartosc);
		new_item->next=NULL;
    	}
    	else
    	{
		new_item=(struct bmdconf_mem_list*)malloc(sizeof(struct bmdconf_mem_list));
		new_item->line_number=numer_lini;
        	new_item->section=(char*)malloc((strlen(sekcja)+1)*sizeof(char));
		strcpy(new_item->section,sekcja);
		new_item->option=(char*)malloc((strlen(opcja)+1)*sizeof(char));
		strcpy(new_item->option,opcja);
		new_item->value=(char*)malloc((strlen(wartosc)+1)*sizeof(char));
		strcpy(new_item->value,wartosc);
		new_item->next=NULL;
		temp=*list_name;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=new_item;
	}
}

void bmdconf_delete_list(struct bmdconf_mem_list **list_name)
{
struct bmdconf_mem_list *temp,*t;

	PRINT_INFO("BMDCONFIGINF Deleting configuration list\n");
	t=*list_name;
	if(t!=NULL)
	{
		while(t->next!=NULL)
		{
			temp=t->next;
			free(t->section);
			free(t->option);
			free(t->value);
			free(t);
			t=temp;
		}
		free(t->section);
		free(t->option);
		free(t->value);
		free(t);
		*list_name=NULL;
	}
}

/*zwraca stringa wartosc dla znalezionej opcji przynalezecej do konkretnej sekcji
	NULL jesli nic nie znajdze */
char* bmdconf_find_list_item(struct bmdconf_mem_list **list_name,char *sekcja, char *opcja)
{
struct bmdconf_mem_list *temp;

	PRINT_INFO("BMDCONFIGINF Getting configuration value %s in section %s\n",opcja, sekcja);
	if( (*list_name)!=NULL)
	{
		temp=*list_name;
		while(temp->next!=NULL)
		{
			if( strcmp(temp->section,sekcja)==0 && strcmp(temp->option,opcja)==0 )
			{
				return temp->value;
			}
			else
			{
				temp=temp->next;
			}
		}

		if( strcmp(temp->section,sekcja)==0 && strcmp(temp->option,opcja)==0 )
		{

			return temp->value;
		}
		else
		{

			PRINT_WARNING("BMDCONFIGWRN Option %s not found in section %s\n",opcja, sekcja);
			return NULL;
		}

	}
	else
	{

		PRINT_INFO("BMDCONFIGERR Invalid name of the configuration list\n");
		return NULL;
	}
}

long bmdconf_remove_list_item(struct bmdconf_mem_list** list_name, char *sekcja, char *opcja)
{
struct bmdconf_mem_list* actual=*list_name;
struct bmdconf_mem_list* prev=NULL;
long first_item_flag=1;

	PRINT_INFO("BMDCONFIGINF Removing item from configuration list\n");
	while(actual != NULL)
	{
		if( strcmp(actual->section, sekcja)==0 )
		{
			/*jesli odnaleziony element do usuniecia z listy*/
			if( strcmp(actual->option, opcja)==0 )
			{
				/*wywalic z poczatku listy*/
				if(first_item_flag==1)
				{
					*list_name=actual->next;
				}
				/*usuwanie ze srodka lub konca listy*/
				else
				{
					prev->next=actual->next;
				}

				free(actual->section);
				free(actual->option);
				free(actual->value);
				free(actual);
				return 1;
			}
			else
			{
				/*zapamietuje poprzedni*/
				first_item_flag=0;
				prev=actual;
			}
		}
		/*jesli aktualnie badany element nie jest tym do usuniecia*/
		else
		{
			/*zapamietuje poprzedni*/
			first_item_flag=0;
			prev=actual;
		}

		actual=actual->next;
	}

	return 0;
}

long bmdconf_update_list_item(struct bmdconf_mem_list* list_name, char* sekcja, char* opcja, char *value)
{
struct bmdconf_mem_list* actual=list_name;

	PRINT_INFO("BMDCONFIGINF Updating item on configuration list\n");
	while(actual != NULL)
	{
		if( strcmp(actual->section, sekcja) == 0 )
		{
			if( strcmp(actual->option, opcja) == 0)
			{
				free(actual->value);
				actual->value=strdup(value);
				return 0;
			}
		}
		actual=actual->next;
	}
	return -1;
}
