<?
session_start();
/**
 * Pobranie timestampa
 */
ini_set("soap.wsdl_cache_enabled", "0");
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
/*obiekt klasy pomocniczej*/
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();
/*  wywołanie serwisu - podajemy zakodowany w base64 plik do timestampa
*/
$file     = file_get_contents($_FILES['filename']['tmp_name']);

$ret = $BMDSOAP->bmdGetTimeStamp($file,$_FILES['filename']['name']);
/*pobranie odpowiedzi z serwera - tablica obiektów(rodzajów metadanych)
$BMDSOAP   = new BMDSOAP($soap_cert, $soap_location); 
            $timestamp = $BMDSOAP->bmdGetTimeStamp(base64_encode($data),$name); 

            $created   = $timestamp->genTime; 
            $timestamp = base64_encode($timestamp->timestamp); 




*/
//$nazwapliku=$filename.".tsr";//TimeStampedFile
if(is_object($ret)) {
header("Content-type: application/octet-stream");
header("Content-Disposition: attachment; filename=".$_FILES['filename']['name'].".tsr");
//foreach ($ret->file as $obj){
header("Content-Length: " . strlen($ret->timestamp));
print($ret->timestamp);
//}
/*
    //header("Content-type: text/plain; charset=utf-8");
    //print($ret);
	if (!$uchwyt = fopen("odebrano/".$nazwapliku, 'w')) {
		echo "Nie mogę otworzyć pliku ($nazwapliku)";
		exit;
	}

// Zapis $trochetresci do naszego otwartego pliku.
	if (fwrite($uchwyt,($ret->timestamp)) === FALSE) {
		echo "Nie mogę zapisać do pliku ($nazwapliku)";
		exit;
		}
   */
/*
	print("Wystawiono timestampa o: ".$ret->genTime."\n");
	fclose($uchwyt);
*/
//$_SESSION['genTime']=$ret->genTime;
}else{
print($ret);
}//end is object
//require_once "tail.php";

?>
