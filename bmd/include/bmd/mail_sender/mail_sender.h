/*****************************************
 *			                 *
 * Implementacja soap eAwiza             *
 *			                 *
 * Data : 12-10-2009                     *
 * Autor : Tomasz Klimek                 *
 *			                 *
 *****************************************/

#ifndef __MAIL_SENDER_H__
#define __MAIL_SENDER_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlmemory.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/mail_sender/configuration.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdcurl/libbmdcurl.h>
#include <bmd/libbmderr/libbmderr.h>
#define MAX_ATTEMPT 10

#define MAIL_SENDER_EXIT_STATUS_LOST_DB_CONNECTION	2


typedef enum {queueNothing, queueFull, queueTime, queueNextMail, cacheClear, queueClear} queueAction_t;
typedef enum {modeNONE, modeSMS, modeEMAIL, modeSMSEMAIL} awizo_mode_t;

/* -------------------------------------------------------------------------------- */
/* UTILS API */

  int  MessageParse( GenBuf_t* const, xmlDocPtr* const );
  long createMessageId(xmlDocPtr, message_t* const );
  long prepareMessageId(xmlDocPtr, char** const);
  long getXpathValue( xmlDocPtr, xmlChar*, char** );
  long getXpathAttrValue( xmlDocPtr, xmlChar*, xmlChar*, char** );
  long getTemplateId( xmlDocPtr const, void*, char** );
  int  getSubstring(const char* const, const char* const, char*** const);
  long cmpCacheNode( const void* const, const void* const );
  long downloadTemplateID( char* const, void** const, void* );
  long getDataFromDB( char* const, void*, void** const );
  long getMsgSenderReceiver( xmlDocPtr, char** const, char** const, void* );
  void DestroyMsgNode( void* );
  long prepareAwizo(void* const, xmlDocPtr, void*, char* const, mail_send_info_t** const );
  long prepareAndSendAwizoSet( void*, char** );
  long prepareMessageNode(GenBuf_t, xmlDocPtr, message_t**);
  long freeMailSendInfo(mail_send_info_t*, long const);
  long getTime( char** const stringTime );

  long saveRaportToDb(xmlDocPtr, void*, char*, long, long, char*, long);
  long updateRaportInDb( void* dbHandler, char* msgID, awizo_mode_t mode, char* awizo_info, char sending_status);

  long getAttachments(xmlDocPtr, bmd_smime_msg* const);
  long getFileByIdFromBAE(bmdserver_t*, long, char*, char*, GenBuf_t**);
  long mailQueueFunc(void);
  long sendAwizoSetFromDataBase(void);
  long setAdviceStatus(void*, long, long);
  long mailQueueTimerFunc(void);
  long getSenderConfiguration(const char* const);
  awizo_mode_t getAwizoMode( xmlDocPtr message );
  long getSMSSenderReceiver( xmlDocPtr message, char** const receiver);
  long sendSMS(void* const cacheNode, xmlDocPtr message, void* dbHandler, char* const msgID);
  char from_hex(char ch);
  char to_hex(char code);
  char *url_encode(char *str);

/* --------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------- */
/* REAL TIME SIGNAL FUNCTIONS */

  void addTemplate( int, siginfo_t*,  void* );
  void clearCache(int, siginfo_t*, void* );
  void addMailToQueue(int id);
  void clearMailQueue(int, siginfo_t*, void* );

/* --------------------------------------------------------------------------------- */
/* SIGNAL FUNCTIONS */

  void queueTimer( int );
  void sendMailQueue( int );
  void changeServerConfiguration( int );


#endif /* __MAIL_SENDER_H__ */

