#pragma once

#include <GL/glew.h>
// clang-format off
#include <GameEngine/Components/Animation/Animator.h>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>
#include <GameEngine/Scene/ISceneStorage.h>

#include <wx/glcanvas.h>
#include <wx/timer.h>
// clang-format on

#include <memory>
#include <string>

#include "WxOpenGLApiWrapper.h"

namespace WxEditor
{
class WxWindowApi;
class WxEditorSceneFactory;
}  // namespace WxEditor

namespace GameEngine
{
class Engine;
}

class GLCanvas : public wxGLCanvas
{
public:
    GLCanvas(wxWindow* parent);
    ~GLCanvas();

    std::string getGlInfo() const;
    bool AddGameObject(const GameEngine::File&);
    void OpenScene(const GameEngine::File&);
    GameEngine::GameObject& GetRootObject();
    GameEngine::Engine& GetEngine();
    GameEngine::Scene& GetScene();

private:
    wxGLContext* context;
    wxTimer renderTimer;
    void SetCursorToCenter();

    void OnShow(wxShowEvent&);
    void OnPaint(wxPaintEvent&);
    void OnTimer(wxTimerEvent&);

    void OnKeyUp(wxKeyEvent&);
    void OnKeyDown(wxKeyEvent&);
    void OnChar(wxKeyEvent&);
    void OnMouseLeftUp(wxMouseEvent&);
    void OnMouseLeftDown(wxMouseEvent&);
    void OnMouseRightUp(wxMouseEvent&);
    void OnMouseRightDown(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent&);

    DECLARE_EVENT_TABLE()

    WxEditor::WxWindowApi* wxWindowApi{nullptr};
    WxEditor::WxEditorSceneFactory* wxSceneFactory{nullptr};
    std::unique_ptr<GameEngine::Engine> engine;
    std::unique_ptr<GameEngine::ISceneStorage> sceneCache;
};
