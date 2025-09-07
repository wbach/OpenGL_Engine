#include <wx/frame.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <wx/txtstrm.h>

#include <mutex>

class BuildLogFrame : public wxFrame
{
public:
    BuildLogFrame(wxWindow* parent);
    void AppendLine(const wxString& line, const wxColour& colour = *wxBLACK);

private:
    wxTextCtrl* textCtrl;
};

class BuildProcess : public wxProcess
{
public:
    BuildProcess(BuildLogFrame* log);

    void ReadOutput();
    std::mutex& GetMutex()
    {
        return mutex;
    }
    void Stop()
    {
        isRunning = false;
    }

private:
    BuildLogFrame* log;
    std::mutex mutex;
    bool isRunning{true};
};
