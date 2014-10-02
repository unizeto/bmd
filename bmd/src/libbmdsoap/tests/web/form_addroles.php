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
if (isset($_GET['status']) && $_GET['status']=='editrole'){
	$BMDSOAP = new BMDSOAP();
	$ret=$BMDSOAP->bmdGetRolesList($cert);
	$newArray=(GetListTree($cert,$ret,"<br/>"));
}
?>
<div class="glowny w_lewo"  style="width: 1024px;">
<fieldset style="width: 100%;">
<legend>Dodawanie roli</legend>
<br />
			<form action="" method="post" name="addRoleForm" >			
			
				<table class="searchtab_results"><tbody>					
					<tr><td class="w_prawo">Role Name</td><td><input class="searchtab_form" style="width: 99%;" type="text" name="roleName" value="<?
					if (isset($_GET['rid'])){ 
						if (isset($newArray[$_GET['rid']]['roleName'])){
								print((string)$newArray[$_GET['rid']]['roleName']);
							}else echo "";
					}
					?>"/></td>
					<td><input type="submit" value="Akceptuj" id="akceptuj" name="akceptuj" onclick="javascript:changeAction(this.form,'editrole.php?status=<?=$_GET['status']?>');"/></td>
					<td style="width: 25px;"></td>
					<td><input type="submit" value="Usuń" id="usun" name="usun" onclick="javascript:changeAction(this.form,'editrole.php?status=remrole');"/></td>
					</tr>
				</tbody></table>
				<br/>
				<div style="float: left; margin-top: 50px; margin-left: 10px;">
				<table class="searchtab_results"><tbody>
					<tr><th>Nazwa akcji</th><th>Opcja</th></tr>
					<?php 
					for($i=0;$i<ceil(sizeof($actionsDesc)/2);$i++){
										
					?>
					<tr><td class="w_lewo"><?=$actionsDesc[$i+1]?></td><td><input class="searchtab_form" type="checkbox" name="<?=$actionsDesc[$i+1]?>"  <?
					if (isset($_GET['rid']) ){
						if ( isset($newArray[$_GET['rid']]['roleActionList']) ){ 
							$temp=explode("<br/>",(string)$newArray[$_GET['rid']]['roleActionList']);
							$akcje=array();
							for($j=0;$j<sizeof($temp);$j++)
									{
										$akcje[$temp[$j]]=$temp[$j];									
									}
							if (isset($akcje[$i+1]) && $i+1==$akcje[$i+1]){
								print('checked="checked');
							}
						}
					}
					?>"/></td></tr>
					
					<?php
					}
					print('							
							</tbody></table><br/>
							</div><div style="float: left; margin-top: 50px; margin-left: 10px;">
							<table class="searchtab_results"><tbody>
							<tr><th>Nazwa akcji</th><th>Opcja</th></tr>
							');
					for ( $i=ceil(sizeof($actionsDesc)/2);$i<sizeof($actionsDesc);$i++ ){
					?>	
					
							<tr><td class="w_lewo"><?=$actionsDesc[$i+1]?></td><td><input class="searchtab_form" type="checkbox" name="<?=$actionsDesc[$i+1]?>"  <?
							
							if (isset($_GET['rid']) ){
								if ( isset($newArray[$_GET['rid']]['roleActionList']) ){ 
									if (isset($akcje[$i+1]) && $i+1==$akcje[$i+1]){
										print('checked="checked');
									}
								}
					}
					?>"/></td></tr>
					<?	
						 
					}
					?>
					</tbody></table>
					</div>
					
					<div style="float: left; margin-top: 50px; margin-left: 10px;">
					<table class="searchtab_results"><tbody>
					<tr><th>Nazwa oida</th><th>Lista praw<br/>&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;0&nbsp;&nbsp;Upd Hist</th></tr>
					<?
					$temp=null;
					if (isset($_GET['rid'])){
						$temp=explode("<br/>",(string)$newArray[$_GET['rid']]['roleRightsCodes']);
					}
					for ($j=0;$j<sizeof($_SESSION['roleRightsOids']);$j++){									
									?>
										<tr><td class="w_lewo"><?=$oidsDesc[str_replace("_",".",substr($_SESSION['roleRightsOids'][$j],3))]['opis']?></td>
										<td><?php 	
										
										for($k=1;$k<=9;$k++){
										
											$prawa='';
											if ($k < 8 ){ $prawa=" disabled "; }
											if (isset($_GET['rid'])){
												if (isset($newArray[$_GET['rid']]['roleRightsCodes']) && $temp[$j][$k-1]=='1'){
													$prawa=' checked="checked"';
												}
											}
											print('<input class="searchtab_form" type="checkbox" name="'.$j.'_'.$k.'"'.$prawa.'/>');											
										}
										?></td></tr>
									<?
							}
					?>		
				</tbody></table><br/>
				</div>	
			</form>
<br />
</fieldset>
</div>
<div class="wysrodkuj">
<a href="roles.php">Lista ról</a>|<a href="apanel.php">Panel administracyjny</a></div>
<?php 
require_once "tail.php";
?>