#pragma once
#include <unordered_map>
#include "../GuiElement.h"
#include "Font.h"
#include "FontShader.h"
#include "GuiTextElement.h"
#include "Logger/Log.h"

typedef std::unordered_map<std::string, SGuiTextElement> GuiTexts;

class CGuiText : public CGuiElement
{
public:
	CGuiText(const std::string& font_file, const wb::vec2i& window_size);
	virtual ~CGuiText() override;
	virtual void Init() override;
	virtual void Render() override;
	virtual void UnSubscribeAll() override;
	GuiTexts texts;

private:
    CFont font;
    FontShader shader;
    std::string fontFile;
    wb::vec2i windowSize;
};
