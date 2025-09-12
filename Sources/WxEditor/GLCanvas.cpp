
#include "GLCanvas.h"

#include <GameEngine/DebugTools/EditorInterface/CameraEditor.h>
#include <GameEngine/DebugTools/MousePicker/DragObject.h>
#include <GameEngine/DebugTools/MousePicker/MousePicker.h>
#include <wx/dcclient.h>

#include <filesystem>
#include <memory>

#include "Logger/Log.h"
#include "WxInputManager.h"
#include "WxKeyEventType.h"
#include "WxWindowApi.h"

// clang-format off
BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_SHOW(GLCanvas::OnShow)
EVT_TIMER(wxID_ANY, GLCanvas::OnTimer)
EVT_KEY_DOWN(GLCanvas::OnKeyDown)
EVT_KEY_UP(GLCanvas::OnKeyUp)
EVT_CHAR(GLCanvas::OnChar)
EVT_LEFT_DOWN(GLCanvas::OnMouseLeftDown)
EVT_LEFT_UP(GLCanvas::OnMouseLeftUp)
EVT_RIGHT_DOWN(GLCanvas::OnMouseRightDown)
EVT_RIGHT_UP(GLCanvas::OnMouseRightUp)
EVT_MOTION(GLCanvas::OnMouseMove)
END_EVENT_TABLE()
// clang-format on

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace WxEditor
{
class WxEditorScene : public GameEngine::Scene
{
public:
    WxEditorScene()
        : GameEngine::Scene("NewScene")
    {
    }
    ~WxEditorScene() override = default;
    int Initialize() override
    {
        camera.SetPosition(vec3(2, 2, 2));
        camera.LookAt(vec3(0, 0.5, 0));
        camera.UpdateMatrix();
        renderersManager_->GetDebugRenderer().Enable();

        return 0;
    }
    void PostInitialize() override
    {
    }
    int Update(float) override
    {
        return 0;
    }
};

class WxEditorSceneFactory : public GameEngine::SceneFactoryBase
{
public:
    WxEditorSceneFactory()
    {
        GameEngine::SceneFactoryBase::AddScene("NewScene", []() { return std::make_unique<WxEditorScene>(); });
    }
    void AddScene(const std::string& sceneName, const File& file)
    {
        GameEngine::SceneFactoryBase::AddScene(sceneName, file);
    }
};
const int glAttributes[] = {WX_GL_RGBA, WX_GL_MIN_RED,    1, WX_GL_MIN_GREEN,    1, WX_GL_MIN_BLUE,
                            1,          WX_GL_DEPTH_SIZE, 1, WX_GL_DOUBLEBUFFER, 0};
// int glAttributes[] = {
//     WX_GL_RGBA,
//     WX_GL_DOUBLEBUFFER,
//     WX_GL_DEPTH_SIZE, 16,  // <--- bufor głębokości
//     0
// };

}  // namespace WxEditor

GLCanvas::GLCanvas(wxWindow* parent, OnStartupDone onStartupDone, SelectItemInGameObjectTree callback)
    : wxGLCanvas(parent, wxID_ANY, WxEditor::glAttributes)
    , context(nullptr)
    , renderTimer(this)
    , onStartupDone{onStartupDone}
    , selectItemInGameObjectTree(callback)
{
    context = new wxGLContext(this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    renderTimer.Start(16);
}

GLCanvas::~GLCanvas()
{
    renderTimer.Stop();
    delete context;
}

void GLCanvas::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    SetCurrent(*context);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    if (!renderer)
    {
        wxLogError("OpenGL context failed!");
        return;
    }

    wxSize size = GetClientSize();
    //  Engine.
    if (not engine)
    {
        auto windowApiPtr         = std::make_unique<WxEditor::WxWindowApi>(vec2i{size.x, size.y},
                                                                    [&](int x, int y)
                                                                    {
                                                                        if (GetHandle())
                                                                        {
                                                                            WarpPointer(x, y);
                                                                        }
                                                                    });
        wxWindowApi               = windowApiPtr.get();
        auto wxEditorSceneFactory = std::make_unique<WxEditor::WxEditorSceneFactory>();
        wxSceneFactory            = wxEditorSceneFactory.get();

        engine = std::make_unique<GameEngine::Engine>(std::make_unique<Bullet::BulletAdapter>(), std::move(wxEditorSceneFactory),
                                                      std::make_unique<WxEditor::WxOpenGLApiWrapper>(std::move(windowApiPtr)));
        engine->Init();
        engine->GetSceneManager().SetActiveScene("NewScene");
        engine->GetSceneManager().SetOnSceneLoadDone(
            [this]()
            {
                onStartupDone();
                SetupCamera();
            });
    }
    if (engine)
    {
        engine->MainLoop();
    }

    if (dragGameObject)
    {
        dragGameObject->Update();
    }

    SwapBuffers();
}

void GLCanvas::SetCursorToCenter()
{
    // Rozmiar canvasu
    int w, h;
    GetSize(&w, &h);

    // Środek klienta
    wxPoint center(w / 2, h / 2);

    // Zamiana na współrzędne ekranowe
    wxPoint screenPos = ClientToScreen(center);

    // Przeniesienie kursora
    this->WarpPointer(screenPos.x, screenPos.y);
}

void GLCanvas::OnTimer(wxTimerEvent&)
{
    Refresh(false);
}

