#ifndef _LIBBMDMIME_INCLUDED_
#define _LIBBMDMIME_INCLUDED_

#include <bmd/common/bmd-const.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmdmail/libbmdmail.h>

#ifdef WIN32
#define strdup _strdup
#endif

#define BMD_MAIL_TRANSFER_ENCODING_BASE64   1
#define BMD_MAIL_TRANSFER_ENCODING_7BIT	    2
#define BMD_MAIL_TRANSFER_ENCODING_QUOTED   3

#define BMD_MAIL_CONTENT_TYPE_HTML	    1
#define BMD_MAIL_CONTENT_TYPE_TEXT	    2

#define BMD_MAIL_CHARSET_UTF8		    1

#define P7M_CONTENT_TYPE_PREFIX 	"Content-Type: application/pkcs7-mime; smime-type=signed-data;name=smime.p7m\r\n"
#define P7M_CONTENT_ENCODING_PREFIX 	"Content-Transfer-Encoding: base64\r\n"
#define P7M_CONTENT_DISPOSITION_PREFIX	"Content-Disposition: attachment; filename=smime.p7m\r\n"
#define P7M_CONTENT_DESCRIPTION_PREFIX	"Content-Description: S/MIME Cryptographic Signature\r\n"
#define P7M_INTERNAL_PREFIX		"MIME-Version: 1.0\r\nContent-Type: text/html; charset=UTF-8; format=flowed\r\nContent-Transfer-Encoding: 7bit\r\n"

typedef struct p7m_info {
    long message_transfer_encoding;  /* transfer-encoding calej wiadomosci - obecnie obslugiwany jest tylko BASE64 */
    long body_content_type;	    /* cialo wiadomosci - czy zwykly tekst czy html */
    long body_charset;		    /* kodowanie ciala wiadomosci */
    long body_transfer_encoding;	    /* transfer-encoding ciala wiadomosci */

    char *body;			    /* tresc wiadomosci */
    char *headers;		    /* naglowki wiadomosci - np. To: From: */
    bmd_crypt_ctx_t *sig_ctx;	    /* kontekst, ktorym podpisze sie wiadomosc */

    char *signed_message_prefix;
    char *internal_prefix;
    char *content_to_sign;
    char *signed_content;

} p7m_info_t;

