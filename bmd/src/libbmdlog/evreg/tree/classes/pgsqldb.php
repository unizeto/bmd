<?
/** 
  * Klasa implementuje podstawowe operacje na bazie danych Postgres
  * \brief implementacja DB dla Postgresa
  */
class PgSqlDB implements DB {

    public $debug = true;

    private $dbhost, $dbport, $dblogin, $dbpassword, $dbname;
    private $db_persistent = false;
    private $error_action = 0;

    protected $db_hdl = NULL;

    function __construct($host,$port,$login,$password,$dbname) {
        $this->dbhost = $host;
        $this->dbport = $port;
        $this->dblogin = $login;
        $this->dbpassword = $password;
        $this->dbname = $dbname;
        file_put_contents('/tmp/test.txt',"CONSTRUCT\n",FILE_APPEND);
        
    }

    function connect() {
         if ($this->db_hdl !== NULL) {
             file_put_contents('/tmp/test.txt',"KNOWN HANDLER\n",FILE_APPEND);
             return 0;
         }

         $login = " host='$this->dbhost' port='$this->dbport' ". 
             " user='$this->dblogin' ".
             " password='$this->dbpassword' dbname='$this->dbname' ".  
             " connect_timeout=10 ";
         if ($this->db_persistent) {
             $this->db_hdl = pg_pconnect($login);
         } else {
             file_put_contents('/tmp/test.txt',"CONNECTING\n",FILE_APPEND);
             $this->db_hdl = pg_connect($login);
         }
        
         if ($this->db_hdl === FALSE) {
            return $this->showError();
         }
         return 0;
    }

    function __destroy() {
//			    pg_close($this->db_hdl);
        if (! $this->db_persistent) {
            //pg_close($this->db_hdl);
        }
//        $this->db_hdl = NULL;
    }

    /** Pokazuje komunikat o błędzie
      * @param $query treść zapytania powodującego błąd
      * @param $result ewentualny wynik zwórcony przez zapytanie
      * @returns numer błędu bazy danych lub -1 (domyślnie)
      */
    function showError($query="", $result = null) {
        if ($result === null) {
            if ($this->debug) echo $query.": ".pg_last_error($this->db_hdl);
        } else {
            if ($this->debug) echo $query.": ".pg_result_error($this->db_hdl);
            return -pg_last_errno();
        }
        return -1;
    }

    function select($table_name, $fields_list, $criteria=" true ", $group='', $order='',
            $offset=0,$limit=1999999999,&$has_more=NULL)  {

        $add_order = " ";
        if ($order != '') {
            $add_order = " ORDER BY $order ";
        }

        $add_group = " ";
        if ($group != '') {
            $add_group = " GROUP BY $group ";
        }

        $query = " SELECT $fields_list FROM $table_name WHERE $criteria ".
                " $add_group $add_order LIMIT ".($limit+1)." OFFSET $offset ";


//        print($query.'<br/><br/>');
//        print($query."\n\n");
//   file_put_contents('/tmp/zapytania.txt',$query."\n",FILE_APPEND);

        $this->connect();

        $result = pg_query ($this->db_hdl, $query);
        if ($result === FALSE ){
            return $this->showError();
        }

        $ret = array();

        if (pg_num_rows($result) > $limit) {
            $has_more = true;
        } else {
            $has_more = false;
            $limit = pg_num_rows($result);
        }

        for ($i=0; $i < $limit; $i++) {
            array_push($ret, pg_fetch_assoc($result));
        }

        return $ret;
    }

    function query($query,$offset=0,$limit=1999999999,&$has_more=NULL)  {

        $query .= " LIMIT ".($limit+1)." OFFSET $offset";

        //print($query.'<br/><br/>');
        //print($query."\n\n");

        $this->connect();

        $result = pg_query ($this->db_hdl, $query);
        if ($result === FALSE ){
            return $this->showError();
        }

        $ret = array();

        if (pg_num_rows($result) > $limit) {
            $has_more = true;
        } else {
            $has_more = false;
            $limit = pg_num_rows($result);
        }

        for ($i=0; $i < $limit; $i++) {
            array_push($ret, pg_fetch_assoc($result));
        }

        return $ret;
    }

