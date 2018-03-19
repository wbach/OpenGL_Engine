#pragma once
#include "FontShader.h"
#include "GuiTextElement.h"
#include "../GuiElement.h"
#include "Logger/Log.h"
#include <unordered_map>

typedef std::unordered_map<std::string, SGuiTextElement> GuiTexts;

class CGuiText : public CGuiElement
{
public:
	CGuiText(GameEngine::IGraphicsApiPtr graphicsApi, const std::string& font_file);
	virtual ~CGuiText() override;
	virtual void Init() override;
	virtual void Render() override;
	virtual void UnSubscribeAll() override;
	GuiTexts texts;

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    FontShader shader;
    std::string fontFile;
};
