<?php
session_start();

function runSearch_fileinfo($idx){
	require_once "oidsDesc.php";
	require_once "certyfikat.php";
	$BMDSOAP = new BMDSOAP();
	
	$sortInfo=array();
	/*
		$OID="1.2.616.1.113527.4.3.3.25";
		if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
			$OID="1.2.616.1.113527.4.3.2.5.6";  		
	  	}
	  	*/
	$sortInfo[] = array("sortOid"=>'1.2.616.1.113527.4.3.2.5.6', "sortOrder"=>"ASC");
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
		print('<table class="searchtab_results_tree"><tbody>');
		foreach($ret[$idx] as $obj){
			foreach($obj->item as $info){
				if ($oidsDesc[$info->mtdOid]['pokaz']){
					print('<tr><td>'.$oidsDesc[$info->mtdOid]['opis'].'</td><td class="fontbold">'.$info->mtdValue.'</td></tr>');
				}	
			}				
		}
		print('</tbody></table>');
	}else
	{
		print('<script language="JavaScript"><!-- //'.$ret.' --></script>');//TODO zmienić sposób dodawania "ukrytych" komentarzy błędów do kodu html
	}
}

runSearch_fileinfo($_POST['index']);
print('<p class="wysrodkuj"><a href="main.php">main</a></p>');
?>