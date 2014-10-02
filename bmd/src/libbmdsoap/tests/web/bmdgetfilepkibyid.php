<?
session_start();
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
$ID=$_POST['ID'];

$ret = $BMDSOAP->bmdGetFilePKIById($ID, $cert);
/*pobranie odpowiedzi z serwera - obiekt z plikiem
    $ret->filename - nazwa pliku
    $ret->file - zawartość pliku
*/
//print($ret);
header("Content-type: text/html; charset=utf-8");
if(is_object($ret)) {
// header("Content-type: text/plain; charset=utf-8");
/*
	header("Content-type: application/octet-stream");
	header("Contentposition: attachment; filename=" . $ret->filename);
	header("Content-Length: " . strlen($ret->file));
	$file_handle = fopen($ret->filename, "w+");
	fwrite($file_handle, $ret->file);
	fclose($file_handle);
    print($ret->file);
	 */
	// require_once "head.php";
print($ret->filename."\n");
//
//TIMESTAMP
//
if (isset($ret->timestamp->item)){
$tsr_count=count($ret->timestamp->item);
print("<dl><dt>");
print("Timestamps :".$tsr_count."\n");
print("</dt>");
	if ( $tsr_count != 0)
	{
		for ($i=0;$i<$tsr_count;$i++)
		{
		   print("<dd>");	
			print("\tTimestamp_".$i."\n");// domyslnie mozna pod to wrzucic $ret->timestamp->item[$i]->value (html link lub cos podobnego), ktory zawiera plik tsr , trzeba go tylko zapisac na dysk , lub dac do podgladu w windzie
			      print("</dd>");
		}	
		}
print("</dl>");
} else
{
print("Plik nie ma timestampa\n");
}
//
//SIGNATURE
//
if (isset($ret->signature->item))
{
$sig_count=count($ret->signature->item);
print("<dl><dt>");
print("Signatures :".$sig_count."\n");
print("</dt>");
if ($sig_count != 0)
	{
		for ($i=0;$i<$sig_count;$i++)
		{
//
//SIGNATURE TIMESTAMP
//

print("<dd>");
print("\tSignature_".$i."\n");
print("</dd>");
if (isset($ret->signature->item[$i]->timestamp->item))
			{
				$sig_tsr_count=count($ret->signature->item[$i]->timestamp->item);
				print("<dl><dt>");
				print("\tSignature timestamps :".$sig_tsr_count."\n");
				print("</dt>");
				for($k=0;$k<$sig_tsr_count;$k++)
				{
				print("<dd>");	
				print("\t\tSig_tsr_".$k."\n");
				print("</dd>");	
				
					//wyciagniecie wartosci $ret->signature->item[$i]->timestamp->item[$k]->value
					}
				print("</dl>");
			}else
			{
       	                        print("\tSignature nie ma timestampa\n");
                        }


			if (isset($ret->signature->item[$i]->dvcs->item))
                        {

//
//DVCS
//
print("<dl><dt>");

				print("\t\tDVCS\n"); 
				print("</dt>");
				if(isset($ret->signature->item[$i]->dvcs->item[$i]->timestamp->item))
				{
//
//DVCS TIMESTAMP
//
                                       	$sig_dvcs_tsr_count=count($ret->signature->item[$i]->dvcs->item[$i]->timestamp->item);
                                       	for($k=0;$k<$sig_dvcs_tsr_count;$k++)
														{
														print("<dd>");	
														
                                               	print("\t\t\tSig_dvcs_tsr_".$k."\n");
																print("</dd>");	
																//wyciagniecie wartosci $ret->signature->item[$i]->dvcs->item[$k]->value
                                       	}
				}else
                                {
               	                        print("\t\tSig dvcs nie ma timestampa\n");
       	                        }

                       }else
                       {
                               print("\tSignature nie ma dvcs\n");
                       }
		}
	}
	//print("Singature timestamps :".count($ret->signature->item->timestamp)."\n");
	print("</dl>");
	print("</dl>");
	
}else
{
print("Plik nie ma signature\n");
}

} else {
//	header("Content-type: text/plain; charset=utf-8");
	print("Błąd zwrotu: ".$ret);
	print("\n");
	}
//	require_once "tail.php";
?>
