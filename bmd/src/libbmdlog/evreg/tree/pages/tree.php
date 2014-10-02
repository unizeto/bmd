<?
  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

	<meta http-equiv="content-type" content="text/html; charset=iso-8859-1"/>
	<title>BMD Tree Viewer</title>
	
	<link rel="stylesheet" href="../css/jquery.treeview.css" />
 <link rel="stylesheet" href="../css/style.css" />
	
	<script src="../lib/jquery.js" type="text/javascript"></script>
	<script src="../lib/jquery.cookie.js" type="text/javascript"></script>
	<script src="../lib/jquery.treeview.js" type="text/javascript"></script>
	<script src="../lib/jquery.treeview.async.js" type="text/javascript"></script>
	<script src="../lib/tscript.js" type="text/javascript"></script>
 	
	<script type="text/javascript">

 var SELECTED_ELEMENT = null;

<?

 if (isset($_REQUEST['node']))
  {
    $params = '?node='.$_REQUEST['node'];
  }
 elseif(isset($_REQUEST['mainRoot']) && isset($_REQUEST['logBranch']) && isset($_REQUEST['hashBranch']))
  {
    $params = '?mainRoot='.$_REQUEST['mainRoot'].'&hashBranch='.$_REQUEST['hashBranch'].'&logBranch='.$_REQUEST['logBranch'];
  }
 else
  {
    $params = '';
  }

 if( isset($_REQUEST['logBranch']) && $_REQUEST['logBranch'] > 0){
    echo 'SELECTED_ELEMENT = "log_'.$_REQUEST['logBranch'].'"';
  }
 elseif( isset($_REQUEST['hashBranch']) && $_REQUEST['hashBranch'] > 0){
    echo 'SELECTED_ELEMENT = "hash_'.$_REQUEST['hashBranch'].'"';
  }

?>

	$(document).ready(function(){
      $("#black").treeview({ url:"source.php<?= $params ?>"});
  });


  function changeSelection(el) {
      var liEl = document.getElementById(SELECTED_ELEMENT);
      if (liEl != null) {
      liEl.style.border = "none";
      }
      SELECTED_ELEMENT = el;
      liEl = document.getElementById(SELECTED_ELEMENT);

      if (liEl != null) {
       liEl.style.border = "3px solid red";
      }
  }
	</script>
	
	</head>
         <body>
	<ul id="black">
 </ul>
 </body></html>