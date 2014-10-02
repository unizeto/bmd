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
<legend>Edycja danych użytkownika</legend>
<br />

<?
$BMDSOAP = new BMDSOAP();
switch($_GET['status']){
	case '128'://dodaj 
		/*
		$params = array();
		//pola wymagane
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.3", "mtdValue"=>(string)$_POST['userCertSerial'], "mtdDesc"=>null,"mtdStatement"=>"0");
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.2", "mtdValue"=>(string)$_POST['userCertIssuer'], "mtdDesc"=>null,"mtdStatement"=>"0");
		///////////////////////////
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.4", "mtdValue"=>(string)$_POST['userName'], "mtdDesc"=>null,"mtdStatement"=>"0");
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.5", "mtdValue"=>(string)$_POST['userIdentity'], "mtdDesc"=>null,"mtdStatement"=>"0");
			*/	
		//zrobic explode
		/*
		$temp=explode(" ",(string)$_POST['userGroupList']);		
		$grupy = array();
		for ($i=0;$i<sizeof($temp);$i++ ){			
			//$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.14.2", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$grupy[]=$temp[$i];
		}
		$temp=null;
		*/
		//$temp=explode(" ",(string)$_POST['userRoleList']);
		/*$role=array();
		for ($i=0;$i<sizeof($_POST['userRoleList']);$i++ ){			
			//$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.13.2", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$role[]=(string)$_POST['userRoleList'][$i];
		}
		*/
		//$temp=null;
		/*
		$temp=explode(" ",(string)$_POST['userSecurityList']);
		$security=array();
		for ($i=0;$i<sizeof($temp);$i++ ){
			//$params[] = array("mtdOid"=>"2.5.1.5.55", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$security[]=$temp[$i];
		}	
		*/
		$security=array();
		foreach($_POST as $key => $value){
			$nazwa='secCatbox_';
			if (strstr($key,$nazwa)){
				$nr=substr($key,strlen($nazwa));
				$security[]=$_POST["secCat_".$nr]."|".$_POST["secLevelInfo_".$nr];
			}			
		}
		
		?>
		<table class="searchtab_results"><tbody>
			<tr class="wysrodkuj"><th>opis</th><th>dane</th></tr>
			<tr><td class="w_prawo">Name</td><td><b><?=(string)$_POST['userName']?></b></td></tr>
			<tr><td class="w_prawo">Serial</td><td><?=(string)$_POST['userCertSerial']?></td></tr>
			<tr><td class="w_prawo">Issuer</td><td><?=htmlentities((string)$_POST['userCertIssuer'])?></td></tr>
			<tr><td class="w_prawo">Identity</td><td><b><?=(string)$_POST['userIdentity']?></b></td></tr>
			<tr><td class="w_prawo">Groups</td><td><b><?			
			for($i=0;$i<sizeof($_POST['userGroupList']);$i++){
				print((string)$_POST['userGroupList'][$i]."<br />");
			}
			?></b></td></tr>
			<tr><td class="w_prawo">Roles</td><td><b><?
			for($i=0;$i<sizeof($_POST['userRoleList']);$i++){
				print((string)$_POST['userRoleList'][$i]."<br />");
			}
			?></b></td></tr>
			<tr><td class="w_prawo">Security</td>
			<td><b><?php foreach($security as $item){
				print($item."<br/>");
			}?></b></td></tr>
			<tr><td class="w_prawo">Default Groups</td><td><b><?=(string)$_POST['userDefaultGroupName']?></b></td></tr>
			<tr><td class="w_prawo">Default Roles</td><td><b><?=(string)$_POST['userDefaultRoleName']?></b></td></tr>
			<tr><td class="w_prawo">Accepted</td><td><b><?php 
				if ( isset($_POST['akceptacja']) && ( (string)$_POST['akceptacja'] == 'on' || (string)$_POST['akceptacja'] == 1 || (string)$_POST['akceptacja'] == 'checked') )
						{
							print('Enabled');
							$akceptacja=1;
						}else{
							print('Disabled');
							$akceptacja=0;
						}
								
				?></b></td></tr>		
		</tbody></table>
				
				<?php
				
			$userListInfo=array(
			 "userId"=>null,
		     "userCertIssuer"=>(string)$_POST['userCertIssuer'],
		     "userCertSerial"=>(string)$_POST['userCertSerial'],
		     "userName"=>(string)$_POST['userName'],
		     "userIdentity"=>(string)$_POST['userIdentity'],
		     "userAccepted"=>$akceptacja,
		     "userRoleList"=>$_POST['userRoleList'],
		     "userGroupList"=>$_POST['userGroupList'], //,$grupy,
		     "userSecurityList"=>$security,
			 "userDefaultGroupName"=>(string)$_POST['userDefaultGroupName'],
			 "userDefaultRoleName"=>(string)$_POST['userDefaultRoleName']);
			
		$ret=$BMDSOAP->bmdRegisterNewUser($cert,$userListInfo);//$params);
		print('<br />Komunikat serwera:'.$ret."<br />");
		break;
	case '3'://edytuj+disable
		/*
		 * zawiera wyłączenie użytkownika i update metadanych
		 * 
		 * przez co case przelatuje przez te dwa warunki
		 * w disabled warunowe wylaczenie breaka
		 */
	case '2'://wylacz/disable
		if ( isset($_POST['akceptacja']) && ( (string)$_POST['akceptacja'] == 'on' || (string)$_POST['akceptacja'] == 1 || (string)$_POST['akceptacja'] == 'checked'))
		{
			$enable=1;
		}else{
			$enable=0;
		}
		
		$ret=$BMDSOAP->bmdDisableUser($cert,(string)$_POST['userCertSerial'],((string)$_POST['userCertIssuer']),$enable); 
		print('<br />Komunikat serwera:'.$ret."<br />");
		
		if ($_GET['status']=3){		
			break;
		}
	case '1'://edytuj
		/*
		$params = array();
		//pola wymagane
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.3", "mtdValue"=>(string)$_POST['userCertSerial'], "mtdDesc"=>null,"mtdStatement"=>"0");
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.2", "mtdValue"=>(string)$_POST['userCertIssuer'], "mtdDesc"=>null,"mtdStatement"=>"0");
		///////////////////////////
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.4", "mtdValue"=>(string)$_POST['userName'], "mtdDesc"=>null,"mtdStatement"=>"0");
		$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.12.5", "mtdValue"=>(string)$_POST['userIdentity'], "mtdDesc"=>null,"mtdStatement"=>"0");
		*/
		/*
		//zrobic explode
		$temp=explode(" ",(string)$_POST['userGroupList']);
		$grupy=array();
		for ($i=0;$i<sizeof($temp);$i++ ){
			//$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.14.2", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$grupy[]=$temp[$i];
		}
		$temp=null;
		*/
		/*
		$temp=explode(" ",(string)$_POST['userRoleList']);
		$role=array();
		for ($i=0;$i<sizeof($temp);$i++ ){
			//$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.13.2", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$role[]=$temp[$i];
		}
		*/
		/*
		$temp=null;
		
		$temp=explode(" ",(string)$_POST['userSecurityList']);
		$security=array();
		for ($i=0;$i<sizeof($temp);$i++ ){
			//$params[] = array("mtdOid"=>"2.5.1.5.55", "mtdValue"=>$temp[$i], "mtdDesc"=>null,"mtdStatement"=>"0");
			$security[]=$temp[$i];
		}*/
		$security=array();
		foreach($_POST as $key => $value){
			$nazwa='secCatbox_';
			if (strstr($key,$nazwa)){
				$nr=substr($key,strlen($nazwa));
				$security[]=$_POST["secCat_".$nr]."|".$_POST["secLevelInfo_".$nr];
			}			
		}		
		?>
		<table class="searchtab_results"><tbody>
			<tr class="wysrodkuj"><th>opis</th><th>dane</th></tr>
			<tr><td class="w_prawo">Name</td><td><b><?=(string)$_POST['userName']?></b></td></tr>
			<tr><td class="w_prawo">Serial</td><td><?=(string)$_POST['userCertSerial']?></td></tr>
			<tr><td class="w_prawo">Issuer</td><td><?=htmlentities((string)$_POST['userCertIssuer'])?></td></tr>
			<tr><td class="w_prawo">Identity</td><td><b><?=(string)$_POST['userIdentity']?></b></td></tr>
			<tr><td class="w_prawo">Groups</td><td><b><?
			for($i=0;$i<sizeof($_POST['userGroupList']);$i++){
				print((string)$_POST['userGroupList'][$i]."<br />");
			}
			?></b></td></tr>
			<tr><td class="w_prawo">Roles</td><td><b><?
			for($i=0;$i<sizeof($_POST['userRoleList']);$i++){
				print((string)$_POST['userRoleList'][$i]."<br />");
			}
			?></b></td></tr>
			<tr><td class="w_prawo">Security</td>
			<td><b><?php foreach($security as $item){
				print($item."<br/>");
			}?></b></td></tr>
			<tr><td class="w_prawo">Default Groups</td><td><b><?=(string)$_POST['userDefaultGroupName']?></b></td></tr>
			<tr><td class="w_prawo">Default Roles</td><td><b><?=(string)$_POST['userDefaultRoleName']?></b></td></tr>
			<tr><td class="w_prawo">Accepted</td><td><b><?php 
				if ( isset($_POST['akceptacja']) && ( (string)$_POST['akceptacja'] == 'on' || (string)$_POST['akceptacja'] == 1 || (string)$_POST['akceptacja'] == 'checked' ) )
						{
							print('Enabled');
							$akceptacja=1;
						}else{
							print('Disabled');
							$akceptacja=0;
						}
								
				?></b></td></tr>		
		</tbody></table>
				
				<?php 
		$userListInfo=array(
			 "userId"=>null,
		     "userCertIssuer"=>(string)$_POST['userCertIssuer'],
		     "userCertSerial"=>(string)$_POST['userCertSerial'],
		     "userName"=>(string)$_POST['userName'],
		     "userIdentity"=>(string)$_POST['userIdentity'],
		     "userAccepted"=>$akceptacja,
		     "userRoleList"=>$_POST['userRoleList'],
		     "userGroupList"=>$_POST['userGroupList'],//$grupy,
		     "userSecurityList"=>$security,
			 "userDefaultGroupName"=>(string)$_POST['userDefaultGroupName'],
			 "userDefaultRoleName"=>(string)$_POST['userDefaultRoleName']);
			
		$ret=$BMDSOAP->bmdUpdateUserRights($cert,$userListInfo);//$params);
		print('<br />Komunikat serwera:'.$ret."<br />");
		
		break;
	
	
	
	case '666': //usun
		$ret=$BMDSOAP->bmdDeleteUser($cert,(string)$_POST['userCertSerial'],((string)$_POST['userCertIssuer']));
		print('<br />Komunikat serwera:'.$ret."<br />");
		break;
}


?>

<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="users.php">Lista użytkowników</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>