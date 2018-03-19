#pragma once
#include "../Types/Animations/LibraryAnimations.h"
#include "ColladaCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		class LibraryAnimationsReader
		{
		public:
			LibraryAnimationsReader(LibraryAnimations& libMaterial)
				: data_(libMaterial)
			{}
			void read(XMLNode* node);

		private:
			void ProcessAnimation(AnimationClip& animation, XMLNode* node);

		private:
			LibraryAnimations & data_;
		};
	} // Collada
} // GameEngine
