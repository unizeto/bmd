/****************************

klasa bmdCountersSummaryDialog
autor: W³odzimierz Szczygie³ (Unizeto Technologies SA)

Klasa implementuje estetycznie wygladajace okno podsumowania (np. wysylki).
****************************/


#ifndef _HAVE_bmdCountersSummaryDialog_HEADER
#define _HAVE_bmdCountersSummaryDialog_HEADER


#include <bmd/libbmddialogs/libbmddialogs.h>
#include <wx/artprov.h>


class LIBBMDDIALOGS_SCOPE bmdCountersSummaryDialog : public wxDialog
{

public:
	bmdCountersSummaryDialog(wxWindow *parent, long icon, wxString caption=wxEmptyString);
	~bmdCountersSummaryDialog();
	
	long AddText(wxString text);
	long AddCounter(wxString counterText, long counterValue);
	long ShowModalDialog(void);


protected:
	DECLARE_EVENT_TABLE();


private:

	void OnButtonOK(wxCommandEvent &event);

	static enum
	{
		BUTTON_OK_ID=0
	};

	long sound;

	wxFlexGridSizer *mainFlexGridSizer;
		wxFlexGridSizer *upperFlexGridSizer;
			wxFlexGridSizer *infoFlexGridSizer;
				wxFlexGridSizer *singleFlexGridSizer;
				wxFlexGridSizer *countersFlexGridSizer;
	wxBoxSizer *buttonSizer;
		wxButton *buttonOk;
};


#endif //ifndef _HAVE_bmdCountersSummaryDialog_HEADER
