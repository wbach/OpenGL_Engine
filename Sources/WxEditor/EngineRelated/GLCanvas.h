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

#include "Camera/ICamera.h"
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
    using OnStartupDone              = std::function<void()>;
    using PrentWindow                = wxWindow*;
    GLCanvas(PrentWindow, OnStartupDone, SelectItemInGameObjectTree, bool addStartupObjects = true);
    ~GLCanvas();

    void CreateNewScene();
    bool OpenScene(const GameEngine::File&, std::function<void()>);
    std::string getGlInfo() const;
    std::optional<IdType> AddGameObject(const GameEngine::File&, GameEngine::GameObject* = nullptr);
    void GameObjectSelectChange(GameEngine::GameObject&);
    void ResetDragObject();
    GameEngine::GameObject& GetRootObject();
    GameEngine::Engine& GetEngine();
    GameEngine::Scene& GetScene();
    GameEngine::GameObject* addPrimitive(GameEngine::PrimitiveType, const vec3& pos = vec3(0.f), const vec3& scale = vec3(1.f));
    void DisablePicker();
    void EnablePicker();
    vec3 GetWorldPosFromCamera();

private:
    wxGLContext* context;
    wxTimer renderTimer;
    void SetCursorToCenter();

    void addContextMenu(wxMouseEvent&);
    void OnShow(wxShowEvent&);
    void OnPaint(wxPaintEvent&);
    void OnTimer(wxTimerEvent&);
    void OnSize(wxSizeEvent&);

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

    bool useMousePicker{true};
    bool addStartupObjects;
    GameEngine::ICamera* cameraEditorPtr{nullptr};
    OnStartupDone onStartupDone;
    SelectItemInGameObjectTree selectItemInGameObjectTree;
    WxEditor::WxWindowApi* wxWindowApi{nullptr};
    WxEditor::WxEditorSceneFactory* wxSceneFactory{nullptr};
    std::unique_ptr<GameEngine::Engine> engine;
    std::unique_ptr<GameEngine::ISceneStorage> sceneCache;
    std::optional<IdType> cameraId;
    std::unique_ptr<GameEngine::DragObject> dragGameObject;
    std::unique_ptr<GameEngine::MousePicker> mousePicker;
};
