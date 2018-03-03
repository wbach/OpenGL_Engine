#pragma once
#include "../Types/Images/LibraryImages.h"
#include "ColladaCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		class LibraryImagesReader
		{
		public:
			LibraryImagesReader(LibraryImages& libMaterial)
				: data_(libMaterial)
			{}
			void read(XMLNode* node);

		private:
			void ProcessImages(XMLNode* node);

		private:
			LibraryImages & data_;
		};
	} // Collada
} // GameEngine
