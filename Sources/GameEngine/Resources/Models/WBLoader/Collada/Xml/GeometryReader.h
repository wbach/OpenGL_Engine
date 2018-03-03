#pragma once
#include "../Types/Geometry/LibraryGeometries.h"
#include "ColladaCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		class LibraryGeometriesReader
		{
		public:
			LibraryGeometriesReader(LibraryGeometries& geometyData)
				: geometyData_(geometyData)
			{}
			void readGeometries(XMLNode* node);
		
		private:
			void ProcessGeometry(XMLNode* node);
			void ProcessMesh(XMLNode* node, Mesh& mesh);
			void ProcessVertices(XMLNode* node, Vertices& vert);
			void ProcessPolyList(XMLNode* node, Polylist& mesh);

		private:
			LibraryGeometries& geometyData_;
		};
	} // Collada
} // GameEngine