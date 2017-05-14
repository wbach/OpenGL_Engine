#include "CubeMapTexture.h"

CCubeMapTexture::CCubeMapTexture(const std::string& name, const std::vector<SImage>& image)
	: CTexture(name, name)
{
	if (image.size() != 6)
	{
		Log("Cube map texture need 6 texutres : " + name);
	}
	else
	{
		for (int x = 0; x < 6; x++)
			m_Images[x] = image[x];
	}	
}

void CCubeMapTexture::OpenGLLoadingPass()
{
	glGenTextures(1, &m_Id);
	GLenum hubo_error = glGetError();

	if (hubo_error)
	{
		for(auto& i : m_Images)
			delete[] i.m_Data;

		Log("[Error] OGL There was an error loading the texture : " + filename);
		return;
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
	for (int x = 0; x < 6; x++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, m_Images[x].m_Width, m_Images[x].m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) m_Images[x].m_Data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (m_Images[x].m_Data != nullptr)
		{
			delete[] m_Images[x].m_Data;
		}
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_IsInit = true;
}
