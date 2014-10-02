<?
/**
 * Pobranie z archiwum pliku o określonej wartości ID
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();
/*wywołanie serwisu - podajemy:
   - zakodowany w base64 certyfikat
   - identyfikator pliku
*/

//$ID=304;

$tmp=$_SERVER['argv'];
$ID=$tmp[1];

$ret = $BMDSOAP->bmdGetFileChunkStart($cert, $ID);
$filename="temp".rand(100,999);
if (isset($ret))
{
	$tmpdir=$ret;
	print("Zwrócono :".$tmpdir."\n");
	$x=0;
	do
	{
	$x++;
	$ret= $BMDSOAP->bmdGetFileChunkIter($cert,$tmpdir,$x);
//	print("ret :".$ret."\n");
	if(is_object($ret)) {
//print("otrzymałem cosik ".$ret->file."\n");
	//usunac headery
	//wykorzystac funkcje fwrite do automatycznego zapisu do pliku, bez koniecznosci akceptacji ze strony usera -> tylko w ramach testów
	/*
	    header("Content-type: application/octet-stream");
	    header("Content-Disposition: attachment; filename=" . $ret->filename);
	    header("Content-Length: " . strlen($ret->file));
	    print($ret->file);
	*/
	//================================================
	
	    //header("Content-type: text/plain; charset=utf-8");
		if (!file_exists("odebrano/".$tmpdir))
		{
			if (!mkdir("odebrano/".$tmpdir,0700)) {print("Nie można było utworzyć katalogu \n"); }
		}
	        if (!$uchwyt = fopen("odebrano/".$tmpdir."/".$filename, 'a')) {
	                echo "Nie mogę otworzyć pliku ($ret->filename)";
	                exit;
	        }//enf if uchwyt
	
	// Zapis $trochetresci do naszego otwartego pliku.
	        if (fwrite($uchwyt, $ret->file) === FALSE) {
	                echo "Nie mogę zapisać do pliku ($ret->filename)";
	                exit;
	        }//end if fwrite
	
	fclose($uchwyt);
	
	} //end if else is_array
	else {
	//    header("Content-type: text/plain; charset=utf-8");
		if (strcmp($ret,"Requested chunk not found")) 
			{
			print("Plik o ID ".$ID."Nie udało się pobrać paczki nr".$x.". Ponowna próba.\n");
			$x--; 
			}
		//if (strcmp($ret,"Could not connect to host")) 
			//{
			//exit("koniec programu\n"); 
			//}
		
	    //print("Error is_object: ".$ret."\n");

	}//end if else is object
	
	
		print("Plik ID: ".$ID." x: ".$x." paczka size: ".strlen($ret->file)."\n");	
	}while(empty($ret->filename));
	if (!rename("odebrano/".$tmpdir."/".$filename,"odebrano/".rand(1,100000).$ret->filename)) {print("\n");}
	if (!rmdir("odebrano/".$tmpdir)) {print("Nie można było skasować katalogu \n");}
}
else
{
	print("Error isset: ".$ret);
}
//}//end  for
?>

