
#include "App.h"
#include "MainFrame.h"

bool App::OnInit() {
    auto* frame = new MainFrame("wxWidgets + OpenGL Example", wxPoint(100, 100), wxSize(1920, 1080));
    frame->Show(true);
    return true;
}
