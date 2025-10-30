#pragma once
#include <wx/frame.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/txtstrm.h>

#include <mutex>

class BuildComponentLogFrame : public wxFrame
{
public:
    BuildComponentLogFrame(wxWindow* parent);
    void AppendLine(const wxString& line, const wxColour& colour = *wxBLACK);

private:
    wxTextCtrl* textCtrl;
};

class BuildProcess : public wxProcess
{
public:
    BuildProcess(BuildComponentLogFrame* log);

    void ReadOutput();
    std::mutex& GetMutex();
    void Stop();

private:
    BuildComponentLogFrame* log;
    std::mutex mutex;
    bool isRunning{true};
};
