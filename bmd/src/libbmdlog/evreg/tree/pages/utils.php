<?
  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);

function __autoload($class_name)
{
    if (file_exists("../classes/".strtolower($class_name) . '.php'))
{
      require_once "../classes/".strtolower($class_name) . '.php';
}
    else if (file_exists("../interfaces/".strtolower($class_name) . '.php'))
{
      require_once "../interfaces/".strtolower($class_name) . '.php';
}
    else return false;

    return true;
}



function checkHashNode($dbHandle, $node)
{
   $nodeSet = $dbHandle->select("hash", "verification_status", "id=".$node );
   $dbHandle->__destroy();

   if ( $nodeSet[0]['verification_status'] == 0 )
    {
      return 0;
    }
   elseif ( $nodeSet[0]['verification_status'] < 0 )
    {
      return -1;
    }

   return 1;
}


function checkLogNode($dbHandle, $node)
{

   $nodeSet = $dbHandle->select("log", "verification_status", "id=".$node );
   $dbHandle->__destroy();

   if ( $nodeSet[0]['verification_status'] == 0 )
    {
      return 0;
    }
   elseif ( $nodeSet[0]['verification_status'] < 0 )
    {
      return -1;
    }

   return 1;
}

  $dbHandle = new PgSqlDB($DB_HOST, $DB_PORT, $DB_LOGIN, $DB_PASSWORD, $DB_NAME );

  function getRoot( $dbHandle )
  {

    $nodeSet = $dbHandle->select("hash", "max(id)", "sig_value is not null" );

    $dbHandle->__destroy();

    return $nodeSet[0]['max'];
  }


  function getHashPath( $dbHandle, $hash, $log, $mainRoot )
  {

    $hashPath = array();

    if ( $log > 0 )
    {
      $nodeSet = $dbHandle->select("assoc", "id", "log_src=".$log );

      $hash = $nodeSet[0]['id'];
    }

    $hashPath[$hash] = $hash;

    if ( $mainRoot == true )
    {
      while(1)
      {
        $nodeSet = $dbHandle->select("assoc", "id", "hash_src=".$hash );

        if ( count($nodeSet) <= 0 ) break;
        
        $hash = $nodeSet[0]['id'];
        $hashPath[$hash] = $hash;

      }
    }
    else
    {
      $nodeSet = $dbHandle->select("hash", "sig_value", "id=".$hash );

      if ($nodeSet[0]['sig_value'] != "")
       {
         $hashPath[$hash] = $hash;
       }
      else
       {
         while(1)
          {
            $nodeSet = $dbHandle->select("assoc", "id", "hash_src=".$hash );

            if ( count($nodeSet) <= 0 ) break;

            $hash = $nodeSet[0]['id'];
            $hashPath[$hash] = $hash;

            $nodeSet = $dbHandle->select("hash", "sig_value", "sig_value is not null and id=".$hash );

            if ( count($nodeSet) > 0 ) break;
          }
       }
    }

    return $hashPath;

  }


 function expandBranch( $dbHandle, &$path, $hashBranch, $logBranch )
  {
    $treeTab = array();

    $parent = array_pop($path);

    $nodeSet = $dbHandle->select("assoc", "log_src,hash_src", "id=$parent" );

    $i = 0;

    foreach( $nodeSet as $node)
     {
       if ( $node['hash_src'] != "" )
        {
          $sigValue = $dbHandle->select("hash", "sig_value", "id=".$node['hash_src']);

          $treeTab[$i]['id'] = $node['hash_src'];

          if ( $sigValue[0]['sig_value'] != "" )
           {
             $treeTab[$i]['text'] = "<img src='../pictures/icon_key.gif' />";
           }
          else
           {
             $treeTab[$i]['text'] = "<img src='../pictures/kformula.png' />";
           }
          switch( checkHashNode($dbHandle, $node['hash_src']) )
          {
           case 1 : {
                       $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_red.gif' />";
                       break;
                    }
           case 0 : {
                       $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_green.gif' />";
                       break;
                    }
           default : {
                       $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_orange.gif' />";
                     }
          }

         if ($node['hash_src'] == $hashBranch){
             $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<a id=\"hash_{$node['hash_src']}\" onclick=\"changeSelection(this.id)\" style=\"border:3px solid red;\" href=\"hash.php?hashId={$node['hash_src']}\" target=\"PropFrame\"><b>{SHA1:{$node['hash_src']}}</b></a><a href=\"?node={$node['hash_src']}\"><img style='border: none' src='../pictures/arrow_down.gif' /></a>";
          }
         else{
           $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<a id=\"hash_{$node['hash_src']}\" onclick=\"changeSelection(this.id)\" href=\"hash.php?hashId={$node['hash_src']}\" target=\"PropFrame\"><b>{SHA1:{$node['hash_src']}}</b></a><a href=\"?node={$node['hash_src']}\"><img style='border: none' src='../pictures/arrow_down.gif' /></a>";
          }
          if (isset($path[$node['hash_src']]))
           {
             $treeTab[$i]['children'] = expandBranch( $dbHandle, $path, $hashBranch, $logBranch );
             $treeTab[$i]['expanded'] = true;
           }
          else
           {
             $treeTab[$i]['hasChildren'] = true;
           }
        }
       else
        {
           $treeTab[$i]['text'] = "<img style='border: none' src='../pictures/mime_cdr.png' />";

           switch (checkLogNode($dbHandle, $node['log_src']))
           {
              case 1 : {
                          $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_red.gif' />";
                          break;
                       }
              case 0 : {
                          $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_green.gif' />";
                          break;
                       }
              default :{
                          $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<img src='../pictures/flag_orange.gif' />";
                       }
           }

          if ( $node['log_src'] == $logBranch ){
             $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<a id=\"log_{$node['log_src']}\" onclick=\"changeSelection(this.id)\" style=\"border:3px solid red;\" href=\"log.php?logId={$node['log_src']}\" target=\"PropFrame\"><b>{LOG:{$node['log_src']}}</b></a>";
           }
          else{
            $treeTab[$i]['text'] = $treeTab[$i]['text'] . "<a id=\"log_{$node['log_src']}\" onclick=\"changeSelection(this.id)\" href=\"log.php?logId={$node['log_src']}\" target=\"PropFrame\"><b>{LOG:{$node['log_src']}}</b></a>";
           }

           $treeTab[$i]['id'] = $node['log_src'];
           $treeTab[$i]['hasChildren'] = false;
        }
       $i++;
     }

   return $treeTab;
  }


