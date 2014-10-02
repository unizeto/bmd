#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


/**----wlasna implementacja funkcji getch pod Linux (zaczerpnieta z 4programmers.net)---------**/
#include <termios.h>

int bmd_getch(void)
{
        int key;
        struct termios oldSettings, newSettings;    /* stuktury z ustawieniami terminala */

        tcgetattr(STDIN_FILENO, &oldSettings);    /* pobranie ustawień terminala */
        newSettings = oldSettings;
        newSettings.c_lflag &= ~(ECHO);    /* ustawienie odpowiednich flag */
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);    /* zastosowanie ustawień */
        key = getchar();    /* pobranie znaku ze standardowego wejścia */
        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);    /* przywrócenie poprzednich ustawień terminala */
        return key;
}
/**-------------*/


//verify_logs -h(HOST_ADDRESS) -p(HOST_PORT) -d(DB_NAME) -u(DB_USER) -l(LogID)
/*
   TK musiałem dodać hasło do bazy danych -m(magic)
   verify_logs -h(HOST_ADDRESS) -p(HOST_PORT) -d(DB_NAME) -u(DB_USER) -m(BD_PASSWD) -l(LogID)
*/
//int parse_params_line(char **argv_input, int argc_input, char **host, char **port, char **dbname, char **user, char **logId)
long parse_params_line(char **argv_input, long argc_input, char **host, char **port, char **dbname, char **dblibrary, char **user, char **pswd,char **logId)
{
	int iter=0;
	char *tmpHost=NULL;
	char *tmpPort=NULL;
	char *tmpDbname=NULL;
	char *tmpDblibrary=NULL;
	char *tmpUser=NULL;
	char *tmpLogId=NULL;
	char *tmpPswd=NULL;
	int correct=0;
	int strLen=0;

	if(argv_input==NULL)
		{ return -11; }
	if(argc_input < 1)
		{ return -12; }
	if(host == NULL)
		{ return -13; }
	if(*host != NULL)
		{ return -14; }
	if(port == NULL)
		{ return -15; }
	if(*port != NULL )
		{ return -16; }
	if(dbname == NULL)
		{ return -17; }
	if(*dbname != NULL)
		{ return -18; }
	if(user == NULL)
		{ return -19; }
	if(*user != NULL)
		{ return -20; }
	if(logId == NULL)
		{ return -21; }
	if(*logId != NULL)
		{ return -22; }

	/* Na potrzeby edytora logów */

	if(pswd == NULL)
		{ return -23; }
	if(*pswd != NULL)
		{ return -24; }
	if(dblibrary == NULL)
		{ return -25; }
	if(*dblibrary != NULL)
		{ return -26; }
		
	/* ------------------------- */

	for(iter=1; iter<argc_input; iter++)
	{
		if( memcmp(argv_input[iter], "-h", 2*sizeof(char)) == 0)
		{
			if(tmpHost != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -h\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpHost=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpHost, argv_input[iter]+2, strLen-2);
				continue;
			}
		}
		if( memcmp(argv_input[iter], "-p", 2*sizeof(char)) == 0)
		{
			if(tmpPort != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -p\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpPort=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpPort, argv_input[iter]+2, strLen-2);
				continue;
			}
		}
		if( memcmp(argv_input[iter], "-d", 2*sizeof(char)) == 0)
		{
			if(tmpDbname != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -d\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpDbname=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpDbname, argv_input[iter]+2, strLen-2);
				continue;
			}
		}
		if( memcmp(argv_input[iter], "-r", 2*sizeof(char)) == 0)
		{
			if(tmpDblibrary != NULL)
			{
			    printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -r\n");
                            correct=-1;
                            break;
                        }
            		else
			{
				strLen=strlen(argv_input[iter]);
				tmpDblibrary=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpDblibrary, argv_input[iter]+2, strLen-2);
		    
				if (strcmp(tmpDblibrary,"postgres")==0)
				{
				    free(tmpDblibrary); tmpDblibrary=NULL;
				    asprintf(&tmpDblibrary, "libbmddb_postgres.so");
				}
				else if (strcmp(tmpDblibrary,"oracle")==0)
				{
				    free(tmpDblibrary); tmpDblibrary=NULL;
				    asprintf(&tmpDblibrary, "libbmddb_oracle.so");
				}
				else if (strcmp(tmpDblibrary,"db2")==0)
				{
				    free(tmpDblibrary); tmpDblibrary=NULL;
				    asprintf(&tmpDblibrary, "libbmddb_db2.so");
				}

				continue;
			}
		}			
		if ( memcmp(argv_input[iter], "-D", 2*sizeof(char)) == 0)
		{
		    _GLOBAL_debug_level = atoi( argv_input[iter]+2 );
		    continue;
		}
		if( memcmp(argv_input[iter], "-u", 2*sizeof(char)) == 0)
		{
			if(tmpUser != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -u\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpUser=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpUser, argv_input[iter]+2, strLen-2);
				continue;
			}
		}

		/* Wprowadzone na potrzeby edytora logów */

		if( memcmp(argv_input[iter], "-m", 2*sizeof(char)) == 0)
		{
			if(tmpPswd != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -m\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpPswd=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpPswd, argv_input[iter]+2, strLen-2);
				continue;
			}
		}
		/* ------------------------------------- */

		if( memcmp(argv_input[iter], "-s", 2*sizeof(char)) == 0)
		{
			if(tmpLogId != NULL)
			{
				printf("NIEPOPRAWNE WYWOLANIE: ZDUBLOWANA OPCJA -l\n");
				correct=-1;
				break;
			}
			else
			{
				strLen=strlen(argv_input[iter]);
				tmpLogId=(char*)calloc( strLen-1 , sizeof(char));
				memcpy(tmpLogId, argv_input[iter]+2, strLen-2);
				continue;
			}
		}
		else
		{
			printf("NIEPOPRAWNE WYWOLANIE: NIEROZPOZNANA OPCJA \"%s\"\n", argv_input[iter]);
			correct=-1;
		}
	}

	if(tmpHost == NULL)
	{
		printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -h\n");
		correct=-1;
	}
	else
	{
		if(strlen(tmpHost) == 0)
		{
			printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -h\n");
			correct=-1;
		}
	}
	if(tmpPort == NULL)
	{
		printf("BRAK OPCJI -p -> USTAWIONO DOMYSLNY PORT 5432\n");
		tmpPort=strdup("5432");
		//ok
	}
	else
	{
		if(strlen(tmpPort) == 0)
		{
			printf("BRAK OPCJI -p -> USTAWIONO DOMYSLNY PORT 5432\n");
			tmpPort=strdup("5432");
		}
	}
	if(tmpDbname == NULL)
	{
		printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -d\n");
		correct=-1;
	}
	else
	{
		if(strlen(tmpDbname) == 0)
		{
			printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -d\n");
			correct=-1;
		}
	}
	if(tmpDblibrary == NULL)
        {
                printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -r\n");
                correct=-1;                                                                                                                                  
        }
	else
        {
            if(strlen(tmpDblibrary) == 0)
	    {
                printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -r\n");
        	correct=-1;
	    }
        }
	
	if(tmpUser == NULL)
	{
		printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -u\n");
		correct=-1;
	}
	else
	{
		if(strlen(tmpUser) == 0)
		{
			printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -u\n");
			correct=-1;
		}
	}

	/* Wprowadzone na potrzeby edytora logów */

	if(tmpPswd == NULL)
	{
		printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -m\n");
		correct=-1;
	}
	else
	{
		if(strlen(tmpPswd) == 0)
		{
			printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -m\n");
			correct=-1;
		}
	}

	/* ------------------------------------- */

	if(tmpLogId == NULL)
	{
		printf("NIEPOPRAWNE WYWOLANIE: BRAK OPCJI -s\n");
		correct=-1;
	}
	else
	{
		if(strlen(tmpLogId) == 0)
		{
			printf("NIEPOPRAWNE WYWOLANIE: PUSTA OPCJA -s\n");
			correct=-1;
		}
	}


	if(correct == -1)
	{
		free(tmpHost);
		free(tmpPort);
		free(tmpDbname);
		free(tmpUser);
		free(tmpLogId);
		free(tmpDblibrary);
		return -1;
	}
	else
	{
		*host=tmpHost;
		*port=tmpPort;
		*dbname=tmpDbname;
		*dblibrary=tmpDblibrary;
		*user=tmpUser;
		*pswd=tmpPswd;
		*logId=tmpLogId;
		return 0;
	}

}