    function last_insert_id($tablename, $field) {

        $this->connect(); 
        $result = pg_query ($this->db_hdl, 
            " SELECT currval('" . $tablename . "_id_seq') ");

        $val = FALSE;
        if ($result !== FALSE) {
            $val = pg_fetch_result($result, 0, 0);
        }
        return $val;
    }

    /** Funkcja wykonuje operację wstawienia danych do bazy
      * @param $table_name nazwa tabeli w bazie danych
      * @param $fields_list lista kolumn
      * @param $values wartości do wstawienia
      * @param $noValues jeśli true, to oznacza, że $values zawiera podzapytanie, jeśli false, $values zawiera gotowe wartości
      * @returns ilość wstawionych rekordów
      */
    function insertRow ($table_name, $fields_list, $values, $noValues = false) {
        if($noValues === false) {
            $values = "VALUES ($values) ";
        }
        $query = " INSERT INTO $table_name($fields_list) $values";
        //print($query."<br><br>");
        //return($query);
        $this->connect();
        $result = pg_query ($this->db_hdl, $query);
        
        if ($result === FALSE) {
            return $this->showError($query);
        }
        
        return pg_affected_rows($result);
    }

    function updateRow($table_name, $criteria, $new_values) {
        $query = " UPDATE $table_name SET $new_values WHERE $criteria ";

        //print($query."<br><br>");
        $this->connect();
        $result = pg_query ($this->db_hdl, $query);

        if ($result === FALSE) {
            return $this->showError();
        }

        return pg_affected_rows($result);
    }

    function deleteRow ($table_name, $criteria) {
        $this->connect();

        $counter = $this->select($table_name, "COUNT(*) AS n", $criteria);

        if ($counter[0]['n'] != 1) {
            if ($counter[0]['n'] == 0) 
                return 0; /* nie znaleziono */
            //if ($counter[0]['n'] != 0) 
//                return 2; /* znaleziono zbyt wiele */
        }
        $query = "DELETE FROM $table_name WHERE $criteria ";
        
        $result = pg_query ($this->db_hdl, $query);
        
        if ($result === FALSE) {
            return $this->showError();
        }

        return pg_affected_rows($result);
    }

    /** Funkcja usuwa wszystkie wiersze spełniające kryteria z bazy danych. 
      * @param $table_name nazwa tabeli w bazie danych
      * @param $criteria warunki brzegowe
      * @returns ilość zmienionych rekordów
    */
    function deleteAll ($table_name, $criteria) {
        $this->connect();
        $counter = $this->select($table_name, "COUNT(*) AS n", $criteria);

        if ($counter[0]['n'] != 1) {
            if ($counter[0]['n'] == 0) 
                return 0; /* nie znaleziono */
        }
        $query = "DELETE FROM $table_name WHERE $criteria ";

        //print($query);
        $result = pg_query ($this->db_hdl, $query);
        
        if ($result === FALSE) {
            return $this->showError();
        }

        return pg_affected_rows($result);
    }

    function escape_string($string) {
        return pg_escape_string($string);
    }

    function strip_single_quotes($string) {
        $search  = array("'", "\\'", "\\");
        $replace = array("", "", "");
        return str_replace($search, $replace, $string);
    }

    function encode_blob($blob) {
        return pg_escape_bytea($blob);
    }

    function decode_blob($identifier) {
        return pg_unescape_bytea($identifier);
    }

    /** Funkcja rozpoczyna transakcję
      * @returns null
      */
    function begin() {
        $query = "BEGIN;";
        $this->connect();
        $result = pg_query ($this->db_hdl, $query);
        return null;
    }

    /** Funkcja kończy transakcję
      * @returns null
      */
    function commit() {
        $query = "COMMIT;";
        $this->connect();
        $result = pg_query ($this->db_hdl, $query);
        return null;
    }
}
?>