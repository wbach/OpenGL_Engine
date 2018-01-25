#pragma once

class CGuiText;
class CGUIRenderer;

namespace GameEngine
{
	namespace GUI
	{
		struct GuiContext
		{
			CGuiText* texts		    = nullptr;
			CGUIRenderer* renderer  = nullptr;
		};
	}
}