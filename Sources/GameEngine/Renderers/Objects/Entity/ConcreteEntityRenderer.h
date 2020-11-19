#pragma once
#include "EntityRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class ConcreteEntityRenderer : public IRenderer
{
public:
    ConcreteEntityRenderer(RendererContext&);
    ~ConcreteEntityRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    void bindShadowMap(uint32 id, uint32 nr) const;

private:
    RendererContext& context_;
    EntityRenderer entityRenderer_;
    ShaderProgram shader_;
};
}  // namespace GameEngine
