#ifdef WIN32
#include <winsock2.h>
#endif
#define _WINSOCK2API_
/****************************

klasa bmdCountersSummaryDialog
autor: W³odzimierz Szczygie³ (Unizeto Technologies SA)

Klasa implementuje estetycznie wygladajace okno podsumowania (np. wysylki).
****************************/


#include <bmd/libbmddialogs/bmdCountersSummaryDialog.h>


BEGIN_EVENT_TABLE(bmdCountersSummaryDialog, wxDialog)
	EVT_BUTTON(bmdCountersSummaryDialog::BUTTON_OK_ID, bmdCountersSummaryDialog::OnButtonOK)
END_EVENT_TABLE()


/**
konstruktor klasy bmdCountersSummaryDialog
@param icon okresla, jaka ikona ma byc wyswietlona w podsumowaniu - przyjmuje nasstepujace wartosci:
wxICON_ERROR
wxICON_WARNING
wxICON_INFORMATION
*/
bmdCountersSummaryDialog::bmdCountersSummaryDialog(wxWindow *parent, long icon, wxString caption) :
	wxDialog(parent, wxID_ANY, caption, wxDefaultPosition, wxSize(250, 100), wxCAPTION|wxCLOSE_BOX)
{
	wxBitmap bitmap;
	wxStaticBitmap *staticBitmap=NULL;

	switch(icon)
	{
		case wxICON_ERROR:
		{
			bitmap=wxArtProvider::GetBitmap(wxART_ERROR);
			this->sound=MB_ICONHAND;
			break;
		}
		case wxICON_WARNING:
		{
			bitmap=wxArtProvider::GetBitmap(wxART_WARNING);
			this->sound=MB_ICONEXCLAMATION;
			break;
		}
		default:
		{
			bitmap=wxArtProvider::GetBitmap(wxART_INFORMATION);
			this->sound=MB_ICONEXCLAMATION;
		}
	}
	staticBitmap=new wxStaticBitmap(this, wxID_ANY, bitmap);


	this->mainFlexGridSizer=new wxFlexGridSizer(2, 1, 5, 0);
	this->mainFlexGridSizer->AddGrowableRow(0);
	this->mainFlexGridSizer->AddGrowableCol(0);
	this->SetSizer(this->mainFlexGridSizer);

	this->upperFlexGridSizer=new wxFlexGridSizer(1, 3, 0, 10);
	this->mainFlexGridSizer->Add(this->upperFlexGridSizer, 1, wxEXPAND|wxALL, 10);
	
	this->upperFlexGridSizer->Add(staticBitmap, 0, wxALL, 5);

	this->infoFlexGridSizer=new wxFlexGridSizer(2, 1, 0, 0);
	this->upperFlexGridSizer->Add(this->infoFlexGridSizer, 1, wxEXPAND|wxTOP, 5);
	this->upperFlexGridSizer->AddSpacer(10);
	
	this->singleFlexGridSizer=new wxFlexGridSizer(0, 1, 0, 0);
	this->infoFlexGridSizer->Add(this->singleFlexGridSizer, 1, wxEXPAND);
	this->countersFlexGridSizer=new wxFlexGridSizer(0, 2, 0, 20);
	this->countersFlexGridSizer->AddGrowableCol(1);
	this->infoFlexGridSizer->Add(this->countersFlexGridSizer, 1, wxEXPAND);
	
	this->buttonOk=new wxButton(this, bmdCountersSummaryDialog::BUTTON_OK_ID, _("Ok"));
	this->buttonSizer=new wxBoxSizer(wxHORIZONTAL);
	this->buttonSizer->AddStretchSpacer();
	this->buttonSizer->Add(this->buttonOk, 0 , wxALIGN_CENTER);
	this->buttonSizer->AddStretchSpacer();

	this->mainFlexGridSizer->Add(this->buttonSizer, 1, wxEXPAND|wxBOTTOM, 15);
}

/**
destruktor klasy bmdCountersSummaryDialog
*/
bmdCountersSummaryDialog::~bmdCountersSummaryDialog()
{

}

/**
metoda AddText() pozwala dodac pojedyncza linie tesktu (zwykly tekst)
Moze byc stosowana naprzemiennie z metoda AddCounter().
Zwykly tekst i tak jest grupowany w pierwszej czesci okienka.
*/
long bmdCountersSummaryDialog::AddText(wxString text)
{
	wxStaticText *staticText=new wxStaticText(this, wxID_ANY, text);

	this->singleFlexGridSizer->SetRows(this->singleFlexGridSizer->GetRows() + 1);
	this->singleFlexGridSizer->Add(staticText, 1, wxEXPAND);

	staticText=NULL;
	return 0;
}

/**
metoda AddCounter() pozwala dadac informacje o stanie jakiejs zmiennej/licznika.
Tekst typu licznik sklada sie z opisu i wartosci. zieki tej mtodzie
nie trzeba jawnie podawac tabulacji/przesuniec, bowiem implementacja klasy automatycznie wykona
potrzebne wyrownania tak, by okno wygladalo estetycznie.
Metoda moze byc wywolywana naprzemienne z AddText().
Tekst typu licznik i tak jest grupowany w drugiej czesci okna.
*/
long bmdCountersSummaryDialog::AddCounter(wxString counterText, long counterValue)
{
	wxStaticText *textStaticText=new wxStaticText(this, wxID_ANY, counterText);
	wxStaticText *valueStaticText=new wxStaticText(this, wxID_ANY, wxString::Format(wxT("%li"), counterValue));

	this->countersFlexGridSizer->SetRows(this->countersFlexGridSizer->GetRows() + 1);
	this->countersFlexGridSizer->Add(textStaticText);
	this->countersFlexGridSizer->Add(valueStaticText, 0 , wxALIGN_RIGHT);

	textStaticText=NULL;
	valueStaticText=NULL;
	return 0;
}

/**
metoda OnButtonOk() jest obsluga akcji wcisniecia przycisku Ok (zamyka okno)
*/
void bmdCountersSummaryDialog::OnButtonOK(wxCommandEvent &event)
{
	event; //zeby sie kompilator nie czepial
	this->EndModal(wxOK);
}

/**
metoda ShowModalDialog() powinna byc uzywana zamiast ShowModal() do wyswietlenia okna,
bowiem zapewnia prawidlowe rysowanie okna, odgrywa dzwiek przy jego rysowaniu oraz centruje na rodzicu.
*/
long bmdCountersSummaryDialog::ShowModalDialog(void)
{
	this->GetSizer()->Fit(this);
	this->Layout();
	this->Center();
	MessageBeep(this->sound);
	return this->ShowModal();
}
