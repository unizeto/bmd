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
				<title>hash</title>
    <link href="../css/style.css" rel="StyleSheet" type="text/css" />
			</head>
		<body>
    <table width="95%" border="0" align="center" cellpadding="1" cellspacing="1">
     <tr>
      <td colspan="6" class="hashValue" style="font-size: 18px;color : #1996c7;"><b>DAMAGED NODES</b></td>
     </tr>
     <tr>
      <td>&nbsp;</td>
     </tr>
     <tr>
      <td colspan="6" class="hashLabel" align="center">LAST VERIFICATION</td>
     </tr>
     <? $verifyInfo = $dbHandle->select("hash", "max(id) as id,max(verification_date_time) as time", "verification_date_time in (select max(verification_date_time) from hash) AND (sig_value is not null)"); ?>
     <tr>
      <td class="hashValue" align="center">SIGN NODE ID</td>
      <td class="hashValue" align="center" colspan="5"><?=$verifyInfo[0]['id'];?></td>
     </tr>
     <tr>
      <td class="hashValue" align="center">VERIFICATION TIME</td>
      <td class="hashValue" align="center" colspan="5"><?=$verifyInfo[0]['time'];?></td>
     </tr>
     <tr>
      <td>&nbsp;</td>
     </tr>
     <tr>
       <td align="center" class="hashLabel">NODES</td>
       <td align="center" class="hashLabel">STATUS</td>
       <td align="center" class="hashLabel">LIST</td>
     </tr>
     <tr><td colspan="3"></td></tr>
     <tr>
      <td align="center" class="hashValue">HASH NODES</td>
      <? $nodes = $dbHandle->select("hash", "id", "verification_date_time in (select max(verification_date_time) from hash) AND (verification_status != 0)"); ?>
      <? $num = count($nodes); ?>
      <td align="center" class="hashValue">
       <? if($num > 0): ?>
        <img src="../pictures/flag_red.gif" />
       <? else: ?>
        <img src="../pictures/flag_green.gif" />
       <? endif ?>
      </td>
      <td align="center" class="hashValue" width="100">
       <? if($num > 0): ?>
        <input style="font-size: 10px" type="button" value="SHOW" onclick="window.location='badhashes.php'" />
       <? else: ?>
        <input style="font-size: 10px" disabled type="button" value="SHOW" />
       <? endif ?>
      </td>
     </tr>
     <tr>
      <td align="center" class="hashValue">LOG NODES</td>
      <? $nodes = $dbHandle->select("log", "id", "verification_date_time in (select max(verification_date_time) from log) AND (verification_status != 0)"); ?>
      <? $num = count($nodes); ?>
      <td align="center" class="hashValue">
       <? if($num > 0): ?>
        <img src="../pictures/flag_red.gif" />
       <? else: ?>
        <img src="../pictures/flag_green.gif" />
       <? endif ?>
      </td>
      <td align="center" class="hashValue" width="100">
       <? if($num > 0): ?>
        <input style="font-size: 10px" type="button" value="SHOW" onclick="window.location='badlogs.php'" />
       <? else: ?>
        <input style="font-size: 10px" disabled type="button" value="SHOW" onclick="" />
       <? endif ?>
      </td>
     </tr>
    </table>
	</body>
</html>
