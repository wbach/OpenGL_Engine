#include "BuildComponentLogFrame.h"

#include <Types.h>
#include <wx/log.h>
#include <wx/settings.h>
#include <wx/utils.h>
#include <wx/wx.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "BuildProcess.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/ProjectPanel/ProjectPanel.h"

BuildComponentLogFrame::BuildComponentLogFrame(wxWindow* parent, ProjectPanel* projectPanel, ReloadComponents reloadComponents)
    : wxFrame(parent, wxID_ANY, "Build Log", wxDefaultPosition, wxSize(800, 600))
    , projectPanel{projectPanel}
    , reloadComponents{reloadComponents}
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
    auto buildDir          = ProjectManager::GetInstance().GetProjectPath() / "build";
    auto engineIncludesDir = ProjectManager::GetInstance().GetEngineIncludesDir();

    if (not std::filesystem::exists(buildDir))
    {
        Utils::CreateDirectories(buildDir);
    }
    if (engineIncludesDir.empty())
    {
        wxLogMessage("engineIncludesDir is empty, check preferences settings. Should be path to engine repo");
        return;
    }

    LOG_DEBUG << "buildDir : " << buildDir;
    LOG_DEBUG << "engineIncludesDir : " << engineIncludesDir;

    Show();

    AppendLine("BuildDir: " + buildDir.string(), *wxLIGHT_GREY);
    AppendLine("EngineIncludesDir: " + engineIncludesDir.string(), *wxLIGHT_GREY);

    wxExecuteEnv env;
    env.cwd = buildDir.string();

    auto* processCmake = new BuildProcess(this);

    Bind(wxEVT_IDLE,
         [processCmake](wxIdleEvent& evt)
         {
             std::lock_guard<std::mutex> lock(processCmake->GetMutex());
             if (processCmake->IsRedirected())
                 processCmake->ReadOutput();
             evt.Skip();
         });

    Bind(wxEVT_END_PROCESS,
         [=](wxProcessEvent& evt)
         {
             std::lock_guard<std::mutex> lock(processCmake->GetMutex());
             processCmake->ReadOutput();
             processCmake->Stop();
             projectPanel->RefreshAll();

             int exitCode = evt.GetExitCode();

             if (exitCode == 0)
             {
                 AppendLine("? Build finished successfully.", *wxGREEN);
             }
             else
             {
                 AppendLine("? Build finished with errors.", *wxRED);
             }

             projectPanel->RefreshAll();

             CallAfter(
                 [this, exitCode]()
                 {
                     wxString msg =
                         (exitCode == 0)
                             ? "Components build finished successfully.\nDo you want to reload the libraries?"
                             : "Components build finished, but some targets failed.\nDo you still want to reload the libraries?";

                     int answer = wxMessageBox(msg, "Build Finished", wxYES_NO | wxICON_QUESTION, this);

                     if (answer == wxYES)
                     {
                         if (reloadComponents)
                         {
                             LOG_DEBUG << "Reloading components...";
                             reloadComponents();
                         }
                     }
                 });
         });

    std::string cmd;
#if defined(_MSC_VER)
    // MSVC / Windows
    std::filesystem::path solutionFile =
        ProjectManager::GetInstance().GetProjectPath() / ProjectManager::GetInstance().GetProjectName() / ".sln";

    std::string vcvars = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\"";

    cmd = "cmd /c call " + vcvars + " && msbuild " + solutionFile.string() + " /t:Build /p:Configuration=Release /p:Platform=x64";
#else
    cmd = "sh -c \"cmake .. -DCOMPONENTS_DIR=Data/Components -DENGINE_INCLUDE_DIR=" + engineIncludesDir.string() +
          " && cmake --build .\"";
#endif

    RunCommand(cmd, buildDir, processCmake);
}

void BuildComponentLogFrame::RunCommand(const std::string& cmd, const std::filesystem::path& workDir, wxProcess* process)
{
    wxExecuteEnv env;
    env.cwd = workDir.string();

    long pid = wxExecute(cmd, wxEXEC_ASYNC, process, &env);
    if (pid == -1)
    {
        wxLogError("Failed to run command: %s", cmd);
    }
}