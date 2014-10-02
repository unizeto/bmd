#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif


#ifndef WIN32
#include "../config.h"
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#ifdef WIN32
#pragma warning(disable:4100)
#endif
#include<bmd/common/bmd-common.h>
#include<bmd/libbmdpki/libbmdpki.h>

#include<wx/datetime.h>
#include<wx/mimetype.h>

#include<wx/filename.h>
#include<wx/file.h>
#include<wx/process.h>

#include <bmd/libbmdX509/libbmdX509.h>
#include "../klient/bmdMultiCard.h"
#include <bmd/libbmdres/libbmdres.h>

#define SEP_CHAR_ERROR \
	if (error == NULL)				\
	{								\
		tmp_msg << wxT("\n");		\
	}								\
	else							\
	{								\
		tmp_msg << wxT(" ");		\
	}

long bmd_session_err_dialog(long error_code, wxString * error)
{
	wxString tmp_msg, tmp_title;
	long L_msg_type = wxICON_ERROR;
	long retval = error_code;
	
	tmp_title = _("Error");
	switch(error_code) {
		case BMD_ERR_LOGIN_AGAIN:
			tmp_msg = _("Server destroyed session. Please login again");
			retval = BMD_ERR_LOGIN_AGAIN;
			break;
		case BMD_ERR_TIMEOUT:
			tmp_msg = _("Session has expired. Please login again");
			retval = BMD_ERR_LOGIN_AGAIN;
			break;
		case BMD_ERR_OP_FAILED:
			tmp_msg = _("Server was not able to handle session request");
			break;
		case BMD_ERR_FORMAT:
			tmp_msg = _("Improper session request was send to server");
			break;
		case BMD_ERR_OVERFLOW:
			tmp_msg = _("Session identifier is too big");
			break;
		case BMD_ERR_PROTOCOL:
			tmp_msg = _("Subsequent sessions do not match");
			break;
		case BMD_ERR_RESOURCE_NOTAVAIL:
			tmp_msg = _("Resource temporaly unavailable");
			break;
		case BMD_ERR_NODATA:
			tmp_msg = _("Session request returned no results");
			tmp_title = _("Information");
			L_msg_type = wxICON_INFORMATION;
			break;
		case BMD_ERR_NET_CONNECT:
			tmp_msg = _("Unable to connect with service");
			break;
		case BMD_ERR_NET_WRITE:
			tmp_msg = _("Unable to send session request to service");
			break;
		case BMD_ERR_NET_READ:
			tmp_msg = _("Unable to receive session response from server");
			break;
		case BMD_ERR_TOO_BIG:
			tmp_msg = _("Too big response for session response");
			break;
		case BMD_ERR_NOT_ALLOWED:
			tmp_msg << _("No privileges to execute this operation");
			break;
		case BMD_ERR_INTERRUPTED:
			tmp_msg = _("Operation interrupted by user");
			tmp_title = _("Information");
			L_msg_type = wxICON_INFORMATION;
			break;
		case BMD_ERR_DISK:
			tmp_msg = _("Unable to write or read from disk");
			break;
		case BMD_CANCEL:
			tmp_msg = _("Operation canceled.");
			tmp_title = _("Information");
			L_msg_type = wxICON_INFORMATION;
			break;
		case BMD_ERR_PKI_VERIFY:
			tmp_msg << _("No smartcard in available readers");
			SEP_CHAR_ERROR
			tmp_msg << _("or connection with card temporary lost.");
			SEP_CHAR_ERROR	//	wxT("\t\n") 
			tmp_msg << _("Login again.");
			L_msg_type = wxICON_INFORMATION;
			break;
		case BMD_SQL_GET_ONLY_FILE_DB_ERROR:												/* blad bazy danych podczas pobierania tylko pliku z bazy danych */ 
		case BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR:											/* brak wpisu o rastrze pliku w tabeli crypto_objects */
		case BMD_SQL_GET_ONLY_FILE_LOB_NOT_FOUND_ERROR:										/* loba danego pliku nie znalezeiono w bazie danych */
		case BMD_SQL_GET_ONLY_FILE_SYMKEY_LOB_NOT_FOUND_ERROR:								/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */
			tmp_msg = _("Error during getting file.");
			if (error_code != BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR)
			{	tmp_msg << wxT(" ") << _("Error code :") << wxT(" ") << error_code;
			}
			SEP_CHAR_ERROR 
			tmp_msg << _("Search files on server again.");
			break;
		case BMD_CLIENT_ROLE_NOT_PERMIT_ACTION:
			tmp_msg = _("Your current role does not permit this action.");
			SEP_CHAR_ERROR
			tmp_msg << _("Contact with your administrator.");
			tmp_title = _("Information");
			L_msg_type = wxICON_EXCLAMATION;
			break;
		case PLUGIN_SESSION_ADVICE_NOT_SENT_ACCORDING_TO_METADATA:
		{
			tmp_msg = _("Advice hasn't been sent because of imposed sending conditions.");
			break;
		}
		default:
			tmp_msg << _("Other error occured during session. Error code : ") << error_code;
			;
			break;
	}

	if (error)
	{
		*error << tmp_msg;
	}
	else
	{
		wxMessageBox(tmp_msg, tmp_title, L_msg_type);
	}

	return retval; /* return BMD_OK; */
}

