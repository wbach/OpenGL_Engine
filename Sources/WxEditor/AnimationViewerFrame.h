#pragma once
#include <Types.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include <memory>
#include <optional>

#include "GLCanvas.h"
#include "IdPool.h"
#include "Resources/File.h"

namespace GameEngine::Components
{
class Animator;
}

class AnimationViewerFrame : public wxFrame
{
public:
    AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void ShowModel(const GameEngine::File&);
    static bool CheckExtension(const GameEngine::File& file);

private:
    void Init();
    void FindAllAnimationsInFolder(GameEngine::Components::Animator&, const std::string&);
    void OnTimer(wxTimerEvent&);

private:
    GLCanvas* canvas;
    wxListBox* animList;
    std::optional<IdType> gameObjectId;
    wxTimer* timer;
    wxStaticText* animationsSearchFolderPath;

    bool isInit{false};
    std::optional<GameEngine::File> showModelAfterInit;
    std::optional<GameEngine::File> currentModelFile;
};
