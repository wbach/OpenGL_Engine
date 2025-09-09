#pragma once
#include <Types.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include <memory>
#include <optional>
#include <string>

#include "GLCanvas.h"
#include "IdPool.h"
#include "Resources/File.h"

namespace GameEngine::Components
{
class Animator;
class RendererComponent;
}  // namespace GameEngine::Components

class AnimationViewerFrame : public wxFrame
{
public:
    AnimationViewerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void ShowModel(const GameEngine::File&);
    void SearchAndAddClipsFromDir(const std::string&);
    static bool CheckExtension(const GameEngine::File& file);
    bool isObjectSelcted() const;

private:
    void Init();
    void CreateMainMenu();
    void OnTimer(wxTimerEvent&);
    void OnAnimListContextMenu(wxContextMenuEvent& event);
    void OnExportToFile(wxCommandEvent& event);
    void OnExportAll(wxCommandEvent& event);
    void CreatePrefab(wxCommandEvent& event);
    void Clear();
    std::string GetStartupDialogPathBasedOnCurrent() const;

private:
    struct CurrentGameObject
    {
        std::optional<IdType> gameObjectId;
        std::optional<GameEngine::File> currentModelFile;
        GameEngine::Components::Animator& animator;
        GameEngine::Components::RendererComponent& rendererComponent;
    };

    GLCanvas* canvas;
    wxListBox* animList;
    std::optional<CurrentGameObject> currentGameObject;
    wxTimer* timer;

    bool isInit{false};
    std::optional<GameEngine::File> showModelAfterInit;
};
