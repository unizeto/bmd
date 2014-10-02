<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once('DAO/com_func.php');

require_once "certyfikat.php";
require_once "GetListTree.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
$BMDSOAP = new BMDSOAP();
$ret=$BMDSOAP->bmdGetRolesList($cert);
$secur=$BMDSOAP->bmdGetSecurityList($cert);
$secLvl=null;
foreach($secur->secLevelInfo as $elem){
	for($c=0;$c<sizeof($elem);$c++){
		$secLvl.='<option value="'.$elem[$c]->secLevelName.'">'.$elem[$c]->secLevelName.'</option>';
	}
}
$secCat=Array();
foreach($secur->secCategoryInfo as $elem){
	for($c=0;$c<sizeof($elem);$c++){
		$secCat[]=$elem[$c]->secCategoryName;//'<option value="'.$elem[$c]->secCategoryName.'">'.$elem[$c]->secCategoryName.'</option>';
	}
}

$newArray=(GetListTree($cert,$ret,"<br/>"));
require_once "bmdGetGroupList.php";
$ret=bmdGetGroupsList($cert," ");
?>
<div class="glowny w_lewo">
<fieldset>
<legend>Dodawanie użytkownika</legend>
<br />
<?php

if (isset($_FILES['plik']['tmp_name'])){
$info=shell_exec('openssl x509 -inform DER -in '.$_FILES['plik']['tmp_name'].' -serial -issuer -noout');

$tmp=explode("\n",$info);

$tmp[1]=str_replace("/","> ",$tmp[1]);
$tmp[1]=str_replace("=","=<",$tmp[1]);
$tmp[0]=mb_substr($tmp[0],7);
$temp=str_split($tmp[0],2);
$tmp[0]=null;
if (sizeof($temp)>1){
for($i=0;$i<sizeof($temp)-1;$i++) 
	$tmp[0].=$temp[$i]." ";
$tmp[0].=$temp[$i];
}else{
	$tmp[0]=$temp[0];
}

$tmp[1]=mb_substr($tmp[1],11).">";
}
?>
			<form action="" method="post" name="addUserForm" id="addUserForm" enctype="multipart/form-data">
				<table class="searchtab_results"><tbody>
					<tr class="wysrodkuj"><th>opis</th><th>dane</th></tr>					
					<tr><td colspan="2" style="text-align: center;">Podaj plik certyfikatu publicznego dodawanego użytkownika.<br/>Pozwoli to automatycznie odczytać i uzupełnić pola "serial" i "issuer".<br/>
					<input type="file" name="plik" onchange="javascript:changeAction(this.form,'form_adduser.php');addUserForm.submit();"/></td></tr>
					<tr><td class="w_prawo">Serial</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userCertSerial"  
						value="<?=(isset($tmp) && !empty($tmp[0]))?$tmp[0]:""?>"/></td></tr>
					<tr><td class="w_prawo">Issuer</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userCertIssuer"  
						value="<?=(isset($tmp) && !empty($tmp[1]))?$tmp[1]:""?>"/></td></tr>
					<tr><td class="w_prawo">Name</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userName" /></td></tr>
					<tr><td class="w_prawo">Identity</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userIdentity"  /></td></tr>
					<tr><td class="w_prawo">Groups</td>
					<td><select name="userGroupList[]" id="userGroupList" style="width: 99%;" multiple="multiple" size="5"
					onchange="javascript:wybrane('userGroupList','userDefaultGroupName',null);">
					<?
					foreach($ret as $nazwy){
					?>
						<option value="<?=isset($nazwy['groupName'])?(string)$nazwy['groupName']:""?>"><?=isset($nazwy['groupName'])?(string)$nazwy['groupName']:""?></option>
					<? }	?>
					</select></td></tr>
					<tr><td class="w_prawo">Roles</td>
					<td><select name="userRoleList[]" id="userRoleList" style="width: 99%;" multiple="multiple"  size="5"
					onchange="javascript:wybrane('userRoleList','userDefaultRoleName',null);">
					<?
					for ($i=0;$i<sizeof($newArray);$i++){
					?>
						<option value="<?=isset($newArray[$i]['roleName'])?(string)$newArray[$i]['roleName']:""?>"><?=isset($newArray[$i]['roleName'])?(string)$newArray[$i]['roleName']:""?></option>
					<? }	?>
					</select></td></tr>
					<tr><td class="w_prawo">Security</td>
						<td><?php 
						for($p=0;$p<sizeof($secur->secCategoryInfo->item);$p++){
						
						?><input name="secCatbox_<?=$p?>" type="checkbox" /><input name="secCat_<?=$p?>" type="text" readonly="readonly" value="<?=$secCat[$p]?>" /><select name="secLevelInfo_<?=$p?>">
							<?=$secLvl?>
						</select><br/><?php	} ?></td></tr>
					<tr><td class="w_prawo">Default Group</td>
						<td><select name="userDefaultGroupName" id="userDefaultGroupName" style="width: 99%;"></select></td>
					</tr>
					<tr><td class="w_prawo">Default Role</td>
					<td><select name="userDefaultRoleName" id="userDefaultRoleName" style="width: 99%;"></select></td></tr>
					<tr><td class="w_prawo">Accepted</td><td class="wysrodkuj"><input id="akceptacja" name="akceptacja" type="checkbox"/></td></tr>		
				</tbody></table>
				<input type="submit" value="Akceptuj" id="akceptuj" name="akceptuj" onclick="javascript:changeAction(this.form,'edituser.php?status=128');"/>
					
			</form>

<br />
</fieldset>



</div>
<div class="wysrodkuj">
<a href="users.php">Lista użytkowników</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
$tmp=null;
?>