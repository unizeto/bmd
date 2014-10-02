#ifndef _BMD_KS_INCLUDED_
#define _BMD_KS_INCLUDED_

#ifdef WIN32
#pragma warning(disable:4201)
#endif

#include <bmd/bmd_ks/bmd_ks_ds.h>
#include <bmd/bmd_ks/bmd_ks_const.h>

#define free0(x) {free(x);x=NULL;};

/* proste funkcje uzytkowe - mapowanie kodow bledow */
DWORD svc_map_err_code(long svc_error_code);
void svc_gen_err_desc(char **dest,char *fmt,...);

/* DEBUG - print */
void print_debug(char *fmt,...);

/* dodaje ustawienia logowanie w rejestrze */
BOOL svc_registry_add(char * pszMsgDLL,DWORD  dwNum);
/* usuwa dane klucz z rejestru */
BOOL svc_registry_delete(void);

/* instaluje usluge w menadzerze SCM */
void svc_install(void);
/* kasuje usluge w menadzerze SCM */
void svc_delete(void);
/* odczytuje jakie klucze prywatne sa w slotach */
void svc_p11_list(void);

/* laduje konfiguracje */
long svc_load_conf(char * conf_file,svc_conf_t **svc_conf);

/* inicjalizuje PKCS#11 w sposob wielowatkowy */
long svc_pkcs11_init(const char *pkcs11_lib,svc_p11_t **svc_p11,char **error);
/* pobiera dane z PKCS#11 */
long svc_pkcs11_prepare(const char *sig_key_id,const char *dec_key_id,const char *pin,svc_p11_t **svc_p11,char **error);
/* podpisuje dane z PKCS#11 */
long svc_pkcs11_sign(char *input,long length,
                    CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hPrivKey,CK_FUNCTION_LIST_PTR pFunctionList,
                    char **output,long *out_length,char **error);
/* deszyfruje dane z PKCS#11 */
long svc_pkcs11_decrypt(char *input,long length,
                       CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hPrivKey,CK_FUNCTION_LIST_PTR pFunctionList,
                       char **output,long *out_length,char **error);

/* obsluguje komunikaty od SCM */
void WINAPI svc_ctrlhandler(DWORD dwCtrl);
/* entry-polong dla uslugi */
void WINAPI svc_main(DWORD dwArgc, char * *lpszArgv);
/* przesyla do SCM wlasciwy status uslugi */
void svc_reportstatus(DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint,DWORD dwCheckPoint);
/* wysyla zdarzenie do event-logu systemowego */
void svc_log(DWORD errCode,WORD eventType,char * addData,char * function,LPVOID binData,DWORD binLength,DWORD win32ErrCode);
/* request */
long svc_encode_request(char type,char *data,long data_length,bmd_ks_request_t **ks_req);
long svc_decode_request(char *buf,long len,bmd_ks_request_t **req);
long svc_serialize_request(bmd_ks_request_t *req,char **buf,long *len);
/* response */
long svc_encode_response(char resp_type,char status,char *od,long odl,
                        char *cd,long cdl,bmd_ks_response_t **response);
long svc_decode_response(char *buf,long len,bmd_ks_response_t **response);
long svc_serialize_response(bmd_ks_response_t *resp,char **buf,long *len);
/* odczytanie danych z sieci */
long svc_net_read(SOCKET s,long max_length,bmd_ks_request_t **ks_req,char **dbuf,long *dlen);
/* wyslanie danych do sieci */
long svc_net_write(SOCKET s,bmd_ks_response_t *ks_resp);

/* inicjalizuje siec */
long svc_net_initialize(char **error);

/* tworzy gniazda do nasluchu, wykonujac sekwencje socket, bind listen */
long svc_net_create_listen_socket(SOCKET *s,const char *adr,const long port,const long lq,char **error);

/* wysyla dane po TCP */
long svc_tcp_sendall(SOCKET s,char *buf,long len);
long svc_tcp_send(SOCKET s,char *data,long data_length);

/* odbiera dane po TCP */
long svc_tcp_recvall(SOCKET s,long timeout_sec,char **buf,long *buf_len);
long svc_tcp_recv(SOCKET s,char **data,long *data_length);

/* funkcja wykonywana przez watek */
DWORD WINAPI bmd_ks_thread(LPVOID lpParameter);
/* tworzenie watkow przez usluge */
long svc_create_threads(SOCKET ls);

/* konwersja identyfikatora klucza na sformatowany komunikat char'ow */
long svc_thread_gen_keyid_wide(char *keyid,char *format,char **output);
/* obsluga requesta podpisania danych, wykona logowanie do EventLog i utworzy ks_resp */
long svc_thread_handle_sign_request(bmd_ks_request_t *ks_req,char *req_buf,long req_len,long nr,
								   bmd_ks_response_t **ks_resp);
/* obsluga requesta deszyfrowania danych, wykona logowania do EventLog i utworzy ks_resp */
long svc_thread_handle_decrypt_request(bmd_ks_request_t *ks_req,char *req_buf,long req_len,
									  long nr,bmd_ks_response_t **ks_resp);
/* obsluga requesta pobierania certyfikatow (S oraz ED), zalokuje pamiec pod wynikowy certyfikat */
long svc_thread_handle_getcert_request(char req_type,char **cert,long *cert_length);

#endif
