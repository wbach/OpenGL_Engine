
#include "App.h"

#include "MainFrame.h"
#include "StartupDialog.h"

bool App::OnInit()
{
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
