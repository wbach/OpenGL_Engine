#include "GuiText.h"

namespace GameEngine
{
	GuiText::GuiText(IGraphicsApiPtr graphicsApi, const std::string & font_file)
		: graphicsApi_(graphicsApi)
		, shader(graphicsApi)
		, fontFile(font_file)
	{
	}

	void GuiText::UnSubscribeAll()
	{
		texts.clear();
	}

	void GuiText::ReloadShaders()
	{
		shader.Stop();
		shader.Reload();
		shader.Init();
	}

	void GuiText::Render()
	{
		shader.Start();
		graphicsApi_->ActiveTexture(0);

		for (const auto& p : texts)
		{
			if (!p.second.isActive)
				continue;

			auto text = p.second;
			shader.SetScale(text.m_size);
			shader.LoadColour(text.colour);
			shader.LoadTranslation(text.position);
			graphicsApi_->PrintText(text.text, vec2i((int32)text.position.x, (int32)text.position.y));
		}
		shader.Stop();
	}

	void GuiText::Init()
	{
		graphicsApi_->CreateFont(fontFile);
		shader.Init();
		Log("GuiText (GuiRenderer) is initialized.");
	}

	GuiText::~GuiText()
	{
		Log(__FUNCTION__);
	}
} // GameEngine
