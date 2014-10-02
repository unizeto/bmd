#ifdef WIN32
#include <winsock2.h>
#endif
#define _WINSOCK2API_
/****************************

klasa bmdCertDialog
autor: W³odzimierz Szczygie³ (Unizeto Technologies SA)

****************************/

#include <bmd/libbmddialogs/bmdCertDialog.h>


BEGIN_EVENT_TABLE(bmdCertDialog, wxDialog)
	EVT_BUTTON(bmdCertDialog::BUTTON_OK_ID, bmdCertDialog::OnButtonOK)
	EVT_BUTTON(bmdCertDialog::BUTTON_CANCEL_ID, bmdCertDialog::OnButtonCancel)
	EVT_LISTBOX(bmdCertDialog::CERTS_LIST_ID, bmdCertDialog::OnListBoxChangeSelection)
	EVT_LISTBOX_DCLICK(bmdCertDialog::CERTS_LIST_ID, bmdCertDialog::OnListBoxDoubleClick)
	EVT_CLOSE(bmdCertDialog::OnClose)
END_EVENT_TABLE()


/**
konstruktor klasy - przed wywolaniem musi byc przygotowany context z pliku pfx (bmd_crypt_ctx_t -> file)
*/
bmdCertDialog::bmdCertDialog(wxWindow *parent, file_crypto_info_t* fileCryptoInfo) :
	wxDialog(parent, -1, _("Choose certificate"), wxDefaultPosition, wxSize(650,350), wxDEFAULT_DIALOG_STYLE)
{
	long retVal=0;

	this->multiCardObjectPtr=NULL;
	this->fileCryptoInfoPtr=fileCryptoInfo;
	this->sourceType=bmdCertDialog::TYPE_PFX;
	this->signingCertVerificationConfig=NULL;
	this->certForSigning=false;
	this->pkcs11Context=NULL;
	this->BuildDialog();
	retVal=this->FillList();
	this->buttonOk->SetFocus();
}

/**
konstruktor klasy - przed wywolaniem musi byc przygotowany obiekt bmdMultiCard (kontekst(y) PKCS#11)
@arg certificateForSigning
	-	ustawiony na true oznacza, ze wybrany w oknie certyfikat przeznaczony jest do podpisu
		i klasa nie bedzie pytala o PIN do wydobycia klucza prywatnego (to maja zapewnic mechanizmy poza klasa)
	-	ustawiony na false oznacza, ze wybrany w oknie certyfikat nie jest przeznaczony do podpisu
		i klasa bmdCertDialog bedzie pytala o PIN potrzebny do wydobycia klucza prywatnego
@arg signingCertVerificationConfig brane jest pod uwage tylko, gdy certificateForSigning=true
	-	ustawione (!=NULL) wykonuje sprawdzenie czy wskazany w oknie certyfikat jest prawidlowy
		do wykonania podpisu kwalifikowanego (jesli nie jest, to takiego certyfikatu nie bedzie mozna wybrac)
	-	ustawione na NULL (wartosc domyslna) nie wykonuje sprawdzenia poprawnosci certyfikatu 
*/
bmdCertDialog::bmdCertDialog(wxWindow *parent, bmdMultiCard_t *multiCardObject, bool certificateForSigning, CertExtConfig_t *signingCertVerificationConfig) :
	wxDialog(parent, -1, _("Choose certificate"), wxDefaultPosition, wxSize(650,350), wxDEFAULT_DIALOG_STYLE)
{
	long retVal=0;

	this->multiCardObjectPtr=multiCardObject;
	this->fileCryptoInfoPtr=NULL;
	this->sourceType=bmdCertDialog::TYPE_PKCS11;
	this->signingCertVerificationConfig=signingCertVerificationConfig;
	this->certForSigning=certificateForSigning;
	this->pkcs11Context=NULL;
	this->BuildDialog();
	retVal=this->FillList();
	this->buttonOk->SetFocus();
}

/**
destruktor klasy bmdCertDialog
*/
bmdCertDialog::~bmdCertDialog()
{
	this->multiCardObjectPtr=NULL;
	this->fileCryptoInfoPtr=NULL;
	bmd_ctx_destroy(&(this->pkcs11Context));
}

/**
metoda ShowCertDialog() pokazuje okno wyboru certyfikatu
@retval wxID_OK albo wxID_CANCEL
*/
long bmdCertDialog::ShowCertDialog(void)
{
	return this->ShowModal();
}

