#include "BuildLogFrame.h"

#include <wx/settings.h>
#include <wx/utils.h>

BuildProcess::BuildProcess(BuildLogFrame* log)
    : wxProcess(log)
    , log(log)
{
    Redirect();
}

void BuildProcess::ReadOutput()
{
    if (not isRunning)
        return;

    if (!IsRedirected())
        return;

    wxInputStream* out = GetInputStream();
    if (out)
    {
        wxTextInputStream textOut(*out);
        while (out->CanRead())
        {
            wxString line = textOut.ReadLine();
            if (!line.IsEmpty() && log)
                log->AppendLine(line, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        }
    }

    wxInputStream* err = GetErrorStream();
    if (err)
    {
        wxTextInputStream textErr(*err);
        while (err->CanRead())
        {
            wxString line = textErr.ReadLine();
            if (!line.IsEmpty() && log)
                log->AppendLine(line, *wxRED);
        }
    }
}

BuildLogFrame::BuildLogFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Build Log", wxDefaultPosition, wxSize(800, 600))
{
    textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxHSCROLL);
}

void BuildLogFrame::AppendLine(const wxString& line, const wxColour& colour)
{
    textCtrl->SetDefaultStyle(wxTextAttr(colour));
    textCtrl->AppendText(line + "\n");
}