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

/*  wywołanie serwisu - podajemy:
   - certyfikat
   - tablicę metadanych
   - zawartość pliku
   - nazwę pliku
*/
$debuger=0; //0- off, 1- on
$transakcja=0;
/*wypisanie odpowiedzi z serwera - tekst*/


$filename   = "plik.zip";//jego nazwa widoczna w bazie
$path = "/home/lkosmidek/bmd/pliki/";

$file = fopen($path.$filename,"r") or exit("Unable to open file!");
$calosc = filesize($path.$filename);
//$filename = "setup.1.03.02.01.exe";
//$file = fopen("/var/www/html/play/setup.1.03.02.01.exe","r") or exit("Unable to open file!");
//$calosc = filesize("/var/www/html/play/setup.1.03.02.01.exe");
$wielkosc = 0;
$paczka="2097152"; //"4194304";//2097152";8288608

$mtds = array();

//kolekcjonowanie metadanych w tablicy
/*
$mtdOid     = "1.2.616.1.113527.4.3.3.13";
$mtdValue   = $filename."_".$calosc;
//zmienna mtdDesc w funkcji insertfile nie jest brana pod uwagę, wrzucanie meta danych do bazy jest oparte o Oid
$mtdDesc    = "Typ faktury";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.1";	
$mtdValue   = "faktura_nr_".rand(100000000,999999999);
$mtdDesc    = "Numer faktury";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.6";	
$mtdValue   = "2007-12-28 23:59:59";
$mtdDesc    = "Data wystawienia";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.16";
$mtdValue   = "2008-".rand(1,12)."-".rand(1,28)." 00:00:00";
$mtdDesc    = "Data sprzedazy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.4";	
$mtdValue   = rand(100,999)."-".rand(100,999)."-".rand(10,99)."-".rand(10,99);
$mtdDesc    = "NIP odbiorcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.5";	
$mtdValue   = "<mtdValue>";
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
$mtdValue   = "2008-02-23 12:00:01";
$mtdDesc    = "Data płatności";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.8";	
$mtdValue   = "lkosmidek@unet.pl";
$mtdDesc    = "E-mail odbiorcy";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.9";	
$mtdValue   = "root@localhost";
$mtdDesc    = "E-mail nadawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.11";	
$kwota	    = rand(0,9999)+(rand(0,99)/100);
$mtdValue   = $kwota;
$mtdDesc    = "Kwota netto";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.12";	
$mtdValue   = $kwota*1.22;
$mtdDesc    = "Kwota brutto";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.15";	
$mtdValue   = "98765678909876";
$mtdDesc    = "Do numer";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.14";	
$mtdValue   = "2008-02-26 01:01:26";
$mtdDesc    = "Do data";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
*/


if ($debuger) {
$mojtemp = fopen("mojtemp.big","a") or exit("Unable to open file!");
}
$licznik=1;
//inicjalizacja tranzakcji
if ($transakcja){ 
	$trans = $BMDSOAP->bmdStartTransaction($cert);
	print("Inicjalizacja transakcyjności: ".$trans."\n");
	}
	else 
	{ $trans=NULL; }

while (!feof($file))
  {
        if ( $wielkosc == 0 )
                {
                $ret = $BMDSOAP->bmdInsertFileChunkStart($cert,$mtds,$filename,$calosc,$trans);
                print($ret."\n");
                $tmpfilename=$ret;
                }
        $content=fread($file,$paczka);
        if($content===false) { print("Fread zwrócił FALSE\n"); break; }
//        if (!fwrite($mojtemp,$content)) {print("Nie udało się zapisać do tempa\n");}
        $wielkosc+=strlen($content);
        $koncowka=$calosc - $wielkosc;
        echo "$licznik : $calosc - $wielkosc - $koncowka ";
        //przypadek ostatniej paczki
        if ( $koncowka==0 ) { print("\t ostatnia paczka\n"); $licznik="final";  }
        print("\n");
        $ret = $BMDSOAP->bmdInsertFileChunkIter($cert, $content, $filename, $tmpfilename, $licznik);

if ($debuger) {
	if (!fwrite($mojtemp,$content)) {print("Nie udało się zapisać do tempa\n");}
	}

        $licznik++;
        //profilaktyczne czyszczenie buffora
        $content=NULL;
        //w wypadku timeoutu lub innych błędów
        if ($ret<0) { print("Jakiś błąd z ChunkIter: ".$ret."\n"); break ; }
  }
fclose($file);
if ($debuger) { fclose($mojtemp); }
print("Po wrzuceniu pliku: ".$ret."\n");
//finalizacja tranzakcji

if ($transakcja) {
	print("weryfikcja trans ID: ".$trans."\n");
	$ret = $BMDSOAP->bmdStopTransaction($cert,$trans);
	print("Po zakończeniu transakcji: ".$ret."\n");
	}
/*wypisanie odpowiedzi z serwera - tekst*/
//header("Content-type: text/plain");

?>
