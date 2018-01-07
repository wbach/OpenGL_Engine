#pragma once
#include <GL/glew.h>
#include "../Utils/Utils.h"

class CShadowFrameBuffer
{
public:
    CShadowFrameBuffer();
    virtual ~CShadowFrameBuffer();

    void BindFBO();
    void UnbindFrameBuffer() const;
    void InitialiseFrameBuffer();
    const GLuint& GetShadowMap() const;

private:
    GLuint m_Fbo;
    GLuint m_ShadowMap;

    vec2i m_Size;
    vec2i m_WindowSize;

    bool m_IsInit = false;
};
