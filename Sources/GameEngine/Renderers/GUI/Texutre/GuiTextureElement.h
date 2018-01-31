#pragma once
#include "Types.h"
#include <unordered_map>

class CTexture;

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			struct GuiTextureElement 
			{
				CTexture* texture = nullptr;
				mat4 transformMatrix_ = mat4(1.f);
			};
		} // Gui
	} // Renderers
} // GameEngine