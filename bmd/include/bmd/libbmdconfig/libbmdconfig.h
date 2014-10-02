#ifndef __HAVE_LIBBMDCONFIG_H
#define __HAVE_LIBBMDCONFIG_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pcre.h>
#ifndef WIN32
#ifdef WITH_POSTGRES
#include <libpq-fe.h> /*!!!!*/
#endif
#endif
#include <errno.h>

#ifdef WIN32
	#ifdef LIBBMDCONFIG_EXPORTS
		#define LIBBMDCONFIG_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDCONFIG_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDCONFIG_SCOPE
		#endif
	#endif
#else
	#define LIBBMDCONFIG_SCOPE
#endif /* ifdef WIN32 */

#if defined(__cplusplus)
extern "C" {
#endif


/* FUNKCJE PUBLICZNE */

/*@added - flaga BMDCONF_SECTION_REQ oznacza, ze sekcja jest wymagana w sprawdzanym pliku konfiguracyjnym
( flaga uzywana w bmdconf_add_section2dict() )*/
#define BMDCONF_SECTION_REQ 1
/*@added - flaga BMDCONF_SECTION_OPTIONAL oznacza, ze sekcja NIE jest wymagana w sprawdzanym pliku konfiguracyjnym mimo zaladowania slownika odpowiadajacego tej sekcji ( flaga uzywana w bmdconf_add_section2dict() )*/
#define BMDCONF_SECTION_OPTIONAL 0

/*do funkcji bmdconf_validate_config()
flaga BMDCONF_RESTRICT wymusza walidacje w trybie restrykcyjnym, tj. jesli w pliku konfiguracyjnym pojawi sie sekcja, ktorej slownik nie zostal zaladowany, wowczas zwrocony zostanie blad (nierozpoznane pole)
flaga BMDCONF_TOLERANT wymusza walidacje w trybie "poblazliwym", tj. jesli w pliku konfiguracyjnym pojawi sie sekcja, ktorej slownik nie zostal zaladowany, sekcja ta jest ignorowana i plik z tego powodu nie jest uznany z bledny.
W trybie TOLERANT nie bedzie mozliwe pobieranie wartosci dla pol w sekcjach, ktorych slowniki nie zostala zaladowane.
Zaklada sie bowiem, ze jesli nie ma slownika, to sekcje takie nie moga byc sprawdzane pod katem semantyki, a wiec moga byc bledne.
*/
#define BMDCONF_RESTRICT 0
#define BMDCONF_TOLERANT 1

struct bmdconf_sekcja;
typedef struct bmdconf_sekcja bmdconf_dict;
struct bmdconf_mem_list;
typedef struct bmdconf_mem_list bmdconf_dump;


/*<bmdconfig.c>*/
struct bmdconf_st
{
	bmdconf_dict *dict;
	bmdconf_dump *dump;
	long tryb;
	char *file_name;
};
typedef struct bmdconf_st bmd_conf;
LIBBMDCONFIG_SCOPE long bmdconf_init(bmd_conf **struktura);
LIBBMDCONFIG_SCOPE long bmdconf_destroy(bmd_conf **struktura);
LIBBMDCONFIG_SCOPE long bmdconf_add_section2dict(bmd_conf *struktura,const char *section_name,const char **registers, long czy_wymagana);
LIBBMDCONFIG_SCOPE long bmdconf_load_file(const char *file_path, bmd_conf *struktura, char **error_string);
LIBBMDCONFIG_SCOPE long bmdconf_get_value(bmd_conf *struktura,const char *section_name,const char *option_name,char **result);
LIBBMDCONFIG_SCOPE const char * bmdconf_get_value_static(bmd_conf *struktura,const char *section_name,const char *option_name);
LIBBMDCONFIG_SCOPE long bmdconf_get_value_int(bmd_conf *struktura,const char *section_name,const char *option_name, long *result);
LIBBMDCONFIG_SCOPE long bmdconf_set_mode(bmd_conf* struktura,long flag);
/*</bmdconfig.c>*/


/*<parse_mem.c>*/
LIBBMDCONFIG_SCOPE long bmdconf_load_memory(char *buf,long length,bmd_conf *struktura,char **error_string);
/*</parse_mem.c>*/


/*<update_file.c>*/
LIBBMDCONFIG_SCOPE long bmdconf_update_option_value( bmd_conf *struktura, char *section, char *option, char *new_value);
LIBBMDCONFIG_SCOPE long bmdconf_save_configuration( bmd_conf *struktura, char **error);
/*</update_file.c>*/




#ifndef WIN32

/*<for_db.c>*/
LIBBMDCONFIG_SCOPE long bmdconf_load_db(char *db_params, bmd_conf *struktura, char **error_string);

/*PUBLICZNE FUNKCJE ADMINISTRATORSKIE*/
LIBBMDCONFIG_SCOPE long bmdconf_insert_option_db(char* conn_params, char *section, char *option, char *value, char **error);
LIBBMDCONFIG_SCOPE long bmdconf_set_value_db(char* conn_params, char *section, char *option, char *value, char **error);
LIBBMDCONFIG_SCOPE long bmdconf_delete_option_db(char* conn_params, char *section, char *option, char **error);

/* 	usuwa cala sekcje tj. usuwa wszystkie wpisy sekcji i sama sekcje
	usuwanie sekcji - iteracyjne usuwanie opcji sekcji zastopione funkcja bmdconf_delete_section_db
LIBBMDCONFIG_SCOPE long bmdconf_delete_section_iter_db(char* conn_params, char *section, char **error);
*/

/*	usuwa cala sekcje  tj. usuwa wszystkie wpisy sekcji i sama sekcje
	usuwanie calej sekcji - kaskadowe usuwanie opcji sekcji po stronie postgres (ON DELETE CASCADE)*/
LIBBMDCONFIG_SCOPE long bmdconf_delete_section_db(char* conn_params, char *section, char **error);
LIBBMDCONFIG_SCOPE long bmdconf_clean_db(char* conn_params, char **error);

LIBBMDCONFIG_SCOPE long bmdconf_file2db(char *file_name, char* conn_params, char** error);
LIBBMDCONFIG_SCOPE long bmdconf_db2file(char *conn_params, char* file_name, char** error);
/*</for_db.c>*/

#endif /* ifndef WIN32*/






/* FUNKCJE PRYWATNE */


/*<for_db.c>*/
char* bmdconf_read_and_parse_config_db(char *conn_params, bmdconf_dump **dumped);
char* bmdconf_blad_niedozwolona_wartosc_db(long id_opcji,char *sekcja,char *pole, char *wartosc);
char* bmdconf_blad_nieznane_pole_db(long id_opcji, char *sekcja, char *pole);
char* bmdconf_blad_braku_wymaganego_pola_db(char *sekcja, char *pole);
char* bmdconf_blad_niepoprawnej_sciezki_db(long id_opcji, char *path);
char* bmdconf_validate_config_restrict_db(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary);
char* bmdconf_validate_config_tolerant_db(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary);
char* bmdconf_validate_config_db(bmd_conf* struktura);
/*</for_db.c>*/


/*<update_file.c>*/
long bmdconf_copy_file(char *src, FILE* dest);
long bmdconf_change_value(char *assign, char* val, char **result);
/*</update_file.c>*/


/*<str.c>*/
char* bmdconf_substr(char *string,long start,long end);
void bmdconf_substr_free(char *str);
char* bmdconf_strmerge(char* src1,char *src2);
void bmdconf_strmerge_free(char *str);
char* bmdconf_itostr(long liczba);
void bmdconf_itostr_free(char *str);
long contain_quotation(char *string);
/*</str.c>*/


/*<mem_list.c>*/
struct bmdconf_mem_list {
	long line_number;
	char *section;
	char *option;
	char *value;
	struct bmdconf_mem_list *next;
};
void bmdconf_init_list(struct bmdconf_mem_list **list_name);
void bmdconf_add_list_item(struct bmdconf_mem_list** list_name,long numer_lini, char *sekcja,char *opcja,char *wartosc);
void bmdconf_delete_list(struct bmdconf_mem_list **list_name);
char* bmdconf_find_list_item(struct bmdconf_mem_list **list_name,char *sekcja, char *opcja);
long bmdconf_remove_list_item(struct bmdconf_mem_list** list_name, char *sekcja, char *opcja);
long bmdconf_update_list_item(struct bmdconf_mem_list* list_name, char* sekcja, char* opcja, char *value);
/*</mem_list.c>*/


/*<lista.c>*/
struct bmdconf_lista
{
    char *element;
    struct bmdconf_lista *next;
};
void bmdconf_init_lista(struct bmdconf_lista **nazwa_listy);
void bmdconf_dodaj_element(struct bmdconf_lista **nazwa_listy,char *str);
void bmdconf_niszcz_liste(struct bmdconf_lista **nazwa_listy);
long bmdconf_znajdz_element(struct bmdconf_lista **nazwa_listy,char *str);
/*</lista.c>*/


/*<hash_pcre.c>*/
struct bmdconf_wzorzec
{
	pcre *wzor; /*string wzorca*/
	long zgodnosc;/*1 dla wymaganej zgodnosci ze wzorcem; else kazda inna dla wymaganego niedopasowania */
	struct bmdconf_wzorzec *next; /*nastepny  w liscie*/
};
void bmdconf_init_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy);
void bmdconf_dodaj_element_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy,pcre *compiled,long czy_zgodny);
void bmdconf_niszcz_wzorzeclist(struct bmdconf_wzorzec **nazwa_listy);
void bmdconf_init_iterator_wzorzec(struct bmdconf_wzorzec **nazwa_listy,struct bmdconf_wzorzec **iterator);
struct bmdconf_wzorzec* bmdconf_get_nextitem_wzorzec(struct bmdconf_wzorzec **iterator);
	/*-----------------*/
