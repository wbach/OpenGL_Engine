#pragma once
#include <memory>
#include "../Resources/ResourceManager.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "Types.h"

namespace GameEngine
{
class DisplayManager;

class IntroRenderer
{
public:
    IntroRenderer(GameEngine::IGraphicsApiPtr graphicsApi, std::shared_ptr<DisplayManager>& displayManager);
    ~IntroRenderer();
    void Render();

private:
    void Init();
    void RenderThis();
    void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

private:
    GameEngine::IGraphicsApiPtr graphicsApi_;
    std::shared_ptr<DisplayManager>& displayManager_;
    ResourceManager resorceManager_;

    LoadingShader shader_;
    Texture* backgroundTexture_;

    bool initialized_;
};
}  // GameEngine
