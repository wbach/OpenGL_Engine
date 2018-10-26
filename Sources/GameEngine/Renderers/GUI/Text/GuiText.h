#pragma once
#include <unordered_map>
#include "../GuiElement.h"
#include "FontShader.h"
#include "GuiTextElement.h"
#include "Logger/Log.h"

namespace GameEngine
{
typedef std::unordered_map<std::string, GuiTextElement> GuiTexts;

class GuiText : public GuiElement
{
public:
    GuiText(IGraphicsApiPtr graphicsApi, const std::string& font_file);
    virtual ~GuiText() override;
    virtual void Init() override;
    virtual void Render() override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    GuiTexts texts;

private:
    IGraphicsApiPtr graphicsApi_;
    FontShader shader;
    std::string fontFile;
};

}  // GameEngine
