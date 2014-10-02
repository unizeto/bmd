<?
session_start();
require_once('DAO/com_func.php');
/**
* Pobranie wartości metadanych opisujących plik wskazany przez podany identyfikator pliku ID
*/
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

$idx=1;

draw_htmlhead('Wysyłka,update plików w SOE SOAP WWW','');

$ret=-123456789;

if (isset($_FILES['filename2']) && !empty($_FILES['filename2']['tmp_name']))
{
	$signature=file_get_contents($_FILES['filename2']['tmp_name']);
	$ret = $BMDSOAP->bmdAddSignature($cert, $signature, $_POST['corespdir']);
}else{
	print("<p>Wybrano wysyłkę pliku z podpisem jednak go nie podano!</p>");
}

if ($ret<0) 
{ 
	print("<br />Błąd: ".$ret."\n"); 
} else 
{ 
	print("<br />Id pliku: ".$ret."\n"); 
}

require_once "tail.php";
?>