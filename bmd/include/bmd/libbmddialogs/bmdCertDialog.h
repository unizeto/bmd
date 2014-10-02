/****************************

klasa bmdCertDialog
autor: W³odzimierz Szczygie³ (Unizeto Technologies SA)

****************************/


#ifndef _HAVE_bmdCertDialog_HEADER
#define _HAVE_bmdCertdialog_HEADER

#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdasn1_common/CommonUtils.h> //oidy dla algorytmu podpisu itp.
#include "../klient/bmdMultiCard.h"

class LIBBMDDIALOGS_SCOPE bmdCertDialog : public wxDialog
{
public:
	bmdCertDialog(wxWindow *parent, file_crypto_info_t* fileCryptoInfo); //dla pliku PKCS#12
	bmdCertDialog(wxWindow *parent, bmdMultiCard_t *multiCardObject, bool certificateForSigning, CertExtConfig_t *signingCertVerificationConfig=NULL); //dla PKCS#11
	~bmdCertDialog();

	long ShowCertDialog(void);
	long GetChosenCertificateIndex(void);
	long GetPKCS11CtxForChosenCert(bmd_crypt_ctx_t **pkcs11Context);
	long IsCertificateGoodForSigning(void);
	void EnableChooseButton(bool enable);
	bool IsChooseButtonEnabled(void);

	wxString GetIssuerValue(void);
	wxString GetSerialValue(void);
	wxString GetOrganizationValue(void);
	wxString GetSubjectValue(void);
	wxString GetValidFromValue(void);
	wxString GetValidToValue(void);

	static long GetCertificateDetails(GenBuf_t *cert,char **serial,char **issuerCommonName,char **subjectCommonName,char **subjectOrganization, wxString &validFrom, wxString &validTo);
	
	static enum 
	{
		TYPE_PKCS11=0,
		TYPE_PFX
	};

	static enum 
	{
		BUTTON_OK_ID=0,
		BUTTON_CANCEL_ID,
		CERTS_LIST_ID
	};
	wxString GetUser() {if (userValue) return userValue->GetLabel();return wxEmptyString;}


protected:
		DECLARE_EVENT_TABLE();


private:
	//metody
	void BuildDialog(void);
	long FillList(void);
	long FillDetails(void);

	void OnButtonOK(wxCommandEvent &event);
	void OnButtonCancel(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnListBoxChangeSelection(wxCommandEvent &event);
	void OnListBoxDoubleClick(wxCommandEvent &event);

	void SetIssuerValue(char *issuer);
	void SetSerialValue(char *serial);
	void SetOrganizationValue(char *organization);
	void SetSubjectValue(char *subject);
	void SetValidFromValue(char *validFrom);
	void SetValidFromValue(wxString *validFrom);
	void SetValidToValue(char *validTo);
	void SetValidToValue(wxString *validTo);
	void ClearCertDetails(void);


	//pola
	long sourceType;
	bmdMultiCard_t *multiCardObjectPtr;
	file_crypto_info_t *fileCryptoInfoPtr;
	bmd_crypt_ctx_t *pkcs11Context;
	CertExtConfig_t *signingCertVerificationConfig; //jesli NULL, to sprawdzanie certyfikatu podpisujacego wylaczone
	bool certForSigning; //jesli true, to wybrany certyfikat bedzie przeznaczony do podpisu
	
	wxFlexGridSizer *mainFlexSizer;
		wxStaticBoxSizer *certsListStaticSizer;
			wxStaticBox *certListStaticBox;
				wxListBox *certsList;
		wxStaticBoxSizer *infoStaticSizer;
			wxStaticBox *infoStaticBox;
				wxFlexGridSizer *infoFlexSizer;
					wxStaticText *userLabel;
					wxStaticText *userValue;
					wxStaticText *organizationLabel;
					wxStaticText *organizationValue;
					wxStaticText *serialLabel;
					wxStaticText *serialValue;
					wxStaticText *issuerLabel;
					wxStaticText *issuerValue;
					wxStaticText *validFromLabel;
					wxStaticText *validFromValue;
					wxStaticText *validToLabel;
					wxStaticText *validToValue;
		wxBoxSizer *buttonsSizer;
			wxButton *buttonCancel;
			wxButton *buttonOk;
	
};


#endif //ifndef _HAVE_bmdCertDialog_HEADER

