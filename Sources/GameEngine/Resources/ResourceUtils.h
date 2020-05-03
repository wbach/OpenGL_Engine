#pragma once
#include <Image/ImageUtils.h>
#include <optional>
#include "TextureParameters.h"
#include "Textures/HeightMap.h"
#include "Textures/Image.h"
#include <vector>

namespace GameEngine
{
std::optional<Image> ReadFile(const InputFileName&, const TextureParameters&);
void CreateHeightMap(const std::string&, const std::string&, const vec3&);
void SaveHeightMap(const HeightMap&, const std::string&);
std::vector<float> createNromalMapData(const vec2ui&, const std::vector<float>&, float);
}  // namespace GameEngine
