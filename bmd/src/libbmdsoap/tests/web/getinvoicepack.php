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

$accountid = $_POST['accountid'];//"user1@localhost";//mail odbiorcy
$invoiceId = $_POST['invoiceId'];//"87811EE084651CAA301B111E10F9BF70911F6073";//hash pliku
$userIdentifier = $_POST['userIdentifier'];//"nadawca2@localhost";
$getReason = $_POST['getReason'];

$ret = $BMDSOAP->getInvoicePack($cert, $accountid, $invoiceId,$userIdentifier,$getReason );

/*pobranie odpowiedzi z serwera - obiekt z plikiem
    $ret->filename - nazwa pliku
    $ret->file - zawartość pliku
*/
//header("Content-type: text/html; charset=utf-8");

if(is_object($ret)) {

//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów

    header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=" . $ret->filename);
	   foreach($ret->file as $obj){
    header("Content-Length: " . strlen($obj));
    print($obj);}

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
/*	if (fwrite($uchwyt, $ret->file) === FALSE) {
		echo "Nie mogę zapisać do pliku ($ret->filename)";
		exit;
	}//end if fwrite


// Zapis $trochetresci do naszego otwartego pliku.

	foreach($ret->file as $obj){
	    if (fwrite($uchwyt, $obj) === FALSE) {
		echo "Nie mogę zapisać do pliku ($ret->filename)";
		exit;
	    }//end if fwrite
	}


	fclose($uchwyt);
	  */
} //end if else is_array
else {
    //header("Content-type: text/plain; charset=utf-8");
    print($ret);

}//end if else is object
//require_once "tail.php";
?>
