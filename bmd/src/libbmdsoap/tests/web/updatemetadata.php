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

draw_htmlhead('Aktualizacja metadanych opisujących plik SOE SOAP WWW','');
$idx=1;
print("<table style='padding: 5px;'><tbody>");

while ( $idx <= $_SESSION['ilosc'] ){
//print_r("<br />".$_POST[$idx]);
$index="wrzuc".$idx;
if ( !empty($_POST[$index])){
$mtdValue   = $_POST[$index];
$mtdOid     = $_SESSION['mtdOid'][$idx-1];	
$mtdDesc    = $_SESSION['mtdDesc'][$idx-1];
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc,"mtdStatement"=>0);
print("<tr><td style='text-align: right;>".$idx."</td><td>".$mtdOid."</td><td>".$mtdDesc."</td><td>".$mtdValue."</td></tr>");

}

$idx+=1;
}
print("</tbody></table>");

$updateReason=$_POST['updateReason'];//"update testowy ".rand(1,99999);
$ID=$_POST['ID'];//16;
//print("<br />".$ID." ".$updateReason."");
$ret = $BMDSOAP->bmdUpdateMetadata($cert, $ID, $mtds, $updateReason);
if ($ret<0) 
{ 
	print("Błąd: ".$ret."\n"); 
} else 
{ 
	print("Wykonano z efektem: ".$ret."<br />0 - OK<br />1 - Error\n");
}
require_once "tail.php";

?>