/**
metoda BuildDialog() tworzy okno (wywolywana w konstruktorze klasy)
*/
void bmdCertDialog::BuildDialog(void)
{
	wxFont boldFont;
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);

	this->mainFlexSizer=new wxFlexGridSizer(2, 2, 0, 0);
	this->mainFlexSizer->AddGrowableCol(1);
	this->mainFlexSizer->AddGrowableRow(0);

	this->certsList=new wxListBox(this, bmdCertDialog::CERTS_LIST_ID, wxDefaultPosition, wxSize(160, -1), 0, NULL, wxLB_SINGLE);
	this->certListStaticBox=new wxStaticBox(this, wxID_ANY, _("Certificates:"));
	this->certsListStaticSizer=new wxStaticBoxSizer(this->certListStaticBox, wxVERTICAL);
	this->certsListStaticSizer->Add(this->certsList, 1 , wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 2);

	this->infoStaticBox=new wxStaticBox(this, wxID_ANY, _("Certificate details:"));
	this->infoStaticSizer=new wxStaticBoxSizer(this->infoStaticBox, wxVERTICAL);
	this->infoFlexSizer=new wxFlexGridSizer(6, 2, 8, 6);
	this->infoStaticSizer->Add(this->infoFlexSizer, 1, wxEXPAND|wxLEFT, 4);
	this->userLabel=new wxStaticText(this, wxID_ANY, _("Subject:"));
	this->userValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->userValue->SetFont(boldFont);
	this->organizationLabel=new wxStaticText(this, wxID_ANY, _("Organization:"));
	this->organizationValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->organizationValue->SetFont(boldFont);
	this->serialLabel=new wxStaticText(this, wxID_ANY, _("Serial number:"));
	this->serialValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->serialValue->SetFont(boldFont);
	this->issuerLabel=new wxStaticText(this, wxID_ANY, _("Issuer:"));
	this->issuerValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->issuerValue->SetFont(boldFont);
	this->validFromLabel=new wxStaticText(this, wxID_ANY, _("Valid from:"));
	this->validFromValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->validFromValue->SetFont(boldFont);
	this->validToLabel=new wxStaticText(this, wxID_ANY, _("Valid to:"));
	this->validToValue=new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->validToValue->SetFont(boldFont);
	
	this->infoFlexSizer->Add(this->userLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->userValue, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->organizationLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->organizationValue, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->serialLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->serialValue, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->issuerLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->issuerValue, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->validFromLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->validFromValue, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->validToLabel, 1 , wxEXPAND);
	this->infoFlexSizer->Add(this->validToValue, 1 , wxEXPAND);

	this->buttonsSizer=new wxBoxSizer(wxHORIZONTAL);
	
	this->buttonOk=new wxButton(this, bmdCertDialog::BUTTON_OK_ID, _("Choose"));
	this->buttonCancel=new wxButton(this, bmdCertDialog::BUTTON_CANCEL_ID, _("Cancel"));
	this->buttonsSizer->AddStretchSpacer();
	this->buttonsSizer->Add(this->buttonOk, 0, wxALIGN_RIGHT|wxRIGHT, 4);
	this->buttonsSizer->Add(this->buttonCancel, 0, wxALIGN_RIGHT|wxRIGHT, 4);


	this->mainFlexSizer->Add(this->certsListStaticSizer, 1, wxEXPAND|wxALL, 2);
	this->mainFlexSizer->Add(this->infoStaticSizer, 1, wxEXPAND|wxALL, 2);
	this->mainFlexSizer->AddStretchSpacer();
	this->mainFlexSizer->Add(this->buttonsSizer, 1, wxEXPAND|wxTOP|wxBOTTOM, 8);
	this->SetSizer(this->mainFlexSizer);
	this->SetAutoLayout(true);
	this->Layout();
}

/**
metoda FillList() uzupelnia liste certyfikatow (wywolywana w konstruktorze klasy)
@retval 0 jesli sukces operacji
@retval < 0 jesli blad
*/
long bmdCertDialog::FillList(void)
{
	long retVal=0;
	long iter=0;
	GenBuf_t *cert=NULL;
	long attrLong=0;
	wxString listElement;
	char *serialNumber=NULL;
	wxString serialString;

	this->EnableChooseButton(false);

	if(this->sourceType == bmdCertDialog::TYPE_PKCS11)
	{
		if(this->multiCardObjectPtr == NULL)
		{
			return -1;
		}

		for(iter=0; iter<this->multiCardObjectPtr->GetCertificatesCount(); iter++)
		{

			this->multiCardObjectPtr->GetCertPtr(iter, &cert);
			retVal=X509CertGetAttr(cert, X509_ATTR_SN, &serialNumber, &attrLong);
			cert=NULL;
			if(retVal != BMD_OK)
			{ 
				listElement<<_("Certificate no ")<<wxT(" ")<<iter+1<<wxT(" (")<< _("wrong format") <<wxT(")");
			}
			else
			{
				serialString=wxString(serialNumber,wxConvUTF8);
				listElement<<_("Certificate no ")<<wxT(" ")<<iter+1<<wxT(" (")<<serialString<<wxT(")");
				serialString.Clear();
			}
			this->certsList->Append(listElement);
			free(serialNumber); serialNumber=NULL;
			listElement.Clear();
			attrLong=0;
		}
	}
	else if(this->sourceType == bmdCertDialog::TYPE_PFX)
	{
		if(this->fileCryptoInfoPtr == NULL)
		{
			return -2;
		}
		retVal=X509CertGetAttr(this->fileCryptoInfoPtr->cert, X509_ATTR_SN, &serialNumber, &attrLong);
		if(retVal != BMD_OK)
		{ 
			listElement<<_("Certificate no ")<<wxT(" ")<<iter+1<<wxT(" (")<< _("wrong format") <<wxT(")");
		}
		else
		{
			serialString=wxString(serialNumber, wxConvUTF8);
			listElement<<_("Certificate no ")<<wxT(" ")<<iter+1<<wxT(" (")<<serialString<<wxT(")");
			serialString.Clear();
		}
		this->certsList->Append(listElement);
		free(serialNumber); serialNumber=NULL;
		listElement.Clear();
		attrLong=0;	   
	}
	else
	{
		return -3;
	}


	if(this->certsList->GetCount() > 0)
	{
		this->certsList->SetSelection(0);
		wxCommandEvent event;
		this->OnListBoxChangeSelection(event);
	}

	return 0;
}

