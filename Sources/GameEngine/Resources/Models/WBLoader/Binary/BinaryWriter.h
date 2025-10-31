#pragma once
#include <filesystem>
#include <memory>
#include <string>

namespace GameEngine
{
class Model;

namespace WBLoader
{
void CreateBinFile(const std::unique_ptr<Model>& model, const std::string& filename);
std::filesystem::path CreateBinPath(const std::filesystem::path& filename);
}  // namespace WBLoader
}  // namespace GameEngine
