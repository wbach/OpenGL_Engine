#include "ForsetGeneration.h"

#include <Utils/PoissonDiskSample.h>

#include <sstream>

#include "Logger/Log.h"
#include "TreeGeneration.h"
#include "Types.h"
#include "WxEditor/TreeGeneration/TreeGenerationParams.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"

namespace WxEditor
{
namespace
{
float DensityToRadius(float density)
{
    const float packing = 0.65f;  // dla Poisson Disk
    return sqrt(1.f / (glm::pi<float>() * density * packing));
}
}  // namespace

void GenerateForest(wxFrame* parent, GLCanvas* canvas, const vec2& min, const vec2& max, float density)
{
    // auto radius    = DensityToRadius(density);
    auto positions = Utils::PoissonDiskSample(min, max, 15.f);

    auto dlg = std::make_shared<LoadingDialog>(parent, "Tree generator", "Generate tree...");
    LOG_DEBUG << "Total trees count target: " << positions.size();

    std::thread(
        [dlg, parent, canvas, pos = std::move(positions)]()
        {
            static int id        = 0;
            auto forestObject    = canvas->GetScene().CreateGameObject("Foreset_" + std::to_string(id++));
            auto forestObjectPtr = forestObject.get();
            canvas->AddGameObject(std::move(forestObject));

            size_t index = 0;
            for (const auto& position : pos)
            {
                std::stringstream infoStr;
                infoStr << "Generate tree: " << index++ << "/" << pos.size();
                LOG_DEBUG << infoStr.str();
                dlg->UpdateMessage(infoStr.str());

                TreeGenerationParams params;
                params.attractorsCount *= getRandomFloat(0.8f, 1.2f);
                params.crownYOffset *= getRandomFloat(0.8f, 1.5f);
                params.crownSize *= getRandomFloat(0.8f, 1.5f);
                params.maxDistance *= getRandomFloat(0.9f, 1.1f);

                vec3 treePos(position.x, 0.f, position.y);
                GenerateTree(parent, canvas, params, treePos, forestObjectPtr);
            }

            parent->CallAfter([dlg]() { dlg->Close(); });
        })
        .detach();

    dlg->Show();
}
}  // namespace WxEditor
