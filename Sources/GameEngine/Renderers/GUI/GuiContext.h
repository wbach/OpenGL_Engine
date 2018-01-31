#pragma once

class CGuiText;
class CGUIRenderer;

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			class CGuiTexture;

			struct GuiContext
			{
				CGuiText* texts = nullptr;
				CGuiTexture* texures = nullptr;
				CGUIRenderer* renderer = nullptr;
			};
		}
	}
}