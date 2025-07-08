
#include "GLCanvas.h"

#include <GL/glew.h>
//#include <GL/gl.h>
//#include <GL/glu.h>

//#include <GameEngine/Scene/Scene.hpp>
//#include <GameEngine/Physics/Bullet/BulletAdapter.h>
//#include <GameEngine/Scene/SceneFactoryBase.h>

#include <wx/dcclient.h>

#include <memory>

// clang-format off
BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
EVT_PAINT(GLCanvas::OnPaint)
EVT_SHOW(GLCanvas::OnShow)
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
}  // namespace WxEditor

GLCanvas::GLCanvas(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, nullptr)
    , context(nullptr)
{
    context = new wxGLContext(this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

GLCanvas::~GLCanvas()
{
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
