<?php
session_start();
require_once('DAO/com_func.php');
check_siteblock();

//require_once("head.php");
draw_htmlhead('Żądania "PLAY specific" dla SOE SOAP WWW','');
print('
<div class="glowny">
<fieldset>');
$block='';
switch($_GET['formtype']){
	case 'verify': 
		$nazwa='VERIFY INVOICE';
		$akcja="verifyInvoice.php?modfunc=check"; 
		$block='Ponowienie żądania DVCS <input type="checkbox" name="forceV" checked="checked">';
		break;
	case 'verifyD': 
		$nazwa='VERIFY INVOICE WITH DETAILS';
		$akcja="verifyInvoice.php?modfunc=save"; 
		$block='Ponowienie żądania DVCS <input type="checkbox" name="forceV" checked="checked">';
		break;
	case 'getpack': 
		$nazwa='GET INVOICE PACK';
		$akcja="getinvoicepack.php";
		$block0='<tr><td>Powód pobrania</td><td><input type="text" name="getReason" class="invoiceform"></td></tr>';
		break;
	case 'htmlimage': 
		$nazwa='GET HTML IMAGE';
		$akcja="gethtmlimage.php";
		$block0='<tr><td>Powód pobrania</td><td><input type="text" name="getReason" class="invoiceform"></td></tr>'; 
		break;
	case 'count': 
		$nazwa='COUNT INVOICE';
		$akcja="countInvoice.php"; break;
	default: print('Złe wywołanie strony<br></fieldset>
</div>
<p style="text-align: center;"><a href="index.html">main</a></p>
</body>
</html>'); die(); break;
}
print('<legend>'.$nazwa.'</legend>
<p>Parametry są uzależnione od ustawień serwera. Zastosowano przykładowe.</p><br />
<form action="'.$akcja.'" method="post"><table class="w_lewo"><tbody> 

<tr><td>Podaj accountid, tu: EMAIL odbiorcy</td>
<td><input type="text" name="accountid" class="invoiceform"></td></tr>

<tr><td>Podaj invoiceId, tu: HASH pliku</td>
<td><input type="text" name="invoiceId" class="invoiceform"></td></tr>

<tr><td>Podaj userIdentifier, tu: EMAIL odbiorcy</td>
<td><input type="text" name="userIdentifier" class="invoiceform"></td></tr>
'.$block0.'
<tr><td>'.$block.'</td><td> <input type="submit" value="Pobierz" class="fontbold"></td></tr>
<tbody></table>
</form>
</fieldset>
</div>');
require_once('tail.php');
?>