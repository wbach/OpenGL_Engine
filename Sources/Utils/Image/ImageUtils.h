#pragma once
#include <optional>
#include <vector>

#include "Types.h"

namespace Utils
{
std::vector<uint8> ConvertColorData(const std::vector<float>&);
void SaveImage(const std::vector<uint8>&, const vec2ui&, const std::string&, const std::optional<vec2>& = std::nullopt);
}  // namespace Utils
