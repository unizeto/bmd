<?
session_start();
/**
 * Pobranie wartości metadanych opisujących plik wskazany przez podany identyfikator pliku ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();
//$cert = file_get_contents("cert/marua.der");

/*  wywołanie serwisu - podajemy:
   - zakodowany w base64 certyfikat
   - identyfikator pliku
   - parametr/flaga: 0 - Podpis, 1 - Znacznik Czasu
*/
$opcja=$_POST['TYP'];//0;
$ID=$_POST['ID'];//67;

$ret = $BMDSOAP->bmdGetFilePropByID($cert,$ID,$opcja);#$obj->id,1);

/*pobranie odpowiedzi z serwera - obiekt z plikiem
    $ret->filename - nazwa właściwości: podpis lub znacznik czasu
    $ret->file - zawartośc właściwości, kodowana w base64
*/
header("Content-type: text/html; charset=utf-8");
if(is_object($ret)) {

		    //header("Content-type: text/plain; charset=utf-8");
			 //print($ret);
			       header("Content-type: application/octet-stream");
					 header('Content-Disposition: attachment; filename="' . $ret->filename.'"');
					 //foreach ($ret->file as $obj){
					 header("Content-Length: " . strlen($ret->file->_));//$obj));
					 print($ret->file->_);//$obj);
					 //          }
			       /*
			$liczba=rand(3333,9999);
			$typ=".tsr";
			   if ( $opcja == 0  )
			   {
				$typ=".sig";
			   }
				
				if (!$uchwyt = fopen("odebrano/".$ret->filename."_".$liczba.$typ, 'w')) {
					echo "Nie mogę otworzyć pliku ($ret->filename.$typ) \n";
					exit;
				}

				// Zapis $trochetresci do naszego otwartego pliku.
				if (fwrite($uchwyt, $ret->file) === FALSE) {
					echo "Nie mogę zapisać do pliku ($ret->filename.$typ) \n";
					exit;
				}

				fclose($uchwyt);
			   
			
			*/
} else {
		    //header("Content-type: text/plain; charset=utf-8");
		    print($ret."\n");
}
#}//end if empty
//require_once "tail.php";
?>
