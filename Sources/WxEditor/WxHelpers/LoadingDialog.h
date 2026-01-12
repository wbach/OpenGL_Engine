#pragma once
#include <wx/wx.h>

class LoadingDialog : public wxDialog
{
public:
    LoadingDialog(wxWindow*, const wxString&, const wxString&);
    ~LoadingDialog() override;

private:
    void OnTimer(wxTimerEvent&);

private:
    wxGauge* gauge{nullptr};
    wxTimer timer;
};
