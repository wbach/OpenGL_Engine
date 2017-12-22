#include "ShadowFrameBuffer.h"
#include "OpenGL/OpenGLUtils.h"
#include "../../Engine/Configuration.h"

CShadowFrameBuffer::CShadowFrameBuffer()
    : m_WindowSize(EngineConf.resolution)
    , m_Size(EngineConf.shadowMapSize)
{
    InitialiseFrameBuffer();
}

CShadowFrameBuffer::~CShadowFrameBuffer()
{
	if (!m_IsInit) return;

	glDeleteTextures(1, &m_ShadowMap);
	glDeleteFramebuffers(1, &m_Fbo);
}

void CShadowFrameBuffer::BindFBO()
{
    Utils::BindFrameBuffer(m_Fbo, m_Size.x, m_Size.y);
}

void CShadowFrameBuffer::UnbindFrameBuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_WindowSize.x, m_WindowSize.y);
}

void CShadowFrameBuffer::InitialiseFrameBuffer()
{
	m_Fbo = Utils::CreateFrameBuffer();
    m_ShadowMap = Utils::CreateDepthBufferAttachment(m_Size.x, m_Size.y);
	UnbindFrameBuffer();
}

const GLuint & CShadowFrameBuffer::GetShadowMap() const
{
	return m_ShadowMap;
}
