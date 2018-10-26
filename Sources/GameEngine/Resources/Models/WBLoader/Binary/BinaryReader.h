#pragma once
#include <memory>
#include <string>

namespace GameEngine
{
class Model;
class ITextureLoader;

namespace WBLoader
{
std::unique_ptr<Model> ReadBinFile(const std::string& filename, GameEngine::ITextureLoader &textureLoader);
}  // WBLoader
}  // namespace GameEngine
