#pragma once
#include <filesystem>
#include <memory>

namespace GameEngine
{
class Model;
void ExportModelBinary(const std::unique_ptr<Model>&, const std::filesystem::path&);
}  // namespace GameEngine
