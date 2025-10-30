#pragma once
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/txtstrm.h>

class BuildComponentLogFrame : public wxFrame
{
public:
    BuildComponentLogFrame(wxWindow* parent);
    void Build();
    void AppendLine(const wxString& line, const wxColour& colour = *wxBLACK);

private:
    wxTextCtrl* textCtrl;
};
