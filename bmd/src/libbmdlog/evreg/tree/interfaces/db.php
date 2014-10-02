<?
/**
 * Interfejs implementuje podstawowe operacje na bazie danych
 * \brief Baza danych - interfejs
*/
interface DB {
    /**
      * Tworzy nowy obiekt i ustawia parametry połączenia
      * @param $host host
      * @param $port port
      * @param $login login
      * @param $password hasło
      * @param $dbname nazwa bazy
      */
    function __construct($host,$port,$login,$password,$dbname);

    /**
      * Otwiera połączenie z bazą danych
      */
    function connect();

    /**
      * Zamyka połączenie z bazą danych
      */
    function __destroy();

    /** Pokazuje komunikat o błędzie
      */
    function showError();
    
    /** Funkcja wykonuje zapytanie i zwraca jego wynik w formie tablicy 
        asocjacyjnej. Opcjonalne offset to "przesunięcie" początku, a $limit to 
        ilość wyświetalnych wierszy.
      * @param $table_name nazwa tabeli w bazie danych
      * @param $fields_list lista kolumn
      * @param $criteria warunki brzegowe zapytania
      * @param $order kolejność uporządkowania rekordów wynikowych
      * @param $offset offset zapytania
      * @param $limit ograniczenie ilości wyników
      * @param $has_more zmienna zostanie ustawiona na 1, jeśli zapytanie zwróciło więcej wyników niż $limit
      * @returns tablica asocjacyjna zawierająca wyniki zapytania
	  */
	function select($table_name, $fields_list, $criteria=" 1 ", $group='', $order='', $offset=0,$limit=9999999999,&$has_more=NULL);

	/** Funkcja zwraca ID ostatnio wstawionego rekordu
      * @param $table_name nazwa tabeli w bazie danych
      * @param $field_name nazwa pola (nie używane)
      * @returns identyfikator ostatnio wstawionego rekordu
      */

	function last_insert_id($table_name, $field_name);
	/** Funkcja wykonuje operację wstawienia danych do bazy
      * @param $table_name nazwa tabeli w bazie danych
      * @param $fields_list lista kolumn
      * @param $values wartości do wstawienia
      * @returns ilość wstawionych rekordów
      */

	function insertRow ($table_name, $fields_list, $values);
	/** Funkcja uaktualnia wpis w bazie danych. 
      * @param $table_name nazwa tabeli w bazie danych
      * @param $criteria warunki brzegowe
      * @param $new_values nowe wartości do wstawienia
      * @returns ilość zmienionych rekordów
      */

    function updateRow ($table_name, $criteria, $new_values);
	/** Funkcja usuwa pojedynczy wiersz z bazy danych
      * @param $table_name nazwa tabeli w bazie danych
      * @param $criteria warunki brzegowe
      * @returns ilość zmienionych rekordów
      */

    function deleteRow ($table_name, $criteria);
	/** Wyescape'uj string 
      * @param $string łańcuch wejściowy
      * @returns łańcuch wyjściowy
      */

    function escape_string($string);
	/** przygotuj BLOBa i zwróć jego identyfikator zależny od bazy danych 
      * @param $data dane wejściowe
      * @returns dane wyjściowe
      */
	function encode_blob($data);
	/** Odczytaj z bazy BLOB o podanym identyfikatorze $identifier
      * @param $identifier identyfikator bloba
      * @returns dane wyjściowe
      */
	function decode_blob($identifier);
}
?>