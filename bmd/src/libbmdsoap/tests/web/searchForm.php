<?
session_start();
require_once('DAO/com_func.php');
check_siteblock();

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

draw_htmlhead('Wyszukiwanie plików w SOE SOAP WWW','search');
require_once "oidsDesc.php";
$count_cal = 0;
$cal_form = array();

print('<div class="glowny"><fieldset>
<legend>SEARCH FILES (opt. CSV)</legend>
<form action="bmdsearchfiles_win.php" method="post" name="searchform">
<table class="w_lewo"><tbody id="tab1">
<tr><td>Podaj punkt startowy wyszukiwań (offset)</td><td>
<input class="searchtab_form_zakres" type="text" name="offset" value="0" /></td></tr>
<tr><td>Podaj liczbę wyników w paczce</td><td>
<input class="searchtab_form_zakres" type="text" name="limit" value="100" /></td></tr>
<tr><td>Logika wyszukiwania - globalna</td><td>
<select class="searchtab_form_sel" name="logical">
<option value="AND">I - AND</option>
<option value="OR">LUB - OR</option>
</select></td></tr>
<tr><td>Sposób wyszukiwania - globalny</td><td>
<select class="searchtab_form_sel" name="goper">
<option value="LIKE" title="zawiera (z wielkością liter)">LIKE</option>
<option value="ILIKE" title="zawiera" selected="selected">ILIKE</option>
<option value="EXACT" title="dokładnie (z wielkością liter)">EXACT</option>
<option value="EQUAL" title="dokładnie">EQUAL</option>
</select></td></tr>');

if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') { 
print('<tr><td>Szukaj katalogu</td><td><input type="checkbox" name="katalog" id="katalog" title="Nie działa przy generowaniu CSV. Niezaznaczone oznacza, że szukamy plików o danym korespondującym ID wpisanym w metadanych." /></td></tr>
<tr><td>ID katalogu nadrzędnego<br />ID pliku korespondującego</td><td><input class="searchtab_form_zakres" type="text" name="corespID" title="Jeśli zaznaczony jako katalog to szukamy plików i katalogów w nim, jeśli nie to szukamy plików dołączenia." /></td></tr>
<tr><td>Typ pliku</td><td><input class="searchtab_form_zakres" type="text" name="filetype" title="Przydatne przy listowaniu katalogów-słowo klucz DIR" /></td></tr>');
}
print('<tr><td>&#160;</td><td>&#160;</td></tr>
</tbody></table>');
//$IDX=count($ret);
$licznik=0;

$_SESSION['mtdOid']=array();
//$_SESSION['mtdDesc']=array();
print('<table class="searchtab w_lewo"><tbody><tr>
<th>Opis metadanej</th>
<th>Szukana wartość</th>');
if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
print('<th>NOT</th>');
}
print('<th>Operator</th>
<th title="Wybór kolumn zwróconych w wyniku">Show</th>
<th title="Wybór metadanej/ych po której ma być sortowane" >Sortowanie</th>
</tr>');
foreach ($oidsDesc as $value) {
	if ($value['szukaj']){	
		$_SESSION['mtdOid'][]=$value['OID'];
		//$_SESSION['mtdDesc'][]=$value['opis'];
		$oid_in_sha=sha1($value['OID']);
		
		print('<tr><td>'. $value['opis'] .'</td>');
		if ($value['typ']==3 || $value['typ']==4){
			print('<td><a class="smallfont">od</a><input style="width:');
			if ($value['typ']==3) { print('135px;'); }else{ print('160px;'); }
			print(' text-align: center;" type="text" name="szukaj'.$licznik. '" />');
			//kalendarz
			if ($value['typ']==3) {
				print('
				<a href="javascript:cal'.++$count_cal.'.popup();">
				<img src="calendar/img/cal.gif" width="16" height="16" border="0" alt="Naciśnij tutaj by wybrać datę" /></a>
				');		
				$cal_form[]="szukaj".$licznik;
			}		
			print('<input type="checkbox" name="zakres'.$licznik.'" id="zakres'.$licznik.'"  onclick="javascript:common_grayinput(this,\'szukajdo'.$licznik.'\');" title="zaznaczenie określa czy ma być wykorzystane wyszukiwanie z zakresem - równoważność dwukropka" />');
			print('<a class="smallfont">do</a><input style="width:');
			if ($value['typ']==3) {print('135px;');}else{ print('160px;'); }
			print(' text-align: center;" type="text" id="szukajdo'.$licznik.'" name="szukajdo'.$licznik.'" disabled="disabled" />');
			//kalendarz
			if ($value['typ']==3) {
				print('
				<a href="javascript:cal'.++$count_cal.'.popup();"> 
				<img src="calendar/img/cal.gif" width="16" height="16" border="0" alt="Naciśnij tutaj by wybrać datę" /></a>
				');		
				$cal_form[]="szukajdo".$licznik;
			}
			print('</td>');
		}else{
		    print('<td> <input class="searchtab_form" type="text" name="szukaj'.$licznik. '" /></td>');
		}
		if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
			print('<td style="text-align: center;">');
			if ( $value['typ']!=3 && $value['typ']!=4 ){ 
				print('<input type="checkbox" name="negacja'.$oid_in_sha.'" 
				title="Zaznaczenie pola oznacza wyszukanie z wyłączeniem wprowadzonego parametru, 
				tj. wszystko poza wskazanym parametrem. Opcja nie działa na parametrów z zakresem." />');
			}else{ print('&#160;'); }
			
		}
		print('</td><td><select name="operwyb'.$licznik.'">
		    <option value="NULL" title="Korzysta z globalnego ustawienia">Globalny</option>
			<option value="LIKE" title="zawiera (z wielkością liter)">LIKE</option>
			<option value="ILIKE" title="zawiera">ILIKE</option>
			<option value="EXACT" title="dokładnie (z wielkością liter)">EXACT</option>
			<option value="EQUAL" title="dokładnie">EQUAL</option>
		</select></td>
		<td style="text-align: center;"><input type="checkbox" name="mtdsparams'.$licznik.'" title="Zaznaczenie pola określi które kolumny będą wyświetlone w wynikach. Jeśli wszystkie są puste w wynikach będą wszystkie możliwe kolumny." /></td>
		<td  style="text-align: center;"><select name="kierunek'.$licznik.'">
		<option value="NULL">Brak</option>
		<option value="ASC">Rosnąco - ASC</option>
		<option value="DESC">Malejąco - DESC</option>
		</select></td></tr>');
		$licznik+=1;
	}//end if szukaj
}
$_SESSION['ilosc']=$licznik;
print('<tr><td title="Zaznaczenie pola oznacza zapis wyników w postaci CSV"  style="background-color: #95ff78">Stworzyć CSV &nbsp;&nbsp;&nbsp;
<input type="checkbox" name="CSVresult" /></td>
<td><input type="submit" value="SZUKAJ" style="width: 100%; height: 45px; font-weight: bold;" /></td>');
if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
print('<td></td>');
}
print('<td><input type="submit" value="Wyczyść" onclick="javascript:common_clearForms(this.form);return false;" /></td><td></td><td></td>
</tr>');

if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') { 
print('<tr><td>Pokaż w postaci drzewa<input type="checkbox" name="drzewo" id="drzewo" /></td></tr>');
}

print('</tbody></table></form>');
print('
<script language="JavaScript" type="text/javascript">
<!-- 
// create calendar object(s) just after form tag closed
// specify form element as the only parameter (document.forms["formname"].elements["inputname"]);
// note: you can have as many calendar objects as you need for your application
');
for ($i=1;$i<=sizeof($cal_form);$i++){
print('
var cal'.$i.' = new calendar3(document.forms[\'searchform\'].elements[\''.$cal_form[$i-1].'\']);
cal'.$i.'.year_scroll = true;
cal'.$i.'.time_comp = false;');
}
print('//-->
</script>');

print('</fieldset></div>');

require_once "tail.php";
?>