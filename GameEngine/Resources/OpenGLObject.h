#pragma once

class COpenGLObject
{
public:
	virtual ~COpenGLObject() {}
	virtual void OpenGLLoadingPass() = 0;
	// for for example instaced vbos which have to be done after loading model
    virtual void OpenGLPostLoadingPass();
    bool isInOpenGL()
    {
        return loaded;
    }
protected:
    bool loaded = false;
};

inline void COpenGLObject::OpenGLLoadingPass()
{
    loaded = true;
}

inline void COpenGLObject::OpenGLPostLoadingPass()
{
    loaded = true;
}
