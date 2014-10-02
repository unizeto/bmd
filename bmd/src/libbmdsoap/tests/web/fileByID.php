<?php

session_start();
require_once('DAO/com_func.php');
check_siteblock();
draw_htmlhead('Pobieranie plików z bazy - SOE SOAP WWW','');
print("
<script type='text/javascript'>
function cursor(event)
{
document.getElementById('trail').style.visibility='visible';
document.getElementById('trail').style.position='absolute';
document.getElementById('trail').style.left=event.clientX+10;
document.getElementById('trail').style.top=event.clientY;
}
</script>
");
//require_once("head.php");
print('<div class="glowny"><fieldset>');
$przycisk="Pobierz";
$title="";
switch($_GET['formtype']){
	case 'hash': 
		$nazwa="GET FILE BY HASH";
		$akcja="bmdgetfilebyhash.php";
		$opis="Podaj HASH pliku do pobrania";
	 break;
	case 'file': 
		$nazwa="GET FILE BY ID";
		$akcja="bmdgetfilebyid.php";
		$opis="Podaj ID pliku do pobrania";
	 break;
	case 'zippack': 
		$nazwa="GET FILE BY ID IN ZIPPACK";
		$akcja="bmdgetfilebyidinzippack.php";
		$opis="Podaj ID pliku do pobrania paczki zip zawierającej również dowody";
	break;
	case 'pki': 
		$nazwa="GET FILE PKI BY ID";
		$akcja="bmdgetfilepkibyid.php";
		$opis="Podaj ID pliku do pobrania listy informacji dowodowych";
	break;
	
	case 'prop': 
		$nazwa="GET FILE PROP BY ID";
		$akcja="bmdgetfilepropbyid.php";
		$opis="Podaj ID pliku do pobrania wartości dowodowych";
	break;
	case 'history': 
		$nazwa="HISTORY";
		$akcja="bmdhistory.php";
		$opis="Podaj ID pliku";
	break;
	case 'del':
		$nazwa="DELETE FILE BY ID";
		$akcja="bmddeletefilebyid.php";
		$opis="Podaj ID pliku do skasowania";
		$przycisk="Usuń";
		$title="Kasowanie (listy) plików o wskazanych oidach.<br>
		Pojedynczo : 1  <br>
		Lista pojedynczych 1,2,3,4<br>  
		Lista 1-4  <br>
		Lista złożona 1-4,5,6,8-12 "; break;
	default: 
		print('Niepoprawne wywołanie strony<br /></fieldset></div>
<p style="text-align: center;"><a href="main.php">main</a></p>
</body>
</html>'); die(); break;//dowiedzieć się czy takie wyjście z aplikacji jest eleganckie
	
}

print('
<legend>'.$nazwa.'</legend>
<form action="'.$akcja.'" method="post">
<table class="w_lewo"><tbody><br /> 
<tr><td>'.$opis.'</td><td  onmouseover="cursor(event)"> 
<input type="text" name="ID"></td></tr>');

if ($_GET['formtype']==prop){
	print('<tr><td>Podaj TYP dowodu: 0 = podpis, 1-99 = znacznik czasu</td><td>
<input type="text" name="TYP"></td></tr>');
}

print('
<tr><td>&#160;</td><tr><tbody></table>
<input type="submit" value="'.$przycisk.'" class="fontbold">
</form>
</fieldset>

</div>');
print('<span id="trail" style="visibility:hidden; font-size: 10px; text-align: left; background-color: white;">'.$title.'</span>');
require_once('tail.php');

?>