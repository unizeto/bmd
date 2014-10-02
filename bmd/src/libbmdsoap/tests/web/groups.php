<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once("DAO/com_func.php");
require_once "oidsDesc.php";
require_once "certyfikat.php";
require_once "bmdGetGroupList.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
?>
<div class="glowny w_lewo">
<fieldset>
<legend>Lista grup</legend>
<br />
<?php 

$ret=bmdGetGroupsList($cert," ");
$szerokosc=Array();
?>
<div style="margin-left: auto; margin-right: auto; text-align: center;">
<button type="button" onclick="window.location.href='form_addgroups.php?status=addgroup'" value="">Dodaj Grupę</button></div>
<table ><tbody>
<?

$rodzic=0;

if(is_array($ret)){
	foreach($ret as $drzewo){
		//print_r($drzewo);
		$opis="nadrzędny";
			if (empty($drzewo['groupChildNames'])){
				$opis="liść";
			}
			
			if (!isset($drzewo['groupParentNames'])){
				$opis="root";
			}
			
			
			print("<tr><td>".$opis."</td><td title='Kliknij by edytować grupę'><a href='form_addgroups.php?status=editgroup&gid=".$drzewo['groupId']."'>".$drzewo['groupName']."</a></td></tr>");
		if (isset($drzewo['groupChildNames']) && isset($drzewo['groupChildNames'])!=null){
			print("<tr><td>podrzędne</td><td>".$drzewo['groupChildNames']."</td></tr>");
		}
		print('<tr><td colspan="2" style="text-align: center; width: 150px;">----------------</td></tr>');
		
	}
}
?>
</tbody></table>
<?
/*
$BMDSOAP = new BMDSOAP();
$ret=$BMDSOAP->bmdGetGroupsList($cert);

print("<br/><pre>");
print_r($ret);
print("</pre>");
*/
?>
<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>