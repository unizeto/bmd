#ifndef _LIBBMDDB_INCLUDED_
#define _LIBBMDDB_INCLUDED_

#ifdef WIN32
	#ifdef LIBBMDDB_EXPORTS
		#define LIBBMDDB_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDDB_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDDB_SCOPE
		#endif
	#endif
#else
	#define LIBBMDDB_SCOPE
#endif /* ifdef WIN32
*/

#include <stdio.h>
#include <bmd/common/bmd-common.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/bmd_db_functions.h>

typedef enum{ADDITIONAL_ESCAPING, STANDARD_ESCAPING}esc_mode_t; 

/**
 * \brief Inicjalizacja biblioteki bazodanowej
 *
 * Funkcja inicjalizuje biblioteke libbmddb do obslugi konkretnej bazy danych.
 *
 * \param db_library biblioteka do obslugi danego typu bazy danych
 * \return status wykonania - wartosc BMD_OK w przypadku poprawnej inicjalizacji,
                              < BMD_OK, w przypadku bledu
 * \warning - funkcja ta powinna byc wywolana przed jakimkolwiek odwolaniem do bazy
 *            danych w kodzie
 */
LIBBMDDB_SCOPE long bmd_db_init(const char *db_library);

/**
 *
 * \brief Finalizuje uzywanie biblioteki
 *
 * Funkcja finalizuje uzywanie biblioteki bazodanowej, poprzez zwolnienie uchwytow
 * systemowych do bibliotek oraz zwalnia zaalokowana pamiec.
 * \return status wykonania - wartosc BMD_OK w przypadku poprawnej inicjalizacji,
 *                            < BMD_OK, w przypadku bledu
 * \warning - po wywolaniu tej funkcji nie jest mozliwa dalsza komunikacja z baza danych
 */
LIBBMDDB_SCOPE long bmd_db_end(void);

/**
 *
 * \brief Zwolnienie struktury przechowujacej wiersz wyniku zapytania SQL
 *
 * Funkcja umozliwia zwolnienie struktury przechowujacej wiersz wyniku zapytania SQL,
 * dodatkowo funkcja zeruje wynikowy wskaznik
 *
 * \param rowstruct - struktura opisujace wiersz wyniku zapytania SQL
 * \return status wykonania - wartosc BMD_OK w przypadku poprawnego zwolnienia,
 *                            < BMD_OK, w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_row_struct_free(db_row_strings_t ** rowstruct);

/**
 *
 * \brief Pobranie pojedynczej wartosci z rezultatu zapytania SQL
 *
 * Funkcja umozliwia pobranie pojedynczej wartosci z zapytania SQL, alokujac
 * przy tym niezbedna pamiec. Moze pracowac w dwoch trybach:
 * @li FETCH_ABSOLUTE - pobiera element okreslony przez parametry which_row i which_column
 * @li FETCH_NEXT     - pobiera element kolejny w stosunku do ostatnio pobranego
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param hRes uchwyt do wyniku zapytania SQL
 * \param which_row okresla ktory wiersz nalezy pobrac
 * \param which_column okresla ktora kolumne nalezy pobrac
 * \param value zmienna gdzie umieszczona bedzie pobierana wartosc
 * \param fetch_mode tryb pracy funkcji FETCH_ABSOLUTE, FETCH_NEXT
 * \param successfully_fetched numer ostanio pobranego wiersza
 * \return status wykonania - BMD_OK przy pobranym pobraniu wartosc,
 *                            <BMD_OK, przy niepoprawnym
 * \warning nie nalezy w miare mozliwosci uzywac trybu FETCH_ABSOLUTE, w przyszlosci zostanie on
 * zupelnie wyelminiowany
 */
LIBBMDDB_SCOPE long bmd_db_result_get_value(void *hDB,void *hRes,long which_row,long which_column,char **value,
											EnumDbFetchMode_t fetch_mode,long *successfully_fetched);
