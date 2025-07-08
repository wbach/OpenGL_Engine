#pragma once
#include <GL/glew.h>
#include "WxOpenGLApiWrapper.h"
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Scene/Scene.hpp>
#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Components/Animation/Animator.h>

#include <wx/glcanvas.h>
#include <string>
#include <memory>

namespace GameEngine
{
class Engine;
}

class GLCanvas : public wxGLCanvas {
public:
    GLCanvas(wxWindow* parent);
    ~GLCanvas();

    std::string getGlInfo() const;
    bool AddGameObject(const GameEngine::File&);

private:
    wxGLContext* context;

    void OnShow(wxShowEvent&);
    void OnPaint(wxPaintEvent&);
    void Render();

    DECLARE_EVENT_TABLE()

    std::unique_ptr<GameEngine::Engine> engine;
};
