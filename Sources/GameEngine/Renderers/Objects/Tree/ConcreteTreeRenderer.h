#pragma once
#include "TreeRenderer.h"
#include "GameEngine/Components/Renderer/Trees/Leaf.h"

namespace GameEngine
{
class ConcreteTreeRenderer : public TreeRenderer
{
public:
    ConcreteTreeRenderer(RendererContext&);

    void init() override;
    void reloadShaders() override;
    void RenderTree(const Components::TreeRendererComponent&, float) override;

private:
    void RenderTrunk(const Components::TreeRendererComponent&, LevelOfDetail) const;
    void RenderLeafs(const Components::TreeRendererComponent&) const;
    void RenderLeafsClusters(const Components::TreeRendererComponent&);
    LevelOfDetail getLevelOfDetail(float) const;
    void UpdateClusterDataSssbo(const TreeClusters&);

private:
    ShaderProgram leafsShader_;
    ShaderProgram leafsClusterShader_;
    ShaderProgram trunkShader_;

    GraphicsApi::ID clusterDataSssbo_;
};
}  // namespace GameEngine