struct bmdconf_pole
{
char *nazwa_pola; /*string z nazwa pola*/
long pole_wymagane; /*1 gdy pole wymagana; else kazda inna gdy pole opcjonalne*/
long flaga_plik; /*1 jesli wartoscia tego pola jest sciezka do pliku, inna wartosc dla innych przypadkow */
struct bmdconf_wzorzec *wz; /*wskaznik listy wzorcow przyporzadkowanej do tego pola*/
struct bmdconf_pole *next; /*nastepny w liscie*/
};
void bmdconf_init_polelist(struct bmdconf_pole **nazwa_listy);
struct bmdconf_pole* bmdconf_dodaj_element_polelist(struct bmdconf_pole **nazwa_listy,char *str,long czy_wymagane,long czy_plik);
void bmdconf_niszcz_polelist(struct bmdconf_pole **nazwa_listy);
struct bmdconf_pole* bmdconf_znajdz_pole(struct bmdconf_pole **nazwa_listy,char *str);
void bmdconf_init_iterator_pole(struct bmdconf_pole **nazwa_listy,struct bmdconf_pole **iterator);
struct bmdconf_pole* bmdconf_get_nextitem_pole(struct bmdconf_pole **iterator);
	/*-----------------*/
struct bmdconf_sekcja {
	long wymagana; /*okresla, czy wystapienie sekcji wymagane w sprawdzanym pliku .conf*/
	char *nazwa_sekcji; /*string z nazwa sekcji*/
	struct bmdconf_pole *field; /*wskaznik na liste pol dla tej sekcji*/
	struct bmdconf_sekcja *next; /*nastepny w liscie*/
};
void bmdconf_init_sekcjalist(struct bmdconf_sekcja **nazwa_listy);
struct bmdconf_sekcja* bmdconf_dodaj_element_sekcjalist(struct bmdconf_sekcja **nazwa_listy,char *str, long czy_wymagana);
void bmdconf_niszcz_sekcjalist(struct bmdconf_sekcja **nazwa_listy);
struct bmdconf_sekcja* bmdconf_znajdz_sekcje(struct bmdconf_sekcja **nazwa_listy,char *str);
	/*-----------------*/
