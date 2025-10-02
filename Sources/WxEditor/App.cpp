
#include "App.h"

#include "AnimationViewerFrame.h"
#include "Logger/Log.h"
#include "MainFrame.h"
#include "StartupDialog.h"
#include "Utils.h"
#include "MaterialEditorFrame.h"

bool App::OnInit()
{
    auto args = Utils::parseArguments(argc, argv);

    if (Utils::GetValue(args, "animationViewer"))
    {
        CLogger::Instance().SetLogFilename("AnimationViewerLogs.txt");
        CLogger::Instance().EnableLogs();
        LOG_DEBUG << args;

        auto* frame = new AnimationViewerFrame("AnimationViewer", wxPoint(100, 100), wxSize(800, 600));
        SetTopWindow(frame);
        frame->Show(true);
        if (auto file = Utils::GetValue(args, "file"))
        {
            frame->ShowModel(*file);
        }
        return true;
    }

    if (Utils::GetValue(args, "materialEditor"))
    {
        CLogger::Instance().SetLogFilename("MaterialEditorLogs.txt");
        CLogger::Instance().EnableLogs();
        LOG_DEBUG << args;
        auto* frame = new MaterialEditorFrame("Material Editor", wxPoint(150, 150), wxSize(1000, 800));
        SetTopWindow(frame);
        frame->Show(true);
        if (auto file = Utils::GetValue(args, "file"))
        {
            frame->LoadMaterial(*file);
        }
        return true;
    }

    CLogger::Instance().SetLogFilename("WxEditorLogs.txt");
    auto* frame = new MainFrame("GameEngine", wxPoint(100, 100), wxSize(1920, 1080));
    SetTopWindow(frame);
    frame->Show(true);  // najpierw nie pokazujemy
    frame->Show(false);

    StartupDialog startup;
    if (startup.ShowModal() != wxID_OK)
        return false;

    frame->Init();
    frame->Show(true);
    frame->Raise();
    frame->SetFocus();

    return true;
}
