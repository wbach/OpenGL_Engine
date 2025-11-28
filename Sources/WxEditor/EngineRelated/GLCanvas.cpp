
#include "GLCanvas.h"

#include <GameEngine/Camera/CameraManager.h>
#include <GameEngine/Components/Camera/CameraComponent.h>
#include <GameEngine/Components/Lights/DirectionalLightComponent.h>
#include <GameEngine/DebugTools/EditorInterface/CameraEditor.h>
#include <GameEngine/DebugTools/MousePicker/DragObject.h>
#include <GameEngine/DebugTools/MousePicker/MousePicker.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Resources/Models/Primitive.h>
#include <Logger/Log.h>
#include <wx/dcclient.h>

#include <filesystem>
#include <memory>

#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"
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

// clang-format off
const int glAttributes[] = {
                            WX_GL_RGBA,
                            WX_GL_DOUBLEBUFFER,
                            WX_GL_DEPTH_SIZE,24,
                            WX_GL_CORE_PROFILE,
                            WX_GL_MAJOR_VERSION,4,
                            WX_GL_MINOR_VERSION,5,0
                        };
// clang-format on
}  // namespace WxEditor

GLCanvas::GLCanvas(wxWindow* parent, OnStartupDone onStartupDone, SelectItemInGameObjectTree callback, bool addStartupObjects)
    : wxGLCanvas(parent, wxID_ANY, WxEditor::glAttributes, wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS | wxTAB_TRAVERSAL)
    , context(nullptr)
    , renderTimer(this)
    , addStartupObjects{addStartupObjects}
    , onStartupDone{onStartupDone}
    , selectItemInGameObjectTree(callback)
{
    context = new wxGLContext(this);
    Bind(wxEVT_SIZE, &GLCanvas::OnSize, this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    renderTimer.Start(16);
}

GLCanvas::~GLCanvas()
{
    renderTimer.Stop();
    delete context;
}

vec3 GLCanvas::GetWorldPosFromCamera()
{
    auto scene = engine->GetSceneManager().GetActiveScene();
    if (not scene)
        return vec3(0.f);

    vec3 position(0.f);
    auto camera = GetScene().GetCameraManager().GetMainCamera();
    position    = camera->GetPosition();
    position += camera->GetDirection() * 5.f;
    return position;
}

void GLCanvas::addContextMenu(wxMouseEvent& event)
{
    wxMenu menu;
    // --- submenu Add Primitive ---
    wxMenu* primitiveMenu = new wxMenu();

    // dla każdego typu primitive tworzymy entry
    auto addPrimitiveItem = [&](PrimitiveType type, const wxString& label)
    {
        int id = wxWindow::NewControlId();
        primitiveMenu->Append(id, label);
        primitiveMenu->Bind(
            wxEVT_COMMAND_MENU_SELECTED,
            [=](wxCommandEvent&)
            {
                if (engine && engine->GetSceneManager().GetActiveScene())
                {
                    addPrimitive(type, GetWorldPosFromCamera());
                }
            },
            id);
    };

    // dodajemy wszystkie typy
    addPrimitiveItem(PrimitiveType::Cube, "Cube");
    addPrimitiveItem(PrimitiveType::Sphere, "Sphere");
    addPrimitiveItem(PrimitiveType::Cylinder, "Cylinder");
    addPrimitiveItem(PrimitiveType::Cone, "Cone");
    addPrimitiveItem(PrimitiveType::Plane, "Plane");
    addPrimitiveItem(PrimitiveType::Torus, "Torus");
    addPrimitiveItem(PrimitiveType::Pyramid, "Pyramid");
    addPrimitiveItem(PrimitiveType::IcoSphere, "IcoSphere");
    addPrimitiveItem(PrimitiveType::Triangle, "Triangle");

    // dodajemy submenu do głównego menu
    menu.AppendSubMenu(primitiveMenu, "Add Primitive");

    // --- opcja Reset Camera ---
    int ID_RESET_CAMERA = wxWindow::NewControlId();
    menu.Append(ID_RESET_CAMERA, "Reset Camera");
    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            if (engine && engine->GetSceneManager().GetActiveScene())
            {
                auto camera = GetScene().GetCameraManager().GetMainCamera();
                camera->SetPosition(cameraPositionStartup);
                camera->LookAt(cameraLookAtStartup);
                camera->UpdateMatrix();
            }
        },
        ID_RESET_CAMERA);
    // --- wyświetlenie ---
    PopupMenu(&menu, event.GetPosition());
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
        const auto& path = ProjectManager::GetInstance().GetLastOpenedScene();
        if (std::filesystem::exists(path))
        {
            LOG_DEBUG << "Last opened scene file exist: " << path;
            auto dlg = std::make_shared<LoadingDialog>(this, "Open scene", "Loading " + path.string());
            OpenScene(path,
                      [loadingDialog = dlg, this]
                      {
                          onStartupDone();
                          loadingDialog->Close();
                      });
            dlg->Show();
        }
        else
        {
            LOG_DEBUG << "Create new scene. Last opened scene file not exist: " << path;
            CreateNewScene();
        }
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

