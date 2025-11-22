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
    void unSubscribe(const Components::IComponent&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    MeasurementValue* measurementValue_;
    RendererContext& context_;
    EntityRenderer entityRenderer_;
    ShaderProgram shader_;
    ShaderProgram instancesShader_;
};
}  // namespace GameEngine