/**
metoda FillDetails() ustawia szczegoly certyfikatu wskazanego na liscie certyfikatow
@retval 0 jesli sukces operacji
@retval < 0 jesli blad
*/
long bmdCertDialog::FillDetails(void)
{
	long retVal=0;
	char *userTemp=NULL;
	char *organizationTemp=NULL;
	char *serialTemp=NULL;
	char *issuerTemp=NULL;
	GenBuf_t *tempCert=NULL;
	wxString validFromString;
	wxString validToString;
	

	if(this->certsList->GetSelection() == -1)
	{
		return -1;
	}

	this->ClearCertDetails();

	if(this->sourceType == bmdCertDialog::TYPE_PKCS11)
	{
		if(this->multiCardObjectPtr == NULL)
		{
			return -2;
		}
		if(this->multiCardObjectPtr->GetCertPtr(this->certsList->GetSelection(), &tempCert) != 0)
		{
			return -3;
		}

	}
	else if(this->sourceType == bmdCertDialog::TYPE_PFX)
	{
		if(this->fileCryptoInfoPtr == NULL)
		{
			return -4;
		}
		tempCert=this->fileCryptoInfoPtr->cert;
		
	}
	else
	{
		return -5;
	}

	retVal=bmdCertDialog::GetCertificateDetails(tempCert, &serialTemp, &issuerTemp, &userTemp, &organizationTemp, validFromString, validToString);
	if(retVal != 0)
	{
		
		tempCert=NULL;
		return -6;
	}
	
	this->SetSubjectValue(userTemp);
	this->SetOrganizationValue(organizationTemp);
	this->SetSerialValue(serialTemp);
	this->SetIssuerValue(issuerTemp);
	this->SetValidFromValue(&validFromString);
	this->SetValidToValue(&validToString);
	free(userTemp); userTemp=NULL;
	free(organizationTemp); organizationTemp=NULL;
	free(serialTemp); serialTemp=NULL;
	free(issuerTemp); issuerTemp=NULL;

	tempCert=NULL;
	return 0;
}

/**
metoda ClearCertDetails() czysci inforamcje w widoku szczegolow certyfikatu
*/
void bmdCertDialog::ClearCertDetails(void)
{
	this->SetSubjectValue(NULL);
	this->SetOrganizationValue(NULL);
	this->SetSerialValue(NULL);
	this->SetIssuerValue(NULL);
	this->SetValidFromValue((char*)NULL);
	this->SetValidToValue((char*)NULL);
}

