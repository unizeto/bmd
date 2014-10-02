#ifndef _LIBBMDDIALOGS_INCLUDED_
#define _LIBBMDDIALOGS_INCLUDED_



#define NOCRYPT	/* Disable include of wincrypt.h */

#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/statline.h>
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <bmd/common/bmd-common.h>
#include <bmd/libbmdutils/libbmdutils.h>

#ifdef WIN32
	/*
	potrzebne dla funkcji IsClassicWindowsStyle() i IsClassicWindowsTheme()
	wewnatrz ktorych wykoystan jest funkcja WinApi IsAppThemed()
	*/
	#include <uxtheme.h>
#endif //ifdef WIN32

#ifdef WIN32
	#ifdef LIBBMDDIALOGS_EXPORTS
	    #define LIBBMDDIALOGS_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
		    #define LIBBMDDIALOGS_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDDIALOGS_SCOPE
		#endif
	#endif
#else
    #define LIBBMDDIALOGS_SCOPE
#endif // ifdef _WIN32


#ifdef WIN32
	long __stdcall uni_show_cms_object(long uiHandler,char *pbBuffer,long cLenBuffer);
	long __stdcall uni_show_ts_object(long uiHandler,char *pbBuffer,long cLenBuffer);
	typedef long (__stdcall *cms_function)(long,const char *,long);
	typedef long (__stdcall *ts_function)(long,const char *,long);
#endif //WIN32


class PINdialog : public wxDialog
{
	public:
		PINdialog(wxWindow *parent, const wxChar* caption, long max_length);
		~PINdialog();
		wxString GetValue();
		void OnCancel(wxCommandEvent &event);
		void OnClose(wxCloseEvent &event);
		void OnOk(wxCommandEvent &event);
		void OnEnter(wxCommandEvent &event);
		wxTextCtrl *text_pin;
		wxButton *button_ok;
		wxButton *button_cancel;
		wxBoxSizer *box_sizer_rows;
		wxBoxSizer *box_sizer_buttons;
		wxStaticText *static_text;
		enum
		{
			ID_BUTTON_OK,
			ID_BUTTON_CANCEL,
			ID_TEXT_PIN,
			ID_TEXT_STATIC
		};
	protected:
		DECLARE_EVENT_TABLE()
		

};


#ifndef __SigGetPIN__
#define __SigGetPIN__


#ifdef WIN32
typedef  long (__stdcall *cert_function)(long,const char *,long); 
long __stdcall uni_show_certificate_object(long uiHandler,char *pbBuffer,long cLenBuffer);
#endif

class SignGetPINdialog : public wxDialog 
{
	private:
		wxTextCtrl* text_pin;
		bool more;
	protected:
		wxBoxSizer* MainSizer;
		wxBoxSizer* StTxtSizer;
		wxStaticBitmap* m_bitmap1;
		wxStaticText* MsgStTxt;
		wxStaticText* m_staticText2;
		wxButton* button_more_less;
		wxButton* button_ok;
		wxButton* button_cancel;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText4;
		wxButton* m_button4;
		wxStaticText* m_staticText41;
		wxListCtrl* m_listCtrl1;
		GenBuf_t *cert;
		DECLARE_EVENT_TABLE()

	public:
		SignGetPINdialog( wxWindow* parent, long id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 446, 305), long style = wxDEFAULT_DIALOG_STYLE );
		~SignGetPINdialog();
		void SetDocCount(long count);
		void SetUsedCert(GenBuf_t *cert);
		wxString GetValue();
		void ClearValue();
		void OnCancel(wxCommandEvent &event);
		void OnClose(wxCloseEvent &event);
		void OnOk(wxCommandEvent &event);
		void OnKey(wxCommandEvent &event);
		void OnMore(wxCommandEvent &event);
		void OnShowCert(wxCommandEvent &event);
		void OnEnter(wxCommandEvent &event);
			enum
		{
			ID_TEXT_PIN,
			ID_MORE,
			ID_SHOW_CERT
		};

};

#endif //__SigGetPIN__



#if defined(__cplusplus)
extern "C" {
#endif

	#ifdef WIN32
		LIBBMDDIALOGS_SCOPE bool IsClassicWindowsStyle(void);
		LIBBMDDIALOGS_SCOPE bool IsClassicWindowsTheme(void);
	#endif //ifdef WIN32

    LIBBMDDIALOGS_SCOPE long bmd_get_password_dialog(char **pass,long *length);
    LIBBMDDIALOGS_SCOPE void bmd_get_pin_dialog(char **pass, long *length);
    LIBBMDDIALOGS_SCOPE void bmd_get_pin_for_sign_dialog(char **pass, long *length, long count, GenBuf_t *cert);
    LIBBMDDIALOGS_SCOPE long bmd_error_dialog(long error_code);
    LIBBMDDIALOGS_SCOPE void bmd_correct_send_dialog(void);
    LIBBMDDIALOGS_SCOPE void bmd_correct_save_dialog(void);
    LIBBMDDIALOGS_SCOPE long bmd_delete_dialog(long how_many);
	///LIBBMDDIALOGS_SCOPE int bmd_pki_dialog(bmd_crypt_ctx_t *ctx);
	LIBBMDDIALOGS_SCOPE long bmd_login_err_dialog(long error_code);
	LIBBMDDIALOGS_SCOPE long bmd_session_err_dialog(long error_code, wxString * error = NULL);
#if defined(__cplusplus)
}
#endif
#endif
