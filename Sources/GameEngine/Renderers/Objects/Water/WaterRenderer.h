#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
namespace Components
{
class WaterRendererComponent;
}  // namespace Components

class WaterRenderer : public IRenderer
{
    struct Subscriber
    {
        Components::WaterRendererComponent* waterRendererComponent_;
    };

public:
    WaterRenderer(RendererContext&);
    ~WaterRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void prepare() override;
    void render() override;

private:
    PerObjectUpdate CalculateTransformMatrix(const vec3&, const vec3&) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    std::unordered_map<uint32, Subscriber> subscribers_;

    GraphicsApi::ID perMeshObjectId_;
};
}  // namespace GameEngine