long bmd_login_err_dialog(long error_code)
{
	wxString tmp;
	switch(error_code) {
		case PLUGIN_DVCS_SERVICE_FAILURE:
		{
			wxMessageBox(_("Can't validate certificate using DVCS service."),_("Error"),wxICON_ERROR);
			break;
		}
		case BMD_ERR_CREDENTIALS:
			wxMessageBox(_("You are using improper server certificate."),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_FORMAT:
			wxMessageBox(_("Improper login request was sent to server.\nPossibly version mismatch."),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_OP_FAILED:
			wxMessageBox(_("Login error.\nMake sure you use proper server certificate."),_("Error"),wxICON_ERROR);
			//wxMessageBox(_("Server was not able to handle login request"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_NODATA:
			wxMessageBox(_("User's certificate is not registered in the service"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_NOTAVAIL:
			wxMessageBox(_("No permission for this type of login"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_PKI_VERIFY:
			wxMessageBox(_("User's certificate cannot be used to login in the service"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_OVERFLOW:
			wxMessageBox(_("Too many currently logged users"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_PKI_SIGN:
			wxMessageBox(_("Unable to sign login request"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_PKI_ENCRYPT:
			wxMessageBox(_("Unable to encrypt login request"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_USER_OP_FAILED:
			wxMessageBox(_("Unable to complete login request"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_NET_CONNECT:
			wxMessageBox(_("Unable to connect with service"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_NET_WRITE:
			wxMessageBox(_("Unable to send login request to service"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_NET_READ:
			wxMessageBox(_("Unable to receive login response from server"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_TOO_BIG:
			wxMessageBox(_("Too big response for login response"),_("Error"),wxICON_ERROR);
			break;
		case BMD_ERR_PROTOCOL:
			wxMessageBox(_("Server and client protocol versions differ"),_("Information"),wxICON_INFORMATION);
			break;
		case VERSION_NUMBER_ERR:
			wxMessageBox(_("Protocol version error. Check for updates of your application. Login again."),_("Error"),wxICON_ERROR);
			break;
		case BMD_WRONG_SERVER_CERT:
		{
			wxMessageBox(_("Login error.\nMake sure you use proper server certificate."),_("Error"),wxICON_ERROR);
			break;
		}
		case BMD_ERR_NET_RESOLV:
		{
			tmp = _("Unable to connect with service");
			tmp << wxT(".\n") << _("System could not resolve server address.");
			tmp << wxT("\n") << _("Check configuration if server address is correct.");
			wxMessageBox(tmp,_("Error"),wxICON_ERROR);
			break;
		}
		case PLUGIN_DVCS_CERTIFICATE_INVALID:
		{
			wxMessageBox(_("Chosen login certificate is not valid."), _("Error"), wxICON_ERROR);
			break;
		}
		default:
			tmp = wxString(GetErrorMsg(error_code),wxConvUTF8);
			tmp = wxGetTranslation(tmp);
			wxMessageBox(_("Error occured during login:\n")+tmp,_("Error"),wxICON_ERROR);
			break;
	};

	return BMD_OK;
}

long bmd_get_password_dialog(char **pass,long *length)
{
	/*wxTextEntryDialog *dlg=new wxTextEntryDialog(NULL,_("Enter your password"),_("Enter your password"),wxEmptyString,
						     wxOK|wxCANCEL|wxCENTRE|wxTE_PASSWORD);*/
	if(pass == NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if(*pass != NULL)
	{
		return BMD_ERR_PARAM2;
	}
	if(length == NULL)
	{
		return BMD_ERR_PARAM3;
	}

	PINdialog *dlg=new PINdialog(NULL, _("Enter your password"), 10);
	if(dlg->ShowModal()==wxID_OK)
	{
		WindowsStringToUTF8String((wchar_t*)(dlg->GetValue().wc_str()), pass, length);
	}
	else
	{
		delete dlg;
		dlg=NULL;
		return BMD_ERR_NODATA;
	}
	delete dlg;
	dlg=NULL;

	return BMD_OK;
}

/*jesli length zostanie ustawione na -1, to oznacza wcisniecie przycisku anuluj*/
void bmd_get_pin_dialog(char **pass,long *length)
{
	/*wxTextEntryDialog *dlg=new wxTextEntryDialog(NULL,_("Enter your PIN"),_("Enter your PIN"),wxEmptyString,
					     wxOK|wxCANCEL|wxCENTRE|wxTE_PASSWORD);*/
	PINdialog *dlg=new PINdialog(NULL, _("Enter your PIN"), 10);
	if(dlg->ShowModal()==wxID_OK)
	{
		*pass=NULL;
		WindowsStringToUTF8String((wchar_t*)(dlg->GetValue().wc_str()), pass, length);
	}
	else
	{
		*pass=NULL;
		*length=-1;
	}
	
	delete dlg;
	dlg=NULL;
}


/*jesli length zostanie ustawione na -1, to oznacza wcisniecie przycisku anuluj*/
void bmd_get_pin_for_sign_dialog(char **pass, long *length, long count, GenBuf_t *cert)
{
	SignGetPINdialog *dlg=new SignGetPINdialog(NULL, wxID_ANY, _("Signing documents"));
	dlg->SetUsedCert(cert);
	dlg->SetDocCount(count);
	if(dlg->ShowModal()==wxID_OK)
	{
		*pass=NULL;
		WindowsStringToUTF8String((wchar_t*)(dlg->GetValue().wc_str()), pass, length);
	}
	else
	{
		*pass=NULL;
		*length=-1;
	}

	dlg->ClearValue();
	delete dlg;
	dlg=NULL;
}

long bmd_error_dialog(long error_code)
{
	wxMessageDialog *dlg;
	long return_code;
	switch(error_code)
	{
		/* bledy cryptlib */
		case -40:
			dlg=new wxMessageDialog(NULL,
						_("Unable to connect with server"),
						_("Connection error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -1000:
			dlg=new wxMessageDialog(NULL,
						_("Error with PKCS11 Library"),
						_("PKCS11 Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -1005:
			dlg=new wxMessageDialog(NULL,
						_("Program now will be closed. Please install proper PKCS11 Library"),
						_("PKCS11 error information"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -1001:
			dlg=new wxMessageDialog(NULL,
						_("No smartcard in available readers"),
						_("Smartcards not found"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -10010:
			dlg=new wxMessageDialog(NULL,
						_("No Smartcard readers available"),
						_("No Smartcard readers"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -1002:
			dlg=new wxMessageDialog(NULL,
						_("No certificates were found"),
						_("Certificates error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
						);
			break;
		case -1003:
			dlg=new wxMessageDialog(NULL,
						_("No suitable certificates were found"),
						_("Suitable certificates error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
						);
			break;
		
		case -4000:
			dlg=new wxMessageDialog(NULL,
						_("Certificate cannot be used for login purpose"),
						_("Certificate error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -7000:
			dlg=new wxMessageDialog(NULL,
						_("Incorrect PIN was entered"),
						_("PIN error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -6000:
			dlg=new wxMessageDialog(NULL,
						_("No objects in Database matching criteria"),
						_("No objects in Database"),
						wxOK|wxCENTRE|wxICON_INFORMATION,
						wxDefaultPosition
					       );
			break;
		
		case -6666:
			dlg=new wxMessageDialog(NULL,
					_("Error while timestamping data"),
					_("Timestamping error"),
					wxOK|wxCENTRE|wxICON_ERROR,
					wxDefaultPosition
			       );
			break;
		case -7777:
			dlg=new wxMessageDialog(NULL,
					_("Error while singing data"),
					_("Signing error"),
					wxOK|wxCENTRE|wxICON_ERROR,
					wxDefaultPosition
			       );
			break;
		
		case -20000:
			dlg=new wxMessageDialog(NULL,
					_("Send form is not fullfilled correctly"),
					_("Send form error"),
					wxOK|wxCENTRE|wxICON_ERROR,
					wxDefaultPosition
			       );
			break;

		case -70000:
			dlg=new wxMessageDialog(NULL,
					_("Probable SQL Injection"),
					_("SQL Injection"),
					wxOK|wxCENTRE|wxICON_ERROR,
					wxDefaultPosition
			       );
			break;
		case -99999:
			dlg=new wxMessageDialog(NULL,
					_("Unable to save file"),
					_("Save file error"),
					wxOK|wxCENTRE|wxICON_ERROR,
					wxDefaultPosition
			       );
			break;
		case PKI_OBJ_ERR_CODE_FORMAT_UNKNOWN:
			dlg=new wxMessageDialog(NULL,
						_("File format not recognized"),
						_("File parsing error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case PKI_OBJ_ERR_CODE_BIO:
		case PKI_OBJ_ERR_CODE_GENBUF:
		case PKI_OBJ_ERR_CODE_FORMAT_NOT_CERT:
			dlg=new wxMessageDialog(NULL,
						_("Internal error occured while parsing file"),
						_("File parsing error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case PKI_OBJ_ERR_CODE_P12_NO_CERT:
			dlg=new wxMessageDialog(NULL,
						_("PFX/P12 file does not contain certificates"),
						_("Improper PFX/P12 file"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case PKI_OBJ_ERR_CODE_P12_NO_EE_CERT:
			dlg=new wxMessageDialog(NULL,
						_("PFX/P12 file does not contain end user certificates"),
						_("Improper PFX/P12 file"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case PKI_OBJ_ERR_CODE_P12_PASS:
			dlg=new wxMessageDialog(NULL,
						_("Wrong PFX/P12 file password"),
						_("Wrong password"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;

		case -1111:
			dlg=new wxMessageDialog(NULL,
						_("Error occured while getting file"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -2222:
			dlg=new wxMessageDialog(NULL,
						_("Error occured while getting file with proofs"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -3333:
			dlg=new wxMessageDialog(NULL,
						_("Error occured while deleting file"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -4444:
			dlg=new wxMessageDialog(NULL,
						_("Error occured while sending file"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -77777:
			dlg=new wxMessageDialog(NULL,
						_("Request rejected by server !!!"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case BMD_ERR_TIMEOUT:
			dlg=new wxMessageDialog(NULL,
						_("User settings have timed out !!!"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -88888:
			dlg=new wxMessageDialog(NULL,
						_("Request failed"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -111111:
			dlg=new wxMessageDialog(NULL,
						_("Unable to show signature's details"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -222222:
			dlg=new wxMessageDialog(NULL,
						_("Improper signature type in configuration file"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		case -333333:
			dlg=new wxMessageDialog(NULL,
						_("Files must be signed due to the program license"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
		default:
			dlg=new wxMessageDialog(NULL,
						_("Other error occured"),
						_("Error"),
						wxOK|wxCENTRE|wxICON_ERROR,
						wxDefaultPosition
					       );
			break;
	}
	switch(dlg->ShowModal()) {
		case wxID_YES: return_code=wxID_YES; break;
		case wxID_NO: return_code=wxID_NO; break;
		case wxID_CANCEL: return_code=wxID_CANCEL; break;
		case wxID_OK: return_code=wxID_OK; break;
		default : return_code=wxID_CANCEL; break;
	};
	dlg->Destroy();
	return return_code;
}

void bmd_correct_send_dialog(void)
{
	wxMessageDialog *dlg;
	dlg=new wxMessageDialog(NULL,
				_("Data successfully sent."),
				_("Sent successfull"),
				wxOK|wxCENTRE|wxICON_INFORMATION,
				wxDefaultPosition
				);
	dlg->ShowModal();
	dlg->Destroy();
}

void bmd_correct_save_dialog(void)
{
	wxMessageDialog *dlg;
	dlg=new wxMessageDialog(NULL,
				_("Data written on disk"),
				_("Save data"),
				wxOK|wxCENTRE|wxICON_INFORMATION,
				wxDefaultPosition
				);
	dlg->ShowModal();
	dlg->Destroy();
}

long bmd_delete_dialog(long how_many=1)
{
	wxMessageDialog *dlg;
	long return_code=-1;

	if(how_many > 1)
	{
		dlg=new wxMessageDialog(NULL,_("Really remove files from list?"),_("Removing files from list"),
							wxYES|wxNO|wxCENTRE|wxICON_QUESTION,wxDefaultPosition);
	}
	else
	{
		dlg=new wxMessageDialog(NULL,_("Really remove file from list?"),_("Removing file from list"),
							wxYES|wxNO|wxCENTRE|wxICON_QUESTION,wxDefaultPosition);
	}
	switch(dlg->ShowModal()) {
		case wxID_YES: return_code=wxID_YES; break;
		case wxID_NO: return_code=wxID_NO;break;
	};
	dlg->Destroy();
	return return_code;
}

void bmd_info_dialog(void)
{
	wxMessageDialog *dlg;
	dlg=new wxMessageDialog(NULL,
				wxT("Unizeto e-FAKTURA\nCopyright 2004-2010"),
				wxT("Informacja"),
				wxOK|wxCENTRE|wxICON_INFORMATION,
				wxDefaultPosition
				);
	dlg->ShowModal();
	dlg->Destroy();
}


PINdialog::PINdialog(wxWindow *parent, const wxChar* caption, long max_length=10)
			:wxDialog(parent, -1, caption, wxDefaultPosition, wxSize(200,120), wxDEFAULT_DIALOG_STYLE)
{
	this->text_pin=new wxTextCtrl(this, ID_TEXT_PIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD|wxTE_PROCESS_ENTER);
	this->text_pin->SetMaxLength( (max_length <= 0 ? 10 : max_length) );
	this->button_ok=new wxButton(this, ID_BUTTON_OK, _("Ok"));
	this->button_cancel=new wxButton(this, wxID_CANCEL, _("Cancel"));
	this->box_sizer_rows=new wxBoxSizer(wxVERTICAL);
	this->box_sizer_buttons= new wxBoxSizer(wxHORIZONTAL);
	this->static_text=new wxStaticText(this, ID_TEXT_STATIC, wxString(caption)<<wxT(":"));

	this->box_sizer_buttons->Add(this->button_ok, 0 , wxRIGHT, 1);
	this->box_sizer_buttons->Add(this->button_cancel, 0 , wxLEFT, 1);

	this->box_sizer_rows->AddSpacer(6);
	this->box_sizer_rows->Add(this->static_text, 0, wxALIGN_CENTER, 5);
	this->box_sizer_rows->AddSpacer(10);
	this->box_sizer_rows->Add(this->text_pin, 0, wxALIGN_CENTER, 10);
	this->box_sizer_rows->AddSpacer(10);
	this->box_sizer_rows->Add(this->box_sizer_buttons, 0, wxALIGN_CENTER, 10);
	this->box_sizer_rows->AddSpacer(15);
	
	this->SetSizer(this->box_sizer_rows);
	this->Layout();
}

PINdialog::~PINdialog()
{
	this->text_pin->Destroy();
	this->button_ok->Destroy();
	this->button_cancel->Destroy();
}

wxString PINdialog::GetValue()
{
	return this->text_pin->GetValue();
}

void PINdialog::OnCancel(wxCommandEvent &event)
{
	this->EndModal(wxID_CANCEL);
}

void PINdialog::OnClose(wxCloseEvent &event)
{
	this->EndModal(wxID_CANCEL);
}
void PINdialog::OnOk(wxCommandEvent &event)
{
	this->EndModal(wxID_OK);
}

void PINdialog::OnEnter(wxCommandEvent &event)
{
	this->EndModal(wxID_OK);
}

BEGIN_EVENT_TABLE(PINdialog,wxDialog)
	EVT_BUTTON(ID_BUTTON_OK,PINdialog::OnOk)
	EVT_BUTTON(ID_BUTTON_CANCEL,PINdialog::OnCancel)
	EVT_CLOSE(PINdialog::OnClose)
	EVT_TEXT_ENTER(ID_TEXT_PIN, PINdialog::OnEnter)
END_EVENT_TABLE()



SignGetPINdialog::SignGetPINdialog( wxWindow* parent, long id, wxString title, wxPoint pos, wxSize size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	wxString tmps;
	this->SetSizeHints( wxSize( 446,305 ), wxDefaultSize );
	
	MainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	libbmdres_t *res=NULL;
	wxBitmap *res_bitmap=(wxBitmap*)NULL;
	wxString tmp_bmdres=wxString(wxT("bmdres"));
	libbmdres_init(tmp_bmdres, &res);
	//jesli nie znalazl, to pusta bitmapa (zeby nie bylo segv)
	wxString tmp_tss=wxString(wxT("timestampedsigned.png"));
	if( (libbmdres_get_wxBitmap(res, tmp_tss, wxBITMAP_TYPE_PNG, &res_bitmap)) != 0)
	{
		res_bitmap=new wxBitmap();
	}
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, *res_bitmap , wxDefaultPosition, wxSize( -1,-1 ), 0);
//	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("F:\\bmd\\bmd\\trunk\\src\\klient\\bitmaps\\signed.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	delete res_bitmap;
	libbmdres_destroy(&res);
	bSizer5->Add( 0, 11, 0, 0, 0 );
	bSizer5->Add( m_bitmap1, 0, wxALL, 10 );
	
	bSizer2->Add( bSizer5, 0, wxEXPAND, 5 );
	
	StTxtSizer = new wxBoxSizer( wxVERTICAL );
	StTxtSizer->SetMinSize(wxSize( -1,120 ));
	//StTxtSizer->Add( 0, 20, 0, 0, 0 );
	
	MsgStTxt = new wxStaticText( this, wxID_ANY, wxT("\n"), wxDefaultPosition, wxSize( -1,169 ), 0 );//wxSize(-1,150)
	StTxtSizer->Add( MsgStTxt, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM, 20 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	tmps << _("PIN \"") << _("Secure profile") << wxT("\":");
	m_staticText2 = new wxStaticText( this, wxID_ANY, tmps, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10 );
	
	text_pin = new wxTextCtrl( this, ID_TEXT_PIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD|wxTE_PROCESS_ENTER );
	text_pin->SetMaxLength(128);
	bSizer71->Add( text_pin, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10 );
	
	StTxtSizer->Add( bSizer71, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer2->Add( StTxtSizer, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	bSizer7->Add( 20, 0, 0, 0, 0 );
	
	bSizer2->Add( bSizer7, 0, wxEXPAND, 5 );
	
	MainSizer->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer8->Add( 0, 0, 1, wxEXPAND, 0 );
	
//	button_more_less = new wxButton( this, ID_MORE, _("More >>"), wxDefaultPosition, wxDefaultSize, 0 );
//	bSizer8->Add( button_more_less, 0, wxALL|wxALIGN_BOTTOM, 5 );
	this->more = true;
	
	MainSizer->Add( bSizer8, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 0 );
	
	button_ok = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	button_ok->Enable( false );
	
	bSizer4->Add( button_ok, 0, wxALL|wxALIGN_BOTTOM, 0 );
	bSizer4->Add( 5, 0, 0, 0, 0 );


	m_button4 = new wxButton( this, ID_SHOW_CERT, _("Certificate details"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL|wxALIGN_BOTTOM, 0 );
	bSizer4->Add( 5, 0, 0, 0, 0 );

	button_cancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( button_cancel, 0, wxALL|wxALIGN_BOTTOM, 0 );
	bSizer4->Add( 6, 0, 0, 0, 0 );

	
	MainSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
//	wxBoxSizer* bSizer9;
//	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
//	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
//	bSizer9->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	//wxBoxSizer* bSizer10;
	//bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	//
	//m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Podpis zostanie z³o¿ony kluczem zwi¹zanym z certyfikatem:"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer10->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	//
	//bSizer10->Add( 0, 0, 1, wxEXPAND, 0 );
	
//	bSizer10->Add( m_button4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	//bSizer9->Add( bSizer10, 0, wxEXPAND, 5 );
	
//	m_staticText41 = new wxStaticText( this, wxID_ANY, wxT("Lista podpisywanych plików i ich skrótów:"), wxDefaultPosition, wxDefaultSize, 0 );
//	bSizer9->Add( m_staticText41, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
//	m_listCtrl1 = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), wxLC_LIST );
//	bSizer9->Add( m_listCtrl1, 5, wxALL|wxEXPAND, 5 );
	
//	MainSizer->Add( bSizer9, 1, wxEXPAND, 5 );
	
	this->cert= NULL;

	this->SetSizer( MainSizer );
	this->Layout();
}

void SignGetPINdialog::SetDocCount(long count)
{
	wxString komunikat;
	char *tmp=NULL;
	long bleble=0;

	komunikat << _("Certificate data:") << wxT("\n\n");
	
	/* pobranie commonname podmiotu */
	X509CertGetAttr(this->cert,X509_ATTR_SUBJECT_COMMONNAME,&tmp,&bleble);
	komunikat << _("Subject:\t") << wxString(tmp,wxConvUTF8) << wxT("\n");				//"Subject common name:\t\t"
	free(tmp);	tmp=NULL;	bleble=0;

	/* pobranie commonname wystawcy */
	X509CertGetAttr(this->cert,X509_ATTR_ISSUER_COMMONNAME,&tmp,&bleble);
	komunikat << _("Issuer:\t") << wxT("\t") << wxString(tmp,wxConvUTF8) << wxT("\n");
	free(tmp);	tmp=NULL;	bleble=0;

	/* pobranie numeru seryjnego */
//	X509CertGetAttr(cert,X509_ATTR_SN,serial,&bleble);


	/* pobranie organizacji podmiotu */
//	bleble=0;
//	X509CertGetAttr(cert,X509_ATTR_SUBJECT_ORGANIZATION,organization,&bleble);
	//get_Organization_from_subject(tmp_cert,organization);
	
	/* pobranie okresu waznosci*/
	X509CertGetAttr(this->cert,X509_ATTR_VALIDITY_NB,&tmp,&bleble);
	komunikat << _("Valid:\t\t") << wxString(tmp,wxConvUTF8) << wxT("  -  ");
	free(tmp);	tmp=NULL;	bleble=0;

	X509CertGetAttr(this->cert,X509_ATTR_VALIDITY_NA,&tmp,&bleble);
	komunikat << wxString(tmp,wxConvUTF8) << wxT("\n\n");
	free(tmp);	tmp=NULL;	bleble=0;
	
	komunikat << _("Do you trust application which reqest document signing?") << wxT("\n\n");

	komunikat << _("Entering PIN code and clicking OK will cause setting e-sign which may cause law causes equal hand sign.") << wxT("\n\n");
	komunikat << _("Count of signed documents: ") << count << wxT("\n");
	MsgStTxt->SetLabel(komunikat);
	this->Layout();
}

void SignGetPINdialog::SetUsedCert(GenBuf_t *cert)
{
	this->cert=cert;
}

SignGetPINdialog::~SignGetPINdialog()
{
	this->text_pin->SetValue(wxT("0000000000"));
	this->text_pin->Clear();
	delete this->text_pin;
	this->text_pin = NULL;
	this->button_ok->Destroy();
	this->button_cancel->Destroy();
	this->m_button4->Destroy();
	this->m_staticText2->Destroy();
	this->MsgStTxt->Destroy();
	this->m_bitmap1->Destroy();
	//delete this->MainSizer;
	//this->MainSizer = NULL;
	//this->StTxtSizer = NULL;
	this->Destroy();
}

wxString SignGetPINdialog::GetValue()
{
	return this->text_pin->GetValue();
}

/*Funkcja zamazuje pin w oknie i zwalnia go. Nie wiadomo jak to dziala wewnatrz wx wiec nie ma 100% pewnosci.*/
void SignGetPINdialog::ClearValue()
{
	long twl_i				= 0;
	long twl_length			= 0;
	wxString clear_string;

	twl_length = this->text_pin->GetLineLength(0);

	/*Tworze wxstring ktory sluzy do likwidacji pin. Nie wiadomo jak to dzialo*/
	for(twl_i = 0; twl_i < twl_length; twl_i++)
	{
		clear_string.Append('*', 1);
	}
	this->text_pin->SetValue(clear_string);
	this->text_pin->Clear();

	return;
}

void SignGetPINdialog::OnCancel(wxCommandEvent &event)
{
	this->EndModal(wxID_CANCEL);
}

void SignGetPINdialog::OnClose(wxCloseEvent &event)
{
	this->EndModal(wxID_CANCEL);
}
void SignGetPINdialog::OnOk(wxCommandEvent &event)
{
	this->EndModal(wxID_OK);
}

void SignGetPINdialog::OnEnter(wxCommandEvent &event)
{
	if (!this->text_pin->IsEmpty())
		this->EndModal(wxID_OK);
}
void SignGetPINdialog::OnKey(wxCommandEvent &event)
{
	if (this->text_pin->IsEmpty())
	{	
		this->button_ok->Enable(	false );
	}
	else
	{	
		this->button_ok->Enable(	true );
	}
}

void SignGetPINdialog::OnMore(wxCommandEvent &event)
{
	if (this->more)
	{
		this->SetSize(-1, 375);
		this->button_more_less->SetLabel(_("Less <<"));
		this->more = false;
	}
	else
	{
		this->SetSize(-1, 325);
		this->button_more_less->SetLabel(_("More >>"));
		this->more = true;
	}
}

void SignGetPINdialog::OnShowCert(wxCommandEvent &event)
{
	long status;
	if (this->cert)
		status=uni_show_certificate_object(0,this->cert->buf,this->cert->size);
}

long __stdcall uni_show_certificate_object(long uiHandler,char *pbBuffer,long cLenBuffer)
{
	HINSTANCE pLibrary=NULL;
	cert_function function_cerificate_object=NULL;
	long return_code;

	pLibrary=LoadLibrary(wxT("uniVdlg2.dll")); /* zaladowanie biblioteki */
	if(pLibrary==NULL)
		return -1;
	function_cerificate_object=(cert_function)GetProcAddress((HMODULE)pLibrary,"_uniVShowCertificate@12"); /* zaladowanie metody */
	if(function_cerificate_object==NULL)
		return -2;

	return_code=function_cerificate_object(uiHandler,pbBuffer,cLenBuffer);

	FreeLibrary((HMODULE)pLibrary);
	return return_code*(-1);
}


bool IsClassicWindowsTheme(void)
{
	if(IsAppThemed() == FALSE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsClassicWindowsStyle(void)
{
	return IsClassicWindowsTheme();
}


BEGIN_EVENT_TABLE(SignGetPINdialog,wxDialog)
	EVT_BUTTON(wxID_OK,SignGetPINdialog::OnOk)
	EVT_BUTTON(wxID_CANCEL,SignGetPINdialog::OnCancel)
	EVT_BUTTON(ID_MORE,SignGetPINdialog::OnMore)
	EVT_BUTTON(ID_SHOW_CERT,SignGetPINdialog::OnShowCert)
	EVT_CLOSE(SignGetPINdialog::OnClose)
	EVT_TEXT_ENTER(ID_TEXT_PIN, SignGetPINdialog::OnEnter)
	EVT_TEXT(ID_TEXT_PIN, SignGetPINdialog::OnKey)
END_EVENT_TABLE()



#ifdef WIN32

long __stdcall uni_show_cms_object(long uiHandler,char *pbBuffer,long cLenBuffer)
{
	HINSTANCE pLibrary=NULL;
	cms_function function_cms_object=NULL;
	long return_code=0;

	pLibrary=LoadLibrary(wxT("uniVdlg2.dll")); /* zaladowanie biblioteki */
	if(pLibrary==NULL)
		{ return -1; }
	function_cms_object=(cms_function)GetProcAddress((HMODULE)pLibrary,"_uniVShowSignedData@12"); /* zaladowanie metody */
	if(function_cms_object==NULL)
	{
		FreeLibrary((HMODULE)pLibrary);
		return -2;
	}

	return_code=function_cms_object(uiHandler,pbBuffer,cLenBuffer);

	FreeLibrary((HMODULE)pLibrary);
	return return_code*(-1);
}

long __stdcall uni_show_ts_object(long uiHandler,char *pbBuffer,long cLenBuffer)
{
	HINSTANCE pLibrary=NULL;
	ts_function function_ts_object=NULL;
	long return_code=0;

	pLibrary=LoadLibrary(wxT("uniVdlg2.dll")); /* zaladowanie biblioteki */
	if(pLibrary==NULL)
		{ return -1; }
	function_ts_object=(ts_function)GetProcAddress((HMODULE)pLibrary,"_uniVShowTimeStamp@12"); /* zaladowanie metody */
	if(function_ts_object==NULL)
	{
		FreeLibrary((HMODULE)pLibrary);
		return -2;
	}

	return_code=function_ts_object(uiHandler,pbBuffer,cLenBuffer);

	FreeLibrary((HMODULE)pLibrary);
	return return_code*(-1);
}

#endif //ifdef WIN32
