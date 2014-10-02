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
<div class="glowny w_lewo">
<fieldset>
<legend>Lista ról</legend>
<br />
<?php 
$BMDSOAP = new BMDSOAP();
$ret=$BMDSOAP->bmdGetRolesList($cert);
$newArray=(GetListTree($cert,$ret,"<br/>"));
/*
print("<pre>");
print_r($ret);
print("</pre>");
*/
if(is_array($newArray))
{
	?>
	<div style="margin-left: auto; margin-right: auto; text-align: center;">
	 <button type="button" onclick="window.location.href='form_addroles.php?status=addrole'" value="">Dodaj role</button>
	 <!-- <button type="button" onclick="" value="">Usuń role</button> -->
	 </div>
	 <br /><br />
	<table class="searchtab_list"><tbody>
		<tr class="wysrodkuj">
			<th>roleName</th>
			<th>!roleActionList<br/>tych akcji rola nie ma</th>
			<th>roleRightsOids</th>
			<th>roleRightsCodes<br/>0000000 Upd Hist</th>
			<th>Action</th>			
		</tr>
				<?
				for ($i=0;$i<sizeof($newArray);$i++){
				?>
					<tr>
					
					
					<td><?=isset($newArray[$i]['roleName'])?(string)$newArray[$i]['roleName']:""?></td>
					<td class="w_lewo"><?php
					if ( isset($newArray[$i]['roleActionList']) ){ 
						$temp=explode("<br/>",(string)$newArray[$i]['roleActionList']);
						$akcje=array();
						if (sizeof($temp)!=sizeof($actionsDesc)){
							for($j=0;$j<sizeof($temp);$j++)
								{
									$akcje[$temp[$j]]=$temp[$j];
									
								}
							for ($j=1;$j<=sizeof($actionsDesc);$j++)
							{
								if (!isset($akcje[$j])){
									print($actionsDesc[$j]."<br/>");
								}
							}
						}else{ echo "Brak restrykcji"; }
					}else{ echo " "; }
					$temp=null;
					?></td>					
					<td class="w_lewo"><?
					if (isset($newArray[$i]['roleRightsOids'])) {
						$_SESSION['roleRightsOids']=explode("<br/>",(string)$newArray[$i]['roleRightsOids']);
							for ($j=0;$j<sizeof($_SESSION['roleRightsOids']);$j++){
								
									print($oidsDesc[str_replace("_",".",substr($_SESSION['roleRightsOids'][$j],3))]['opis'].'<br/>');
							}					
					
					}else{ echo " "; }					
					
					?></td>
					<td class="wysrodkuj"><?=isset($newArray[$i]['roleRightsCodes'])?(string)$newArray[$i]['roleRightsCodes']:" "?></td>
					<td><a  href="form_addroles.php?status=editrole&rid=<?=$i?>">Edytuj</a></td>
					<!-- <td><input type="checkbox" /></td>   -->
					
					</tr>
				<? 
				}
				print("</tbody></table>");
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