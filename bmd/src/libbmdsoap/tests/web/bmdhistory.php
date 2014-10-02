<?
session_start();
/**
 * Pobranie wartości metadanych plików spełniających kryteria wyszukiwania.
 */

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');
#require_once "DAO/bmdsoap080513.php";
require_once "certyfikat.php";
/*obiekt klasy pomocniczej*/
$BMDSOAP = new BMDSOAP();
$ID=$_POST['ID'];//31;
//$cert=NULL;
//print("przed wywolaniem funkcji\n<br>");
$ret = $BMDSOAP->bmdHistory($cert, $ID);
//print("po wywolaniu funkcji\n<br>");
header("Content-type: text/html; charset=utf-8");
if(is_array($ret)) 
{

draw_htmlhead('Historia pliku w bazie SOE SOAP WWW','');
//print_r($ret);
    //header("Content-type: text/html; charset=utf-8");
    print "Zapytanie zwrocilo " . count($ret) . " wyników: \n<br>";
    foreach ($ret as $obj) 
	 { 
		print("update date: ".$obj->updateDate.'<br> update Owner:<textarea rows="6" cols="40" readonly>'.$obj->updateOwner."</textarea><br>update Reason: ".$obj->updateReason."<br>");
							
		print("<table border='1'><tr><th>OID</th><th>nazwa</th><th>wartość</th></tr>");
		foreach ($obj->mtds->item as $mtd) 
		{
			if (!empty($mtd->mtdValue))
			{
			print("<tr><td>" . $mtd->mtdOid . "</td><td>" . $mtd->mtdDesc . "</td><td>" . $mtd->mtdValue . "</td></tr>");
			}else{
			print("<tr><td>" . $mtd->mtdOid . "</td><td>" . $mtd->mtdDesc . "</td><td> &nbsp</td></tr>");
			}//end if
		}//end foreach
		print("</table>");
		print("================================================\n<br>");	
     }//end foreach

} else {
    //header("Content-type: text/plain; charset=utf-8");
    print("Błąd : ".$ret);
}//end iff
require_once "tail.php";
?>
