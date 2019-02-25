#pragma once
#include <memory>
#include "../Resources/ResourceManager.h"
#include "Types.h"

namespace GameEngine
{
class DisplayManager;
class IShaderProgram;
class IShaderFactory;

class IntroRenderer
{
public:
    IntroRenderer(GraphicsApi::IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>& displayManager,
                  IShaderFactory& shaderFactory);
    ~IntroRenderer();
    void Render();

private:
    void Init();
    void RenderThis();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID perUpdateObjectBuffer_;

    std::shared_ptr<DisplayManager>& displayManager_;
    IShaderFactory& shaderFactory_;
    ResourceManager resorceManager_;

    std::unique_ptr<IShaderProgram> shader_;
    Texture* backgroundTexture_;

    bool initialized_;
};
}  // namespace GameEngine
