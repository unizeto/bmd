<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once('DAO/com_func.php');
require_once "oidsDesc.php";
require_once "certyfikat.php";
require_once "GetListTree.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
?>
<div class="centruj w_lewo">
<fieldset>
<legend>Lista uzytkownikow</legend>
<br />
<?php 
$BMDSOAP = new BMDSOAP();
$ret=$BMDSOAP->bmdGetUsersList($cert);

$newArray=GetListTree($cert,$ret,"<br/>");
if(is_array($newArray))
{
	 ?>
	 <div style="margin-left: auto; margin-right: auto; text-align: center;">
	 <button type="button" onclick="window.location.href='form_adduser.php'" value="">Dodaj użytkownika</button>
	 <!-- <button type="button" onclick="" value="">Usuń użytkowników</button> -->
	 </div>
	 <br /><br />
	<table class="searchtab_results"><tbody>
		<tr class="wysrodkuj">
			<th>Name</th>
			<th>Serial</th>
			<th>Issuer</th>
			<th>Identity</th>
			<th>Groups</th>
			<th>Roles</th>
			<th>Security</th>
			<th>Accepted</th>
			<th>Action</th>
			<th>Def Group</th>
			<th>Def Role</th>
		</tr>
				<?
				for ($i=0;$i<sizeof($newArray);$i++){
				?>
					<tr>
					<td><?=isset($newArray[$i]['userName'])?(string)$newArray[$i]['userName']:""?></td>
					<td><?=isset($newArray[$i]['userCertSerial'])?(string)$newArray[$i]['userCertSerial']:" "?></td>
					<td><?=isset($newArray[$i]['userCertIssuer'])?htmlentities((string)$newArray[$i]['userCertIssuer']):" "?></td>
					<td><?=isset($newArray[$i]['userIdentity'])?(string)$newArray[$i]['userIdentity']:" "?></td>					
					<td><?=isset($newArray[$i]['userGroupList'])?(string)$newArray[$i]['userGroupList']:" "?></td>
					<td><?=isset($newArray[$i]['userRoleList'])?(string)$newArray[$i]['userRoleList']:" "?></td>
					<td><?=isset($newArray[$i]['userSecurityList'])?(string)$newArray[$i]['userSecurityList']:" "?></td>			
					<td><?=isset($newArray[$i]['userAccepted'])?(string)$newArray[$i]['userAccepted']:" "?></td>
					<td><a  href="form_edituser.php?user=<?=hash('sha512',(string)$newArray[$i]['userId'].(string)$newArray[$i]['userName'].(string)$newArray[$i]['userCertSerial'])?>">Edytuj</a></td>
					<td><?=isset($newArray[$i]['userDefaultGroupName'])?(string)$newArray[$i]['userDefaultGroupName']:" "?></td>
					<td><?=isset($newArray[$i]['userDefaultRoleName'])?(string)$newArray[$i]['userDefaultRoleName']:" "?></td>
					</tr>
				<? 
				}
				?>
				</tbody></table>
<?php 
}else{
	print($newArray);
}

?>
<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>