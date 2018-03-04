#pragma once
#include "Geometry/LibraryGeometries.h"
#include "Materials/LibraryMaterials.h"
#include "Effects/LibraryEffects.h"
#include "Images/LibraryImages.h"
#include "Animations/LibraryAnimations.h"
#include "Controllers/LibraryControllers.h"
#include "VisualScenes/LibraryVisualScenes.h"

namespace GameEngine
{
	namespace Collada
	{
		struct ColladaData
		{
			LibraryVisualScenes libraryVisualScenes_;
			LibraryControllers libraryControllers_;
			LibraryAnimations libraryAnimations_;
			LibraryGeometries libraryGeometries_;
			LibraryMaterials libraryMaterials_;
			LibraryEffects libraryEffects_;
			LibraryImages libraryImages_;
		};
	} // Collada
} // GameEngine
