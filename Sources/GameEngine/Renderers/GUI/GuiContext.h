#pragma once


namespace GameEngine
{
	class GuiTextRenderer;
	class GUIRenderer;

	namespace Renderer
	{
		namespace Gui
		{
			class GuiTextureRenderer;

			struct GuiContext
			{
				GuiTextRenderer* texts = nullptr;
				GuiTextureRenderer* texures = nullptr;
				GUIRenderer* renderer = nullptr;
			};
		}
	}
}