#include "ForsetGeneration.h"

#include <GameEngine/Scene/SceneUtils.h>
#include <Utils/PoissonDiskSample.h>

#include <filesystem>
#include <sstream>
#include <vector>

#include "Logger/Log.h"
#include "Resources/File.h"
#include "TreeGeneration.h"
#include "Types.h"
#include "WxEditor/TreeGeneration/TreeGenerationParams.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"

namespace WxEditor
{
void GenerateForest(wxFrame* parent, GLCanvas* canvas, const vec2& min, const vec2& max, float radius,
                    const std::optional<int>& typesCount)
{
    auto positions = Utils::PoissonDiskSample(min, max, radius);

    auto dlg = std::make_shared<LoadingDialog>(parent, "Tree generator", "Generate tree...");
    LOG_DEBUG << "Total trees count target: " << positions.size();

    std::thread(
        [dlg, parent, canvas, typesCount, pos = std::move(positions)]()
        {
            static int id        = 0;
            auto forestObject    = canvas->GetScene().CreateGameObject("Foreset_" + std::to_string(id++));
            auto forestObjectPtr = forestObject.get();
            canvas->AddGameObject(std::move(forestObject));

            const std::vector<std::tuple<std::filesystem::path, vec2, float>> leafsTexturesPacks{
                {"Textures/Tree/Leafs/LeafSet030_2K-PNG", vec2{4, 4}, -90},
                {"Textures/Tree/Leafs/LeafSet024_2K-PNG", vec2{3, 3}, 0},
                {"Textures/Tree/Leafs/LeafSet027_2K-PNG", vec2{3, 3}, 0},
                {"Textures/Tree/Leafs/LeafSet002_2K-PNG", vec2{3, 2}, 0},
                {"Textures/Tree/Leafs/LeafSet023_2K-PNG", vec2{2, 3}, 90},
                {"Textures/Tree/Leafs/LeafSet017_2K-PNG", {2, 3}, 0}};

            if (typesCount.has_value())
            {
                std::vector<IdType> generatedTrees;
                size_t i = 0;
                for (i = 0; i < static_cast<size_t>(typesCount.value()); i++)
                {
                    if (i >= pos.size())
                    {
                        LOG_DEBUG << "Not need generated new types of trees";
                        break;
                    }
                    const auto& position = pos[i];
                    vec3 treePos(position.x, 0.f, position.y);

                    std::stringstream infoStr;
                    infoStr << "Generate tree: " << i << "/" << typesCount.value();
                    LOG_DEBUG << infoStr.str();
                    dlg->UpdateMessage(infoStr.str());

                    TreeGenerationParams params;
                    params.attractorsCount *= getRandomFloat(0.8f, 1.2f);
                    params.crownYOffset *= getRandomFloat(0.8f, 1.5f);
                    params.crownSize *= getRandomFloat(0.8f, 1.5f);
                    params.maxDistance *= getRandomFloat(0.9f, 1.1f);

                    const auto texturePackIndex =
                        i < leafsTexturesPacks.size() ? i : getRandomInt(0, leafsTexturesPacks.size() - 1);

                    LOG_DEBUG << "texturePackIndex : " << texturePackIndex;

                    const auto& [textureSetFiles, atlasSize, textureRotation] = leafsTexturesPacks[texturePackIndex];
                    applyTextureSetToParams(params, GameEngine::File{textureSetFiles}.GetAbsolutePath());
                    params.meshBuilderParams.textureAtlasSize    = atlasSize;
                    params.meshBuilderParams.leafTextureRotation = textureRotation;

                    auto id = GenerateTree(parent, canvas, params, treePos, forestObjectPtr);
                    generatedTrees.push_back(id);
                }

                for (; i < pos.size(); i++)
                {
                    std::stringstream infoStr;
                    infoStr << "Clone tree: " << i << "/" << pos.size();
                    LOG_DEBUG << infoStr.str();
                    dlg->UpdateMessage(infoStr.str());

                    auto treeIndex = getRandomInt(0, generatedTrees.size() - 1);
                    LOG_DEBUG << "treeIndex : " << treeIndex;
                    auto& gameObjectId = generatedTrees[treeIndex];

                    auto gameObject = canvas->GetScene().GetGameObject(gameObjectId);

                    if (gameObject)
                    {
                        const auto& position = pos[i];
                        vec3 treePos(position.x, 0.f, position.y);
                        auto newGameObject = GameEngine::cloneGameObject(canvas->GetScene(), *gameObject);
                        newGameObject->SetWorldPosition(treePos);
                    }
                }
            }
            else
            {
                size_t index = 0;
                for (const auto& position : pos)
                {
                    std::stringstream infoStr;
                    infoStr << "Generate tree: " << index << "/" << pos.size();
                    LOG_DEBUG << infoStr.str();
                    dlg->UpdateMessage(infoStr.str());

                    TreeGenerationParams params;
                    params.attractorsCount *= getRandomFloat(0.8f, 1.2f);
                    params.crownYOffset *= getRandomFloat(0.8f, 1.5f);
                    params.crownSize *= getRandomFloat(0.8f, 1.5f);
                    params.maxDistance *= getRandomFloat(0.9f, 1.1f);

                    const auto texturePackIndex =
                        index < leafsTexturesPacks.size() ? index : getRandomInt(0, leafsTexturesPacks.size() - 1);
                    const auto& [textureSetFiles, atlasSize, textureRotation] = leafsTexturesPacks[texturePackIndex];
                    applyTextureSetToParams(params, textureSetFiles);
                    params.meshBuilderParams.textureAtlasSize    = atlasSize;
                    params.meshBuilderParams.leafTextureRotation = textureRotation;

                    vec3 treePos(position.x, 0.f, position.y);
                    GenerateTree(parent, canvas, params, treePos, forestObjectPtr);
                    ++index;
                }
            }

            parent->CallAfter([dlg]() { dlg->Close(); });
        })
        .detach();

    dlg->Show();
}
}  // namespace WxEditor
