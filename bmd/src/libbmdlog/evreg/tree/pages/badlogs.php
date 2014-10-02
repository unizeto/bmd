<?
  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);

if( file_exists('utils.php') && file_exists('../../config/config.php')) {
    include_once('../../config/config.php');
    include('utils.php');
}

$limit = 5;

if (isset($_GET['offset'])){
 $offset = ( $_GET['offset'] < 0 ) ? 0 : $_GET['offset'];
 }
else{
 $offset = 0;
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
      <td colspan="2" class="hashValue" style="font-size: 18px;color : #1996c7;"><b>DAMAGED LOGS LIST</b></td>
      <td align="center" class="hashValue">
      </td>
      <td align="center" class="hashValue">
      <? if ( $offset == 0 ): ?>
       <input type="button" value="<" disabled onclick="window.location='?offset=<?= $offset-$limit; ?>'" />
      <? else: ?>
       <input type="button" value="<" onclick="window.location='?offset=<?= $offset-$limit; ?>'" />
      <? endif ?>
      </td>
      <td align="center" class="hashValue">
       <? $logs = $dbHandle->select("log", "*", "verification_date_time in (select max(verification_date_time) from log) AND (verification_status != 0)","","id", $offset, $limit) ?>
       <? if (count($logs) < $limit): ?>
        <input type="button" value=">" disabled onclick="window.location='?offset=<?= $offset+$limit; ?>'" />
       <? else: ?>
        <input type="button" value=">" onclick="window.location='?offset=<?= $offset+$limit; ?>'" />
       <? endif ?>
     </td>
     <td align="center" class="hashValue">
      <a href="badnode.php"><img id="lock" src="../pictures/exit.png" border="0" /></a>
     </td>
     </tr>
     <tr><td>&nbsp;</td></tr>
     <tr>
      <td align="center" class="hashLabel"><b>ID</b></td>
      <td align="center" class="hashLabel"><b>HASH VALUE</b></td>
      <td align="center" class="hashLabel"><b>STATUS</b></td>
      <td align="center" class="hashLabel"><b>SUBPATH</b></td>
      <td align="center" class="hashLabel"><b>FULLPATH</b></td>
      <td align="center" class="hashLabel"><b>INFO</b></td>
     </tr>
       <? foreach( $logs as $log ): ?>
        <tr>
          <td align="center" class="hashValue"><?= $log['id'] ?></td>
          <td align="center" class="hashValue"><?= $log['hash_value'] ?></td>
          <td align="center" class="hashValue">
           <? if ($log['verification_status'] == 1): ?>
            <img src="../pictures/flag_red.gif" />
           <? else: ?>
            <img src="../pictures/flag_orange.gif" />
           <? endif ?>
          </td>
          <td align="center" class="hashValue">
           <input type="button" value="../" <?=($log['verification_status'] < 0) ? "disabled" : "" ?> onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?logBranch=<?= $log['id'] ?>&hashBranch=0&mainRoot=0'" />
          </td>
          <td align="center" class="hashValue">
           <input type="button" value="/" <?=($log['verification_status'] < 0) ? "disabled" : "" ?>  onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?logBranch=<?= $log['id'] ?>&hashBranch=0&mainRoot=1'" />
          </td>
           <td align="center" class="hashValue">
            <input style="font-size: 10px" type="button" value="SHOW" onclick="window.parent.document.getElementsByName('PropFrame')[0].src = 'pages/log.php?logId=<?= $log['id'];?>'" />
           </td>
       </tr>
       <? endforeach ?>
       <? $dbHandle->__destroy(); ?>
    </table>
	</body>
</html>
