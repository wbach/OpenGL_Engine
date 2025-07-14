
#include "GLCanvas.h"

#include <wx/dcclient.h>

#include <memory>

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
        : GameEngine::Scene("WxEditorScene")
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
        AddScene("WxEditorScene", []() { return std::make_unique<WxEditorScene>(); });
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

GLCanvas::GLCanvas(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, WxEditor::glAttributes)
    , context(nullptr)
    , renderTimer(this)
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

    wxSize size = GetClientSize();
    if (not engine)
    {
        auto windowApiPtr  = std::make_unique<WxEditor::WxWindowApi>(vec2i{size.x, size.y});
        wxWindowApi        = windowApiPtr.get();
        auto openglWrapper = std::make_unique<WxEditor::WxOpenGLApiWrapper>(std::move(windowApiPtr));

        engine =
            std::make_unique<GameEngine::Engine>(std::make_unique<Bullet::BulletAdapter>(),
                                                 std::make_unique<WxEditor::WxEditorSceneFactory>(), std::move(openglWrapper));
        engine->Init();
        engine->GetSceneManager().SetActiveScene("WxEditorScene");
    }
    if (engine)
    {
        engine->MainLoop();
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
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, event.GetKeyCode());
}

void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == 27) // ESC
    {
        DEBUG_LOG("Escape");
        GetParent()->SetFocus();
    }
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, event.GetKeyCode());
}

void GLCanvas::OnChar(wxKeyEvent& evt)
{
}

void GLCanvas::OnMouseLeftUp(wxMouseEvent& event)
{
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT);
}

void GLCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
    SetFocus();
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, WxEditor::WxKeySpecialKodes::WX_MOUSE_LEFT);
}

void GLCanvas::OnMouseRightUp(wxMouseEvent& event)
{
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_UP, WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT);
}

void GLCanvas::OnMouseRightDown(wxMouseEvent& event)
{
    SetFocus();
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, WxEditor::WxKeySpecialKodes::WX_MOUSE_RIGHT);
}

void GLCanvas::OnMouseMove(wxMouseEvent& evt)
{
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

bool GLCanvas::AddGameObject(const GameEngine::File& file)
{
    if (engine)
    {
        auto scene                         = engine->GetSceneManager().GetActiveScene();
        auto newGameObject                 = scene->CreateGameObject(file.GetBaseName());
        auto& rendererComponent            = newGameObject->AddComponent<Components::RendererComponent>();
        auto& animator                     = newGameObject->AddComponent<Components::Animator>();
        animator.startupAnimationClipName_ = "noname";
        rendererComponent.AddModel(file.GetAbsoultePath());
        scene->AddGameObject(std::move(newGameObject));
        return true;
    }
    return false;
}

void GLCanvas::OpenScene(const GameEngine::File& file)
{
    auto scene = engine->GetSceneManager().GetActiveScene();
    scene->ClearGameObjects();
    scene->LoadFromFile(file);
}

GameObject& GLCanvas::GetRootObject()
{
    return engine->GetSceneManager().GetActiveScene()->GetRootGameObject();
}

Engine &GLCanvas::GetEngine()
{
    return *engine;
}

Scene &GLCanvas::GetScene()
{
    return *engine->GetSceneManager().GetActiveScene();
}

void GLCanvas::OnShow(wxShowEvent&)
{
}
