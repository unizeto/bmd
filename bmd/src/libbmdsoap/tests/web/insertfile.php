<?
session_start();
require_once('DAO/com_func.php');
/**
* Pobranie wartości metadanych opisujących plik wskazany przez podany identyfikator pliku ID
*/
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
//require_once "DAO/bmdsoap080513.php";
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();
$mtds = array();
/*
print_r($_POST);
print("<br />");
print_r($_FILES);
print("<br />");
*/
$idx=1;

draw_htmlhead('Wysyłka,update plików w SOE SOAP WWW','');
print("<table style='padding: 5px;'><tbody>");

while ( $idx < $_SESSION['ilosc'] ){
//print_r("<br />".$_POST[$idx]);
$index="wrzuc".$idx;
	if ( !empty($_POST[$index])){
	$mtdValue   = $_POST[$index];
		$mtdOid     = $_SESSION['mtdOid'][$idx-1];	
		$mtdDesc    = $_SESSION['mtdDesc'][$idx-1];
		$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
		print("<tr><td style='text-align: right;'>".$idx."</td><td>".$mtdOid."</td><td>".$mtdDesc."</td><td>".$mtdValue."</td></tr>");
		}
		    
$idx+=1;
}
print("</tbody></table>");

if (isset($_POST['trans']) && $_POST['trans'] == 'on'){
$transID = $BMDSOAP->bmdStartTransaction($cert);
print("<br />Inicjalizacja transakcyjności: ".$transID."\n");
//print("wybrano wysylke z transakcja.<br />");
}else{$transID=NULL;}

//$file = file_get_contents($sciezka);//plik który
$file=file_get_contents($_FILES['filename']['tmp_name']);

$type=0;
$ret=-123456789;
//coresponding id, if id == id dir then insterting to that catalog, otherwise linking file with another file
if (!empty($_POST['corespdir'])) {
	$OID="1.2.616.1.113527.4.3.3.26";
	  	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
	  		$OID="1.2.616.1.113527.4.3.2.5.7";
	  	}
	$mtds[] = array("mtdOid"=>$OID, "mtdValue"=>$_POST['corespdir'], "mtdStatement"=>0);
}


if (isset($_POST['podpis']) && $_POST['podpis'] == 'on'){
	if (isset($_FILES['filename2']) && !empty($_FILES['filename2']['tmp_name']))
	{
		$signature=file_get_contents($_FILES['filename2']['tmp_name']);
		$ret = $BMDSOAP->bmdInsertSignedFile($cert, $mtds, $file, $_FILES['filename']['name'], $signature, $transID);
	}else{print("<p>Wybrano wysyłkę pliku z podpisem jednak go nie podano!</p>");}
}
else
{
$ret = $BMDSOAP->bmdInsertFile($cert, $mtds, $file, $_FILES['filename']['name'], $transID, $type);
}

if ($ret<0) 
{ 
print("<br />Błąd: ".$ret."\n"); 
} else 
{ 
print("<br />Id pliku: ".$ret."\n"); 
}

if ( isset($_POST['trans']) && $_POST['trans'] == 'on'){
$ret = $BMDSOAP->bmdStopTransaction($cert,$transID);
print("<br />Stop trans :".$ret."\n");
//print("koniec transakcji.<br />");
}
require_once "tail.php";


?>