void GLCanvas::OnKeyUp(wxKeyEvent& event)
{
    if (not engine)
        return;
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, event.GetKeyCode());
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{event.GetKeyCode()}, false);
}

void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
    if (not engine)
        return;

    if (event.GetKeyCode() == 27)  // ESC
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Escape");
        GetParent()->SetFocus();
    }
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{event.GetKeyCode()}, true);
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, event.GetKeyCode());
}

void GLCanvas::OnChar(wxKeyEvent& evt)
{
}

void GLCanvas::OnMouseLeftUp(wxMouseEvent& event)
{
    if (not engine)
        return;

    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT);
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT}, false);
    mousePicker.reset();
    ResetDragObject();
}

void GLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
    if (not engine)
        return;

    SetFocus();
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT);
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT}, true);

    if (engine->GetSceneManager().GetActiveScene() and not mousePicker)
    {
        mousePicker          = std::make_unique<GameEngine::MousePicker>(GetScene().GetCamera(),
                                                                engine->GetEngineContext().GetRenderersManager().GetProjection());
        auto maybeGameObject = mousePicker->SelectObject(engine->GetEngineContext().GetInputManager().GetMousePosition(),
                                                         GetScene().GetGameObjects());
        if (maybeGameObject)
        {
            GameObjectSelectChange(*maybeGameObject);
            selectItemInGameObjectTree(maybeGameObject->GetId(), true);
        }
    }
}

void GLCanvas::OnMouseRightUp(wxMouseEvent& event)
{
    if (not engine)
        return;

    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT);
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT}, false);
}

void GLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    if (not engine)
        return;

    SetFocus();
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT);
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT}, true);
}

void GLCanvas::OnMouseMove(wxMouseEvent& evt)
{
    if (not engine)
        return;

    if (wxWindowApi)
    {
        wxWindowApi->OnMouseMove(evt);
    }
}

std::string GLCanvas::getGlInfo() const
{
    std::string ver(reinterpret_cast<char const*>(glGetString(GL_VERSION)));
    std::string glslver(reinterpret_cast<char const*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    return "GL version:" + ver + "\nGLSL: version" + glslver;
}

std::optional<IdType> GLCanvas::AddGameObject(const GameEngine::File& file, GameEngine::GameObject* parent)
{
    if (engine)
    {
        auto scene                        = engine->GetSceneManager().GetActiveScene();
        auto newGameObject                = scene->CreateGameObject(file.GetBaseName());
        auto& rendererComponent           = newGameObject->AddComponent<Components::RendererComponent>();
        auto& animator                    = newGameObject->AddComponent<Components::Animator>();
        animator.startupAnimationClipName = "noname";
        rendererComponent.AddModel(file.GetAbsolutePath());

        vec3 position(0.f);
        position = scene->GetCamera().GetPosition();
        position += scene->GetCamera().GetDirection() * 5.f;
        newGameObject->GetTransform().SetPosition(position);

        auto result = newGameObject->GetId();
        if (not parent)
        {
            scene->AddGameObject(std::move(newGameObject));
        }
        else
        {
            parent->AddChild(std::move(newGameObject));
        }
        return result;
    }
    return std::nullopt;
}

bool GLCanvas::OpenScene(const GameEngine::File& file, std::function<void()> callback)
{
    if (not engine)
        return false;

    if (not std::filesystem::exists(file.GetAbsolutePath()))
    {
        return false;
    }

    const auto name = file.GetBaseName();
    wxSceneFactory->AddScene(name, file);
    engine->GetSceneManager().SetOnSceneLoadDone(
        [this, callback]()
        {
            callback();
            SetupCamera();
        });
    engine->GetSceneManager().SetActiveScene(name);

    return engine->GetSceneManager().GetActiveScene() != nullptr;
}

void GLCanvas::GameObjectSelectChange(GameEngine::GameObject& gameObject)
{
    if (engine and engine->GetSceneManager().GetActiveScene())
    {
        dragGameObject =
            std::make_unique<GameEngine::DragObject>(*GetScene().getInputManager(), gameObject, GetScene().GetCamera(),
                                                     engine->GetEngineContext().GetRenderersManager().GetProjection());
    }
}

void GLCanvas::ResetDragObject()
{
    dragGameObject.reset();
}

void GLCanvas::SetupCamera()
{
    if (not engine)
        return;

    auto& scene = GetScene();
    scene.GetCamera().Lock();

    auto cameraEditor = std::make_unique<GameEngine::CameraEditor>(*scene.getInputManager(), *scene.getDisplayManager());
    cameraEditor->SetPosition(scene.GetCamera().GetPosition());
    cameraEditor->SetRotation(scene.GetCamera().GetRotation());
    scene.getInputManager()->ShowCursor(true);
    scene.getInputManager()->SetReleativeMouseMode(false);
    cameraId = scene.GetCamera().addAndSet(std::move(cameraEditor));
}

GameObject& GLCanvas::GetRootObject()
{
    if (not engine->GetSceneManager().GetActiveScene())
        LOG_ERROR << "No scene is set";

    return engine->GetSceneManager().GetActiveScene()->GetRootGameObject();
}

Engine& GLCanvas::GetEngine()
{
    if (not engine)
        LOG_ERROR << "Engine not created";

    return *engine;
}

Scene& GLCanvas::GetScene()
{
    return *engine->GetSceneManager().GetActiveScene();
}

void GLCanvas::OnShow(wxShowEvent&)
{
}
