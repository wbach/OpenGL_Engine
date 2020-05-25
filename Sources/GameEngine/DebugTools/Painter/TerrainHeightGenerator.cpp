#include "TerrainHeightGenerator.h"
#include <random>
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightGenerator::TerrainHeightGenerator(const Components::ComponentController& componentController,
                                               const vec2ui& perTerrainHeightMapsize)
    : componentController_(componentController)
    , perTerrainHeightMapsize_(perTerrainHeightMapsize)
    , octaves_(5)
    , bias_(2.f)
{
}

void TerrainHeightGenerator::generateHeightMapsImage()
{
    getAllSceneTerrains();
    createSeed();
}

void TerrainHeightGenerator::createSeed()
{
    if (terrains_.size() != 1)
        return;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;
    noiseSeed_.resize(count);
    //perlinNoise_.resize(count);

    for (auto& noise : noiseSeed_)
    {
        noise = dist(mt);
    }
}

void TerrainHeightGenerator::getAllSceneTerrains()
{
    const auto& components = componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainRenderer);

    std::vector<Components::TerrainRendererComponent*> terrains;
    for (auto& terrain : components)
    {
        terrains.push_back(static_cast<Components::TerrainRendererComponent*>(terrain.second));
    }
}

void TerrainHeightGenerator::perlinNoise2D()
{
    auto width  = perTerrainHeightMapsize_.x;
    auto height = perTerrainHeightMapsize_.y;

    for (auto& terrain : terrains_)
    {
        auto heightMap = terrain->GetHeightMap();
        auto& image    = heightMap->GetImage();

        for (uint32 x = 0; x < width; x++)
        {
            for (uint32 y = 0; y < height; y++)
            {
                float noise    = 0.0f;
                float scaleAcc = 0.0f;
                float scale    = 1.0f;

                for (uint32 o = 0; o < octaves_; o++)
                {
                    uint32 pitch    = width >> o;
                    uint32 sampleX1 = (x / pitch) * pitch;
                    uint32 sampleY1 = (y / pitch) * pitch;

                    uint32 sampleX2 = (sampleX1 + pitch) % width;
                    uint32 sampleY2 = (sampleY1 + pitch) % width;

                    float blendX = static_cast<float>(x - sampleX1) / static_cast<float>(pitch);
                    float blendY = static_cast<float>(y - sampleY1) / static_cast<float>(pitch);

                    float sampleT = (1.0f - blendX) * noiseSeed_[sampleY1 * width + sampleX1] +
                                    blendX * noiseSeed_[sampleY1 * width + sampleX2];

                    float sampleB = (1.0f - blendX) * noiseSeed_[sampleY2 * width + sampleX1] +
                                    blendX * noiseSeed_[sampleY2 * width + sampleX2];

                    scaleAcc += scale;
                    noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                    scale = scale / bias_;
                }

                // Scale to seed range <0, 1>
                image.setPixel(vec2ui(y, x), Color(noise / scaleAcc, 0.f, 0.f, 0.f));
                //perlinNoise_[y * width + x] = noise / scaleAcc;
            }
        }
    }
}
}  // namespace GameEngine
