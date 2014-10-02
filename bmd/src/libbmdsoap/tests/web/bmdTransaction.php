<?
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

//$ret = $BMDSOAP->bmdStartTransaction($cert);

//usunac headery 
//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów
/*
    header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=" . $ret->filename);
    header("Content-Length: " . strlen($ret->file));
    print($ret->file);
*/
//================================================


    //header("Content-type: text/plain; charset=utf-8");
    //print("Start trans :".$ret."\n");//zrwaca jakis id transakcji
$trans="200807081000472472922029";
$ret = $BMDSOAP->bmdStopTransaction($cert,$trans);

print("Stop trans :".$ret."\n");

//end if else is object

?>
