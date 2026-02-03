#pragma once
#include <wx/wx.h>

class LoadingDialog : public wxDialog
{
public:
    LoadingDialog(wxWindow*, const wxString&, const wxString&);
    ~LoadingDialog() override;

    void UpdateMessage(const wxString&);

private:
    void OnTimer(wxTimerEvent&);

private:
    wxGauge* gauge{nullptr};
    wxTimer timer;
    wxStaticText* statusText;
};
