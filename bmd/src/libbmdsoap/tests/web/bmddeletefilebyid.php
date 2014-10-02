<?
session_start();
/**
 * Usunięcie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

draw_htmlhead('Wynik usuwania plików w SOE SOAP WWW','');
#$cert = file_get_contents("cert/marua.der");
//$cert = file_get_contents("cert/log_soe_1.der");
/*
wywołanie serwisu - podajemy:
   - zakodowany w base64 certyfikat
   - identyfikator pliku
*/
//header("Content-type: text/html; charset=utf-8");

//for($k=20;$k<=23;$k+=1){
//chcemy kasowac pliki z serii "nowyplik.testowo" które sa ponumerowane od 1 do 10000 co dodatkowo okrasla również ich wielkość
#$filename="Kopia (10) f06.xml";
#print($filename);
#$params[] = array("mtdOid" => "1.2.616.113527.4.3.2.5.2","mtdValue" =>$filename);
//zapytanie do bazy 
#$return = $BMDSOAP->bmdSearchFiles($cert, $params, 1, 0);
#	if(is_array($return)) {
#		foreach ($return as $obj) {
#		break;//tylko pierwszy wynik
#		}
#	}
//for ($id=68;$id<=68;$id++) //;
//{
$id=$_POST["ID"];
#if (!empty($obj->id)){

$tablica=explode(",",$id);
for ($i=0;$i<sizeof($tablica);$i++)
{		
		$temp=explode("-",$tablica[$i]);
		$zakres=false;
		if (!isset($temp[1]))
		{
			$temp[1]=$temp[0];
			$zakres=true; 
		}
		
		for($j=$temp[0];$j<($temp[1]+1);$j++)
		{
			$ret = $BMDSOAP->bmdDeleteFileByID($cert,$j);//$id);
			if (empty($ret)){
				print("skasowano plik ".$j."<br>\n");#.$filename." z id ".$obj->id." z bazy "); 
			}else
			{
				print("UWAGA : ".$ret."<br>\n");
			}
			if ($zakres) break;
		}		
}
		
//}//end for
#	}else
#	{
#		print("brak pliku lub błąd");
#	}
		//cleaning
#		$return=NULL;
#		$obj=NULL;
#		$params=array();
//} //end for
require_once "tail.php";
?>
