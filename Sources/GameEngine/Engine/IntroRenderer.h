#pragma once
#include <memory>
#include "../Resources/ResourceManager.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class DisplayManager;

class IntroRenderer
{
public:
    IntroRenderer(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, DisplayManager&);
    ~IntroRenderer();
    void Render();

private:
    void Init();
    void RenderThis();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID perUpdateObjectBuffer_;

    DisplayManager& displayManager_;
    ResourceManager resourceManager_;

    ShaderProgram shader_;
    Texture* backgroundTexture_;

    bool initialized_;
};
}  // namespace GameEngine