/**
metoda SetIssuerValue() ustawia wystawce certyfikatu w widoku szczegolow certyfikatu
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetIssuerValue(char *issuer)
{
	wxString tempString;

	if(issuer == NULL)
	{
		this->issuerValue->SetLabel(wxEmptyString);
	}
	else
	{
		tempString=wxString(issuer, wxConvUTF8);
		this->issuerValue->SetLabel(tempString);
		this->issuerValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->issuerLabel->GetSize().GetWidth()-50);
		this->issuerValue->SetToolTip(tempString);
	}
}

/**
metoda SetSerialValue() ustawia numer seryjny certyfikatu w widoku szczegolow certyfikatu
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetSerialValue(char *serial)
{
	wxString tempString;

	if(serial == NULL)
	{
		this->serialValue->SetLabel(wxEmptyString);
	}
	else
	{
		tempString=wxString(serial, wxConvUTF8);
		this->serialValue->SetLabel(tempString);
		this->serialValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->serialLabel->GetSize().GetWidth()-50);
		this->serialValue->SetToolTip(tempString);
	}
}

/**
metoda SetOrganizationValue() ustawia nazwe organizacji dla podmiotu w widoku szczegolow certyfikatu
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetOrganizationValue(char *organization)
{
	wxString tempString;

	if(organization == NULL)
	{
		this->organizationValue->SetLabel(wxEmptyString);
	}
	else
	{
		tempString=wxString(organization, wxConvUTF8);
		this->organizationValue->SetLabel(tempString);
		this->organizationValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->organizationLabel->GetSize().GetWidth()-50);
		this->organizationValue->SetToolTip(tempString);
	}
}

/**
metoda SetSubjectValue() ustawia nazwe podmiotu w widoku szczegolow certyfikatu
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetSubjectValue(char *subject)
{
	wxString tempString;

	if(subject == NULL)
	{
		this->userValue->SetLabel(wxEmptyString);
	}
	else
	{
		tempString=wxString(subject, wxConvUTF8);
		this->userValue->SetLabel(tempString);
		this->userValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->userLabel->GetSize().GetWidth()-50);
		this->userValue->SetToolTip(tempString);
	}
}

/**
metoda SetValidFromValue() ustawia date waznosci (od) w widoku szczegolow certyfikatu
@arg validFrom to lancuch znakowy (char*) z data (od)
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetValidFromValue(char *validFrom)
{
	wxString tempString(validFrom, wxConvUTF8);
	this->SetValidFromValue(&tempString);
}

/**
metoda SetValidFromValue() ustawia date waznosci (od) w widoku szczegolow certyfikatu
@arg validFrom to wxString z data (od)
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetValidFromValue(wxString *validFrom)
{
	if(validFrom == NULL)
	{
		this->validFromValue->SetLabel(wxEmptyString);
	}
	else
	{
		this->validFromValue->SetLabel(*validFrom);
		this->validFromValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->validFromLabel->GetSize().GetWidth()-50);
		this->validFromValue->SetToolTip(*validFrom);
	}
}


/**
metoda SetValidToValue() ustawia date waznosci (do) w widoku szczegolow certyfikatu
@arg validTo to lancuch znakowy (char*) z data (do)
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetValidToValue(char *validTo)
{
	wxString tempString(validTo, wxConvUTF8);
	this->SetValidToValue(&tempString);
}

/**
metoda SetValidToValue() ustawia date waznosci (do) w widoku szczegolow certyfikatu
@arg validTo to wxString z data (do)
dla NULL ustawia lancuch pusty
*/
void bmdCertDialog::SetValidToValue(wxString *validTo)
{
	if(validTo == NULL)
	{
		this->validToValue->SetLabel(wxEmptyString);
	}
	else
	{
		this->validToValue->SetLabel(*validTo);
		this->validToValue->Wrap(this->infoStaticBox->GetSize().GetWidth()-this->validToLabel->GetSize().GetWidth()-50);
		this->validToValue->SetToolTip(*validTo);
	}
}

/**
metoda GetIssuerValue() zwraca wystawce certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetIssuerValue(void)
{
	return this->issuerValue->GetLabel();
}

/**
metoda GetSerialValue() zwraca numer seryjny certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetSerialValue(void)
{
	return this->serialValue->GetLabel();
}

/**
metoda GetOrganizationValue() zwraca organizacje dla podmiotu certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetOrganizationValue(void)
{
	return this->organizationValue->GetLabel();
}

/**
metoda GetSubjectValue() zwraca nazwe podmiotu certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetSubjectValue(void)
{
	return this->userValue->GetLabel();
}

/**
metoda GetValidFromValue() zwraca date waznosci (od) certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetValidFromValue(void)
{
	return this->validFromValue->GetLabel();
}

/**
metoda GetValidToValue() zwraca date waznosci (do) certyfikatu (wxString) z widoku szczegolow certyfikatu
*/
wxString bmdCertDialog::GetValidToValue(void)
{
	return this->validToValue->GetLabel();
}

/**
metoda GetChosenCertificateIndex() zwraca indeks wybranego certyfiaktu (-1 jesli nie wskazano certyfikatu)
*/
long bmdCertDialog::GetChosenCertificateIndex(void)
{
	return this->certsList->GetSelection();
}

/**
metoda EnableChooseButton() wyszarza/uaktywnia przycisk 'Wybierz' odpowiednio dla false/true
*/
void bmdCertDialog::EnableChooseButton(bool enable)
{
	this->buttonOk->Enable(enable);
}

