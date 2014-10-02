#ifndef _LIBBMDMAIL_INCLUDED_
#define _LIBBMDMAIL_INCLUDED_

#include <bmd/common/bmd-include.h>
#include <bmd/common/bmd-const.h>
#include <bmd/libbmdbase64/libbmdbase64.h>

#ifndef WIN32
	#include <auth-client.h>
	#include <libesmtp.h>
#endif

#define BMD_MAIL_WITH_AUTH	1
#define BMD_MAIL_WITHOUT_AUTH	0

#define BMDMAIL_ADVICE_SENDING_STATUS_UNDEFINED		'3'
#define BMDMAIL_ADVICE_SENDING_STATUS_FAILED		'0'
#define BMDMAIL_ADVICE_SENDING_STATUS_SUCCESS		'1'


#if defined(__cplusplus)
extern "C" {
#endif

#if 0 /*OBSOLETE*/
typedef struct msg_header_info {
    char *email_to;
    char *email_from;
    char *subject;
    char *msg_id;
    char *date;
} msg_header_info;
#endif /* if 0 </OBSOLETE>*/

#if 0 /* <OBSOLETE> */
typedef struct msg_body_info {
    char *msg_content;
} msg_body_info;
#endif /* if 0 </OBSOLETE> */

typedef struct state {
    long m_state;
    long length;
} state;

typedef struct auth_data_struct {
    char *username;
    char *password;
} auth_data_struct_t;

typedef struct mail_send_info
{
	char *recipient_to;
	char *recipient_cc;
	char *recipient_bcc;
	char *reverse_path;
	auth_data_struct_t auth_data;
	long with_auth;
	long mail_to_send;
	long mail_status;
	char *mail_body;

	char *server_port;
/* 	char *msg_id;*/
    /* Sesja SMTP */
#ifndef WIN32 /*NPORTED dotyczy libbmdmail i smtp*/
	smtp_session_t session;
#endif

} mail_send_info_t;

#ifndef WIN32 /*kod potrzebny tylko do wspolpracy z libesmtp (Linux)*/
	int authinteract (auth_client_request_t request, char ** result, int fields, void * arg);
	const char *libesmtp_callback(void **ctx, int *len, void *arg);
#endif /*ifndef WIN32*/

long bmd_send_mail(mail_send_info_t *mi);

long bmd_send_envoice_avizo(mail_send_info_t *mi, long count, char **result);

long bmd_get_recipients(	char *recipients,
					char ***recipient_list,
					long *recipients_count);


/**funkcja alokuje i ustawia zawartość struktury mail_send_info_t*/
long set_mail_send_info(mail_send_info_t **mi, char *_server_port, char *_recipient_to, char *_recipient_cc, char *_recipient_bcc, char *_mail_body, long _with_auth, char *_user, char* _password, char *_reverse_path);
long free_mail_send_info(mail_send_info_t **mi);

long get_einvoice_recipient(char *email,char **out_recipient);
long getSMTPResponseStatusCode(char *input, long *statusCode);

#if 0 /* <OBSOLETE> */
long set_header_info(char *email_to, char *email_from,char *subject, char *msg_id,char *date,msg_header_info ** output);
long prepare_headers(msg_header_info * header,char **headers_content);
void free_headers(msg_header_info ** header);
#endif /* if 0 </OBSOLETE> */

#if 0 /* <OBSOLETE> */
long set_body_info(char *msg_content,msg_body_info ** output);
long prepare_body(msg_body_info * body,char **body_content);
void free_body(msg_body_info ** body);
#endif /* if 0 </OBSOLETE> */

#if 0 /* <OBSOLETE> */
/** API służące do wysyłki wiele wiele maili */
long bmdmail_add_message(mail_send_info_t *mi, const char * rcpt_to, const char * body);
long bmdmail_send(mail_send_info_t *mi);
#endif /* if 0 </OBSOLETE> */

#if 0 /*kod nie jest wykorzystywany*/
long print_mail_send_info(mail_send_info_t *mi);
#endif /*if 0*/


#if defined(__cplusplus)
}
#endif

#endif /*ifndef _LIBBMDMAIL_INCLUDED_*/
