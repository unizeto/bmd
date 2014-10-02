<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

require_once('DAO/com_func.php');
require_once "oidsDesc.php";
require_once "certyfikat.php";
require_once "bmdGetUsersList.php";
require_once "GetListTree.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
?>
<div class="glowny w_lewo">
<fieldset>
<legend>Edycja danych użytkownika</legend>
<br />

<?php 

function szukaj_w_tablicy($wartosc,$tablica){
	
	for($m=0;$m<sizeof($tablica);$m++){
		$wynik=strpos($tablica[$m],$wartosc);
		if($wynik !== false){
			return $m;
		}
	}
}
$BMDSOAP = new BMDSOAP();
$ret=$BMDSOAP->bmdGetRolesList($cert);
$lista_rol=(GetListTree($cert,$ret,"<br/>"));

require_once "bmdGetGroupList.php";
$lista_grup=bmdGetGroupsList($cert," ");
$secur=$BMDSOAP->bmdGetSecurityList($cert);
$secLvl=Array();
foreach($secur->secLevelInfo as $elem){
	for($c=0;$c<sizeof($elem);$c++){
		//$secLvl.='<option value="'.$elem[$c]->secLevelName.'">'.$elem[$c]->secLevelName.'</option>';
		$secLvl[]=$elem[$c]->secLevelName;
	}
}
$secCat=Array();
foreach($secur->secCategoryInfo as $elem){
	for($c=0;$c<sizeof($elem);$c++){
		$secCat[]=$elem[$c]->secCategoryName;//'<option value="'.$elem[$c]->secCategoryName.'">'.$elem[$c]->secCategoryName.'</option>';
	}
}

$newArray=bmdGetUsersList($cert," ");

if(is_array($newArray))
{
	for ($i=0;$i<sizeof($newArray);$i++){
		if ($_GET['user']==hash('sha512',(string)$newArray[$i]['userId'].(string)$newArray[$i]['userName'].(string)$newArray[$i]['userCertSerial'])){
			?>

<script language="javascript" type="text/javascript">
var defGroup = '<?=(string)$newArray[$i]['userDefaultGroupName']?>';
var defRole = '<?=(string)$newArray[$i]['userDefaultRoleName']?>';
</script>
			<form action="edituser.php?status=1" method="post" name="editUserForm" >
				<table class="searchtab_results"><tbody>
					<tr class="wysrodkuj"><th>opis</th><th>dane</th></tr>					
					<tr><td class="w_prawo">Serial</td><td><?=(string)$newArray[$i]['userCertSerial']?></td></tr>
					<tr><td class="w_prawo">Issuer</td><td><?=htmlentities((string)$newArray[$i]['userCertIssuer'])?></td></tr>
					<tr><td class="w_prawo">Name</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userName" value="<?=(string)$newArray[$i]['userName']?>" /></td></tr>
					<tr><td class="w_prawo">Identity</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="userIdentity" value="<?=(string)$newArray[$i]['userIdentity']?>" /></td></tr>
					<tr><td class="w_prawo">Groups</td>
					<td><select name="userGroupList[]" id="userGroupList" style="width: 99%;" multiple="multiple"  size="5"
					onchange="javascript:wybrane('userGroupList','userDefaultGroupName',null);">
					<?
					foreach($lista_grup as $nazwy){
					?>
						<option value="<?=isset($nazwy['groupName'])?(string)$nazwy['groupName']:""?>" 
						<?php 
						$temp=explode(" ",$newArray[$i]['userGroupList']);
						foreach($temp as $grupa){
							if ($grupa==$nazwy['groupName']) print(' selected="selected"');
						}						
						?>
						><?=isset($nazwy['groupName'])?(string)$nazwy['groupName']:""?></option>
					<? } ?></select></td></tr>
					<tr><td class="w_prawo">Roles</td>
					<td><select id="userRoleList" name="userRoleList[]" style="width: 99%;" multiple="multiple"  size="5" 
					onchange="javascript:wybrane('userRoleList','userDefaultRoleName',null);">
					<?
					for ($j=0;$j<sizeof($lista_rol);$j++){
						$tmp=explode(" ",(string)$newArray[$i]['userRoleList']);
						$mojarola='';
						for ($k=0;$k<sizeof($tmp);$k++){
							if ($tmp[$k]==(string)$lista_rol[$j]['roleName']){
								$mojarola=' selected="selected"';
							}
						}
					?>						
						<option value="<?=isset($lista_rol[$j]['roleName'])?(string)$lista_rol[$j]['roleName']:""?>" <?=$mojarola?>><?=isset($lista_rol[$j]['roleName'])?(string)$lista_rol[$j]['roleName']:""?></option>
					<?
					$mojarola=''; 
					}	?>
					</select></td></tr>
					
					<tr><td class="w_prawo">Security</td>
						<td><?php
						$secChoice=explode(" ",(string)$newArray[$i]['userSecurityList']);
							 
						for($p=0;$p<sizeof($secur->secCategoryInfo->item);$p++){
							$key=szukaj_w_tablicy($secCat[$p],$secChoice);
							
						?><input name="secCatbox_<?=$p?>" type="checkbox" <?php if (isset($key)) print('checked="checked"'); else print("");?> /><input name="secCat_<?=$p?>" type="text" readonly="readonly" value="<?=$secCat[$p]?>" /><select name="secLevelInfo_<?=$p?>">
							<?
							foreach($secLvl as $elem){
								$wybrane="";
								if (isset($key)){
									$tmp=strpos($secChoice[$key],$elem);
									if(($tmp)!==false){
										$wybrane='selected="selected"';
									}
								}
								
								print('<option value="'.$elem.'" '.$wybrane.'>'.$elem.'</option>');
								$tmp=null;
							}
							?></select><br/><?php	$key=null; } ?></td></tr>
					<tr><td class="w_prawo">Default Group</td>
					<td><select name="userDefaultGroupName" id="userDefaultGroupName" style="width: 99%;"></select></td></tr>
					<tr><td class="w_prawo">Default Role</td>
					<td><select name="userDefaultRoleName" id="userDefaultRoleName" style="width: 99%;"></select></td></tr>
					<tr><td class="w_prawo">Accepted</td><td class="wysrodkuj"><input id="akceptacja" name="akceptacja" type="checkbox" 
					<?php 
					if ((string)$newArray[$i]['userAccepted']){
						print(' checked="checked"');
					}					
					?>
					/></td></tr>		
				</tbody></table>
				<button type="submit" onclick='javascript:formDeleteUser(this.form,"<?=(string)$newArray[$i]['userName']?>","666");'>Usuń</button>
				<input type="submit" value="Akceptuj" id="akceptuj" name="akceptuj" /> 
				<input type="hidden" name="userCertSerial" value="<?=(string)$newArray[$i]['userCertSerial']?>"/>
				<input type="hidden" name="userCertIssuer" value="<?=htmlentities((string)$newArray[$i]['userCertIssuer'])?>"/>
				<input type="hidden" 
					<?php 
					if ((string)$newArray[$i]['userAccepted']){
						print(' checked="checked"');
					}					
					?>
					id="checkbox_disabled" name="checkbox_disabled"/>		
			</form>

<?
		break;
		}
	}
}else{
	print($newArray);
}
?>
<script language="javascript" type="text/javascript">
/*
 * uzupełnienie pól defGroup i defRole przy uruchomieniu
 * Jako że to edycja później te rzeczy są nadpisywane
 */ 	
	wybrane('userGroupList','userDefaultGroupName',defGroup);
	wybrane('userRoleList','userDefaultRoleName',defRole);
</script>

<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="users.php">Lista użytkowników</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>