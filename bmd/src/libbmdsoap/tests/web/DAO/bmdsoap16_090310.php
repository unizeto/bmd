<?
//session_start();
require_once('DAO/com_func.php');
if ( !isset($_SESSION['SOAPaddress'] ) || empty($_SESSION['SOAPaddress']) ) { 
draw_htmlhead('Komunikat SOE SOAP WWW','');
print('<div class="glowny wysrodkuj">Nie podano adresu serwera usług SOAP.<br /> Proszę wrócić na stronę główną logowania lub kliknąć <a href="index.php">tutaj</a>
</div>');	

require_once("tailtree.php");	
	die(); 
}

/**
 * Klasa pośrednicząca poprzez SOAP z BMD
 * \brief Obsługa SOAP dla BMD dla PHP5
 */
class BMDSOAP {

    /**
     * Zmienna przechowująca uchwyt obiektu "klient"
     */
    public $client;
    
    /**
     * Konstruktor klasy
     * @returns null
     */
    function __construct() {
        ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
        $this->client = new SoapClient("./lib/ns16_090310.wsdl",array("trace" => 1 ,
		      "location" => $_SESSION['SOAPaddress'],
                                                            "soap_version"  => SOAP_1_1,
                                                            "features" => SOAP_SINGLE_ELEMENT_ARRAYS,
                                                            "exceptions" => 1,
                                                            "encoding" => "UTF-8"
                                                            //"local_cert" => "/home/mizia/public_html/soap_bmd/cert/apache.pem",
                                                            //"passphrase" => "12345678"
                                                            ));
        return null;
    }
	 
	   /**
     * Testuje połączenie
     * @returns string z odpowiedzią w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function testConnection() {
        try {
            $return = $this->client->testConnection();
            return $return->result;
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	  /**
     * Pobranie listy metadanych charakteryzujących dane przechowywane w archiwum po zalogowaniu się użytkownika do serwera przez podanie certyfikatu
     * @param $base64Cert certyfikat użytkownika zakodowany w base64
     * @returns tablicę obiektów zawierających identyfikatory OID i opisy metadanych dokumentów
     */
    function bmdLogin($base64Cert) {
        try {
            $params = array("cert"=>$base64Cert);
            $return = $this->client->bmdLogin($params);
            return($return->userMtds->item);
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
             		$detail = ": " . $exception->detail;
            	}
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	  /**
     * Pobranie wartości metadanych plików spełniających kryteria wyszukiwania
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $params tablica kryteriów wyszukiwania
     * @param $limit - ilość dokumentów do zwrócenia z bazy
     * @param $offset - indeks pierwszego dokumentu do zwrócenia
     * @returns tablicę obiektów zawierających identyfikatory OID i opisy metadanych dokumentów
     */
    function bmdSearchFiles($cert, $params, $formMtds, $offset,$limit, $logical, $oper, $range, $sortInfo) {
        try {
            $params = array("cert"=>$cert,"mtds"=>$params,"formMtds"=>$formMtds,  "offset"=>$offset, "limit"=>$limit, "logical"=>$logical, "oper"=>$oper, "range"=>$range, "sortInfo"=>$sortInfo);
            $return = $this->client->bmdSearchFiles($params);
				return($return->searchResults->item);
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	  /**
     * Wrzuca plik do bazy
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $mtds tablica metadanych o określonej strukturze
     * @param $mtdValue metadana - wartość
     * @param $metadana zawartość pliku
     * @returns obiekt z właściwościami w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdInsertFile($cert, $mtds, $file, $filename,$transactionId,$type) {
        try {
            //zawartość pliku
            $input = array("file"=>$file, "filename"=>$filename);
            //właściwa struktura
            $params = array("cert"=>$cert, "mtds"=>$mtds, "input" => $input,"transactionId"=>$transactionId,"type"=>$type);
            $return = $this->client->bmdInsertFile($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
            if(!empty($methods['id'])) {
                return $return->id;
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	/*if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{*/ // bo ta funkcja nie ma rozroznienia
					$detail=$exception->detail;
            	//}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }
	 
	  /**
     * Wrzuca plik z  plikeim podpisu do bazy
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $mtds tablica metadanych o określonej strukturze
     * @param $mtdValue metadana - wartość
     * @param $metadana zawartość pliku
     * @returns obiekt z właściwościami w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdInsertSignedFile($cert, $mtds, $file, $filename,$filesignature,$trans) {
        try {
            //zawartość pliku
            $input = array("file"=>$file, "filename"=>$filename);
            //właściwa struktura
            $params = array("cert"=>$cert, "mtds"=>$mtds, "input" => $input , "binarySignature"=> $filesignature,"transactionId"=>$trans,"type"=>0 );
            $return = $this->client->bmdInsertSignedFile($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
            if(!empty($methods['id'])) {
                return $return->id;
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj;
					} //trunk
            	}else{
				$detail=$exception->detail; //1.7
            	}				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 /**
     * Usuwa plik o podanym identyfikatorze
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $id identyfikator pliku
     * @returns obiekt z zawartością pliku w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdDeleteFileByID($cert, $id) {
        try {
            $params = array("cert"=>$cert, "id"=>$id);
            $return = $this->client->bmdDeleteFileByID($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
			//print($methods);
            if(!empty($methods['result'])) {
                return $return->result;
				
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 /**
     * Pobiera wybrane właściwości pliku o podanym identyfikatorze
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $SHA1 hash pliku
     * @param $property wybór właściwości: 1 - Podpis, 2 - Znacznik Czasu
     * @returns obiekt z właściwościami w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdGetFilePropByID($cert, $id, $property) {
        try {
		    $params = array("cert"=>$cert, "id"=>$id, "property"=>$property);
		    $return = $this->client->bmdGetFilePropByID($params);
		    return ($return->output);
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
            		$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 /**
     * Pobranie timestampa dla wybranego pliku
     * @param $file treść pliku zakodowana w base64
     * @param $filename nazwa pliku
     * @returns obiekt zawierający wartość timestampa i godzine
     */
    function bmdGetTimeStamp($file,$filename) {
        try {
            $input = array("file" => $file, "filename"=>$filename);
            $params = array("input" => $input);
            $return = $this->client->bmdGetTimeStamp($params);
            return($return->output);
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }
	 
	/** 
	*Pobranie drzewa wartości dowodowych dla wskazanego pliku
	*@param $id odentyfikator pliku w bazie
	*@param $binaryCert certyfikat logującego
	*@returns obiekt zawirający timestampa pliku ,podpis, dvcs i timestampy do dvcs i podpisu
	*/ 
	 
	 function bmdGetFilePKIById($id,$base64Cert) {
	try {
            $params = array("id"=>$id,"cert"=>$base64Cert);
		//print_r($params);
            $return = $this->client->bmdGetFilePKIById($params);
		//print("return".$return);
            $methods = (get_object_vars($return));
		//print("methods".$methods);
//            if(!empty($methods['output'])) {
                return $return->output;
//            }
//echo "request";
//	echo $client->__getLastRequest();
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);

//echo "response";
//        echo $client->__getLastResponse();
 

        }
        return null;

	}
	 
	   /**
     * Wrzuca plik do bazy
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $fileslist tablica z plikami 
     * @returns obiekt z wartościami id przyporządkowanymi do każdego wrzuconego z listy pliku 
     */
    function bmdInsertFileList($cert, $fileslist,$transactionId) {
        try {
				///////////po stronie klietna- budowa paczki/tablicy plików//////////////
            //zawartość pliku
				//$input = array("file"=>$file, "filename"=>$filename);
            //właściwa struktura
				//$fileslist = array ("mtds"=>$mtds, "input" => $input);
				/////////////////////////////////////////////////////
				$params = array("cert"=>$cert, "input" => $fileslist,"transactionId"=>$transactionId );
            $return = $this->client->bmdInsertFileList($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
            if(!empty($methods['ids'])) {
                return $return->ids;
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }
	 
	 /**
     * Pobiera plik o podanym skrótem
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $SHA1 hash pliku
     * @returns obiekt z zawartością pliku w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdGetFileByHash($cert, $SHA1) {
        try {
            $params = array("cert"=>$cert, "hash"=>$SHA1);
            $return = $this->client->bmdGetFileByHash($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }	
	

 /**
     * Pobiera plik o podanym identyfikatorze
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $ident ID pliku
     * @returns obiekt z zawartością pliku w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
 
	 function bmdGetFileById($cert, $ident,$type) {
        try {
            $params = array("cert"=>$cert, "id"=>$ident,"type"=>$type);
            $return = $this->client->bmdGetFileById($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	/**
     * Pobiera plik zip o podanym identyfikatorze
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $ident ID pliku
     * @returns obiekt z zawartością pliku zip w przypadku powodzenia, string z błędem w przypadku niepowodzenia
	*w zipie  cała paczka : plik ze wszystkimi dowodami
     */
 
	 function bmdGetFileByIdInZipPack($cert, $ident,$type) {
        try {
            $params = array("cert"=>$cert, "id"=>$ident,"type"=>$type);
            $return = $this->client->bmdGetFileByIdInZipPack($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	 	    /**
			 OPIS DO POPRAWIENIA
     * Pobranie  paczki faktur  (pdf , html i co tam jeszcze )
	* @returns  zip z fakturami w kilku formatach ?
     */
	 function getInvoicePack($cert, $accountid, $invoiceId,$userIdentifier,$getReason ) {
        try {
            $params = array("cert"=>$cert, "accountId" => $accountid , "invoiceId" => $invoiceId,"userIdentifier"=>$userIdentifier,"getReason"=>$getReason,"type"=>0);
            $return = $this->client->getInvoicePack($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
        			foreach($exception->detail as $obj){
                		$detail = ": " . $obj; //$exception->detail;
                	}
            	}else{
                	$detail=$exception->detail;
            	}
            }            
            return($exception->faultstring ." --- ". $detail);
        }
        return null;
    }

	 	    /**
			 OPIS DO POPRAWIENIA
     * Pobranie  faktury w formacie HTML
	* @returns  plik html z fakturą
     */
	 function getHTMLImage($cert, $accountid, $invoiceId,$userIdentifier,$getReason ) {
        try {
            $params = array("cert"=>$cert, "accountId" => $accountid , "invoiceId" => $invoiceId,"userIdentifier"=>$userIdentifier,"getReason"=>$getReason,"type"=>0 );
            $return = $this->client->getHTMLImage($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

		
		   /**
     * Wrzuca plik do bazy
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $mtds tablica metadanych o określonej strukturze
     * @param $mtdValue metadana - wartość
     * @param $metadana zawartość pliku
     * @returns obiekt z właściwościami w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdInsertFileChunkStart($cert,$mtds,$finalfilename,$tot_size,$transactionId) {
        try {
            //zawartość pliku
            //właściwa struktura
            $params = array("cert"=>$cert,"mtds"=>$mtds,"finalfilename"=>$finalfilename,"totalfilesize"=>$tot_size,"transactionId"=>$transactionId);
            $return = $this->client->bmdInsertFileChunkStart($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
            if(!empty($methods['tmpfilename'])) {
                return $return->tmpfilename;
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }	
		
		   /**
     * Wrzuca plik do bazy
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $mtds tablica metadanych o określonej strukturze
     * @param $mtdValue metadana - wartość
     * @param $metadana zawartość pliku
     * @returns obiekt z właściwościami w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */
    function bmdInsertFileChunkIter($cert, $file, $filename,$tmpfilename,$licznik) {
        try {
            //zawartość pliku
            $input = array("file"=>$file, "filename"=>$filename);
            //właściwa struktura
            $params = array("cert"=>$cert, "input" => $input,"tmpfilename"=>$tmpfilename,"count"=>$licznik);
            $return = $this->client->bmdInsertFileChunkIter($params);

            //jesli jest zwracany result
            $methods = (get_object_vars($return));
            if(!empty($methods['result'])) {
                return $return->result;
            }
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
            		$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }	

 /**
     * Pobiera plik o podanym identyfikatorze
     * @param $cert certyfikat użytkownika zakodowany w base64
     * @param $ident ID pliku
     * @returns obiekt z zawartością pliku w przypadku powodzenia, string z błędem w przypadku niepowodzenia
     */

    function bmdGetFileChunkStart($cert, $ident) {
        try {
            $params = array("cert"=>$cert, "id"=>$ident);
            $return = $this->client->bmdGetFileChunkStart($params);
                                        return $return->tmpdir;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
            		$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

	function bmdGetFileChunkIter($cert, $tmpdir, $_count) {
        try {
            $params = array("cert"=>$cert, "tmpdir"=>$tmpdir,"count"=>$_count);
            $return = $this->client->bmdGetFileChunkIter($params);
                                        return $return->output;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 /**
                         OPIS DO POPRAWIENIA
     * Pobranie  informacji z DVCS
        * @returns  int
     */
    function verifyInvoice($cert, $accountid, $invoiceId, $userIdentifier, $forceVerification ) {
        try {
            $params = array("cert"=>$cert, "accountId" => $accountid , "invoiceId" => $invoiceId,"userIdentifier"=>$userIdentifier, "forceVerification"=>$forceVerification ,"type"=>0);
            $return = $this->client->verifyInvoice($params);
                                        return $return->result;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

    
	function verifyInvoiceWithDetails($cert, $accountid, $invoiceId, $userIdentifier, $forceVerification) {
        try {
            $params = array("cert"=>$cert, "accountId" => $accountid , "invoiceId" => $invoiceId,"userIdentifier"=>$userIdentifier, "forceVerification"=>$forceVerification ,"type"=>0);
            $return = $this->client->verifyInvoiceWithDetails($params);
					return $return->output;
				} catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }
    
/**
                         OPIS DO POPRAWIENIA
     * Pobranie  informacji o liczbe plików spełniających kryteria szukania
        * @returns  int
     */
    function countInvoice($cert, $accountid, $invoiceId, $userIdentifier) {
        try {
            $params = array("cert"=>$cert, "accountId" => $accountid , "invoiceId" => $invoiceId,"userIdentifier"=>$userIdentifier,"type"=>0);
            $return = $this->client->countInvoice($params);
                                        return $return->count;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
            		$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 /*
        Rozpoczęcie transakcji
        */
    function bmdStartTransaction($cert) {
        try {
            $params = array("cert"=>$cert);
            $return = $this->client->bmdStartTransaction($params);
                                        return $return->transactionId;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

         /*
        Zakończenie transakcji
        */
    function bmdStopTransaction($cert,$transactionId) {
        try {
            $params = array("cert"=>$cert,"transactionId"=>$transactionId);
            $return = $this->client->bmdStopTransaction($params);
                                        return $return->result;
                                } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 	function bmdHistory($cert, $ident) {
        try {
				
            $params = array("cert"=>$cert, "id"=>$ident);

            $return = $this->client->bmdHistory($params);
//print_r($return);
            return $return->historyResults->item;
            } catch (SoapFault $exception) {
	//			print_r($this->client);
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				            
            }
				return($exception->faultstring . $detail);
        }
        return null;
    }

	function bmdUpdateMetadata($cert, $ident, $params, $updateReason) {
        try {
            $params = array("cert"=>$cert,"id"=>$ident,"mtds"=>$params, "updateReason"=>$updateReason,"type"=>0);
            $return = $this->client->bmdUpdateMetadata($params);
            return($return->result);
        } catch (SoapFault $exception) {
//print_r($this->client);
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
				$detail=$exception->detail;
            	}
				            
            }
            return($exception->faultstring . $detail);
        }
        return null;
    }

 
    function bmdSearchFilesCSV($cert, $params, $formMtds, $offset,$limit, $logical, $oper, $sortInfo) {
        try {
            $params = array("cert"=>$cert,"mtds"=>$params,"formMtds"=>$formMtds,  "offset"=>$offset, "limit"=>$limit, "logical"=>$logical, "oper"=>$oper, "sortInfo"=>$sortInfo);
            $return = $this->client->bmdSearchFilesCSV($params);
				return($return->output);
        } catch (SoapFault $exception) {
            $methods = (get_object_vars($exception));
            $detail = null;
            if(!empty($methods['detail'])) {
            	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
            	}else{
					$detail=$exception->detail;
            	}
				}            
            return($exception->faultstring . $detail);
        }
        return null;
		  }

	function bmdCreateDirectory($cert, $mtds, $description, $dirName, $transactionId){

			try {
			$params = array("cert"=>$cert,"mtds"=>$mtds, "description"=>$description, "dirName"=>$dirName, "transactionId"=>$transactionId);
			$return = $this->client->bmdCreateDirectory($params);
			return($return->id);
			} catch (SoapFault $exception) {
				$methods = (get_object_vars($exception));
				$detail = null;
				if(!empty($methods['detail'])) {
					if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
					}else{
					$detail=$exception->detail;
					}
				}
					return($exception->faultstring . $detail);
				}
				return null;
			}

	function bmdGetDirectoryDetails($cert,$ID){
			try {
			$params = array("cert"=>$cert,"id"=>$ID);
			$return = $this->client->bmdGetDirectoryDetails($params);
			return($return->output);
			} catch (SoapFault $exception) {
			$methods = (get_object_vars($exception));
			$detail = null;
			if(!empty($methods['detail'])) {
				if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					foreach($exception->detail as $obj){
						$detail = ": " . $obj; //$exception->detail;
					}
				}else{
					$detail=$exception->detail;
				}
				}
				return($exception->faultstring . $detail);
				}
				return null;
			}
	 
}//end class declaration
?>
