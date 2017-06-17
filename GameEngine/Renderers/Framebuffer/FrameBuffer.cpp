#include "FrameBuffer.h"
#include "OpenGL/OpenGLUtils.h"
#include "Logger/Log.h"

void CFrameBuffer::CreateFrameBuffer()
{
    glGenFramebuffers(1, &fbo);
    isInitialized = true;
}

void CFrameBuffer::AddTexture(GLuint & texture)
{
    textures.push_back(texture);
}

void CFrameBuffer::SetDepthTexture(const GLuint & texture)
{
    depthTexture = texture;
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
    return fbo;
}

const GLuint & CFrameBuffer::GetDepthTexture()
{
    return depthTexture;
}

const GLuint & CFrameBuffer::GetTexture(const uint32 & id)
{
    if (id > textures.size())
		return Utils::s_GLuint_zero;
    return textures[id];
}

void CFrameBuffer::BindTextures(int offset)
{
	int nr = 0;
    for (GLuint& i : textures)
	{
		glActiveTexture(GL_TEXTURE0 + offset + nr++);
		glBindTexture(GL_TEXTURE_2D, i);
	}
	glActiveTexture(GL_TEXTURE0 + offset + nr);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void CFrameBuffer::BindToDraw()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void CFrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
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
    if (!isInitialized) return;

    for (GLuint& tex : textures)
		glDeleteTextures(1, &tex);

    glDeleteTextures(1, &depthTexture);
    if (depthStorage)
        glDeleteRenderbuffers(1, &depthTexture);
	else
        glDeleteTextures(1, &depthTexture);

    if (fbo != 0)
        glDeleteFramebuffers(1, &fbo);
}
