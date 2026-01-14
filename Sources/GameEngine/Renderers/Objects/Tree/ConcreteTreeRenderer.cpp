#include "ConcreteTreeRenderer.h"

#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Objects/Tree/TreeRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
ConcreteTreeRenderer::ConcreteTreeRenderer(RendererContext& context)
    : TreeRenderer(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafs)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
{
}
void ConcreteTreeRenderer::RenderTree(const Components::TreeRendererComponent& treeRendererComponent,
                                      float distanceToCamera) const
{
    auto lvl = getLevelOfDetail(distanceToCamera);
    RenderTrunk(treeRendererComponent, lvl);
    RenderLeafs(treeRendererComponent, lvl);
}
LevelOfDetail ConcreteTreeRenderer::getLevelOfDetail(float distanceToCamera) const
{
    if (distanceToCamera > EngineConf.renderer.lodDistance0)
    {
        return LevelOfDetail::L2;
    }
    return LevelOfDetail::L1;
}
void ConcreteTreeRenderer::RenderTrunk(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail lvl) const
{
    if (auto model = treeRendererComponent.GetTrunkModel().Get(lvl))
    {
        trunkShader_.Start();
        RenderModel(*model);
    }
}
void ConcreteTreeRenderer::RenderLeafs(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail) const
{
    if (auto id = treeRendererComponent.GetLeafsShaderBufferId())
    {
        context_.graphicsApi_.BindShaderBuffer(*id);
    }

    leafsShader_.Start();
    BindMaterial(treeRendererComponent.GetLeafMaterial());
    context_.graphicsApi_.RenderProcedural(treeRendererComponent.GetLeafsCount() * 6);
    leafsShader_.Stop();
}
void ConcreteTreeRenderer::init()
{
    TreeRenderer::init();
    leafsShader_.Init();
    trunkShader_.Init();
}
void ConcreteTreeRenderer::reloadShaders()
{
    trunkShader_.Reload();
    leafsShader_.Reload();
}
}  // namespace GameEngine
