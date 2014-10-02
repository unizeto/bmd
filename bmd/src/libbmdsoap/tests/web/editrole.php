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
<legend>Edycja danych roli</legend>
<br />

<?
$BMDSOAP = new BMDSOAP();

/*
 * #define OID_SYS_METADATA_ACTION              "1.2.616.1.113527.4.3.2.10.9"
	#define OID_SYS_METADATA_CERT_ROLE_NAME         "1.2.616.1.113527.4.3.2.13.2"
 */


//090629
	/* 
	 * roleName - wypelnic
	 * roleId
	 * roleActionList - wypelnic 
	 * roleRightsOids - wypelnic
	 * roleRightsCodes - wypelnic wartosci 0000000xx
	 */

//sekcja nazwy roli
//$roleInfo[]=array("mtdOid"=>"1.2.616.1.113527.4.3.2.13.2","mtdValue"=>$_POST['roleName'],"mtdDesc"=>null,"mtdStatement"=>"0");

//sekcja akcji
$actionlist=Array();

for ($j=1;$j<=sizeof($actionsDesc);$j++){
	if (isset($_POST[$actionsDesc[$j]]) && ( $_POST[$actionsDesc[$j]]=='1' || $_POST[$actionsDesc[$j]]=='checked' || $_POST[$actionsDesc[$j]]=='on' ) )
	{
		//$roleInfo[]=array("mtdOid"=>"1.2.616.1.113527.4.3.2.10.9","mtdValue"=>$j,"mtdDesc"=>null,"mtdStatement"=>"0");
		$actionlist[]=$j;
	}else{
		if (isset($_GET['status']) && $_GET['status']=='editrole'){
			//$roleInfo[]=array("mtdOid"=>"1.2.616.1.113527.4.3.2.10.9","mtdValue"=>-$j,"mtdDesc"=>null,"mtdStatement"=>"0");
			$actionlist[]=-$j;
		}
	}	
}

//sekcja praw dla danych oidow
$rightsOids=Array();
$rightsCodes=Array();

$prawa=null;
for ($j=0;$j<sizeof($_SESSION['roleRightsOids']);$j++){
	for ($k=1;$k<=9;$k++){
		if ( isset($_POST[$j.'_'.$k]) && ( $_POST[$j.'_'.$k]=='on' || $_POST[$j.'_'.$k]=='1' || $_POST[$j.'_'.$k]=='checked') ){
			$prawa.="1";
		}else{
			$prawa.="0";
		}
	}
	//$roleInfo[]=array("mtdOid"=>$_SESSION['roleRightsOids'][$j],"mtdValue"=>$prawa,"mtdDesc"=>null,"mtdStatement"=>"0");
	$rightsCodes[]=$prawa;
	$rightsOids[]=$_SESSION['roleRightsOids'][$j];
	
	$prawa=null;
}
$roleInfo=array("roleName"=>$_POST['roleName'],"roleId"=>NULL,"roleActionList"=>$actionlist,
"roleRightsOids"=>$rightsOids,"roleRightsCodes"=>$rightsCodes);

switch($_GET['status']){
	case 'addrole':
		$ret=$BMDSOAP->bmdRegisterNewRole($cert,$roleInfo); 
		$komunikat="add role"; break;
	case 'editrole':
		$ret=$BMDSOAP->bmdUpdateRole($cert,$roleInfo); 
		$komunikat="edit role"; break;
	case 'remrole':
		$ret=$BMDSOAP->bmdDeleteRole($cert,$roleInfo); 
		$komunikat="delete role"; break;
		break;
		
}

print("Komunikat -".$komunikat."- ".$ret."<br/>");
?>

<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="roles.php">Lista ról</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>