/**
metoda OnButtonOK() to obsluga wyboru certyfikatu (przycisku 'Wybierz')
*/
void bmdCertDialog::OnButtonOK(wxCommandEvent &event)
{
	long retVal=0;
	char *pin=NULL;
	long pinLength=-1;
	long indexInContext=0;
	GenBuf_t *certFromCertsList=NULL;

	event; //zeby sie kompilator nie czepial
	
	if(this->sourceType == bmdCertDialog::TYPE_PFX)
	{
		this->EndModal(wxID_OK);
	}
	else if(this->sourceType == bmdCertDialog::TYPE_PKCS11)
	{
		//jesli certyfikat do podpisu, to istnieje zewnetrzny mechanizm podawania PIN i tworzenia kontekstu
		if(this->certForSigning == true) 
		{
			this->EndModal(wxID_OK);
			return;
		}

		this->EnableChooseButton(false);

		bmd_get_pin_dialog(&pin, &pinLength);
		if(pinLength == -1)
		{
			this->EnableChooseButton(true);
			return; //zostal przycisniety przycisk anuluj przy podawaniu numeru PIN
		}

		retVal=this->multiCardObjectPtr->GetContext(this->GetChosenCertificateIndex(), &(this->pkcs11Context));
		if(retVal != 0)
		{
			this->EnableChooseButton(true);
			memset(pin, 0, pinLength);
			free(pin); pin=NULL;
			pinLength=-1;
			wxMessageBox(_("Can't initialize session with smartcard."),_("Error"),wxICON_ERROR);
			this->EndModal(wxID_CANCEL);
			return;
		}
		
		retVal=this->multiCardObjectPtr->GetCertIndexInCTX(this->GetChosenCertificateIndex(), &indexInContext);
		if(retVal != 0)
		{
			this->EnableChooseButton(true);
			memset(pin, 0, pinLength);
			free(pin); pin=NULL;
			pinLength=-1;
			bmd_ctx_destroy(&(this->pkcs11Context));
			wxMessageBox(_("Can't find certificate on smartcard."),_("Error"),wxICON_ERROR);
			this->EndModal(wxID_CANCEL);
			return;
		}

		//to potrzebne, bowiem miedzy zbudowaniem okna a kliknieciem 'Wybierz' mogla zostac podmieniona karta
		if(bmdMultiCard::IsIndexPresentInPKCS11Context(indexInContext, this->pkcs11Context) != 0)
		{
			this->EnableChooseButton(true);
			memset(pin, 0, pinLength);
			free(pin); pin=NULL;
			pinLength=-1;
			bmd_ctx_destroy(&(this->pkcs11Context));
			wxMessageBox(_("Smartcard was replaced."),_("Error"),wxICON_ERROR);
			this->EndModal(wxID_CANCEL);
			return;
		}
		
		retVal=this->multiCardObjectPtr->GetCert(this->GetChosenCertificateIndex(), &certFromCertsList);
		if(retVal != 0)
		{
			this->EnableChooseButton(true);
			memset(pin, 0, pinLength);
			free(pin); pin=NULL;
			pinLength=-1;
			bmd_ctx_destroy(&(this->pkcs11Context));
			wxMessageBox(_("Smartcard was replaced."),_("Error"),wxICON_ERROR);
			this->EndModal(wxID_CANCEL);
			return;
		}

		if(this->pkcs11Context->pkcs11->all_certs[indexInContext]->gb_cert->size != certFromCertsList->size)
		{
			retVal=-1;
		}
		else
		{
			retVal=memcmp(this->pkcs11Context->pkcs11->all_certs[indexInContext]->gb_cert->buf, certFromCertsList->buf, certFromCertsList->size);
		}
		free_gen_buf(&certFromCertsList);
		if(retVal != 0)
		{
			this->EnableChooseButton(true);
			memset(pin, 0, pinLength);
			free(pin); pin=NULL;
			pinLength=-1;
			bmd_ctx_destroy(&(this->pkcs11Context));
			wxMessageBox(_("Smartcard was replaced."),_("Error"),wxICON_ERROR);
			this->EndModal(wxID_CANCEL);
			return; 
		}
		

		retVal=bmd_set_ctx_pkcs11_privkey(&(this->pkcs11Context), this->pkcs11Context->pkcs11->all_certs[indexInContext], pin, pinLength);
		if(retVal != BMD_OK) 
		{
			this->EnableChooseButton(true);
			bmd_ctx_destroy(&(this->pkcs11Context));
		}
		memset(pin, 0, pinLength);
		free(pin); pin=NULL;
		pinLength=-1;
		switch(retVal) {
			case BMD_ERR_CREDENTIALS:
			{
				wxMessageBox(_("Invalid PIN was entered"),_("Error"),wxICON_ERROR);
				return;
			}
			case BMD_PKCS11_PIN_LOCKED:
			{
				wxMessageBox(_("Chosen certificate is locked."),_("Error"),wxICON_ERROR);
				this->EndModal(wxID_CANCEL);
				return;
			}
			case BMD_ERR_OP_FAILED:
			{
				wxMessageBox(_("Error while getting private key"),_("Error"),wxICON_ERROR);
				this->EndModal(wxID_CANCEL);
				return;
			}
			case BMD_ERR_NODATA:
			{
				wxMessageBox(_("Cannot find any suitable private key"),_("Error"),wxICON_ERROR);
				this->EndModal(wxID_CANCEL);
				return;
			}
			case BMD_OK:
			{
				this->EndModal(wxID_OK);
				break;
			}
			default:
			{
				wxMessageBox(_("Internal error occured while getting private key"),_("Error"),wxICON_ERROR);
				this->EndModal(wxID_CANCEL);
				return;
			}
		}
	}
}

