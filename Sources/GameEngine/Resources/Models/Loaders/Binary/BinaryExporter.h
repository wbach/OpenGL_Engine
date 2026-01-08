#pragma once
#include <filesystem>
#include <memory>

namespace GameEngine
{
class Model;
void ExportModelBinary(const Model&, const std::filesystem::path&);
}  // namespace GameEngine
