<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once('DAO/com_func.php');
require_once "oidsDesc.php";
require_once "certyfikat.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
?>
<div class="glowny w_lewo">
<fieldset>
<legend>Edycja danych grupy</legend>
<br />

<?
$BMDSOAP = new BMDSOAP();

$parent=null;
$child=null;

if (isset($_POST['groupParentNames']) && $_POST['groupParentNames']!=null){
	$parent=$_POST['groupParentNames'];
}
if (isset($_POST['groupChildNames']) && $_POST['groupChildNames']!=null){
	$child=$_POST['groupChildNames'];
}


$groupInfo=array("groupName"=>(string)$_POST['groupName'],"groupId"=>null,"groupParentNames"=>$parent,"groupChildNames"=>$child);

switch($_GET['status']){
	case 'addgroup':
		$ret=$BMDSOAP->bmdRegisterNewGroup($cert,$groupInfo); 
		$komunikat="add group"; break;
	case 'editgroup':
		$ret=$BMDSOAP->bmdUpdateGroup($cert,$groupInfo); 
		$komunikat="edit group"; break;
	case 'remgroup':
		$ret=$BMDSOAP->bmdDeleteGroup($cert,$groupInfo); 
		$komunikat="delete group"; break;
		break;
		
}

print("Komunikat -".$komunikat."- ".$ret."<br/>");
?>

<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="groups.php">Lista grup</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>