/**
 *
 * \brief Funkcja pobiera pojedynczy wiersz z rezultatu zapytania SQL
 *
 * Funkcja umozliwia pobranie pojedynczego wiersza z rezultatu zapytania SQL, dodatkowo
 * umozliwiajac okreslenie ile kolumn ma byc zwroconych. Funkcja moze pracowac podobnie jak
 * funkcja bmd_db_result_get_value w dwoch trybach: FETCH_ABSOLUTE i FETCH_NEXT
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param hRes uchwyt do wyniku zapytania SQL
 * \param which_row wskazanie ktory wiersz ma byc pobrany z wyniku zapytania SQL
 * \param max_columns okresla ile kolumn ma byc zwroconych
 * \param rowstruct wynikowa struktura zawieraja pojedynczy wiersz wyniku
 * \param fetch_mode tryb pracy funkcji FETCH_ABSOLUTE, FETCH_NEXT
 * \param successfully_fetched numer ostanio pobranego wiersza
 * \return status wykonania BMD_OK gdy udalo sie pobrac wiersz, <BMD_OK gdy sie nie udalo
 *
 * \warning nie nalezy w miare mozliwosci uzywac trybu FETCH_ABSOLUTE, w przyszlosci zostanie on
 * zupelnie wyelminiowany
 *
 */
LIBBMDDB_SCOPE long bmd_db_result_get_row(void *hDB,void *hRes,long which_row,long max_columns,
										  db_row_strings_t **rowstruct,
										  EnumDbFetchMode_t fetch_mode,long *successfully_fetched);

/**
 *
 * \brief Zwolnienie uchwytu do rezultatu wykonywania zapytania SQL
 *
 * Funkcja umoziwia zwolnienie uchwytu do rezultatu wykonywania zapytania SQL,
 * zwalniajac zaalokowana pamiec oraz zerujac wskaznik
 * \param hRes - uchwyt do zwolnienia
 * \return status wykonania - BMD_OK w przypadku poprawnego zwolnienia, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_result_free(void **hRes);

/**
 *
 * \brief Eksport danych binarnych do BLOB'a
 *
 * Funkcja umozliwia wyeksportowania danych binarnych do BLOB'a w pojedynczej iteracji,
 * tzn. caly BLOB jest eksportowany za jednym razem. W zaleznosci od silnika bazodanowego
 * oraz od tego czy transakcja jest juz obecna w polaczeniu z baza danych, funkcja ta moze
 * rozpoczac lokalna transakcje potrzebna na eksport BLOB'a.
 *
 * \param hDB uchwyt na polaczenie z baza danych
 * \param dataGenBuf binarne do wyeksportowania jako BLOB
 * \param blob_number numer blob'a przydzielony przez silnik bazy danych, numer ten mozna
 *                    wykorzystac gdy zajdzie potrzeba pobrania BLOB'a
 * \return status wykonania BMD_OK gdy poprawnie wyekportowano BLOB'a, <BMD_OK gdy byl blad
 */
LIBBMDDB_SCOPE long bmd_db_export_blob(void *hDB,GenBuf_t *dataGenBuf,char **blob_number);

/**
 *
 * \brief Pobranie BLOB'a z bazy danych
 *
 * Funkcja umozliwia pobranie BLOB'a z bazy danych w pojedynczej iteracji, tzn.
 * caly BLOB jest pobierany za jednym razem. W zaleznosci od silnika bazodanowego
 * oraz od tego czy transakcja jest juz obecna w polaczeniu z baza danych, funkcja ta moze
 * rozpoczac lokalna transakcje potrzebna na eksport BLOB'a.
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param blob_number numer BLOB'a przydzielony przez baze danych, a uzyskany po wywolaniu bmd_db_export_blob
 * \param dataGenBuf pobrane binarne dane z obiektu BLOB
 */
LIBBMDDB_SCOPE long bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf);

/**
 *
 * \brief Inicjalizacja operacji eksportu BLOB
 *
 * Funkcja inicjalizuje operacje eksportu BLOB do bazy danych.
 * Eksport odbywa sie w sposob strumieniowy.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param blob_number identyfikator BLOB przydzielony przez silnik bazy danych
 * \param blob_locator uchwyt do obiektu BLOB utworzonego w bazie danych, do ktorego mozna zapisywac
 *                     strumieniowo fragmenty BLOB'a
 * \return status wykonania BMD_OK gdy inicjalizacji operacji przebiegla poprawnie, <BMD_OK gdy byl blad
 */
LIBBMDDB_SCOPE long bmd_db_export_blob_begin(void *hDB,char **blob_number,void **blob_locator);

