#pragma once
#include "../Types/ColladaData.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
namespace Collada
{
bool ReadCollada(const File& filename, ColladaData& data);
}  // namespace Collada
}  // namespace GameEngine
