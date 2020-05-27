#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
namespace Components
{
class WaterRendererComponent;
} // namespace Components

class WaterRenderer : public IRenderer
{
    struct Subscriber
    {
        Components::WaterRendererComponent* waterRendererComponent_;
    };

public:
    WaterRenderer(RendererContext&);
    ~WaterRenderer();

    virtual void Init() override;
    virtual void Subscribe(GameObject*) override;
    virtual void UnSubscribe(GameObject*) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene&, const Time&);
    PerObjectUpdate CalculateTransformMatrix(const vec3&, const vec3&) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    std::unordered_map<uint32, Subscriber> subscribers_;

    GraphicsApi::ID perMeshObjectId_;
};
}  // namespace GameEngine
