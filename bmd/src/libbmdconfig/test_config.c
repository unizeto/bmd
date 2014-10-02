#include <bmd/libbmdconfig/libbmdconfig.h>
#include "test_dictionary.h"


/*#define TEST_LOAD_MEM*/


#ifndef TEST_LOAD_MEM
int main()
{
char *result_char=NULL;
char *komunikat=NULL;
int result_int=0;
long int wynik_int=0;
bmd_conf *wsk=NULL;

printf("Jesli komentarz bledu mowi o braku pliku o podanej sciezce 'plik.txt', nalezy stworzyc taki plik.\nWynika to z tego, ze slownik definiuje pewne pola jako sciezki do pliku.\n\n");
/*inicjalizacja*/
result_int=bmdconf_init(&wsk);
printf("inicjalizacja: |%i|\n",result_int);
/*przykladowy slownik test_dictionary.h*/
result_int=bmdconf_add_section2dict(wsk,section_name0,sek0_options_dict, BMDCONF_SECTION_REQ);
printf("ladowanie slownika: |%i|\n",result_int);
/*przykladowy slownik test_dictionary.h*/
result_int=bmdconf_add_section2dict(wsk,section_name1,sek1_options_dict, BMDCONF_SECTION_OPTIONAL);
printf("ladowanie slownika: |%i|\n",result_int);
/*wlaczanie trybu tolerancyjnego*/
/*sekcja 'dupa' zostanie zignorowana i blad nie bedzie zgloszony w trybie tolerancyjnym*/
bmdconf_set_mode(wsk, BMDCONF_TOLERANT);
/*ladowanie pliku konfiguracyjnego "test_file.conf"*/
result_int=bmdconf_load_file("test_file.conf", wsk, &komunikat);
printf("load file: |%i|; komunikat bledu: %s\n",result_int, komunikat);
free(komunikat); komunikat=NULL;
/*pobieranie wartosci*/
result_int=bmdconf_get_value_int(wsk,"sekcja0","sek0_opcja6",&wynik_int);
printf("get int: |%i| val: %i\n", result_int, (int)wynik_int); /*!!*/
result_int=bmdconf_get_value(wsk,"sekcja1","sek1_opcja0",&result_char);
printf("get (dynamicznie alokowany result): |%i|; val:\"%s\"\n", result_int, result_char);
free(result_char); result_char=NULL;
/*zmiana wartosci opcji*/
result_int=bmdconf_update_option_value(wsk, "sekcja1", "sek1_opcja0", "nowa wartosc po update");
printf("zmiana wartosci opcji: |%i|\n", result_int );
printf("wywolania bmdconf_get_value_static:\n");
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja0"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja1"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja4"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja5"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja6"));

printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja0","sek0_opcja0"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja0","sek0_opcja1"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja0","sek0_opcja4"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja0","sek0_opcja5"));
printf("get: \"%s\"\n",bmdconf_get_value_static(wsk,"sekcja0","sek0_opcja6"));

/*nie jest ladowany slownik sekcji do_zignorowania, dlatego opcje tej sekcji nie sa dostepne*/
printf("get: \"%s\" (powinien byc null)\n",bmdconf_get_value_static(wsk,"do_zignorowania","opt1"));
/*zapis zmodyfikowanych wartosci opcji*/
result_int=bmdconf_save_configuration(wsk, &komunikat);
printf("zapis modyfikacji: |%i|; komunikat bledu: %s\n", result_int, komunikat);
free(komunikat); komunikat=NULL;
result_int=bmdconf_destroy(&wsk);
printf("destroy: |%i|\n",result_int);

return 0;
}

#else
int main()
{
bmd_conf *wsk=NULL;
unsigned char *tmp_buf=NULL;
int tmp_length=-1;
int fd;
struct stat file_info;
char *error=NULL;
int status;

printf("Jesli komentarz bledu mowi o braku pliku o podanej sciezce 'plik.txt', nalezy stworzyc taki plik.\nWynika to z tego, ze slownik definiuje pewne pola jako sciezki do pliku.\n\n");
bmdconf_init(&wsk);
bmdconf_add_section2dict(wsk,section_name0,sek0_options_dict, BMDCONF_SECTION_REQ);
bmdconf_add_section2dict(wsk,section_name1,sek1_options_dict, BMDCONF_SECTION_OPTIONAL);
/*wlaczanie trybu tolerancyjnego*/
/*sekcja 'dupa' zostanie zignorowana i blad nie bedzie zgloszony w trybie tolerancyjnym*/
bmdconf_set_mode(wsk, BMDCONF_TOLERANT);
fd=open("test_file.conf",O_RDONLY);
if(fd<0)
	{ return -1; }
status=stat("test_file.conf",&file_info);
if(status!=0)
{
	close(fd);
	return -1;
}

tmp_buf=(unsigned char *)malloc(file_info.st_size+2);
memset(tmp_buf,0,file_info.st_size+1);
tmp_length=read(fd,tmp_buf,file_info.st_size);
close(fd);
status=bmdconf_load_memory(tmp_buf,tmp_length,wsk,&error);
free(tmp_buf);
printf("status: |%i|; komunikat bledu: %s\n",status, error);
free(error); error=NULL;
printf("get: \"%s\"\n", bmdconf_get_value_static(wsk,"sekcja1","sek1_opcja0") );
bmdconf_destroy(&wsk);

return 0;
}
#endif
