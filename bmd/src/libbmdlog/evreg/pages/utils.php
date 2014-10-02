<?
	function sortColumns($colName)
	{
		if ( isset($_POST['sortBy'] ) && $_POST['sortBy'] == $colName )
		{
			if ( $_SESSION['sortBy'] == $colName )
			{
				if ( $_SESSION['sortDir'] == "ASC" )
				{
					$_SESSION['sortDir'] = "DESC";
?>
					<img src="pictures/14_layer_lowerlayer.png">
<?			}
				else
				{
					$_SESSION['sortDir'] = "ASC";
?>
					<img src="pictures/14_layer_raiselayer.png">
<?			}
			}
			else
			{
				$_SESSION['sortDir'] = "ASC";
				$_SESSION['sortBy'] = $colName;
?>
				<img src="pictures/14_layer_raiselayer.png">
<?
			}
		}
		else if ( $_SESSION['sortBy'] == $colName && empty($_POST['sortBy']) )
		{
			if ( $_SESSION['sortDir'] == "ASC")
			{
?>
				<img src="pictures/14_layer_raiselayer.png">
<?
			}
			else
			{
?>
				<img src="pictures/14_layer_lowerlayer.png">
<?
			}
		}
	}

  $dbHandle = new PgSqlDB($DB_HOST, $DB_PORT, $DB_LOGIN, $DB_PASSWORD, $DB_NAME );

  if($dbHandle->connect())
  {
    print("Database connection failed");
    die();
  }

  ini_set("display_errors",1);
  error_reporting(E_STRICT|E_ALL);

  function __autoload($class_name)
  {
    if (file_exists("classes/".strtolower($class_name) . '.php'))
    {
      require_once "classes/".strtolower($class_name) . '.php';
    }
    else if (file_exists("interfaces/".strtolower($class_name) . '.php'))
    {
      require_once "interfaces/".strtolower($class_name) . '.php';
    }
    else return false;

    return true;
  }

	if ( empty($_SESSION['offset']) )
	{
		$_SESSION['offset'] =  0;
	}
	if ( empty($_SESSION['limit']) )
	{
		$_SESSION['limit']  = 10;
	}
	if ( empty($_SESSION['user']) )
	{
		$_SESSION['user'] = '';
	}
	if ( empty($_SESSION['appState']) )
	{
		$_SESSION['appState'] = 1;
	}
	if ( empty($_SESSION['opType']) )
	{
		$_SESSION['opType'] = '';
	}
	if ( empty($_SESSION['remotePid']) )
	{
		$_SESSION['remotePid'] = '';
	}
	if ( empty($_SESSION['remotePort']) )
	{
		$_SESSION['remotePort'] = '';
	}
	if ( empty($_SESSION['logId']) )
	{
		$_SESSION['logId'] = '';
	}
	if ( empty($_SESSION['sortBy']) )
	{
		$_SESSION['sortBy'] =  "date_time_begin";
	}
	if ( empty($_SESSION['sortDir']) )
	{
		$_SESSION['sortDir'] = "ASC";
	}

	if (strtolower($_SERVER['REQUEST_METHOD']) == 'post')
	{
		if ( isset($_POST['offset']) )
		{
			$_SESSION['offset'] = $_POST['offset'];
		}
		if ( isset($_POST['limit']) )
		{
			$_SESSION['limit'] = $_POST['limit'];
		}
		if ( isset($_POST['user']) )
		{
			$_SESSION['user'] = $_POST['user'];
		}
		if ( isset($_POST['opType']) )
		{
			$_SESSION['opType'] = $_POST['opType'];
		}
		if ( isset($_POST['remotePid']) )
		{
			$_SESSION['remotePid'] = $_POST['remotePid'];
		}
		if ( isset($_POST['remotePort']) )
		{
			$_SESSION['remotePort'] = $_POST['remotePort'];
		}
		if ( isset($_POST['logId']) )
		{
			$_SESSION['logId'] = $_POST['logId'];
		}
		if ( isset($_POST['columns']) )
		{
			$_SESSION['columns'] = $_POST['columns'];
		}
	}

	if ( isset($_SESSION['columns']) )
	{
		$menu = $_SESSION['columns'];
	}

	$offset 		= $_SESSION['offset'];
	$limit  		= $_SESSION['limit'];
	$user 			= $_SESSION['user'];
	$opType 		= $_SESSION['opType'];
	$remotePid 	= $_SESSION['remotePid'];
	$remotePort = $_SESSION['remotePort'];
	$logId 			= $_SESSION['logId'];
		

	if ( empty( $_POST['appState'] ) || $_POST['appState'] != $_SESSION['appState'] )
	{
		if ( isset($_POST['appState']) )
		{
			$_SESSION['appState'] = $_POST['appState'];
		}

		switch ( $_SESSION['appState'] )
		{

				case 1:
        	      $maxID = $dbHandle->select("(select log_owner, min(id) as id from log group by log_owner order by min(id))A","count(*)");
              	$_SESSION['maxOffset'] = $maxID[0]['count']-1;

              	break;
      	case 2:
        	      $maxID = $dbHandle->select("(select operation_type, remote_pid, remote_port, min(id) as id from log where log_owner = '".$user."' group by remote_pid, operation_type, remote_port order by min(id))Actions","count(*)");
               $_SESSION['maxOffset'] = $maxID[0]['count']-1;

              	break;
      	case 3:
        	      $maxID = $dbHandle->select("log","count(*)", "log_owner = '".$user."' AND operation_type = '".$opType."' AND remote_pid = '".$remotePid."' AND remote_port = '".$remotePort."'");
               $_SESSION['maxOffset'] = $maxID[0]['count']-1;
    }
	}

	$maxOffset = $_SESSION['maxOffset'];
	$appState  = $_SESSION['appState'];

?>