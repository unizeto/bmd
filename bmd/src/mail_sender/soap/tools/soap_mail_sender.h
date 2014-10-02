//gsoap ns service location: http://127.0.0.1:9999
//gsoap ns service namespace: ns.xsd


/********************************/
/*       Typy podstawowe        */
/********************************/

typedef int    xsd__int;
typedef char*  xsd__string;



/********************************/
/*       Bufor generyczny       */
/********************************/

struct xsd__base64Binary {
	unsigned char *__ptr;		/* bufor */
	int __size;			/* liczba elementów w buforze */
};


/********************************/
/*   struktura konfiguracyjna 	*/
/********************************/
struct ns__awizoConfig {
	char*	        smtpAddr;	/* adres serwera pocztowego */
	char*	        smtpPort;	/* port serwera pocztowego */
	char*           smtpUser;	/* nazwa użytkownika */
	char*           smtpPswd;	/* haslo */
	char*       awizoSuccess;	/* nazwa katalogu na awiza, których wysyłka zakończyła się
						 sukcesem w formacie eml */
	char*        awizoFailed;	/* nazwa katalogu na awiza, których wysyłka zakończyła się 
						niepowodzeniem w formacie eml */
	char*   allowImageAbsent;	/* czy zezwolić na wysyłkę awiza bez załącznika */
	char*		  csvDir;       /* katalog na raport w formacie csv */
	char*       csvSeparator;	/* separator pliku csv */
	char*		 pfxfile;	/* identyfikator loba z certyfikatem klucza publicznego
						do podpisywania wiadomości */
	char*		  pfxPIN;	/* PIN */
	char*	    maxImageSize;	/* maksymalny rozmiar pliku załącznika */
	char*	     awizoAction;	/* 1-tylko wysyłka, 2-zapis do eml, 4-zapis do html */
	char*          mailqueue;       /* czy kolejkować e-Awiza (1) - tak, (0) - nie, wysyłka od razu */
	char*       maxcachesize;	/* maksymalna wielkość pamięci cache (ilość slotów) */
	char*       maxmailqueue;	/* maksymalna ilość zakolejkowanych maili */ 
	char*	     sendingtime;	/* czas po upływie którego należy uruchomić proces wysyłki eAwiz */
};



/************************************************/
/*	treść komunikatu dla mail sendera	*/
/************************************************/
struct ns__awizoMessage {
    struct xsd__base64Binary *value;	/* Komunikat w base64 */
};


struct ns_dateTime{
    xsd__string dateTime;
};


struct ns__dateTimeComplexList {
	struct ns_dateTime *__ptr;
	int __size;
};




/******************************************/
/*    informacje o nadawcy wiadomosci     */
/******************************************/
struct ns__senderComplex {
	int id;
	xsd__string name;		/* Nazwa wystawcy */
	xsd__string email;		/* Adres email wystawcy */
	xsd__string created;		/* Data utworzenia wpisu dotyczącego wystawcy */
	xsd__string deleted;		/* Data usunięcia wpisu dotyczącego wystawcy */
	xsd__string smtpaddr;		/* Adres serwera SMTP */
	xsd__string smtpport;		/* Port serwera SMTP */
	xsd__string smtpuser;		/* Nazwa użytkownika SMTP */
	xsd__string smtppassword;	/* Hasło użytkownika SMTP */
	xsd__string pfxfile;		/* Certyfikat podpisujący */
	xsd__string pfxpin;		/* PIN */
	xsd__string maximagesize;	/* Maksymalny dopuszczalny rozmiar załącznika, przewidziany dla wystawcy */
	xsd__string csvdir;		/* obecnie nie używane */
	xsd__string csvseparator;	/* separator dokumentu csv */
	int awizoaction;		/* akcja dla awiza */
	int allowimageabsent;		/* czy dopuszczamy możliwość wysyłki awiza bez załącznika */
	xsd__string smsapiurl;		/* adres i ustawienia bramki sms */
	xsd__string smsapierror;	/* komunikat błędu bramki sms */
};

/******************************************/
/*    informacje o szablonie wiadomosci   */
/******************************************/
struct ns__templateComplex {
	int id;
	xsd__string     created;	/* Data utworzenia wpisu */
	xsd__string     deleted;	/* Data usunięcia wpis */
	int     messagesenderid;	/* Szablon przypisany do wystawcy */
	xsd__string        name;	/* Nazwa szablonu */
	xsd__string        lang;	/* Jęyk - nie wykorzystywane obecnie */
	xsd__string     version;	/* Wersja szablonu */
	xsd__string description;	/* Opis szablonu */
	xsd__string   xsltohtml;	/* szablon XSL (text) */
	xsd__string   xsltotext;	/* szablon XSL (text) */
	int 		   sign;	/* czy podpisać wiadomość */
};


/**********************************/
/*    informacje o załączniku     */
/**********************************/


struct ns__attachmentComplex {
	int 			id;
	xsd__string	   created;
	xsd__string	   deleted;
	int		templateid;
	xsd__string	  filename;
	xsd__string	  mimetype;
	xsd__string     disposition;
	xsd__string	      text;
};

