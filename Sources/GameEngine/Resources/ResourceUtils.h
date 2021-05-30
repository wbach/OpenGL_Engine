#pragma once
#include <Image/ImageUtils.h>
#include <Types.h>

#include <optional>
#include <vector>

#include "File.h"
#include "TextureParameters.h"
#include "Textures/HeightMap.h"
#include <Utils/Image/Image.h>
#include "Textures/GeneralTexture.h"

namespace GameEngine
{
std::optional<Utils::Image> ReadFile(const File&, const TextureParameters&);
void CreateHeightMap(const File&, const vec2ui&);
void CreateHeightMap(const File&, const File&, const vec3&);
void SaveHeightMap(const HeightMap&, const File&);
Utils::Image GenerateBlendMapImage(const vec3&, const HeightMap&, const vec2& = vec2(.5f, .1f));
void GenerateBlendMap(const vec3&, const HeightMap&, const File&, const vec2&);
std::unique_ptr<GeneralTexture> CreateNormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, const vec3&,
                                                   const HeightMap&);
void GenerateBlendMap(const vec3&, const HeightMap&, const File&, const vec2& = vec2(.5f, .1f));

template<typename T>
Utils::Image CreateZerosImage(const vec2ui& size, uint8 channels)
{
    Utils::Image image;
    image.width = size.x;
    image.height = size.y;
    image.setChannels(channels);
    image.allocateImage<T>();
    return image;
}
}  // namespace GameEngine
