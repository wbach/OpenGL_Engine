#pragma once


namespace GameEngine
{
	class GuiText;
	class GUIRenderer;

	namespace Renderer
	{
		namespace Gui
		{
			class GuiTexture;

			struct GuiContext
			{
				GuiText* texts = nullptr;
				GuiTexture* texures = nullptr;
				GUIRenderer* renderer = nullptr;
			};
		}
	}
}