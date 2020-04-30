#include "TerrainNormalMapRenderer.h"

#include <FreeImage.h>
#include <Utils/Image/ImageUtils.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTessellationRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Logger/Log.h"

namespace GameEngine
{
const float NORMAL_STRENGTH = 12.f;

struct TerrainNormalMapInputBuffer
{
    AlignWrapper<vec2> inputs;  // x = N, y = normalStrength
};

TerrainNormalMapRenderer::TerrainNormalMapRenderer(RendererContext &context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TerrainNormal)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainNormalMapRenderer::Render);
}

void TerrainNormalMapRenderer::Render(const Scene &, const Time &)
{
    if (subscribers_.empty())
        return;

    for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
    {
        if (iter->second->GetHeightMap() and iter->second->GetHeightMap()->GetGraphicsObjectId())
        {
            if (shader_.IsReady())
            {
                auto heightMap = RenderTexture(*iter->second->GetHeightMap());
                if (heightMap)
                {
                    iter->second->SetNormalMap(std::move(heightMap));
                }
            }
            else
            {
                auto heightMap = RenderOnCpu(*iter->second->GetHeightMap());
                if (heightMap)
                {
                    iter->second->SetNormalMap(std::move(heightMap));
                }
            }
        }

        iter = subscribers_.erase(iter);
    }
}

std::unique_ptr<Texture> TerrainNormalMapRenderer::RenderTexture(const Texture &heightMap) const
{
    auto imageSize = heightMap.GetSize().y;
    auto storageId = context_.graphicsApi_.CreateTextureStorage(
        GraphicsApi::TextureType::FLOAT_TEXTURE_2D, GraphicsApi::TextureFilter::LINEAR, static_cast<int>(imageSize));

    if (not storageId)
    {
        return nullptr;
    }

    auto bufferId = context_.graphicsApi_.CreateShaderBuffer(6, sizeof(TerrainNormalMapInputBuffer));

    if (not bufferId)
    {
        return nullptr;
    }

    TerrainNormalMapInputBuffer buffer;
    buffer.inputs = vec2{static_cast<int32>(imageSize), NORMAL_STRENGTH};
    context_.graphicsApi_.UpdateShaderBuffer(*bufferId, &buffer);

    shader_.Start();
    context_.graphicsApi_.BindShaderBuffer(*bufferId);
    context_.graphicsApi_.ActiveTexture(0);
    context_.graphicsApi_.BindImageTexture(*storageId, GraphicsApi::TextureAccess::WRITE_ONLY);

    if (heightMap.GetGraphicsObjectId())
        context_.graphicsApi_.ActiveTexture(1, *heightMap.GetGraphicsObjectId());
    else
        ERROR_LOG("Height map is not in gpu.");

    context_.graphicsApi_.Compute(imageSize / 16, imageSize / 16, 1);
    shader_.Stop();

    return std::make_unique<Texture>(context_.graphicsApi_, storageId);
}
void TerrainNormalMapRenderer::Init()
{
    shader_.Init();
}

void TerrainNormalMapRenderer::Subscribe(GameObject *gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Tessellation)
        return;

    subscribers_.insert({gameObject->GetId(), terrain->GetTesselationTerrain()});
}

void TerrainNormalMapRenderer::UnSubscribe(GameObject *gameObject)
{
    subscribers_.erase(gameObject->GetId());
}

void TerrainNormalMapRenderer::UnSubscribeAll()
{
    subscribers_.clear();
}

void TerrainNormalMapRenderer::ReloadShaders()
{
    shader_.Reload();
}

float getHeight(const std::vector<uint8> &imageData, int32 heightMapResolution, vec2i textCoord)
{
    if (textCoord.x < 0)
    {
        textCoord.x = 0;
    }
    if (textCoord.y < 0)
    {
        textCoord.y = 0;
    }
    if (textCoord.x > heightMapResolution)
    {
        textCoord.x = heightMapResolution - 1;
    }
    if (textCoord.y > heightMapResolution)
    {
        textCoord.y = heightMapResolution - 1;
    }
    return imageData[static_cast<size_t>(textCoord.x + textCoord.y * heightMapResolution)];
}

vec3 calculateNormal(const std::vector<uint8> &heightmap, vec2i texCoord, int32 heightMapResolution)
{
    // z0 -- z1 -- z2
    // |     |     |
    // z3 -- h  -- z4
    // |     |     |
    // z5 -- z6 -- z7

    float z0 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(-1, -1));
    float z1 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(0, -1));
    float z2 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(1, -1));
    float z3 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(-1, 0));
    float z4 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(1, 0));
    float z5 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(-1, 1));
    float z6 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(0, 1));
    float z7 = getHeight(heightmap, heightMapResolution, texCoord + vec2i(1, 1));

    vec3 normal(0);

    // Sobel Filter
    normal.z = 1.f / NORMAL_STRENGTH;
    normal.x = z0 + 2.f * z3 + z5 - z2 - 2.f * z4 - z7;
    normal.y = z0 + 2.f * z1 + z2 - z5 - 2.f * z6 - z7;

    return (glm::normalize(normal) + 1.f) / 2.f;
}

std::unique_ptr<Texture> TerrainNormalMapRenderer::RenderOnCpu(const Texture &)
{
    return nullptr;
}
}  // namespace GameEngine