GameEngine::GameObject* GLCanvas::addPrimitive(GameEngine::PrimitiveType type, const vec3& pos, const vec3& scale)
{
    if (not engine->GetSceneManager().GetActiveScene())
        return nullptr;

    auto& resourceManager = GetScene().GetResourceManager();
    auto model            = resourceManager.GetPrimitives(type);
    auto obj              = GetScene().CreateGameObject(std::string(magic_enum::enum_name(type)));
    obj->AddComponent<GameEngine::Components::RendererComponent>().AddModel(model);
    obj->SetLocalPosition(pos);
    obj->SetLocalScale(scale);
    auto result = obj.get();
    GetScene().AddGameObject(std::move(obj));

    return result;
}

void GLCanvas::DisablePicker()
{
    useMousePicker = false;
}

void GLCanvas::EnablePicker()
{
    useMousePicker = true;
}

void GLCanvas::OnSize(wxSizeEvent& event)
{
    if (not engine)
        return;

    int w, h;
    GetClientSize(&w, &h);
    if (h <= 0)
        h = 1;

    SetCurrent(*context);

    vec2ui newSize{static_cast<uint32>(w), static_cast<uint32>(h)};
    LOG_DEBUG << "Window resized to: " << newSize;

    EngineConf.window.size         = newSize;
    EngineConf.renderer.resolution = newSize;

    Refresh(false);
    event.Skip();
}

void GLCanvas::SetCursorToCenter()
{
    int w, h;
    GetSize(&w, &h);
    wxPoint center(w / 2, h / 2);
    wxPoint screenPos = ClientToScreen(center);
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

    if (useMousePicker and engine->GetSceneManager().GetActiveScene() and not mousePicker)
    {
        mousePicker          = std::make_unique<GameEngine::MousePicker>(*GetScene().GetCameraManager().GetMainCamera());
        auto maybeGameObject = mousePicker->SelectObject(engine->GetEngineContext().GetInputManager().GetMousePosition(),
                                                         GetScene().GetGameObjects());
        if (maybeGameObject)
        {
            GameObjectSelectChange(*maybeGameObject);
            selectItemInGameObjectTree(maybeGameObject->GetId(), true);
        }
    }

    event.Skip();
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

    if (event.ShiftDown())
    {
        addContextMenu(event);
        return;
    }

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
        rendererComponent.AddModel(file.GetAbsolutePath().string());

        newGameObject->SetLocalPosition(GetWorldPosFromCamera());

        auto result = newGameObject->GetId();

        if (not parent)
        {
            scene->AddGameObject(std::move(newGameObject));
        }
        else
        {
            scene->AddGameObject(*parent, std::move(newGameObject));
        }
        return result;
    }
    return std::nullopt;
}