/**
 *
 * \brief Eksport pojedynczej porcji BLOB
 *
 * Funkcja umozliwa wyslanie pojedynczego porcji binarnych danych do uzyskanego wczesniej
 * uchwyt BLOB z dazy danych. Obecnie parametr which_piece jest wykorzystywany tylko w bazie Oracle,
 * gdzie nalezy poinformowac ktora porcja binarnych danych jest aktualnia wysylania (pierwsza,
 * nastepna, ostatnia).
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param chunkGenBuf porcja binarnych danych do umieszczenia w bazie
 * \param blob_locator uchwyt do BLOB z bazy danych
 * \param which_piece okreslenie ktora porcja binarnych danych jest aktualnie wysylana
 *                    @li LOB_FIRST_PIECE - pierwsza porcja
 *					  @li LOB_NEXT_PIECE - kolejna porcja
					  @li LOB_LAST_PIECE - ostatnia porcja
					  @li LOB_ONE_PIECE - pojedyncza porcja
 * \return status wykonania BMD_OK w przypadku poprawnego eksportu danych, <BMD_OK gdy byl blad
 */

LIBBMDDB_SCOPE long bmd_db_export_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
											    EnumDbWhichPiece_t which_piece);

/**
 * \brief Zakonczenie strumieniowego eksportu porcji danych
 *
 * Funkcja umozliwia zakonczenie strumieniowego eksportu porcji danych do serwera.
 * W zaleznosci od silnika bazy danych funkcja ta moze zakonczyc lokalna transakcje
 * bazodanowa. W przypadku gdy transakacja na eksport nie zostala otwarta, istniejaca
 * transakcja nie bedzie konczona.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param blob_locator - uchwyt do BLOB z bazy otrzymany z funkcji bmd_db_export_blob_begin
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_export_blob_end(void *hDB,void **blob_locator);

/**
 * \brief Inicjalizacja strumieniowego pobierania BLOB'a
 *
 * Funkcja umozliwia inicjalizacja operacji strumieniowego pobierania BLOB'a. W jej wywolaniu ustalany jest rozmiar
 * BLOB'a z bazy danych i alokowany bufor wyjsciowy na wielkosci porcji jednorazowo pobieranej.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param blob_number numer BLOB'a w bazie danych do pobrania
 * \param blob_size pobrana wielkosc BLOB'a
 * \param blob_locator uchwyt do BLOB'a w bazie danych
 * \param dataGenBuf bufor na jednorazowa porcje danych
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_import_blob_begin(void *hDB,char *blob_number,long *blob_size,void **blob_locator,
											 GenBuf_t **dataGenBuf);

/**
 * \brief Kontynuacja strumieniowego pobierania BLOB
 *
 * Funkcja umozliwia pobieranie strumieniowe porcji BLOB z bazy danych.
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param chunkGenBuf bufor z zaalokowanym miejscem na wielkosc pojedynczej pobieranej porcji danych
 * \param blob_locator uchwyt do BLOB'a z bazy danych
 * \param which_piece okresla ktora porcja jest pobierana. Znaczenie tej opcji jest takie samo jak
 *                    przy funkcjach wysylajacych BLOB do bazy danych. W niektorych bazach parametr
 *                    ten jest ignorowany.
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_import_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
												EnumDbWhichPiece_t which_piece);

/**
 * \brief Zakonczenie strumieniowego pobierania BLOB'a
 *
 * Funkcja umozliwia zakonczenie strumieniowego pobierania BLOB z bazy danych. W zaleznosci od bazy
 * danych funkcja moze zakonczyc rozpoczeta lokalna transakacje na operacje na BLOB. Gdy transakcja jest
 * obecna, ale nie jest transakcja lokalna dla BLOB transakacja nie zostanie zakonczona.
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param blob_locator uchwyt do BLOB'a z bazy danych
 * \param dataGenBuf bufor do ktorego zapisana bedzie ostatnia porcja BLOB'a
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_import_blob_end(void *hDB,void **blob_locator,GenBuf_t **dataGenBuf);

/**
 * \brief Kasowanie BLOB
 *
 * Funkcja umozliwia skasowanie BLOB z bazy danych. Podobnie jak przy innych funkcjach dotyczacych
 * BLOB, moze ona rozpoczac i skonczyc lokalna transakcje dla operacji BLOB.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param blob_number identyfikator BLOB'a do skasowania
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_delete_blob(void *hDB, char *blob_number);

/**
 * \brief Nawiazanie polaczenie z baza danych
 *
 * Funkcja umozliwia polaczenie z baza danych z wykorzystaniem tzw. connection-string wlasciwego dla danej
 * bazy danych. Np. w przypadku Oracle bedzie do lancuch w formacie TNS Listenera.
 *
 * \param conn_info connection string okreslajacy parametry bazy danych
 * \param hDB wyjsciowy uchwyt do nawiazanego polaczenia z baza danych
 * \return status wykonania, BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_connect(const char *conn_info,void **hDB);

/**
 * \brief Nawiazanie polaczenia z baza danych
 *
 * Funkcja umozliwia polaczenie z baza danych bez connection-string
 *
 * \param adr adres IP serwera bazy danych
 * \param port numer portu serwera bazy danych
 * \param db_name nazwa bazy danych
 * \param user identyfikator uzytkownika bazy danych
 * \param pass haslo uzytkownika bazy danych
 * \param hDB wyjsciowy uchwyt do nawiazanego polaczenia z baza danych
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_connect2(const char *adr,char *port,const char *db_name,const char *user,
					 const char *pass,void **hDB);

/**
 * \brief Rozlaczenie z baza danych
 *
 * Funkcja umozliwia rozlaczenie z baza danych - w przypadku rozlaczenia aktywna transakcja jest konczona.
 * Nie jest juz mozliwe dalsze wykonywanie zapytan do bazy danych. Funkcja zwalnia zaalokowana pamiec oraz
 * zeruje wskaznik na uchwyt.
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_disconnect(void **hDB);

/**
* @author WSZ
* Funkcja, za pomoca ktorej mozna ustawic funckje definiujaca dodatkowe czynnosci wykonywane
* w przypadku problemow z wykonaniem polecenia/zapytania bazodanowego ze wzgledu na utrate polaczenia.
* Wystarczy ustawic callback raz przy wywolaniem pierwszego execute, badz zgodnie z potrzebami
* ustawiac sobie inny callback przed roznymi wywolaniami execute.
*
* @param hDB uchwyt do nawiazanego polaczenia bazodanowego
* @param callbackFunction to funkcja, ktora ma byc wykonywana, gdy execute nie moze sie wykonac
* ze zwgledu na problemy z polaczeniem do bazy danych (moze byc np. wykonany restart polaczenia)
* Mozna podac NULL, aby uniewaznic ustawiony callback dla polaczenia.
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, a w przypadku bledu wartosc < BMD_OK
*/
LIBBMDDB_SCOPE long bmd_db_set_lost_connection_callback(void *hDB, bmd_db_restart_callback callbackFunction);


