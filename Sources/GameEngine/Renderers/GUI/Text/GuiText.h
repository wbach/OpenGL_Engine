#pragma once
#include <map>
#include "../GuiElement.h"
#include "Font.h"
#include "FontShader.h"
#include "GuiTextElement.h"
#include "Logger/Log.h"

class CGuiText : public CGuiElement
{
public:
    CGuiText(const std::string& font_file, const wb::vec2i& window_size) : fontFile(font_file), windowSize(window_size)
    {
    }
    virtual ~CGuiText() override
    {
		Log(__FUNCTION__);
    }
    virtual void Init() override
    {
        font.Init(fontFile, windowSize);
        shader.Init();
        Log("CGuiText (GuiRenderer) is initialized.");
    }
    virtual void Render() override
    {
        shader.Start();
        glActiveTexture(GL_TEXTURE0);
        glPushMatrix();
        glLoadIdentity();
        for (const auto& p : texts)
        {
            if (!p.second.isActive)
                continue;

            auto text              = p.second;
            GLfloat active_color[] = {text.colour.x, text.colour.y, text.colour.z, 1};
            glColor4fv(active_color);
            shader.loadTranslation(text.position);
            glScalef(text.m_size, text.m_size, text.m_size);
            font.Print((int)text.position.x, (int)text.position.y, text.text.c_str());
        }
        glPopMatrix();
        shader.Stop();
    }
    std::map<std::string, SGuiTextElement> texts;

private:
    CFont font;
    FontShader shader;
    std::string fontFile;
    wb::vec2i windowSize;
};
