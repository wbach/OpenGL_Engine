#pragma once
#include "../Types/Controllers/LibraryControllers.h"
#include "ColladaCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		class LibraryControllersReader
		{
		public:
			LibraryControllersReader(LibraryControllers& lib)
				: data_(lib)
			{}
			void read(XMLNode* node);

		private:
			void Process(XMLNode* node);
			void Process(XMLNode* node, Skin& skin);
			void Process(XMLNode* node, VertexWeights& vw);

		private:
			LibraryControllers & data_;
		};
	} // Collada
} // GameEngine
