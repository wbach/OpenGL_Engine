#include "BuildComponentLogFrame.h"

#include <Types.h>
#include <wx/settings.h>
#include <wx/utils.h>

BuildComponentLogFrame::BuildComponentLogFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Build Log", wxDefaultPosition, wxSize(800, 600))
{
    textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxHSCROLL);
}

void BuildComponentLogFrame::AppendLine(const wxString& line, const wxColour& colour)
{
    textCtrl->SetDefaultStyle(wxTextAttr(colour));
    textCtrl->AppendText(line + "\n");
}

void BuildComponentLogFrame::Build()
{
}