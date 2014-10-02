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
     <td colspan="3" class="hashValue" style="font-size: 18px;color : #1996c7;"><b>TREE NODE PROPERTIES</b></td>
     <td class="hashValue" align="center"><a href="hash.php"><img src="../pictures/exit.png" border="0" /></a></td>
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
    <? if ( isset( $_GET['hashId' ] ) ) : ?>
     <? $hash = $dbHandle->select("hash", "*", "id=". $_GET['hashId']); ?>
     <tr>
      <td align="center" class="hashValue">
       <? if( $hash[0]['sig_value'] != "" ): ?>
        <input style="font-size: 10px" type="button" value="Verify" onclick="verifyFunction('audit','<?= $hash[0]['id'] ?>','?node=<?= $hash[0]['id'] ?>');" />
       <? else: ?>
        <input style="font-size: 10px" type="button" value="Verify" disabled />
       <? endif ?>
      </td>
      <td align="center" class="hashValue">
       <input style="font-size: 10px" type="button" value="../" onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?hashBranch=<?= $hash[0]['id'] ?>&logBranch=0&mainRoot=0'" />
      </td>
      <td align="center" class="hashValue">
       <input style="font-size: 10px" type="button" value="&nbsp;/&nbsp;" onclick="window.parent.document.getElementsByName('TreeFrame')[0].src = 'pages/tree.php?hashBranch=<?= $hash[0]['id'] ?>&logBranch=0&mainRoot=1'" />
      </td>
      <td align="center" class="hashValue">
       <? if ($hash[0]['verification_status'] == ""): ?>
        <img src="../pictures/status_unknown2.png" />
       <? elseif ( $hash[0]['verification_status'] == 0 ): ?>
        <img src="../pictures/flag_green.gif" />
       <? else: ?>
        <img src="../pictures/flag_red.gif" />
       <? endif ?>
      </td>
     </tr>
     <tr>
      <td>&nbsp;</td>
     </tr>
     <tr>
      <td class="hashLabel" colspan="4" align="center">HASH NODE INFORMATION</td>
     </tr>
     <tr>
      <td colspan="4" align="center"></td>
     </tr>
     <tr>
      <td class="hashLabel">ID</td><td colspan="3" class="hashValue"><?= $hash[0]['id'] ?></td>
     </tr>
     <tr>
      <td class="hashLabel">HASH</td><td colspan="3" class="hashValue"><?= $hash[0]['hash_value'] ?></td>
     </tr>
     <tr>
      <td class="hashLabel">SIGNATURE</td><td colspan="3" class="hashValue"><?= $hash[0]['sig_value'] ?></td>
     </tr>
     <tr>
      <td class="hashLabel">STATUS INFORMATION</td>
      <td class="hashValue" colspan="3">
       <? if ( $hash[0]['verification_status'] == "" ): ?>
        Not yet verified
       <? elseif ( $hash[0]['verification_status'] == 0 ): ?>
        Verification success
       <? elseif( $hash[0]['verification_status'] == 1 ): ?>
        Hash node modification
       <? else: ?>
        Internal error occurred
       <? endif ?>
     </td>
    </tr>
    <? endif ?>
   </table>
	</body>
</html>
