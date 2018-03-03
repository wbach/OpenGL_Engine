#pragma once
#include "../Types/ColladaData.h"

namespace GameEngine
{
	namespace Collada
	{
		void ReadCollada(const std::string& filename, ColladaData& data);
	} // Collada
} // GameEngine
