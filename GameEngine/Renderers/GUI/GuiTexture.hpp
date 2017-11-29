#pragma once
#include "GuiElement.h"
#include "../../Resources/Textures/Texture.h"

class CGuiText : public CGuiElement
{
public:
    CGuiText(int32* tex)
    {

    }
    virtual ~CGuiText() override
    {
    }
    virtual void Init() override
    {
        font.Init(fontFile, windowSize);
        shader.Init();
        Log("CGuiText (GuiRenderer) is initialized.");
    }
    virtual void Render() override
    {

    }
private:
    int32* texutre;

};