/**
* @author WSZ
* Funkcja wykonujaca reset polaczenia do bazy danych.
*
* @param hDB to uchwyt do restartowanego polaczenia bazodanowego
* @return Funkcja w przypadku sukcesu zwraca BMD_OK, a w przypadku bledu wartosc < BMD_OK
*/
LIBBMDDB_SCOPE long bmd_db_reconnect(void *hDB);

/**
 * \brief Wystartowanie transakcji bazodanowej
 *
 * Funkcja umozliwia wystartowanie transakcji bazodanowej na okreslonym poziomie.
 * Transakcja moze byc wystartowana z domyslnymi opcjami, a mozna tez okreslic poziom
 * izolacji transakcji. Obecnie poziom transakcji moze byc okreslone jako domyslny lub
 * serializowalny.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param transaction_isolation_level poziom izolacji transakcji bazodanowej.
 * \return status wykonania BMD_OK w przypadku udanego startu transakcji, <BMD_OK gdy nie
 *                          nie udalo sie wystartowac transakcji
 */
LIBBMDDB_SCOPE long bmd_db_start_transaction(void *hDB,long transaction_isolation_level);

/**
 * \brief Zatwierdzenie transakcji bazodanowej
 *
 * Funkcja umozliwia zatwierdzenie transakacji bazodanowej co jest rownoznaczne z zapisem
 * wszystkich poczynionych zmian
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_end_transaction(void *hDB);

/**
 * \brief Wycofanie transakcji bazodanowej
 *
 * Funkcja umozliwia wycofanie transakacji bazodanowej poprzez wykonanie komendy ROLLBACK.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param params parametry do wycofania transakcji bazodanowej - aktualnie nieuzywany parametr
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_rollback_transaction(void *hDB,void *params);

/**
 * \brief Wykonanie zapytania SQL
 *
 * Funkcja umozliwia wykonanie zapytania SQL bez opcji bindowania zmiennych - dlatego nalezy
 * pamietac o odpowiednim escapowaniu danych bedacych kryteriami wyszukiwania
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param sql_query zapytania SQL do wykonania
 * \param rows zmienna gdzie zwrocona bedzie ilosc wierszy wyniku, jesli poda sie NULL ilosc
 *                wierszy nie bedzie zwrocona
 * \param cols zmienna okreslajaca ilosc zwroconych kolumn w wyniku
 * \param hRes uchwyt przechowujacy wynik zapytania SQL
 * \return status wykonania BMD_OK w przypadku powodzenie, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_execute_sql(void *hDB,const char *sql_query,long *rows,long *cols,void **hRes);

/**
 * \brief Wykonania zapytania z opcja bindowania zmiennych
 *
 * Funkcja umozliwia wykonywanie zapytania z opcja bindowania zmiennych. Lista zmiennych bindowanych
 * moze byc dostarczona bezposrednio jako parametry funkcji lub tez byc dostarczona w uchwycie polaczenia
 * z baza danych.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \param SQLQuery zapytanie SQL do wykonania
 * \param nParams ilosc parametrow bindowanych w zapytaniu SQL
 * \param params_values lista bindowanych parametrow zapytania SQL
 * \param ms_rows zmienna gdzie zwrocona bedzie ilosc wierszy wyniku, jesli poda sie NULL ilosc
 *                wierszy nie bedzie zwrocona
 * \param ms_cols zmienna okreslajaca ilosc zwroconych kolumn w wyniku
 * \param hRes uchwyt przechowujacy wynik zapytania SQL
 * \return status wykonania BMD_OK w przypadku powodzenie, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_execute_sql_bind(void *hDB,char *SQLQuery,
											long nParams,const char * const *params_values,
											long *ms_rows,long *ms_cols,void **hRes);
/**
 * \brief Dodawanie zmiennej typu znakowego do listy zmiennych bindowanych
 *
 * Funkcja umozliwia dodanie kolejnej zmiennej do listy zmiennych ktore sa bindowane jako
 * parametry zapytania SQL.
 *
 * \param var zmienna do dodanie
 * \param var_type typ dodawanej zmiennej (jako typ SQL)
 * \param hDB uchwyt do polaczenia z baza danych
 * \param bind_sql_str lancuch przechowujacy bindowana zmienne i jej typ - np. $1::varchar
 * \param bv lista bindowanych zmiennych w zapytaniu SQL
 * \param bc ilosc bindowanych zmiennych w zapytaniu SQL
 * \return status wykonania BMD_OK w przypadku poprawnego dodania, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,char **bind_sql_str,
										char ***bv,long *bc);

/**
 * \brief Dodawanie zmiennej typu long do listy zmiennych bindowanych
 *
 * Funkcja umozliwia dodanie kolejnej zmiennej do listy zmiennych ktore sa bindowane jako
 * parametry zapytania SQL.
 *
 * \param var zmienna do dodanie
 * \param var_type typ dodawanej zmiennej (jako typ SQL)
 * \param hDB uchwyt do polaczenia z baza danych
 * \param bind_sql_str lancuch przechowujacy bindowana zmienne i jej typ - np. $1::varchar
 * \param bv lista bindowanych zmiennych w zapytaniu SQL
 * \param bc ilosc bindowanych zmiennych w zapytaniu SQL
 * \return status wykonania BMD_OK w przypadku poprawnego dodania, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
											 char ***bv,long *bc);

LIBBMDDB_SCOPE long bmd_db_link_bind_var_list(char **var_list,long var_count,void *hDB);

LIBBMDDB_SCOPE long bmd_db_free_bind_var_list(void *hDB);
LIBBMDDB_SCOPE long bmd_db_free_user_bind_var_list(void *hDB);

/**
 * \brief Escape zapytania SQL
 *
 * Funkcja wykonuje escapowanie zapytania SQL
 *
 * \param hDB uchwyt do polaczenie z baza danych
 * \param src wejsciowe zapytanie SQL
 * \param dest zapytanie SQL po wykonaniu escapowania
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_escape_string(void *hDB,const char *src, esc_mode_t mode, char **dest);

/**
 * \brief Pobranie typu bazy danych
 *
 * Funkcja umozliwia pobranie typu bazy danych, ktora jest aktualnie obslugiwana.
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \return typ obslugiwanej bazy danych. Obecnie moze to byc:
 *         @li postgres
 *         @li oracle
 */
