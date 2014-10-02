<?
session_start();
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

$accountid=$_POST['accountid'];
$invoiceId=$_POST['invoiceId'];
$userIdentifier=$_POST['userIdentifier'];

$forceVerification=0;//pobiera z bazy
if (isset($_POST['forceV']) && ($_POST['forceV'] == 'on' || $_POST['forceV'] == 'checked') ){
$forceVerification=1;//zadanie nowego 
}

switch($_GET['modfunc']){
	
	case 'check':
		
		draw_htmlhead('Verify invoice result w SOE SOAP WWW','');
		$ret = $BMDSOAP->verifyInvoice($cert, $accountid, $invoiceId, $userIdentifier, $forceVerification );
		//header("Content-type: text/html; charset=utf-8");
		print("check".$forceVerification);
		print($ret);
		require_once "tail.php";
		break;
	case 'save':
		//usunac headery 
		//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony urzystkownika -> tylko w ramach testów
		$ret = $BMDSOAP->verifyInvoiceWithDetails($cert, $accountid, $invoiceId, $userIdentifier, $forceVerification );
		print("save".$forceVerification);
		if(is_object($ret)) {
			
    	header("Content-type: application/octet-stream");
    	header("Content-Disposition: attachment; filename=" . $ret->filename);
    	header("Content-Length: " . strlen($ret->file));		
    		print($ret->file);
		}else{
			print($ret);
		}
		
		break;
}
?>
