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
    IntroRenderer(IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>& displayManager,
                  IShaderFactory& shaderFactory);
    ~IntroRenderer();
    void Render();

private:
    void Init();
    void RenderThis();
    void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

private:
    IGraphicsApi& graphicsApi_;
    std::shared_ptr<DisplayManager>& displayManager_;
    IShaderFactory& shaderFactory_;
    ResourceManager resorceManager_;

    std::unique_ptr<IShaderProgram> shader_;
    Texture* backgroundTexture_;

    bool initialized_;
};
}  // GameEngine
