#pragma once
#include <filesystem>
#include <memory>

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Model;
class ITextureLoader;

std::unique_ptr<Model> ImportModelBinary(GraphicsApi::IGraphicsApi&, ITextureLoader&, const std::filesystem::path&);
}  // namespace GameEngine
