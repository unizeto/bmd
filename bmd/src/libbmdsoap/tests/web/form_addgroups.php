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
require_once "bmdGetGroupList.php";
$ret=bmdGetGroupsList($cert," ");

$legenda="Dodawanie grupy";
$wartosc="";
if ($_GET['status']=='editgroup'){
	$legenda="Edycja grupy";
	foreach($ret as $nazwy){
		if ($nazwy['groupId']==$_GET['gid']){
			$wartosc=$nazwy['groupName'];
			if (isset($nazwy['groupParentNames'])) $rodzice=$nazwy['groupParentNames'];
			if (isset($nazwy['groupChildNames'])) $dzieci=$nazwy['groupChildNames'];			
		}
	}
	
}
//print_r($dzieci);
//selected="selected"
?>
<div class="glowny w_lewo"  style="width: 1024px;">
<fieldset style="width: 100%;">
<legend><?=$legenda?></legend>

<form action="" method="post" name="addGroupForm" >
	<table><tbody>
		<tr><td></td><td></td></tr>
		<tr><td>Group Name</td><td><input type="text" name="groupName" value="<?=$wartosc?>"/></td></tr>
		<tr><td>Group Parent Names</td><td><select name="groupParentNames[]" multiple="multiple">
		<?php foreach($ret as $nazwy){?>
		<option value="<?=$nazwy['groupName']?>"
		<?php
			if (isset($rodzice)){
				$tmp=explode(" ",$rodzice);
				foreach($tmp as $r){
					if ($nazwy['groupName']==$r) print(' selected="selected"');
				}
			} 
		?>
		><?=$nazwy['groupName']?></option>
		<?php }?>
		</select></td></tr>
		<tr><td>Group Child Names</td><td><select name="groupChildNames[]" multiple="multiple" >
		<?php foreach($ret as $nazwy){?>
		<option value="<?=$nazwy['groupName']?>" 
		<?php
			if (isset($dzieci)){
				$tmp=explode(" ",$dzieci);
				foreach($tmp as $d){
					if ($nazwy['groupName']==$d) print(' selected="selected"');
				}
			} 
		?>		
		><?=$nazwy['groupName']?></option>
		<?php }?>
		</select></td></tr>
		<tr><td colspan="2" style="text-align: center;"><input type="submit" value="Akceptuj" id="akceptuj" name="akceptuj" onclick="javascript:changeAction(this.form,'editgroup.php?status=<?=$_GET['status']?>');"/></td></tr>
		<?php if ($_GET['status']=='editgroup'){?>
		<tr><td colspan="2" style="text-align: center;"><input type="submit" value="Usuń" id="usun" name="usun" onclick="javascript:changeAction(this.form,'editgroup.php?status=remgroup');"/></td></tr>
		<?php }?>
	</tbody></table>
</form>

</fieldset>
</div>
<div class="wysrodkuj">
<a href="groups.php">Lista grup</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>