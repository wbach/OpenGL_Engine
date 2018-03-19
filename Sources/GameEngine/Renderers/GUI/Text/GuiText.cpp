#include "GuiText.h"

CGuiText::CGuiText(GameEngine::IGraphicsApiPtr graphicsApi, const std::string & font_file)
	: graphicsApi_(graphicsApi)
	, shader(graphicsApi)
	, fontFile(font_file)
{
}

void CGuiText::UnSubscribeAll()
{
	texts.clear();
}

void CGuiText::Render()
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
		graphicsApi_->PrintText(text.text, vec2i((int32) text.position.x, (int32) text.position.y));
	}
	shader.Stop();
}

void CGuiText::Init()
{
	graphicsApi_->CreateFont(fontFile);
	shader.Init();
	Log("CGuiText (GuiRenderer) is initialized.");
}

CGuiText::~CGuiText()
{
	Log(__FUNCTION__);
}
