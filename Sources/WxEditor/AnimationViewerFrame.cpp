#include "AnimationViewerFrame.h"

AnimationViewerFrame::AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    Init();
}

void AnimationViewerFrame::Init()
{
    wxInitAllImageHandlers();

    // === Główny splitter: lewo/prawo ===
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    // --- Canvas po lewej ---
    auto onStartupDone              = [this]() {};
    auto selectItemInGameObjectTree = [this](uint32 gameObjectId, bool select) {};

    m_canvas = new GLCanvas(mainSplitter, onStartupDone, selectItemInGameObjectTree);

    // --- Panel po prawej (lista animacji + potencjalnie inne widgety) ---
    wxPanel* rightPanel    = new wxPanel(mainSplitter);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(rightPanel, wxID_ANY, "Lista animacji:");
    rightSizer->Add(label, 0, wxALL, 5);

    m_animList = new wxListBox(rightPanel, wxID_ANY);
    rightSizer->Add(m_animList, 1, wxEXPAND | wxALL, 5);

    rightPanel->SetSizer(rightSizer);

    // --- Podział na lewo/prawo ---
    auto size = GetSize();
    mainSplitter->SplitVertically(m_canvas, rightPanel, 3 * size.x / 4);

    // === Status bar / menu (opcjonalnie) ===
    CreateStatusBar();
    SetStatusText("Animation Viewer ready");

    // przykładowe animacje
    m_animList->Append("Idle");
    m_animList->Append("Walk");
    m_animList->Append("Run");
    m_animList->Append("Jump");
}
