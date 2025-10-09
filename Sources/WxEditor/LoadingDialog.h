#pragma once
#include <wx/wx.h>

class LoadingDialog : public wxDialog
{
public:
    LoadingDialog(wxWindow* parent, const wxString& title, const wxString& message)
        : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(250, 100), wxCAPTION | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR)
    {
        auto* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, message), 0, wxALIGN_CENTER | wxALL, 10);

        gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(200, -1), wxGA_HORIZONTAL);
        sizer->Add(gauge, 0, wxALIGN_CENTER | wxALL, 10);

        SetSizerAndFit(sizer);

        // uruchamiamy timer do animacji
        timer.Bind(wxEVT_TIMER, &LoadingDialog::OnTimer, this);
        timer.Start(50);  // 20 FPS
    }

    ~LoadingDialog() override
    {
        timer.Stop();
    }

private:
    wxGauge* gauge{nullptr};
    wxTimer timer;

    void OnTimer(wxTimerEvent&)
    {
        static int pos = 0;
        pos            = (pos + 3) % 100;
        gauge->SetValue(pos);
    }
};