/**
metoda OnButtonCancel() to obsluga przycisku 'Anuluj'
*/
void bmdCertDialog::OnButtonCancel(wxCommandEvent &event)
{
	event; //zeby sie kompilator nie czepial
	wxMessageDialog confirmDlg(this,_("Are you sure you want to terminate operation?"),_("Question"), wxYES_NO|wxICON_QUESTION);

	if(confirmDlg.ShowModal() == wxID_YES)
	{
		this->EndModal(wxID_CANCEL);
	}
}

/**
metoda OnClose() to obsluga CloseBox'a
*/
void bmdCertDialog::OnClose(wxCloseEvent &event)
{
	event; //zeby sie kompilator nie czepial
	wxMessageDialog confirmDlg(this,_("Are you sure you want to terminate operation?"),_("Question"), wxYES_NO|wxICON_QUESTION);

	if(confirmDlg.ShowModal() == wxID_YES)
	{
		this->EndModal(wxID_CANCEL);
	}
}

/**
metoda OnListBoxChangeSelection() to obsluga zmiany zaznaczenia na liscie certyfikatow
*/
void bmdCertDialog::OnListBoxChangeSelection(wxCommandEvent &event)
{
	long retVal=0;
	event; //zeby sie kompilator nie czepial
	
	retVal=this->FillDetails();
	if(retVal != 0)
	{
		this->EnableChooseButton(false);
		return;
	}
	this->infoStaticSizer->Layout();
	
	if(this->signingCertVerificationConfig != NULL)
	{
		retVal=this->IsCertificateGoodForSigning();
		if(retVal != 0)
		{
			this->EnableChooseButton(false);
			if(this->signingCertVerificationConfig->debug_raport_error == 1)
			{
				wxMessageBox( wxString(_("Certificate verification error: ")) << retVal, _("Error"), wxICON_ERROR );
			}
			return;
		}
	}

	this->EnableChooseButton(true);
}

/**
metoda IsChooseButtonEnabled() zwraca true/false, jesli przycisk 'Wybierz' jest odpowiednio uaktywniony/wyszarzony
*/
bool bmdCertDialog::IsChooseButtonEnabled(void)
{
	return this->buttonOk->IsEnabled();
}

/**
metoda OnListBoxDoubleClick() realizuje obsluge dwukliku na elemencie listy certyfikatow
*/
void bmdCertDialog::OnListBoxDoubleClick(wxCommandEvent &event)
{
	wxCommandEvent myEvent;
	event; //zeby sie kompilator nie czepial

	if(this->certsList->GetSelection() >= 0 && this->IsChooseButtonEnabled())
	{
		this->OnButtonOK(myEvent);
	}
}

