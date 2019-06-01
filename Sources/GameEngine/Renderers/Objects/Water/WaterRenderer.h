#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
class IShaderProgram;

namespace Components
{
class WaterRendererComponent;
}

class WaterRenderer : public IRenderer
{
    struct Subscriber
    {
        PerObjectUpdate perObjectUpdate_;
        Components::WaterRendererComponent* waterRendererComponent_;
    };

public:
    WaterRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);
    PerObjectUpdate CalculateTransformMatrix(const vec3&, const vec3&) const;

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    std::unordered_map<uint32, Subscriber> subscribers_;

    GraphicsApi::ID perObjectUpdateId_;
};
}  // namespace GameEngine
