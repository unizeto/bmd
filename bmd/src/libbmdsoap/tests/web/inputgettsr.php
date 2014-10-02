<?
session_start();
require_once('DAO/com_func.php');
check_siteblock();


draw_htmlhead('Pobieranie znacznika czasu dla pliku - SOE SOAP WWW','');
print('<div class="glowny">
<fieldset>
<legend>GET TIMESTAMP</legend>

<form action="bmdgettimestamp.php" method="post" enctype="multipart/form-data">
<table class="w_lewo"><tbody>
<tr><td>Plik do znakowania czasem</td>
<td><input type="file" name="filename" id="filename"></td></tr>
</tbody></table><br />
<input type="submit" value="Wyślij" class="fontbold">
</form>
</fieldset></div>');
require_once "tail.php";
?>