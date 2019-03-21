#include "TerrainNormalMapRenderer.h"
#include <FreeImage.h>
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Shaders/TerrainNormalMapShaderUniforms.h"

namespace GameEngine
{
const uint32 NORMAL_STRENGTH = 12;

TerrainNormalMapRenderer::TerrainNormalMapRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::TerrainNormal);
}
std::unique_ptr<Texture> TerrainNormalMapRenderer::Render(const Texture& heightMap) const
{
    int32 N      = 256;  // heightMap.GetSize().y;
    auto storage = context_.graphicsApi_.CreateTextureStorage(GraphicsApi::TextureType::FLOAT_TEXTURE_2D,
                                                              GraphicsApi::TextureFilter::LINEAR, N);

    if (not storage)
    {
        return nullptr;
    }
    shader_->Start();
    shader_->Load(TerrainNormalMapShaderUniforms::N, N);
    shader_->Load(TerrainNormalMapShaderUniforms::normalStrength, NORMAL_STRENGTH);

    context_.graphicsApi_.ActiveTexture(0, heightMap.GetId());
    context_.graphicsApi_.BindImageTexture(*storage, GraphicsApi::TextureAccess::WRITE_ONLY);
    context_.graphicsApi_.Compute(N / 16, N / 16, 1);
    shader_->Stop();

    auto resultData = context_.graphicsApi_.GetTextureData(*storage);

    auto bitmap = FreeImage_Allocate(N, N, 4);

    RGBQUAD color;
    if (bitmap)
    {
        int x = 0;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                color.rgbRed   = resultData[x++];
                color.rgbGreen = resultData[x++];
                color.rgbBlue  = resultData[x++];
                color.rgbReserved = 255;
                x++;
                FreeImage_SetPixelColor(bitmap, i, j, &color);
            }
        }

        FreeImage_Save(FIF_PNG, bitmap, "terrainNormalMap.png", 0);
    }

    return std::make_unique<Texture>(context_.graphicsApi_, *storage);
}
void TerrainNormalMapRenderer::Init()
{
    shader_->Init();
}
}  // namespace GameEngine