/*
metoda IsCertificateGoodForSigning() sprawdza, czy certyfikat jest prawidlowy do podpisu kwalifikowanego
@retval 0 jesli certyfikat nadaje sie do podpisu kwalifikowanego
@retval < 0 jesli jakis blad badz certyfikat nie spelnia wymogow dot. podpisu kwalifikowanego
*/
long bmdCertDialog::IsCertificateGoodForSigning(void)
{
	long retVal=0;
	GenBuf_t *tempCert=NULL;
	long longAttributeValue=0;
	char *charAttributeValue=NULL;

	if(this->certsList->GetSelection() == -1)
	{
		return -1;
	}

	if(this->sourceType == bmdCertDialog::TYPE_PKCS11)
	{
		if(this->multiCardObjectPtr == NULL)
		{
			return -2;
		}
		this->multiCardObjectPtr->GetCertPtr(this->certsList->GetSelection(), &tempCert);

	}
	else if(this->sourceType == bmdCertDialog::TYPE_PFX)
	{
		if(this->fileCryptoInfoPtr == NULL)
		{
			return -3;
		}
		tempCert=this->fileCryptoInfoPtr->cert;
		
	}
	else
	{
		return -4;
	}
	
	if(this->signingCertVerificationConfig == NULL)
	{
		return -5;
	}

	/*Sprawdzam rozszerzenia certyfikatu na potrzeby deklaracji zgodnosci*/
	if(this->signingCertVerificationConfig->check_certificate_extensions == 1)
	{
		retVal = CheckCertificateExtensionsInX509Certificate(tempCert, this->signingCertVerificationConfig);
		if(retVal != BMD_OK)
		{
			return retVal;
		}
	}

	/**************************************************************/
	/* sprawdzenie opcji non-repundation zaznaczonego certyfikatu */
	/**************************************************************/
	if(this->signingCertVerificationConfig->check_non_repudation == 1)
	{
		retVal = X509_is_non_repudiation(tempCert, this->signingCertVerificationConfig);
		if(retVal != BMD_OK )
		{
			return retVal;
		}
	}

	/***********************************************/
	/* sprawdzenie versji zaznaczonego certyfikatu */
	/***********************************************/
	if(this->signingCertVerificationConfig->check_certificate_version == 1)
	{
		retVal=X509CertGetAttr(tempCert,X509_ATTR_VERSION, &charAttributeValue, &longAttributeValue);
		if(retVal != BMD_OK)
		{
			return retVal;
		}

		if (longAttributeValue != 2)
		{
			free(charAttributeValue); charAttributeValue=NULL;
			longAttributeValue=0;
			return ERR_X509_WRONG_CERTIFICATE_VERSION;
		}
		free(charAttributeValue); charAttributeValue=NULL;
		longAttributeValue=0;
	}

	/**************************************************************/
	/* sprawdzenie algortymu podpisu dla zaznaczonego certyfikatu */
	/**************************************************************/
	if(this->signingCertVerificationConfig->check_certificate_sign_algorithm == 1)
	{
		retVal=X509CertGetAttr(tempCert,X509_ATTR_SIGALG_OID, &charAttributeValue, &longAttributeValue);	// sprawdzic trzeba jeszcze czy to ma wartosc... Sha-1WithRSAEncryption, Dsa-with-sha1, Ecdsa-with_sha1 
		if(retVal != BMD_OK)
		{
			return retVal;
		}
		
		if( (strcmp(charAttributeValue, OID_SHA1_WITH_RSA_STR) != 0) &&
			(strcmp(charAttributeValue, OID_DSA_WITH_SHA1_STR) != 0) &&
			(strcmp(charAttributeValue, OID_ECDSA_WITH_SHA1_STR) != 0) )
		{
			free(charAttributeValue); charAttributeValue=NULL;
			longAttributeValue=0;
			return ERR_X509_WRONG_CERTIFICATE_SIGNING_ALGHORITM;
		}
		free(charAttributeValue); charAttributeValue=NULL;
		longAttributeValue=0;
	}

	/**************************************************************/
	/* sprawdzenie algortymu klucza publicznego podmiotu */
	/**************************************************************/
	if(this->signingCertVerificationConfig->check_public_key_alghoritm == 1)
	{
		retVal=X509CertGetAttr(tempCert,X509_ATTR_SUBJECTPUBLICKEYINFO_ALG_OID,&charAttributeValue,&longAttributeValue);
		if(retVal != BMD_OK)
		{
			return retVal;
		}

		if( (strcmp(charAttributeValue, OID_RSA_ENCRYPTION_PUB_KEY_STR) !=0) &&
			(strcmp(charAttributeValue, OID_DSA_PUB_KEY_STR) != 0) ) 
		{
			free(charAttributeValue); charAttributeValue=NULL;
			longAttributeValue=0;
			return ERR_X509_WRONG_SUBJECT_PUBLIC_KEY_SIGNING_ALGHORITM;
		}
		free(charAttributeValue); charAttributeValue=NULL;
		longAttributeValue=0;
	}

	/*Sprawdzam wystawce certyfikatu na potrzeby deklaracji zgodnosci.*/
	if(this->signingCertVerificationConfig->chcek_certificate_issuer == 1)
	{
		retVal = CheckCertificateIssuer(tempCert, this->signingCertVerificationConfig);
		if(retVal != BMD_OK)
		{
			return retVal;
		}
	}

	/*Sprawdzenie istnienia pola podmiotu na potrzeby deklaracji zgodnosci
	Sprawdzam rowniez kategorie certyfikatu*/
	if(this->signingCertVerificationConfig->check_certificate_subject == 1)
	{
		retVal=CheckCertificateSubjectInX509Certificate(tempCert, &longAttributeValue, this->signingCertVerificationConfig);		
		if (retVal!=BMD_OK)
		{
			longAttributeValue=0;
			return retVal;
		}
		
		if(this->signingCertVerificationConfig->check_certificate_category == 1)
		{
			if (longAttributeValue <= 0 || longAttributeValue >= 3)
			{
				longAttributeValue = 0;
				return ERR_X509_WRONG_CERTIFICATE_CATEGORY;
			}
		}
		longAttributeValue = 0;
	}
	
	//
	// sprawdzenie okresu waznosci zaznaczonego certyfikatu
	//
	// TODO tutaj moze byc cos palniete z localami
	if(this->signingCertVerificationConfig->check_cert_validity_existence == 1)
	{
		time_t notBefore, notAfter;
		wxDateTime vFrom;
		wxDateTime vTo;
		wxDateTime now;
		retVal=bmd_get_validity_time_from_cert(tempCert,&notBefore,&notAfter);
		if(retVal != BMD_OK)
		{
			tempCert=NULL;
			return -6;
		}
		vFrom.Set(notBefore);
		vTo.Set(notAfter);
		now.Set(wxDateTime::GetTimeNow());
		if ( now.IsStrictlyBetween( vFrom, vTo ) == false)
		{
			tempCert=NULL;
			return -7;
		}
	}

	return 0;
}

