<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
			<head>
			<meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
			<meta name="author" content="Unizeto Technologies SA" />
				<title>BMD Tree Viewer</title>
   <script src="lib/jquery.js" type="text/javascript"></script>
   <script src="lib/jquery.cookie.js" type="text/javascript"></script>
		 </head>
 <frameset scrolling="auto" resize="no" frameborder="1" border="5" framespacing="0" rows="13%,87%" id="MainFrame">
   <frame scrolling="no" noresize frameborder="1" marginheight="0" marginwidth="0" src="pages/header.html" title="Header" name="HeaderFrame" style="border : 0" />
   <frameset scrolling="yes" frameborder="1" border="5" framespacing="0" cols="50%,50%" id="TreeFrameset">
   <? if (isset( $_POST['logBranch'])): ?>
    <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/tree.php?mainRoot=0&logBranch=<?= $_POST['logBranch'];?>&hashBranch=0" title="Layout frame: NavFrame" name="TreeFrame" style="border : 0" />
   <? elseif (isset( $_POST['hashBranch'])): ?>
    <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/tree.php?mainRoot=0&hashBranch=<?= $_POST['hashBranch'];?>&logBranch=0" title="Layout frame: NavFrame" name="TreeFrame" style="border : 0" />
   <? else: ?>
    <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/tree.php" title="Layout frame: NavFrame" name="TreeFrame" style="border : 0" />
   <? endif ?>
     <frameset scrolling="yes" frameborder="1" border="5" framespacing="0" rows="60%,40%" id="HashFrameset">
      <? if (isset( $_POST['logBranch'])): ?>
       <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/log.php?logId=<?= $_POST['logBranch'];?>" title="Layout frame: NavFrame" name="PropFrame" style="border : 0" />
      <? elseif (isset( $_POST['hashBranch'])): ?>
       <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/hash.php?hashId=<?= $_POST['hashBranch'];?>" title="Layout frame: NavFrame" name="PropFrame" style="border : 0" />
      <? else: ?>
       <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/hash.php" title="Layout frame: NavFrame" name="PropFrame" />
      <? endif ?>
     <frame scrolling="yes" resize="yes" frameborder="1" marginheight="0" marginwidth="0" src="pages/badnode.php" title="Layout frame: ContentFrame" name="LogFrame"/>
      </frameset>
   </frameset>
  </frameset>
</html>
