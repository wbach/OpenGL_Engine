#pragma once
#include "Geometry/LibraryGeometries.h"
#include "Materials/LibraryMaterials.h"
#include "Effects/LibraryEffects.h"
#include "Images/LibraryImages.h"

namespace GameEngine
{
	namespace Collada
	{
		struct ColladaData
		{
			LibraryGeometries libraryGeometries_;
			LibraryMaterials libraryMaterials_;
			LibraryEffects libraryEffects_;
			LibraryImages libraryImages_;
		};
	} // Collada
} // GameEngine
