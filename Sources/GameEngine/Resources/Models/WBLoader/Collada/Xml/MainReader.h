#pragma once
#include "../Types/ColladaData.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
namespace Collada
{
void ReadCollada(const File& filename, ColladaData& data);
}  // Collada
}  // GameEngine