/**
metoda GetPKCS11CtxForChosenCert() pozwala na jednokrotne pobranie kontekstu PKCS#11
(po pobraniu wskaznik na kontekst jest NULLowany wewnatrz obiektu, czyli po pobraniu kontekst nalezy samemu zniszczyc)
@retval 0 jesli sukces i przypisany kontekst (nie NULL)
@retval < 0 jesli blad
*/
long bmdCertDialog::GetPKCS11CtxForChosenCert(bmd_crypt_ctx_t **pkcs11Ctx)
{
	if(this->sourceType != bmdCertDialog::TYPE_PKCS11)
		{ return -1; }
	
	if(pkcs11Ctx == NULL)
		{ return -2; }
	if(*pkcs11Ctx != NULL)
		{ return -3; }

	if(this->pkcs11Context == NULL)
		{ return -4; }

	*pkcs11Ctx=this->pkcs11Context;
	this->pkcs11Context=NULL;
	return 0;
}

/**
metoda statyczna

metoda GetCertificateDetails() pobiera szczegoly certyfikatu
@retval 0 jesli sukces operacji
@retval < 0 jesli blad
*/
long bmdCertDialog::GetCertificateDetails(GenBuf_t *cert,char **serial,char **issuerCommonName,char **subjectCommonName,char **subjectOrganization, wxString &validFrom, wxString &validTo)
{
	long retVal=0;
	long attrLong=0;
	time_t notBefore;
	time_t notAfter;
	wxDateTime dateTime;
	wxString dateString;
	
	if(cert==NULL)
		{ return -1; }
	if(cert->buf == NULL || cert->size <= 0)
		{ return -2; }
	
	if(serial==NULL)
		{ return -3; }
	if((*serial)!=NULL)
		{ return -4; }
	if(issuerCommonName==NULL)
		{ return -5; }
	if((*issuerCommonName)!=NULL)
		{ return -6; }
	if(subjectCommonName==NULL)
		{ return -7; }
	if((*subjectCommonName)!=NULL)
		{ return -8; }
	if(subjectOrganization==NULL)
		{ return -9; }
	if((*subjectOrganization)!=NULL)
		{ return -10; }
	if(subjectOrganization==NULL)
		{ return -11; }
	if((*subjectOrganization)!=NULL)
		{ return -12; }
	
	// pobranie numeru seryjnego
	retVal=X509CertGetAttr(cert, X509_ATTR_SN, serial, &attrLong);
	if(retVal != BMD_OK)
	{
		return -13;
	}

	// pobranie commonname podmiotu
	attrLong=0;
	retVal=X509CertGetAttr(cert, X509_ATTR_SUBJECT_COMMONNAME, subjectCommonName, &attrLong);
	if(retVal != BMD_OK)
	{
		free(*serial); *serial=NULL;
		return -14;
	}
	
	// pobranie commonname wystawcy
	attrLong=0;
 	retVal=X509CertGetAttr(cert, X509_ATTR_ISSUER_COMMONNAME, issuerCommonName, &attrLong);
	if(retVal != BMD_OK)
	{
		free(*serial); *serial=NULL;
		free(*subjectCommonName); *subjectCommonName=NULL;
		return -15;
	}

	// pobranie organizacji podmiotu (tego atrybutu nie musi byc)
	attrLong=0;
	retVal=X509CertGetAttr(cert, X509_ATTR_SUBJECT_ORGANIZATION, subjectOrganization, &attrLong);
	
	// okres waznosci
	retVal=bmd_get_validity_time_from_cert(cert, &notBefore, &notAfter);
	if(retVal != BMD_OK)
	{
		free(*serial); *serial=NULL;
		free(*subjectCommonName); *subjectCommonName=NULL;
		free(*issuerCommonName); *issuerCommonName=NULL;
		free(*subjectOrganization); *subjectOrganization=NULL;
		return -16;
	}
	dateTime.Set(notBefore);
	dateString=dateTime.FormatISODate();
	dateString.Append(wxString(wxT(" ")));
	dateString.Append(dateTime.FormatISOTime());
	validFrom=dateString;

	dateTime.Set(notAfter);
	dateString=dateTime.FormatISODate();
	dateString.Append(wxString(wxT(" ")));
	dateString.Append(dateTime.FormatISOTime());
	validTo=dateString;

	return 0;
}
