#pragma once
#include <filesystem>
#include <memory>

#include "BinarySerializeHelpers.h"
#include "GameEngine/Resources/Models/Loaders/Binary/ModelReleatedSerializationData.h"

namespace GameEngine
{
class Model;
void ExportModelBinary(const Model&, const std::filesystem::path&);

ModelSerializeData convert(const Model&);
MaterialSerilizeData convert(const Material&);
}  // namespace GameEngine