LIBBMDDB_SCOPE long bmd_db_connect_get_db_type(void *hDB);

/**
 * \brief Pobiera aktualnego czasu w SQL
 *
 * Funkcja umozliwia pobranie ciagu okreslajacego aktualny czas w dialekcie SQL
 *
 * \param hDB uchwyt polaczenia z baza danych
 * \return ciag okreslajacy aktualny czas w SQL lub NULL w przypadku bledu
 */
LIBBMDDB_SCOPE char *bmd_db_get_actualtime_sql(void *hDB);

/**
 * \brief Pobranie kolejnej wartosci z sekwencji
 *
 * Funkcja umozliwia pobranie kolejnej wartosci z sekwencji o danej nazwie
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param SequenceName nazwa sekwencji z ktorej pobrana ma byc nastepna wartosc
 * \return nastepna wartosc z sekwencji lub NULL w przypadku bledu
 */
LIBBMDDB_SCOPE char *bmd_db_get_sequence_nextval(void *hDB,char *SequenceName);

/**
 * \brief Pobranie aktualnej wartosci z sekwencji
 *
 * Funkcja umozliwia pobranie aktualnej wartosci z sekwencji o danej nazwie
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param SequenceName nazwa sekwencji z ktorej pobrana ma byc aktualna wartosc
 * \return aktualna wartosc z sekwencji lub NULL w przypadku bledu
 */
