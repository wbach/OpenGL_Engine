#pragma once
#include <filesystem>

#include "MeshRawData.h"

namespace GraphicsApi
{
std::optional<MeshRawData> LoadMeshRawDataBinary(const std::filesystem::path&);
bool SaveMeshRawDataBinary(const MeshRawData&, const std::filesystem::path&);
}  // namespace GraphicsApi
