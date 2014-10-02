<?

  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);
  
  header("Cache-Control: no-cache, must-revalidate");
  header('Pragma: no-cache');

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
				<title>log</title>
    <link href="../css/style.css" rel="StyleSheet" type="text/css" />
			</head>
		<body>
   <table width="95%" border="0" align="center" cellpadding="1" cellspacing="1">
    <tr>
     <td colspan="3" class="hashValue" style="font-size: 18px;color : #1996c7;"><b>TREE NODE PROPERTIES</b></td>
     <td class="hashValue" align="center"><a href="log.php"><img src="../pictures/exit.png" border="0" /></a></td>
    </tr>
    <tr>
     <td>&nbsp;</td>
    </tr>
    <tr>
     <td align="center" class="hashLabel">ACTION</td>
     <td align="center" class="hashLabel">SUBPATH</td>
     <td align="center" class="hashLabel">FULLPATH</td>
     <td align="center" class="hashLabel">STATUS</td>
    </tr>
    <? if ( isset( $_GET['logId' ] ) ) : ?>
     <? $log = $dbHandle->select("log", "*", "id=". $_GET['logId']); ?>
     <tr>
      <td align="center" class="hashValue">
       <input style="font-size: 10px" type="button" value="Verify" onclick="verifyFunction('verify','<?= $log[0]['id'] ?>','?node=<?= $log[0]['id'] ?>');"/>
      </td>
      <td align="center" class="hashValue">
       <input style="font-size: 10px" type="button" value="../" onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?hashBranch=0&logBranch=<?= $log[0]['id'] ?>&mainRoot=0'" />
      </td>
      <td align="center" class="hashValue">
       <input style="font-size: 10px" type="button" value="&nbsp;/&nbsp;" onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?hashBranch=0&logBranch=<?= $log[0]['id'] ?>&mainRoot=1'" />
      </td>
      <td align="center" class="hashValue">
       <? if ( $log[0]['verification_status'] == "" ): ?>
        <img src="../pictures/status_unknown2.png" />
       <? elseif ( $log[0]['verification_status'] == 0 ): ?>
        <img src="../pictures/flag_green.gif" />
       <? elseif( $log[0]['verification_status'] == 1 ): ?>
        <img src="../pictures/flag_red.gif" />
       <? else: ?>
        <img src="../pictures/flag_orange.gif" />
       <? endif ?>
      </td>
     </tr>
     <tr>
      <td>&nbsp;</td>
     </tr>
     <tr>
      <td class="hashLabel" colspan="4" align="center">LOG NODE INFORMATION</td>
     </tr>
     <tr>
      <td colspan="4" align="center"></td>
     </tr>
     <tr>
      <td class="logLabel">ID</td><td class="logValue" colspan="3"><?= $log[0]['id'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">REMOTE HOST</td><td class="logValue" colspan="3"><?= $log[0]['remote_host'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">REMOTE PORT</td><td class="logValue" colspan="3"><?= $log[0]['remote_port'] ?></td>
     </tr> 
     <tr>
      <td width="150" class="logLabel">REMOTE PID</td><td class="logValue" colspan="3"><?= $log[0]['remote_pid'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">DATE AND TIME BEGIN</td><td class="logValue" colspan="3"><?= $log[0]['date_time_begin'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">DATE AND TIME END</td><td class="logValue" colspan="3"><?= $log[0]['date_time_end'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">DATE AND TIME COMMIT</td><td class="logValue" colspan="3"><?= $log[0]['db_date_time_commit'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">SOURCE FILE</td><td class="logValue" colspan="3"><?= $log[0]['src_file'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">SOURCE LINE</td><td class="logValue" colspan="3"><?= $log[0]['src_line'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">SOURCE LINE</td><td class="logValue" colspan="3"><?= $log[0]['src_line'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">SOURCE FUNCTION</td><td class="logValue" colspan="3"><?= $log[0]['src_function'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">LOG LEVEL</td><td class="logValue" colspan="3"><?= $log[0]['log_level'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">LOG OWNER</td><td class="logValue" colspan="3"><?= $log[0]['log_owner'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">SERVICE</td><td class="logValue" colspan="3"><?= $log[0]['service'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">OPERATION TYPE</td><td class="logValue" colspan="3"><?= $log[0]['operation_type'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">LOG STRING</td><td class="logValue" colspan="3"><?= $log[0]['log_string'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">LOG REASON</td><td class="logValue" colspan="3"><?= $log[0]['log_reason'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">LOG SOLUTION</td><td class="logValue" colspan="3"><?= $log[0]['log_solution'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">DOCUMENT FILE NAME</td><td class="logValue" colspan="3"><?= $log[0]['document_filename'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">DOCUMENT SIZE</td><td class="logValue" colspan="3"><?= $log[0]['document_size'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">HASH VALUE</td><td class="logValue" colspan="3"><?= $log[0]['hash_value'] ?></td>
     </tr>
     <tr>
      <td width="150" class="logLabel">VERIFICATION STATUS</td>
      <td class="logValue" colspan="3">
       <? if ( $log[0]['verification_status'] == "" ): ?>
        Not yet verified
       <? elseif ( $log[0]['verification_status'] == 0 ): ?>
        Verification Success
       <? elseif( $log[0]['verification_status'] == 1 ): ?>
        Log node modification
       <? else: ?>
        Internal error occurred
       <? endif ?>
      </td>
     </tr>
    <? endif ?>
   </table>
	</body>
</html>
