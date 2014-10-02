<?php

function draw_htmlhead($title,$option){
	
	print('<?xml version="1.0" encoding="UTF-8"?>
	<!DOCTYPE html PUBLIC 
	"-//W3C//DTD XHTML 1.0 Transitional//EN" 
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html xmlns="http://www.w3.org/1999/xhtml" lang="pl" xml:lang="pl">
	<head>	
		<meta http-equiv="content-type" content="text/html; charset=UTF-8" />
		<meta name="author" content="Unizeto Technologies SA - Bezpieczne Systemy Informatyczne" />
		<meta name="description" content="Producent i integrator rozwiązań informatycznych" />
		<meta name="keywords" content="Unizeto, Centrum Certyfikacji, e-podpis, certyfikat kwalifikowany, System Obsługi e-faktur, Bezpieczne Archiwum elektroniczne, soe, bae" />
		<meta name="Language" content="pl" />
	
		<title>'.$title.'</title>
<link rel="stylesheet" href="css/style.css" type="text/css" media="screen" />');
	switch($option){
		case 'searchtree':
			print('<script language="javascript"  src="js/ajax.js"  type="text/javascript"></script>
				<script  language="javascript"  src="js/jquery-1.3.2.js"  type="text/javascript"></script>
				<script language="javascript"  type="text/javascript">
				<!--
				function res(res,id) {				
					var container = document.getElementById(id);
					container.innerHTML += res;
				
					var newtip = document.createAttribute(\'onclick\');
					newtip.value = "schowaj(" + id + ");";
					document.getElementById(\'nad\' + id).attributes.setNamedItem(newtip);
				}
				
				function res_tree(res_tree,id) {				
					var container = document.getElementById(id);
					container.innerHTML = res_tree;
				}
							
				function schowaj(id){
					var container = document.getElementById(id);
					container.innerHTML = "";
				
					var newtip = document.createAttribute(\'onclick\');
					newtip.value = "parametry=\'corespID=" + id + "\'; ajaxMakeRequest(\'funkcja_tree.php\', \'res\', \'post\', parametry, false," + id + "); ";
					document.getElementById(\'nad\' + id).attributes.setNamedItem(newtip);
				}
				-->
				</script>'); 
			break; 
			 
		case 'search': print('<script  language="javascript" type="text/javascript" src="js/ajax.js"></script>
			<script  language="javascript" type="text/javascript" src="js/jquery-1.3.2.js"></script>
			<script  language="javascript" type="text/javascript" src="js/common.js"></script>
			<script  language="javascript" type="text/javascript" src="calendar/calendar3.js"></script>
			<script  language="javascript" type="text/javascript">
				<!--
					var kolejny=0;
					function res_criteria(res_criteria,id) {				
						var tableRef = document.getElementById(id);						
						tableRef.innerHTML = res_criteria;
					}
					
					function res_select_add(res_select_add,id) {
										
						var tableRef = document.getElementById(id);
						var newCell  = tableRef.insertCell();
						
						var newText  = document.createTextNode(res_select_add);
					  	newCell.appendChild(newText);
					  						  
					  	newRow.insertCell("warunek" + kolejny);
						kolejny=kolejny+1;
					}
					
					function res_select_rm(res_select_rm,id) {				
						var container = document.getElementById(id);
						container.innerHTML = res_select_rm;						
					}
										
				-->
			</script>

		'); break;
		default: print('<script language="javascript" type="text/javascript" src="js/common.js"></script>'); break;
	}
print('</head><body>');
}

function check_siteblock()
{
	if ( (isset($_SESSION['ERROR']) && !($_SESSION['ERROR']=='8')) || !isset($_SESSION['ERROR'])) { 
		//require_once("head.php");
		draw_htmlhead('Portal SOE WWW - komunikat','');
		print('<div class="glowny wysrodkuj">Nie udane połączenie z usługą lub niepoprawny wpis w polu adresu serwera SOAP.<br />
		Niemożliwa praca w systemie.<br /> Proszę wrócić na stronę główną logowania lub kliknąć <a href="index.php">tutaj</a></div>');	
		require_once("tailtree.php");	
		die(); 
	}
}


?>