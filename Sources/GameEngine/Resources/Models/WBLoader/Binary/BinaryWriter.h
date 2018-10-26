#pragma once
#include <memory>
#include <string>

namespace GameEngine
{
class Model;

namespace WBLoader
{
void CreateBinFile(const std::unique_ptr<Model>& model, const std::string& filename);
std::string CreateBinPath(const std::string& filename);
}
}  // namespace GameEngine
