#pragma once
#include "Types.h"
#include "Mesh.h"

namespace GameEngine
{
	namespace Collada
	{
		struct Geometry
		{
			std::list<Mesh> meshes_;
		};
	}
}