int get_password(char** output_password)
{
char symbol;
unsigned int symbolCount=0;
char *password=NULL;
char *tmpPass=NULL;


	if(output_password == NULL)
		{ return -11; }
	if(*output_password != NULL)
		{ return -12; }

	printf("\nPROSZE PODAC HASLO DO WSKAZANEJ BAZY:> ");
	while((symbol=bmd_getch()) != '\n')
	{
		if(password == NULL)
		{
			password=(char*)calloc(2, sizeof(char)); //calloc jednoczesnie zeruje mi komorki, wiec NULL terminated string
			password[0]=symbol;
			symbolCount++;
		}
		else
		{
			tmpPass=(char*)calloc(symbolCount+2, sizeof(char));
			memcpy(tmpPass, password, symbolCount);
			free(password);
			tmpPass[symbolCount]=symbol;
			password=tmpPass;
			tmpPass=NULL;
			symbolCount++;
		}
	}

	if(password == NULL)
		{ password=strdup(""); }
	*output_password=password;
	return 0;
}

int main(int argc ,char **argv)
{
int retVal=0;
long int longLogId=0;

char *password=NULL;
char *host=NULL;
char *port=NULL;
char *dbname=NULL;
char *dblibrary=NULL;
char *user=NULL;
char *logId=NULL;
void *connectionHandler=NULL;

_GLOBAL_debug_level = 20;


	printf("\n===============================================================================\n");
	printf("AUDYTOR STRUKTURY DRZEWA MERKLA\n");
	printf("===============================================================================\n\n");

	retVal=parse_params_line(argv, argc, &host, &port, &dbname, &dblibrary, &user, &password, &logId);
	if(retVal != 0)
	{
		//printf("\nSPOSOB WYWOLANIA:\nverify_logs -h(HOST_ADDRESS) -p(HOST_PORT) -d(DB_NAME) -u(DB_USER) -l(LOG_ID)\n\n");

		/* Wprowadzono na potrzeby edytora logów */

		printf("\nSPOSOB WYWOLANIA:\naudit_tree -h(HOST_ADDRESS) -p(HOST_PORT) -d(DB_NAME) -r(DB_LIBRARY[postgres|oracle|db2]) -u(DB_USER) -m(DB_PASSWD) -s(SIGN_ID)\n\n");

		/* ------------------------------------- */

		return -1;
	}

	printf("\n\n===============================================================================\n");
	printf("DANE PODANE PRZEZ UZYTKOWNIKA:\n");
	printf("Host:\t\t%s\n", host);
	printf("Port:\t\t%s\n", port);
	printf("User:\t\t%s\n", user);
	printf("Database:\t%s\n", dbname);
	printf("DbLibrary:\t%s\n", dblibrary);
	printf("Sign ID:\t\t%s\n", logId);
	printf("===============================================================================\n\n");


	//get_password(&password);
	printf("bmd_db_init_audit_tree_main\n");
	retVal = bmd_db_init(dblibrary);
	if ( retVal != BMD_OK )
	{
	    printf("Błąd inicjowania biblioteki obsługi bazy danych");
	    return -2;
	}                  

	printf("bmd_db_connect2_audit_Tree_main\n");
	retVal=bmd_db_connect2(host, port, dbname, user, password, &connectionHandler);
	longLogId=atol(logId);
	free(logId);
	memset(password, 0, strlen(password));
	free(password); password=NULL;
	free(host);
	free(port);
	free(user);
	free(dbname);
	free(dblibrary);
	if(retVal != 0)
	{
		printf("\nBLAD: NIE MOZNA NAWIAZAC POLACZENIA Z BAZA\n\n");
 		return -3;
	}

	retVal = bmdLogCheckSubtree( connectionHandler, longLogId, (depth_t)subtree );


	printf("\n===============================================================================\n\n");


	bmd_db_disconnect(&connectionHandler);

	bmd_db_end();

	return retVal;
}
