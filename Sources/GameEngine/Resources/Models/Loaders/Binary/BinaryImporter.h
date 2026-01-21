#pragma once
#include <filesystem>
#include <memory>

#include "BinarySerializeHelpers.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Model;
class ITextureLoader;

std::unique_ptr<Model> ImportModelBinary(GraphicsApi::IGraphicsApi&, ITextureLoader&, const std::filesystem::path&);
Material convert(ITextureLoader&, const MaterialSerilizeData&);
std::unique_ptr<Model> convert(ModelSerializeData&&, GraphicsApi::IGraphicsApi&, ITextureLoader&);
}  // namespace GameEngine