function prepareRoot( $dbHandle, &$path, $hashBranch, $logBranch )
 {
   $treeTab = array();

   $parent = end($path);

   $isRoot   = $dbHandle->select("assoc", "id", "hash_src=".$parent);

   $treeTab[0]['id'] = $parent;

   $treeTab[0]['text'] = "<img src='../pictures/icon_key.gif' />";

   switch(checkHashNode($dbHandle, $parent))
   {
     case 1 : {
                 $treeTab[0]['text'] = $treeTab[0]['text'] . "<img src='../pictures/flag_red.gif' />";
                 break;
              }
     case 0 : {
                 $treeTab[0]['text'] = $treeTab[0]['text'] . "<img src='../pictures/flag_green.gif' />";
                 break;
              }
     default :{
                 $treeTab[0]['text'] = $treeTab[0]['text'] . "<img src='../pictures/flag_orange.gif' />";
              }
   }

   if($parent == $hashBranch){
     $treeTab[0]['text'] = $treeTab[0]['text'] . "<a id=\"hash_{$parent}\" onclick=\"changeSelection(this.id)\" style=\"border:3px solid red;\" href=\"hash.php?hashId={$parent}\" target=\"PropFrame\"><b>{SHA1:{$parent}}</b></a>";
    }
   else{
     $treeTab[0]['text'] = $treeTab[0]['text'] . "<a id=\"hash_{$parent}\" onclick=\"changeSelection(this.id)\" href=\"hash.php?hashId={$parent}\" target=\"PropFrame\"><b>{SHA1:{$parent}}</b></a>";
    }

    if (count($isRoot)>0){
      $treeTab[0]['text'] = $treeTab[0]['text'] . "<a href=\"?node={$isRoot[0]['id']}\"><img style='border: none' src='../pictures/arrow_up.gif' /></a>";
     }

   $treeTab[0]['children'] = expandBranch( $dbHandle, $path, $hashBranch, $logBranch );
   $treeTab[0]['expanded'] = true;

  return $treeTab;

 }

 

?>