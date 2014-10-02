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

$accountid = $_POST['accountid'];//"rafal.jarczak@playmobile.pl";
$invoiceId = $_POST['invoiceId'];//"0F5F938F1401E0D2214FBDB61CBC3FC6CA134A89";
$userIdentifier = $_POST['userIdentifier'];//"rafal.jarczak@playmobile.pl";
$getReason = $_POST['getReason'];

$ret = $BMDSOAP->getHTMLImage($cert, $accountid, $invoiceId, $userIdentifier,$getReason );

//header("Content-type: text/html; charset=utf-8");

if(is_object($ret)) {
//print_r($ret);
//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów

    header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=".$ret->filename);
	 foreach($ret->file as $obj){
    header("Content-Length: " . strlen($obj));
    print($obj);
	   }

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

	fclose($uchwyt);
	  */
} //end if else is_array
else {
    //header("Content-type: text/plain; charset=utf-8");
    print($ret);

}//end if else is object
//require_once "tail.php";
?>

