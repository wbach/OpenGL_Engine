#pragma once
#include <Image/ImageUtils.h>
#include <optional>
#include "TextureParameters.h"
#include "Textures/HeightMap.h"
#include "Textures/Image.h"
#include <vector>
#include <Types.h>

namespace GameEngine
{
std::optional<Image> ReadFile(const InputFileName&, const TextureParameters&);
void CreateHeightMap(const std::string&, const std::string&, const vec3&);
void SaveHeightMap(const HeightMap&, const std::string&);
void GenerateBlendMap(const vec3&, const HeightMap&, const OutputFileName&, const vec2& = vec2(.5f, .1f));
std::vector<float> createNromalMapData(const vec2ui&, const std::vector<float>&, float);
}  // namespace GameEngine
