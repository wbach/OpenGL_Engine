
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

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <filesystem>
#include <memory>

#include "WxEditor/Commands/AddObjectCommand.h"
#include "WxEditor/Commands/RemoveObjectCommand.h"
#include "WxEditor/Commands/TransformCommand.h"
#include "WxEditor/Commands/UndoManager.h"
#include "WxEditor/EngineRelated/NewScene.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"
#include "WxEditorSceneFactory.h"
#include "WxInputManager.h"
#include "WxKeyEventType.h"
#include "WxScenesDef.h"
#include "WxWindowApi.h"

// clang-format off
BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_IDLE(GLCanvas::OnIdle)
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
EVT_MOUSEWHEEL(GLCanvas::OnMouseWheel)
END_EVENT_TABLE()
// clang-format on

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace
{
wxGLAttributes GetWxGLAttributes()
{
    wxGLAttributes canvasAttrs;
    canvasAttrs.PlatformDefaults().RGBA().DoubleBuffer().Depth(24).Stencil(8).EndList();
    return canvasAttrs;
}
}  // namespace

GLCanvas::GLCanvas(wxWindow* parent, OnStartupDone onStartupDone, SelectItemInGameObjectTree callback,
                   const std::string& startupSceneName)
    : wxGLCanvas(parent, GetWxGLAttributes(), wxID_ANY, wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS)
    , context(nullptr)
    , startupSceneName{startupSceneName}
    , onStartupDone{onStartupDone}
    , selectItemInGameObjectTree(callback)
{
    wxGLContextAttrs contextAttrs;
    contextAttrs.CompatibilityProfile().MajorVersion(4).MinorVersion(5).EndList();
    context = new wxGLContext(this, nullptr, &contextAttrs);

    Bind(wxEVT_SIZE, &GLCanvas::OnSize, this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void GLCanvas::OnIdle(wxIdleEvent& event)
{
    EmergencyKeyRelease();

    if (not IsShownOnScreen() or not context)
        return;

    SetCurrent(*context);

    wxSize size = GetClientSize();
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

        auto dlg = std::make_shared<LoadingDialog>(this, "Open scene", "Loading " + startupSceneName);
        dlg->CenterOnParent();
        dlg->Show();
        engine->GetSceneManager().SetOnSceneLoadDone(
            [this, loadingDialog = dlg]()
            {
                SetupCamera();
                onStartupDone();
                this->CallAfter([loadingDialog]() { loadingDialog->Close(); });
            });

        if (std::filesystem::exists(startupSceneName) && std::filesystem::is_regular_file(startupSceneName))
        {
            GameEngine::File file{startupSceneName};
            const auto name = file.GetBaseName();
            wxSceneFactory->AddScene(name, file);
            engine->GetSceneManager().SetActiveScene(name);
        }
        else
        {
            engine->GetSceneManager().SetActiveScene(startupSceneName);
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

    event.RequestMore();
}

GLCanvas::~GLCanvas()
{
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
    AddGameObject(std::move(obj));

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

    auto keyCode = event.GetKeyCode();
    if (wxGetKeyState((wxKeyCode)keyCode))
    {
        return;
    }

    if (pressedKeys.count(keyCode) == 0)
    {
        return;
    }

    pressedKeys.erase(keyCode);

    auto& inputManager = engine->GetEngineContext().GetInputManager();
    LOG_DEBUG << "Send key up event";

    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, keyCode);
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{keyCode}, false);
}

void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
    auto keyCode = event.GetKeyCode();

    if (pressedKeys.contains(keyCode) > 0)
    {
        return;
    }
    pressedKeys.insert(keyCode);

    if (not engine)
        return;

    if (event.GetKeyCode() == 27)  // ESC
    {
        GetParent()->SetFocus();
    }
    wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{keyCode}, true);
    LOG_DEBUG << "Send key down event";
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, keyCode);
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
    // wxWindowApi->GetWxInputManager().SetKeyToBuffer(Input::KeyInteger{WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT}, true);
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

void GLCanvas::OnMouseWheel(wxMouseEvent& event)
{
    if (not engine)
        return;

    if (wxWindowApi)
    {
        int rotation = event.GetWheelRotation();
        int delta    = rotation / event.GetWheelDelta();

        auto& inputManager = engine->GetEngineContext().GetInputManager();
        if (delta > 0)
            inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, WxEditor::WxKeySpecialKodes::WX_MOUSE_MIDDLE);
        else
            inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, WxEditor::WxKeySpecialKodes::WX_MOUSE_MIDDLE);
    }
}

std::string GLCanvas::getGlInfo() const
{
    std::string ver(reinterpret_cast<char const*>(glGetString(GL_VERSION)));
    std::string glslver(reinterpret_cast<char const*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    return "GL version:" + ver + "\nGLSL: version" + glslver;
}

void GLCanvas::AddGameObject(std::unique_ptr<GameEngine::GameObject> go, GameEngine::GameObject* parent)
{
    if (auto scene = engine->GetSceneManager().GetActiveScene())
    {
        auto ptr = std::make_unique<AddObjectCommand>(*scene, *go);
        UndoManager::Get().Push(std::move(ptr));

        scene->AddGameObject(std::move(go), parent);
    }
}

void GLCanvas::RemoveGameObject(IdType gameObjectId)
{
    if (auto scene = engine->GetSceneManager().GetActiveScene())
    {
        if (auto go = scene->GetGameObject(gameObjectId))
        {
            auto ptr = std::make_unique<RemoveObjectCommand>(*scene, *go);
            UndoManager::Get().Push(std::move(ptr));
        }

        scene->RemoveGameObject(gameObjectId);
    }
}

void GLCanvas::CreateNewScene()
{
    if (engine)
    {
        LOG_DEBUG << "Creating new scene";
        UndoManager::Get().Clear();
        engine->GetSceneManager().SetActiveScene(WxEditor::NEW_SCENE);
        engine->GetSceneManager().SetOnSceneLoadDone(
            [this]()
            {
                LOG_DEBUG << "Load scene callback";
                SetupCamera();
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

    UndoManager::Get().Clear();
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

    engine->GetSceneManager().SetActiveScene(name);
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
    if (dragGameObject)
    {
        const auto& gameObject = dragGameObject->GetGameObject();
        auto cmd = std::make_unique<TransformCommand>(GetScene(), gameObject.GetId(), dragGameObject->GetObjectStartupTransfrom(),
                                                      gameObject.GetLocalTransform());
        UndoManager::Get().Push(std::move(cmd));
    }
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
GameEngine::CameraEditor* GLCanvas::GetCameraEditor()
{
    return cameraEditorPtr;
}

bool GLCanvas::IsSceneReady() const
{
    return cameraEditorPtr != nullptr;
}
void GLCanvas::EmergencyKeyRelease()
{
    if (not pressedKeys.empty())
    {
        auto it = pressedKeys.begin();
        while (it != pressedKeys.end())
        {
            int code = *it;
            if (not wxGetKeyState((wxKeyCode)code))
            {
                LOG_DEBUG << "OnIdle: Fixing lost key through artificial OnKeyUp: " << code;
                wxKeyEvent dummyEvent(wxEVT_KEY_UP);
                dummyEvent.m_keyCode = code;
                this->OnKeyUp(dummyEvent);
                it = pressedKeys.begin();
            }
            else
            {
                ++it;
            }
        }
    }
}
