var glob;

function ajaxFunction(proc,id,url)
	{
		var xmlHttp;
		
		try
		{
			xmlHttp=new XMLHttpRequest();
  	}
		catch (e)
		{
			try
			{
				xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
			}
			catch (e)
			{
				try
				{
					xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
				}
				catch (e)
				{
      return false;
				}
			}
		}
		xmlHttp.onreadystatechange=function()
  {
			if(xmlHttp.readyState==4)
    {
       glob = xmlHttp.responseText;

       var form = document.createElement("form");
        form.action = "../index.php";
        form.method = "post";
       var inp1 = document.createElement("input");
        inp1.type = "hidden";
        inp1.value = id;

       if ( proc == 'verify' ){
         inp1.name = "logBranch";
       }
       else{
         inp1.name = "hashBranch";
      }

       form.appendChild(inp1);
       document.body.appendChild(form);
       form.submit();
    }
  }
    xmlHttp.open("GET",proc+".php?id="+id,true);
    xmlHttp.send(null);
  }


 function verifyFunction(proc,id,url)
  {
      var form = document.createElement("form");
      form.action = "verify_bar.php";
      form.method = "post";
      form.target="_top";
      var inp1 = document.createElement("input");
      inp1.type = "hidden";
      inp1.name = "proc";
      inp1.value = proc;
      form.appendChild(inp1);
      var inp2 = document.createElement("input");
      inp2.type = "hidden";
      inp2.name = "id";
      inp2.value = id;
      form.appendChild(inp2);
      document.body.appendChild(form);
      form.submit();
  }