#if defined(__cplusplus)
extern "C" {
#endif

/* tworzy i inicjalizuje strukture p7m_info_t */
long bmd_p7m_info_create(p7m_info_t **p7m);

/* niszczy strukture */
long bmd_p7m_info_destroy(p7m_info_t **p7m);

/* ustawia domyslne wartosci dla p7m - UTF-8, BASE-64 , text/html, 7bit */
long bmd_p7m_info_set_default(p7m_info_t **p7m);

/* ustawia cialo */
long bmd_p7m_set_body(char *body,p7m_info_t **p7m);

/* ustawia naglowki */
long bmd_p7m_set_headers(char *to,char *from,char *subject,p7m_info_t **p7m);

/* ustawia kontekst */
long bmd_p7m_set_sig_ctx(bmd_crypt_ctx_t *sig_ctx,p7m_info_t **p7m);

/* przygotowuje naglowek wiadomosci P7M */
long _bmd_p7m_prepare_signed_message_prefix(p7m_info_t *p7m,char **prefix);

/* przygotowuje naglowek wewnetrzny wiadomosci P7M */
long _bmd_p7m_prepare_internal_prefix(p7m_info_t *p7m,char **prefix);

/* przygotowuje mime content do podpisania */
long _bmd_p7m_prepare_content_to_sign(p7m_info_t **p7m);

/* podpisuje wiadomosc zawarta w "input" w formacie P7M - wynik umieszcza w "output" */
long bmd_p7m_sign_content(p7m_info_t **p7m);

/* przygotowuje bufor gotowy do wyslania z wykorzystaniem biblioteki libbmdmail */
long bmd_p7m_prepare_email_body(p7m_info_t *p7m,char **output);


/**WSZ*/

/** SMIME P7S API
Autor: Włodzimierz Szczygieł (Dział Programowania, Unizeto Technologies SA)
*/

struct _bmd_mime_attachment
{
	char *file_name;
	long hidden; //jesli zalacznik ma nie byc pokazywany na liscie zalacznikow w kliecie poczty, to wartosc != 0
	GenBuf_t *content;

};
typedef struct _bmd_mime_attachment  bmd_mime_attachment;

struct _bmd_attachment_list
{
	bmd_mime_attachment *item;
	struct _bmd_attachment_list *next;
};
typedef struct _bmd_attachment_list bmd_attach_list;

struct _bmd_mime_message
{
	GenBuf_t *body; //wlasciwa tresc wiadomosci
	long body_type; // np. BODY_TYPE_TEXT_PLAIN
	long body_charset; // np. BODY_CHARSET_UTF8
	char *from; 	//string z nadawca maila(do utworzenie nagłowka wiadomosci) -> FROM: from
	char *to; 	//string z adresatem maila(do utworzenia nagłówka wiadomości) -> TO: to
	char *subject;	//string z tematem maila(do utworzenia nagłówka wiadomości) -> Subject: subject
	char *messageId;
	bmd_attach_list *attachments; //lista struktur odpowiadajacych zalacznikom

	GenBuf_t *mime_entity; // jednostka mime stworzona z body i zalacznikow (to ta jest podpisywana)
	GenBuf_t *signature; // podpis mime_entity
	GenBuf_t *smime; //podpisana wiadomosc gotowa do wysyłki
};
typedef struct _bmd_mime_message  bmd_smime_msg;


/*
to makro tworzy na podstawie tablicy stringow przypisujacych typ mime danemu rozszerzeniu statyczna tablice wykorzystywane przez
funkcje bmd_smime_sign() - POKI CO , MAKRO NIE JEST WYKORZYSTYWANE!!!
rownie dobrze moglaby to byc funkcja ustawiajaca taka tablice w strukturze bmd_mime_msg

#define BIND_MIME_CONTET_TYPES_TABLE( table )  static char *MIME_types_table[]= table;
*/


//attachment_utils.c
#ifndef WIN32
	#define BMD_SMIME_PATH_SEPARATOR '/'
#else
	#define BMD_SMIME_PATH_SEPARATOR '\\'
#endif

#define BMD_END_OF_LIST -883

/*
BMD_SMIME_SHOWN_ATTACHMENT oznacza, ze zalacznik bedzie pokazywany w liscie zalacznikow po stronie klienta poczty
BMD_SMIME_HIDDEN_ATTACHMENT oznacza, ze zalacznik nie bedzie pokazywany na liscie zalacznikow po stronie klienta poczty
*/
#define BMD_SMIME_SHOW_ATTACHMENT   0
#define BMD_SMIME_HIDDEN_ATTACHMENT  1


long init_attach_list(bmd_attach_list **list);
long add_to_attch_list(bmd_attach_list **list, bmd_mime_attachment *attach);
long remove_from_attach_list(bmd_attach_list** list, char *file_name);
long is_attachment_in_attach_list(bmd_attach_list* list, char *file_name);
long destroy_attach_list(bmd_attach_list **list);
long init_iter_attach_list(bmd_attach_list **iterator, bmd_attach_list *head);
long get_next_item_attach_list(bmd_attach_list **iterator, bmd_mime_attachment **next_item);
long bmd_create_attachment(char *file_name, long hidden_flag, bmd_mime_attachment **attach);
long bmd_create_attachment_from_buf(char *file_buffer, long file_len, char *file_name, long hidden_flag, bmd_mime_attachment **attach);
long bmd_destroy_attachment(bmd_mime_attachment **attach);

long bmd_change_linebreak( char *text, char **cut_text );
/* w tej chwili funkcje nie sa wykorzystywane
long character_utf8_to_quotedprintable(char first_char_in_seq, char second_char_in_seq, char** quoted_printable);
long utf8string_to_quotedprintable(char *utf8string, char **quotedstring);
*/
long mail_attachment_name(char *file_name, char** filename_in_message);
long check_ma_ogonki(char* str);

//p7s_internal.c
long _bmd_simple_entity_to_sign(bmd_smime_msg * msg);
long _bmd_createSMIMEmessage(bmd_smime_msg *msg);
long _bmd_createMimeMessage(bmd_smime_msg *msg);
//long _bmd_make_MIMEentity_to_sign(bmd_smime_msg * msg);
long __bmd_make_MIMEentity_to_sign_basic(bmd_smime_msg * msg);
long _bmd_make_MIMEentity_to_sign_advanced(bmd_smime_msg * msg);
/* zrezygnowano z wykorzystania bmd_smime_sign_jpeg_inline() i tym samym z _bmd_make_MIMEentity_to_sign_jpeg_inline()
long _bmd_make_MIMEentity_to_sign_jpeg_inline(bmd_smime_msg * msg);*/
//long bmd_compare_extension(char *file_name, char *extension);
long _find_mime_content_type(char *file_name, char **mime_content_type); //funkcja pomocnicza



//p7s.c
#define BODY_CHARSET_UTF8	0
#define BODY_CHARSET_ISO8859_1	1
#define BODY_CHARSET_ISO8859_2	2
#define BODY_CHARSET_CP1250	3

#define BODY_TYPE_TEXT_PLAIN 0
#define BODY_TYPE_TEXT_HTML 1

#define BMD_SMIME_ERR_CREATE_ENTITY	-884
#define BMD_SMIME_ERR_SIGN_ENTITY	-885
#define BMD_SMIME_ERR_CREATE_SMIME	-886


long bmd_smime_create(bmd_smime_msg ** msg);
long bmd_smime_set_header(bmd_smime_msg * msg, char *from, char *to, char *subject);
long bmd_smime_set_header2(bmd_smime_msg * msg, char *from, char *to, char *subject, char *messageId);
long bmd_smime_set_body(bmd_smime_msg * msg, GenBuf_t *body, long body_type, long body_charset);
long bmd_smime_add_attachment(bmd_smime_msg * msg, char * file_name, long hidden_flag);
long bmd_smime_add_attachment_from_buf(bmd_smime_msg * msg, char *file_buf, long file_len, char *file_name, long hidden_flag);
long bmd_smime_has_attachment(bmd_smime_msg * msg, char *file_name);
long bmd_smime_remove_attachment(bmd_smime_msg * msg, char *file_name);
long bmd_smime_sign(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx);
long bmd_smime_noSign(bmd_smime_msg *msg);
long bmd_smime_toGenBuf(bmd_smime_msg * msg, GenBuf_t ** dest);
long bmd_smime_send(bmd_smime_msg * msg, mail_send_info_t * mailer);
long bmd_smime_destroy(bmd_smime_msg ** msg);
/*funkcja dodatkowa*/
long bmd_smime_fill_mailer(bmd_smime_msg *msg, mail_send_info_t * mailer);

/*play specific*/
/*long bmd_smime_sign_play(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx);*/
/*zrezygnowano z wykorzystania bmd_smime_sign_jpeg_inline
long bmd_smime_sign_jpeg_inline(bmd_smime_msg *msg, bmd_crypt_ctx_t *ctx);*/


#if defined(__cplusplus)
}
#endif

#endif