LIBBMDDB_SCOPE char *bmd_db_get_sequence_currval(void *hDB,char *SequenceName);

/**
 * \brief Pobranie maksymalnej wartosci id z danej tabeli
 *
 * Funkcja umozliwia pobranie maksymalnej wartosci dla id z danej tabeli. Identyfikator
 * id musi znajdowac sie w kolumnie okreslonej przez parametr column_name.
 *
 * \param hDB uchwyt do polaczenia z baza danych
 * \param column_name nazwa kolumny ktora przechowuje id danej tabeli
 * \param table_name nazwa tabeli z ktorej pobierana jest wartosc
 * \param id pobrana wartosc
 * \return status wykonania BMD_OK w przypadku sukcesu, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_get_max_entry(void *hDB,char *column_name,char *table_name,char **id);

/**
 * \brief Konwersja daty na lancuch typu TIMESTAMP
 *
 * Funkcja umozliwia konwersje daty w formacie ISO na lancuch typu TIMESTAMP
 * Wygenerowany lancuch jest mozliwy do umieszczenia tam gdzie w zapytaniu wystepuje
 * zmienne o typie TIMESTAMP
 * \param Date data do umieszczenia w typie TIMESTAMP
 * \param Timestamp wygenerowany lancuch o typie TIMESTAMP
 * \return status wykonania BMD_OK w przypadku poprawnej generacji, <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_date_to_timestamp(const char *Date, char **Timestamp);

/**
 * \brief Generowanie dyrektywy ORDER BY
 *
 * Funkcja umozliwia wygenerowanie dyrektywy ORDER BY na podstawie danych umieszczonych
 * w datagramie. Sortowanie danych moze odbywac sie wedlug wielu kryteriow i roznych
 * kierunkow (jedna metadana w kierunku rosnacym, druga w malejacym)
 * \param dtg datagram zawierajacy kryteria sortowania
 * \param req struktura przechowujace dane obslugiwanego zadania (potrzebne ze wzgledu na formularz uzytkownika)
 * \param order_by wygenerowane kryterium
 * \return status wykonania BMD_OK w przypadku poprawnej generacji,
 *                          <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_generate_order_clause(bmdDatagram_t *dtg, server_request_data_t* req, char **order_by);

/**
 * \brief Stronicowanie zapytan SQL
 *
 * Funkcja generuje zapytanie SQL na podstawie juz istniejacego, ale takie ze umozliwia
 * wyswietlenie porcji wynikow - czyli np. rekordow od 10 do 20. Graniczne numery rekordow
 * sa przekazywane w datagramie i wraz z odpowiednia dyrektywa ORDER BY tworza finalne
 * zapytanie gotowe do wyslanie do serwera bazy danych.
 *
 * \param sql_begin  zapytanie SQL bez parametrow stronicujacych
 * \param order_by zmienna okreslajaca dyrektywe ORDER BY
 * \param dtg datagram zawierajacy kryteria stronicowania
 * \param sql_offset informacja od ktorego rekordu prezentowac wyniki
 * \param sql_limit ilosc wierszy do zwrocenia w wyniku
 * \param sql_final przygotowane, ostateczne zapytanie SQL
 * \return status wykonania BMD_OK w przypadku poprawnego przygotowania zapytania,
                            <BMD_OK w przypadku bledu
 */
LIBBMDDB_SCOPE long bmd_db_paginate_sql_query(char *sql_begin,char *order_by,bmdDatagram_t *dtg,
											  long sql_offset,long sql_limit,char **sql_final, char **sql_limits);

#endif /*_LIBBMDDB_INCLUDED_*/

