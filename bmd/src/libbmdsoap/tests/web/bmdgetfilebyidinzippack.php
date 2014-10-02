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

$ID=$_POST['ID'];//15;
$ret = $BMDSOAP->bmdGetFileByIdInZipPack($cert, $ID,0);

header("Content-type: text/html; charset=utf-8");
if(is_object($ret)) {

//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów

    header("Content-type: application/octet-stream");
    header('Content-Disposition: attachment; filename="'. $ret->filename.'"');
	header("Content-Length: " . strlen($ret->file->_));
    print($ret->file->_);
	  

//================================================

/*
    //header("Content-type: text/plain; charset=utf-8");
    print($ret->filename);

	//print($ret->file);
	if (!$uchwyt = fopen("odebrano/".rand(1,10000)."_".$ret->filename, 'w')) {
		echo "Nie mogę otworzyć pliku ($ret->filename)";
		exit;
	}//enf if uchwyt

// Zapis $trochetresci do naszego otwartego pliku.
	if (fwrite($uchwyt, $ret->file) === FALSE) {
		echo "Nie mogę zapisać do pliku ($ret->filename)";
		exit;
	}//end if fwrite

fclose($uchwyt);*/
} //end if else is_array
else {
    //header("Content-type: text/plain; charset=utf-8");
    print($ret);

}//end if else is object
//require_once "tail.php";
?>

