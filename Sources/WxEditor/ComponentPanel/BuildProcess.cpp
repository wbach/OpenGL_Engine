#include "BuildProcess.h"

#include <wx/settings.h>

#include "BuildComponentLogFrame.h"

BuildProcess::BuildProcess(BuildComponentLogFrame* log)
    : wxProcess(log)
    , log(log)
{
    Redirect();
}

void BuildProcess::ReadOutput()
{
    if (!isRunning)
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

std::mutex& BuildProcess::GetMutex()
{
    return mutex;
}
void BuildProcess::Stop()
{
    isRunning = false;
}
