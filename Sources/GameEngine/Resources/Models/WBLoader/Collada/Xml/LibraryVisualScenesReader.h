#pragma once
#include "../Types/VisualScenes/LibraryVisualScenes.h"
#include "ColladaCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		class LibraryVisualSceneReader
		{
		public:
			LibraryVisualSceneReader(LibraryVisualScenes& lib)
				: data_(lib)
			{}
			void read(XMLNode* node);

		private:
			void ProccesVisualScene(XMLNode* node);
			void ProccesNode(XMLNode* xmlnode, Node& node);
			void ProccesInstanceController(XMLNode* node, InstanceController& instanceController);

		private:
			LibraryVisualScenes & data_;
		};
	} // Collada
} // GameEngine

