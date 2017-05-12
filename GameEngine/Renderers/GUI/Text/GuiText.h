#pragma once
#include "../GuiElement.h"
#include "FontShader.h"
#include "Font.h"
#include "../../../../GameEngine/Debug_/Log.h"
#include "GuiTextElement.h"
#include <map>

class CGuiText : public CGuiElement
{
public:
	CGuiText(const std::string& font_file, const wb::vec2i& window_size) 
		: m_FontFile(font_file)
		, m_WindowSize(window_size)
	{
	}
	virtual ~CGuiText() override
	{
	}
	virtual void Init() override
	{
		m_Font.Init(m_FontFile, m_WindowSize);
		m_Shader.Init();		
		Log("CGuiText (GuiRenderer) is initialized.");
	}
	virtual void Render() override
	{
		m_Shader.Start();
		glActiveTexture(GL_TEXTURE0);
		glPushMatrix();
		glLoadIdentity();
		for (const auto& p : texts)
		{
			if (!p.second.isActive)
				continue;

			auto text = p.second;
			GLfloat active_color[] = { text.colour.x, text.colour.y, text.colour.z, 1 };
			glColor4fv(active_color);
			m_Shader.loadTranslation(text.position);
			glScalef(text.m_size, text.m_size, text.m_size);
			m_Font.Print((int)text.position.x, (int)text.position.y, text.text.c_str());
		}
		glPopMatrix();
		m_Shader.Stop();
	}
	std::map<std::string, SGuiTextElement> texts;
private:
	CFont		 m_Font;
	FontShader	 m_Shader;
	std::string  m_FontFile;
	wb::vec2i	 m_WindowSize;
};
