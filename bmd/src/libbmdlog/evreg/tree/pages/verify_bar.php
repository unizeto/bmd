<?
  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);

  if( file_exists('utils.php') && file_exists('../../config/config.php')) {
    include_once('../../config/config.php');
    include('utils.php');
   }

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
			<head>
			<meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
			<meta name="author" content="Unizeto Technologies SA" />
   <script src="../lib/tscript.js" type="text/javascript"></script>
   
			<title>BMD Tree Viewer</title>
			</head>
	 <body onload="ajaxFunction('<?=$_POST['proc'];?>','<?= $_POST['id'];?>','');">
   <table style="margin-left: auto; margin-right: auto; margin-top: 20%" width="40%" border="0" cellpadding="1" cellspacing="1">
    <tr>
     <td align="center"><img src="../pictures/ajax-loader.gif" /></td>
    </tr>
    <tr>
     <td align="center">&nbsp;</td>
    </tr>
    <tr>
     <td style="font-family: verdena, sans-serif;color : #1996c7;" align="center"> Trwa werifikacja struktury drzewa, proszę czekać... </td>
    </tr>
   </table>
 	</body>
</html>
