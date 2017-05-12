#include "FrameBuffer.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../../Debug_/Log.h"

void CFrameBuffer::CreateFrameBuffer()
{
	glGenFramebuffers(1, &m_Fbo);
	m_IsInitialized = true;
}

void CFrameBuffer::AddTexture(GLuint & texture)
{
	m_Textures.push_back(texture);
}

void CFrameBuffer::SetDepthTexture(const GLuint & texture)
{
	m_DepthTexture = texture;
}

int CFrameBuffer::CheckStatus()
{
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		Log("[Error] FB error, status: " + std::to_string(Status));
		return -1;
	}
	return 0;
}

const GLuint & CFrameBuffer::GetFbo()
{
	return m_Fbo;
}

const GLuint & CFrameBuffer::GetDepthTexture()
{
	return m_DepthTexture;
}

const GLuint & CFrameBuffer::GetTexture(const uint & id)
{
	if (id > m_Textures.size())
		return Utils::s_GLuint_zero;
	return m_Textures[id];
}

void CFrameBuffer::BindTextures(int offset)
{
	int nr = 0;
	for (GLuint& i : m_Textures)
	{
		glActiveTexture(GL_TEXTURE0 + offset + nr++);
		glBindTexture(GL_TEXTURE_2D, i);
	}
	glActiveTexture(GL_TEXTURE0 + offset + nr);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
}

void CFrameBuffer::BindToDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Fbo);
}

void CFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void CFrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFrameBuffer::UnBindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

CFrameBuffer::~CFrameBuffer()
{
	if (!m_IsInitialized) return;

	for (GLuint& tex : m_Textures)
		glDeleteTextures(1, &tex);

	glDeleteTextures(1, &m_DepthTexture);
	if (m_DepthStorage)
		glDeleteRenderbuffers(1, &m_DepthTexture);
	else
		glDeleteTextures(1, &m_DepthTexture);

	if (m_Fbo != 0)
		glDeleteFramebuffers(1, &m_Fbo);
}
