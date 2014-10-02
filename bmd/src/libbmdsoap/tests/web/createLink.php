<?
session_start();

/**
* Pobranie wartości metadanych opisujących plik wskazany przez podany identyfikator pliku ID
*/
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once "certyfikat.php";
require_once('DAO/com_func.php');
$BMDSOAP = new BMDSOAP();
$mtds = array();
/*
print_r($_POST);
print("<br />");
print_r($_FILES);
print("<br />");
*/
$idx=1;

draw_htmlhead('Wynik tworzenia Skrótu w SOE SOAP WWW','');
print("<table style='padding: 5px; border: 1px solid black;'><tbody>");

while ( $idx < $_SESSION['ilosc'] ){
//print_r("<br />".$_POST[$idx]);
$index="wrzuc".$idx;
	if ( !empty($_POST[$index])){
	$mtdValue   = $_POST[$index];
		$mtdOid     = $_SESSION['mtdOid'][$idx-1];	
		$mtdDesc    = $_SESSION['mtdDesc'][$idx-1];
		$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdStatement"=>0);//, "mtdDesc"=>$mtdDesc);
		print("<tr><td style='text-align: right;'>".$idx."</td><td>".$mtdOid."</td><td>".$mtdDesc."</td><td>".htmlspecialchars($mtdValue,ENT_QUOTES)."</td></tr>");
		}
		    
$idx+=1;
}
print("</tbody></table>");

if (isset($_POST['trans']) && $_POST['trans'] == 'on'){
$transID = $BMDSOAP->bmdStartTransaction($cert);
print("<br />Inicjalizacja transakcyjności: ".$transID."\n");
//print("wybrano wysylke z transakcja.<br />");
}else{$transID=NULL;}
$linkName=null;
$description=null;
if (!empty($_POST['linkname'])) {$linkName=$_POST['linkname'];}else{die('Nie podano nazwy skrótu');}
if (!empty($_POST['linkdesc'])) {$description=$_POST['linkdesc'];}else{die('Nie podano opisu');}
if (!empty($_POST['corespdir'])) {$mtds[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.5.7", "mtdValue"=>$_POST['corespdir'], "mtdStatement"=>0);}


$ret = $BMDSOAP->bmdCreateLink($cert, $mtds, $description, $linkName, $transID);

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