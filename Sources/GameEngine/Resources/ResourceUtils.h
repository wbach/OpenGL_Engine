#pragma once
#include <Image/ImageUtils.h>
#include <Types.h>

#include <optional>
#include <vector>

#include "File.h"
#include "TextureParameters.h"
#include "Textures/HeightMap.h"
#include "Textures/Image.h"
#include "Textures/NormalTexture.h"

namespace GameEngine
{
std::optional<Image> ReadFile(const File&, const TextureParameters&);
void CreateHeightMap(const File&, const File&, const vec3&);
void SaveHeightMap(const HeightMap&, const std::string&);
Image GenerateBlendMapImage(const vec3&, const HeightMap&, const vec2& = vec2(.5f, .1f));
void GenerateBlendMap(const vec3&, const HeightMap&, const File&, const vec2&);
Image CreateZerosImage(const vec2ui&);
std::unique_ptr<NormalTexture> CreateNormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, const vec3&,
                                                   const HeightMap&);
void GenerateBlendMap(const vec3&, const HeightMap&, const File&, const vec2& = vec2(.5f, .1f));
}  // namespace GameEngine
