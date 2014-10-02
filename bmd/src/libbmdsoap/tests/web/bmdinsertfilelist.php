<?
/**
 * Pobranie wartości metadanych opisujących plik wskazany przez podany identyfikator pliku ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

for ($i=1;$i<=5;$i++){

$mtds = array();

//kolekcjonowanie metadanych w tablicy

$mtdOid     = "1.2.616.1.113527.4.3.3.13";
$mtdValue   = "play 1574";
//zmienna mtdDesc w funkcji insertfile nie jest brana pod uwagę, wrzucanie meta danych do bazy jest oparte o Oid
$mtdDesc    = "Typ faktury";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.1";	
$mtdValue   = "play 1574 signed";
$mtdDesc    = "Numer faktury";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.6";
$mtdValue	= "2008-02-23";
$mtdDesc    = "Data wystawienia";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.16";
$mtdValue   = "2008-".rand(1,5)."-".rand(1,28);
$mtdDesc    = "Data sprzedazy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.4";	
$mtdValue   = rand(100,999)."-".rand(100,999)."-".rand(10,99)."-".rand(10,99);
$mtdDesc    = "NIP odbiorcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.5";	
$mtdValue   = "qwerty";
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
	
$mtdOid     = "1.2.616.1.113527.4.3.3.8";	
$mtdValue   = "user0@localhost";
$mtdDesc    = "E-mail odbiorcy";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.9";	
$mtdValue   = "root@localhost";
$mtdDesc    = "E-mail nadawcy";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.11";	
$kwota	    = rand(0,9999)+(rand(0,99)/100);
$mtdValue   = " ".$kwota;
$mtdDesc    = "Kwota netto";	
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.12";	
$mtdValue   = " ".$kwota*1.22;
$mtdDesc    = "Kwota brutto";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.15";	
$mtdValue   = "98765678909876";
$mtdDesc    = "Do numer";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);
	
$mtdOid     = "1.2.616.1.113527.4.3.3.14";	
$mtdValue   = "2008-02-26";
$mtdDesc    = "Do data";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$mtdOid     = "1.2.616.1.113527.4.3.3.52";	
$mtdValue   = "987-633-33-33";
$mtdDesc    = "Numer konta";
$mtds[] = array("mtdOid"=>$mtdOid, "mtdValue"=>$mtdValue, "mtdDesc"=>$mtdDesc);

$filename   = "plik_145".$i."_0.odt";//jego nazwa widoczna w bazie
$file       = file_get_contents("wysylka/".$filename);//plik który wysyłamy



///////////po stronie klietna- budowa paczki/tablicy plików//////////////

//Dla zwielokrotnienia ilości plików należy dodać petle dla każdego pliku w katalogu (foreach) lub inkrementacje po nazwie pliku jeśli zawiera serujna numeracje.

           //zawartość pliku
$fileinfo = array("file"=>$file, "filename"=>$filename);

            //właściwa struktura
$fileslist[] = array ( "mtds"=>$mtds, "file" => $fileinfo);
				/////////////////////////////////////////////////////
}//end for
/*  wywołanie serwisu - podajemy:
   - certyfikat
   - tablicę metadanych
   - zawartość pliku
   - nazwę pliku
*/

$ret = $BMDSOAP->bmdInsertFileList($cert, $fileslist,NULL);


/*wypisanie odpowiedzi z serwera - tekst*/
//header("Content-type: text/plain");

if (is_object($ret))
{

$liczba=count($ret->item);
print("Liczba plików :".$liczba."\n");

for($i=0;$i<$liczba;$i++) {
	print("id: ".$ret->item[$i]."\n");
	}

}else{
print("Błąd : ".$ret);//do poprawy , $ret jest obiektem zawierającym listę id wrzuconych plików
}

?>
