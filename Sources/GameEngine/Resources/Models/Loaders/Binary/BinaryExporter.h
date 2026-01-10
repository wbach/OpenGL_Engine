#pragma once
#include <filesystem>
#include <memory>

#include "BinarySerializeHelpers.h"

namespace GameEngine
{
class Model;
void ExportModelBinary(const Model&, const std::filesystem::path&);
MaterialSerilizeData convert(const Material&);
}  // namespace GameEngine
