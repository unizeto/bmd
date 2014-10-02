<?
session_start();
require_once('DAO/com_func.php');
check_siteblock();

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once "certyfikat.php";
$BMDSOAP = new BMDSOAP();

draw_htmlhead('Wysyłka,update plików w SOE SOAP WWW','search');
require_once "oidsDesc.php";

$count_cal = 0;
$cal_form = array();

///////////////roznice///////////////////
$przycisk="Wyślij";
$block1='<tr><td>Wysyłka w transakcji</td><td>
<input type="checkbox" name="trans" /></td></tr>';
$block2='';
$block3='<tr><td>ID katalogu/pliku nadrzędnego</td><td>
<input type="text" name="corespdir" id="corespdir" title="coresponding id" style="width: 300px;" /></td></tr>';

switch($_GET['formtype']){
	case 'insertF': 
		$nazwa="INSERT FILE (opt. signed) w/wo TRANSACTION";
		$akcja="insertfile.php";
		$block2='<tr><td>Wysłać z zewn. podpisem</td><td>
<input type="checkbox" name="podpis" id="podpis" onclick="javascript:common_grayinput(this,\'filename2\')"></td></tr>
<tr><td>Plik do wysyłki</td><td>
<input type="file" name="filename" id="filename" style="width: 300px;" /></td></tr>
<tr><td>Podpis dla pliku do wysyłki</td><td>
<input type="file" name="filename2" id="filename2" style="width: 300px;" disabled="true" /></td></tr>';
		break;
	case 'insertSig':
		$nazwa="INSERT ADDITIONAL SIGNATURE";
		$akcja="insertaddsig.php";
		$block1="";
		$block2='<tr><td>Podpis do pliku </td><td>
<input type="file" name="filename2" id="filename2" style="width: 300px;" /></td></tr>';
		break;
	case 'createD': 
		$nazwa="CREATE DIRECTORY (opt. signed) w/wo TRANSACTION";
		$akcja="createDir.php";
		$block2='<tr><td>Nazwa katalogu/folderu</td><td>
<input type="text" name="dirname" id="dirname" style="width:300px" /></td></tr>
<tr><td>Opis katalogu/folderu</td><td>
<input type="text" name="dirdesc" id="dirdesc" style="width:300px" /></td></tr>';
		break;
	case 'createL': 
		$nazwa="CREATE LINK  w/wo TRANSACTION";
		$akcja="createLink.php";
		$block2='<tr><td>Nazwa skrótu</td><td>
<input type="text" name="linkname" id="linkname" style="width:300px" /></td></tr>
<tr><td>Opis skrótu</td><td>
<input type="text" name="linkdesc" id="linkdesc" style="width:300px" /></td></tr>';
		$block3='<tr><td>ID katalogu linkowanego<br/> (opcja zamienna z Pointing ID)</td><td>
<input type="text" name="corespdir" id="corespdir" title="coresponding id" style="width: 300px;" /></td></tr>';
		break;
	case 'updateF': 
		$nazwa="UPDATE METADATA";
		$akcja="updatemetadata.php";
		$block1="";
		$block2='<tr><td>Podaj ID pliku</td><td>
<input type="text" name="ID" style="width:300px" /></td></tr>
<tr><td>Podaj powód updateu</td><td>
<input type="text" name="updateReason"  style="width:300px" /></td></tr>';
		$block3="";
		$przycisk="Uaktualnij";
		break;	
	default: 
		$block1="";
		$block2="";
		$block3="";
		print('Niepoprawne wywołanie strony<br /></fieldset></div>
<p style="text-align: center;"><a href="main.php">main</a></p>
</body>
</html>'); die();
		break;	
}

//////////////////////////////////////
print('<div class="glowny"><fieldset>
<legend>'.$nazwa.'</legend>');
if ($_GET['formtype'] !== 'insertSig'){
	print('<p><b>Pozostawienie pól pustych omija je.</b></p><p>Skrypt automatycznie odrzuca z tworzenia formularza oidy systemowe!</p>');	
}

print('<br/><form action="'.$akcja.'" method="post" enctype="multipart/form-data" name="insertform">
<table class="w_lewo"><tbody>'.$block1.$block2.$block3.'
<tr><td>&#160;</td><td>&#160;</td></tr>
');

if ($_GET['formtype'] !== 'insertSig'){

	$licznik=1;
	
	$_SESSION['mtdOid']=array();
	$_SESSION['mtdDesc']=array();
	foreach ($oidsDesc as $value) {
		if ($value['updt'] && $value['pokaz']){
			$_SESSION['mtdOid'][]=$value['OID'];
			$_SESSION['mtdDesc'][]=$value['opis'];
		print('<tr><td ');
		if ($value['opcja']) { print('title="metadana opcjonalna" ');}
		print('>' . $value['opis'] .'</td><td><input type="text" name=wrzuc'.$licznik.' style="');
			if ($value['opcja']) { print('background-color: #dddddd; ');}
			print(' width:');
			if ($value['typ']==3) {
			print('
			280px; text-align: center;" /><a href="javascript:cal'.++$count_cal.'.popup();"><img src="calendar/img/cal.gif" width="16" height="16" border="0" margin="0"  alt="Naciśnij tutaj by wybrać datę"></a>
			');		
			$cal_form[]="wrzuc".$licznik;
			} else {print('300px;">');}
			print('</td></tr>');
			$licznik+=1;
		}
	}//end foreach
	$_SESSION['ilosc']=$licznik;
	print('<tr><td>&#160;</td><td> <input type="submit" value="'.$przycisk.'" class="fontbold">&nbsp;&nbsp;&nbsp;&nbsp;<input type="submit" value="Wyczyść" onclick="javascript:common_clearForms(this.form);return false;" ></td></tr></tbody></table></form>');
	
	print('
	<script language="JavaScript">
	<!-- 
	// create calendar object(s) just after form tag closed
	// specify form element as the only parameter (document.forms["formname"].elements["inputname"]);
	// note: you can have as many calendar objects as you need for your application
	');
	for ($i=1;$i<=sizeof($cal_form);$i++){
		print('
		var cal'.$i.' = new calendar3(document.forms[\'insertform\'].elements[\''.$cal_form[$i-1].'\']);
		cal'.$i.'.year_scroll = true;
		cal'.$i.'.time_comp = false;');
	}
	print('//-->
	</script>');

}
if ($_GET['formtype'] == 'insertSig'){
	print('<tr><td>&#160;</td><td> <input type="submit" value="'.$przycisk.'" class="fontbold">&nbsp;&nbsp;&nbsp;&nbsp;<input type="submit" value="Wyczyść" onclick="javascript:common_clearForms(this.form);return false;" ></td></tr></tbody></table></form>');
}
print('</fieldset></div>');
require_once "tail.php";
?>