struct bmdconf_pole* bmdconf_dodaj_sekcje_pole_hash(struct bmdconf_sekcja **sek_lista,char *nazwa_sekcji,
		char *nazwa_pola,long czy_pole_wymagane,long czy_plik, long czy_sek_wymagana);
void bmdconf_dodaj_wzorzec_pola_hash(struct bmdconf_pole *element, pcre *wzor, long czy_wymagana_zgodnosc);
struct bmdconf_pole *bmdconf_znajdz_pole_sekcji_hash(struct bmdconf_sekcja **sek_lista,char *nazwa_sekcji,
		char *nazwa_pola);
/*</hash_pcre.c>*/


/*<dict.c>*/
char *bmdconf_komunikat_bledu_general(char *nazwa_sekcji,long number);
char *bmdconf_komunikat_bledu_wzorca(char *nazwa_sekcji,long number, long nr_wzorca);
char *bmdconf_komunikat_bledu_pola(char *nazwa_sekcji,long number, char *nazwa_pola);
void bmdconf_init_dictionary_config(struct bmdconf_sekcja **dictionary);
void bmdconf_delete_dictionary_config(struct bmdconf_sekcja **dictionary);
char* bmdconf_load_dictionary_config(struct bmdconf_sekcja **dictionary,char *section_name,char **registers,long czy_wymagana);
/*</dict.c>*/


/*<do_config.c>*/
char* bmdconf_wiersz_pliku(FILE *plik);
void bmdconf_init_dump_config(struct bmdconf_mem_list **dumped);
void bmdconf_delete_dump_config(struct bmdconf_mem_list **dumped);
char* bmdconf_read_and_parse_config(FILE *fileconf,struct bmdconf_mem_list **dumped );
char* bmdconf_get_value_config(struct bmdconf_mem_list **list_name,char *section_name,char *option_name);
const char* bmdconf_get_value_static_config(struct bmdconf_mem_list **list_name,char *section_name,char *option_name);
char* bmdconf_blad_niedozwolona_wartosc(long nr_linii,char *nazwa_pola);
char* bmdconf_blad_nieznane_pole(long nr_linii,char *nazwa_pola);
char* bmdconf_blad_braku_wymaganego_pola(long nr_linii,char *nazwa_sekcji, char *nazwa_pola);
char* bmdconf_blad_niepoprawnej_sciezki(long nr_linii, char *path);
long bmdconf_file_exists(char *path);
char* bmdconf_validate_config(bmd_conf* struktura);
char* bmdconf_validate_config_tolerant(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary);
char* bmdconf_validate_config_restrict(struct bmdconf_mem_list **dumped, struct bmdconf_sekcja **dictionary);
/*</do_config.c>*/


/*<parse_mem.c>*/
char* bmdconf_wiersz_mem(char *buf,long length,long *read_index);
char* bmdconf_read_and_parse_config_mem(char *buf,long length,struct bmdconf_mem_list **dumped );
/*</parse_mem.c>*/



#if defined(__cplusplus)
}
#endif

#endif /*ifndef __HAVE_LIBBMDCONFIG_H*/
