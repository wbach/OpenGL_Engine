#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "SkyBoxRenderer.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
struct RendererContext;

class ConcreteSkyBoxRenderer : public IRenderer
{
public:
    ConcreteSkyBoxRenderer(RendererContext&);
    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    SkyBoxRenderer skyBoxRenderer_;
    ShaderProgram shader_;
};
}  // namespace GameEngine
