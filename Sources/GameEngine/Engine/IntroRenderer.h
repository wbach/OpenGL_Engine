#pragma once
#include <memory>

#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class DisplayManager;
class GeneralTexture;

class IntroRenderer
{
public:
    IntroRenderer(GraphicsApi::IGraphicsApi&, DisplayManager&);
    ~IntroRenderer();

    void Render();

private:
    void Init();
    void CleanUp();
    void RenderThis();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID perUpdateObjectBuffer_;

    DisplayManager& displayManager_;

    ShaderProgram shader_;
    std::unique_ptr<GeneralTexture> backgroundTexture_;
};
}  // namespace GameEngine
