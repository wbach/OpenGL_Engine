#pragma once
#include <unordered_map>
#include "../GuiElement.h"
#include "GuiTextElement.h"
#include "Logger/Log.h"
#include "GameEngine/Api/IGraphicsApi.h"

namespace GameEngine
{
typedef std::unordered_map<std::string, GuiTextElement> GuiTexts;
class IShaderProgram;
class IShaderFactory;

class GuiText : public GuiElement
{
public:
    GuiText(IGraphicsApi& graphicsApi, const std::string& font_file, IShaderFactory& shaderFactory);
    virtual ~GuiText() override;
    virtual void Init() override;
    virtual void Render() override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    GuiTexts texts;

private:
    void InitShader();

private:
    IGraphicsApi& graphicsApi_;
    std::unique_ptr<IShaderProgram> shader_;
    std::string fontFile;
    vec2 defaultScale_;
};

}  // GameEngine
