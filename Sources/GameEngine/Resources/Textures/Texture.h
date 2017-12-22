#pragma once
#include <GL/glew.h>
#include <string>
#include "../OpenGLObject.h"
#include "TextureFlip.h"

class CTexture : public COpenGLObject
{
public:
    CTexture()
    {
    }
    CTexture(const std::string& file, const std::string& filepath, bool applySizeLimit = true);
    virtual ~CTexture();

    virtual void OpenGLLoadingPass() = 0;
    const GLuint& GetId() const
    {
        return id;
    }
    const std::string GetFileName()
    {
        return filename;
    }
    const std::string GetFilPath()
    {
        return fullpath;
    }

protected:
    std::string filename;
    std::string fullpath;

    bool applySizeLimit = true;

    GLuint id   = 0;
    bool isInit = false;
};
