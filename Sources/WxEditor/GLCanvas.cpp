
#include "GLCanvas.h"

#include <wx/dcclient.h>

#include <memory>

// clang-format off
BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_SHOW(GLCanvas::OnShow)
EVT_TIMER(wxID_ANY, GLCanvas::OnTimer)
EVT_KEY_DOWN(GLCanvas::OnKeyDown)
EVT_KEY_UP(GLCanvas::OnKeyUp)
EVT_CHAR(GLCanvas::OnChar)
EVT_LEFT_DOWN(GLCanvas::OnMouseDown)
EVT_LEFT_UP(GLCanvas::OnMouseUp)
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

        // RunNetworkEditorInterface();
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

    if (not engine)
    {
        engine = std::make_unique<GameEngine::Engine>(std::make_unique<Bullet::BulletAdapter>(),
                                                      std::make_unique<WxEditor::WxEditorSceneFactory>(),
                                                      std::make_unique<WxEditor::WxOpenGLApiWrapper>());
        engine->Init();
        engine->GetSceneManager().SetActiveScene("WxEditorScene");
    }
    if (engine)
    {
        engine->MainLoop();
    }
    // Render();
    SwapBuffers();
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
    auto& inputManager = engine->GetEngineContext().GetInputManager();
    inputManager.AddKeyEvent(WxEditor::WX_KEY_DOWN, event.GetKeyCode());
}

void GLCanvas::OnChar(wxKeyEvent& evt)
{
}

void GLCanvas::OnMouseUp(wxMouseEvent&)
{
}

void GLCanvas::OnMouseDown(wxMouseEvent& evt)
{
}

void GLCanvas::OnMouseMove(wxMouseEvent& evt)
{
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
    scene->LoadFromFile(file);
    // scene->Stop();
}

GameObject &GLCanvas::GetRootObject()
{
    return engine->GetSceneManager().GetActiveScene()->GetRootGameObject();
}

void GLCanvas::OnShow(wxShowEvent&)
{
    //    SetCurrent(*context);

    //    if (not engine)
    //    {
    //        engine = std::make_unique<GameEngine::Engine>(std::make_unique<Bullet::BulletAdapter>(),
    //                                                      std::make_unique<WxEditor::WxEditorSceneFactory>(),
    //                                                      std::make_unique<WxEditor::WxOpenGLApiWrapper>());
    //    }
}

void GLCanvas::Render()
{
    glViewport(0, 0, GetSize().x, GetSize().y);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)GetSize().x / GetSize().y, 0.01, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glTranslatef(0, 0, -5);
    gluLookAt(1, 1, -10, 0, 0, 0, 0, 1, 0);

    glBegin(GL_QUADS);

    // Front
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1, 1);
    glColor3f(0, 1, 0);
    glVertex3f(1, -1, 1);
    glColor3f(0, 0, 1);
    glVertex3f(1, 1, 1);
    glColor3f(1, 1, 0);
    glVertex3f(-1, 1, 1);

    // Back
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glColor3f(0, 1, 1);
    glVertex3f(1, -1, -1);
    glColor3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(-1, 1, -1);

    // Top
    glColor3f(1, 0, 0);
    glVertex3f(-1, 1, -1);
    glColor3f(0, 1, 0);
    glVertex3f(1, 1, -1);
    glColor3f(0, 0, 1);
    glVertex3f(1, 1, 1);
    glColor3f(1, 1, 0);
    glVertex3f(-1, 1, 1);

    // Bottom
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glColor3f(0, 1, 1);
    glVertex3f(1, -1, -1);
    glColor3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(-1, -1, 1);

    // Left
    glColor3f(1, 0.5, 0);
    glVertex3f(-1, -1, -1);
    glColor3f(0.5, 1, 0);
    glVertex3f(-1, 1, -1);
    glColor3f(0.5, 0, 1);
    glVertex3f(-1, 1, 1);
    glColor3f(0, 0.5, 1);
    glVertex3f(-1, -1, 1);

    // Right
    glColor3f(1, 0.5, 1);
    glVertex3f(1, -1, -1);
    glColor3f(0.5, 1, 1);
    glVertex3f(1, 1, -1);
    glColor3f(1, 1, 0.5);
    glVertex3f(1, 1, 1);
    glColor3f(0, 1, 0.5);
    glVertex3f(1, -1, 1);

    glEnd();
}
