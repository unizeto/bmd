<?
session_start();
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');
require_once "oidsDesc.php";
require_once "certyfikat.php";

$BMDSOAP = new BMDSOAP();
draw_htmlhead('Przegląd zawartości bazy - SOE SOAP WWW','searchtree');

$server_name = strtolower($_SERVER["SERVER_NAME"]);
$server_port = ":" . $_SERVER["SERVER_PORT"];
$server_dir  = dirname($_SERVER["SCRIPT_NAME"]);

if (! empty($_SERVER["HTTPS"])) {
			$url = "https://";
			if ($server_port == ":443") {
				$server_port = "";
			}
		} else {
			$url = "http://";
			if ($server_port == ":80") {
				$server_port = "";
			}
		}

$url .= "$server_name$server_port$server_dir/";

$ret = $BMDSOAP->bmdSearchFiles($cert, $_SESSION['params'], $_SESSION['mtdsparams'], $_SESSION['offset'], $_SESSION['limit'], 
$_SESSION['logical'], $_SESSION['goper'], $_SESSION['range'], $_SESSION['sortInfo']);

/*pobranie odpowiedzi z serwera - tablica obiektów zawierających dane
$obj->mtds - tablica obiektów(rodzajów metadanych) - struktura: patrz metoda bmdLogin
$obj->filename - nazwa pliku
$obj->getFileURL - skrót SHA1
*/
$pl=0;
if(is_array($ret)) {
//header("Content-type: text/html; charset=utf-8");
print "Zapytanie zwróciło " . count($ret) . " wyników: \n";
print('<div><table class="searchtab_results"><tbody>');
//print("<th><th>OID</th><th>nazwa</th><th>wartość</th></th>");
$lp=$_SESSION['offset']+1;

print('<tr class="wysrodkuj"><th class="fontnormal">Lp</th>');
////opcjonalne na potrzeby testow////////////////////
if ($_SESSION['kol_filename'] || !isset($mtdsparams)) {
	print('<th class="fontnormal">ID</th><th class="fontnormal">filename</th>');
}//end if
////////////////////////////////////////////////////////////
	foreach ($ret[0] as $obj){
		if (isset($obj->item)){
			foreach ($obj->item as $mtd) {
				if (isset($oidsDesc[$mtd->mtdOid])){
					if ($oidsDesc[$mtd->mtdOid]['pokaz']) { print('<th class="fontnormal">'.$oidsDesc[$mtd->mtdOid]['opis'].'</th>'); }
				}
			}
		}
	}		
//} //end if
print('</tr>');
$skryptjs="";
$objid=0;
foreach ($ret as $obj) {
$katalog=false;
	print('<tr name="'.$obj->id.'"><td style="text-align: right;">&#160;'.$lp.'</td>');
	$objid=$obj->id;
	////opcjonalne na potrzeby testow////////////////////
	if ($_SESSION['kol_filename'] || !isset($mtdsparams)) {
		print('<td class="w_prawo fontbold">'.$obj->id.'</td><td><a id="'.$obj->id.'" >'.$obj->filename.'</a></td>');
	}//end if
	////////////////////////////////////////////////////////////
	if (isset($obj->mtds->item)){
		foreach ($obj->mtds->item as $mtd) {
	//print("<td>&#160;" . $mtd->mtdOid . "</td><td>&#160;" . $mtd->mtdDesc . "</td><td>&#160;" . $mtd->mtdValue . "</td></tr>");
		
			if (isset($oidsDesc[$mtd->mtdOid]))
			{ 
				if ($oidsDesc[$mtd->mtdOid]['pokaz']=='1') {
					$wartosc="&#160;";
					if (isset($mtd->mtdValue) && !empty($mtd->mtdValue) ) 
					{
						
						
						if($mtd->mtdValue == 'dir'){ 
							$katalog=true;														
						}
						
						if ($katalog){	
														
								$skryptjs.="
									link=document.getElementById('".$objid."');
									link.setAttribute('href','".$url."searchresults_dirredir.php?cid=".urlencode(base64_encode($objid))."'); 
								";							
							$katalog=false;
														
						}
						
						$wartosc=$mtd->mtdValue;
					}
					print('<td class="fontbold">'.$wartosc . '</td>');	//do poprawienia, dostaje mniej oidow niz normalnie, brak tych pustych 			
				}
			}//end if		
		}//end  foreach
	}//end if
	print("</tr>");	
	$lp+=1;
}//end foreach
$lp-=1;
print("</tbody></table></div>");
// print('<div class="searchtab_navi">
print('<div class="">
<form action="posredniksearch.php" method="post" name="searchform">
<table><tbody>
<tr><td>');
print('<input type="hidden" value="'.$lp.'" name="iloscwynikow" />');
if ($_SESSION['offset']!=0){
	print('<input type="submit" value="prev" name="prev" />'); 
}
if ( $lp>=$_SESSION['offset']){//ze wzgledu na bląd w bazie uwzgledniono testowo wartosci wieksze, domyslnie powinny sie sobie rownac
	print('<input type="submit" value="next" name="next" />');
}
 
print('
</td></tr>
</tbody></table>
</form></div>
<script type="text/javascript">
var link="";
'.
$skryptjs
.'
</script>
');

}else{
print($ret);
}//end if
print('<div class="wysrodkuj">
<a href="searchForm.php">Ponownie wyszukaj</a></div>');
require_once "tail.php";
?>