/* ***********************************************
 *                                               *
 * Definicja klas komunikatów wysyłanych do      *
 * serwera logów.				 *
 *						 *
 * Data : 04-03-2008				 *
 *						 *
 *************************************************/

#ifndef _BMD_LOGS_POLICY_H_
#define _BMD_LOGS_POLICY_H_

/* ---------------  EVENTS CLASS  -------------- */

	#define _CONNECT_ERR_			 0
	#define _BINDING_ERR_			 1
	#define _START_SERV_ERR_		 2
	#define _START_PLUGIN_ERR_		 3
	#define _LOGIN_ERR_			 4
	#define _EXECUTE_ERR_			 5
	#define _FUNCTION_ERR_			 6
	#define _DATAGRAM_ERR_			 7
	#define _LOGIN_DENY_			 8
	#define _WRONG_DATA_			 9
	#define _CA_VERIFY_ERR_			10
	#define _LOGIN_OK_			11
	#define _EXECUTE_OK_			12
	#define _EXECUTE_PLUGIN_OK_		13
	#define _START_SERV_OK_			14
	#define _DATAGRAM_OK_			15
	#define _RUNNING_SERVER_		16
	#define _RUNNING_PLUGIN_		17
	#define _ATTEMPT_TO_EXECUTE_		18
	#define _ATTEMPT_TO_LOGIN_		19
	#define _ATTEMPT_TO_DATAGRAM_		20
	#define _FUNCTION_DEBUG_		21
	#define _DBASE_REQUEST_			22

/* ------------   MESSAGE CLASS ---------------  */

	#define _Q_FATAL_			 0
	#define _V_FATAL_			 1
	#define _Q_DENY_			 2
	#define _V_DENY_			 3
	#define _Q_DONE_			 4
	#define _V_DONE_			 5
	#define _Q_TRY_				 6
	#define _V_TRY_				 7
	#define _Q_DEBUG_			 8
	#define _V_DEBUG_			 9

/* --------------------------------------------- */


#endif

