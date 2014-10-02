<?
  session_start();

  if( file_exists("config/config.php") && file_exists("pages/utils.php") && file_exists("dictionary/dict_eng.php")) {
          include("config/config.php");
          include("pages/utils.php");
          include("dictionary/dict_eng.php");
   }

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
			<head>
			<meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
			<meta name="author" content="Unizeto Technologies SA" />
				<title><?= $appTitle; ?></title>
			<script language="Javascript" src="scripts/scripts.js"></script>
			<link href="css/style.css" rel="StyleSheet" type="text/css" />
		</head>
		<body>
			<table width="90%" border="0" align="center" cellpadding="1" cellspacing="1">
				<tr>
					<td colspan="8"><h1><b><?= " ".$appTitle. " "; ?></b></h1></td>
				</tr>
				<tr>
					<td style="width:60px;">
						<input type="image" src="pictures/2leftarrow.png" title="<?= $firsPage; ?>" name="first" value="0" onclick="changeoffset(this.value);">
					</td>
					<td style="width:60px">
						<input type="image" src="pictures/2rightarrow.png" title="<?= $lastPage; ?>" name="last" value="<?= ((int)($maxOffset/$limit))*$limit; ?>" onclick="changeoffset(this.value);">
					</td>
					<td style="width:60px">
						<input type="image" src="pictures/1leftarrow.png" title="<?= $prevPage; ?>" name="prev" value="<?= (($offset-$limit < 0) ? 0 : $offset-$limit); ?>" onclick="changeoffset(this.value);">
					</td>
					<td style="width:60px">
						<input type="image" src="pictures/1rightarrow.png" title="<?= $nextPage; ?>" name="next" value="<?= (($offset+$limit > $maxOffset ) ? $offset : $offset+$limit); ?>" onclick="changeoffset(this.value);">
					</td>
					<td width="55%">&nbsp;</td>
					<td style="width:50px;" align="center">
						<a href="tree/index.php" ><img style="border: 0" src="pictures/tree.png" title="<?= $settings; ?>" /></a>
					</td>
					<td style="width:10%;text-align:right;font-family: verdena, sans-serif;color: #116b8e"><nobr><b><?= $numberLines; ?></b></nobr></td>
					<td align="right" style="width:20px;">
						<form id='panel' method='POST'>
							<input type="hidden" name="offset" id="offset" value="<? echo $offset; ?>" />
							<input type="hidden" name="appState" id="appState" value="<?= $appState; ?>" />
							<input type="hidden" name="user" id="user" value="<? echo $user; ?>" />
							<input type="hidden" name="opType" id="opType" value="<? echo $opType; ?>" />
							<input type="hidden" name="remotePid" id="remotePid" value="<? echo $remotePid; ?>" />
							<input type="hidden" name="remotePort" id="remotePort" value="<? echo $remotePort; ?>" />
							<input type="hidden" name="logId" id="logId" value="<? echo $logId; ?>" />
							<input type="hidden" name="sortBy" id="sortBy" value="" />
							<select name="limit" onchange="document.getElementById('panel').submit();">
								<option value="5" <?=(($limit == '5') ? 'selected':'')?>>5</option>
								<option value="10" <?=(($limit == '10') ? 'selected':'')?>>10</option>
								<option value="20" <?=(($limit == '20') ? 'selected':'')?>>20</option>
								<option value="50" <?=(($limit == '50') ? 'selected':'')?>>50</option>
								<option value="100" <?=(($limit == '100') ? 'selected':'')?>>100</option>
								<option value="500" <?=(($limit == '500') ? 'selected':'')?>>500</option>
							</select>
						</form>
					</td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>

				<?
					 switch( $appState )
						{
							case 1 :
												include('pages/users.php');
												break;
							case 2 :
												include('pages/events.php');
												break;
							case 3 :
												include('pages/addinfo.php');
						}
				?>
			
				</table>
			</td>
		</tr>
		<tr>
			<th colspan="8" align="center"><?= $stopka1 . " " . ceil(((($maxOffset+1)/$limit)-((($maxOffset+1)-$offset)/$limit))+1) . " " . $stopka2 . " " . ceil(($maxOffset+1) / $limit); ?></th>
		</tr>
	</table>
	</body>
</html>