/******************************************/
/*	lista nadawców wiadomości	  */
/******************************************/
struct ns__senderComplexList
{
	struct ns__senderComplex* __ptr;
	int __size;
};



/*********************************************/
/*  żądanie pobrania listy nadawców eawiza   */
/*********************************************/

int ns__awizoGetSendersList(struct ns__senderComplexList **senderList);


/************************************************/
/*  żądanie wstawiania szablonu do bazy eAwiza  */
/************************************************/

int ns__awizoInsertTemplate(struct ns__templateComplex *emailTemplate, long* result);

/************************************************/
/*  żądanie usunięcia szablonu z bazy eAwiza    */
/************************************************/

int ns__awizoDeleteTemplate(long id, long* result);


/*********************************************/
/*  żądanie pobrania szablonu z bazy eAwiza  */
/*********************************************/

int ns__awizoDownloadTemplate(long id, struct ns__templateComplex **emailTemplate);

/****************************************************/
/*  żądanie zmiany informacji dotyczących szablonu  */
/****************************************************/

int ns__awizoChangeTemplateInfo(long id, struct ns__templateComplex *emailTemplate, long* result);


/**********************************************************/
/*  żądanie dodania pliku załącznika do danego szablonu   */
/**********************************************************/

int ns__awizoInsertAttachment(struct ns__attachmentComplex *emailAttachment, long* result);


/***************************************************/
/*  żądanie usunięcia załącznika z bazy e-Awiza    */
/***************************************************/

int ns__awizoDeleteAttachment(long id, long* result);


/**************************************/
/* pobranie załącznika z bazy eAwiza  */
/**************************************/

int ns__awizoDownloadAttachment(long id, struct ns__attachmentComplex **emailAttachment);


/******************************************************/
/*  żądanie zmiany informacji dotyczącej załącznika   */
/******************************************************/

int ns__awizoChangeAttachmentInfo(long id, struct ns__attachmentComplex *emailAttachment, long* result);


/****************************************************************/
/*  żądanie wstawienia nadawcy wiadomości email do bazy eAwiza  */
/****************************************************************/

int ns__awizoInsertSender(struct ns__senderComplex* sender, long* result);


/************************************************/
/*  żądanie usunięcia nadawcy z bazy eAwiza     */
/************************************************/

int ns__awizoDeleteSender(long id, long* result);


/*********************************************************/
/*  żądanie pobrania informacji o nadawcy z bazy eAwiza  */
/*********************************************************/

int ns__awizoGetSenderInfo(long id, struct ns__senderComplex** sender);


/****************************************************/
/*  żądanie zmiany informacji dotyczących nadawcy   */
/****************************************************/

int ns__awizoChangeSenderInfo(long id, struct ns__senderComplex* sender, long* result);


/**********************************************************/
/*  żądanie wysłania komunikatu wysyłki eawiza do serwera */
/**********************************************************/
int ns__awizoSend(struct ns__awizoMessage *message, char** msgID);


/********************************************/
/*  żądanie wysłania komunikatów z kolejki  */
/********************************************/
int ns__awizoSetFromMsgQueueSend(long* result);


/************************************************/
/*  żądanie usuwania szablonów z pamięci cache  */
/************************************************/
int ns__clearCache(long* result);


/********************************************************/
/*  żądanie usuwania komunikatów z kolejki komunikatów  */
/********************************************************/
int ns__clearMailQueue(long* result);


/***************************************************/
/*  żądanie pobrania liczby komunikatów w kolejce  */
/***************************************************/
int ns__countMailFromQueue(long* result);


/*********************************************************************/
/*  żądanie wstrzymania/wznowienia procesu wysyłki wiadomości email  */
/*********************************************************************/
int ns__stopSendingProcess(long awizo_switch, long* result);


/**********************************/
/*  żądanie zmiany konfiguracji   */
/**********************************/
int ns__changeConfiguration(struct ns__awizoConfig *conf, long* result);


/**********************************************************/
/*  żądanie wysłania komunikatu wysyłki sms               */
/**********************************************************/
int ns__awizoSendSMS(char *message, char *to, long* result);


/*********************************************/
/*  żądanie dodania komunikatu do bufora     */
/*********************************************/
int ns__addAdviceDispatchSetting(struct ns__awizoMessage* message, struct ns__dateTimeComplexList *dateTime, long* result);


/*********************************************/
/*  żądanie zmiany parametrów powiadomień    */
/*********************************************/
int ns__changeAdviceDispatchSetting(long dispatchSettingId, struct ns__awizoMessage* message, struct ns__dateTimeComplexList *dateTime, long* result);


/*************************************/
/*  żądanie usunięcia powiadomień    */
/*************************************/
int ns__removeAdviceDispatchSetting(long dispatchSettingId, long* result);


/**********************************************************/
/*  żądanie wysłania komunikatu wysyłki eawiza do serwera */
/**********************************************************/
/* int ns__connectWithAwizoServer(struct ns__awizoMessage *message, char** msgID); */
