<?
session_start();
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$HASH=$_POST["ID"];
$BMDSOAP = new BMDSOAP();
$ret = $BMDSOAP->bmdGetFileByHash($cert, $HASH );#$obj->getFileURL);
header("Content-type: text/html; charset=utf-8");
if(is_object($ret)) {

//usunac headery 
	
    header("Content-type: application/octet-stream");
    header('Content-Disposition: attachment; filename="'. $ret->filename.'"');
	//   foreach ($ret->file as $obj){
    header("Content-Length: " .strlen($ret->file->_));//$obj) );
	 print($ret->file->_);//$obj);
	 //  }
	
/*	

	if (!$uchwyt = fopen("odebrano/".$ret->filename."_".rand(1,10000), 'w')) {
		echo "Nie mogę otworzyć pliku ($ret->filename)";
		exit;
	}

// Zapis $trochetresci do naszego otwartego pliku.
	if (fwrite($uchwyt, $ret->file) === FALSE) {
		echo "Nie mogę zapisać do pliku ($ret->filename)";
		exit;
	}

fclose($uchwyt);
*/	
} else {
    //header("Content-type: text/plain; charset=utf-8");
    print($ret);
}
//$ret=NULL;//zerujemy 
#}//end if empty
//end for
//require_once "tail.php";
?>
