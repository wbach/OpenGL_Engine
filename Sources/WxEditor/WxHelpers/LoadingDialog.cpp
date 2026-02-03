#include "LoadingDialog.h"

namespace
{
wxPoint CalculateCenterPos(wxWindow* parent, const wxSize& dialogSize)
{
    if (!parent)
        return wxDefaultPosition;

    wxPoint parentPos = parent->GetScreenPosition();
    wxSize parentSize = parent->GetSize();

    int x = parentPos.x + (parentSize.GetWidth() - dialogSize.GetWidth()) / 2;
    int y = parentPos.y + (parentSize.GetHeight() - dialogSize.GetHeight()) / 2;

    return wxPoint(x, y);
}
const wxSize DIALOG_SIZE{250, 100};
}  // namespace

LoadingDialog::LoadingDialog(wxWindow* parent, const wxString& title, const wxString& message)
    : wxDialog(parent, wxID_ANY, title, CalculateCenterPos(parent, DIALOG_SIZE), DIALOG_SIZE,
               wxCAPTION | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    statusText  = new wxStaticText(this, wxID_ANY, message);
    sizer->Add(statusText, 0, wxALIGN_CENTER | wxALL, 10);

    gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(200, -1), wxGA_HORIZONTAL);
    sizer->Add(gauge, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit(sizer);
    timer.Bind(wxEVT_TIMER, &LoadingDialog::OnTimer, this);
    timer.Start(50);

    Show();
    Raise();
    Update();
    wxYield();
}
LoadingDialog::~LoadingDialog()
{
    timer.Stop();
}
void LoadingDialog::OnTimer(wxTimerEvent&)
{
    static int pos = 0;
    pos            = (pos + 3) % 100;
    gauge->SetValue(pos);
}
void LoadingDialog::UpdateMessage(const wxString& newMessage)
{
    if (statusText)
    {
        statusText->SetLabel(newMessage);
        this->Layout();
    }
}
