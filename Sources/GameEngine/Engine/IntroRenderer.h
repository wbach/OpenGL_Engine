#pragma once
#include <memory>
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/IResourceManagerFactory.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class DisplayManager;
class IResourceManagerFactory;
class GeneralTexture;

class IntroRenderer
{
public:
    IntroRenderer(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, DisplayManager&, IResourceManagerFactory&);
    ~IntroRenderer();
    void Render();

private:
    void Init();
    void RenderThis();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID perUpdateObjectBuffer_;

    DisplayManager& displayManager_;
    std::unique_ptr<IResourceManager> resourceManager_;

    ShaderProgram shader_;
    GeneralTexture* backgroundTexture_;

    bool initialized_;
};
}  // namespace GameEngine