void GLCanvas::CreateNewScene()
{
    if (engine)
    {
        engine->GetSceneManager().SetActiveScene("NewScene");
        engine->GetSceneManager().SetOnSceneLoadDone(
            [this]()
            {
                LOG_DEBUG << "Load scene callback";
                onStartupDone();
                SetupCamera();

                if (addStartupObjects)
                {
                    addPrimitive(GameEngine::PrimitiveType::Plane, vec3(0.f, 0.f, 0.f), vec3(10.f, 1.f, 10.f));
                    addPrimitive(GameEngine::PrimitiveType::Cube, vec3(0.f, 1.0f, 0.f));

                    {
                        auto scene                 = engine->GetSceneManager().GetActiveScene();
                        auto defaultSceneCamera    = scene->CreateGameObject("DefaultCamera");
                        auto& cameraComponent      = defaultSceneCamera->AddComponent<Components::CameraComponent>();
                        cameraComponent.settings   = Components::CameraComponent::Settings::GlobalConfig;
                        cameraComponent.mainCamera = true;
                        defaultSceneCamera->SetWorldPosition(vec3(2, 2, 2));
                        cameraComponent.LookAt(vec3(0));
                        scene->AddGameObject(std::move(defaultSceneCamera));
                    }
                    {
                        auto scene            = engine->GetSceneManager().GetActiveScene();
                        auto directionLightGo = scene->CreateGameObject("DirectionLight");
                        directionLightGo->AddComponent<Components::DirectionalLightComponent>();
                        directionLightGo->SetWorldPosition(vec3(1000, 1500, 1000));
                        scene->AddGameObject(std::move(directionLightGo));
                    }
                }
            });
    }
}

bool GLCanvas::OpenScene(const GameEngine::File& file, std::function<void()> callback)
{
    if (not engine)
        return false;

    if (not std::filesystem::exists(file.GetAbsolutePath()))
    {
        return false;
    }
    cameraEditorPtr = nullptr;
    const auto name = file.GetBaseName();
    wxSceneFactory->AddScene(name, file);
    engine->GetSceneManager().SetOnSceneLoadDone(
        [this, callback]()
        {
            if (callback)
                callback();
            SetupCamera();
        });

    LOG_DEBUG << "SetActiveScene: " << name;
    engine->GetSceneManager().SetActiveScene(name);

    LOG_DEBUG << "Return : " << (bool)(engine->GetSceneManager().GetActiveScene() != nullptr);
    return engine->GetSceneManager().GetActiveScene() != nullptr;
}

void GLCanvas::GameObjectSelectChange(GameEngine::GameObject& gameObject)
{
    if (engine and engine->GetSceneManager().GetActiveScene())
    {
        dragGameObject = std::make_unique<GameEngine::DragObject>(*GetScene().getInputManager(), gameObject,
                                                                  *GetScene().GetCameraManager().GetMainCamera());
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

    auto& scene         = GetScene();
    auto& cameraManager = scene.GetCameraManager();
    cameraManager.LockAll();

    auto cameraEditor = std::make_unique<GameEngine::CameraEditor>(*scene.getInputManager(), *scene.getDisplayManager());

    cameraEditorPtr = cameraEditor.get();
    cameraEditor->SetPosition(cameraPositionStartup);
    cameraEditor->LookAt(cameraLookAtStartup);
    cameraEditor->UpdateMatrix();

    scene.getInputManager()->ShowCursor(true);
    scene.getInputManager()->SetReleativeMouseMode(false);

    auto id = cameraManager.AddCamera(std::move(cameraEditor));
    cameraManager.ActivateCamera(id);
    cameraManager.SetCameraAsMain(id);
    cameraManager.MainCameraCouldbeChange(false);
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
void GLCanvas::SetCameraStartupPosition(const vec3& position, const vec3& lookAt)
{
    cameraPositionStartup = position;
    cameraLookAtStartup   = lookAt;
}
