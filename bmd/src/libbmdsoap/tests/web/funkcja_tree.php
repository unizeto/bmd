<?
session_start();
function runSearch($corespid){
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

if (count($_SESSION['params'])==1 && ($corespid!=null || $corespid!=0 )) {
	$OID="1.2.616.1.113527.4.3.3.26";
		if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
			$OID="1.2.616.1.113527.4.3.2.5.7";  		
	  	}
	$_SESSION['params'][1]=array('mtdOid'=>$OID,
	'mtdValue'=>$corespid, 'mtdDesc'=>null,
	'mtdStatement'=>0);
}else{
for($lp=0;$lp<count($_SESSION['params']);$lp++)
{
	$OID="1.2.616.1.113527.4.3.3.26";
		if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
			$OID="1.2.616.1.113527.4.3.2.5.7";  		
	  	}
	if($_SESSION['params'][$lp]['mtdOid']==$OID){
		$_SESSION['params'][$lp]['mtdValue']=$corespid; //nadpisywanie correspondanceID w zależności na klikniety katalog
	}//end if
}//end for
}//end else
/*
na potrzeby testow, sortowanie wg konwencji:
katalogi na gorze + sortowanie po nazwach alfabetycznie

pomijane sa ustawienia z formsearcha
*/
$sortInfo=array();
		/*$OID="1.2.616.1.113527.4.3.3.25";
		if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {*/
			$OID="1.2.616.1.113527.4.3.2.5.6";  		
	  	//}
$sortInfo[] = array("sortOid"=>$OID, "sortOrder"=>"ASC");
$sortInfo[] = array("sortOid"=>"1.2.616.1.113527.4.3.2.5.2", "sortOrder"=>"ASC");
/////////////////////////////////////////////////////////////////////////////////
$ret = $BMDSOAP->bmdSearchFiles(
$cert,
$_SESSION['params'],
null,
$_SESSION['offset'],
$_SESSION['limit'],
$_SESSION['logical'],
$_SESSION['goper'],
$_SESSION['range'],
$sortInfo//$_SESSION['sortInfo']
);

if(is_array($ret)){

	$idx=0;
	
	foreach ($ret as $obj){
		$rodzaj=0; // 0 = plik , 1 = katalog
		if (isset($obj->mtds->item)){
			foreach ($obj->mtds->item as $mtd) {
			$OID="1.2.616.1.113527.4.3.3.25";
				if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
					$OID="1.2.616.1.113527.4.3.2.5.6";  		
			  	}
				if ( $mtd->mtdOid==$OID && $mtd->mtdValue=="dir" ){
					print('<dd title="katalog" onclick="
					parametry=\'corespID='.$obj->id.'\'; ajaxMakeRequest(\'funkcja_tree.php\', \'res\', \'post\', parametry, false,'.$obj->id.');" id="nad'.$obj->id.'">
					<img src="gfx/folder.png" alt="katalog" />');
					$rodzaj=1;
				}//end if
			}//end foreach
		}//end if
		
		if ($rodzaj!=1) print('<dd title="plik" id="plik'.$obj->id.'" name="plik'.$obj->id.'" onclick="
					parametry=\'index='.$idx.'\'; ajaxMakeRequest(\'searchresultstree_metadata.php\', \'res_tree\', \'post\', parametry, false,\'searchtree_metadane_pliku\');"><img src="gfx/document.png" alt="katalog" />');
		//print($obj->id.'-');
		if ($rodzaj==1) print('<a><b>');
		print($obj->filename);
		if ($rodzaj==1) print('</b></a>');
		print('</dd>');
		if ($rodzaj==1) print('<dl><dd id="'.$obj->id.'" ></dd></dl>');
		
		$idx++;
	}//end foreach
	$_SESSION['ret']=$ret;//nie wiem jak to inaczej przekezac ;[
	}else{
		print('<script language="JavaScript"><!-- //'.$ret.' --></script>');//TODO zmienić sposób dodawania "ukrytych" komentarzy błędów do kodu html
	}//end if
}//end function runSearch

$podano=(isset($_POST['corespID']))?$_POST['corespID']:(isset($_SESSION['corespid'])?$_SESSION['corespid']:0);
runSearch($podano);
print('</dl></div><div id="searchtree_metadane_pliku" class="searchtree_column plywajacy w_lewo pozycja_prawo">');
print('</div>');
?>