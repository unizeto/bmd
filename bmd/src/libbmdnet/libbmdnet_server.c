/* plik wykorzystywany obecnie tylko pod linux  */
#ifndef WIN32
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <bmd/common/global_types.h>

void kill_me(int status)
{
	PRINT_INFO("BMDSERVERINF Finishing timestamp conservation process pid=%i\n",getpid());
	exit(0);
}

void child_died(int id)
{
	int pid=0;
	PRINT_TEST("BMDSERVERINF One of the subprocesses (grandson) died\n");
	while ((pid = waitpid (-1, &id, WNOHANG)) > 0);
}

long __bmdnet_plaintcp_bind (	bmdnet_handler_ptr handler,
					char * net_interface,
					long port,
					long maxconn,
					char * serverCertFilename)
{
#ifndef _WIN32
	struct hostent hostinfo_buf;
	struct hostent *hostinfo;
	struct sockaddr_in serv_addr;
	char aux[256];
	int errno_r; /* to musi byc int */
	PRINT_INFO("TCP network method: bind\n");

	memset(&serv_addr, 0, sizeof(serv_addr));
	if (gethostbyname_r( net_interface, &hostinfo_buf, aux, sizeof(aux), &hostinfo, &errno_r)!=0)
	{
		PRINT_ERROR("Cannot fill hostent structure with gethostbyname.\n");
		perror("__bmd_plaintcp_bind -> gethostbyname_r: ");
		return(-1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr=*(struct in_addr *)hostinfo->h_addr;

	serv_addr.sin_port=htons(port);
	if (bind(handler->s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
	{
		PRINT_ERROR("Error binding to socket.\n");
	/*	perror("__bmdnet_plaintcp_bind -> bind: "); */
                Q_FATAL("Address already in use\n");
		kill(getpid(),2);
		return -1;
	}
	if (listen( handler->s, SOMAXCONN-1 ))
	{
		PRINT_ERROR("Error listening on socket.\n");
		perror("__bmdnet_plaintcp_bind -> listen: ");
		return -1;
	}

	return 0;
#else
	return -1;
#endif
}

bmdnet_handler_ptr __bmdnet_plaintcp_accept (bmdnet_handler_ptr handler)
{
#ifndef _WIN32
	long sd,err;
	struct sockaddr_in cli_addr;
	bmdnet_handler_ptr hdl = NULL;
	socklen_t cli_size=sizeof(cli_addr);

	PRINT_INFO("TCP network method: accept\n");
	if ( handler == NULL )
	{
		return NULL;
	}

	err=bmdnet_create(&hdl);
	if (err)
	{
	    return NULL;
	}

	if (hdl==NULL) return NULL;

	while (1)
	{
		sd=accept(handler->s, &cli_addr, &cli_size);
		if (sd==-1)
		{
			if (EINTR==errno)
			{
				continue; /* Restart accept */
			}
			else
			{
				PRINT_ERROR("Error accepting connection.\n");
				perror("__bmdnet_plaintcp_accept -> accept");

				//free(hdl->data[0]);  free(hdl->data);
				free(hdl);

				return(NULL);
			}
		}
		break;
	}

	hdl->s = sd;

	asprintf( &(hdl->stringClientHost), "%s", inet_ntoa(cli_addr.sin_addr));
	asprintf( &(hdl->stringClientPort), "%d", ntohs(cli_addr.sin_port));

	PRINT_INFO("New connection accepted\n");

	return(hdl);
#else
	return NULL;
#endif
}

long bmdnet_server(bmdnet_handler_ptr handler,char * net_interface,long port,long maxconn,
				   char * serverCertFilename,void* (*server_func)(bmdnet_handler_ptr handler))
{
	long status=0;
	pid_t pid;
	int optval = 1;
	int errorCode = 0;
	
	handler->s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( handler->s == -1 )
	{
		PRINT_ERROR("Problem creating socket in bmdnet_server.\n");
	}
	
	if (setsockopt(handler->s,SOL_SOCKET,SO_REUSEADDR,&optval, sizeof(optval)) != 0)
	{
		PRINT_ERROR("Problem setting SO_REUSEADDR socket option in bmdnet_server.\n");
	}
	

	status = __bmdnet_plaintcp_bind(handler,net_interface, port, maxconn, serverCertFilename);
	if( status)
	{
		PRINT_ERROR("Error - socket binding.\n");
		return status;
	}
	
	Q_DONE("BMD server started\n");
	/************************************************************************************/
	/*	skoro sewer ruszyl, mozna uruchomic proces konserwacji wartosci dowodowej	*/
	/************************************************************************************/
	if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_conservate_plugin_do)
	{
		if(_GLOBAL_std_conservate_configuration->enable_conservation == 1)
		{
			_GLOBAL_children_number++;
			/* czy to jest ok - zmiana wymagana przez unikniecie server_request_data_t jako global */
			status=_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_conservate_plugin_do(NULL);
			if (status<BMD_OK)
			{
				V_FATAL("Error in conservate plugin work. Error=%li\n",status);
				_GLOBAL_children_number--;
			}
		}
		else
		{
			V_DONE("Proofs conservation functionality is disabled.\n")
		}
	}
	/******************************************************************************/
	/*	skoro sewer ruszyl, mozna uruchomic proces czyszczenia bazy danych	*/
	/******************************************************************************/
	if(_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_clear_plugin_do)
	{
		_GLOBAL_children_number++;
		status=_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_clear_plugin_do(_GLOBAL_bmd_configuration);
		if (status<BMD_OK)
		{
			V_FATAL("Error in clear plugin work. Error=%li\n",status);
			_GLOBAL_children_number--;
		}

		_GLOBAL_children_number++;
		status=_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_run_expired_links_remover();
		if (status<BMD_OK)
		{
			V_FATAL("Error in expired links remover work. Error=%li\n",status);
			_GLOBAL_children_number--;
		}
	}

	/*  uruchomienie procesu archiwizacji dokumentow */
	if(_GLOBAL_bmd_configuration->archiving_plugin_conf.bmd_archiving_plugin_start)
	{
		_GLOBAL_children_number++;
		status=_GLOBAL_bmd_configuration->archiving_plugin_conf.bmd_archiving_plugin_start();
		if(status<BMD_OK)
		{
			V_FATAL("Error in archiving plugin work. Error=%li\n",status);
			_GLOBAL_children_number--;
		}
	}
	else
	{
		V_DENY("Archiving plugin unavailable\n");
	}

	
	if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_run_asynchronous_timestamping)
	{
		// inkrementacji ilosci potomkow wewnatrz funkcji
		status=_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_run_asynchronous_timestamping();
		if(status<BMD_OK)
		{
			V_FATAL("Error in asynchronous timestamping work. Error=%li\n",status);
		}
	}
	else
	{
		V_DENY("Asynchronous timestamping functionality unavailable\n");
	}


	if(_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_run_lobs_remover)
	{
		status=_GLOBAL_bmd_configuration->clear_plugin_conf.bmd_run_lobs_remover();
		if(status<BMD_OK)
		{
			V_FATAL("Error in lobs remover work. Error=%li\n",status);
		}
	}
	else
	{
		V_DENY("Lobs remover functionality unavailable\n");
	}

	while (1)
	{
		bmdnet_handler_ptr newconn = NULL;
		newconn = __bmdnet_plaintcp_accept((bmdnet_handler_ptr )handler);
		if (newconn == NULL)
		{
			PRINT_ERROR("Error - accepting connection.\n");
			continue;
		}

		if (server_func)
		{
			pid=fork();
			_GLOBAL_child=pid;
			if( pid == 0 )
			{
				signal(SIGUSR,kill_me);signal(SIGCHLD,child_died);
				signal(SIGINT,SIG_IGN);signal(SIGTERM,SIG_DFL);signal(SIGHUP,SIG_IGN);
				server_func(newconn);
				exit(0);
			}
			else
			{
				if( pid > 0 )
				{
					signal(SIGUSR,SIG_IGN);
					_GLOBAL_children_number++;
				}
				else
				{
					errorCode = errno;
					PRINT_ERROR("Unable to create new process (%i)\n", pid);
					PRINT_ERROR("Error code = %i\n", errorCode);
					PRINT_ERROR("Error message: <%s>\n", strerror(errorCode));
					bmdnet_close(&newconn);
					continue;
				}
			}
		}
		else
		{
			PRINT_ERROR("LIBBMDNETERR Server function not defined...\n");
			V_FATAL("LIBBMDNETERR Server function not defined...\n");
		}
		//if (status != 0)
		//{
		//	PRINT_ERROR("Error status=%li\n",status);
		//	return status;
		//}
		bmdnet_close(&newconn);
	}
	
	return -1;
}

long bmdnet_logserver(bmdnet_handler_ptr handler,char * net_interface,long port,long maxconn,
				   char * serverCertFilename,void* (*server_func)(bmdnet_handler_ptr handler))
{
	long status=0;
	pid_t pid;

	handler->s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( handler->s == -1 )
	{
		PRINT_ERROR("Problem creating socket in bmdnet_server.\n");
	}

	status = __bmdnet_plaintcp_bind(handler,net_interface, port, maxconn, serverCertFilename);
	if (status)
	{
		return status;
	}

	while (1)
	{
		bmdnet_handler_ptr newconn = NULL;
		newconn = __bmdnet_plaintcp_accept((bmdnet_handler_ptr )handler);
		if (newconn == NULL)
			return -1;

		if (server_func)
		{
			pid=fork();
			_GLOBAL_child=pid;
			if( pid == 0 )
			{
				signal(SIGUSR,kill_me);signal(SIGCHLD,child_died);
				signal(SIGINT,SIG_IGN);signal(SIGTERM,SIG_DFL);signal(SIGHUP,SIG_IGN);
				server_func(newconn);
				exit(0);
			}
			else
			{
				if( pid > 0 )
				{
					signal(SIGUSR,SIG_IGN);
					_GLOBAL_children_number++;
				}
				else
					return pid;
				//break;
			}
		}
		else
		{
			V_FATAL("LIBBMDNETERR Server function not defined...\n");
		}
		if (status != 0)
		{
			return status;
		}
		bmdnet_close(&newconn);
	}

	return -1;
}
#else
#pragma warning(disable:4100)
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-errors.h>
long bmdnet_server(bmdnet_handler_ptr handler,char * net_interface,long port,long maxconn,
				   char * serverCertFilename,void* (*server_func)(bmdnet_handler_ptr handler))
{
	return BMD_ERR_UNIMPLEMENTED;
}
#endif
