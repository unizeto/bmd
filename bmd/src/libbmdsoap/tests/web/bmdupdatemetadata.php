<?
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

//kolekcjonowanie metadanych w tablicy
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.1";	
$mtdValue   = "faktura_ęółąśłżźćń ŻŹĆŃĄŚŁÓĘ _nr_".rand(100000000,999999999);
$mtdDesc    = "Numer faktury";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.6";	
$mtdValue   = "2007-12-28";
$mtdDesc    = "Data wystawienia";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.4";	
$mtdValue   = rand(100,999)."-".rand(100,999)."-".rand(10,99)."-".rand(10,99);
$mtdDesc    = "NIP odbiorcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.5";	
$mtdValue   = "Modliszka";
$mtdDesc    = "Nazwa odbiorcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.2";	
$mtdValue   = "987-633-33-33";
$mtdDesc    = "NIP wystawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.3";	
$mtdValue   = "Gżegżółka Brzęczyszczykiewicz Konstantynopolitańczykiewicz Nowak ".rand(1,99999);
$mtdDesc    = "Nazwa wystawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/
$mtdOid     = "1.2.616.1.113527.4.3.3.8";	
$mtdValue   = "lkosmidek@unet.pl";//;pawel.lesniak@unizeto.pl";
$mtdDesc    = "E-mail odbiorcy";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.9";	
$mtdValue   = "plesniak@unet.pl";
$mtdDesc    = "E-mail nadawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.15";	
$mtdValue   = "98765678909876";
$mtdDesc    = "Do numer";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.14";	
$mtdValue   = "2008-02-26";
$mtdDesc    = "Do data";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
//inicjalizacja tranzakcji 

//$trans = $BMDSOAP->bmdStartTransaction($cert);
//print("Inicjalizacja transakcyjności: ".$trans."\n");
$updateReason=$_POST['updateReason'];//"update testowy ".rand(1,99999);
$ID=$_POST['ID'];//16;

$ret = $BMDSOAP->bmdUpdateMetadata($cert, $ID, $mtds, $updateReason);
if ($ret<0) 
	{ 
		print("Błąd: ".$ret."\n"); 
	} else 
	{ 
		print("Id pliku: ".$ret."\n"); 
	}

?>
