<?
session_start();
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();
/*wywołanie serwisu - podajemy:
   - zakodowany w base64 certyfikat
   - identyfikator pliku
*/

//for ($ID=5;$ID<=24;$ID++){
$ID=23;

$type=0;
$ret = $BMDSOAP->bmdGetFileByID($cert, $ID,$type );
if(is_object($ret)) {

//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów
/*
	 header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=" . $ret->filename);
	 foreach ($ret->file as $obj){
    header("Content-Length: " . strlen($obj));
    print($obj);
	   }
*/
//================================================

    //header("Content-type: text/plain; charset=utf-8");
    print("filename = ".$ret->filename."\n");
	//print($ret->file);
	if (!$uchwyt = fopen("odebrano/".$ret->filename."_".rand(1,10000), 'w')) {
		echo "Nie mogę otworzyć pliku ($ret->filename)";
		exit;
	}//enf if uchwyt

// Zapis $trochetresci do naszego otwartego pliku.
	//if (
	foreach ($ret->file as $obj)
	fwrite($uchwyt, $obj);// === FALSE) {
	//	echo "Nie mogę zapisać do pliku ($ret->filename)";
	//	exit;
	//}//end if fwrite

	fclose($uchwyt);
	  
} //end if else is_array
else {
    //header("Content-type: text/plain; charset=utf-8");
    print("Error: ".$ret);

}//end if else is object
//}//end for
//require_once "tail.php";
?>

