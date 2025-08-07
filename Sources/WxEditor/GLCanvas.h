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
class DragObject;
class MousePicker;
}  // namespace GameEngine

class GLCanvas : public wxGLCanvas
{
public:
    using SelectItemInGameObjectTree = std::function<void(uint32, bool)>;
    using PrentWindow = wxWindow*;
    GLCanvas(PrentWindow, SelectItemInGameObjectTree);
    ~GLCanvas();

    std::string getGlInfo() const;
    bool AddGameObject(const GameEngine::File&, GameEngine::GameObject* = nullptr);
    bool OpenScene(const GameEngine::File&, std::function<void()>);
    void GameObjectSelectChange(GameEngine::GameObject&);
    void ResetDragObject();
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

    void SetupCamera();

    DECLARE_EVENT_TABLE()

    SelectItemInGameObjectTree selectItemInGameObjectTree;
    WxEditor::WxWindowApi* wxWindowApi{nullptr};
    WxEditor::WxEditorSceneFactory* wxSceneFactory{nullptr};
    std::unique_ptr<GameEngine::Engine> engine;
    std::unique_ptr<GameEngine::ISceneStorage> sceneCache;
    std::optional<IdType> cameraId;
    std::unique_ptr<GameEngine::DragObject> dragGameObject;
    std::unique_ptr<GameEngine::MousePicker> mousePicker;
};
