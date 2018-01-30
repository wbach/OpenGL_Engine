#include "GuiText.h"

CGuiText::CGuiText(const std::string & font_file, const wb::vec2i & window_size) : fontFile(font_file), windowSize(window_size)
{
}

void CGuiText::UnSubscribeAll()
{
	texts.clear();
}

void CGuiText::Render()
{
	shader.Start();
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
		shader.loadTranslation(text.position);
		glScalef(text.m_size, text.m_size, text.m_size);
		font.Print((int)text.position.x, (int)text.position.y, text.text.c_str());
	}
	glPopMatrix();
	shader.Stop();
}

void CGuiText::Init()
{
	font.Init(fontFile, windowSize);
	shader.Init();
	Log("CGuiText (GuiRenderer) is initialized.");
}

CGuiText::~CGuiText()
{
	Log(__FUNCTION__);
}
