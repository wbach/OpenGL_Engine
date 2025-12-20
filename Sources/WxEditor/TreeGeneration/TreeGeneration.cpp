#include "TreeGeneration.h"

#include <GameEngine/Components/Renderer/Trees/Tree.h>
#include <GameEngine/Components/Renderer/Trees/TreeMeshBuilder.h>
#include <GameEngine/Components/Renderer/Trees/TreeRendererComponent.h>
#include <GameEngine/Resources/ITextureLoader.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/MaterialPresets.h>

#include "WxEditor/WxHelpers/LoadingDialog.h"

namespace
{
struct TreeGenerationParams
{
    size_t attractorsCount;
    float crownRadius;
    float trunkMaterialTiledScale                    = 2.f;
    std::string trunkMaterialBaseColorTexture        = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialAmbientOcclusionTexture = "Textures/Tree/trunk/light-tree-bark_ao.png";
    std::string trunkMaterialDisplacementTexture     = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialMetallicTexture         = "Textures/Tree/trunk/light-tree-bark_metallic.png";
    std::string trunkMaterialNormalTexture           = "Textures/Tree/trunk/light-tree-bark_normal-ogl.png";
    std::string trunkMaterialRoughnessTexture        = "Textures/Tree/trunk/light-tree-bark_roughness.png";
    std::string leafMaterialBaseColorTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Color.png";
    std::string leafMaterialOpacityTexture           = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Opacity.png";
    std::string leafMaterialRoughnessTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Roughness.png";
    std::string leafMaterialNormalTexture            = "Textures/Tree/Leafs/LeafSet024_2K-PNG_NormalGL.png";

    GameEngine::TreeMeshBuilder::EntryParameters meshBuilderParams;
};

TreeGenerationParams treeBuidlerParams;
}  // namespace
namespace WxEditor
{
void GenerateTree(wxFrame* parent, GLCanvas* canvas)
{
    LOG_DEBUG << "Create tree";
    auto dlg = std::make_shared<LoadingDialog>(parent, "Tree generator", "Generate tree...");

    std::thread(
        [dlg, parent, canvas]()
        {
            const int attractorCount = 400;
            const float crownRadius  = 10.0f;
            GameEngine::Tree tree{};
            tree.rootPosition = vec3(0, -3.f, 0);
            tree.prepareAttractors(attractorCount, crownRadius);

            auto status = tree.build();

            if (status and not status->empty())
            {
                LOG_WARN << status.value();
            }

            LOG_DEBUG << "Buildng tree mesh... ("
                      << "Branches : " << tree.GetBranches().size() << ")";
            GameEngine::TreeMeshBuilder builder(tree.GetBranches());
            auto treeMesh = builder.build(GameEngine::TreeMeshBuilder::EntryParameters{});
            if (treeMesh.positions_.empty())
            {
                wxLogMessage("generateTree failed");
                return;
            }

            auto& engineContext   = canvas->GetEngine().GetEngineContext();
            auto& resourceManager = canvas->GetScene().GetResourceManager();
            auto& tl              = resourceManager.GetTextureLoader();

            auto model    = std::make_unique<GameEngine::Model>();
            auto modelPtr = model.get();

            GameEngine::TextureParameters tp;
            tp.mimap = GraphicsApi::TextureMipmap::LINEAR;

            auto trunkMaterial                    = GameEngine::MaterialPresets::Trunk();
            trunkMaterial.baseColorTexture        = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_albedo.png", tp);
            trunkMaterial.ambientOcclusionTexture = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_ao.png", tp);
            trunkMaterial.displacementTexture     = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_albedo.png", tp);
            trunkMaterial.metallicTexture         = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_metallic.png", tp);
            trunkMaterial.normalTexture           = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_normal-ogl.png", tp);
            trunkMaterial.roughnessTexture        = tl.LoadTexture("Textures/Tree/trunk/light-tree-bark_roughness.png", tp);
            trunkMaterial.tiledScale              = 2.f;

            model->AddMesh(
                GameEngine::Mesh(GraphicsApi::RenderType::TRIANGLES, engineContext.GetGraphicsApi(), treeMesh, trunkMaterial));
            resourceManager.AddModel(std::move(model));

            GameEngine::Material leafMaterial = GameEngine::MaterialPresets::Leaf();
            leafMaterial.baseColorTexture     = tl.LoadTexture("Textures/Tree/Leafs/LeafSet024_2K-PNG_Color.png", tp);
            leafMaterial.opacityTexture       = tl.LoadTexture("Textures/Tree/Leafs/LeafSet024_2K-PNG_Opacity.png", tp);
            leafMaterial.roughnessTexture     = tl.LoadTexture("Textures/Tree/Leafs/LeafSet024_2K-PNG_Roughness.png", tp);
            leafMaterial.normalTexture        = tl.LoadTexture("Textures/Tree/Leafs/LeafSet024_2K-PNG_NormalGL.png", tp);

            auto obj  = canvas->GetScene().CreateGameObject("GeneratedTree");
            auto& trc = obj->AddComponent<GameEngine::Components::TreeRendererComponent>();
            trc.SetGeneratedModel(modelPtr).SetLeafPosition(builder.GetLeafs()).SetLeafMaterial(leafMaterial);
            obj->SetWorldPosition(canvas->GetWorldPosFromCamera());
            canvas->AddGameObject(std::move(obj));
            parent->CallAfter([dlg]() { dlg->Close(); });
        })
        .detach();

    dlg->Show();
}
}  // namespace WxEditor
