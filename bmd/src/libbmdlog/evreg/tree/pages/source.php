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

<? if ($_REQUEST['root'] == 'source' && isset($_REQUEST['mainRoot']) && ( isset($_REQUEST['hashBranch']) || isset($_REQUEST['logBranch']))): ?>
<?

 $mainRoot = $_REQUEST['mainRoot'];
 $hashBranch = $_REQUEST['hashBranch'];
 $logBranch = $_REQUEST['logBranch'];

  $path = getHashPath( $dbHandle, $hashBranch, $logBranch, (bool)$mainRoot );

  $tree = prepareRoot( $dbHandle, $path, $hashBranch, $logBranch );

  echo json_encode($tree);
?>
<? elseif ($_REQUEST['root'] == 'source' && isset($_REQUEST['node'])) : ?>
[
{
    "text": <? $sigValue = $dbHandle->select("hash", "sig_value", "id=".$_REQUEST['node'] ); ?>
            <? $parent = $dbHandle->select("assoc", "id", "hash_src=".$_REQUEST['node'] ); ?>
            <? if ( $sigValue[0]['sig_value'] != "" ): ?>
             "<img src='../pictures/icon_key.gif' /><? $nodeStatus = checkHashNode($dbHandle,$_REQUEST['node']); ?>
             <? if ( $nodeStatus == 1 ): ?><img src='../pictures/flag_red.gif' /><? elseif( $nodeStatus == 0 ) : ?><img src='../pictures/flag_green.gif' /><? else : ?>
             <img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"hash_<?=$_REQUEST['node']?>\" onclick=\"changeSelection(this.id)\" href=\"hash.php?hashId=<?=$_REQUEST['node']?>\" target=\"PropFrame\"><b>{SHA1:<?= $_REQUEST['node'] ?>}</b>"
             <? if ( isset($parent[0]) ): ?>
               + "<a href='?node=<?= $parent[0]['id'] ?>'><img style='border: none' src='../pictures/arrow_up.gif' /></a>"
             <? endif ?>
            <? else : ?>
             "<img src='../pictures/kformula.png' /><? $nodeStatus = checkHashNode($dbHandle,$_REQUEST['node']); ?>
             <? if ( $nodeStatus == 1 ): ?><img src='../pictures/flag_red.gif' /><? elseif( $nodeStatus == 0 ) : ?><img src='../pictures/flag_green.gif' /><? else : ?>
             <img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"hash_<?=$_REQUEST['node']?>\" onclick=\"changeSelection(this.id)\" href=\"hash.php?hashId=<?=$_REQUEST['node']?>\" target=\"PropFrame\"><b>&nbsp;{SHA1:<?= $_REQUEST['node'] ?>}</b>"
              <? if ( isset($parent[0]) ): ?>
               + "<a href='?node=<?= $parent[0]['id'] ?>'><img style='border: none' src='../pictures/arrow_up.gif' /></a>"
             <? endif ?>
            <? endif ?>
    ,"id": "<?= $_REQUEST['node'] ?>",
    hasChildren: true
}
]
<?php
  elseif ($_REQUEST['root'] == 'source') :
  ?>
[
{
    "text": "<img src='../pictures/icon_key.gif' /><? $nodeStatus=checkHashNode($dbHandle,getRoot( $dbHandle )) ?><? if ( $nodeStatus == 1 ): ?><img src='../pictures/flag_red.gif' /><? elseif( $nodeStatus == 0 ) : ?><img src='../pictures/flag_green.gif' /><? else : ?>
    <img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"hash_<?=getRoot( $dbHandle )?>\" onclick=\"changeSelection(this.id)\"  href='hash.php?hashId=<?=getRoot( $dbHandle )?>' target='PropFrame'><b>&nbsp;{SHA1:<?= getRoot( $dbHandle ) ?>}</b></a>",
    "id": "<?= getRoot( $dbHandle ) ?>",
    hasChildren: true
}
]
<? else : ?>

<? $node = $_REQUEST['root']; ?>
[
<?
    $nodeSet = $dbHandle->select("assoc", "log_src,hash_src", "id=$node" );
    $array = array();
    $i = 0;
    foreach( $nodeSet as $treeNode ) : ?>
{
     <? $i++; ?>
     <?  if ( $treeNode['hash_src'] != "" ) : ?>
       <?  $sigValue = $dbHandle->select("hash", "sig_value", "id=".$treeNode['hash_src']); ?>
       <? if ( $sigValue[0]['sig_value'] != "" ) : ?>
           text: "<img src='../pictures/icon_key.gif' /><? $nodeStatus=checkHashNode($dbHandle,$treeNode['hash_src']); ?><? if($nodeStatus == 1): ?><img src='../pictures/flag_red.gif' /><? elseif($nodeStatus == 0) : ?><img src='../pictures/flag_green.gif' /><? else: ?><img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"hash_<?=$treeNode['hash_src']?>\" onclick=\"changeSelection(this.id)\" href=\"hash.php?hashId=<?=$treeNode['hash_src']?>\" target=\"PropFrame\"><b>&nbsp;{SHA1:<?=$treeNode['hash_src']?>}</b></a><a href=\"?node=<?=$treeNode['hash_src']?>\"><img style='border: none' src='../pictures/arrow_down.gif' /></a>",
           id: "<?=$treeNode['hash_src']?>",
           hasChildren: true
       <?   else : ?>
           text: "<img src='../pictures/kformula.png' /><? $nodeStatus=checkHashNode($dbHandle,$treeNode['hash_src']); ?><? if ($nodeStatus == 1): ?><img src='../pictures/flag_red.gif' /><? elseif($nodeStatus == 0 ) : ?><img src='../pictures/flag_green.gif' /><? else : ?><img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"hash_<?=$treeNode['hash_src']?>\" onclick=\"changeSelection(this.id)\"  href=\"hash.php?hashId=<?=$treeNode['hash_src']?>\" target=\"PropFrame\" ><b>&nbsp;{SHA1:<?=$treeNode['hash_src'] ?>}</b></a><a href=\"?node=<?=$treeNode['hash_src']?>\"><img style='border: none' src='../pictures/arrow_down.gif' /></a>",
           id: "<?=$treeNode['hash_src']?>",
          hasChildren: true
       <? endif ?>
     <? else : ?>
         text: "<img style='border: none' src='../pictures/mime_cdr.png' /><? $nodeStatus= checkLogNode($dbHandle,$treeNode['log_src']); ?><? if ($nodeStatus == 1): ?><img src='../pictures/flag_red.gif' /><? elseif( $nodeStatus == 0 ): ?><img src='../pictures/flag_green.gif' /><? else: ?><img src='../pictures/flag_orange.gif' /><? endif ?><a id=\"log_<?=$treeNode['log_src']?>\" onclick=\"changeSelection(this.id)\" href=\"log.php?logId=<?=$treeNode['log_src']?>\" target=\"PropFrame\"><b>&nbsp;{LOG:<?=$treeNode['log_src']?>}</b></a>",
         id: "<?= $treeNode['log_src']?>",
         hasChildren: false
     <? endif ?>
}
<? if ($i < count($nodeSet)): ?>
,
<? endif ?>
<? endforeach ?>
 ]
	<? endif ?>