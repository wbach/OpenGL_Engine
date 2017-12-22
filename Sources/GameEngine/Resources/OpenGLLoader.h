#pragma once
#include <list>
#include "OpenGLObject.h"
#include "Thread.hpp"

class COpenGLLoader
{
public:
	void 			AddObjectToOpenGLLoadingPass(COpenGLObject* obj);
	COpenGLObject*	GetObjectToOpenGLLoadingPass();
	void 			AddObjectToOpenGLPostLoadingPass(COpenGLObject* obj);
	COpenGLObject*	GetObjectToOpenGLPostLoadingPass();

private:
    std::list<COpenGLObject*> openGLPassLoad;
    std::list<COpenGLObject*> openGLPostPassLoad;
    std::mutex mutex;
    std::mutex mutex2;
};
