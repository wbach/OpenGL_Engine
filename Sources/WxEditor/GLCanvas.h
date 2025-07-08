#pragma once
#include <GL/glew.h>
#include <GameEngine/Components/Animation/Animator.h>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>

#include <wx/glcanvas.h>
#include <wx/timer.h>

#include <memory>
#include <string>

#include "WxOpenGLApiWrapper.h"

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

private:
    wxGLContext* context;
    wxTimer renderTimer;

    void OnShow(wxShowEvent&);
    void OnPaint(wxPaintEvent&);
    void OnTimer(wxTimerEvent&);

    void OnKeyUp(wxKeyEvent&);
    void OnKeyDown(wxKeyEvent&);
    void OnChar(wxKeyEvent&);
    void OnMouseUp(wxMouseEvent&);
    void OnMouseDown(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent&);

    void Render();

    DECLARE_EVENT_TABLE()

    std::unique_ptr<GameEngine::Engine> engine;
};
