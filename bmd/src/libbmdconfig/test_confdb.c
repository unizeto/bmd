#ifndef WIN32
#include <bmd/libbmdconfig/libbmdconfig.h>
#include "test_dictionary.h"

/*!!
niniejszy test oparty jest na bazie, ktora utworzyc i wypelnic mozna za pomoca
skryptu testdb.sql
*/

int main()
{

bmd_conf *test=NULL;
char *error=NULL;
int result=0;

printf("Jesli komentarz bledu mowi o braku pliku o podanej sciezce 'plik.txt', nalezy stworzyc taki plik.\nWynika to z tego, ze slownik definiuje pewne pola jako sciezki do pliku.\n\n");
bmdconf_init(&test);
bmdconf_set_mode(test , BMDCONF_TOLERANT);
bmdconf_add_section2dict(test, "sekcja0", sek0_options_dict, BMDCONF_SECTION_REQ);
bmdconf_add_section2dict(test, "sekcja1", sek1_options_dict, BMDCONF_SECTION_REQ);

/*!! w bmdconf_load_db podac odpowiedni adres i ewentualnie uzytkownika w zaleznosci, gdzie zostala postawiona baza*/
result=bmdconf_load_db("hostaddr=127.0.0.1 dbname = testconf", test, &error);
printf("wynik %i: \nbmdconf_load_db komentarz bledu: %s\n", result, error);
free(error); error=NULL;

printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja1","sek1_opcja0"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja1","sek1_opcja1"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja1","sek1_opcja4"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja1","sek1_opcja5"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja1","sek1_opcja6"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja0","sek0_opcja0"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja0","sek0_opcja1"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja0","sek0_opcja4"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja0","sek0_opcja5"));
printf("get: \"%s\"\n",bmdconf_get_value_static(test,"sekcja0","sek0_opcja6"));
bmdconf_destroy(&test); test=NULL;


/*testy funkcji "administratorskich"*/
printf("wynik: %i;  ", bmdconf_insert_option_db("host=localhost dbname=testconf", "nowa", "nowa_opt1", "", &error) );
printf("komunikat bledu bmdconf_insert_option_db: %s\n", error);
free(error); error=NULL;
printf("wynik: %i;  ", bmdconf_set_value_db("host=localhost dbname=testconf", "nowa", "nowa_opt1", "updated", &error) );
printf("komunikat bledu bmdconf_set_value_db: %s\n", error);
free(error); error=NULL;
printf("wynik: %i;  ", bmdconf_insert_option_db("host=localhost dbname=testconf", "do_usuniecia", "nowa_opt1", "", &error) );
printf("komunikat bledu bmdconf_insert_option_db: %s\n", error);
free(error); error=NULL;
printf("wynik: %i;  ", bmdconf_delete_option_db("host=localhost dbname=testconf", "nowa", "nowa_opt1", &error));
printf("komunikat bledu bmdconf_delete_option_db: %s\n", error);
free(error); error=NULL;
printf("wynik: %i;  ", bmdconf_delete_section_db("host=localhost dbname=testconf", "do_usuniecia", &error));
printf("komunikat bledu bmdconf_delete_section_db: %s\n", error);
free(error); error=NULL;

/*eksport konfiguracji plik <---> baza danych*/
printf("wynik: %i;  ", bmdconf_db2file("dbname=testconf", "dump_testconf.conf", &error));
printf("komunikat bledu bmdconf_db2file: %s\n", error);
free(error); error=NULL;
printf("Zrzut konfiguracji z bazy zapisany został do pliku 'dump_testconf.conf'\n");
printf("wynik: %i;  ", bmdconf_file2db("dump_testconf.conf", "dbname=testconf", &error));
printf("komunikat bledu bmdconf_file2db: %s\n", error);
free(error); error=NULL;
/*dla wygody nie czyszcze bazy, bo jej odtwarzanie jest irytujace przy kilku wywolaniach testu*/
/*printf("wynik: %i;  ", bmdconf_clean_db("dbname=testconf", &error));
printf("komunikat bledu bmdconf_clean_db: %s\n", error);
free(error); error=NULL;
*/

return 0;
}
#else
#include<stdio.h>
int main()
{
	printf("Pod Windows nie mozna w obecnym stanie testowac libbmdconfig na PostgreSQL.\n");
	return 0;
}

#endif /*ifndef WIN32*/
