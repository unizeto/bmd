<?
session_start();
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
require_once('DAO/com_func.php');
$BMDSOAP = new BMDSOAP();

draw_htmlhead('Count invoice result w SOE SOAP WWW','');
$accountid=$_POST['accountid'];//"eawizo@wp.pl";//mail odbiorcy
$invoiceId=$_POST['invoiceId'];//"55B4E0726086ECA49B2318B620F568A095767D27";//hash pliku
$userIdentifier=$_POST['userIdentifier'];//"eawizo@wp.pl";

$ret = $BMDSOAP->countInvoice($cert, $accountid, $invoiceId, $userIdentifier );

//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów
/*
    header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=" . $ret->filename);
    header("Content-Length: " . strlen($ret->file));
    print($ret->file);
*/
//================================================


    header("Content-type: text/html; charset=utf-8");
    print($ret);

	 require_once "tail.php";

?>
