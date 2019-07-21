#include "TerrainNormalMapRenderer.h"
#include <FreeImage.h>
#include <Utils/Image/ImageUtils.h>
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Logger/Log.h"
#include "Shaders/TerrainNormalMapShaderUniforms.h"

namespace GameEngine
{
const float NORMAL_STRENGTH = 12.f;

TerrainNormalMapRenderer::TerrainNormalMapRenderer(RendererContext &context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::TerrainNormal);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainNormalMapRenderer::Render);
}

void TerrainNormalMapRenderer::Render(const Scene &, const Time &)
{
    if (subscribers_.empty() or not shader_)
        return;

    for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
    {
        if ((*iter)->GetHeightMap() and (*iter)->GetHeightMap()->IsInitialized())
        {
            auto heightMap = RenderTexture(*(*iter)->GetHeightMap());
            (*iter)->SetTexture(std::move(heightMap));
            iter = subscribers_.erase(iter);
        }
        else
        {
            DEBUG_LOG("Height map not ready.");
            ++iter;
        }
    }
}

std::unique_ptr<Texture> TerrainNormalMapRenderer::RenderTexture(const Texture &heightMap) const
{
    auto imageSize    = heightMap.GetSize().y;
    auto storageId = context_.graphicsApi_.CreateTextureStorage(GraphicsApi::TextureType::FLOAT_TEXTURE_2D,
                                                              GraphicsApi::TextureFilter::LINEAR, static_cast<int>(imageSize));

    if (not storageId)
    {
        return nullptr;
    }
    shader_->Start();
    shader_->Load(TerrainNormalMapShaderUniforms::N, static_cast<int32>(imageSize));
    shader_->Load(TerrainNormalMapShaderUniforms::normalStrength, NORMAL_STRENGTH);

    context_.graphicsApi_.ActiveTexture(0);
    context_.graphicsApi_.BindImageTexture(*storageId, GraphicsApi::TextureAccess::WRITE_ONLY);
    context_.graphicsApi_.ActiveTexture(1, heightMap.GetId());
    context_.graphicsApi_.Compute(imageSize / 16, imageSize / 16, 1);
    shader_->Stop();

    return std::make_unique<Texture>(context_.graphicsApi_, *storageId);
}
void TerrainNormalMapRenderer::Init()
{
    shader_->Init();
}

void TerrainNormalMapRenderer::Subscribe(GameObject *gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (terrain == nullptr)
        return;

    subscribers_.push_back(terrain);
}

void TerrainNormalMapRenderer::UnSubscribe(GameObject *)
{
}

void TerrainNormalMapRenderer::UnSubscribeAll()
{
}

void TerrainNormalMapRenderer::ReloadShaders()
{
    shader_->Reload();
}
}  // namespace GameEngine
