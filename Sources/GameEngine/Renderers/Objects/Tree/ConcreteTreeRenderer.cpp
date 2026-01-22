#include "ConcreteTreeRenderer.h"

#include <vector>

#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Objects/Tree/TreeLeafClusterRenderer.h"
#include "GameEngine/Renderers/Objects/Tree/TreeRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
struct ClusterData
{
    AlignWrapper<vec4> center;
    AlignWrapper<vec4> size;
};

std::vector<ClusterData> getClusterData(const TreeClusters& treeClusters)
{
    std::vector<ClusterData> gpuData;
    gpuData.reserve(treeClusters.clusters.size());

    for (const auto& cluster : treeClusters.clusters)
    {
        ClusterData data;

        vec3 voxelMin = cluster.minBound;
        vec3 voxelMax = cluster.minBound + treeClusters.voxelSize;

        vec3 center     = (voxelMin + voxelMax) * 0.5f;
        vec3 dimensions = voxelMax - voxelMin;
        float maxDim    = std::max({dimensions.x, dimensions.y, dimensions.z});

        data.center = vec4(center, 1.0f);
        data.size   = vec4(dimensions, maxDim);

        gpuData.push_back(data);
    }

    return gpuData;
}
}  // namespace

ConcreteTreeRenderer::ConcreteTreeRenderer(RendererContext& context)
    : TreeRenderer(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafs)
    , leafsClusterShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafsCluster)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
{
}
void ConcreteTreeRenderer::RenderTree(const Components::TreeRendererComponent& treeRendererComponent, float distanceToCamera)
{
    auto lvl = getLevelOfDetail(distanceToCamera);
    RenderTrunk(treeRendererComponent, lvl);

    if (distanceToCamera < endFade)
    {
        RenderLeafs(treeRendererComponent, lvl);
    }

    if (distanceToCamera > startFade)
    {
        RenderLeafsClusters(treeRendererComponent);
    }
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
    leafsClusterShader_.Init();
}
void ConcreteTreeRenderer::reloadShaders()
{
    trunkShader_.Reload();
    leafsShader_.Reload();
    leafsClusterShader_.Reload();
}
void ConcreteTreeRenderer::RenderLeafsClusters(const Components::TreeRendererComponent& component)
{
    if (component.getTreeClusters().clusters.empty())
    {
        return;
    }
    leafsClusterShader_.Start();

    const auto& textures = component.getClusterTextures();
    if (textures.baseColorTexture and textures.baseColorTexture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(0, *textures.baseColorTexture->GetGraphicsObjectId());
    }
    if (textures.normalTexture and textures.normalTexture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(1, *textures.normalTexture->GetGraphicsObjectId());
    }

    const auto& treeClusters = component.getTreeClusters();
    UpdateClusterDataSssbo(treeClusters);
    auto count = treeClusters.clusters.size() * 12;

    context_.graphicsApi_.DisableCulling();
    context_.graphicsApi_.RenderProcedural(count);
}
void ConcreteTreeRenderer::UpdateClusterDataSssbo(const TreeClusters& treeClusters)
{
    auto clusterData = getClusterData(treeClusters);
    auto size        = static_cast<uint32>(clusterData.size() * sizeof(ClusterData));

    if (not clusterDataSssbo_)
    {
        clusterDataSssbo_ =
            context_.graphicsApi_.CreateShaderStorageBuffer(PER_INSTANCES_BIND_LOCATION, size, GraphicsApi::DrawFlag::Dynamic);

        context_.graphicsApi_.UpdateShaderStorageBuffer(*clusterDataSssbo_, clusterData.data(), size);
    }

    if (clusterDataSssbo_)
    {
        context_.graphicsApi_.BindShaderBuffer(*clusterDataSssbo_);
    }
}
}  // namespace GameEngine
