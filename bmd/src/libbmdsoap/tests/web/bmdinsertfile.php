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


for ($i=0;$i<10;$i++){

//kolekcjonowanie metadanych w tablicy

$mtdOid     = "1.2.616.1.113527.4.3.3.13";
$mtdValue   = "ęółąśłżźćń ŻŹĆŃĄŚŁÓĘ php soap 080211";
//zmienna mtdDesc w funkcji insertfile nie jest brana pod uwagę, wrzucanie meta danych do bazy jest oparte o Oid
$mtdDesc    = "Typ faktury";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.1";	
$mtdValue   = "faktura_ęółąśłżźćń ŻŹĆŃĄŚŁÓĘ _nr_".rand(100000000,999999999);
$mtdDesc    = "Numer faktury";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.6";	
$mtdValue   = "2007-12-28";
$mtdDesc    = "Data wystawienia";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.16";
$mtdValue   = "2008-".rand(1,12)."-".rand(1,28);
$mtdDesc    = "Data sprzedazy";
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
	
$mtdOid     = "1.2.616.1.113527.4.3.3.7";	
$mtdValue   = "2008-02-23";
$mtdDesc    = "Data płatności";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/	
$mtdOid     = "1.2.616.1.113527.4.3.3.8";	
$mtdValue   = "lkosmidek@unet.pl"; //eawizo@wp.pl";//;pawel.lesniak@unizeto.pl";
$mtdDesc    = "E-mail odbiorcy";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.9";	
$mtdValue   = "eawizo@eup.mpips.gov.pl";
$mtdDesc    = "E-mail nadawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/	
$mtdOid     = "1.2.616.1.113527.4.3.3.11";	
$kwota	    = rand(0,9999)+(rand(0,99)/100);
$mtdValue   = $kwota;
$mtdDesc    = "Kwota netto";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.12";	
$mtdValue   = $kwota*1.22;
$mtdDesc    = "Kwota brutto";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
/*	
$mtdOid     = "1.2.616.1.113527.4.3.3.15";	
$mtdValue   = "98765678909876";
$mtdDesc    = "Do numer";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.14";	
$mtdValue   = "2008-02-26";
$mtdDesc    = "Do data";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.24";
$mtdValue   = "0";
$mtdDesc    = "";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.27";
//$mtdValue   = "/mnt/part2/SVN/bmd/trunk/domains/soe/html/zastepczy2.html";
//$mtdValue   = "http://192.168.144.128/testy/zastepczy2.html";
//$mtdValue   = NULL;
$mtdDesc    = NULL;
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/
$filename   = "file1.pdf";//plik2.test";//jego nazwa widoczna w bazie
$file       = file_get_contents("/home/lkosmidek/bmd/pliki/".$filename);//plik który wysyłamy

/*  wywołanie serwisu - podajemy:
   - certyfikat
   - tablicę metadanych
   - zawartość pliku
   - nazwę pliku
*/
//inicjalizacja tranzakcji 

//$trans = $BMDSOAP->bmdStartTransaction($cert);
//print("Inicjalizacja transakcyjności: ".$trans."\n");

//for ($i=0;$i<100;$i++){

$trans = $BMDSOAP->bmdStartTransaction($cert);
print("Inicjalizacja transakcyjności: ".$trans."\n");

for ($k=0;$k<10;$k++){
$filename="a".$i.$k;//testowo
//$trans=NULL;
//$cert=NULL;
$type=0;
$ret = $BMDSOAP->bmdInsertFile($cert, $mtds, $file, $filename,$trans,$type);

if ($ret<0) 
	{ 
		print("Błąd: ".$ret."\n"); 
	} else 
	{ 
		print("Id pliku: ".$ret."\n"); 
	}
}//end for k

$mtds=array();
$ret = $BMDSOAP->bmdStopTransaction($cert,$trans);
print("Stop trans :".$ret."\n");

}//end for i

//$ret = $BMDSOAP->bmdStopTransaction($cert,$trans);
//print("Stop trans :".$ret."\n");

/*wypisanie odpowiedzi z serwera - tekst*/
//header("Content-type: text/plain");
?>
